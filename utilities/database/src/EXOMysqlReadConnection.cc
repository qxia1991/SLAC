#include "EXODBUtilities/EXOMysqlReadConnection.hh"
#include "EXODBUtilities/EXOMysqlResults.hh"
#include "EXODBUtilities/EXORdbException.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

#include <iostream>
#include <sstream>
#include <cerrno>

#include "mysql.h"
#include "errmsg.h"
#include "mysqld_error.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>

// #include "facilities/Util.h"
#include "EXOUtilities/EXOMiscUtil.hh"

// Local utilities

namespace {
  std::string getMysqlError(MYSQL* my, unsigned* myErrno) {
    if ((*myErrno = mysql_errno(my)) ) {
      const char* errstring = mysql_error(my);
      std::string report("MySQL error code "), code;
      EXOMiscUtil::utoa(*myErrno, code);
      report += code + std::string(": ") + std::string(errstring);
      return report;
    }
    return std::string("");
  }
  unsigned 
  int realQuery(MYSQL* my, std::string const& qstring) {
    unsigned long sz = qstring.size();
    const char* str = qstring.c_str();
    return mysql_real_query(my, str, sz);
  }
  std::string mysqlEscape(MYSQL* my, std::string const& s)
  {
    char * asciiz = new char[s.size()*2+1];
    unsigned sz = (unsigned) 
      mysql_real_escape_string(my, asciiz, s.c_str(), s.size());
    std::string result(asciiz, sz);
    delete [] asciiz;
    return result;
  }
}

EXOMysqlReadConnection::EXOMysqlReadConnection(std::ostream* out, 
                                               std::ostream* errOut)
  : m_out(out), m_err(errOut), m_mysql(0), m_connected(false), m_mycnf(false),
    m_host(""), m_port(-1), m_user(""), m_pw(""), m_dbName(""),
    m_wasOpen(false), m_maxRetry(2), m_avgWait(10000), m_maxOpenTry(3)
{
  if (m_out == 0) m_out = &std::cout;
  if (m_err == 0) m_err = &std::cerr;

  m_mycnfValue = "";

  // Seed random number generator with a random thing
  srand(time(0));
}

// Return old state of verbose flag
bool EXOMysqlReadConnection::setVerbose(bool verbose) {
  if (verbose) {
    if (m_out) return true;
    m_out = &std::cout;
    return false;
  } 
  if (m_out) {
    m_out = 0;
    return true;
  }
  return false;
}


bool EXOMysqlReadConnection::close() 
{
  /** Close the current open connection , if any.  Return true if there
      was a connection to close and it was closed successfully */
  if (m_mysql) mysql_close(m_mysql);
  m_mysql = 0;
  m_connected = false;
  return true;
}

EXOMysqlReadConnection::~EXOMysqlReadConnection() {
  close();
}

bool EXOMysqlReadConnection::open(const std::string& host, 
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& dbName) {
  // If using default file and password is empty string, substitute null
  if (m_mycnf && (password.size() == 0)) {
    return open(host, user.c_str(), NULL, dbName.c_str());
  } else {
    return open(host, user.c_str(), password.c_str(), dbName.c_str());
  }
}



bool EXOMysqlReadConnection::init() {
  m_mysql = mysql_init(NULL);
  if (!m_mysql) return false;
  return true;
}


/* 
   if option = DbreadDefaultFile, value is alternative to my.cnf
   if option = DBreadDefaultGroup, value is group from which connection
               parameters should come
*/
bool EXOMysqlReadConnection::setOption(EXOMysql::DBOPTIONS option, 
                                       const char* value)
{
  if (!m_mysql) return false;
  mysql_option myOpt;
  if (option == EXOMysql::DBreadDefaultFile) myOpt = MYSQL_READ_DEFAULT_FILE;
  else if (option == EXOMysql::DBreadDefaultGroup) 
    myOpt = MYSQL_READ_DEFAULT_GROUP;
  else return false;
  int ret =  mysql_options(m_mysql, myOpt, value);
  if (ret == 0) {
    m_mycnf = true;
    m_mycnfValue = std::string(value);
    return true;
  }
  return false;
}

bool EXOMysqlReadConnection::reopen() {
  if (m_connected) return true;  // nothing to do

  if (!m_wasOpen) return false;

  // Otherwise redo, using cached parameters.
  if (!init()) return false;

  if (m_mycnf) {
    bool setret = setOption(EXOMysql::DBreadDefaultFile, m_mycnfValue.c_str());
    if (!setret) return false;
  }
  return open(m_host.c_str(), m_port, m_user.c_str(), m_pw.c_str(), 
              m_dbName.c_str() );
}

