#ifndef EXOCoincidences_hh
#define EXOCoincidences_hh

#include "Rtypes.h"
#include "TChain.h"
#include "EXOControlRecordList.hh"
#include <vector>
#include <string>
#include <map>

class EXOEventData;
class EXOVetoEventHeader;
class EXOGlitchRecord;
class TVirtualIndex;
namespace EXODelegate {
  class VRegisterDelegate;
}

class EXOCoincidences
{
 public:
  EXOCoincidences();
  ~EXOCoincidences();
  void Load(TChain& chain);
  void Load(std::string FileName);

  bool IsVetoed(const EXOEventData& event) const;
  bool IsVetoed_VetoPanel(const EXOEventData& event,Double_t &veto_minus, Double_t &veto_plus, Int_t &veto_multiplicity) const;
  bool IsVetoed_MuonTrack(const EXOEventData& event,Double_t &muon_minus, Double_t &muon_plus) const;
  bool IsVetoed_TpcEvent(const EXOEventData& event) const;
  bool IsVetoed_BlankedOutTime(const EXOEventData& event) const;

  // Types of blanked-out times, in case we need to distinguish.
  bool IsVetoed_RunBoundary(const EXOEventData& event) const;
  bool IsVetoed_BadEnvironment(const EXOEventData& event) const;

  Long64_t Time_to_TpcEvent(const EXOEventData& event, const int returnas = 0 ) const; // scan for min (0)  next (1) / prior (-1) -- return in nanoseconds.

  // All time quantities are in ns (CLHEP units).
  // As signed 64-bit integers, they can handle up to 292 years.
  bool fUseVetoPanel;
  Long64_t fVetoPanel_PastTime; // Time prior to an event that a coincident veto may occur (ns); skip if < 0.
  Long64_t fVetoPanel_FutureTime; // Time after an event that a coincident veto may occur (ns); skip if < 0.
  bool fUseMuonTrack;
  Long64_t fMuonTrack_PastTime; // Time prior to an event that a coincident muon track may occur (ns); skip if < 0.
  Long64_t fMuonTrack_FutureTime; // Time after an event that a coincident muon track may occur (ns); skip if < 0.
  bool fUseTpcEvent;
  Long64_t fTpcEvent_PastTime; // Time prior to an event that a coincident tpc event may occur (ns); skip if < 0.
  Long64_t fTpcEvent_FutureTime; // Time after an event that a coincident tpc event may occur (ns); skip if < 0.
  bool fUseBlankedOutTime;
  Long64_t fBlankedOut_PastTime; // Time prior to an event that we must have good data (ns); skip if < 0.
  Long64_t fBlankedOut_FutureTime; // Time after an event that we must have good data (ns); skip if < 0.

  bool fUseFridgeCut; //Use fridge cut in phase II data
  std::string fWithFridgeStart; //2016-01-01 00:00:00 default set in .cc 
  
  bool SetTPCEventTimeCut(double pastsec, double futuresec = -1.0); // set fTpcEvent_times and dependents symmetrically 

  void RegisterDelegation();
  void DeregisterDelegation();

 private:
  mutable TChain fEventChain;
  mutable TChain fVetoChain;
  mutable EXOVetoEventHeader* fVetoRecord;
  mutable TChain fGlitchChain;
  mutable EXOGlitchRecord* fGlitchRecord;

  std::map<Int_t, EXOControlRecordList> fControlRecordListCache;

  mutable bool fHasFetchedBadEnvironmentTimes;

 public:
  struct StartStopTimes {
    StartStopTimes() {}
    StartStopTimes(Long64_t start, Long64_t stop) : fStart(start), fStop(stop) {}
    bool operator<(const StartStopTimes &rhs) const
    {//for std::sorting
      return fStart == rhs.fStart ? fStop < rhs.fStop : fStart < rhs.fStart;
    }
    bool operator==(const StartStopTimes &rhs) const
    {//for std::unique
      return fStart != rhs.fStart ? false : fStop == rhs.fStop;
    }
    Long64_t fStart;
    Long64_t fStop;
  };
  std::vector<EXOCoincidences::StartStopTimes> FillBadEnvironmentTimes() const;
  void OptimizeStartStopTimes(std::vector<EXOCoincidences::StartStopTimes>&) const;
  void ResetBETiterator() const;
  const StartStopTimes* GetNextBET()     const;
  const StartStopTimes* GetPreviousBET() const;

 private:
  mutable std::vector<StartStopTimes> fBadEnvironmentTimes; // Start-stop times for a bad environment.
  mutable std::vector<StartStopTimes>::const_iterator fIt;
  Long64_t ConvertDateStringToNanoSec(std::string dateString) const;

  std::vector<Long64_t> fEventsYieldingMuonVeto;
  std::vector<Long64_t> fEventsYieldingTpcVeto;
  void ExtractVetoingEvents();

  bool CanYieldMuonTrackVeto(const EXOEventData& event) const;

  static Long64_t ConvertTimeToNanoSec(Long64_t sec, Long64_t microsec);
  static const TVirtualIndex& GetOrBuildIndex(TChain& chain, std::string major, std::string minor);
  static Long64_t GetEntryNumberWithBestIndex(const TVirtualIndex& index, Int_t major, Int_t minor);

  EXODelegate::VRegisterDelegate *fRegisterDeligate;

};
#endif
