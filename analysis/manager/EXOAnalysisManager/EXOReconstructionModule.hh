#ifndef EXOReconstructionModule_hh
#define EXOReconstructionModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXODefineAPDSumProcessList.hh"
#include "EXOReconstruction/EXODefineCrossProductProcessList.hh"
#include "EXOReconstruction/EXODefineUWireIndProcessList.hh"
#include "EXOReconstruction/EXOMatchedFilterFinder.hh"
//#include "EXOReconstruction/EXOYMatchExtractor.hh"
#include "EXOReconstruction/EXONewYMatchExtractor.hh"
#include "EXOReconstruction/EXOAPDGangSignalFinder.hh"
#include "EXOReconstruction/EXOSignalFitter.hh"
#include "EXOReconstruction/EXOMultipleSignalFinder.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOControlRecord.hh"
#include <string>

class EXOChannelMap;
class EXODriftVelocityCalib;

class EXOReconstructionModule: public EXOAnalysisModule
{
  public:
    EXOReconstructionModule();
    ~EXOReconstructionModule();

    int Initialize();
    EventStatus BeginOfRun(EXOEventData *ED);
    EventStatus ProcessEvent(EXOEventData *ED);
    EventStatus EndOfRun(EXOEventData *ED);
    int TalkTo(EXOTalkToManager *tm);

    void SetSumBothAPDPlanes(bool val){fSumBothAPDPlanes = val;}
    void SetUWireScalingFactor(double val);
    void SetVWireScalingFactor(double val);
    void SetAPDScalingFactor(double val);
    void SetElectronicDatabaseFlavor(std::string aval) 
      { fElectronicsDatabaseFlavor = aval; }
    void SetVWireThresholdDatabaseFlavor(std::string aval)
      { fVWireThresholdDatabaseFlavor = aval; }
    void SetSkipTruncatedData(bool val) {fSkipTruncatedData = val;}
    void SetUserDriftVelocity(double val){fUserDriftVelocity = val;}
    void SetUserCollectionVelocity(double val){fUserCollectionVelocity = val;}
    void SetZ_Separation(double val){fZ_Separation = val;}
    void EnableStage(std::string stage_plus_bool);
    void PrintStageStatus();
    EXOReconProcessList CompileUWireIndProcessList(const EXOReconProcessList&,const EXOSignalCollection&) const;

  protected:

    struct RecProc {
      RecProc(bool proc = true, const std::string& aname = "") : 
        fDoProcess(proc), fName(aname) {}
      bool fDoProcess; 
      std::string fName;
    };
  
  struct sortBySecondDescending
  {
    //Sort container of pairs by second in descending order.
    bool operator()(const std::pair<double,double> &lhs,const std::pair<double,double> &rhs) const
    {
      return (lhs.second > rhs.second);
    }
  };

    enum EDriftVelStatus {
      kFirstCall,            // First time through
      kUserSet,              // The user set a value manually
      kDatabase,              // Database value (successfully retrieved)
      kDBUpdate,             // Used to que collection updates to use just one fDriftVelocityCalib for drift and collection drift 
      kDefaultMC,            // Default EXODimensions.hh value because it was a MC event
      kDefaultQueryFail,     // Default EXODimensions.hh value because the database query failed
      kDefaultQueryDisabled  // Default EXODimensions.hh value because code was compiled without HAVE_MYSQL
    };
    EXODriftVelocityCalib* fDriftVelocityCalib;
    EDriftVelStatus fDriftStatus;
    double fUserDriftVelocity;   //A drift velocity set by the user.
    double fDriftVelocityTPC1;   //Drift velocity for tpc 1.
    double fDriftVelocityTPC2;   //Drift velocity for tpc 2.
    void SetDriftVelocity(EXOEventData* ED);
    EDriftVelStatus fCollectionStatus;
    double fUserCollectionVelocity;   //A Collection velocity set by the user.
    double fCollectionVelocityTPC1;   //Collection velocity for tpc 1.
    double fCollectionVelocityTPC2;   //Collection velocity for tpc 2.
    void SetCollectionVelocity(EXOEventData* ED);
    double fZ_Separation;

    EXOBeginRecord::RunFlavor fRunFlavor;

    typedef std::vector< std::pair<EXOVDefineReconProcessList*, RecProc> > ProcessLists;
    typedef std::vector< std::pair<EXOVSignalFinder*, RecProc> > Finders;
    typedef std::vector< std::pair<EXOVSignalParameterExtractor*, RecProc> > Extractors;
    
    // The manager must be constructed first, before all signal model
    // registrants.  It must *live* longer than them.
    EXOSignalModelManager      fSignalModelManager;

    EXODefineCrossProductProcessList fDefineCrossProduct;
    EXODefineAPDSumProcessList fDefineAPDsums;
    EXODefineUWireIndProcessList fDefineUWireInd;

    EXOMatchedFilterFinder  fMatchedFilterFinder;
    EXOMultipleSignalFinder    fMultFinder;
    EXOAPDGangSignalFinder     fAPDGangFinder;

    EXOSignalFitter            fUandAPDExtractor;
  //EXOYMatchExtractor         fVExtractor;
  //EXONewYMatchExtractor      fVExtractor;

    ProcessLists fProcLists;
    Finders      fSignalFinders;
    Extractors   fSignalExtractors;

    EXOTimingStatisticInfo        fTimingInfo; // Timing information for processing 
    const EXOChannelMap *fChannelMap;
    bool fSumBothAPDPlanes;
    double fUWireScalingFactor; //ADC counts * fUWireScalingFactor = fRawEnergy
    double fVWireScalingFactor; //ADC counts * fVWireScalingFactor = fMagnitude
    double fAPDScalingFactor; //ADC counts * fAPDScalingFactor = fRawCounts
    std::string fElectronicsDatabaseFlavor;
    std::string fVWireThresholdDatabaseFlavor;
    bool fSkipTruncatedData;
    double fUMatchTime;
    double fInductionThresh;  //Threshold for U-wire signals to look for induction on neighboring channels

 private:
  Bool_t fUWireAdjacentIndSigFindingEnabled;//u-wire adjacent induction signal finding has been enabled

 protected:
    DEFINE_EXO_ANALYSIS_MODULE( EXOReconstructionModule )
};

#endif
