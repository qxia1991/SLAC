//______________________________________________________________________________
// 
// EXODefineAPDSumProcessList calculates the sum APD waveforms and adds these
// waveforms to the process list.  For more information, see GetProcessList()
//
//______________________________________________________________________________

#include "EXOReconstruction/EXODefineAPDSumProcessList.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOAPDSignalModelBuilder.hh"

//______________________________________________________________________________
EXODefineAPDSumProcessList::EXODefineAPDSumProcessList() :
  fSumBothAPDPlanes(false)
{}

//______________________________________________________________________________
EXOReconProcessList 
EXODefineAPDSumProcessList::GetProcessList(const EXOReconProcessList& data) const
{
  // Calculates APD sums and adds them to the process list.  The signal model
  // from the APD signal is built using EXOAPDSignalModelBuilder and the
  // transfer function used is the *first* APD gang found.  This absolutely
  // *assumes* that the transfer functions of all the channels are the same. 

  EXOReconProcessList retList;
  
  //std::cout << "APDSum: "<< data.GetSize() << std::endl;  //DEBUG
  if (data.GetSize() == 0) return retList;

  data.ResetIterator();
  const EXOWaveform* wfptr = data.GetNextWaveform();
  fAPDSumNorthOrBoth.MakeSimilarTo(*wfptr);
  fAPDSumSouth.MakeSimilarTo(*wfptr);
  fAPDSumNorthOrBoth.Zero();
  fAPDSumSouth.Zero();

  const EXOTransferFunction* trans = 0;

  data.ResetIterator();
  const EXOReconProcessList::WaveformWithType* wfWithType = NULL;
  while ((wfWithType = data.GetNextWaveformAndType()) != NULL) {
    const EXOWaveform& wf = *wfWithType->fWf;
    //std::cout << "In WHILE\n"; //DEBUG

    // Only look at APD gangs.
    if(wf.GetChannelType() != EXOMiscUtil::kAPDGang) continue;
    // Furthermore, only look at APDs that *behave like* APDs -- we don't want to sum up charge injection signals.
    if(wfWithType->fType != EXOReconUtil::kAPD) continue;

    // We get the first transfer function we can, we *ARE* assuming all the
    // transfer funtions are the same.
    if (trans == NULL) {
      //std::cout << "Try to get sigmodel for channel:" << wf.fChannel <<"\n"; //DEBUG
      const EXOSignalModel* sigmodel = SigModel()->GetSignalModelForChannelOrTag(wf.fChannel);

      //if (sigmodel == NULL) std::cout << "sigmodel == NULL\n"; //DEBUG
      if (sigmodel != NULL) trans = &sigmodel->GetTransferFunction();
    }
    
    switch (wf.GetTPCSide()) {

      // North APD Plane
      case EXOMiscUtil::kNorth:
        fAPDSumNorthOrBoth += wf;
        break;

      // South APD Plane
      case EXOMiscUtil::kSouth:
        if (not fSumBothAPDPlanes) fAPDSumSouth += wf;
        else fAPDSumNorthOrBoth += wf; 
        break;
    }

  } // end loop over waveforms

  if (trans == NULL) {
    // No APD waveforms found -- so go ahead and return now.
    //std::cout << " No APD waveforms found -- so go ahead and return now.";  //DEBUG
    return retList;
  }
  
  fAPDSumNorthOrBoth.fChannel = EXOReconUtil::kAPDSumNorthOrBoth;
  fAPDSumSouth.fChannel = EXOReconUtil::kAPDSumSouth;

  /////////////////////////////////////////////////////////////////////////////
  // Add the signal models for the sum channels.  We use the
  // EXOAPDSignalModelBuilder because we want these signals to be generally
  // treated like APD signals.
  UpdateSignalModelMgrWithBuilder(
      fAPDSumNorthOrBoth.fChannel,
      EXOAPDSignalModelBuilder(*trans));

   // Add to the return list
  retList.Add(fAPDSumNorthOrBoth, EXOReconUtil::kAPD);
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  if (not fSumBothAPDPlanes) {
    // We only add the south if we are *not* summing the planes together 
    UpdateSignalModelMgrWithBuilder(
        fAPDSumSouth.fChannel, 
        EXOAPDSignalModelBuilder(*trans));
    retList.Add(fAPDSumSouth, EXOReconUtil::kAPD);
  }
  /////////////////////////////////////////////////////////////////////////////

  return retList;

}
