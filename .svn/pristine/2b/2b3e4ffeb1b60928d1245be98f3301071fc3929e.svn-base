//______________________________________________________________________________
// EXOCoincidences
// 
// This class is intended to supercede the coincidences work in EXOFitting, and
// provide a unified interface to management of coincidences.  Time cuts can be
// adjusted, and cuts can be turned on and off; however, the defaults are
// intended to match what we use in analysis.  Note that reprocessing is not
// needed to change the vetoing defaults; you just have to change this class.
//______________________________________________________________________________


#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXODelegates.hh"
#include "EXOUtilities/EXOVetoEventHeader.hh"
#include "EXOUtilities/EXOGlitchRecord.hh"
#include "EXOUtilities/EXOCoincidences.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TVirtualIndex.h"
#include "TList.h"
#include "TChainElement.h"
#include "TError.h"
#include "TTimeStamp.h"
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TRefTable.h"
#include "TEventList.h"
#include <cstring>
#include <stack>
#include <utility>
#include <algorithm>
#include <vector>
#include <sstream>
#include <limits>

class TClass;

// At destruction, class ResetTRefTable automatically reverts the TRefTable
// back to the state it had at construction.  This is used to ensure that the
// state of TRefTable is the same when it enters a EXOCoincidences function as
// when it exits.  The problem is that this global is reset every time
// TTree::GetEntry is called on a tree with a BranchRef (which standard exo
// trees have).  This can then make the global invalid for future calls.   
//
// - IMPORTANT - A variable of this type must be instantiated in all public
// EXOCoincidences functions that call (or call through to funcitons that call)
// fEventChain.GetEntry.  If you have a doubt and are implementing a new public
// function, simply add a variable as it will be harmless if it is *not*
// necessary. - M. Marino
class ResetTRefTable {
  public:
    ResetTRefTable() : fCurrentTable(TRefTable::GetRefTable()) {} 
    ~ResetTRefTable() 
      { TRefTable::SetRefTable(fCurrentTable); }
  private:
    TRefTable* fCurrentTable;
}; 

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed(const EXOEventData& event) const
{
  // Return true if this event should be vetoed, given the settings for what counts as a coincidence.
  bool veto = false;
  if(fUseBlankedOutTime) veto = veto or IsVetoed_BlankedOutTime(event);
  Double_t dummy1, dummy2,dummy3,dummy4;
  Int_t dummyi1;
  if(fUseVetoPanel) veto = veto or IsVetoed_VetoPanel(event,dummy1,dummy2,dummyi1);
  if(fUseMuonTrack) veto = veto or IsVetoed_MuonTrack(event,dummy3,dummy4);
  if(fUseTpcEvent) veto = veto or IsVetoed_TpcEvent(event);
  return veto;
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_BlankedOutTime(const EXOEventData& event) const
{
  // If we don't know what was going on (run beginning or end, paused run, bad environment)
  // then we need to assume the worst and veto.
  // Note:  this function fails if we permit negative event numbers.  But this is assumed other places, too.
  return IsVetoed_RunBoundary(event) or IsVetoed_BadEnvironment(event);
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_RunBoundary(const EXOEventData& event) const
{
  /*
      Beginning of run, end of run
      Pause/resume
      Note that we're not here to check the integrity of data files --
      we assume that if there is no begin/end record, none was expected,
      and that events never occur between a pause and a resume.
  */

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  // If we haven't been given an event tree (or if it's empty), then a precondition is violated.
  if(fEventChain.GetEntries() == 0) {
    LogEXOMsg("Queried event does not exist in the datasets I've been given.", EEAlert);
  }

  // Go ahead and get the time in ns of the event in question.
  const Long64_t eventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                                  event.fEventHeader.fTriggerMicroSeconds);

  if(fControlRecordListCache.count(event.fRunNumber) > 0) {
    const EXOControlRecordList& controlList = fControlRecordListCache.at(event.fRunNumber);

    if(fBlankedOut_PastTime >= 0) {
      // Did the run begin too recently?
      const EXOBeginRecord* begin = controlList.GetPreviousRecord<EXOBeginRecord>(event.fRunNumber, event.fEventNumber);
      if(begin and begin->GetRunNumber() == event.fRunNumber) {
        Long64_t beginTime = begin->GetTimestamp();
        if(eventTime - beginTime <= fBlankedOut_PastTime) return true;
      }
      // What about if a pause/resume happened recently?
      const EXOResumeRecord* resume = controlList.GetPreviousRecord<EXOResumeRecord>(event.fRunNumber, event.fEventNumber);
      if(resume and resume->GetRunNumber() == event.fRunNumber) {
        Long64_t resumeTime = resume->GetTimestamp();
        if(eventTime - resumeTime <= fBlankedOut_PastTime) return true;
      }
    }

    if(fBlankedOut_FutureTime >= 0) {
      // Is the run about to end?
      const EXOEndRecord* end = controlList.GetNextRecord<EXOEndRecord>(event.fRunNumber, event.fEventNumber);
      if(end and end->GetRunNumber() == event.fRunNumber) {
        Long64_t endTime = end->GetTimestamp();
        if(endTime - eventTime <= fBlankedOut_FutureTime) return true;
      }
      // Or are we about to pause the run?
      const EXOPauseRecord* pause = controlList.GetNextRecord<EXOPauseRecord>(event.fRunNumber, event.fEventNumber);
      if(pause and pause->GetRunNumber() == event.fRunNumber) {
        Long64_t pauseTime = pause->GetTimestamp();
        if(pauseTime - eventTime <= fBlankedOut_FutureTime) return true;
      }
    }
  } // End checks -- only done if control records exist.  Else, OK to skip!
  return false;
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_BadEnvironment(const EXOEventData& event) const
{
  /*
      Bad environment time?
      This currently includes the clean room siren and "bad times" as identified by data quality.
      We add a buffer around those windows just as though the run had temporarily ended during that time,
      since the data may be meaningless during that window.
      Note that this could be made more efficient if we assume more about the structure of the database
      (eg. are there any overlaps in the time intervals?), but this is fast enough
      and the simplest way to code it.
  */
  if(not fHasFetchedBadEnvironmentTimes) FillBadEnvironmentTimes();
  const Long64_t eventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                                  event.fEventHeader.fTriggerMicroSeconds);
  for(std::vector<StartStopTimes>::iterator it = fBadEnvironmentTimes.begin();
      it != fBadEnvironmentTimes.end();
      it++) {
    // If the event falls during this bad time, veto it.
    // Remember to also include the buffer time around it, fBlankedOut_FutureTime and PastTime.
    // (If either of those are negative, that means no buffer should be applied -- use zero.)
    if(it->fStart - std::max(fBlankedOut_FutureTime, Long64_t(0)) <= eventTime and
       eventTime <= it->fStop + std::max(fBlankedOut_PastTime, Long64_t(0))) return true;
  }

  return false;
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_VetoPanel(const EXOEventData& event,Double_t &veto_minus, Double_t &veto_plus, Int_t &veto_multiplicity) const
{
  // Scan through veto tree (if it has been loaded) to find the veto times immediately before and after this event.

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  // If we haven't been given a veto tree (or if it's empty), then we can't veto -- that's OK :)
  if(fVetoChain.GetEntries() == 0) return false;

  // Make sure there's an appropriate index; then get it.
  const TVirtualIndex& index = GetOrBuildIndex(fVetoChain, "fVetoUTime", "fVetoMuTime");

  // Post-condition:  fVetoChain->GetEntry(bestIndex) gets the veto before or coincident with the event trigger time.
  const Long64_t bestIndex = GetEntryNumberWithBestIndex(index,
                                                         event.fEventHeader.fTriggerSeconds,
                                                         event.fEventHeader.fTriggerMicroSeconds);

  // Go ahead and get the time in ns of the event in question.
  const Long64_t eventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                                  event.fEventHeader.fTriggerMicroSeconds);

  // Test for coincidence in the past, provided fVetoPanel_PastTime is non-negative.
  if(fVetoPanel_PastTime >= 0 and bestIndex >= 0) {
    Int_t ret = fVetoChain.GetEntry(bestIndex);
    if(ret <= 0) LogEXOMsg("GetEntry failed; terminating.", EEAlert);
    Long64_t vetoTime = ConvertTimeToNanoSec(fVetoRecord->fVetoUTime, fVetoRecord->fVetoMuTime);
    veto_plus = (eventTime - vetoTime)/1e3;

    // Let's also record the number of hit veto panels!
    // We need to count only even bits, as odd bits are junk data (probably zero, but... better to be safe)
    TBits vetobits = fVetoRecord->fVetoPanelHit;
    veto_multiplicity = 0;
    for (UInt_t ivbit = 0; ivbit < 29; ++ivbit) {
      if (vetobits.TestBitNumber(2*ivbit)) {
        veto_multiplicity++;
      }
    }
    if(eventTime - vetoTime <= fVetoPanel_PastTime) return true;
      
  }

  // Test for coincidence in the future, provided fVetoPanel_FutureTime is non-negative.
  if(fVetoPanel_FutureTime >= 0 and bestIndex + 1 < fVetoChain.GetEntries()) {
    Int_t ret = fVetoChain.GetEntry(bestIndex+1);
    if(ret <= 0) LogEXOMsg("GetEntry failed; terminating.", EEAlert);
    Long64_t vetoTime = ConvertTimeToNanoSec(fVetoRecord->fVetoUTime, fVetoRecord->fVetoMuTime);
    veto_minus = (vetoTime - eventTime)/1e3;
    if(vetoTime - eventTime <= fVetoPanel_FutureTime) return true;
  }

  return false;
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_TpcEvent(const EXOEventData& event) const
{
  // Scan backwards and forwards, looking for a coincident physics event.
  // Exactly what counts is TBD -- for now, any physics trigger (not solicited) counts.

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  Long64_t EventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                            event.fEventHeader.fTriggerMicroSeconds);

  // If fTpcEvent_PastTime is non-negative, look for events that might cause a veto.
  if(fTpcEvent_PastTime >= 0) {
    std::vector<Long64_t>::const_iterator it = std::lower_bound(fEventsYieldingTpcVeto.begin(),
                                                          fEventsYieldingTpcVeto.end(),
                                                          EventTime);
    if(it != fEventsYieldingTpcVeto.begin() and EventTime - *(--it) < fTpcEvent_PastTime) return true;
  }

  // If fTpcEvent_FutureTime is non-negative, look for events that might cause a veto.
  if(fTpcEvent_FutureTime >= 0) {
    std::vector<Long64_t>::const_iterator it = std::upper_bound(fEventsYieldingTpcVeto.begin(),
                                                          fEventsYieldingTpcVeto.end(),
                                                          EventTime);
    if(it != fEventsYieldingTpcVeto.end() and *it - EventTime < fTpcEvent_FutureTime) return true;
  }

  return false;
}

//______________________________________________________________________________
Long64_t EXOCoincidences::Time_to_TpcEvent(const EXOEventData& event, const int returnas ) const
{
  // Scan backwards and forwards, looking for a coincident physics event.
  // returnas min (0) of  next (1) and  prior (-1) time til other event

  ResetTRefTable restRef; // Ensure that TRefTable will get reset on return

  Long64_t EventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                            event.fEventHeader.fTriggerMicroSeconds);
  Long64_t TimePre= 0, TimePost = 0 ;

  if (returnas <= 0 ) {
    std::vector<Long64_t>::const_iterator it =
      std::lower_bound(fEventsYieldingTpcVeto.begin(),
		       fEventsYieldingTpcVeto.end(),
		       EventTime);
    TimePre = (it != fEventsYieldingTpcVeto.begin()) ? EventTime - *(--it) : fBlankedOut_PastTime ;
  }

  if (returnas >= 0 ) {
    std::vector<Long64_t>::const_iterator it2 = 
      std::upper_bound(fEventsYieldingTpcVeto.begin(),
		       fEventsYieldingTpcVeto.end(),
		       EventTime);
    TimePost = (it2 != fEventsYieldingTpcVeto.end()) ? *it2 - EventTime  : fBlankedOut_FutureTime ;
  }

  if      (returnas < 0 ) return TimePre ; 
  else if (returnas > 0 ) return TimePost;
  
  return std::min(TimePre,TimePost);
}

