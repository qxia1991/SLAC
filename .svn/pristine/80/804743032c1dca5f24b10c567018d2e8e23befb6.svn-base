//______________________________________________________________________________
// EXOCalibManager implements generic calibration services 
//   - communication with metadata database
//   - registration and invocation of handlers for particular calibration
//     types
//   - keeps track of whether calibration last fetched, if any, is still
//     current
#include <cmath>
#include <fstream>
#include <limits>
#include <map>

#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOTimestamp.hh"

#ifdef HAVE_MYSQL
#include "EXODBUtilities/EXOMysqlReadConnection.hh"
#include "EXODBUtilities/EXOMysqlResults.hh"
#include "EXODBUtilities/EXORdbException.hh"
#endif

#include <iostream>
using namespace std;

#define STAY_CONNECTED_LIMIT 8
//______________________________________________________________________________
EXOCalibManager::EXOCalibManager() 
  : 
#ifdef HAVE_MYSQL
    m_msrc(EXOCalib::METADATASOURCEmysql), 
#else
    m_msrc(EXOCalib::METADATASOURCEdefault), 
#endif
    fAllCalibrationsAreFromDatabase(true),
    m_metatable("conditions"),
    m_dbname("rd_exo_cond"),
    m_user("rd_exo_cond_ro"),
    m_host("mysql-node03.slac.stanford.edu"),
    m_port(3306),
    m_useMycnf(false),
    m_verbose(false),
    m_stayConnected(0),
    m_mysqlConn(NULL)
{
}

//______________________________________________________________________________
EXOCalibManager::~EXOCalibManager() 
{
#ifdef HAVE_MYSQL
  if (m_mysqlConn != 0) {
    m_mysqlConn->close();
    m_mysqlConn = 0;
  }
#endif
  // Also return memory
  HandlerMap::iterator iter = m_handlerInfo.begin();
  for (; iter != m_handlerInfo.end(); iter++) delete iter->second; 
  m_handlerInfo.clear();
}

// Configure methods

//______________________________________________________________________________
void EXOCalibManager::registerHandler(EXOVCalibHandlerBuilder* build, 
                                     const std::string& name) 
{
  // Assume for now only one handler per calib type.  If there are
  // different functions which know how to handle different
  // ftypes, a manager function which knows how to call the
  // specific per-ftype functions should be registered Return a
  // status
  if (build == NULL) {
    LogEXOMsg("builder is NULL", EEWarning);
    return;
  }
  HandlerMap::iterator iter = m_handlerInfo.find(name);
  if (iter != m_handlerInfo.end()) {
    LogEXOMsg(
      Form("overwriting previously issued calibration handler of type %s", 
       name.c_str()), EEError);
    delete iter->second;
  }
  m_handlerInfo[name] = new HandlerInfo(build);
}

//______________________________________________________________________________
void EXOCalibManager::unRegisterHandler(EXOVCalibHandlerBuilder* build) 
{
  // un register whatever handler is in the map that corresponds to the
  // EXOVCalibHandlerBuilder passed in.
  HandlerMap::iterator iter = m_handlerInfo.begin();
  std::vector<std::string> names_removed;
  for (; iter != m_handlerInfo.end();iter++) {
    if (iter->second->m_builder == build) {
      delete iter->second;
      names_removed.push_back(iter->first);
    }
  }  
  for (size_t i=0;i<names_removed.size();i++) {
    m_handlerInfo.erase(names_removed[i]);
  }  
}

//______________________________________________________________________________
EXOCalibBase* EXOCalibManager::getCalib(const std::string& type,
                                        const std::string& flavor,
                                        long int seconds, int nano /*= 0*/)
{
  // Overload to accept seconds and nanoseconds
  return getCalib(type, flavor, EXOTimestamp(seconds, nano));
}

//______________________________________________________________________________
EXOCalibBase* EXOCalibManager::getCalib(const std::string& type,
                                        const std::string& flavor,
                                        const EXOEventHeader& header)
{
  // Overload to accept EXOEventHeader
  return getCalib(type, flavor, header.fTriggerSeconds, 1000*header.fTriggerMicroSeconds);
}

