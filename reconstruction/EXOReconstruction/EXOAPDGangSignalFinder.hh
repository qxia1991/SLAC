#ifndef EXOAPDGangSignalFinder_hh
#define EXOAPDGangSignalFinder_hh

#include "EXOVSignalFinder.hh"

class EXOAPDGangSignalFinder: public EXOVSignalFinder
{
  public:

    EXOSignalCollection FindSignals(const EXOReconProcessList& processList, 
      const EXOSignalCollection& inputSignals) const;

};

#endif
