#ifndef EXOClusteringModule_hh
#define EXOClusteringModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOVWireSignal.hh"
#include "EXOUtilities/EXOUWireInductionSignal.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOMatrix.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <list>
#include <vector>
#include <set>
#include <cmath>
#include <iostream>
#include <exception>

using EXOMiscUtil::GetMeanUorVPositionFromChannel;

class EXODriftVelocityCalib;
class EXOAPDSignal;
class EXOEventData;
class EXOMatching;
class TTree;
class TFile;
class EXOClusterDebugData;

class EXOClusteringModule : public EXOAnalysisModule 
{
protected:
  struct APDSignalSorter
  {
    bool operator()(const EXOAPDSignal* lhs, const EXOAPDSignal* rhs) const;
  };

  struct USignalEnergySorter
  {
    bool operator()(const EXOUWireSignal* lhs, const EXOUWireSignal* rhs) const;
  };

  struct USignalChannelSorter
  {
    bool operator()(const EXOUWireSignal* lhs, const EXOUWireSignal* rhs) const;
  };

  struct VSignalEnergySorter
  {
    bool operator()(const EXOVWireSignal* lhs, const EXOVWireSignal* rhs) const;
  };

  struct VSignalChannelSorter
  {
    bool operator()(const EXOVWireSignal* lhs, const EXOVWireSignal* rhs) const;
  };

  struct CombinationLimitException : public std::exception
  {
    virtual const char* what() const throw()
    {
      return "Maximum number of signal bundle combination exceeded.";
    }
  };

  template <typename D>
  class Cluster
  {
    public:
      Cluster() : fRefScintCluster(NULL) {;}
      double Energy() const;
      double EnergyError() const;
      double RawEnergy() const;
      double InductionEnergy() const;
      double RawEnergyError() const;
      double InductionEnergyError() const;
      double Time() const;
      double AverageTime() const;
      double TimeRMS() const;
      double Position() const;
      double PositionRMS() const;
      double DriftTime() const;
      double CalculateZwithDriftVelocity(double driftVel, double collTime) const;
      std::string Type() const;
      size_t Size() const {return signals.size();}
      void AddSignal(D* signal){signals.push_back(signal);}
      void AddCluster(const Cluster<D>& cluster);
      Cluster<D> GetChild(size_t i) const;
      bool IsCombined() const {return childClusterPositions.size();}
      size_t NumChilds() const {return childClusterPositions.size();}
      int SignalChannel(size_t i) const;

      std::vector<D*> signals;
      std::vector<size_t> childClusterPositions;
      EXOScintillationCluster* fRefScintCluster;
      std::vector<EXOUWireInductionSignal*> fRefInductionVector;

    private:
      double SignalEnergy(size_t i) const;
      double SignalRawEnergy(size_t i) const;
      double SignalInductionEnergy(size_t i) const;
      double SignalEnergyError(size_t i) const;
      double SignalRawEnergyError(size_t i) const;
      double SignalInductionEnergyError(size_t i) const;
  };

  template <typename D>
  void PrintSignals(const std::vector< Cluster<D> > &clusters) const;
  template <typename D>
  EventStatus ClusterWires(std::list<D*> &energySorted, std::list<D*> &channelSorted, double matchTime, double timeOffsetPerChannelDiff, std::vector< Cluster<D> > &clusters);
  template <typename D>
  std::set< std::set<int> > CreateCombinations(const std::vector< Cluster<D> > &clusters, double matchTime) const;
  void CullVWires(EXOEventData* ED, const std::vector< Cluster<EXOUWireSignal> > &uc, std::vector< Cluster<EXOVWireSignal> > &vc) const;
  void NewChargeCluster(EXOEventData* ED, const Cluster<EXOUWireSignal> *uc = 0, const Cluster<EXOVWireSignal> *vc = 0, double UEnergyRatio = 1.0, double VEnergyRatio = 1.0) const;
  EXOMatrix<int> LogLikelihoodMatrix(const std::vector< Cluster<EXOUWireSignal> > &uc, const std::vector< Cluster<EXOVWireSignal> > &vc) const;
  double totalUVNLPdf(const Cluster<EXOUWireSignal> &uc, const Cluster<EXOVWireSignal> &vc) const;
public:
  double energyNLPdf(double Uamplitude, double Vamplitude, double Z) const;
  double positionNLPdf(double uPos, double vPos) const;
  double timeNLPdf(double Utime, double Vtime, double Z, bool combined) const;
protected:
  double TryCombination(const std::vector< Cluster<EXOUWireSignal> > &uc, const std::vector< Cluster<EXOVWireSignal> > &vc, EXOMatching& matching, size_t maxN) const;
  bool IsMatchingPhysical(const std::vector< Cluster<EXOUWireSignal> > &uc, const std::vector< Cluster<EXOVWireSignal> > &vc, const EXOMatching& matching) const;
  void FindScintillationCluster(EXOEventData* ED, std::vector< Cluster<EXOUWireSignal> >& uclusters) const;
  void FindUWireInduction(EXOEventData* ED, std::vector< Cluster<EXOUWireSignal> >& uclusters) const;

