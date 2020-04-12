//______________________________________________________________________________
//EXOClusteringModule combines EXOUWireSignals, EXOVWireSignals and
//EXOAPDSignals to EXOChargeClusters and EXOScintillationClusters.
//
//______________________________________________________________________________
#include "EXOAnalysisManager/EXOClusteringModule.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOPowerSet.hh"
#include "EXOUtilities/EXOMatching.hh"
#include "EXOUtilities/EXOWireCrossing.hh"
#include "EXOUtilities/EXOCoordinates.hh"
#include "EXOUtilities/EXOClusterDebugData.hh"
#include "EXOCalibUtilities/EXODriftVelocityCalib.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include <deque>
#include <cassert>
#include <algorithm> //for sort()
#include <limits>
#include <iostream>
#include <sstream>
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;
using EXOMiscUtil::GetTPCSide;
using EXOMiscUtil::OnSameDetectorHalf;

template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalRawEnergy(size_t i) const
{
  return signals[i]->fRawEnergy;
}

template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalInductionEnergy(size_t i) const
{
      return fRefInductionVector[i]->fMagnitude;
}


template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalEnergy(size_t i) const
{
  return signals[i]->fCorrectedEnergy;
}

template<>
int EXOClusteringModule::Cluster<EXOUWireSignal>::SignalChannel(size_t i) const
{
  return signals[i]->fChannel;
}

template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalRawEnergyError(size_t i) const
{
  return signals[i]->fRawEnergyError;
}

template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalInductionEnergyError(size_t i) const
{
      return fRefInductionVector[i]->fMagnitudeError;
}


template<>
double EXOClusteringModule::Cluster<EXOUWireSignal>::SignalEnergyError(size_t i) const
{ 
  return signals[i]->fCorrectedEnergyError;
}

template<>
double EXOClusteringModule::Cluster<EXOVWireSignal>::SignalRawEnergy(size_t i) const
{
  return signals[i]->fMagnitude;
}

template<>
double EXOClusteringModule::Cluster<EXOVWireSignal>::SignalEnergy(size_t i) const
{
  return signals[i]->fCorrectedMagnitude;
}

template<>
double EXOClusteringModule::Cluster<EXOVWireSignal>::SignalRawEnergyError(size_t i) const
{
  return signals[i]->fMagnitudeError;
}

template<>
double EXOClusteringModule::Cluster<EXOVWireSignal>::SignalEnergyError(size_t i) const
{
  return signals[i]->fCorrectedMagnitudeError;
}

template<>
std::string EXOClusteringModule::Cluster<EXOUWireSignal>::Type() const
{
  return "U-cluster";
}

template<>
std::string EXOClusteringModule::Cluster<EXOVWireSignal>::Type() const
{
  return "V-cluster";
}

template<>
double EXOClusteringModule::Cluster<EXOVWireSignal>::Time() const
{
  //Return the time of the largest (energy) signal.
  //If cluster is combined, return the energy weighted average over its childs.
  if(IsCombined()){
    double timeSum = 0.0;
    double energySum = 0.0;
    size_t nchilds = NumChilds();
    for(size_t i=0; i<nchilds; i++){
      Cluster<EXOVWireSignal> child = GetChild(i);
      double energy = child.Energy();
      timeSum += child.Time() * energy;
      energySum += energy;
    }
    return timeSum / energySum;
  }
  double timeOfMax = -999.;
  double energyOfMax = -999.;
  for(size_t i=0; i<signals.size(); i++){
    if(SignalEnergy(i) > energyOfMax){
      energyOfMax = SignalEnergy(i);
      timeOfMax = signals[i]->fTime;
    }
  }
  return timeOfMax;
}

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOClusteringModule, "cluster" )

EXOClusteringModule::EXOClusteringModule()
: fMaxCost(10000000),
  fReasonableCost(682),
  fVerbose(0),
  fAPDMatchTime(RC_TAU_APD_1+RC_TAU_APD_2),
  fUMatchTime(3.5*SAMPLE_TIME),
  fVMatchTime(4.5*SAMPLE_TIME),
  fUIndMatchTime(50.0*SAMPLE_TIME),
  fVTimeOffsetPerChannelDiff(-2.97*SAMPLE_TIME),
  fNumSamples(0), //Will be set each event
  fVetoMuonStart(-1),  //Will be set each event
  fVetoMuonStop(-1),  //Will be set each event
  fUserDriftVelocity(-1),
  fDriftVelocityTPC1(0), //Will be set each event
  fDriftVelocityTPC2(0), //Will be set each event
  
  fUserCollectionTime(-1),
  fCollectionTimeTPC1(0), //Will be set each event
  fCollectionTimeTPC2(0), //Will be set each event
  fTriggerSample(0), //Will be set each event
  fUseTriggerT0(false),
  fNLogLikelihoodThreshold(8000),
  fIgnoreInduction(false),
  fNoMaxDriftTime(false),
  fUseNewEnergyPDF(false),
  fDriftVelocityCalib(NULL),
  fDriftStatus(kFirstCall),
  fCollectionStatus(kFirstCall),
  fWriteDebugTree(false),
  fDebugFilename("cluster-debug.root"),
  fDebugTree(NULL),
  fDebugFile(NULL),
  fDebugData(NULL)
{

}

EXOClusteringModule::~EXOClusteringModule()
{
  if(fDebugFile){
    if(fDebugFile->IsOpen()){
      fDebugFile->Close();
      fDebugTree = NULL;
    }
  }
  delete fDebugFile;
  delete fDebugTree;
  delete fDebugData;
}

EXOAnalysisModule::EventStatus EXOClusteringModule::BeginOfRun(EXOEventData *ED)
{
  if(fWriteDebugTree){
    fDebugFile = new TFile(fDebugFilename.c_str(),"RECREATE");
    fDebugTree = new TTree("ClusterDebug","ClusterDebug");
    fDebugData = new EXOClusterDebugData();

    fDebugTree->Branch("DebugData",fDebugData);
  }
  return kOk;
}

EXOAnalysisModule::EventStatus EXOClusteringModule::EndOfRun(EXOEventData *ED)
{
  if(fWriteDebugTree){
    fDebugFile->Write("", TObject::kOverwrite);
    fDebugFile->Close();
    fDebugTree = NULL;
    delete fDebugFile;
    fDebugFile = NULL;
    delete fDebugData;
    fDebugData = NULL;
  }
  return kOk;
}

EXOAnalysisModule::EventStatus EXOClusteringModule::ProcessEvent(EXOEventData *ED)
{
  if(fVerbose){
    cout << "-------------------------------------------------------------------------" << endl;
    cout << endl << endl;
    cout << "Processing event " << ED->fEventNumber << endl;
  }

  if(fWriteDebugTree){
    fDebugData->Clear();
    fDebugData->fEventNumber = ED->fEventNumber;
  }

  // Clear out all clusters before starting.
  while(ED->GetNumScintillationClusters() > 0) ED->Remove(ED->GetScintillationCluster(0));
  while(ED->GetNumChargeClusters() > 0) ED->Remove(ED->GetChargeCluster(0));

  fNumSamples = ED->fEventHeader.fSampleCount + 1;

  SetDriftVelocity(ED);
  SetCollectionTime(ED); // now event by event as well as ^^

  // Need to handle events tagged as muons separately to avoid skipping them
  // if there's a capture gamma that we want to reconstruct
  if(ED->fEventHeader.fTaggedAsMuon){
    // veto the muon start time pulse the max drift time, with 10 sample
    // buffer on either side

    // First get the max drift time for this event, round to nearest sample, 
    // and convert to microseconds
    int maxDriftTimeTPC1 = int((CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE) /  (fDriftVelocityTPC1*CLHEP::microsecond));
    int maxDriftTimeTPC2 = int((CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE) /  (fDriftVelocityTPC2*CLHEP::microsecond));
    int maxDriftTime = maxDriftTimeTPC1 > maxDriftTimeTPC2 ? maxDriftTimeTPC1 : maxDriftTimeTPC2;

    fVetoMuonStart = ED->fEventHeader.fMuonTime > 10 ? ED->fEventHeader.fMuonTime-10 : 0;
    fVetoMuonStop = ED->fEventHeader.fMuonTime < fNumSamples-maxDriftTime-10 ? ED->fEventHeader.fMuonTime+maxDriftTime+10 : fNumSamples;
  } else {
    // Not a muon, so don't veto any signals
    fVetoMuonStart = -1;
    fVetoMuonStop = -1;
  }

  int trig_offset = ED->fEventHeader.fTriggerOffset; // ED->fEventHeader.fTriggerOffset==0 means we keep the old value.
  if ( trig_offset > fNumSamples ) {
    LogEXOMsg("trigger time lies outside of trace", EEDebug);
    ED->fSkippedByClustering = true;
    return kDrop;
  }
  if (trig_offset > 0) {
    fTriggerSample = trig_offset;
  }

  EventStatus returnStatus;
  returnStatus = CreateScintillationClusters(ED);
  if(returnStatus != kOk) return returnStatus;
  returnStatus = CreateChargeClusters(ED);
  if(returnStatus != kOk) return returnStatus;
  CheckCathodeSplit(ED);

  if(fWriteDebugTree){
    fDebugTree->Fill();
  }

  return returnStatus;
}

int EXOClusteringModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/cluster/use_trigger_t0",
                               "Forces the use of MC true t0",
                               this,
                               fUseTriggerT0,
                               &EXOClusteringModule::SetUseTriggerT0 );

  talktoManager->CreateCommand("/cluster/drift_velocity_mm_per_ns",
                               "Set a user electron drift velocity instead of database or MC value",
                               this,
                               fUserDriftVelocity,
                               &EXOClusteringModule::SetUserDriftVelocity );

  talktoManager->CreateCommand("/cluster/verbose",
                               "Set verbosity",
                               this,
                               fVerbose,
                               &EXOClusteringModule::SetVerbose );

  talktoManager->CreateCommand("/cluster/apd_match_time",
                               "Set the apd match time in ns",
                               this,
                               fAPDMatchTime,
                               &EXOClusteringModule::SetAPDMatchTime );

  talktoManager->CreateCommand("/cluster/charge_match_time",
                               "Set the charge match time in ns",
                               this,
                               fUMatchTime,
                               &EXOClusteringModule::SetChargeMatchTime );

  talktoManager->CreateCommand("/cluster/charge_v-match_time",
                               "Set the charge match time in ns",
                               this,
                               fVMatchTime,
                               &EXOClusteringModule::SetChargeVMatchTime );

  talktoManager->CreateCommand("/cluster/v_time_offset_per_channel",
                               "Set the time offset per #channels away from central V-signal",
                               this,
                               fVTimeOffsetPerChannelDiff,
                               &EXOClusteringModule::SetVTimeOffsetPerChannelDiff );

  talktoManager->CreateCommand("/cluster/nll_threshold",
                               "Set the negative log likelihood threshold for a combination of a v- and u-cluster",
                               this,
                               fNLogLikelihoodThreshold,
                               &EXOClusteringModule::SetNLLThreshold );

  talktoManager->CreateCommand("/cluster/debug_tree",
                               "Set whether to fill and write the debug tree",
                               this,
                               fWriteDebugTree,
                               &EXOClusteringModule::SetWriteDebugTree );

  talktoManager->CreateCommand("/cluster/debug_file",
                               "Set the file into which the debug tree should be written",
                               this,
                               fDebugFilename,
                               &EXOClusteringModule::SetDebugFile );
  
  talktoManager->CreateCommand("/cluster/ignore_induction",
                               "Ignore U-wire signals identified as induction",
                               this,
                               fIgnoreInduction,
                               &EXOClusteringModule::SetIgnoreInduction );

  talktoManager->CreateCommand("/cluster/collection_time",
                               "Set a user-given time needed to drift from V-wire to U-wire plane (instead of DB values)",
                               this,
                               fUserCollectionTime,
                               &EXOClusteringModule::SetUserCollectionTime );

  talktoManager->CreateCommand("/cluster/SetNoMaxDriftTime",
                               "If true, there is no maximum drift time for pairing scint and charge",
                               this,
                               fNoMaxDriftTime,
                               &EXOClusteringModule::SetNoMaxDriftTime );

  talktoManager->CreateCommand("/cluster/useNewEnergyPDF",
                               "If true than use the updated Clustering PDF that corrects the expected V-Energy based on Z-Position",
                               this,
                               fUseNewEnergyPDF,
                               &EXOClusteringModule::SetUseNewEnergyPDF );
  

  return 0;
}

EXOAnalysisModule::EventStatus EXOClusteringModule::CreateScintillationClusters(EXOEventData *ED)
{
  //Creates ScintillationClusters based on summed EXOAPDSignals.
  //If we have separate sum signals for each apd plane, then two
  //sum signals are clustered together if they lie on opposite APD planes
  //and have a time difference which is smaller than fAPDMatchTime.
  //Also adds individual gang EXOAPDSignals to those clusters.

  std::deque<EXOAPDSignal*> signals;
  bool sumBothPlanes = false;
  int nSignals = ED->GetNumAPDSignals();

  //First add all summed signals to a deque.
  for(int i=0; i<nSignals; i++){
    EXOAPDSignal *sig = ED->GetAPDSignal(i);
    if(sig->fType == EXOAPDSignal::kGangFit){
      continue;
    }
    if(sig->fType == EXOAPDSignal::kFullFit){
      sumBothPlanes = true;
    }
    //Don't include signals associated with a muon
    if( sig->fTime > fVetoMuonStart*CLHEP::microsecond and 
	sig->fTime < fVetoMuonStop*CLHEP::microsecond ) {
      continue;
    }
    signals.push_back(sig);
  };

  //Now sort the summed signals in time.
  APDSignalSorter sorter;
  std::sort(signals.begin(),signals.end(),sorter);

  if(sumBothPlanes){
    //If we summed both apdplanes it's easy. Just create a scintillation cluster for each
    //summed signal.
    while(signals.size() > 0){
      EXOAPDSignal *sig = signals.front();
      signals.pop_front();
      EXOScintillationCluster* NewCluster = ED->GetNewScintillationCluster();
      sig->SetScintCluster(NewCluster);
      NewCluster->fTime = sig->fTime;
      NewCluster->InsertAPDSignal(sig);
    }
  }
  else{
    //If we have separate sum signals for each apd plane, cluster those sum-apd-signals
    //that lie on different apd planes and have only a small time difference (fAPDMatchTime).
    while(signals.size() > 1){
      EXOAPDSignal *sig1 = signals.front();
      signals.pop_front();
      EXOAPDSignal *sig2 = signals.front();
      if((sig1->fChannel != sig2->fChannel) && (fabs(sig1->fTime-sig2->fTime) < fAPDMatchTime)){
        //Opposite APD planes and similar time
        double time = (sig1->fTime*sig1->fRawCounts + sig2->fTime*sig2->fRawCounts) / (sig1->fRawCounts + sig2->fRawCounts);
        EXOScintillationCluster* NewCluster = ED->GetNewScintillationCluster();
        NewCluster->fTime = time;
        sig1->SetScintCluster(NewCluster);
        NewCluster->InsertAPDSignal(sig1);
        sig2->SetScintCluster(NewCluster);
        NewCluster->InsertAPDSignal(sig2);
        signals.pop_front();
      }
      else{
        EXOScintillationCluster* NewCluster = ED->GetNewScintillationCluster();
        sig1->SetScintCluster(NewCluster);
        NewCluster->fTime = sig1->fTime;
        NewCluster->InsertAPDSignal(sig1);
      }
    }
    if(signals.size() > 0){
      //There might still be one signal left
      EXOAPDSignal *sig = signals.front();
      signals.pop_front();
      EXOScintillationCluster* NewCluster = ED->GetNewScintillationCluster();
      sig->SetScintCluster(NewCluster);
      NewCluster->fTime = sig->fTime;
      NewCluster->InsertAPDSignal(sig);
    }
  }

  //Now add the individual gang signals
  signals.clear();

  for(int i=0; i<nSignals; i++){
    EXOAPDSignal *sig = ED->GetAPDSignal(i);

    //Don't include signals associated with a muon
    if( sig->fTime > fVetoMuonStart*CLHEP::microsecond and 
	sig->fTime < fVetoMuonStop*CLHEP::microsecond ) {
      continue;
    }

    if(sig->fType == EXOAPDSignal::kGangFit){
      signals.push_back(sig);
    }
  };

  //Sort the individual gang signals in time
  std::sort(signals.begin(),signals.end(),sorter);

  unsigned int i = 0;
  //The scintillation clusters as well as the individual gang signals are ordered in time
  while(signals.size() > 0){
    assert(i < ED->GetNumScintillationClusters());
    EXOScintillationCluster* sc = ED->GetScintillationCluster(i);
    EXOAPDSignal *sig = signals.front();
    if(fabs(sig->fTime - sc->fTime) > fAPDMatchTime){
      //If gang signal and scintillation cluster time are not similar anymore, go to the
      //next scintillation cluster
      i++;
      continue;
    }
    //Add the gang signal to the scintillation cluster
    sig->SetScintCluster(sc);
    sc->InsertAPDSignal(sig);
    signals.pop_front();
  }

  // clear out empty clusters
  std::vector<EXOScintillationCluster*> clear_vector;
  for (size_t i = 0; i < ED->GetNumScintillationClusters(); i++) {
    EXOScintillationCluster* scint_clus = ED->GetScintillationCluster(i);
    if ( (scint_clus->FindAPDSignalLocation(EXOAPDSignal::kGangFit,0))==-1 ) { 
      // set the cluster to be cleared 
      clear_vector.push_back(scint_clus);
      break;
    }
  }
  for (unsigned int i=0;i<clear_vector.size();i++) {
    // Removing scintillation clusters that were empty
    ED->Remove(clear_vector[i]);
  }
  return kOk;
}