//______________________________________________________________________________
bool EXOCoincidences::IsVetoed_MuonTrack(const EXOEventData& event, Double_t &minus_muon,Double_t &plus_muon) const
{
  // Scan backwards and forwards, looking for a coincident physics event.
  // Exactly what counts is TBD -- for now, any physics trigger (not solicited) counts.

  // A muon track event should veto itself.

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  if(fMuonTrack_PastTime >= 0 or fMuonTrack_FutureTime >= 0) {
    if(CanYieldMuonTrackVeto(event)) return true;
  }
  bool after_muon = false;	
  if(fEventsYieldingMuonVeto.empty()) return false;
  Long64_t EventTime = ConvertTimeToNanoSec(event.fEventHeader.fTriggerSeconds,
                                            event.fEventHeader.fTriggerMicroSeconds);
  
  std::vector<Long64_t>::const_iterator itu = std::upper_bound(fEventsYieldingMuonVeto.begin(),
							       fEventsYieldingMuonVeto.end(),
							       EventTime);
  
  std::vector<Long64_t>::const_iterator itl = std::lower_bound(fEventsYieldingMuonVeto.begin(),
							       fEventsYieldingMuonVeto.end(),
							       EventTime);
  
  if(itu != fEventsYieldingMuonVeto.begin()){
    plus_muon = (EventTime - *(--itu))/1e3;
    after_muon = true;
  }
  
  if(itl != fEventsYieldingMuonVeto.end()){
    minus_muon = (*itl - EventTime)/1e3;
  }
  // If fMuonTrack_PastTime is non-negative, look for events that might cause a veto.
  if(fMuonTrack_PastTime >= 0) {
    if(EventTime - *itu < fMuonTrack_PastTime and after_muon) return true;
  }
  
  // If fMuonTrack_FutureTime is non-negative, look for events that might cause a veto.
  if(fMuonTrack_FutureTime >= 0) {
    
    if(itl != fEventsYieldingMuonVeto.end() and *itl - EventTime < fMuonTrack_FutureTime) return true;
  }
  
  return false;
}