  EventStatus CreateScintillationClusters(EXOEventData *ED);
  EventStatus CreateChargeClusters(EXOEventData *ED);
  EventStatus CheckCathodeSplit(EXOEventData *ED);
  void SetDriftVelocity(EXOEventData* ED);
  void SetCollectionTime(EXOEventData* ED);

public :

  EXOClusteringModule();
  virtual ~EXOClusteringModule();

  virtual EventStatus BeginOfRun(EXOEventData *ED);
  virtual EventStatus ProcessEvent(EXOEventData *ED);
  virtual EventStatus EndOfRun(EXOEventData *ED);
  virtual int TalkTo(EXOTalkToManager *tm);

  void SetVerbose(int val){fVerbose = val;}
  void SetAPDMatchTime(double val){fAPDMatchTime = val;}
  void SetChargeMatchTime(double val){fUMatchTime = val;}
  void SetChargeVMatchTime(double val){fVMatchTime = val;}
  void SetVTimeOffsetPerChannelDiff(double val){fVTimeOffsetPerChannelDiff = val;}
  void SetUserDriftVelocity(double val){fUserDriftVelocity = val;}
  void SetUserCollectionTime(double val){fUserCollectionTime = val;}
  // void SetCollectionTime(double val){fCollectionTime = val;}
  void SetUseTriggerT0(bool val){fUseTriggerT0 = val;}
  void SetNLLThreshold(int val){fNLogLikelihoodThreshold = val;}
  void SetWriteDebugTree(bool val){fWriteDebugTree = val;}
  void SetDebugFile(std::string val){fDebugFilename = val;}
  void SetIgnoreInduction(bool val){fIgnoreInduction = val;}
  void SetNoMaxDriftTime(bool val){fNoMaxDriftTime = val;} 
  void SetUseNewEnergyPDF(bool val){fUseNewEnergyPDF=val;}

protected:
  enum EDriftVelStatus {   // for status tracking
    kFirstCall,            // First time through
    kUserSet,              // The user set a value manually
    kDatabase,             // Database value (successfully retrieved)
    kDBUpdate,             // Used to que collection updates to use just one fDriftVelocityCalib for drift and collection time
    kDefaultMC,            // Default EXODimensions.hh value because it was a MC event
    kDefaultQueryFail,     // Default EXODimensions.hh value because the database query failed
    kDefaultQueryDisabled  // Default EXODimensions.hh value because code was compiled without HAVE_MYSQL
  };

  int fMaxCost;                //Maximum value in 'cost matrix' for matching
  int fReasonableCost;         //A reasonable cost for an 'ok' matching. Used to fill rows/columns in non-square cost matrices

  int fVerbose;

  double fAPDMatchTime;        //Time used for APD clustering in CLHEP standard units
  double fUMatchTime;          //Time used for u-wire clustering in CLHEP standard units
  double fVMatchTime;          //Time used for v-wire clustering in CLHEP standard units
  double fUIndMatchTime;       //Time used for u-wire induction clustering in CLHEP standard units
  double fVTimeOffsetPerChannelDiff;  //Time offset per #channels away from central signal for V-wire signals in CLHEP standard units

