#include "EXOUtilities/EXODigitizeAPDs.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOElectronics.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include <sstream>
#include <utility>

EXODigitizeAPDs::EXODigitizeAPDs() :
    trigger_time(TRIGGER_TIME),
    nsample(0),
    fManualYieldFactor(1.),
    fDdata(NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE),
    fSampledData(NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE),
    fElectronics(NULL),
    fApplyEmpiricalScaling(true),
    fACSmearFactor(0.0)
{ }

void EXODigitizeAPDs::Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData)
{

  // Intialize the signals
  ResetAPDs();

  // Add up the number of internal APD hits on each gang of seven at each time increment

  for (size_t i=0; i < MonteCarloData.GetNumAPDHits(); i++) {
    // We assume that the number of hits is added to the charge
    // This is to deal with "internal" APD hits which deposit energy/charge
    // and straight hits which only deposit a "count"
    const EXOMCAPDHitInfo* apdInfo = MonteCarloData.GetAPDHit(i);
    AddAPDHitToSignals(apdInfo->fGangNo,
                       apdInfo->fTime,
                       apdInfo->fCharge + apdInfo->fNumHits);

  } // End loop over APD internal hit collection

  // Shape the high bandwidth signals which got hit.
  ShapeAPDSignals();

  // Sample the APD signals down to the correct sampling frequency, save in fSampledData
  DoADCSamplingAPDs();

  // Multiply by the APD gain and scale for the amplifier; add baselines.
  ApplyAPDGain();

  // Apply noise transform
  AddNoiseToAPDSignals();

  // Digitize and save into EXOWaveformData.
  ScaleAndDigitizeAPDSignals(WaveformData);
}

void EXODigitizeAPDs::AddAPDHitToSignals(Int_t GangNo, Double_t Time, Double_t Magnitude)
{
  // Add the unshaped signal for this gang to the high-bandwidth waveform.
  // Register that the gang has been hit.
  
  if ( GangNo < 0 or GangNo >= NAPDPLANE*NUMBER_APDS_PER_PLANE ) {
    std::ostringstream stream;
    stream << "APD error: gangNo = " << GangNo;
    LogEXOMsg(stream.str(), EEError);
    return;
  }	

  if(Time + trigger_time < 0) {
    LogEXOMsg("APD hit occurs before the trace begins; consider using a later trigger time.", EEWarning);
    return;
  }
  size_t HitIndex = (size_t)((Time + trigger_time)/SAMPLE_TIME_HIGH_BANDWIDTH);
  if(HitIndex >= nsample*BANDWIDTH_FACTOR) {
    LogEXOMsg("APD hit occurs after the trace ends; consider lengthening the digitization window.", EEWarning);
    return;
  }

  // Now add it to the high-sampling-rate waveform (unshaped).
  EXODoubleWaveform& wf = fDdata[GangNo];
  for(size_t i = HitIndex; i < wf.GetLength(); i++) wf[i] += Magnitude;

  // This channel is now hit; flag it so we know to shape it, etc.
  fAPDSignalChannels.insert(GangNo);
}

void EXODigitizeAPDs::ScaleAndDigitizeAPDSignals(EXOWaveformData& wfData)
{
  // Add waveforms to WaveformData
  for ( size_t i = 0; i < fSampledData.size(); i++ ) {
    EXODoubleWaveform& tempD = fSampledData[i];
    EXOWaveform& NewWaveform = *wfData.GetNewWaveform();
    NewWaveform.MakeSimilarTo(tempD);
    NewWaveform.fChannel = NWIREPLANE*NCHANNEL_PER_WIREPLANE + i;
    for (size_t m = 0; m < nsample; m++ ) {
      NewWaveform[m] =  (int)(tempD[m]+0.5); 
    }
  }
}

void EXODigitizeAPDs::ResetAPDs()
{

  for (size_t i = 0; i < fDdata.size(); i++) {
    fDdata[i].Zero(); 
  }

  fAPDSignalChannels.clear();

}

