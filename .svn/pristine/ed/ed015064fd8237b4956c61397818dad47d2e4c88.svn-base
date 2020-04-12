#ifndef EXOReconUtil_hh
#define EXOReconUtil_hh
//______________________________________________________________________________
//
// EXOReconUtil defines a namespace with utilities for reconstruction.  It
// contains enums for:
//
//   ESignalBehaviorType : signal behavior type
//
//   EDerivedTypes : derived signal type, i.e. signals derived from other
//   signals
//
// For new derived types, users should add a new named enum to EDerivedTypes. 
//______________________________________________________________________________
#include <string>
#include <cstddef> //for size_t
#include "EXOUtilities/EXOVWaveformExtractor.hh"

class EXOWaveform;
namespace EXOReconUtil
{
  enum ESignalBehaviorType {
    kUndefined,        // Undefined behavior 
    kUWire,            // Signal behaves like a U-wire
    kVWire,            // Signal behaves like a V-wire
    kAPD,              // Signal behaves like an APD signal
    kChargeInjection,  // Charge-injection signals are treated differently from data signals
    kUWireInd,         // Signal behaves like induciton on a U-wire
    kDerived           // Derived is a catchall for behaviours that are not defined
  };

  // Derived types name different types of derived signals.  These numbers
  // should be *negative*.
  enum EDerivedTypes {
    kAPDSumNorthOrBoth = -1,
    kAPDSumSouth = -2,
    kUWireIndOffset = -500,   // Offset to apply to U wires denoting induction signal type
    // ... etc.  Derived tags defininition 
  };

  enum EReconConstants{
    kMaxBaselineCalculationIterations = 20
  };

} 

#endif /* EXOReconUtil_hh */
