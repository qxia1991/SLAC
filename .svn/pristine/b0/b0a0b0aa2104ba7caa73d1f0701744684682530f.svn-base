#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include "EXOUtilities/EXODimensions.hh"

EXOBaselineAndNoiseCalculator::EXOBaselineAndNoiseCalculator()
: EXOVWaveformExtractor("Baseline"),
  fTriggerSample((size_t)TRIGGER_SAMPLE),
  fMaxIterations(20)
{

}

void EXOBaselineAndNoiseCalculator::DoExtractAll(const EXODoubleWaveform& wf) const
{
  EXOMiscUtil::ParameterMap& params = GetParameterMapForUpdates();
  double baseline = wf[0];
  double noisecounts = 4096.;

  // iterate up to kMaxBaselineCalculationIterations times
  size_t use_length = (4*fTriggerSample/5 < wf.GetLength()) ? 4*fTriggerSample/5 : wf.GetLength();
  for (Int_t j = 0; j<fMaxIterations; j++) {
    Double_t baseline_temp = 0.0;
    Double_t noisecounts_temp = 0.0;
    size_t ignore = 0;
    for(size_t i = 0; i < use_length; i++) {
      if (fabs((Double_t)wf[i] - baseline) <= 3.0*noisecounts) {
        baseline_temp += (Double_t) wf[i];
        noisecounts_temp += fabs((Double_t) wf[i] - baseline);
      }
      else ignore++;
    }
    if (ignore == use_length) {
      LogEXOMsg("Baseline/noise calculation failed -- too noisy.", EEWarning);
      baseline = 0.0;
      noisecounts = 100.0;
      break;
    }

    baseline_temp /= (Double_t)use_length - ignore;
    // multiply by 1.01*1.25 because we're cutting off the tails of the
    // distribution and we're using mean absolute deviation instead of RMS.
    noisecounts_temp *= 1.01*1.25 / ((Double_t)use_length - ignore);

    if ( j !=0 && 
         fabs(baseline_temp - baseline) < 0.1 && 
         fabs(noisecounts_temp - noisecounts) < 0.1 ) {
      // Convergence
      baseline = baseline_temp;
      noisecounts = noisecounts_temp;
      break;
    }

    baseline = baseline_temp;
    noisecounts = noisecounts_temp;
  }
  
  // Insert and return the results.
  params.insert(std::pair<std::string,double>("Baseline",baseline));
  params.insert(std::pair<std::string,double>("Noisecounts",noisecounts));
}