bool EXOMysqlReadConnection::open(const std::string& host, 
                                  const char* user,
                                  const char* password,
                                  const char* dbName) {
                             //     , unsigned int       port) {
  if (dbName == 0) {
    (*m_err) << 
      "EXOMysqlReadConnection::open : null db name not allowed!" <<
      std::endl;
    m_err->flush();
    return false;
  } 
  
  if (!m_mysql) {
    bool ok = init();
    if (!ok) return false;
  }
    // 'host' argument is of the form hostname[:port]
    //  That is, port section is optional.  If not supplied, use
    // default port.
  std::string hostOnly;
  int port = 0;
  std::string::size_type colonLoc = host.find(":");
  if (colonLoc == std::string::npos) {
    hostOnly = host;
  }
  else {
    hostOnly = host.substr(0, colonLoc);
    std::string portString = host.substr(colonLoc+1);
    try {
      port = EXOMiscUtil::stringToInt(portString);
    }
    catch (EXOExceptWrongType ex) {
      (*m_err) << "From EXOMysqlReadConnection::connect.  Bad port: "
               << ex.getMsg() << std::endl;
      m_err->flush();
      return false;
    }
    
  }
  
  return open(hostOnly.c_str(), port, user, password, dbName);
}

bool EXOMysqlReadConnection::open(const char* host, 
                                  int         port,
                                  const char* user,
                                  const char* password,
                                  const char* dbName) {
  
  if (!m_mysql) {
    bool ok = init();
    if (!ok) return false;
  }
  
  unsigned retriesRemaining = m_maxOpenTry;
  
  do {
    retriesRemaining--;
    MYSQL *connected = mysql_real_connect(m_mysql, host,
                                          user,
                                          password, dbName,
                                          port, NULL, 0);
    
    if (connected != 0) {  // Everything is fine.  
      if (m_out) { // Put out an info message if we have a stream
        (*m_out) << "Successfully connected to MySQL host " 
                 << ((host != 0) ? host : "from init file" )
                 << ", database " << dbName << std::endl;
        m_out->flush();
      }
      m_connected = true;

      // Cache information in case we're asked to re-open
      if (host) m_host = std::string(host);
      else m_host = std::string("");
      m_port = port;
      if (user) m_user = std::string(user);
      else m_user = std::string("");
      if (password) m_pw = std::string(password);
      else m_pw = std::string("");
      m_dbName = dbName;
      m_wasOpen = true;
      return connected;
    }
    else {
      (*m_err) <<  "Failed to connect to MySQL host " << host <<
        " with error " << mysql_error(m_mysql) << std::endl;
      (*m_err) << "Retries remaining: " << retriesRemaining;
      m_err->flush();
      m_connected = false;
      if (retriesRemaining > 0) {
        unsigned maxRnd = 0xfff;
        unsigned wait = m_avgWait;
        if (wait < maxRnd/2) wait += maxRnd/2;
        
        unsigned rnd = rand() & maxRnd;  // just use last 12 bits
        wait += (rnd - maxRnd/2);
        EXOMiscUtil::gsleep(wait);
      }
    }
  } while (retriesRemaining > 0);
  return m_connected;
}
/*
  bool EXOMysqlReadConnection::open(const std::string& parms) {
    using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
    using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
    xmlBase::XmlParser parser;
    DOMDocument* doc = parser.parse(parms.c_str(), "mysqlConnection");
    if (doc == 0) {
      (*m_err) << "parse of connection parameters failed" << std::endl;
      m_err->flush();
      return false;
    }
    DOMElement*  conn = doc->getDocumentElement();
    
    std::string host = xmlBase::Dom::getAttribute(conn, "host");
    std::string user = xmlBase::Dom::getAttribute(conn, "user");
    std::string password = xmlBase::Dom::getAttribute(conn, "password");
    std::string dbname = xmlBase::Dom::getAttribute(conn, "dbname");

    if (password.size() == 0 ) { // prompt for password?
      (*m_out) << "interactive login NYI " << std::endl;
      m_out->flush();
      return false;
    }

    return this->open(host, user, password, dbname);
  }
*/