//______________________________________________________________________________
void EXOCoincidences::Load(TChain& EventChain)
{
  // Read the list of files in this chain, and add them to our TChains.
  // This function should not be called more than once on a given EXOCoincidences object.

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  if(std::strcmp(EventChain.GetName(), "tree")) {
    LogEXOMsg("This function requires the event chain (\"tree\") to be loaded; try again.", EEError);
    return;
  }
  assert(fEventChain.GetNtrees() == 0); // We can only load into an EXOCoincidences object once.
  fEventChain.Add(&EventChain);
  

  // Extract the list of events which can induce a veto.
  ExtractVetoingEvents();

  // We have to use fEventChain to identify which of the files contain the last information about the runs.
  // We simultaneously check that the run numbers are ordered -- not a complete check, but it's cheap here.
  Long64_t* TreeOffsets = fEventChain.GetTreeOffset();
  TEventList el;
  for(Int_t i = fEventChain.GetNtrees() - 1; i >= 0; i--) el.Enter(TreeOffsets[i]); 

  // Set the event list and grab the run numbers
  fEventChain.SetEventList(&el);
  fEventChain.Draw("fRunNumber:Entry$", "", "goff");

  // Fill a map which is ( key : value ) : (entry number : run Number)
  std::map<Long64_t, Int_t> RunNumberMap;
  for (Int_t i=0;i<el.GetN();i++) {
    RunNumberMap[Long64_t(fEventChain.GetV2()[i])] = Int_t(fEventChain.GetV1()[i]);
  }

  // Reset the event list
  fEventChain.SetEventList(NULL);

  std::stack<std::pair<Int_t, std::string> > OneFilePerRun;
  for(Int_t i = fEventChain.GetNtrees() - 1; i >= 0; i--) {
    // Load tree i
    assert(RunNumberMap.find(TreeOffsets[i]) != RunNumberMap.end());
    Int_t runNumber = RunNumberMap[TreeOffsets[i]];
    if(not OneFilePerRun.empty()) {
      if(OneFilePerRun.top().first < runNumber) LogEXOMsg("Error:  the chain you gave me had files out-of-order.", EEAlert);
      if(OneFilePerRun.top().first == runNumber) continue; // There was a later file with the same run number.
    }

    // Else, add the run/file to the queue.
    TChainElement* elem = dynamic_cast<TChainElement*>(fEventChain.GetListOfFiles()->At(i));
    OneFilePerRun.push(std::make_pair(runNumber, elem->GetTitle()));
  }

  while(not OneFilePerRun.empty()) {
    fVetoChain.AddFile(OneFilePerRun.top().second.c_str());
    fGlitchChain.AddFile(OneFilePerRun.top().second.c_str());
    OneFilePerRun.pop();
  }

  if(fVetoChain.GetEntries()) fVetoChain.SetBranchAddress("VetoBranch", &fVetoRecord);
  if(fGlitchChain.GetEntries()) fGlitchChain.SetBranchAddress("GlitchBranch", &fGlitchRecord);

  // Fill the control record cache.
  std::vector<TClass*> FilterRecords; // Only grab the control records we want.
  FilterRecords.push_back(EXOBeginRecord::Class());
  FilterRecords.push_back(EXOEndRecord::Class());
  FilterRecords.push_back(EXOPauseRecord::Class());
  FilterRecords.push_back(EXOResumeRecord::Class());
  fControlRecordListCache = EXOMiscUtil::ExtractControlRecords(fEventChain, FilterRecords);
}

