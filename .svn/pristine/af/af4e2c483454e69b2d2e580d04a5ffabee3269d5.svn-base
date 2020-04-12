//______________________________________________________________________________
// 
// EXOReconProcessList contains a list of waveforms that should be processed.
// Waveforms are tagged with their channel number (EXOWaveform::fChannel),
// which means that derived waveforms (e.g. APD sums, Gang sums, etc.) can be
// added to the list as long as their channel/tag does not conflict with
// something already in the list. 
//
// This class does not own any of the data, rather it only saves pointers to
// the correct addresses of the waveforms.  This means that users of this class
// must ensure that any waveforms added to this class must be at least as
// long-lived as this class.
//
// Written: M. Marino 6 Dec 2011
//______________________________________________________________________________

#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"

EXOReconProcessList::EXOReconProcessList() :
  fProcessWFList(),
  fIter(fProcessWFList.begin())
{}

//______________________________________________________________________________
void EXOReconProcessList::Add(const EXOReconProcessList& list, bool force) 
{
  // Add another recon process list to this one.  The bool force is set to true
  // when entries with the same channel should be replaced.
  const WaveformWithType* wfAndType; 
  list.ResetIterator();
  while ((wfAndType = list.GetNextWaveformAndType()) != NULL) Add(*wfAndType->fWf, wfAndType->fType, force);
}

//______________________________________________________________________________
void EXOReconProcessList::Add(const EXOWaveformData& list, bool force) 
{
  // Add an EXOWaveformData list to this one.  The bool force is set to true
  // when entries with the same channel should be replaced.
  size_t len = list.GetNumWaveforms();
  for (size_t i=0;i<len;i++) { 
    EXOMiscUtil::EChannelType type = EXOMiscUtil::TypeOfChannel(list.GetWaveform(i)->fChannel);
    switch(type) {
      case EXOMiscUtil::kUWire:
          Add(*list.GetWaveform(i), EXOReconUtil::kUWire, force); break;
      case EXOMiscUtil::kVWire: 
          Add(*list.GetWaveform(i), EXOReconUtil::kVWire, force); break;
      case EXOMiscUtil::kAPDGang: 
      case EXOMiscUtil::kAPDSumOfGangs: 
          Add(*list.GetWaveform(i), EXOReconUtil::kAPD, force); break;
      default: break; // Other types shouldn't exist in the waveform data.
    }
  }
}

//______________________________________________________________________________
void EXOReconProcessList::Add(const EXOWaveform& wf, 
  EXOReconUtil::ESignalBehaviorType type, bool force) 
{
  // Add a waveform to this list, using the waveform's channel as a tag.  force
  // is true when entries with the same channel or tag should be replaced.  The
  // channel should be positive for *real* waveforms, negative for derived
  // waveforms.  
  if ((fProcessWFList.find(wf.fChannel) == fProcessWFList.end()) || force) {
    WaveformWithType& wfAndType = fProcessWFList[wf.fChannel];
    wfAndType.fWf = &wf;
    wfAndType.fType = type;
  } else {
    LogEXOMsg(
      Form("Channel %d already in list, pass force = true to replace", wf.fChannel), 
      EENotice);
  }
}

//______________________________________________________________________________
void EXOReconProcessList::ResetIterator() const 
{
  // Reset the iterator to the beginning of the list
  fIter = fProcessWFList.begin();
} 

//______________________________________________________________________________
const EXOWaveform* EXOReconProcessList::GetNextWaveform() const
{
  // Get the next waveform in the list, will return NULL if there are no more
  // waveforms.
  const WaveformWithType* wfAndType = GetNextWaveformAndType(); 
  if (wfAndType == NULL) return NULL;
  return wfAndType->fWf;
}

//______________________________________________________________________________
const EXOReconProcessList::WaveformWithType* 
  EXOReconProcessList::GetNextWaveformAndType() const
{
  // Get the next waveform/type in the list, will return NULL if there are no
  // more waveforms.
  if (fIter == fProcessWFList.end()) return NULL;
  const WaveformWithType* retWFAndType = &fIter->second;
  fIter++;
  return retWFAndType;
}

//______________________________________________________________________________
const EXOReconProcessList::WaveformWithType* 
  EXOReconProcessList::GetWaveformAndTypeWithChannelOrTag(int chanOrTag) const
{
  // Return waveform and type with channel or tag number equal to chanOrTag.
  // Returns NULL if the requested chanOrTag cannot be found.
  ProcMap::const_iterator iter = fProcessWFList.find(chanOrTag);
  if (iter == fProcessWFList.end()) return NULL;
  return &iter->second;
}

//______________________________________________________________________________
const EXOWaveform* EXOReconProcessList::GetWaveformWithChannelOrTag(int chanOrTag) const
{
  // Return waveform with channel or tag number equal to chanOrTag.  Returns
  // NULL if the requested chanOrTag cannot be found.
  const WaveformWithType* ret = GetWaveformAndTypeWithChannelOrTag(chanOrTag);
  if (ret == NULL) return NULL;
  return ret->fWf;
}

//______________________________________________________________________________
EXOReconProcessList EXOReconProcessList::GetSubProcessListOfType(
  EXOReconUtil::ESignalBehaviorType type) const
{
  // Returns a sub list with only the type of channels requested.  
  EXOReconProcessList retList;
  ProcMap::const_iterator iter = fProcessWFList.begin();
  while (iter != fProcessWFList.end()) {
    const WaveformWithType& wfAndtype = iter->second;
    if (wfAndtype.fType == type) retList.Add(*wfAndtype.fWf, wfAndtype.fType);
    iter++;
  }
  return retList;
}

