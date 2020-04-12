#ifndef EXOBandpassFilter_hh
#define EXOBandpassFilter_hh

#include "EXOUtilities/EXOVWaveformTransformer.hh"

class EXOBandpassFilter : public EXOVWaveformTransformer 
{
  public:
    EXOBandpassFilter(); 
    virtual ~EXOBandpassFilter() {}

    virtual bool IsInPlace() const { return true; }
    
    virtual void SetLowerBandpass(double lower) 
      { fLowerBandpass = lower; }
    virtual double GetLowerBandpass() const 
      { return fLowerBandpass; }
    virtual void SetUpperBandpass(double upper) 
      { fUpperBandpass = upper; }
    virtual double GetUpperBandpass() const 
      { return fUpperBandpass; }

   protected: 
     double fLowerBandpass; // The lower bandpass
     double fUpperBandpass; // The upper bandpass
    
      virtual void TransformInPlace(EXODoubleWaveform& anInput) const;
};

#endif