//_____________________________________________________________________________

EXOCalibBase* EXOCalibManager::getCalib(const std::string& type,
                                        const std::string& flavor,
                                        const std::string& timestr)
{
  // Overload to accept properly formatted string
  return getCalib(type, flavor, EXOTimestamp(timestr) );
}
//______________________________________________________________________________
EXOCalibBase* EXOCalibManager::getCalib(const std::string& type,
                                        const std::string& flavor,
                                        const EXOTimestamp& time)
{
  // return the calibration for a given type, flavor, and time.  Returns NULL
  // if no calibration is found.
  using namespace EXOCalib;
 
  // Look for the correct calibration type 
  HandlerMap::iterator iter = m_handlerInfo.find(type);
  if (iter == m_handlerInfo.end()) {
    LogEXOMsg("calibration type not registered", EEAlert);
    return NULL; // Just terminated; don't reach this point.
  }
  HandlerInfo* handlerInfo = iter->second; 

  // Look for a calibration in handlerInfo with the right flavor and where isValid(time) returns true.
  for(size_t i = 0; i < handlerInfo->m_calibBase.size(); i++) {
    if(handlerInfo->m_calibBase[i]->getFlavor() == flavor and handlerInfo->m_calibBase[i]->isValid(time)) {
      // We found one.
#ifdef HAVE_MYSQL
      // Close mysql connection if we keep querying and the calib already exists.
      if (m_msrc == METADATASOURCEmysql && m_mysqlConn) {
        if (m_stayConnected <= 0) {  // we've exhausted stayConnected count
          m_mysqlConn->close();
          delete m_mysqlConn;
          m_mysqlConn = 0;
        }  else m_stayConnected--;
      }
      // if we have a MySQL connection decrement event counter; check if
      // we should close connection
#endif
      // Return the calibration we found.
      return handlerInfo->m_calibBase[i];
    }
  }

  // We failed to find a pre-existing calibration valid at this time.  So, make one.
  // First, a couple of checks.
  if(m_msrc == METADATASOURCEuninitialized) {
    LogEXOMsg("m_msrc was not initialized -- did you forget to call the TalkTo function in a script?", EEAlert);
    return NULL; // Just terminated; don't reach this point.
  }
#ifndef HAVE_MYSQL
  if(m_msrc == METADATASOURCEmysql) {
    LogEXOMsg("You've requested database interactions, but your executable was compiled with MYSQL disabled.", EEAlert);
    return NULL; // Just terminated; don't reach this point.
  }
#endif

  switch(m_msrc) {
    case METADATASOURCEdefault:
    {
      fAllCalibrationsAreFromDatabase = false; // No matter what happens here, fAllCalibrationsAreFromDatabase -> false.
      EXOCalibBase* newCalib = handlerInfo->hnd()->read(type, "", "", m_msrc);
      if (newCalib == NULL) {
        LogEXOMsg("Unable to get calibration from handler with type, flavor: " + 
                  type + ", " + flavor, EEWarning);
        return NULL;
      }
      newCalib->setSerNo(0);
      newCalib->setFlavor(flavor);
      if(newCalib->validSince() == EXOTimestamp(0) and newCalib->validTill() == EXOTimestamp(0)) {
        // Handler functions can give default calibration values specific timestamps;
        // if they fail to do so, valid for all time.
        newCalib->setValidity(EXOTimestamp(0), EXOTimestamp(std::numeric_limits<time_t>::max()));
      }
      handlerInfo->m_calibBase.push_back(newCalib); // EXOCalibManager owns newCalib
      return newCalib;
      break;
    }
#ifdef HAVE_MYSQL
    case METADATASOURCEmysql:
    {
      if (!m_mysqlConn) {
        // retry logic?
        if (makeMysqlConnection() != 0) {
          // complain
	  LogEXOMsg("makeMysqlConnection() !=0", EEError); 
          fAllCalibrationsAreFromDatabase = false; // Connection lost means we failed to return a calibration from the db.
          return NULL;
        }
      }
      // If we make it here, it means that we are requesting the MYSQL server
      // reasonably often, bump back up to the limit.
      m_stayConnected = STAY_CONNECTED_LIMIT;
      std::string ident;
      std::string formatVersion;
      std::vector<std::string> prodLevels;
      std::string actualLevel;
      unsigned int serNo;
      EXOTimestamp vstart;
      EXOTimestamp vend;
      // May support requests for multiple levels someday
      int ret = getMysqlBest(serNo, ident, 
                             formatVersion, vstart, 
                             vend, actualLevel, 
                             type, flavor, time);
      if (ret != 0) {
        LogEXOMsg(Form("EXOCalibManager::getMysqlBest has error %d.", ret), EEError);
        fAllCalibrationsAreFromDatabase = false; // Failed to locate calibration from the database.
        return NULL;
      }
    
      EXOCalibBase* newCalib =
        handlerInfo->hnd()->read(type, ident,
                               formatVersion, m_msrc);
      if (newCalib == NULL){
        LogEXOMsg("Current calib is NULL.", EEError);
        fAllCalibrationsAreFromDatabase = false; // Failed to locate calibration from the database.
      } else {
        // Save newCalib -- EXOCalibManager now owns it.
        newCalib->setSerNo(serNo);
        newCalib->setFlavor(flavor);
        newCalib->setValidity(vstart, vend);
        handlerInfo->m_calibBase.push_back(newCalib);
      }
      return newCalib;
      break;
    }
#endif
    case METADATASOURCEtext: 
    {
      std::string file = "";
      if(m_textdbdir != ""){
        LogEXOMsg("looking for DB files in "+m_textdbdir, EENotice);
        file = EXOMiscUtil::SearchForFile(m_textdbdir+"/"+m_metatable+".txt");
        LogEXOMsg("Found DB file "+file, EENotice);
      }
      else {
        // If the user did not put a textdbdir string then try the defaults
        file = EXOMiscUtil::SearchForFile("data/db/"+m_metatable+".txt");
        if(file == "") {
          // Try searching for it without folders.
          file = EXOMiscUtil::SearchForFile(m_metatable + ".txt");
        }
        if(file == "") {
          LogEXOMsg("Failed to find \"" + m_metatable + ".txt\" in default locations.", EEAlert);
          fAllCalibrationsAreFromDatabase = false; // Failed to obtain calibration from file.
          return NULL;
        }
      }
      LogEXOMsg("Still have DB file? "+file, EENotice);
      std::ifstream fin(file.c_str());
      if (!fin.good()) {
        LogEXOMsg("can't open database file '"+file+"'", EEAlert);
        fAllCalibrationsAreFromDatabase = false; // Failed to obtain calibration from file.
        return NULL;
      }
      std::string ident;
      std::string formatVersion;
      std::vector<std::string> prodLevels;
      std::string actualLevel;
      unsigned int serNo = 0;
      EXOTimestamp vstart;
      EXOTimestamp vend;
      bool where = false;
      std::string line;
      std::vector<std::string> header, row;
      std::map<std::string,unsigned int> key;
      if (getline(fin,line)) { // header
        EXOMiscUtil::stringTokenize(line, "\t", header);
        for(unsigned int i=0;i<header.size();i++) key[header[i]] = i;
      }
      while (getline(fin,line)) {
        EXOMiscUtil::stringTokenize(line, "\t", row);
        try {
          serNo = EXOMiscUtil::stringToUnsigned(row.at(key["ser_no"]));
          vstart = EXOTimestamp(row.at(key["vstart"]));
          vend = EXOTimestamp(row.at(key["vend"]));
          ident = row.at(key["data_ident"]);
          formatVersion = row.at(key["fmt_version"]);
          where = ( row.at(key["completion"]) == "OK" &&
        	    row.at(key["calib_type"]) == type &&
        	    row.at(key["flavor"]) == flavor &&
        	    vstart<=time && time<=vend &&
        	    row.at(key["level"]) == "PROD");
        } catch (EXOExceptWrongType ex) {
	  LogEXOMsg("Bad line #? in ?", EEWarning); 
	}
        if (where) break;
      }
      fin.close();
      if (where) {
          // this is our catch
//          cout << "line " << line.substr(0,-1) <<" "<< vend.getString() << endl;
          LogEXOMsgShort(Form("DB ident = %s",ident.c_str()), EEDebug); 
          EXOCalibBase* newCalib = handlerInfo->hnd()->read(type, ident, formatVersion, m_msrc);
          if (newCalib) {
            newCalib->setSerNo(serNo);
            newCalib->setFlavor(flavor);
            newCalib->setValidity(vstart, vend);
            // Save newCalib -- EXOCalibManager now owns it.
            handlerInfo->m_calibBase.push_back(newCalib);
          } else {
            LogEXOMsg("Current calib is Null", EEWarning); 
            fAllCalibrationsAreFromDatabase = false; // Failed to locate calibration in file.
          }
          return newCalib;
      }
      fAllCalibrationsAreFromDatabase = false; // Must have failed to locate calibration in file, somehow or other.
      return NULL;
      break;
    }
    default:
      LogEXOMsg("I don't understand the calibration source", EEAlert);
      return NULL; // Never reach this point due to termination.
    //  break;
  } // switch(m_msrc)
  LogEXOMsg("Reached an unreachable place", EEAlert);
  return NULL; // To placate some compilers
}