  int fNumSamples;             //Number of samples of the current EXOWaveformData
  int fVetoMuonStart;          //Start time to veto traces associated with a muon
  int fVetoMuonStop;           //Stop time to veto traces associated with a muon
  double fUserDriftVelocity;   //A drift velocity set by the user.
  double fDriftVelocityTPC1;   //Drift velocity for tpc 1.
  double fDriftVelocityTPC2;   //Drift velocity for tpc 2.

  double fUserCollectionTime;  //Time in CLHEP units required to drift from V-wire to U-wire plane set by the user if non-negative
  double fCollectionTimeTPC1;  //... for tpc 1.
  double fCollectionTimeTPC2;  //... for tpc 2.
  
  int fTriggerSample;          //The sample number at which the trigger occured.
  bool fUseTriggerT0;          //Whether to use the trigger time as t0.
  int fNLogLikelihoodThreshold; //The threshold for the negative log likelihood of a combination of a v- and u-cluster
  bool fIgnoreInduction;	//If true, ignore U-wire signals identified as induction
  bool fNoMaxDriftTime;         //If true, let us pair scint and charge with no maximum drift time.
  bool fUseNewEnergyPDF;        //If true, use the New Energy PDF which extends to z=185mm with a Correction

  EXODriftVelocityCalib* fDriftVelocityCalib;
  EDriftVelStatus fDriftStatus;
  EDriftVelStatus fCollectionStatus;

  bool fWriteDebugTree;
  std::string fDebugFilename;
  TTree* fDebugTree;
  TFile* fDebugFile;
  EXOClusterDebugData* fDebugData;
  
  DEFINE_EXO_ANALYSIS_MODULE( EXOClusteringModule )
};

template<typename D>
void EXOClusteringModule::Cluster<D>::AddCluster(const EXOClusteringModule::Cluster<D> &cluster)
{
  childClusterPositions.push_back(signals.size());
  signals.insert(signals.end(),cluster.signals.begin(),cluster.signals.end());
}

template<typename D>
EXOClusteringModule::Cluster<D> EXOClusteringModule::Cluster<D>::GetChild(size_t i) const
{
  Cluster<D> ret;
  ret.fRefScintCluster = fRefScintCluster;
  if(i >= childClusterPositions.size()){
    return ret;
  }
  size_t lower = 0;
  size_t upper = Size();
  if(i > 0){
    lower = childClusterPositions[i];
  }
  if(i+1 < childClusterPositions.size()){
    upper = childClusterPositions[i+1];
  }
  for(size_t j=lower; j<upper; j++){
    ret.AddSignal(signals[j]);
  }
  return ret;
}

template<typename D>
double EXOClusteringModule::Cluster<D>::SignalEnergy(size_t i) const
{
  LogEXOMsg("Not Implemented!",EEAlert);
  return 0.0;
}

template<typename D>
double EXOClusteringModule::Cluster<D>::SignalInductionEnergy(size_t i) const
{
    LogEXOMsg("Not Implemented!",EEAlert);
    return 0.0;
}


template<typename D>
int EXOClusteringModule::Cluster<D>::SignalChannel(size_t i) const
{
  LogEXOMsg("Not Implemented!",EEAlert);
  return 0;
}

template<typename D>
double EXOClusteringModule::Cluster<D>::SignalEnergyError(size_t i) const
{
  LogEXOMsg("Not Implemented!",EEAlert);
  return 0.0;
}

template<typename D>
double EXOClusteringModule::Cluster<D>::SignalInductionEnergyError(size_t i) const
{
  LogEXOMsg("Not Implemented!",EEAlert);
  return 0.0;
}


template <typename D>
std::string EXOClusteringModule::Cluster<D>::Type() const
{
  return "No Type";
}

template <typename D>
double EXOClusteringModule::Cluster<D>::Energy() const
{
  double energy = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    energy += SignalEnergy(i);
  }
  return energy;
}

