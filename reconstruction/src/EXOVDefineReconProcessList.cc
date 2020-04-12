//______________________________________________________________________________
//
// EXOVDefineReconProcessList defines an interface to define a process list for
// the reconstruction processing.  A derived function can, for example,
// implement GetProcessList and calculate summed waveforms and return a
// EXOReconProcessList with these included.  
// The function GetProcessList takes as input:
//   data : EXOReconProcessList, any previous list generated by other
//   EXOVDefineReconProcessList classes.
//  and returns:
//    EXOReconProcessList: a list (can be empty) of entities to process.  This
//    does *not* need to append to the input list.
//
//  Since any derived waveforms will also need to have associated channel
//  data, derived classes can add a transfer function for a channel by calling
//  the protected member function
//
//   UpdateSignalModelMgrWithSignalBuilder();
//
//______________________________________________________________________________
#include "EXOReconstruction/EXOVDefineReconProcessList.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

//______________________________________________________________________________
void EXOVDefineReconProcessList::UpdateSignalModelMgrWithBuilder(
  int chanOrTag,
  const EXOVSignalModelBuilder& builder) const
{
  // Derived classes can call this to add a transfer function to the
  // EXOSignalModelManager.  This is, for example, useful when a class
  // generates a derived waveform (summed APD, etc.) and needs to define a
  // transfer function for this waveform.

  if (!SigModel()) {
    LogEXOMsg("Must have registered signal model manager", EEError);
    return;
  } 
  SigModel()->AddDerivedSignalModelForChannelOrTag(chanOrTag, builder);
}
  