std::vector<std::string> EXOCalibManager::findFlavorTime
(const std::string& type,const std::string& flavor, int opt)
{  // if mysql get a list of all calibration times, can return NULLs on fail?

  using namespace EXOCalib;
  std::vector<std::string> notgood ; // this vector is bad!

  // Look for the correct calibration type by type string
  HandlerMap::iterator iter = m_handlerInfo.find(type);
  if (iter == m_handlerInfo.end()) {
    LogEXOMsg("calibration type not registered", EEAlert);
    return notgood; // Just terminated; don't reach this point.
  }
  HandlerInfo* handlerInfo = iter->second;
  // dont Look for a calibration in handlerInfo with the right flavor and where isValid(time) returns true.

  // First, a couple of checks?
  if(m_msrc == METADATASOURCEuninitialized) {
    LogEXOMsg("m_msrc was not initialized -- did you forget to call the TalkTo function in a script?", EEAlert);
    return notgood; // Just terminated; don't reach this point.
  }
#ifndef HAVE_MYSQL
  if(m_msrc == METADATASOURCEmysql) {
    LogEXOMsg("You've requested database interactions, but your executable was compiled with MYSQL disabled.", EEAlert);
    return notgood; // Just terminated; don't reach this point.
  }
#endif

  std::vector<std::string> exoTime ; // for output
  std::vector<std::string> exoTimeDefault ; // for output
  exoTimeDefault.push_back(("2011-10-06")); // run 2464.
  switch(m_msrc) {
    // case METADATASOURCEtext:
    // case METADATASOURCEdefault:
    default: // ^^ and more
    {
      LogEXOMsg(Form("Unable to check mysql database from handler with type, flavor: %s, %s", type.c_str() , flavor.c_str()), EEWarning);
      return exoTimeDefault;
      break;
    }
#ifdef HAVE_MYSQL
    case METADATASOURCEmysql:
    {
      if (!m_mysqlConn) { // then connect
	if (makeMysqlConnection() != 0) { // or complain
	  LogEXOMsg("makeMysqlConnection() !=0", EEError);
	  return notgood;
	}
      }
      // If we make it here, it means that we are requesting the MYSQL server
      // reasonably often, bump back up to the limit.
      m_stayConnected = STAY_CONNECTED_LIMIT;

      // May support requests for multiple levels someday --> bah.
      std::string where = Form(" where completion='OK' AND calib_type='%s' AND flavor='%s' AND level='PROD' ",
			       type.c_str(),flavor.c_str());
      StringVector getCols;
      StringVector orderCols;

      if      ( opt == 0 ) getCols.push_back("vstart");
      else if ( opt == 1 ) getCols.push_back("vend");
      else if ( opt == 2 ) getCols.push_back("from_unixtime((unix_timestamp(vstart)+unix_timestamp(vend))/2.)"); // whoo whoo - mysql maths on timestamps.
      else {
	LogEXOMsg("opt not understood",EEError);
	return notgood;
      }

      orderCols.push_back("vstart");
      orderCols.push_back("ser_no");

      std::auto_ptr<EXOMysqlResults> results;
      try {
	results = m_mysqlConn->select(m_metatable, getCols, orderCols, where,
				      EXOMysql::SELECTdesc); // don't limit to 1
      } catch (EXORdbException rexcept) {
	LogEXOMsg("MYSQL exception: " + rexcept.getMsg(), EEError);
      }
      if (results.get() == NULL) {
	LogEXOMsg(Form("DB panic found for (type, flavor): (%s, %s)",type.c_str(),flavor.c_str()),EEError);
	return notgood;
      }
      if (results->getNRows() == 0) {
	LogEXOMsg(Form("No rows found for (type, flavor): (%s, %s)",
		       type.c_str(), flavor.c_str()), EEError);
	return exoTimeDefault;
      } else {
	std::vector<std::string> fields;
	//	std::cout <<  "rows " << (int) results->getNRows() << std::endl;
	try {
	  for(int rower = 0 ; rower < (int) results->getNRows() ; rower++) {
	    results->getRow(fields,rower);
	    exoTime.push_back((fields[0])); //fields should be length 1 always
	    //  std::cout << exoTime.back() <<  std::endl;
	  }
	}
	catch (EXOExceptWrongType ex) {
	  LogEXOMsg("Art thou not time",EEError);
	}
	return exoTime;
      }
      break;
    }
#endif
  } // switch(m_msrc)
  LogEXOMsg("Reached an unreachable place", EEAlert);
  return notgood; // To placate some compilers
}

