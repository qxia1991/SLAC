#ifndef EXOSignal_hh
#define EXOSignal_hh
#include "TObject.h"
//______________________________________________________________________________
// EXOSignal encapsulates a single signal, which includes the time and
// magnitude and their respective errors.
//______________________________________________________________________________

class EXOSignal: public TObject
{
  public:
    double fTime;              //signal peaking time in nanoseconds
    double fTimeError;         //error of signal peaking time in nanoseconds
    double fMagnitude;         //signal amplitude in ADC counts
    double fMagnitudeError;    //error of signal amplitude in ADC counts
    double fFilteredWFPeakMagnitude;   //Peak magnitude of the filtered waveform
    double fFilteredWFPeakTime;        //Time at which the filtered waveform peaks

    EXOSignal():TObject(),
                fTime(0),
                fTimeError(0),
                fMagnitude(0),
                fMagnitudeError(0),
                fFilteredWFPeakMagnitude(0),
                fFilteredWFPeakTime(0)
                {};
    virtual void Clear(Option_t * /*opt*/ = "")
    {
      fTime = 0;
      fTimeError = 0;
      fMagnitude = 0;
      fMagnitudeError = 0;
      fFilteredWFPeakMagnitude = 0;
      fFilteredWFPeakTime = 0;
    }

    bool operator<(const EXOSignal& sig) const
      { return fTime < sig.fTime; }

    void Print(Option_t* opt = "") const;

  ClassDef( EXOSignal, 2 )
};

#endif
