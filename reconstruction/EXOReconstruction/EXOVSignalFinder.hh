#ifndef EXOVSignalFinder_hh
#define EXOVSignalFinder_hh
#include "EXOReconstruction/EXOSignalModelRegistrant.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"

//______________________________________________________________________________
// EXOVSignalFinder
// Lightweight base-class for that provides an interface for finding signals. 
// Derived classes should overload FindSignals to return a collection of
// signals found (EXOSignalCollection).  
//    The function FindSignals takes as input:
//      processList: an EXOReconProcessList.  This list contains the list of
//        entities to process.
//      inputSignals: an EXOSignalCollection.  This is a collection of signals
//        found by previous EXOVSignalFinder classes.  It may be empty.
//    and returns:
//      EXOSignalCollection : a collection of signals found by this processing.
//        It does not need to append to the input signals.
//
//______________________________________________________________________________

class EXOReconProcessList;
class EXOVSignalFinder : public EXOSignalModelRegistrant 
{
  public:

    virtual EXOSignalCollection FindSignals(
      const EXOReconProcessList& processList, 
      const EXOSignalCollection& inputSignals) const = 0;
};

#endif /* EXOVSignalFinder_hh */