//______________________________________________________________________________
/*const char* EXOCalibManager::GetMetadataAccessType() const
{
  const char *ret = 0;
  switch (m_msrc) {
  case EXOCalib::METADATASOURCEmysql:   ret = "mysql"; break;
  case EXOCalib::METADATASOURCEtext:    ret = "text"; break;
  case EXOCalib::METADATASOURCEdefault: ret = "default"; break;
  default:                              ret = "unknown"; break;
  }
  return ret;
}*/

//______________________________________________________________________________
void EXOCalibManager::SetMetadataAccessType(std::string aval)
{
  // Set meta-data source
  if (aval == "default" || aval == "none") { // "none" supported for historical reasons [9/15/2011]
    SetMetadataAccessType(EXOCalib::METADATASOURCEdefault);
  } else if (aval == "mysql") {
#ifdef HAVE_MYSQL
    SetMetadataAccessType(EXOCalib::METADATASOURCEmysql);
#else
    LogEXOMsg("mysql data source not available for this build", EEWarning);
#endif
  } else if (aval == "text") {
    SetMetadataAccessType(EXOCalib::METADATASOURCEtext);
  } else {
#ifdef HAVE_MYSQL
    SetMetadataAccessType(EXOCalib::METADATASOURCEmysql);
    LogEXOMsg(Form("unknown meta data source: '%s'; using mysql", aval.c_str()), EENotice);
#else
    SetMetadataAccessType(EXOCalib::METADATASOURCEdefault);
    LogEXOMsg(Form("unknown meta data source: '%s'; using default", aval.c_str()), EENotice);
#endif
  }
}

