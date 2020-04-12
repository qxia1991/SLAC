#ifndef EXOFrequencyPeakFilter_hh
#define EXOFrequencyPeakFilter_hh

#include "EXOUtilities/EXOVWaveformTransformer.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOWaveformFT.hh"

class EXOFrequencyPeakFilter: public EXOVWaveformTransformer
{
  public:
    EXOFrequencyPeakFilter();
    void SetSpectrum(const EXOWaveformFT& spectrum, double nsigma, size_t nbins);

  protected:
    virtual bool IsOutOfPlace() const {return true;}
    virtual bool IsInPlace() const {return true;}
    virtual void TransformInPlace(EXODoubleWaveform& input) const;
    virtual void TransformOutOfPlace(const EXODoubleWaveform& input, EXODoubleWaveform& output) const;

    EXODoubleWaveform fFilter;
};

#endif
