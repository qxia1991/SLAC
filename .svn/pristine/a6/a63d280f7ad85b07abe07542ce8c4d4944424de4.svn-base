#ifndef EXOCalibManager_hh
#define EXOCalibManager_hh

#include <string>
#include <vector>
#include <map>
#include "EXOCalibUtilities/EXOCalibDataTypes.hh"

class EXOCalibBase;
class EXOCalibHandlerBase;
class EXOMysqlReadConnection;
class EXOEventHeader;
class EXOTimestamp;
class EXOTalkToManager;
class EXOVCalibHandlerBuilder;
class EXOAnalysisManager;

class EXOCalibManager {
  protected:
    // Make this class a singleton by protecting the 3 following
    // constructors/operators.
    EXOCalibManager();
    EXOCalibManager(const EXOCalibManager&);
    EXOCalibManager& operator=(const EXOCalibManager&);
    ~EXOCalibManager();

  public:
    static EXOCalibManager& GetCalibManager()
    {
      static EXOCalibManager gfCalibMgr;
      return gfCalibMgr;
    }

    EXOMysqlReadConnection*& borrowConnection()
      { return m_mysqlConn; }

    EXOCalibBase* getCalib(const std::string& type,
                           const std::string& flavor,
                           const EXOTimestamp& time); // the functional form

    //overloads :
    EXOCalibBase* getCalib(const std::string& type,
                           const std::string& flavor,
                           long int seconds, int nano = 0);

    EXOCalibBase* getCalib(const std::string& type,
                           const std::string& flavor,
                           const std::string& timestr );

    EXOCalibBase* getCalib(const std::string& type,
                           const std::string& flavor,
                           const EXOEventHeader& header);

    std::vector<std::string> findFlavorTime(const std::string& type,
					    const std::string& flavor,
					    int opt = 0 );
    // get a list of times for each conditions entry of the flavor opts >> 0 : vstart, 1 : vend , 2 : use mysql magic to find the average between vstart and vend

    std::vector<std::string> GetRegisteredHandlers();
    void PrintHandlerInfo(const std::string& type); // for the type find all calibbases registered and ask them for their validity periods, could be extended for different calibs to print relevant details, 

    // Should this instance of CalibManager get metadata from a MySQL db?
    // If so, need host, dbname, etc.
    // Other alternatives are:
    //         use flat file to find metadata
    //         turn off (no CalibManager at all; calibrations are unused or
    //         hard-coded)
    EXOCalib::METADATASOURCE GetMetadataAccessType() const { return m_msrc; }
    //const char* GetMetadataAccessType() const;
    void SetMetadataAccessType(EXOCalib::METADATASOURCE src) { m_msrc = src; } // FIXME add reset
    void SetMetadataAccessType(std::string aval);
    void SetTextDBdir(std::string aval) {m_textdbdir = aval; }
    std::string GetTextDBdir() const {return m_textdbdir;}
    void SetDb(std::string aval)    { m_dbname = aval; }
    void SetHost(std::string aval)  { m_host = aval; }
    void SetUser(std::string aval)  { m_user = aval; }
    void SetTable(std::string aval) { m_metatable = aval; }
    void SetPort(int aval)          { m_port = aval; }
    int  GetVerbosity() const       { return m_verbose; }
    void SetVerbosity(int aval)     { m_verbose = aval; }
    // Following only used for mysql
    void SetUseMycnf(bool val = true) { m_useMycnf = val; }
    bool AllCalibrationsAreFromDatabase() const {return fAllCalibrationsAreFromDatabase;}

  protected:
    // EXOVCalibHandlerBuilder is a friend class to enable calling the
    // registration functions below.
    friend class EXOVCalibHandlerBuilder;
    void registerHandler(EXOVCalibHandlerBuilder* builder,
                         const std::string& name);
    void unRegisterHandler(EXOVCalibHandlerBuilder* builder);

    /*******************************************************/
    // The following two functions enable the TalkTo behavior
    // of the EXOCalibManager and is generally only called:
    friend class EXOAnalysisManager;
    int TalkTo(EXOTalkToManager* tm);
    /*******************************************************/

  private:

    EXOCalib::METADATASOURCE m_msrc;  // Form of metadata storage
    bool fAllCalibrationsAreFromDatabase; // True until a calibration request fails, or is returned that didn't come from the database (or a text file).

    // connection parameters, also used for MySQL
    std::string  m_metatable;
    std::string  m_textdbdir;
    std::string  m_dbname;
    std::string  m_user;
    std::string  m_host;
    int          m_port;
    bool         m_useMycnf;
    bool         m_verbose;

    //  To be replaced with parameters for configuration of the
    //  Calibration Service, e.g.  form of metadata storage (MySQL,
    //  file, none) connection parameters for metadata access (e.g.
    //  MySQL host, etc.) form of time stamp access (e.g. from
    //  event; fixed; incrementing) parameters for time stamp
    //  access, if needed

    int            m_stayConnected;
    EXOMysqlReadConnection* m_mysqlConn;

    class HandlerInfo {
      public:
        std::vector<EXOCalibBase*> m_calibBase;
        EXOVCalibHandlerBuilder* m_builder;
        EXOCalibHandlerBase* hnd();
        HandlerInfo(EXOVCalibHandlerBuilder* build);
        ~HandlerInfo();
      protected:
        EXOCalibHandlerBase*     m_hnd;
    };

    typedef std::map<std::string, HandlerInfo* > HandlerMap;
    HandlerMap m_handlerInfo;

    int  getMysqlBest(unsigned& serNo, std::string& ident,
                      std::string& fmtVersion,
                      EXOTimestamp& validStart,
                      EXOTimestamp& validEnd, std::string& actualLevel,
                      const std::string& type, const std::string& flavor,
                      const EXOTimestamp& thetime,
                      const std::vector<std::string>* requestLevels=0);

    int  makeMysqlConnection();

};

// Most user code should use the following macro.  Note header can be an
// EXOEventHeader, and EXOTimestamp, or a time in seconds since these functions
// are all overloaded.
#define GetCalibrationFor(data, type, flavor, header) \
dynamic_cast<data*>(                                  \
  EXOCalibManager::GetCalibManager().getCalib(        \
    type::GetHandlerName(), flavor, header))

#endif