//______________________________________________________________________________
int EXOCalibManager::TalkTo(EXOTalkToManager *talktoManager)
{
  // Set up talk-to commands to enable command-line modification
  // of the EXOCalibManager's behavior. 

  // Get metadata storage type: 
  talktoManager->CreateCommand("/calibmgr/maccess",
                               "Calib metadata access: default, text, mysql",
                               this,
#ifndef HAVE_MYSQL
                               "default", 
#else
                               "mysql", 
#endif
                               &EXOCalibManager::SetMetadataAccessType);

  talktoManager->CreateCommand("/calibmgr/textDBdir", "Add a directory to store text based DB files",
                               this, "", &EXOCalibManager::SetTextDBdir);

  talktoManager->CreateCommand("/calibmgr/mysqlport", "MySQL server port",
                               this, 0, &EXOCalibManager::SetPort);

  talktoManager->CreateCommand("/calibmgr/verbose", 
                               "Calib manager verbosity level", 
                               this, 0, &EXOCalibManager::SetVerbosity);

  talktoManager->CreateCommand("/calibmgr/mysqldb", "MySQL db name",
                               this,
                               "rd_exo_cond",
                               &EXOCalibManager::SetDb);

  talktoManager->CreateCommand("/calibmgr/mysqlhost", "MySQL server node",
                               this,
                               "mysql-node03.slac.stanford.edu",
                               &EXOCalibManager::SetHost);

  talktoManager->CreateCommand("/calibmgr/mysqluser", "MySQL user",
                                this,
                                "rd_exo_cond_ro",
                                &EXOCalibManager::SetUser);

  talktoManager->CreateCommand("/calibmgr/mysqltable", 
                                "MySQL metadata table", this,
                                "conditions",
                                &EXOCalibManager::SetTable);

  return 0;
}


