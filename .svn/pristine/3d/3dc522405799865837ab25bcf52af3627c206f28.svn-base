#ifndef EXOGLITCHRECORD_HH
#define EXOGLITCHRECORD_HH

#ifndef ROOT_TObject
#include "TObject.h"
#endif

class EXOGlitchRecord : public TObject
{
  public:

    EXOGlitchRecord() : TObject() { EXOGlitchRecord::Clear(); }
    virtual void Clear(Option_t* = "");

    UInt_t fRunNumber;                          // The run in which this record occurred.
    UInt_t fSerialNumber;			// Serial number of the veto record.
    UInt_t fSeconds;				// Time of the glitch in Unix time (seconds since Jan. 1, 1970).
    UInt_t fMicroSeconds;			// Subsecond part of the time (microseconds).
    Bool_t fGlitchLevelOne;                     // Glitch of magnitude 5 mV.
    Bool_t fGlitchLevelTwo;                     // Glitch of magnitude 50 mV.
    Bool_t fGlitchLevelThree;                   // Glitch of magnitude 2 V.

    UInt_t fVersion;				// Low-level -- the version of the veto record.
    Bool_t fTimingError;			// Low-level -- never was quite clear what this meant.

    bool operator==(const EXOGlitchRecord& rec) const;

  ClassDef( EXOGlitchRecord, 3 )
};

#endif /* EXOGLITCHRECORD_HH */
