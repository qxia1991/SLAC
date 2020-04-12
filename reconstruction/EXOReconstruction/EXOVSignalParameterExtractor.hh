#ifndef EXOVSignalParameterExtractor_hh
#define EXOVSignalParameterExtractor_hh
#include "EXOReconstruction/EXOSignalModelRegistrant.hh"
//______________________________________________________________________________
// EXOVSignalParameterExtractor
// Lightweight base-class providing an interface for extracting information for
// waveforms in an EXOReconProcessList process list.
//
// Derived clsses should implement Extract, which takes as input:
//  processList : an EXOReconProcessList, contains  the whole list of entities
//    to process. 
//  inputSignals: an EXOSignalCollection.  A collection of signals that have
//    been found by EXOVSignalFinder classes. 
// 
// this returns an EXOSignalCollection, which should be filled with information
// on the found waveforms. (e.g. height of signal, time of signal)
//______________________________________________________________________________

class EXOEventData;
class EXOSignalCollection;
class EXOReconProcessList;
class EXOVSignalParameterExtractor : public EXOSignalModelRegistrant 
{
  public:
    virtual EXOSignalCollection Extract(
      const EXOReconProcessList& processList, 
      const EXOSignalCollection& inputSignals) const = 0;
};

#endif /* EXOVSignalParameterExtractor_hh */
