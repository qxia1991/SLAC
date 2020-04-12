#ifndef EXODefineUWireIndProcessList_hh
#define EXODefineUWireIndProcessList_hh

#include "EXOReconstruction/EXOVDefineReconProcessList.hh"//includes GetProcessList
#include "EXOUtilities/EXOWaveform.hh"
#include <list>
#include <map>
class EXODefineUWireIndProcessList :  public EXOVDefineReconProcessList{

 public:
  EXODefineUWireIndProcessList() {}
  typedef std::list<std::pair<Int_t,double> > signalList_t;
  typedef std::map<Int_t,EXOWaveform>        waveformMap_t;

  static void AddCandidateSignal(const Int_t,const double);
  static Int_t          GetChannelToAdd(){ return fChannelToAdd; }
  static waveformMap_t& GetWaveformMap() { return fWaveformMap;  }
  static void ResetCandidateSignalList();
  static void SetChannelToAdd(Int_t chn) { fChannelToAdd = chn; }

  static EXOReconProcessList StaticGetProcessList(const EXOReconProcessList&);
  EXOReconProcessList              GetProcessList(const EXOReconProcessList&) const;

 protected:
  static Int_t fChannelToAdd;//channel of waveform to add to the process list
  static signalList_t fCandidateSignalList;//channel and time of candidate induction signals
  static waveformMap_t fWaveformMap;//keep waveforms in the process list from going out of scope
};

#endif /* EXODefineUWireIndProcessList_hh */
