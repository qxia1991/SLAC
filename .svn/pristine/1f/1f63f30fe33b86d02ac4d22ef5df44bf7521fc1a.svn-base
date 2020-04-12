//___________________________________________________________________________
// EXOTPCVetoSummary
//
// This class holds information associated with one veto hit; the time is relative to an associated TPC event.

#include "EXOUtilities/EXOTPCVetoSummary.hh"
#include <limits>

ClassImp(EXOTPCVetoSummary)

//___________________________________________________________________________
EXOTPCVetoSummary::EXOTPCVetoSummary()
: TObject(),
  fMask(64)
{
  Clear();
}

//___________________________________________________________________________
void EXOTPCVetoSummary::Clear(Option_t*)
{
  // Try to pick useful defaults; fMicroSecondsBeforeTPCTrigger is set to its
  // max value so it's least likely to influence a cut.
  fChannel = std::numeric_limits<UChar_t>::max();
  fMicroSecondsBeforeTPCTrigger = std::numeric_limits<Long64_t>::max();
  fMask.ResetAllBits();
}

//___________________________________________________________________________
bool EXOTPCVetoSummary::operator==(const EXOTPCVetoSummary& comp) const
{
  // Return true if all values are the same.
  return ( fChannel == comp.fChannel and
           fMicroSecondsBeforeTPCTrigger == comp.fMicroSecondsBeforeTPCTrigger and
           fMask == comp.fMask );
}
//___________________________________________________________________________