template <typename D>
void EXOClusteringModule::PrintSignals(const vector< Cluster<D> >& clusters) const
{
  for(size_t i=0; i<clusters.size(); i++){
    stringstream channels;
    channels << "{";
    for(size_t j=0; j<clusters[i].signals.size(); j++){
      if(j){
        channels << ",";
      }
      channels << clusters[i].signals[j]->fChannel;
    }
    channels << "}";
    cout << "  " << clusters[i].Type() << "[" << i << "]: energy = " << clusters[i].Energy() << ", time = " << clusters[i].Time()/1000. << ", channels = " << channels.str() << endl;
  }
}

EXOAnalysisModule::EventStatus EXOClusteringModule::CreateChargeClusters(EXOEventData *ED)
{
  //Creates EXOChargeClusters given the EXO[U,V]WireSignals in ED

  //For u- and v-wire signals create two lists. One that is sorted by energy
  //and one that is sorted by channel number
  list<EXOUWireSignal*> UenergySorted;
  for(unsigned int i=0; i<ED->GetNumUWireSignals(); i++){
    EXOUWireSignal* uws = ED->GetUWireSignal(i);
    if(fIgnoreInduction && uws->fIsInduction){
      //Do not cluster induction signals
      continue;
    }
    //Don't include signals associated with a muon
    if( uws->fTime > fVetoMuonStart*CLHEP::microsecond and 
	uws->fTime < fVetoMuonStop*CLHEP::microsecond ) {
      continue;
    }
    UenergySorted.push_back(uws);
  }
  USignalEnergySorter UenergySorter;
  UenergySorted.sort(UenergySorter);
  list<EXOUWireSignal*> UchannelSorted(UenergySorted);
  USignalChannelSorter UchannelSorter;
  UchannelSorted.sort(UchannelSorter);

  list<EXOVWireSignal*> VenergySorted;
  for(unsigned int i=0; i<ED->GetNumVWireSignals(); i++){
    VenergySorted.push_back(ED->GetVWireSignal(i));
  }
  VSignalEnergySorter VenergySorter;
  VenergySorted.sort(VenergySorter);
  list<EXOVWireSignal*> VchannelSorted(VenergySorted);
  VSignalChannelSorter VchannelSorter;
  VchannelSorted.sort(VchannelSorter);

  //Bundle together (separately) U-wire and v-wire signals.
  //i.e. fill Uclusters and Vclusters
  vector< Cluster<EXOUWireSignal> > Uclusters;
  vector< Cluster<EXOVWireSignal> > Vclusters;
  EventStatus status = ClusterWires(UenergySorted,UchannelSorted,fUMatchTime,0.0,Uclusters);
  if(status != kOk){
    return status;
  }
  status = ClusterWires(VenergySorted,VchannelSorted,fVMatchTime,fVTimeOffsetPerChannelDiff,Vclusters);
  if(status != kOk){
    return status;
  }

  CullVWires(ED,Uclusters,Vclusters);

  if(fVerbose > 1){
    cout << "Initial Bundles:" << endl;
    PrintSignals(Uclusters);
    PrintSignals(Vclusters);
  }
  set< set<int> > uCombinations;
  set< set<int> > vCombinations;
  try{
    //Combining U-clusters actually means splitting V-clusters, so use fVMatchTime
    uCombinations = CreateCombinations(Uclusters,1.5*fVMatchTime);
    //Combining V-clusters actually means splitting U-clusters, so use fUMatchTime
    vCombinations = CreateCombinations(Vclusters,1.5*fUMatchTime);
  }
  catch(CombinationLimitException& e){
    LogEXOMsg("Too many signals at the same time. Skipping event.",EEWarning);
    ED->fSkippedByClustering = true;
    return kDrop;
  }

  //Adding the empty set to the combinations, so that uncombined u-clusters are
  //matched with combined v-clusters and vice versa
  uCombinations.insert(set<int>());
  vCombinations.insert(set<int>());

  if(fVerbose > 1){
    cout << "The following U-cluster combinations are proposed:" << endl;
    for(set< set<int> >::iterator it1 = uCombinations.begin(); it1 != uCombinations.end(); it1++){
      cout << "{";
      for(set<int>::iterator it2 = it1->begin(); it2 != it1->end(); it2++){
        cout << *it2;
        set<int>::iterator next = it2;
        next++;
        if(next != it1->end()){
          cout << ",";
        }
      }
      cout << "}" << endl;
    }

    cout << "The following V-cluster combinations are proposed:" << endl;
    for(set< set<int> >::iterator it1 = vCombinations.begin(); it1 != vCombinations.end(); it1++){
      cout << "{";
      for(set<int>::iterator it2 = it1->begin(); it2 != it1->end(); it2++){
        cout << *it2;
        set<int>::iterator next = it2;
        next++;
        if(next != it1->end()){
          cout << ",";
        }
      }
      cout << "}" << endl;
    }
  }

  vector< Cluster<EXOUWireSignal> > bestUclusters(Uclusters);
  vector< Cluster<EXOVWireSignal> > bestVclusters(Vclusters);
  FindScintillationCluster(ED,bestUclusters);
  EXOMatching matching,bestMatching(max(bestUclusters.size(),bestVclusters.size()),fMaxCost);
  size_t originalNumClusters = min(Uclusters.size(),Vclusters.size());
  double nllRatio, minNllRatio = numeric_limits<double>::max();
  for(set< set<int> >::iterator uSet = uCombinations.begin(); uSet != uCombinations.end(); uSet++){
    vector<Cluster<EXOUWireSignal> > UclustersAndCombined(Uclusters);
    Cluster<EXOUWireSignal> uCombined;
    for(set<int>::iterator uIter = uSet->begin(); uIter != uSet->end(); uIter++){
      uCombined.AddCluster(Uclusters[*uIter]);
    }
    if(uCombined.Size() > 0){
      //remove clusters that were combined
      size_t counter = 0;
      for(set<int>::iterator uIter = uSet->begin(); uIter != uSet->end(); uIter++){
        UclustersAndCombined.erase(UclustersAndCombined.begin() + *uIter - counter);
        counter++;
      }
      UclustersAndCombined.push_back(uCombined);
    }
    FindScintillationCluster(ED,UclustersAndCombined);
    FindUWireInduction(ED,UclustersAndCombined);
    for(set< set<int> >::iterator vSet = vCombinations.begin(); vSet != vCombinations.end(); vSet++){
      vector<Cluster<EXOVWireSignal> > VclustersAndCombined(Vclusters);
      Cluster<EXOVWireSignal> vCombined;
      for(set<int>::iterator vIter = vSet->begin(); vIter != vSet->end(); vIter++){
        vCombined.AddCluster(Vclusters[*vIter]);
      }
      if(vCombined.Size() > 0){
        //remove clusters that were combined
        size_t counter = 0;
        for(set<int>::iterator vIter = vSet->begin(); vIter != vSet->end(); vIter++){
          VclustersAndCombined.erase(VclustersAndCombined.begin() + *vIter - counter);
          counter++;
        }
        VclustersAndCombined.push_back(vCombined);
      }
      nllRatio = TryCombination(UclustersAndCombined,VclustersAndCombined,matching,originalNumClusters);
      if(nllRatio < minNllRatio and IsMatchingPhysical(UclustersAndCombined,VclustersAndCombined,matching)){
        minNllRatio = nllRatio;
        bestMatching = matching;
        bestUclusters = UclustersAndCombined;
        bestVclusters = VclustersAndCombined;
      }
    }
  }
  if(fVerbose){
    cout << "The best matching reads:" << endl;
    bestMatching.Print();
    for(int i=0; i<bestMatching.GetN(); i++){
      int whichV = bestMatching.GetMatchedY(i);
      cout << "Cost(U[" << i << "] --> V[" << whichV << "]) = " << bestMatching.GetCost(i,whichV) << endl;
    }
  }

  for(size_t whichU=0; whichU<size_t(bestMatching.GetN()); whichU++){
    size_t whichV = bestMatching.GetMatchedY(whichU);
    if(whichU >= bestUclusters.size() && whichV >= bestVclusters.size()){
      continue;
    }
    if(whichU >= bestUclusters.size()){
      //An unmatched v-cluster
      if(fWriteDebugTree){
        double venergy = bestVclusters[whichV].Energy();
        double vtime = bestVclusters[whichV].Time();
        fDebugData->fZMatched.push_back(-999);
        fDebugData->fEnergyMatched.push_back(0);
        fDebugData->fVEnergyMatched.push_back(venergy);
        fDebugData->fTimeMatched.push_back(vtime);
        fDebugData->fTimeDiffMatched.push_back(-999);
        fDebugData->fCostMatched.push_back(-999);
        fDebugData->fNLEnergyMatched.push_back(-999);
        fDebugData->fNLTimeMatched.push_back(-999);
        fDebugData->fIsUCombinedMatched.push_back(false);
        fDebugData->fIsVCombinedMatched.push_back(bestVclusters[whichV].IsCombined());
      }
      NewChargeCluster(ED,0,&bestVclusters[whichV]);
    }
    else if(whichV >= bestVclusters.size()){
      //An unmatched u-cluster
      if(fWriteDebugTree){
        const double& driftVel = GetTPCSide(bestUclusters[whichU].signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
	const double& collTime = GetTPCSide(bestUclusters[whichU].signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2;
        double Z = bestUclusters[whichU].CalculateZwithDriftVelocity(driftVel,collTime);
        double uenergy = bestUclusters[whichU].Energy();
        double utime = bestUclusters[whichU].Time();
        fDebugData->fZMatched.push_back(Z);
        fDebugData->fEnergyMatched.push_back(uenergy);
        fDebugData->fVEnergyMatched.push_back(0);
        fDebugData->fTimeMatched.push_back(utime);
        fDebugData->fTimeDiffMatched.push_back(-999);
        fDebugData->fCostMatched.push_back(-999);
        fDebugData->fNLEnergyMatched.push_back(-999);
        fDebugData->fNLTimeMatched.push_back(-999);
        fDebugData->fIsUCombinedMatched.push_back(bestUclusters[whichU].IsCombined());
        fDebugData->fIsVCombinedMatched.push_back(false);
      }
      NewChargeCluster(ED,&bestUclusters[whichU],0);
    }
    else{
      //A matching was found ...
      int cost = bestMatching.GetCost(whichU,whichV);
      if(fWriteDebugTree){
        const double& driftVel = GetTPCSide(bestUclusters[whichU].signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
	const double& collTime = GetTPCSide(bestUclusters[whichU].signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2; 
        double Z = bestUclusters[whichU].CalculateZwithDriftVelocity(driftVel,collTime);
        double uenergy = bestUclusters[whichU].Energy();
        double venergy = bestVclusters[whichV].Energy();
        double utime = bestUclusters[whichU].Time();
        double vtime = bestVclusters[whichV].Time();
        fDebugData->fZMatched.push_back(Z);
        fDebugData->fEnergyMatched.push_back(uenergy);
        fDebugData->fVEnergyMatched.push_back(venergy);
        fDebugData->fTimeMatched.push_back(utime);
        fDebugData->fTimeDiffMatched.push_back(utime - vtime);
        fDebugData->fCostMatched.push_back(cost);
        fDebugData->fNLEnergyMatched.push_back(energyNLPdf(uenergy,venergy,Z));
        fDebugData->fNLTimeMatched.push_back(timeNLPdf(utime,vtime,Z,bestUclusters[whichU].IsCombined() or bestVclusters[whichV].IsCombined()));
        fDebugData->fIsUCombinedMatched.push_back(bestUclusters[whichU].IsCombined());
        fDebugData->fIsVCombinedMatched.push_back(bestVclusters[whichV].IsCombined());
      }
      if(cost < fNLogLikelihoodThreshold){
        //... and it's likely enough. Match them.
        NewChargeCluster(ED,&bestUclusters[whichU],&bestVclusters[whichV]);
      }
      else{
        //... but it's very unlikely. Don't match them
        NewChargeCluster(ED,&bestUclusters[whichU],0);
        NewChargeCluster(ED,0,&bestVclusters[whichV]);
      }
    }
  }

  return kOk;
}

bool EXOClusteringModule::IsMatchingPhysical(const vector< Cluster<EXOUWireSignal> > &uc, const vector< Cluster<EXOVWireSignal> > &vc, const EXOMatching& matching) const
{
  //Return whether matching is reasonable or not
  for(size_t whichU=0; whichU<uc.size(); whichU++){
    size_t whichV = matching.GetMatchedY(whichU);
    if(whichV >= vc.size()){
      continue;
    }
    if(!OnSameDetectorHalf(uc[whichU].signals[0]->fChannel,vc[whichV].signals[0]->fChannel)){
      return false;
    }
    if(uc[whichU].IsCombined() && vc[whichV].IsCombined()){
      return false;
    }
  }
  return true;
}

double EXOClusteringModule::TryCombination(const vector< Cluster<EXOUWireSignal> > &uc, const vector< Cluster<EXOVWireSignal> > &vc, EXOMatching& matching, size_t maxN) const
{
  EXOMatrix<int> costMatrix = LogLikelihoodMatrix(uc,vc);
  //if the matrix is not a square matrix, add rows/columns with "reasonable" cost so that
  //it becomes square
  int Nmore = int(costMatrix.GetNrows()) - int(costMatrix.GetNcols());
  if(Nmore > 0){
    for(int i=0; i<Nmore; i++){
      costMatrix.AddColumn(fReasonableCost);
    }
  }
  else if(Nmore < 0){
    Nmore = -Nmore;
    for(int i=0; i<Nmore; i++){
      costMatrix.AddRow(fReasonableCost);
    }
  }
  double costSum = matching.hungarian(costMatrix,maxN,true);
  if(fVerbose > 1){
    costMatrix.Print();
    cout << "Sum of costs: " << costSum << endl;
    matching.Print();
  }
  if(maxN == 0){
    maxN = 1;
  }
  return costSum/maxN;
}

void EXOClusteringModule::NewChargeCluster(EXOEventData* ED, const Cluster<EXOUWireSignal> *uc, const Cluster<EXOVWireSignal> *vc, double UEnergyRatio, double VEnergyRatio) const
{
  static const double sqrt3 = sqrt(3.0);
  double driftVel = fDriftVelocityTPC1;
  double collTime = fCollectionTimeTPC1;
  if(!(uc || vc)){
    return;
  }
  
  if(!uc){
    //No charge was collected but we have one or more v-clusters
    driftVel = GetTPCSide(vc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
    collTime = GetTPCSide(vc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2;
    if(vc->IsCombined()){
      for(size_t i=0; i<vc->NumChilds(); i++){
        Cluster<EXOVWireSignal> child = vc->GetChild(i);
        NewChargeCluster(ED,uc,&child);
      }
      return;
    }
    EXOChargeCluster* cc = ED->GetNewChargeCluster();
    cc->fDetectorHalf = GetTPCSide(vc->signals[0]->fChannel);
    cc->fRawEnergy = 0.0;
    cc->fRawEnergyError = 0.0;
    cc->fInductionEnergy = 0.0;
    cc->fInductionEnergyError = 0.0;
    cc->fCorrectedEnergy = 0.0;
    cc->fCorrectedEnergyError = 0.0;
    cc->fAmplitudeInVChannels = vc->RawEnergy();
    cc->fCorrectedAmplitudeInVChannels = vc->Energy();
    cc->fCollectionTime = vc->Time();
    cc->fU = -999.0;
    cc->fV = vc->Position();
    cc->fX = -999.0;
    cc->fY = -999.0;
    cc->fURMSFoundSignals = 0.0;
    cc->fVRMSFoundSignals = vc->PositionRMS();
    cc->fZRMSFoundSignals = 0.0;
    cc->fDriftTime = -999.0 * CLHEP::microsecond;
    cc->fZ = -999.0;

    //Set links
    for(size_t i=0; i<vc->signals.size(); i++){
      cc->InsertVWireSignal(vc->signals[i]);
      vc->signals[i]->InsertChargeCluster(cc);
    }
    return;
  }
  else if(!vc){
    //One or more charge cluster was collected but we have no v-cluster
    driftVel = GetTPCSide(uc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
    collTime = GetTPCSide(uc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2;
    if(uc->IsCombined()){
      for(size_t i=0; i<uc->NumChilds(); i++){
        Cluster<EXOUWireSignal> child = uc->GetChild(i);
        NewChargeCluster(ED,&child,vc);
      }
      return;
    }
    EXOChargeCluster* cc = ED->GetNewChargeCluster();
    cc->fDetectorHalf = GetTPCSide(uc->signals[0]->fChannel);
    cc->fRawEnergy = uc->RawEnergy();
    cc->fRawEnergyError = uc->RawEnergyError();
    cc->fInductionEnergy = uc->InductionEnergy();
    cc->fInductionEnergyError = uc->InductionEnergyError();
    cc->fCorrectedEnergy    = uc->Energy();
    cc->fCorrectedEnergyError = uc->EnergyError();
    cc->fAmplitudeInVChannels = 0.0;
    cc->fCorrectedAmplitudeInVChannels = 0.0;
    cc->fCollectionTime = uc->Time();
    cc->fU = uc->Position();
    cc->fV = -999.0;
    cc->fX = -999.0;
    cc->fY = -999.0;
    cc->fURMSFoundSignals = uc->PositionRMS();
    cc->fVRMSFoundSignals = 0.0;
    cc->fZRMSFoundSignals = uc->TimeRMS() * driftVel;
    cc->fDriftTime = uc->DriftTime();
    cc->fZ = uc->CalculateZwithDriftVelocity(driftVel,collTime);

    //Set links
    if(uc->fRefScintCluster){
      cc->SetScintillationCluster(uc->fRefScintCluster);
      cc->GetScintillationCluster()->InsertChargeCluster(cc);
    }
    for(size_t i=0; i<uc->signals.size(); i++){
      cc->InsertUWireSignal(uc->signals[i]);
      uc->signals[i]->InsertChargeCluster(cc);
    }
    for(size_t i=0; i<uc->fRefInductionVector.size(); i++){
        cc->InsertUWireInductionSignal(uc->fRefInductionVector[i]);
    }
    return;
  }

  else{
    //Charge was collected and we have a corresponding v-cluster
    driftVel = GetTPCSide(uc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
    collTime = GetTPCSide(uc->signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2;
    if(uc->IsCombined() && vc->IsCombined()){
      LogEXOMsg("Trying to associate U- and V- clusters which *both* were combined!",EEError);
      return;
    }
    if(uc->IsCombined()){
      double totalEnergy = uc->Energy();
      for(size_t i=0; i<uc->NumChilds(); i++){
        Cluster<EXOUWireSignal> child = uc->GetChild(i);
        NewChargeCluster(ED,&child,vc,1.0,child.Energy()/totalEnergy);
      }
      return;
    }
    else if(vc->IsCombined()){
      double totalAmplitude = vc->Energy();
      for(size_t i=0; i<vc->NumChilds(); i++){
        Cluster<EXOVWireSignal> child = vc->GetChild(i);
        NewChargeCluster(ED,uc,&child,child.Energy()/totalAmplitude,1.0);
      }
      return;
    }
    else{
      //Neither uc nor vc is combined
      EXOChargeCluster* cc = ED->GetNewChargeCluster();
      cc->fDetectorHalf = GetTPCSide(uc->signals[0]->fChannel);
      cc->fRawEnergy = uc->RawEnergy() * UEnergyRatio;
      cc->fRawEnergyError = uc->RawEnergyError();
      cc->fInductionEnergy = uc->InductionEnergy();
      cc->fInductionEnergyError = uc->InductionEnergyError();
      cc->fCorrectedEnergy    = uc->Energy() * UEnergyRatio;
      cc->fCorrectedEnergyError = uc->EnergyError();
      cc->fAmplitudeInVChannels = vc->RawEnergy() * VEnergyRatio;
      cc->fCorrectedAmplitudeInVChannels = vc->Energy() * VEnergyRatio;
      cc->fCollectionTime = uc->Time();
      cc->fU = uc->Position();
      cc->fV = vc->Position();
      if(cc->fDetectorHalf == 0){
        cc->fX = cc->fV - cc->fU;
      }
      else{
        cc->fX = cc->fU - cc->fV;
      }
      static const double sqrt3 = sqrt(3.0);
      cc->fY = (cc->fU + cc->fV) / sqrt3;
      cc->fURMSFoundSignals = uc->PositionRMS();
      cc->fVRMSFoundSignals = vc->PositionRMS();
      cc->fZRMSFoundSignals = uc->TimeRMS() * (cc->fDetectorHalf==0 ? fDriftVelocityTPC1 : fDriftVelocityTPC2);
      cc->fDriftTime = uc->DriftTime();
      cc->fZ = uc->CalculateZwithDriftVelocity(driftVel,collTime);

      //Set links
      if(uc->fRefScintCluster){
        cc->SetScintillationCluster(uc->fRefScintCluster);
        cc->GetScintillationCluster()->InsertChargeCluster(cc);
      }
      for(size_t i=0; i<uc->signals.size(); i++){
        cc->InsertUWireSignal(uc->signals[i]);
        uc->signals[i]->InsertChargeCluster(cc);
      }
      for(size_t i=0; i<vc->signals.size(); i++){
        cc->InsertVWireSignal(vc->signals[i]);
        vc->signals[i]->InsertChargeCluster(cc);
      }
      for(size_t i=0; i<uc->fRefInductionVector.size(); i++){
        cc->InsertUWireInductionSignal(uc->fRefInductionVector[i]);
      }
    }
  }
}

EXOMatrix<int> EXOClusteringModule::LogLikelihoodMatrix(const vector< Cluster<EXOUWireSignal> > &uc, const vector< Cluster<EXOVWireSignal> > &vc) const
{
  //Return a matrix whose elements are 1000 * the negative log of the total pdf of U-V-combinations.
  //U-clusters correspond to rows, V-clusters to columns.
  EXOMatrix<int> mat(uc.size(),vc.size());
  for(int i=0; i<int(uc.size()); i++){
    for(int j=0; j<int(vc.size()); j++){
      if(!OnSameDetectorHalf(uc[i].signals[0]->fChannel,vc[j].signals[0]->fChannel)){
        //Don't match u- and v- clusters if they are on opposite detector sides
        //Highest possible 'cost'
        mat[i][j] = fMaxCost;
        continue;
      }
      if(uc[i].IsCombined() && vc[j].IsCombined()){
        //Don't match a combined u-cluster with a combined v-cluster.
        //Highest possible 'cost'
        mat[i][j] = fMaxCost;
      }
      mat[i][j] = int(min(1000*totalUVNLPdf(uc[i],vc[j]),double(fMaxCost)));
    }
  }
  return mat;
}

template <typename D>
EXOAnalysisModule::EventStatus EXOClusteringModule::ClusterWires(std::list<D*> &energySorted, std::list<D*> &channelSorted, double matchTime, double timeOffsetPerChannelDiff, std::vector< Cluster<D> > &clusters)
{
  while(energySorted.size() > 0){
    std::list<D*> clusterSignals;
    Cluster<D> cluster;
    D* comparisonSignal = energySorted.front();

    //Let comparisonIterator point to the element with biggest energy
    typename std::list<D*>::iterator comparisonIterator = channelSorted.begin();
    while(*comparisonIterator != comparisonSignal){
      comparisonIterator++;
    }

    int previousChannel = comparisonSignal->fChannel;
    typename std::list<D*>::reverse_iterator riter(comparisonIterator);
    while(riter != channelSorted.rend()){
      if(abs((*riter)->fChannel - previousChannel)> 1){
        break;
      }
      if( fabs((*riter)->fTime - timeOffsetPerChannelDiff*abs((*riter)->fChannel - comparisonSignal->fChannel) - comparisonSignal->fTime) > matchTime ){
        riter++;
        continue;
      }
      previousChannel = (*riter)->fChannel;
      cluster.AddSignal(*riter);
      D* temp = *riter;
      if(temp == comparisonSignal){
        LogEXOMsg("WARNING removing comparisonSignal!",EECritical);
        return kError;
      }
      //riter++;
      energySorted.remove(temp);
      channelSorted.remove(temp);
    }
    previousChannel = comparisonSignal->fChannel;
    /*
    comparisonIterator = channelSorted.begin();
    while(*comparisonIterator != comparisonSignal){
      comparisonIterator++;
    }
    */
    typename std::list<D*>::iterator iter(comparisonIterator);
    iter++;
    while(iter != channelSorted.end()){
      if(abs((*iter)->fChannel - previousChannel) > 1){
        break;
      }
      if( fabs((*iter)->fTime - timeOffsetPerChannelDiff*abs((*iter)->fChannel - comparisonSignal->fChannel) - comparisonSignal->fTime) > matchTime ){
        iter++;
        continue;
      }
      previousChannel = (*iter)->fChannel;
      cluster.AddSignal(*iter);
      D* temp = *iter;
      iter++;
      energySorted.remove(temp);
      channelSorted.remove(temp);
    }
    channelSorted.remove(comparisonSignal);
    energySorted.remove(comparisonSignal);
    cluster.AddSignal(comparisonSignal);

    clusters.push_back(cluster);
  }
  return kOk;
}

void EXOClusteringModule::CullVWires(EXOEventData* ED, const vector< Cluster<EXOUWireSignal> > &uc, vector< Cluster<EXOVWireSignal> > &vc) const
{
  for(size_t i=0; i<vc.size(); i++){
    if(vc[i].Size() > 1){
      continue;
    }
    double time = vc[i].Time();
    bool cull = true;
    for(size_t j=0; j<uc.size(); j++){
      if(abs(time - uc[j].Time()) < fVMatchTime){
        cull = false;
        break;
      }
    }
    if(cull){
      LogEXOMsg("Culling single V-signal because there is no near U-signal",EEDebug);
      ED->Remove(vc[i].signals[0]);
      vc.erase(vc.begin() + i);
      i--;
    }
  }
}

template <typename D>
std::set< std::set<int> > EXOClusteringModule::CreateCombinations(const std::vector< Cluster<D> > &clusters, double matchTime) const
{
  using namespace std;
  set< set<int> > ret;
  size_t nclusters = clusters.size();
  if(nclusters < 2){
    //In this case theres nothing to combine
    return ret;
  }
  EXOPowerSet<int> ps(2);
  for(size_t i=0; i<nclusters-1; i++){
    std::set<int> combination;
    for(size_t j=i+1; j<nclusters; j++){
      if(fabs(clusters[i].Time() - clusters[j].Time()) > matchTime){
        continue;
      }
      if(!OnSameDetectorHalf(clusters[i].signals[0]->fChannel,clusters[j].signals[0]->fChannel)){
        continue;
      }
      combination.insert(i);
      combination.insert(j);
    }
    set< set<int> > powerSet;
    if(combination.size() > 5){
      throw CombinationLimitException();
    }
    else if(combination.size() > 1){
      powerSet = ps.FindPowerSet(combination);
    }
    for(set< set<int> >::iterator it = powerSet.begin(); it != powerSet.end(); it++){
      ret.insert(*it);
    }
  }
  return ret;
}


void EXOClusteringModule::FindScintillationCluster(EXOEventData* ED, vector< Cluster<EXOUWireSignal> >& uclusters) const
{
  //For each ucluster in uclusters find and set the corresponding
  //Scintillation Cluster in ED.
  //If no corresponding Scintillation Cluster can be found, set
  //Cluster::fRefScintCluster to NULL

  size_t nsc = ED->GetNumScintillationClusters();
  for(vector< Cluster<EXOUWireSignal> >::iterator iter = uclusters.begin(); iter != uclusters.end(); iter++){
    if(iter->Size() < 1){
      continue;
    }
    EXOScintillationCluster* bestSc = NULL;
    double bestdt = numeric_limits<double>::max();
    for(size_t i=0; i<nsc; i++){
      EXOScintillationCluster* sc = ED->GetScintillationCluster(i);
      double dt = iter->Time() - sc->fTime;
      EXOMiscUtil::ETPCSide detectorHalf = GetTPCSide(iter->signals[0]->fChannel);
      double maxDriftTime = (CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE) / (detectorHalf == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2);
      //FIXME where do these 3 microseconds come from?
      if ( dt > -3.0*CLHEP::microsecond &&
           (dt < maxDriftTime + 3.0*CLHEP::microsecond  or fNoMaxDriftTime)) {
        dt = fabs(dt);
        if (dt < bestdt) {
          bestdt = dt;
          bestSc = sc;
        }
      }
    }
    iter->fRefScintCluster = bestSc;
  }
}

void EXOClusteringModule::FindUWireInduction(EXOEventData* ED, vector< Cluster<EXOUWireSignal> >& uclusters) const
{
  // Cycle through all the U-wire induction signals
  // If they are on a neighboring channel of the cluster and within the time window, add them to the cluster
  // Note that this can double count signals -- i.e., if they could be added to more than one
  // cluster based on these criteria, they will be

  size_t nUind = ED->GetNumUWireInductionSignals();
  size_t nUsigs = ED->GetNumUWireSignals();
  for(vector< Cluster<EXOUWireSignal> >::iterator iter = uclusters.begin(); iter != uclusters.end(); iter++) {
    if(iter->Size() < 1){
      continue;
    }  

    Double_t clust_time = iter->Time();
    
    // First find min and max channels in the cluster
    Int_t min_chan = 99999;
    Int_t max_chan = -1;
    for(size_t i = 0; i < iter->Size(); i++) {
      if( iter->SignalChannel(i) > max_chan ) max_chan = iter->SignalChannel(i);
      if( iter->SignalChannel(i) < min_chan ) min_chan = iter->SignalChannel(i);
    }

    for(size_t j=0; j<nUind; j++){
      EXOUWireInductionSignal* uind = ED->GetUWireInductionSignal(j);

      Int_t IndChan = EXOReconUtil::kUWireIndOffset -  uind->fChannel;

      bool isUsig = false;
      for(size_t k=0; k<nUsigs; k++){
          EXOUWireSignal* usig = ED->GetUWireSignal(k);
          Double_t dt = (usig->fTime - uind->fTime);
          if(not usig->fIsInduction and usig->fChannel==IndChan and dt < fUIndMatchTime and dt > -fUMatchTime)
          {
              isUsig = true;
              break;
          }

      }

      if (isUsig) continue;
      // Correct channel for offset that distinguishes induction signals
      Double_t dt = clust_time - uind->fTime;

      // make sure the induction signal neigbors the cluster and is within the proper time
      // window.  This will usually be positive (induction leads the signal), but apply a 
      // small buffer equal to fUMatchTime on the negative side
      if( (IndChan == min_chan-1 || IndChan == max_chan+1) and (dt < fUIndMatchTime and dt > -fUMatchTime) ) {
	iter->fRefInductionVector.push_back( uind );
      }
    }

  }
}

EXOAnalysisModule::EventStatus EXOClusteringModule::CheckCathodeSplit(EXOEventData *ED)
{
  //Loops over 3D ChargeClusters, and combines pairs which have been split by the cathode.

  for ( size_t i = 0; i < ED->GetNumChargeClusters(); i++ ) {

    EXOChargeCluster* cluster1 = ED->GetChargeCluster(i);

    // Only look at 3d clusters.
    if( not cluster1->Is3DCluster() ) continue;

    // don't bother with clusters far from the cathode
    if ( fabs(cluster1->fZ) > 1.0*CLHEP::cm ) continue; 

    // don't combine clusters which have already been combined
    if ( cluster1->fDetectorHalf > 1 ) continue;

    // don't bother with clusters with no energy
    if ( cluster1->fCorrectedEnergy < 1.0 ) continue;

    // This should be redundant, but check that there's a scintillation cluster
    if ( cluster1->GetScintillationCluster() == NULL ) continue;

    for ( size_t j = i+1; j < ED->GetNumChargeClusters(); j++ ) {

      // Veto the inner loop if the first cluster has already been combined.
      if ( cluster1->fDetectorHalf > 1 ) continue;
       
      EXOChargeCluster* cluster2 = ED->GetChargeCluster(j);

      // don't bother with clusters that aren't 3D
      if ( cluster2->Is3DCluster() == false ) continue;

      // don't bother with clusters far from the cathode
      if ( fabs(cluster2->fZ) > 1.0*CLHEP::cm ) continue; 

      // don't combine clusters which have already been combined
      if ( cluster2->fDetectorHalf > 1 ) continue;

      // don't bother with clusters that have no energy
      if ( cluster2->fCorrectedEnergy < 1.0 ) continue;

      // This should be redundant, but check that there's a scintillation cluster.
      if ( cluster2->GetScintillationCluster() == NULL ) continue;

      // don't combine clusters on the same side of the cathode
      if ( cluster1->fDetectorHalf == cluster2->fDetectorHalf ) continue;

      // see if they have the same x
      if ( fabs(cluster1->fX - cluster2->fX) > 1.0*CLHEP::cm ) continue;

      // see if they obviously have different y's
      if ( fabs(cluster1->fY - cluster2->fY) > 1.0*CLHEP::cm ) continue;

      // see if they come from the same scintillation cluster (ie. have t0's that are the same)
      if ( cluster1->GetScintillationCluster() != cluster2->GetScintillationCluster() ) continue;

      // Don't combine them if the scintillation algorithms are different
      if ( cluster1->GetScintillationCluster()->GetAlgorithmUsed() != cluster2->GetScintillationCluster()->GetAlgorithmUsed() ) continue;

      // Combine the two clusters; put result into cluster1.
      // Note:  be careful about the order in which values are overwritten. 

      // Take the u, v, x, y positions from the larger cluster.  (Should this be the way?)
      if( cluster1->fCorrectedEnergy < cluster2->fCorrectedEnergy ) {
        cluster1->fU = cluster2->fU;
        cluster1->fV = cluster2->fV;
        cluster1->fX = cluster2->fX;
        cluster1->fY = cluster2->fY;
        cluster1->fDetectorHalf = cluster2->fDetectorHalf + 2;
      }
      else {
        cluster1->fDetectorHalf += 2;
      }

      cluster1->fZ = ( cluster1->fZ + cluster2->fZ ) / 2.0;
      cluster1->fCollectionTime = (cluster1->fCollectionTime + cluster2->fCollectionTime) / 2.0;
      cluster1->fDriftTime = (cluster1->fDriftTime + cluster2->fDriftTime) / 2.0;

      cluster1->fRawEnergy += cluster2->fRawEnergy;
      cluster1->fRawEnergyError = sqrt( pow(cluster1->fRawEnergyError, 2) + pow(cluster2->fRawEnergyError, 2) );
      cluster1->fCorrectedEnergy += cluster2->fCorrectedEnergy;
      cluster1->fCorrectedEnergyError = sqrt( pow(cluster1->fCorrectedEnergyError, 2) + pow(cluster2->fCorrectedEnergyError, 2) );

      cluster1->fAmplitudeInVChannels += cluster2->fAmplitudeInVChannels;
      cluster1->fCorrectedAmplitudeInVChannels += cluster2->fCorrectedAmplitudeInVChannels;

      // Update links between charge and uwire objects.
      // Note that since scint clusters match, it is not necessary to do anything.
      for(size_t k=0; k<cluster2->GetNumUWireSignals(); k++) {
        cluster1->InsertUWireSignal( cluster2->GetUWireSignalAt(k) );
        cluster2->GetUWireSignalAt(k)->InsertChargeCluster(cluster1);
      }
      for(size_t k=0; k<cluster2->GetNumVWireSignals(); k++) {
        cluster1->InsertVWireSignal( cluster2->GetVWireSignalAt(k) );
        cluster2->GetVWireSignalAt(k)->InsertChargeCluster(cluster1);
      }

      // Delete cluster 2.
      // This should propagate and delete all TRefs that link to it.
      ED->Remove( cluster2 );

      if ( fVerbose ) {
  cout<<"Merged two charge clusters into charge cluster "<<i<<endl;
      }

    } 

  } // end loop over charge clusters for combining across the cathode

  return kOk;
}

void EXOClusteringModule::SetDriftVelocity( EXOEventData* ED )
{
  //Sets the drift velocity that is used for this event.
  //The current order of preferences on data we can identify as monte carlo:
  //1) User-set value (command /rec/drift_velocity_mm_per_ns)
  //2) EXODimensions.hh value
  //If we can't identify it as monte carlo:
  //1) User-set value (command /rec/drift_velocity_mm_per_ns)
  //2) Fetch from database.
  //3) EXODimensions.hh value.

  if ( fUserDriftVelocity > 0 ) {
    if ( fDriftStatus != kUserSet ) {
      fDriftVelocityTPC1 =  fUserDriftVelocity;
      fDriftVelocityTPC2 =  fUserDriftVelocity;
      if ( fDriftStatus != kFirstCall ) cout<<"EXOClusteringModule: changing drift velocity mid-job."<<endl;
      cout<<"EXOClusteringModule: using user-set drift velocity = "<<fUserDriftVelocity<<" mm/ns."<<endl;
      fDriftStatus = kUserSet;
    }
    return;
  }

  if ( ED->fEventHeader.fIsMonteCarloEvent ) {
    if ( fDriftStatus != kDefaultMC) {
      fDriftVelocityTPC1 = DRIFT_VELOCITY;
      fDriftVelocityTPC2 = DRIFT_VELOCITY;
      if ( fDriftStatus != kFirstCall ) cout<<"EXOClusteringModule: changing drift velocity mid-job."<<endl;
      cout<<"EXOClusteringModule: running on monte carlo data; using drift velocity = "<<DRIFT_VELOCITY<<" mm/ns from EXODimensions.hh file."<<endl;
      fDriftStatus = kDefaultMC;
    }
    return;
  }

  // It's not MC at this point, and there's no user setting, so query the database.
  if ( fDriftVelocityCalib != NULL ) {
    // is the old calibration still good?
    if( fDriftVelocityCalib->isValid( 
           EXOTimestamp(ED->fEventHeader.fTriggerSeconds, 
                        static_cast<int>(ED->fEventHeader.fTriggerMicroSeconds*CLHEP::microsecond)) ) ) {
      if( fDriftStatus != kDatabase ) {
        fDriftVelocityTPC1 = fDriftVelocityCalib->get_drift_velocity_TPC1();
        fDriftVelocityTPC2 = fDriftVelocityCalib->get_drift_velocity_TPC2();
        cout<<"EXOClusteringModule: changing drift velocity mid-job."<<endl;
        cout<<"Reusing old database values that are still valid."<<endl;
        fDriftStatus = kDatabase;
      }
      return;
    }
  }

  if ( fDriftStatus != kDefaultQueryFail) {
    cout<<"EXOClusteringModule: querying calibManager for drift velocity."<<endl;
    if ( fDriftStatus == kDatabase ) cout<<"The old timestamp was no longer valid."<<endl;
    fDriftVelocityCalib = GetCalibrationFor(EXODriftVelocityCalib, 
                                              EXODriftVelocityHandler, 
                                              "vanilla", 
                                              ED->fEventHeader);
    if(fDriftVelocityCalib) {
      fDriftVelocityTPC1 = fDriftVelocityCalib->get_drift_velocity_TPC1();
      fDriftVelocityTPC2 = fDriftVelocityCalib->get_drift_velocity_TPC2();
      cout<<"EXOClusteringModule: successfully retrieved drift velocity from database."<<endl;
      cout<<"drift_velocity_TPC1 = "<<fDriftVelocityCalib->get_drift_velocity_TPC1()<<endl;
      cout<<"drift_velocity_TPC2 = "<<fDriftVelocityCalib->get_drift_velocity_TPC2()<<endl;
      fDriftStatus = kDatabase;
      if (fCollectionStatus == kDatabase){ fCollectionStatus = kDBUpdate;}
      return;
    }
  }

  // the database read failed
  if ( fDriftStatus != kDefaultQueryFail ) {
    fDriftVelocityTPC1 = DRIFT_VELOCITY;
    fDriftVelocityTPC2 = DRIFT_VELOCITY;
    cout<<"EXOClusteringModule: database query was unsuccessful; henceforth using default drift velocity = "<<DRIFT_VELOCITY<<" mm/ns from EXODimensions.hh."<<endl;
    fDriftStatus = kDefaultQueryFail;
  }
  return;
}

void EXOClusteringModule::SetCollectionTime( EXOEventData* ED )
{
  //Sets the collection time used for this event.
  //The current order of preferences : 
  //1) User-set value (command /cluster/collection_time)
  //2) if not identified as monte carlo :
  //      fetch from database
  //3) EXODimensions.hh value

  if ( fUserCollectionTime > 0 ) { // any negative skips eg default of -1.
    if ( fCollectionStatus != kUserSet ) {
      fCollectionTimeTPC1 =  fUserCollectionTime;
      fCollectionTimeTPC2 =  fUserCollectionTime;
      if ( fCollectionStatus != kFirstCall ) cout<<"EXOClusteringModule: changing collection time mid-job."<<endl;
      cout<<"EXOClusteringModule: using user-set collection time = "<<fUserCollectionTime<<" ns."<<endl;
      fCollectionStatus = kUserSet;
    }
    return;
  }

  if ( ED->fEventHeader.fIsMonteCarloEvent ) {
    if ( fCollectionStatus != kDefaultMC) {
      fCollectionTimeTPC1 = COLLECTION_TIME;
      fCollectionTimeTPC2 = COLLECTION_TIME;
      if ( fCollectionStatus != kFirstCall ) cout<<"EXOClusteringModule: changing collection time mid-job."<<endl;
      cout<<"EXOClusteringModule: running on monte carlo data; using collection time = "<<COLLECTION_TIME <<" ns from EXODimensions.hh file"<<endl;
      fCollectionStatus = kDefaultMC;
    }
    return;
  }

  // It's not MC at this point, and there's no user setting, so query the database.
  if ( fDriftVelocityCalib != NULL ) { // the drift velocity calib also holds the collection time,
    if (fCollectionStatus == kDBUpdate or fCollectionStatus == kFirstCall){ // setdriftvelocity updated the database to a new valid one -- allows db sharing while keeping the option of just looking up the collection status and setting the drift velocity manually
        fCollectionTimeTPC1 = fDriftVelocityCalib->get_collection_time_TPC1();
        fCollectionTimeTPC2 = fDriftVelocityCalib->get_collection_time_TPC2();
	fCollectionStatus = kDatabase;
    }
    // if we get here we aren't using db in setdriftvelocity so we have to ask all the questions, first -- is the old calibration still valid?
    if( fDriftVelocityCalib->isValid( 
           EXOTimestamp(ED->fEventHeader.fTriggerSeconds, 
                        static_cast<int>(ED->fEventHeader.fTriggerMicroSeconds*CLHEP::microsecond)) ) ) {
      if( fCollectionStatus != kDatabase ) {
        fCollectionTimeTPC1 = fDriftVelocityCalib->get_collection_time_TPC1();
        fCollectionTimeTPC2 = fDriftVelocityCalib->get_collection_time_TPC2();
	cout<<"EXOClusteringModule: changing collection time mid-job."<<endl;
	cout<<"Reusing old database values that are still valid."<<endl;
        fCollectionStatus = kDatabase;
      }
      return;
    }
  }

  if ( fCollectionStatus != kDefaultQueryFail) {
    cout<<"EXOClusteringModule: querying calibManager for collection time."<<endl;
    if ( fCollectionStatus == kDatabase ) cout<<"The old timestamp was no longer valid."<<endl;
    fDriftVelocityCalib = GetCalibrationFor(EXODriftVelocityCalib, 
					    EXODriftVelocityHandler, 
					    "vanilla", // hard coded flavor
					    ED->fEventHeader);
    if(fDriftVelocityCalib) {
      fCollectionTimeTPC1 = fDriftVelocityCalib->get_collection_time_TPC1();
      fCollectionTimeTPC2 = fDriftVelocityCalib->get_collection_time_TPC2();
      cout<<"EXOClusteringModule: successfully retrieved collection time from database."<<endl;
      cout<<"collection_time_TPC1 = "<<fDriftVelocityCalib->get_collection_time_TPC1()<<endl;
      cout<<"collection_time_TPC2 = "<<fDriftVelocityCalib->get_collection_time_TPC2()<<endl;
      fCollectionStatus = kDatabase;
      return;
    }
  }

  // the database read failed :(
  if ( fCollectionStatus != kDefaultQueryFail ) {
    fCollectionTimeTPC1 = COLLECTION_TIME;
    fCollectionTimeTPC2 = COLLECTION_TIME;
    cout<<"EXOClusteringModule: database query was unsuccessful; henceforth using default collectiontime = "<<COLLECTION_TIME<<" ns from EXODimensions.hh."<<endl;
    fCollectionStatus = kDefaultQueryFail;
  }
  return;
}


bool EXOClusteringModule::APDSignalSorter::operator()(const EXOAPDSignal* lhs, const EXOAPDSignal* rhs) const
{
  return lhs->fTime < rhs->fTime;
}

bool EXOClusteringModule::USignalEnergySorter::operator()(const EXOUWireSignal* lhs, const EXOUWireSignal* rhs) const
{
  return lhs->fCorrectedEnergy > rhs->fCorrectedEnergy;
}

bool EXOClusteringModule::USignalChannelSorter::operator()(const EXOUWireSignal* lhs, const EXOUWireSignal* rhs) const
{
  return lhs->fChannel > rhs->fChannel;
}

bool EXOClusteringModule::VSignalEnergySorter::operator()(const EXOVWireSignal* lhs, const EXOVWireSignal* rhs) const
{
  return lhs->fCorrectedMagnitude > rhs->fCorrectedMagnitude;
}

bool EXOClusteringModule::VSignalChannelSorter::operator()(const EXOVWireSignal* lhs, const EXOVWireSignal* rhs) const
{
  return lhs->fChannel > rhs->fChannel;
}

double EXOClusteringModule::totalUVNLPdf(const Cluster<EXOUWireSignal> &uc, const Cluster<EXOVWireSignal> &vc) const
{
  //Calculate the total negative log pdf value for the combination of uc with vc.
  //Calculate the position pdf value. We need to take into account that uc or vc might be a combined cluster.
  //If that's the case we calculate the mean of the pdf values of the combined cluster's childs.
  double positionPart = 0;
  if(uc.IsCombined()){
    //means vc is not combined
    for(size_t i=0; i<uc.NumChilds(); i++){
      Cluster<EXOUWireSignal> subCluster = uc.GetChild(i);
      positionPart += positionNLPdf(subCluster.Position(),vc.Position());
    }
    positionPart /= uc.NumChilds();
  }
  else if(vc.IsCombined()){
    for(size_t i=0; i<vc.NumChilds(); i++){
      Cluster<EXOVWireSignal> subCluster = vc.GetChild(i);
      positionPart += positionNLPdf(uc.Position(),subCluster.Position());
    }
    positionPart /= vc.NumChilds();
  }
  else{
    //means neither uc nor vc is combined
    positionPart = positionNLPdf(uc.Position(),vc.Position());
  }
  

  const double& driftVel = GetTPCSide(uc.signals[0]->fChannel) == EXOMiscUtil::kNorth ? fDriftVelocityTPC1 : fDriftVelocityTPC2;
  const double& collTime = GetTPCSide(uc.signals[0]->fChannel) == EXOMiscUtil::kNorth ? fCollectionTimeTPC1 : fCollectionTimeTPC2;
  double Z = uc.CalculateZwithDriftVelocity(driftVel,collTime);
  double energyPart = energyNLPdf(uc.Energy(),vc.Energy(),Z);
  double timePart = timeNLPdf(uc.Time(), vc.Time(),Z,uc.IsCombined() or vc.IsCombined());

  if(fVerbose > 2){
    cout << "totalUVNLPdf position/energy/time parts = " << positionPart << "/" << energyPart << "/" << timePart << endl;
  }

  if(fWriteDebugTree){
    fDebugData->fZ.push_back(Z);
    fDebugData->fEnergy.push_back(uc.Energy());
    fDebugData->fVEnergy.push_back(vc.Energy());
    fDebugData->fTime.push_back(uc.Time());
    fDebugData->fTimeDiff.push_back(uc.Time() - vc.Time());
    fDebugData->fNLPosition.push_back(positionPart);
    fDebugData->fNLEnergy.push_back(energyPart);
    fDebugData->fNLTime.push_back(timePart);
  }

  return positionPart + energyPart + timePart;
}

double EXOClusteringModule::energyNLPdf(double Uamplitude, double Vamplitude, double Z)  const
{
  //Return negative log of u-v-energy matching pdf

  if(fabs(Z) > 160 and (not fUseNewEnergyPDF)){
    //Means we're near the anodes. Don't use the U-V-energy pdf.
    //Use MAD of the standard normal distribution instead as a 'reasonable likelihood value' for one pdf
    //i.e. return (MAD)^2 / 2
    return (double(fReasonableCost) / 1000.) / 3.;
  }
  else if(fabs(Z) > 185){
    return (double(fReasonableCost) / 1000.) / 3.;
  }

  if(Uamplitude <= 0.){
    //some reasonably high number (not too high)
    return 1e7;
  }
  //FIXME Hardcoded parameters. Make them class variables (and settable via TalkTo)
  //The parameters come from a fit to a U-V energy histogram filled with real data.
  const double scale = 0.2378;
  const double offset = -30.79;
  double v = 0.;
  if(Uamplitude > -offset/scale){
    v = offset + Uamplitude*scale; 
  }

  //if (fabs(Z) > 100 and fUseNewEnergyPDF){
  if (fUseNewEnergyPDF){
      //Apply a Z-Correction to the Energy PDF betwen 100 and 185
     double p0   =   0.80299698;
     double p1   =   21.93337053;
     double zmax =   205.9221048;
     
     //double corr = -1./(1. + 0.53*TMath::Exp(-(fabs(Z) - 198.5)/32.9));
     double corr = -1./(1. + p0*TMath::Exp(-(fabs(Z) - zmax)/p1));
     
     v = v*(corr+1);
  }

  //If v == Vamplitude, the U- and V- cluster match perfectly
  const double sigmaLinearScale = 0.0101;
  const double sigmaSqrtScale = 0.892;
  double sigma = 20.22;  //sigma is constant for small energies
  if(Uamplitude > 350){
    sigma = Uamplitude*sigmaLinearScale + sqrt(Uamplitude)*sigmaSqrtScale;
  }
  double deviation = (v-Vamplitude)/sigma;
  return deviation*deviation/2.;
}

double EXOClusteringModule::positionNLPdf(double uPos, double vPos) const
{
  //Return negative log of u-v-position matching pdf
  //This essentially takes into account the distance of (uPos,vPos) to
  //the hexagon defined by crossing u- and v-wires.
  //FIXME Hardcoded parameters. Make them class variables (and settable via TalkTo)
  const double sigma = CHANNEL_WIDTH/2.;
  EXOWireCrossing& crossing = EXOWireCrossing::GetInstance();
  EXOCoordinates coord(EXOMiscUtil::kUVCoordinates,uPos,vPos,0.0,0.0);
  double dist = crossing.GetDistanceFromHexagon(coord); //Is zero if inside the hexagon.
  double a = dist/sigma;
  return a*a/2; 
}

double EXOClusteringModule::timeNLPdf(double Utime, double Vtime, double Z, bool combined) const
{
  //Return negative log of u-v-time matching pdf
  //The pdf also depends on the z-position.
  //FIXME Hardcoded parameters. Make them class variables (and settable via TalkTo)
  double sigma = 1.0 * CLHEP::microsecond;
  //If either the U- or V-cluster is combined, use a less strict sigma.
  if(combined){
    sigma = 1.0 * CLHEP::microsecond;
  }
  double offset = 0.0;
  //For z-positions near the anodes, the V-signals seem to fit to some later time
  //Take that into account using a z-dependent offset for the mean value. 
  //The numbers come from a fit of a cubic polynomial to data.
  //Remember, that if no scintillation cluster was found for a charge cluster, fabs(Z) == 999
  if(fabs(Z) > 998){
    offset = 0.;
  }
  else if(fabs(Z) > 194.1){
    offset = 3.0*CLHEP::microsecond;
  }
  else if(fabs(Z) > 185.2){
    double x = fabs(Z) - 190.;
    offset = (2.728 + 0.5466*x - 0.06538*x*x - 0.01275*x*x*x)*CLHEP::microsecond;
  }
  double a = (Utime - Vtime - offset)/sigma;
  return a*a/2; 
}