//______________________________________________________________________________
void EXOCoincidences::Load(std::string FileName)
{
  // Use the trees from only one file.
  // Note:  DO NOT use this function multiple times;
  // instead, you can add multiple trees with Load(TChain& chain).

  // Ensure that TRefTable will get reset on return from this function.
  ResetTRefTable restRef;

  assert(fEventChain.GetNtrees() == 0); // We can only load into an EXOCoincidences object once.
  Int_t ret;
  ret = fEventChain.AddFile(FileName.c_str());
  if(ret != 1) LogEXOMsg("Failed to add " + FileName + " to the event chain; aborting.", EEAlert);
  

  // Extract the list of events which can induce a veto.
  ExtractVetoingEvents();

  // This is the only file, so naturally we will read in veto and glitch data from it.
  // Note that for long runs, this means that this function isn't quite right on the boundary between files!
  ret = fVetoChain.AddFile(FileName.c_str());
  if(ret != 1) LogEXOMsg("Failed to add " + FileName + " to the veto chain; aborting.", EEAlert);
  if(fVetoChain.GetEntries()) fVetoChain.SetBranchAddress("VetoBranch", &fVetoRecord);
  ret = fGlitchChain.AddFile(FileName.c_str());
  if(ret != 1) LogEXOMsg("Failed to add " + FileName + " to the glitch chain; aborting.", EEAlert);
  if(fGlitchChain.GetEntries()) fGlitchChain.SetBranchAddress("GlitchBranch", &fGlitchRecord);

  // Fill the control record cache.
  std::vector<TClass*> FilterRecords; // Only grab the control records we want.
  FilterRecords.push_back(EXOBeginRecord::Class());
  FilterRecords.push_back(EXOEndRecord::Class());
  FilterRecords.push_back(EXOPauseRecord::Class());
  FilterRecords.push_back(EXOResumeRecord::Class());
  fControlRecordListCache = EXOMiscUtil::ExtractControlRecords(fEventChain, FilterRecords);
}

