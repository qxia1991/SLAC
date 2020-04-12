#ifndef EXORunInfoManager_hh 
#define EXORunInfoManager_hh 

#ifndef EXORunInfo_hh
#include "EXOUtilities/EXORunInfo.hh"
#endif
#ifndef EXODataSet_hh
#include "EXOUtilities/EXODataSet.hh"
#endif
#include <string>
#include <map>
#include <set>

class TXMLDocument;
class TXMLNode;
class EXORunInfoManager {
  public:
    struct DRIComp {
      bool operator()(const EXODataRunInfo& left, const EXODataRunInfo& right) const
      { return left.GetRunNumber() > right.GetRunNumber(); }
    };
    typedef std::set<EXODataRunInfo, DRIComp> DRIList; 
    
    static DRIList GetDataRunsInfo(Int_t minNumber, Int_t maxNumber); 

    static const EXODataRunInfo& GetDataRunInfo(Int_t runNumber); 

    static const EXORunInfo& GetRunInfo(Int_t runNumber, 
                                        const std::string& dir);

    static const EXODataSet& GetDataSet(const std::string& dir,
                                        const std::string& filter = "");

    static const EXODataSet& GetDataSetWithQuality(const std::string& dir,
                                                   const std::string& quality = "");

    static void SetBaseServer(const std::string& baseServer)
      {  GetRunMgr().fBaseServer = baseServer; } 

    static void PrintDirectoryContents(const std::string& dir); 

    static std::string DownloadURL(const std::string& url);

  private:
    EXORunInfoManager(const std::string& baseServer = "");
    EXORunInfoManager(const EXORunInfoManager&);
    EXORunInfoManager& operator=(const EXORunInfoManager&);
    ~EXORunInfoManager();

    typedef std::set<Int_t> RunNumList;
    typedef std::pair<std::string, Int_t> RunTag;
    typedef std::pair<std::string, std::string> DSTag;
    typedef std::map<RunTag, EXORunInfo> RIMap; 
    typedef std::map<DSTag, EXODataSet> DSMap; 
    typedef std::map<Int_t, EXODataRunInfo> DRIMap; 

    RunNumList PopulateRunInfo(Int_t runNumber, const std::string& runType);
    RunNumList PopulateRunInfo(const std::string& runType,
                               const std::string& filter);

    EXODataRunInfo PopulateDataRunInfo(TXMLNode* node); 

    static EXORunInfoManager& GetRunMgr();


    RIMap  fRunCache;        //! run cache
    DSMap  fDataSetCache;    //! data set cache
    DRIMap fDataRunInfoCache;//! data-run info cache 
    std::string fBaseServer; //  server name
};

#endif /* EXORunInfoManager_hh */ 