//______________________________________________________________________________
int  EXOCalibManager::makeMysqlConnection() 
{
#ifdef HAVE_MYSQL
  m_mysqlConn = new EXOMysqlReadConnection();
  m_mysqlConn->setVerbose(m_verbose);

  static std::string usual="w0rking.St1fff!";
  bool ok;
  if (m_useMycnf) {
    m_mysqlConn->setOption(EXOMysql::DBreadDefaultFile, ".my.cnf");
    // assume at least pwd is in .my.conf
    ok = m_mysqlConn->open(m_host.c_str(), m_port, m_user.c_str(), NULL, 
                           m_dbname.c_str());
  }
  else { // use standard read-only password
    ok = m_mysqlConn->open(m_host.c_str(), m_port, m_user.c_str(), 
                           usual.c_str(), m_dbname.c_str());
  }
  if (!ok) {
    delete m_mysqlConn;
    m_mysqlConn = 0;
    return -1;
  } 
  m_stayConnected = STAY_CONNECTED_LIMIT;
  return 0;
#else
  LogEXOMsg("calling disabled function", EEError); 
  LogEXOMsg("Please compile with mysql to recover functionality", EEError);
  return -1;
#endif
}

//______________________________________________________________________________
int  EXOCalibManager::getMysqlBest(unsigned& serNo, std::string& ident, 
                                   std::string& fmtVersion,
                                   EXOTimestamp& vstart, 
                                   EXOTimestamp& vend, 
                                   std::string& actualLevel,
                                   const std::string &type, 
                                   const std::string& flavor, 
                                   const EXOTimestamp& thetime, 
                                   const std::vector<std::string>* requestLevels) 
{
  // Look for best calibration matching type, flavor, etc.  Output
  // arguments come first since they are required Default of 0 for
  // request levels is interpreted as single-element vector
  // containing value "PROD"
#ifdef HAVE_MYSQL
  using namespace EXOCalib;

  // compose fixed part of "WHERE" string.  End result should be
  //  " where 'completion'='OK' and 'calib_type'=string-ctype and
  //   'flavor'=flavor and 'vstart'<= thetime.getString() and
  //   'vend' > thetime.getString()
  // Then have to add one more condition on 'level' column.
  // Assume failure until proven otherwise
  serNo = 0;
  int retValue;

  std::string ctypename= type;
  std::string ts = thetime.getString();
  std::string where;
  std::string whereBase =
    std::string(" where completion='OK' and calib_type='") + ctypename;
  whereBase +=  std::string("' and flavor='")+flavor;
  whereBase += std::string("' and vstart<='") + ts + std::string("' and vend>'");
  whereBase += ts + std::string("' and level='");
  if (requestLevels != NULL) {
    where = whereBase + (*requestLevels)[0];
  }
  else where= whereBase + std::string("PROD");
  where += std::string("'");

  StringVector getCols;
  StringVector orderCols;

  getCols.push_back("ser_no");
  getCols.push_back("vstart");
  getCols.push_back("vend");
  getCols.push_back("data_ident");
  getCols.push_back("fmt_version");

  orderCols.push_back("ser_no");   

  // EXOMysql::SELECTdesc  follows  'where' in call to select to 
  // make order descending

//  cout << "mysqlbest table='" << m_metatable <<"' getcols='"<< 0 <<"' ordercols='"<< 0 <<"' where='"<< where <<"' "<< endl;
  std::auto_ptr<EXOMysqlResults> results;
  try {
    results = m_mysqlConn->select(m_metatable, getCols, orderCols, where, 
                        EXOMysql::SELECTdesc, 1);
  } catch (EXORdbException rexcept) {
    LogEXOMsg("MYSQL exception: " + rexcept.getMsg(), EEError);
  }
  if (results.get() == NULL) {
    // log error?
    return -1;
  } 
  if (results->getNRows() == 0) {
    LogEXOMsg(Form("No rows found for (type, time, flavor): (%s, %s, %s)", 
              ctypename.c_str(), ts.c_str(), flavor.c_str()), EEError);
    retValue =  -2;
  } else {
    std::vector<std::string> fields;
    results->getRow(fields);
//    std::cout << "getrow "; for(int j=0;j<fields.size();j++){std::cout <<" "<< fields[j];} std::cout << std::endl;
    try {
      serNo = EXOMiscUtil::stringToUnsigned(fields[0]);
      vstart = EXOTimestamp(fields[1]);
      vend = EXOTimestamp(fields[2]);
      ident = fields[3];
      fmtVersion = fields[4];
      LogEXOMsgShort(Form("DB ident = %s",ident.c_str()), EEDebug); 
      retValue = 0;
    }
    catch (EXOExceptWrongType ex) {
      // complain
      retValue = -3;
    }
  }
  return retValue;
#else
  LogEXOMsg("calling disabled function", EEError); 
  LogEXOMsg("Please compile with mysql to recover functionality", EEError);
  return -1;
#endif
}