std::auto_ptr<EXOMysqlResults> 
EXOMysqlReadConnection::select(const std::string& tableName,
                               const StringVector& getCols,
                               const StringVector& orderCols,
                               const std::string& where,
                               EXOMysql::SELECTOPTIONS flags,
                               int   rowLimit) 
{
  // Returns results in the form of EXOMysqlResults.  The caller *owns* this. 

  std::string sqlString = "SELECT ";
  unsigned nGet = getCols.size();
  unsigned nOrder = orderCols.size();
  std::auto_ptr<EXOMysqlResults> retPtr;

  sqlString += getCols[0];
  for (unsigned iGet = 1; iGet < nGet; iGet++) {
    sqlString += ",";
    sqlString += getCols[iGet];
  }
  sqlString +=  " FROM " + tableName +  " ";
  if (where.size() >  0) {
    sqlString += where + " ";
  }
  if (nOrder > 0 ) {
    sqlString += " ORDER BY " + orderCols[0]; 
    for (unsigned iOrder = 1; iOrder < nOrder; iOrder++) {
      sqlString += ",";
      sqlString += orderCols[iOrder];
    }
    if (flags & EXOMysql::SELECTdesc) {
      sqlString += " DESC ";
    }
  }
  if (rowLimit > 0) {
    sqlString += " LIMIT ";
    std::string limitStr;
    limitStr.clear();
    EXOMiscUtil::itoa(rowLimit, limitStr);
    sqlString += limitStr;
  }
  
  if (flags & EXOMysql::SELECTforUpdate) {
    sqlString += " FOR UPDATE ";
  }
  else if (flags & EXOMysql::SELECTshareLock) {
    sqlString += " LOCK IN SHARE MODE ";
  }
  if (m_out) {
    (*m_out) << std::endl << "# About to issue SELECT:" << std::endl;
    (*m_out) << sqlString << std::endl;
    m_out->flush();
  }
    
  int mysqlRet;
  if (flags & (EXOMysql::SELECTforUpdate | EXOMysql::SELECTshareLock)) { // no retry
    mysqlRet = realQuery(m_mysql, sqlString);
  }
  else mysqlRet = realQueryRetry(sqlString);
  if (mysqlRet) {
    unsigned errcode;
    std::string msg = "EXOMysqlReadConnection::select. ";
    msg += getMysqlError(m_mysql, &errcode);
    (*m_err) << std::endl << msg << std::endl;
    m_err->flush();
    throw EXORdbException(msg, mysqlRet);
    return retPtr;
  }

  MYSQL_RES *myres = mysql_store_result(m_mysql);
  return std::auto_ptr<EXOMysqlResults>(new EXOMysqlResults(myres));

}


std::auto_ptr<EXOMysqlResults> EXOMysqlReadConnection::dbRequest(const std::string& request) 
{
  // Transmit raw request of any form to our other end.  If it is a 
  // request that returns results, those results will be stored in a 
  // newly-allocated EXOMysqlResults object
  // and dbRequest will return a pointer 
  // to it. Otherwise dbRequest will return a null pointer.
  // Throw an exception if request fails for any reason.

  std::auto_ptr<EXOMysqlResults> returnPtr;
  if (m_out) {
    (*m_out) << std::endl << "# About to issue SQL request:" << std::endl;
    (*m_out) << request << std::endl;
    m_out->flush();
  }
    
  // no retries for totally arbitrary request
  int mysqlRet = realQuery(m_mysql, request);
  if (mysqlRet) {
    unsigned errcode;
    std::string msg = 
      "EXOMysqlReadConnection::dbRequest. ";
    msg += getMysqlError(m_mysql, &errcode);
    (*m_err) << std::endl << msg << std::endl;
    m_err->flush();
    throw EXORdbException(msg, mysqlRet);
    return returnPtr;
  }

  MYSQL_RES *myres = mysql_store_result(m_mysql);
  if (!myres) {
    // Was it supposed to return data?
    if (mysql_field_count(m_mysql) == 0) { // no data expected
      return returnPtr;
    }
    else {
      std::string msg =
        "EXOMysqlReadConnection::dbRequest: expected data; none returned";
      (*m_err) << std::endl << msg << std::endl;
      m_err->flush();
      throw EXORdbException(msg);
      return returnPtr;
    }
  }
  return std::auto_ptr<EXOMysqlResults>(new EXOMysqlResults(myres));
}


/**
   Retry up to retry count if query fails with retriable error
*/
int EXOMysqlReadConnection::realQueryRetry(const std::string& qstring) {
  unsigned remain = m_maxRetry;
  int mysqlRet = realQuery(m_mysql, qstring);
  while (remain) {
    --remain;
    switch (mysqlRet) {
      // retriable errors
    case CR_SERVER_GONE_ERROR:
    case CR_SERVER_LOST:
    case CR_UNKNOWN_ERROR: {
      std::ostringstream message;
      message << "Mysql connection bad (return status was " << mysqlRet << ").  Attempting to reopen it.";
      message << "  " << remain << " tries will remain.";
      LogEXOMsg(message.str(), EENotice);
      close();        // close old connection
      // calculate sleep time, sleep
      // rand returns a value in the range 0 - MAX_RAND.  The latter
      // is OS-dependent, but must be at least 32767 = 0x7fff
      unsigned maxRnd = 0xfff;
      unsigned wait = m_avgWait;
      if (wait < maxRnd/2) wait += maxRnd/2;
      
      unsigned rnd = rand() & maxRnd;  // just use last 12 bits
      wait += (rnd - maxRnd/2);
      EXOMiscUtil::gsleep(wait);
      // open new connection, retry
      const char* host =0;
      const char* user = 0;
      const char* pw = 0;
      if (m_host.size()) host = m_host.c_str();
      if (m_user.size()) user = m_user.c_str();
      if (m_pw.size()) pw = m_pw.c_str();
      bool ok = open(host, m_port, user, pw, m_dbName.c_str());
      if (!ok) continue;  // on to next retry
      
      mysqlRet = realQuery(m_mysql, qstring);
      break;
    }
      // For other errors or success just return
    default:
      return mysqlRet;
    }
  }
  return mysqlRet;
  
}