//______________________________________________________________________________
EXOCoincidences::EXOCoincidences()
: fEventChain("tree"),
  fVetoChain("veto"),
  fVetoRecord(NULL),
  fGlitchChain("glitch"),
  fGlitchRecord(NULL),
  fHasFetchedBadEnvironmentTimes(false),
  fRegisterDeligate(NULL)
{

  // veto panel coincidence defaults
  fUseVetoPanel = true;
  fVetoPanel_PastTime = Long64_t(25*CLHEP::millisecond);
  fVetoPanel_FutureTime = Long64_t(1*CLHEP::millisecond);

  // tpc muon track defaults
  fUseMuonTrack = true;
  fMuonTrack_PastTime = Long64_t(60*CLHEP::second);
  fMuonTrack_FutureTime = Long64_t(1*CLHEP::millisecond);

  // tpc-tpc coincidence defaults updated 12/6/17 relaxed from 1s to 100ms
  // Make sure these are symmetric, unless you have a very good reason!
  fUseTpcEvent = true;
  fTpcEvent_PastTime   = Long64_t(0.1*CLHEP::second);
  fTpcEvent_FutureTime = Long64_t(0.1*CLHEP::second);

  // run boundary default cuts -- the most aggressive cuts we have.
  // +1 ns for safety.
  fUseBlankedOutTime = true;
  fBlankedOut_PastTime = std::max(std::max(Long64_t(0), fVetoPanel_PastTime),
                         std::max(fMuonTrack_PastTime, fTpcEvent_PastTime)) + 1;
  fBlankedOut_FutureTime = std::max(std::max(Long64_t(0), fVetoPanel_FutureTime),
                                    std::max(fMuonTrack_FutureTime, fTpcEvent_FutureTime)) + 1;

  // fridge cut option for Phase II 
  fUseFridgeCut = true;
  fWithFridgeStart = "'2016-01-01 00:00:00'";
  

}

