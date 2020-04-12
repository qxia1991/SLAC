#ifndef EXOBaselineAndNoiseCalculator_hh
#define EXOBaselineAndNoiseCalculator_hh

#include <cstddef>
#include "EXOUtilities/EXOVWaveformExtractor.hh"

class EXOBaselineAndNoiseCalculator: public EXOVWaveformExtractor
{
  public:
    EXOBaselineAndNoiseCalculator();

    virtual void DoExtractAll(const EXODoubleWaveform& wf) const;
    void SetTriggerSample(size_t sample){fTriggerSample = sample;}
    void SetMaxIterations(Int_t val){fMaxIterations = val;}

  protected:
    size_t fTriggerSample;
    Int_t fMaxIterations;
};

#endif