std::vector<std::string> EXOCalibManager::GetRegisteredHandlers()
{
  std::vector<std::string> outputVec;
  HandlerMap::iterator iter = m_handlerInfo.begin();
  for ( ; iter != m_handlerInfo.end(); iter++ ) outputVec.push_back(iter->first);
  return outputVec;
}

void  EXOCalibManager::PrintHandlerInfo(const std::string& type){
  using namespace EXOCalib;
 
  // Look for the correct calibration type 
  HandlerMap::iterator iter = m_handlerInfo.find(type);
  if (iter == m_handlerInfo.end()) {
    LogEXOMsg("calibration type not registered", EEAlert);
  }
  HandlerInfo* handlerInfo = iter->second; 
  
  std::cout<< "Handler for "<< type << " has "<< handlerInfo->m_calibBase.size() <<  " entries " << std::endl;
  for(size_t i = 0; i < handlerInfo->m_calibBase.size(); i++) {
    std::cout<< i <<" : "; 
    handlerInfo->m_calibBase[i]->PrintValidTime();
  }
  //   if(handlerInfo->m_calibBase[i]->getFlavor() == flavor and handlerInfo->m_calibBase[i]->isValid(time)) {
 
}

//______________________________________________________________________________
EXOCalibManager::HandlerInfo::HandlerInfo(EXOVCalibHandlerBuilder* build) 
 : m_builder(build), m_hnd(0) 
{}

//______________________________________________________________________________
EXOCalibManager::HandlerInfo::~HandlerInfo()
{
  for(size_t i = 0; i < m_calibBase.size(); i++) {
    delete m_calibBase[i];
  }
  m_builder->DeleteCalibHandler(m_hnd);
}

//______________________________________________________________________________
EXOCalibHandlerBase* EXOCalibManager::HandlerInfo::hnd()
{
  if ( m_hnd == 0 ) m_hnd = m_builder->GetNewHandler();
  return m_hnd;
}