bool EXOCoincidences::SetTPCEventTimeCut(double pastsec, double futuresec){
  // tpc-tpc coincidence setting, only passing one sets both symmetrically
  // which is what you want to do -- unless you have a very good reason!

  //adjust first to properly set blanked out 
  if (futuresec < 0 ) futuresec = pastsec ; 
  fTpcEvent_PastTime   = Long64_t(  pastsec*CLHEP::second);
  fTpcEvent_FutureTime = Long64_t(futuresec*CLHEP::second);

  // update run boundary default cuts -- the most aggressive cuts we have.
  // +1 ns for safety.
  fBlankedOut_PastTime = std::max(std::max(Long64_t(0), fVetoPanel_PastTime),
                         std::max(fMuonTrack_PastTime, fTpcEvent_PastTime)) + 1;
  fBlankedOut_FutureTime = std::max(std::max(Long64_t(0), fVetoPanel_FutureTime),
                                    std::max(fMuonTrack_FutureTime, fTpcEvent_FutureTime)) + 1;

  if ( pastsec < 0 ) { // then decide to use cut or not.
    fUseTpcEvent = false;  
    return false;
  }
  fUseTpcEvent = true;
  return true;
}


//______________________________________________________________________________
Long64_t EXOCoincidences::ConvertTimeToNanoSec(Long64_t sec, Long64_t microsec)
{
  // Given a time in seconds and microseconds, convert it to nanoseconds.
  // This works for times up to 292 years, at which point the return value is not big enough.
  // (Make sure you are calling this function with sufficiently large values, though!
  //  Part of the benefit of this function is it automatically converts everything to Long64_t before doing math.)
  return sec*Long64_t(CLHEP::second) + microsec*Long64_t(CLHEP::microsecond);
}

//______________________________________________________________________________
const TVirtualIndex& EXOCoincidences::GetOrBuildIndex(TChain& chain, std::string major, std::string minor)
{
  // Check that chain has an index, and that it matches (major, minor).
  // If not, build it (and terminate if that fails).
  // (Do not pass an empty chain to this function -- it will terminate.
  //  You should check yourself for empty trees, since you have the context to know whether it's an error or OK.)

  if(chain.GetEntries() == 0) LogEXOMsg("Tried to build an index on an empty tree.", EEAlert); // precondition violated.

  TVirtualIndex* index = chain.GetTreeIndex();

  // If it's the wrong index (or nonexistent), build an index.
  if(index == NULL or index->GetMajorName() != major or index->GetMinorName() != minor) {
    Int_t ret = chain.BuildIndex(major.c_str(), minor.c_str());
    if(ret < 0) LogEXOMsg("Failed to build necessary index for chain", EEAlert);
    index = chain.GetTreeIndex();
  }
  return *index;
}

//______________________________________________________________________________
Long64_t EXOCoincidences::GetEntryNumberWithBestIndex(const TVirtualIndex& index, Int_t major, Int_t minor)
{
  // Call the corresponding function of index.
  // But, for root <5.34, an error message is printed too eagerly; suppress it.
  Int_t SaveIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kBreak;
  Long64_t ret = index.GetEntryNumberWithBestIndex(major, minor);
  gErrorIgnoreLevel = SaveIgnoreLevel;
  return ret;
}

//______________________________________________________________________________
bool EXOCoincidences::CanYieldMuonTrackVeto(const EXOEventData& event) const
{
  // Can the given event be considered a muon track?
  return event.fEventHeader.fTaggedAsMuon and not event.fEventHeader.fTaggedAsNoise;
}

