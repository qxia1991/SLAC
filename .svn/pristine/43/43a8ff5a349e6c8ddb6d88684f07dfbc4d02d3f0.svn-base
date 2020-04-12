#ifndef EXOMultipleSignalFinder_hh
#define EXOMultipleSignalFinder_hh

#include "EXOReconstruction/EXOVSignalFinder.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include <cstddef> //for size_t

class EXOTalkToManager;

class EXOMultipleSignalFinder : public EXOVSignalFinder
{

private :

  double        fFilterTimeConstant;
  double        fFilterFlatTime;
  double        fSigmaThreshold;
  size_t        fTriggerSample;
  bool          fUseMatchedTriangleFilter;
  EXOBaselineAndNoiseCalculator fBaselineCalculator;

public :

  EXOMultipleSignalFinder();
  ~EXOMultipleSignalFinder() {};
  virtual EXOSignalCollection FindSignals(
    const EXOReconProcessList &processList, 
    const EXOSignalCollection& inputSignals) const;
  void FindSignals(EXOChannelSignals& returnChannelSignals,
             const EXOChannelSignals& inputChannelSignals,
             const EXODoubleWaveform& Waveform,
             size_t Begin,
             size_t End,
             double baseline) const;

  void SetFilterTimeConstant (double aval) { fFilterTimeConstant=aval; }
  void SetFilterFlatTime (double aval) { fFilterFlatTime=aval; }
  void SetSigmaThreshold (double aval) { fSigmaThreshold=aval; }
  void SetUseMatchedTriangleFilter(bool aval) { fUseMatchedTriangleFilter = aval; }
  void GetShapedModel(EXODoubleWaveform &fShapedModel, double fFilterTimeConstant) const;
  double CalculateNoiseCounts(const EXODoubleWaveform &wf) const;
  void Filter(EXODoubleWaveform& filtered_signal, int Channel) const;

  void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

  void SetTriggerSample(size_t trig_sample)
    { fTriggerSample = trig_sample; }
};
#endif

  


