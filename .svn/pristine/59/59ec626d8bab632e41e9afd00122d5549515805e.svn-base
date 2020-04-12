#ifndef EXOMysqlReadConnection_hh 
#define EXOMysqlReadConnection_hh

#include <vector>
#include <string>
#include <ostream>
#include <memory>

typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;


class EXOMysqlResults;

typedef std::vector<std::string> StringVector;

namespace EXOMysql {
  enum SELECTOPTIONS {
    SELECTnone=0,
    SELECTdesc=1,       /* sort descending, rather than default ascending */
    SELECTforUpdate=2,  /* SELECT .... FOR UPDATE */
    SELECTshareLock=4   /* SELECT ... LOCK IN SHARE MODE */
  };

  // Add more if they become interesting
  enum DBOPTIONS {
    DBreadDefaultFile,   
    DBreadDefaultGroup
  };

}

class EXOMysqlReadConnection {

public:
  EXOMysqlReadConnection(std::ostream* out=0, std::ostream* errOut=0);
  virtual ~EXOMysqlReadConnection();
  
  bool init();
  bool setVerbose(bool verbose);

  bool setOption(EXOMysql::DBOPTIONS option, const char* value);
  bool open(const std::string& host, const std::string& userid,
            const std::string& password,
            const std::string& dbName);
  
  bool open(const std::string& host, const char* userid,
            const char* password,
            const char* dbName);
  
  bool open(const char* host, int port, const char* userid,
            const char* password,
            const char* dbName);

  bool reopen();
  
  bool close();

  /// Return true iff open has been done with no matching close
  bool isConnected() {return m_connected;}
  
  std::ostream* getOut() const {return m_out;}
  std::ostream* getErrOut() const {return m_err;}
  
  unsigned getMaxRetry() const {return m_maxRetry;}
  void setMaxRetry(unsigned maxRetry) { m_maxRetry = maxRetry;}
  
  unsigned getMaxOpenTry() const {return m_maxOpenTry;}
  void setMaxOpenTry(unsigned maxOpenTry) { m_maxOpenTry = maxOpenTry;}
  
  /** Get/set avg wait in milliseconds between retries  */
  unsigned getAvgWait() const {return m_avgWait;}
  void setAvgWait(unsigned avgWait) { m_avgWait = avgWait;}

  //unsigned  getLastError( ) const;

  
  
  virtual std::auto_ptr<EXOMysqlResults> 
  select(const std::string& tableName, const StringVector& getCols,
         const StringVector& orderCols,
         const std::string& where=std::string(""),
         EXOMysql::SELECTOPTIONS flags=EXOMysql::SELECTnone, int rowLimit=0);
  
  virtual std::auto_ptr<EXOMysqlResults> dbRequest(const std::string& request);

private:
  std::ostream* m_out;
  std::ostream* m_err;


  MYSQL*      m_mysql;
  bool        m_connected;
  bool        m_mycnf;
  std::string m_mycnfValue;
  
  // cache all connection parameters in case we have to reconnect
  std::string m_host;
  int m_port;
  std::string m_user;
  std::string m_pw;
  std::string m_dbName;
  bool        m_wasOpen;   // set to true after successful connect

  // Needed for retry logic
  unsigned      m_maxRetry;
  unsigned      m_avgWait;   // milliseconds
  unsigned      m_maxOpenTry;   // 1 means no retries

  //     Retry query up to maxRetry times if it fails.
  int realQueryRetry(const std::string& qstring);
};

#endif
