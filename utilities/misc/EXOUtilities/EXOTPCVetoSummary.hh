#ifndef EXOTPCVetoSummary_hh
#define EXOTPCVetoSummary_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TBits
#include "TBits.h"
#endif

class EXOTPCVetoSummary : public TObject
{
  public:
    UChar_t fChannel;                           // The hardware channel of the hit
    Long64_t fMicroSecondsBeforeTPCTrigger;     /* The time before the tpc trigger when the veto trigger occurred. 
                                                   If the veto happened during the tpc trace but after the tpc trigger,
                                                   this is recorded as a negative number. */
    TBits fMask;                                /* The veto mask associated with this particular fire
                                                   (in case multiple channels fired). */

    bool operator==(const EXOTPCVetoSummary& comp) const;

    EXOTPCVetoSummary();
    void Clear(Option_t* = "");

  ClassDef(EXOTPCVetoSummary, 1)
};
#endif
