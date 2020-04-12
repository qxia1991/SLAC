/*
A very simple class to trim waveforms to match the digitization range.
Generally (barring a major hardware upgrade -- 5/16/2012) the digitization is 12-bit unsigned, so the range is [0,4096).
*/

#include "EXOUtilities/EXOTrimWaveforms.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"

EXOTrimWaveforms::EXOTrimWaveforms()
: fMinValue(0),
  fMaxValue(ADC_BITS)
{
  if(fMinValue >= fMaxValue) LogEXOMsg("Minimum digitization value must be less than maximum digitization value.", EEAlert);
}

void EXOTrimWaveforms::SetMinValue(int minValue) 
{
  if(minValue >= fMaxValue) throw EXOMiscUtil::EXOBadCommand("You cannot set the minimum digitization value to exceed the maximum digitization value.");
  fMinValue = minValue;
}

void EXOTrimWaveforms::SetMaxValue(int maxValue)
{
  if(fMinValue >= maxValue) throw EXOMiscUtil::EXOBadCommand("You cannot set the maximum digitization value lower than the minimum digitization value.");
  fMaxValue = maxValue;
}

int EXOTrimWaveforms::GetMinValue() const
{
  return fMinValue;
}

int EXOTrimWaveforms::GetMaxValue() const
{
  return fMaxValue;
}

void EXOTrimWaveforms::TrimWaveforms(EXOWaveformData& wfData) const
{
  // Scan through all available waveforms and trim them.
  for(size_t i = 0; i < wfData.GetNumWaveforms(); i++) {
    // Optimize the common case (no saturation).
    // Also, once we get waveform to edit, the waveforms are forevermore marked as edited --
    // regardless of whether we changed them.
    int minValue = wfData.GetWaveform(i)->GetMinValue();
    int maxValue = wfData.GetWaveform(i)->GetMaxValue();
    if(minValue < fMinValue or maxValue >= fMaxValue) {
      EXOWaveform& wf = *wfData.GetWaveformToEdit(i);
      for(size_t t = 0; t < wf.GetLength(); t++) {
        if(wf[t] <  fMinValue) wf[t] = fMinValue;
        if(wf[t] >= fMaxValue) wf[t] = fMaxValue-1;
      }
    }
  }
}