void EXODigitizeAPDs::AddNoiseToAPDSignals()
{
  // Add noise the the APD signals.

  // If this is NULL, get out.
  if (not fElectronics) return;
  for ( size_t i = 0; i < NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE; i++ ) {
    size_t chan = NWIREPLANE*NCHANNEL_PER_WIREPLANE + i;
    if (not fElectronics->HasNoiseTransformForChannel(chan)) continue;
    fElectronics->GetNoiseTransformForChannel(chan).Transform(&fSampledData[i]);
  }

}

void EXODigitizeAPDs::ApplyAPDGain()
{
  double bits_over_fullscale = double(ADC_BITS)/APD_ADC_FULL_SCALE_ELECTRONS;
  for ( size_t i = 0; i < NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE; i++ ) {
    fSampledData[i] *= APD_GAIN*bits_over_fullscale;
    fSampledData[i] *= (1-fACSmearFactor);

    //Apply empirical scaling to match Run 2 data amplitudes
    if( fApplyEmpiricalScaling ) {
      Double_t scale_fac = GetScalingOnChannel( NWIREPLANE*NCHANNEL_PER_WIREPLANE + i );
      fSampledData[i] *= scale_fac;
    }

    fSampledData[i] *= fManualYieldFactor; // This is 1 by default, so it's usually a no-op.
                                           // (Energy -> photon conversion factor is set in EXOEventAction, unfortunately,
                                           //  so this lets me scale it manually without re-generating monte carlo.)
    fSampledData[i] += APD_ADC_BASELINE_COUNTS;
  }

}

void EXODigitizeAPDs::ShapeAPDSignals()
{
  // Shape the APD signals.  This function uses the input EXOElectronics class
  // to get the correct transforms to shape for a given channel.

  if (not fElectronics) return;
  // We shape *only* the signals which were filled
  std::set<int>::const_iterator iter;
  Double_t gain = 0.0; 
  const EXOTransferFunction* lastTransfer = 0;
  for ( iter = fAPDSignalChannels.begin(); iter != fAPDSignalChannels.end(); iter++ ) {
    if (not fElectronics->HasTransferFunctionForChannel(NWIREPLANE*NCHANNEL_PER_WIREPLANE + *iter)) continue;

    const EXOTransferFunction& transFunc = fElectronics->GetTransferFunctionForChannel(NWIREPLANE*NCHANNEL_PER_WIREPLANE + *iter);
    if ( not lastTransfer || *lastTransfer != transFunc ) {
      // means we need to recalculate the gain.  FixMe, if this is going to be
      // called with many different shaping times, this will be a slowdown.
      gain = transFunc.GetGain();
      lastTransfer = &transFunc;
    }
    EXODoubleWaveform& wf = fDdata[*iter];
    transFunc.Transform(&wf);
    // Divide by the shaper gain
    wf /= gain; 
  }
}

void EXODigitizeAPDs::DoADCSamplingAPDs()
{
  // Do the ADC sampling of the shaped signal

  if(fDdata.size() != fSampledData.size()) {
    LogEXOMsg("The source and destination waveform arrays are different lengths!", EEAlert);
  }

  for ( size_t i = 0; i < fSampledData.size(); i++ ) {
    for ( size_t m = 0; m < fSampledData[i].GetLength(); m++ ) {
      fSampledData[i][m] = fDdata[i][m*BANDWIDTH_FACTOR];
    }
  }
}

void EXODigitizeAPDs::set_nsample( size_t asample )
{

  // Get memory for signals
  if ( asample <= 0 ) {
    std::ostringstream stream;
    stream << "nsample = " << asample << " not valid";
    LogEXOMsg(stream.str(), EEError);
    return;
  }

  if ( asample != nsample ) {
    nsample = asample;

    for (size_t i = 0; i < NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE; i++) {
      fDdata[i].SetLength(nsample*BANDWIDTH_FACTOR);
      fDdata[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH);
      fSampledData[i].SetLength(nsample);
      fSampledData[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH*BANDWIDTH_FACTOR);
    }
  }

}

double EXODigitizeAPDs::GetScalingOnChannel(const size_t channel) const
{
  std::map<size_t, double>::const_iterator Iter = fScaling.find(channel);
  if(Iter == fScaling.end()) {
    std::ostringstream stream;
    stream<<"The scaling on channel "<<channel<<" was not found.";
    LogEXOMsg(stream.str(), EEAlert);
    return 0.0;
  }
  return Iter->second;
}