template <typename D>
double EXOClusteringModule::Cluster<D>::RawEnergy() const
{
  double energy = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    energy += SignalRawEnergy(i);
  }
  return energy;
}

template <typename D>
double EXOClusteringModule::Cluster<D>::InductionEnergy() const
{
    double energy = 0.0;
    for(size_t i=0; i<fRefInductionVector.size(); i++){
        energy += SignalInductionEnergy(i);
    }
    return energy;
}


template <typename D>
double EXOClusteringModule::Cluster<D>::EnergyError() const
{
  double energyerror = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    energyerror += SignalEnergyError(i) * SignalEnergyError(i);
  }
  return sqrt(energyerror);
}

template <typename D>
double EXOClusteringModule::Cluster<D>::RawEnergyError() const
{
  double energyerror = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    energyerror += SignalRawEnergyError(i) * SignalRawEnergyError(i);
  }
  return sqrt(energyerror);
}

template <typename D>
double EXOClusteringModule::Cluster<D>::InductionEnergyError() const
{
    double energyerror = 0.0;
    for(size_t i=0; i<fRefInductionVector.size(); i++){
        energyerror += SignalInductionEnergyError(i) * SignalInductionEnergyError(i);
    }
    return sqrt(energyerror);
}


template <typename D>
double EXOClusteringModule::Cluster<D>::Time() const
{
  return AverageTime();
}

template <typename D>
double EXOClusteringModule::Cluster<D>::AverageTime() const
{
  double time = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    time += SignalEnergy(i) * signals[i]->fTime;
  }
  return time/Energy();
}

template <typename D>
double EXOClusteringModule::Cluster<D>::TimeRMS() const
{
  double timeRMS = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    timeRMS += SignalEnergy(i) * signals[i]->fTime * signals[i]->fTime;
  }
  timeRMS /= Energy();
  double time = AverageTime();
  timeRMS -= time*time;
  if(timeRMS < 0.0){
    timeRMS = 0.0;
  }
  return sqrt(timeRMS);
}

template <typename D>
double EXOClusteringModule::Cluster<D>::Position() const
{
  //Return U- or V- position
  double position = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    position += SignalEnergy(i) * GetMeanUorVPositionFromChannel(signals[i]->fChannel);
  }
  return position/Energy();
}

template<typename D>
double EXOClusteringModule::Cluster<D>::DriftTime() const
{
  //Return the drift time
  if(fRefScintCluster){
    return Time() - fRefScintCluster->fTime;
  }
  return -999.0*CLHEP::microsecond;
}

template<typename D>
double EXOClusteringModule::Cluster<D>::CalculateZwithDriftVelocity(double driftVel, double collTime) const
{
  //Calculate the Z position using a given drift velocity driftVel
  //Assume the units of driftVel to be [length]/microsecond
  //The returned unit is then [length]
  double z = -999.0;
  if(signals.size() < 1){
    return z;
  }

  // Take into account the higher drift velocity in the collection region.  This
  // requires knowing the total drift time from the U-wire to V-wire plane, set
  // by collTime
  if(fRefScintCluster){
    double dt = DriftTime();
    if( dt < collTime ) {
      z = CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE - UPLANE_VPLANE_DISTANCE*dt/collTime;
    } else {
      z = CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE - UPLANE_VPLANE_DISTANCE - driftVel*(dt - collTime);
    }
  }
  else{
    return -999.0;
  }
  if(EXOMiscUtil::GetTPCSide(signals[0]->fChannel) == EXOMiscUtil::kSouth){
    z = -z;
  }
  return z;
}

template <typename D>
double EXOClusteringModule::Cluster<D>::PositionRMS() const
{
  double positionRMS = 0.0;
  for(size_t i=0; i<signals.size(); i++){
    double pos = GetMeanUorVPositionFromChannel(signals[i]->fChannel);
    positionRMS += SignalEnergy(i) * pos * pos;
  }
  positionRMS /= Energy();
  double position = Position();
  positionRMS -= position * position;
  if(positionRMS < 0.0){
    positionRMS = 0.0;
  }
  return sqrt(positionRMS);
}

#endif