//______________________________________________________________________________
std::vector<EXOCoincidences::StartStopTimes> EXOCoincidences::FillBadEnvironmentTimes() const
{
  // Fetch times from the database when the clean room siren was active or the data was otherwise bad;
  // fill fBadEnvironmentTimes appropriately.
  // We only call this once.

  // This is the same calibration database we use in utilities/calib.
  // However, it's set up to answer "given a specific time, what is the state?" questions.
  // We want to ask for the time-history of the environment state, and it's not too hard to do it ourselves.
  TSQLServer* server = TSQLServer::Connect("mysql://mysql-node03.slac.stanford.edu:3306/rd_exo_cond",
                                           "rd_exo_cond_ro", "w0rking.St1fff!");
  if(not server) {
    // I want to make sure people realize if their internet connection is faulty, or if the database is down.
    // OK, it's inconvenient if you don't have internet; maybe it should be possible to disable clean room siren checking.
    LogEXOMsg("Failed to get a connection to the database; we can't check for bad environment times.", EEAlert);
  }

  // Which id's correspond to bad environment times?  It's all here.
  TSQLResult* idResult =
    server->Query(Form("select id from environment where evacuation_alarm_string='true' or bad_time_interval='true'%s;",
		       ( (fUseFridgeCut) ? " or fridge_veto='true'" : "") ) );
  if(not idResult) LogEXOMsg("Failed to get id from environment table of the calibration database.", EEAlert);
  std::set<std::string> idSet;
  TSQLRow* idRow;
  while((idRow = idResult->Next())) {
    idSet.insert(idRow->GetField(0));
    delete idRow;
  }
  delete idResult;
  if(idSet.size() == 0) LogEXOMsg("Failed to get any ids corresponding to a bad environment.", EEAlert);

  // Grab the start and end times of all periods when the environment was bad.
  // We have already done the necessary checks to guarantee idSet has at least one element.
  std::set<std::string>::iterator it = idSet.begin();
  std::string whereString = "data_ident='environment:id:" + *it + "'";
  while(++it != idSet.end()) whereString += " or data_ident='environment:id:" + *it + "'";

  std::string QueryString =
    Form("select vstart,vend from conditions where %s and %s;", whereString.c_str(),
	 (fUseFridgeCut ? 
	  Form("( ( vend > %s  and flavor like 'withfridge') or ( vend <= %s and flavor like 'vanilla') )",
	       fWithFridgeStart.c_str(),fWithFridgeStart.c_str() ) :
	  "flavor like 'vanilla'")
	 );
  TSQLResult* timeResult = server->Query(QueryString.c_str());
  if(not timeResult) LogEXOMsg("Failed to get start and end times for bad environment times.", EEAlert);

  // Parse the result.
  TSQLRow* timeRow;
  while((timeRow = timeResult->Next())) {
    StartStopTimes timepair;
    timepair.fStart = ConvertDateStringToNanoSec(timeRow->GetField(0));
    timepair.fStop = ConvertDateStringToNanoSec(timeRow->GetField(1));
    if (timepair.fStop < timepair.fStart) LogEXOMsg("Error:  bad environment time stops before it starts.", EEAlert);
    fBadEnvironmentTimes.push_back(timepair);
    delete timeRow;
  }
  delete timeResult;

  delete server;

  OptimizeStartStopTimes(fBadEnvironmentTimes);//don't see how this can hurt
  fIt = fBadEnvironmentTimes.begin();//might as well initialize this
  fHasFetchedBadEnvironmentTimes = true;
  return fBadEnvironmentTimes;
}

void EXOCoincidences::OptimizeStartStopTimes(std::vector<EXOCoincidences::StartStopTimes> &startStopTimes) const
{
  //Combine overlapping time periods into one. Includes erasure of identical or
  //superflouous time periods.
  sort(startStopTimes.begin(),startStopTimes.end());//order by start time
  std::vector<EXOCoincidences::StartStopTimes>::iterator it = startStopTimes.begin()+1;
  while (it != startStopTimes.end()) {
    if (it->fStart <= (it-1)->fStop) {//overlapping
      if (it->fStop > (it-1)->fStop) {//not fully contained within previous period
        (it-1)->fStop = it->fStop;//extend previous to new end time
      }
      //At this point the current time period must be accounted for by the
      //previous time period so erase this one.
      it = startStopTimes.erase(it);
      continue;//iterator already updated by erase so skip the ++ below
    }
    it++;
  }
}

