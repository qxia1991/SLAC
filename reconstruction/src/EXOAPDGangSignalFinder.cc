//______________________________________________________________________________
// EXOAPDGangSignalFinder
//
// A signal finder that "finds" gang signals.  At the moment, it simply adds
// gang signals to the signal collection list so that they will be processed by
// signal extractors.  See FindSignals for more information.
//______________________________________________________________________________
#include "EXOReconstruction/EXOAPDGangSignalFinder.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOMiscUtil.hh"

//______________________________________________________________________________
EXOSignalCollection EXOAPDGangSignalFinder::FindSignals(
  const EXOReconProcessList& processList, 
  const EXOSignalCollection& inputSignals) const
{
  // Find Signals simply adds APD Gang channels to a signal collection list and
  // returns.  Later extractors can then process these channels as they like.
  // It is possible that in the future, this might do something more advanced. 

  // Collection that we will return
  EXOSignalCollection returnCollection;

  //Loop over all single APD waveforms
  const EXOReconProcessList::WaveformWithType *wf = 0;
  processList.ResetIterator();
  while((wf = processList.GetNextWaveformAndType()) != NULL){
    if( not ( (wf->fType == EXOReconUtil::kAPD) && 
              (wf->fWf->fChannel >=0) ) ){
      continue;
    }

    // We have a hardware signal for a gan
    EXOChannelSignals newChannelSignal;

    newChannelSignal.SetChannel(wf->fWf->fChannel);
    newChannelSignal.SetWaveform(wf->fWf);
    newChannelSignal.SetBehaviorType(wf->fType);
    returnCollection.AddChannelSignal(newChannelSignal);
  }

  return returnCollection;
}
