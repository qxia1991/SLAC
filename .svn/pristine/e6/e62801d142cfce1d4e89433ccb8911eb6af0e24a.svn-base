//__________________________________________________________
// Compute and save the risetimes of the u-wire signals.
// Currently we define the risetime as the time difference between 10% and 95% of the full amplitude of a signal.
// This module will do silly things when a single waveform has multiple signals on it -- currently no attempt at signal subtraction is made.
//
// 2014/09/10 DCM: Updated to also calulate risetime for summed U-wire signal
//                 over all channels in each charge cluster

#include "EXOAnalysisManager/EXORisetimeModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOReconstruction/EXOReconUtil.hh"

using EXOMiscUtil::ParameterMap;

IMPLEMENT_EXO_ANALYSIS_MODULE(EXORisetimeModule, "risetime")

EXORisetimeModule::EXORisetimeModule()
{
  // Initialize the risetime calculation object.
  // These values are hard-coded now, and 5%-95% are selected based on preliminary work by Clayton (2/9/2012) and updated by DCM (2014/09/10).
  fRisetimeCalculation.SetInitialThresholdPercentage(0.05);
  fRisetimeCalculation.SetFinalThresholdPercentage(0.95);
  fExtremumFinder.SetFindMaximum(true);
}

EXOAnalysisModule::EventStatus EXORisetimeModule::ProcessEvent(EXOEventData* ED)
{
  // Fill the risetime calculations for all u-wire signals in ED.
  for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
    EXOUWireSignal* signal = ED->GetUWireSignal(i);
    const EXOWaveform* wf = ED->GetWaveformData()->GetWaveformWithChannel(signal->fChannel);
    EXODoubleWaveform DoubleWF;
    DoubleWF.SetSamplingFreq(wf->GetSamplingFreq());
    DoubleWF.SetTOffset(wf->GetTOffset());
    DoubleWF.SetData(wf->GetData(), wf->GetLength());
    DoubleWF -= signal->fBaseline;
    // It is possible to set the pulse peak height from signal->fRawEnergy, with an appropriate scaling factor.
    // However, the data sometimes undershoots the fit at the peak;
    // This can cause the risetime calculation to fail to find a place where the waveform exceeds 95% of the peak value.
    fRisetimeCalculation.SetPulsePeakHeight(DoubleWF.GetMaxValue());
    fRisetimeCalculation.SetScanFrom(wf->GetIndexAtTime(signal->fTime) > 10 ? wf->GetIndexAtTime(signal->fTime)-10 : 0);
    fRisetimeCalculation.Transform(&DoubleWF);
    signal->fRisetime = fRisetimeCalculation.GetRiseTime();
  }

  // now for each charge cluster, also find the summed rise time over all traces
  for(size_t i = 0; i < ED->GetNumChargeClusters(); i++) {

    EXOChargeCluster* cc = ED->GetChargeCluster(i);
    size_t numUWS = cc->GetNumUWireSignals();
    if( numUWS == 0 ) continue;

    // make a unique list of the channels in the cluster
    std::set<Int_t> UChannels;
    for(size_t j=0; j<numUWS; j++){
      UChannels.insert(cc->GetUWireSignalAt(j)->fChannel);
    }

    // initialize a double waveform with zeros
    const EXOWaveform* wf = ED->GetWaveformData()->GetWaveformWithChannel( *(UChannels.begin()) );
    EXODoubleWaveform DoubleWF;
    DoubleWF.SetSamplingFreq(wf->GetSamplingFreq());
    DoubleWF.SetTOffset(wf->GetTOffset());
    DoubleWF.SetLength(wf->GetLength());
    DoubleWF.Zero();

    // now add any U-wire channel waveforms in the cluster
    for(std::set<Int_t>::iterator iter = UChannels.begin(); iter != UChannels.end(); iter++) {

      const EXOWaveform* curr_wf = ED->GetWaveformData()->GetWaveformWithChannel( *iter );

      EXODoubleWaveform currDWF;
      currDWF.SetSamplingFreq(curr_wf->GetSamplingFreq());
      currDWF.SetTOffset(curr_wf->GetTOffset());
      currDWF.SetData(curr_wf->GetData(), curr_wf->GetLength());

      DoubleWF += currDWF;
    }
   
    // subtract baseline
    ParameterMap noiseAndBaseline = fBaselineCalculator.ExtractAll(DoubleWF);
    DoubleWF -= noiseAndBaseline["Baseline"];
    
    fExtremumFinder.Transform(&DoubleWF);

    fRisetimeCalculation.SetPulsePeakHeight(fExtremumFinder.GetTheExtremumValue());    
    size_t max_time = fExtremumFinder.GetTheExtremumPoint();
    fRisetimeCalculation.SetScanFrom(max_time > 20 ? max_time-20 : 0);

    fRisetimeCalculation.Transform(&DoubleWF);
    cc->fSumRisetime = fRisetimeCalculation.GetRiseTime();
  }
  return kOk;
}