void EXOCoincidences::ResetBETiterator() const
{
  //Reset internal iterator over BETs.
  fIt = fBadEnvironmentTimes.begin();
}
const EXOCoincidences::StartStopTimes* EXOCoincidences::GetNextBET() const
 {
  //Get next (potentially first) pointer to next BET StartStopTimes struct.
  //Returns NULL beyond the last BET.
  if (fIt == fBadEnvironmentTimes.end()) return NULL;
  const StartStopTimes *ret=&(*fIt);
  fIt++;
  return ret;
}
const EXOCoincidences::StartStopTimes* EXOCoincidences::GetPreviousBET() const
 {
  //Get previous pointer to next BET StartStopTimes struct.
  //Returns NULL if iterator is already at first BET.
  if (fIt == fBadEnvironmentTimes.begin()) return NULL;
  fIt--;
  const StartStopTimes *ret=&(*fIt);
  return ret;
}




//______________________________________________________________________________
Long64_t EXOCoincidences::ConvertDateStringToNanoSec(std::string dateString) const
{
  // Parse the date strings returned from the calibration database into a time in ns.
  std::istringstream dateStream(dateString);
  dateStream.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  // These times are UTC; so we don't have to do that conversion.  Just parse the strings.
  std::vector<UInt_t> dateValues(6);
  for(size_t i = 0; i < 6; i++) {
    dateStream >> dateValues[i];
    // Assume that the delimiter format will remain the same -- each is exactly one character long.
    if(i != 5) dateStream.ignore();
  }
  TTimeStamp dateTimestamp(dateValues[0], dateValues[1], dateValues[2], dateValues[3], dateValues[4], dateValues[5]);
  Long64_t timeNanoSec = dateTimestamp.GetSec();
  timeNanoSec *= 1000*1000*1000;
  timeNanoSec += dateTimestamp.GetNanoSec(); // probably zero, but this is easy enough.
  return timeNanoSec;
}

//______________________________________________________________________________
EXOCoincidences::~EXOCoincidences()
{
  DeregisterDelegation();
}

//______________________________________________________________________________
void EXOCoincidences::RegisterDelegation()
{
  // Register EXOCoincidences::IsVetoed to be called when
  // EXOEventData::IsVetoed is called.
  delete fRegisterDeligate; 
  fRegisterDeligate = new EXODelegate::RegisterDelegate(
    EXO_DELEGATED_FUNCTION(EXOEventData, IsVetoed),
    EXO_BIND_MEMBER_FUNCTION(EXOCoincidences::IsVetoed, this));
}

//______________________________________________________________________________
void EXOCoincidences::DeregisterDelegation()
{
  // Deregister any previous delegation set up by this instance.
  delete fRegisterDeligate;
  fRegisterDeligate = NULL;
}

//______________________________________________________________________________
void EXOCoincidences::ExtractVetoingEvents()
{
  // Read the chain once, and extract the times of events which can cause a veto.
  fEventsYieldingMuonVeto.clear();
  fEventsYieldingTpcVeto.clear();
  fEventChain.SetEstimate(fEventChain.GetEntries()+1);
  fEventChain.Draw("fEventHeader.fTriggerSeconds:fEventHeader.fTriggerMicroSeconds:fEventHeader.fTaggedAsMuon:(fEventHeader.fSumTriggerRequest!=0 || fEventHeader.fIndividualTriggerRequest!=0) && (fScintClusters@.size()>0 || fHasSaturatedChannel)", "!fEventHeader.fTaggedAsNoise", "goff");
  Double_t* v1 = fEventChain.GetV1();
  Double_t* v2 = fEventChain.GetV2();
  Double_t* v3 = fEventChain.GetV3();
  Double_t* v4 = fEventChain.GetV4();
  for(Long64_t row = 0; row < fEventChain.GetSelectedRows(); row++) {
    bool YieldsMuonVeto = not (-0.5 < v3[row] and v3[row] < 0.5);
    bool YieldsTpcVeto = not (-0.5 < v4[row] and v4[row] < 0.5);
    UInt_t seconds = UInt_t(v1[row]+0.5);
    UInt_t microseconds = UInt_t(v2[row]+0.5);
    Long64_t time = ConvertTimeToNanoSec(seconds, microseconds);
    if(YieldsMuonVeto) fEventsYieldingMuonVeto.push_back(time);
    if(YieldsTpcVeto) fEventsYieldingTpcVeto.push_back(time);
  }
  std::sort(fEventsYieldingMuonVeto.begin(), fEventsYieldingMuonVeto.end());
  std::sort(fEventsYieldingTpcVeto.begin(), fEventsYieldingTpcVeto.end());
}
