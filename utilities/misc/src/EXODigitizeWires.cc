#include "EXOUtilities/EXODigitizeWires.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOElectronics.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOWireCrossing.hh"
#include "TRandom.h"
#include "TMath.h"
#include <fstream>    // for reading real noise files
#include <cstdlib>    // for the int form of abs
using CLHEP::second;
using CLHEP::keV;
using CLHEP::cm;
using CLHEP::microsecond;

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXODigitizeWires::EXODigitizeWires() : 
    fEField("phid.dat.bin"),
    fWeightField("phiwx.dat.bin", "phiwy.dat.bin"),
    fDdata(NUMBER_READOUT_CHANNELS),
    fSampledData(NUMBER_READOUT_CHANNELS),
    fElectronLifetime(1.0*second),
    fDriftVelocity(DRIFT_VELOCITY),
    fDriftVelocityCollection(0.0),
    fUBaseline(UWIRES_ADC_BASELINE_COUNTS),
    fVBaseline(VWIRES_ADC_BASELINE_COUNTS),
    fVShift(0.0),
    fWvalue_energy_per_electron(W_VALUE_LXE_EV_PER_ELECTRON),
    fTransverseDiffusionCoeff(0),
    fLongitudinalDiffusionCoeff(0),
    fNumDiffusePCDs(0),
    fDiffusionDuringDrifting(false),
    fDigitizeInduction(true),
    fNSample(0),
    fTriggerTime(TRIGGER_TIME),
    fDigitizeVNeighborSignals(1),
    fApplyEmpiricalScaling(true),
    fElectronics(NULL),
    fTimingInfo(NULL)
{

  // Make sure the number of channels is even
  
  assert( (NCHANNEL_PER_WIREPLANE%2) == 0 ); 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXODigitizeWires::set_nsample( size_t asample )
{

  // Get memory for signals
  if ( asample == 0 ) {
    std::ostringstream stream;
    stream << "nsample = " << asample << " not valid";
    LogEXOMsg(stream.str(), EEError);
    return;
  }

  fNSample = asample;

  for (size_t i = 0 ; i < NUMBER_READOUT_CHANNELS; i++) {
    fDdata[i].SetLength(fNSample*BANDWIDTH_FACTOR);
    fDdata[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH);
    fSampledData[i].SetLength(fNSample);
    fSampledData[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH*BANDWIDTH_FACTOR);
  }
}
                                             
void EXODigitizeWires::ShapeWireSignals()
{
  // Shape the high bandwidth signals

  if (not fElectronics) return;
  // We shape *only* the signals which were filled
  std::set<int>::const_iterator iter;
  Double_t gain = 0.0; 
  const EXOTransferFunction* lastTransfer = 0;
  for (iter = fWireSignalChannels.begin();iter != fWireSignalChannels.end();iter++) { 
    
    EXODoubleWaveform& wf = fDdata[*iter];
    if (not fElectronics->HasTransferFunctionForChannel(*iter)) continue;
    const EXOTransferFunction& transFunc = fElectronics->GetTransferFunctionForChannel(*iter);
    if ( not lastTransfer || *lastTransfer != transFunc ) {
      // means we need to recalculate the gain.  FixMe, if this is going to be
      // called with many different shaping times, this will be a slowdown.
      gain = transFunc.GetGain();
      lastTransfer = &transFunc;
    }
    transFunc.Transform(&wf);
    // Divide by the shaper gain
    wf /= gain; 
  }
}

void EXODigitizeWires::DoADCSamplingWires()
{
  // Do the ADC sampling of the shaped signal

  for ( int i = 0; i < NWIREPLANE; i++ ) {
    for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
      EXODoubleWaveform& temp = fDdata[i*NCHANNEL_PER_WIREPLANE + j];
      EXODoubleWaveform& dest = fSampledData[i*NCHANNEL_PER_WIREPLANE + j];
      for ( size_t m = 0; m < fNSample; m++ ) {
        dest[m] = temp[m*BANDWIDTH_FACTOR];
      }
    }
  }

}

void EXODigitizeWires::AddNoiseToWireSignals()
{
  // Add in random noise

  // If this is NULL, get out.
  if (not fElectronics) return;
  for ( int i = 0; i < NWIREPLANE; i++ ) {
    for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
      size_t chan = i*NCHANNEL_PER_WIREPLANE + j;
      if (not fElectronics->HasNoiseTransformForChannel(chan)) continue;
      fElectronics->GetNoiseTransformForChannel(chan).Transform(&fSampledData[chan]);
    }
  }

}

void EXODigitizeWires::ScaleAndDigitizeWireSignals(EXOWaveformData& wfData)
{
  // Scale the signal for the ADC and turn it into integers
  // Also add in the offset to the x and y channels
  // Put waveforms into WaveformData.

  for ( int i = 0; i < NWIREPLANE; i++ ) { 
    for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
      EXODoubleWaveform& tempD = fSampledData[i*NCHANNEL_PER_WIREPLANE + j]; 
      EXOWaveform& NewWaveform = *wfData.GetNewWaveform();

      NewWaveform.SetLength(fNSample);
      NewWaveform.fChannel = i*NCHANNEL_PER_WIREPLANE + j;

      //Apply empirical scaling to match Run 2 data amplitudes
      Double_t scale_fac = fApplyEmpiricalScaling ? GetScalingOnChannel( i*NCHANNEL_PER_WIREPLANE + j ) : 1.0;

      for ( size_t k = 0; k < fNSample; k++ ) {
        double tempValue = tempD[k]*ADC_BITS*scale_fac/(ADC_FULL_SCALE_ELECTRONS_WIRE*fWvalue_energy_per_electron);
        tempValue += ((i/NCHANNEL_PER_WIREPLANE)%2 == 0 ? fUBaseline : fVBaseline); // Use the appropriate baseline.
        NewWaveform[k] = (int)(tempValue + (tempValue > 0.0 ? 0.5 : -0.5) ); // Implement correct rounding.
      }
    }
  }
}

void EXODigitizeWires::ResetWires()
{
  for (size_t i=0;i<NWIREPLANE*NCHANNEL_PER_WIREPLANE;i++) fDdata[i].Zero(); 
  fWireSignalChannels.clear();
}

void EXODigitizeWires::Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData)
{

#define PERFORM_TIMING_AND_FUNCTION(func, temp)         \
  if (fTimingInfo) fTimingInfo->StartTimerForTag(#func);\
  func(temp);                                           \
  if (fTimingInfo) fTimingInfo->StopTimerForTag(#func);
  // Intialize the signals, zeroing them out
  PERFORM_TIMING_AND_FUNCTION(ResetWires, );

  // Following is a value that will create *under 1* ADC count.  We will ignore these.
  double adc_limit = ADC_FULL_SCALE_ENERGY_WIRE/ADC_BITS; 

  if(fTimingInfo) fTimingInfo->StartTimerForTag("GenerateWireSignalAt");

  bool doDiffusionBeforeDrift = fNumDiffusePCDs > 1 and
                                (fTransverseDiffusionCoeff > 0. or fLongitudinalDiffusionCoeff >0.) and
                                not fDiffusionDuringDrifting;
  if(fNumDiffusePCDs < 1){
    fNumDiffusePCDs = 1;
  }
  vector<EXOMCPixelatedChargeDeposit> OriginalPCDs;


  // Loop over PCDs and either digitize them directly (no diffusion)
  // or store them in a vector so they can be split and digitized later (with diffusion)
  for(size_t i = 0; i < MonteCarloData.GetNumPixelatedChargeDeposits(); i++) {
    EXOMCPixelatedChargeDeposit* PixelDeposit = MonteCarloData.GetPixelatedChargeDeposit(i);
    // If the pixel deposit deposits less energy than will even register one
    // ADC count, ignore it.
    if (PixelDeposit->fTotalIonizationEnergy <= adc_limit) continue; 
    // If the pixel deposit is outside the wire hexagon, ignore it.
    EXOWireCrossing& crossing = EXOWireCrossing::GetInstance();
    if(crossing.GetDistanceFromHexagon(PixelDeposit->GetPixelCenter()) > 0){
      continue;
    }
    if(doDiffusionBeforeDrift){
      // If we do diffusion prior to drifting, save the original PCDs and clear the PCDs in 
      // the MonteCarloData afterwards. Then split the original PCDs and create new ones
      OriginalPCDs.push_back(*PixelDeposit);
    }
    else{
      // If we do diffusion during drifting (or no diffusion), just drift the original PCD
      // multiple times
      double OriginalEnergy = PixelDeposit->fTotalEnergy;
      double OriginalIonizationEnergy = PixelDeposit->fTotalIonizationEnergy;
      PixelDeposit->fTotalEnergy = OriginalEnergy / double(fNumDiffusePCDs);
      PixelDeposit->fTotalIonizationEnergy = OriginalIonizationEnergy / double(fNumDiffusePCDs);
      for(size_t j=0; j<fNumDiffusePCDs; j++){
        // Drift charge to generate signals (fills fDdata)
        GenerateUnshapedSignals(PixelDeposit);
      }
      PixelDeposit->fTotalEnergy = OriginalEnergy;
      PixelDeposit->fTotalIonizationEnergy = OriginalIonizationEnergy;
    }
  }

  if(doDiffusionBeforeDrift){
    //Clear the PCDs in the MonteCarloData. We've stored them in a vector.
    MonteCarloData.ClearPixelatedChargeDeposits();
    for(size_t i=0; i<OriginalPCDs.size(); i++){
      const EXOCoordinates& OriginalCoord = OriginalPCDs[i].GetPixelCenter();
      for(size_t j=0; j<fNumDiffusePCDs; j++){
        double X = OriginalCoord.GetX();
        double Y = OriginalCoord.GetY();
        double Z = OriginalCoord.GetZ();
        if(fabs(Z) < CATHODE_ANODE_y_DISTANCE){
          continue;
        }
        double driftTime = (CATHODE_ANODE_y_DISTANCE - fabs(Z)) / fDriftVelocity;
        if(fDriftVelocityCollection > 0){
          driftTime += UPLANE_VPLANE_DISTANCE / fDriftVelocityCollection;
        }
        else{
          driftTime += UPLANE_VPLANE_DISTANCE / fDriftVelocity;
        }
        if(fTransverseDiffusionCoeff > 0.){
          double sigmaTrans = sqrt(2 * fTransverseDiffusionCoeff * driftTime);
          double r = gRandom->Gaus(0.0,sigmaTrans);
          double phi = gRandom->Uniform(TMath::TwoPi());
          X += r * cos(phi);
          Y += r * sin(phi);
        }
        if(fLongitudinalDiffusionCoeff > 0.){
          double sigmaLong = sqrt(2 * fLongitudinalDiffusionCoeff * driftTime);
          Z += gRandom->Gaus(0.,sigmaLong);
        }
        EXOCoordinates newcoord(EXOMiscUtil::kXYCoordinates,X,Y,Z,OriginalCoord.GetT());
        EXOMCPixelatedChargeDeposit* childPCD = MonteCarloData.FindOrCreatePixelatedChargeDeposit(newcoord);
        childPCD->fTotalEnergy += OriginalPCDs[i].fTotalEnergy / double(fNumDiffusePCDs);
        childPCD->fTotalIonizationEnergy += OriginalPCDs[i].fTotalIonizationEnergy / double(fNumDiffusePCDs);
        // Drift charge to generate signals (fills fDdata)
        GenerateUnshapedSignals(childPCD);
      }
    }
  }
  
  if(fTimingInfo) fTimingInfo->StopTimerForTag("GenerateWireSignalAt");

  // Do shaping, sampling, add noise, digitization.
  PERFORM_TIMING_AND_FUNCTION(ShapeWireSignals, );

  PERFORM_TIMING_AND_FUNCTION(DoADCSamplingWires, );

  PERFORM_TIMING_AND_FUNCTION(AddNoiseToWireSignals, );

  PERFORM_TIMING_AND_FUNCTION(ScaleAndDigitizeWireSignals, WaveformData);

}

void EXODigitizeWires::set_electron_lifetime( double value ) 
{

  if ( value <= 1.0*microsecond ) {
    LogEXOMsg("Error: use an electron lifetime greater than one microsecond, substituting default value of one second", EEWarning);
    fElectronLifetime = 1.0*second;
    return;
  }

  std::ostringstream stream;
  stream << "setting the electron lifetime to " << value/microsecond << " microseconds";

  LogEXOMsg(stream.str(), EEDebug);
  fElectronLifetime = value;
  return;

}
void EXODigitizeWires::set_drift_velocity( double value ) 
{
  // Setting the drift velocity in the main part of the detector.

  if ( value <= 0 *cm/microsecond ) {
    LogEXOMsg("use a drift velocity greater than 0, substituting default value of .28 cm/microsecond", EEWarning);
    fDriftVelocity = .28 *cm/microsecond;
    return;
  }

  std::ostringstream stream;
  stream << "setting the drift velocity to " << value/(cm/microsecond) << " cm/microsecond";
  LogEXOMsg(stream.str(),  EEDebug);
  fDriftVelocity = value;
}
void EXODigitizeWires::set_collection_drift_velocity( double value ) 
{
  // Setting the drift velocity in the region once the charge has passed the
  // v-wires. 


  if ( value < 0.0 ) { 
    LogEXOMsg("use a collection drift velocity greater than 0," 
              "substituting default value of 0.0 cm/microsecond", EEWarning);
    fDriftVelocityCollection = 0.0; 
    return;
  }

  std::ostringstream stream;
  stream << "setting the collection drift velocity to " << value/(cm/microsecond) << " cm/microsecond";
  LogEXOMsg(stream.str(),  EEDebug);
  fDriftVelocityCollection = value;
}

void EXODigitizeWires::SetTimingStatisticInfo(EXOTimingStatisticInfo* value)
{
  if(value == NULL) LogEXOMsg("You set fTimingInfo to a null pointer?", EEWarning);
  fTimingInfo = value;
}

void EXODigitizeWires::GenerateUnshapedSignals(EXOMCPixelatedChargeDeposit* Pixel)
{

  // Reset Pixel information that we're about to fill (in case we're redigitizing).
  Pixel->fWireHitTime = 0;
  Pixel->fDepositChannel = -999;
  Pixel->fWireChannelsAffected.clear();

  // Get Pixel information
  const double Upos = Pixel->GetPixelCenter().GetU();
  const double Vpos = Pixel->GetPixelCenter().GetV();
  const double Zpos = Pixel->GetPixelCenter().GetZ();
  const double AbsZpos = CATHODE_APDFACE_DISTANCE - std::fabs(Zpos);
  const double Time = Pixel->GetPixelCenter().GetT();
  const double Energy = Pixel->fTotalIonizationEnergy;

  // Create data structures to store channel information -- these are the channels we'll trace.
  // We'll fill it separately for u-wires and v-wires.
  std::vector<WireToDigitize> ChannelsToUse;
  int Channel;
  WireToDigitize aWire;

  ////////////////////////
  // Start with V-wires //
  ////////////////////////

  // Which channel is closest?
  Channel = (int)(Vpos/CHANNEL_WIDTH) + NCHANNEL_PER_WIREPLANE/2;
  if(Vpos < 0) Channel--; // round down, not towards zero.
  if(Channel < 0) Channel = 0;
  if(Channel >= NCHANNEL_PER_WIREPLANE) Channel = NCHANNEL_PER_WIREPLANE-1;
  Channel += (Zpos >= 0 ? NCHANNEL_PER_WIREPLANE : 3*NCHANNEL_PER_WIREPLANE); // North vs. South plane.

  // This will be the primary reference channel -- it will have position CHANNEL_WIDTH/2, to match the field geometry.
  aWire.fPosition = 0.5*CHANNEL_WIDTH + fVShift;
  aWire.fWaveform = &fDdata[Channel];
  ChannelsToUse.push_back(aWire);
  fWireSignalChannels.insert(Channel);
  Pixel->fWireChannelsAffected.insert(Channel);

  if(fDigitizeInduction) {
    // Also the channels 2 below and 2 above, if available.
    for(int chanGap = -(int)fDigitizeVNeighborSignals; 
        chanGap <= (int)fDigitizeVNeighborSignals; chanGap++) {
      if (chanGap == 0) continue;
      int chosen_channel = Channel + chanGap; 
      if((chosen_channel >= 1*NCHANNEL_PER_WIREPLANE and chosen_channel < 2*NCHANNEL_PER_WIREPLANE) or
         (chosen_channel >= 3*NCHANNEL_PER_WIREPLANE and chosen_channel < 4*NCHANNEL_PER_WIREPLANE)) {
        aWire.fPosition = (0.5+chanGap)*CHANNEL_WIDTH + fVShift;
        aWire.fWaveform = &fDdata[chosen_channel];
        ChannelsToUse.push_back(aWire);
        fWireSignalChannels.insert(chosen_channel);
        Pixel->fWireChannelsAffected.insert(chosen_channel);
      }
    }
  }

  // Electric field reader has wires centered at 1.5mm, 3mm, 4.5mm, etc.
  // Weight potential has reference channel centered at 4.5mm.  Use that, since it has longer period.
  double RelativeVpos = Vpos - (Channel%NCHANNEL_PER_WIREPLANE - NCHANNEL_PER_WIREPLANE/2)*CHANNEL_WIDTH;
  // Don't pass in Pixel -- we don't want to save deposit information from the v-plane trajectory.
  GenerateSignals(ChannelsToUse, EXOMiscUtil::kVWire, EXOMiscUtil::GetTPCSide(Channel), Channel,
                  RelativeVpos, AbsZpos, Time, Energy);

  //////////////////////////////////////////////////////////////
  // Done with v-wires; now u-wires follow same prescription. //
  //////////////////////////////////////////////////////////////

  ChannelsToUse.clear();

  // Which channel is closest?
  Channel = (int)(Upos/CHANNEL_WIDTH) + NCHANNEL_PER_WIREPLANE/2;
  if(Upos < 0) Channel--; // round down, not towards zero.
  if(Channel < 0) Channel = 0;
  if(Channel >= NCHANNEL_PER_WIREPLANE) Channel = NCHANNEL_PER_WIREPLANE-1;
  Channel += (Zpos >= 0 ? 0 : 2*NCHANNEL_PER_WIREPLANE); // North vs. South plane.

  // This will be the primary reference channel -- it will have position CHANNEL_WIDTH/2, to match the field geometry.
  aWire.fPosition = 0.5*CHANNEL_WIDTH;
  aWire.fWaveform = &fDdata[Channel];
  ChannelsToUse.push_back(aWire);
  fWireSignalChannels.insert(Channel);
  Pixel->fWireChannelsAffected.insert(Channel);

  if(fDigitizeInduction) {
    // Also the channels below and above, if available.
    if((Channel-1 >= 0*NCHANNEL_PER_WIREPLANE and Channel-1 < 1*NCHANNEL_PER_WIREPLANE) or
       (Channel-1 >= 2*NCHANNEL_PER_WIREPLANE and Channel-1 < 3*NCHANNEL_PER_WIREPLANE)) {
      aWire.fPosition = -0.5*CHANNEL_WIDTH;
      aWire.fWaveform = &fDdata[Channel-1];
      ChannelsToUse.push_back(aWire);
      fWireSignalChannels.insert(Channel-1);
      Pixel->fWireChannelsAffected.insert(Channel-1);
    }
    if((Channel+1 >= 0*NCHANNEL_PER_WIREPLANE and Channel+1 < 1*NCHANNEL_PER_WIREPLANE) or
       (Channel+1 >= 2*NCHANNEL_PER_WIREPLANE and Channel+1 < 3*NCHANNEL_PER_WIREPLANE)) {
      aWire.fPosition = 1.5*CHANNEL_WIDTH;
      aWire.fWaveform = &fDdata[Channel+1];
      ChannelsToUse.push_back(aWire);
      fWireSignalChannels.insert(Channel+1);
      Pixel->fWireChannelsAffected.insert(Channel+1);
    }
  }

  // Electric field reader has wires centered at 1.5mm, 3mm, 4.5mm, etc.
  // Weight potential has reference channel centered at 4.5mm.  Use that, since it has longer period.
  double RelativeUpos = Upos - (Channel%NCHANNEL_PER_WIREPLANE - NCHANNEL_PER_WIREPLANE/2)*CHANNEL_WIDTH;
  // Pass in Pixel -- we want to save u-wire deposit information.
  GenerateSignals(ChannelsToUse, EXOMiscUtil::kUWire, EXOMiscUtil::GetTPCSide(Channel), Channel,
                  RelativeUpos, AbsZpos, Time, Energy, Pixel);
}

void EXODigitizeWires::GenerateSignals(const std::vector<WireToDigitize>& ChannelsToUse,
                                       EXOMiscUtil::EChannelType ChannelType, EXOMiscUtil::ETPCSide TPCSide,
                                       int ReferenceChannel,
                                       double Xpos, double Zpos, double Time, double Energy,
                                       EXOMCPixelatedChargeDeposit* Pixel)
{
  // Trace the deposit given by the variables Xpos, Zpos, Time, Energy.
  // Save into fDdata.
  // Note that the fineness of our time resolution is set by the SAMPLE_TIME_HIGH_BANDWIDTH --
  // we don't concern ourselves with getting details right that only involve fractions of this amount of time.

  if(ChannelType != EXOMiscUtil::kUWire and ChannelType != EXOMiscUtil::kVWire) {
    LogEXOMsg("Invalid channel type", EEAlert);
    return;
  }

  if(Time + fTriggerTime < 0.0) {
    LogEXOMsg("Event occurs before the traces start; skipping", EEWarning);
    return;
  }
  size_t TimeIndex = static_cast<size_t>((Time + fTriggerTime)/SAMPLE_TIME_HIGH_BANDWIDTH);
  if(TimeIndex > fNSample*BANDWIDTH_FACTOR) {
    LogEXOMsg("Event occurs after the traces end; skipping", EEWarning);
    return;
  }

  // The trajectory is currently computed according to:
  // dX = V(z) * unit(E) * dt,
  // where dt = SAMPLE_TIME_HIGH_BANDWIDTH,
  // V(z) = fDriftVelocity           for z >  APDPLANE_UPLANE_DISTANCE + UPLANE_VPLANE_DISTANCE,
  //        fDriftVelocityCollection for z <= APDPLANE_UPLANE_DISTANCE + UPLANE_VPLANE_DISTANCE
  // unit(E) is the normalized electric field vector,
  // dX is the change in Xpos, Zpos.

  // At each step along the way, free charge (drifting electrons)
  // get converted into bound charge (ions -- drift so slowly we consider them bound).
  // The quantity of free charge after a given drift_time is
  // Q_free(drift_time) = Energy * exp(-drift_time/fElectronLifetime)

  // For each step along the trajectory and each signal Sig, we compute a change to the signal:
  // dSig = Q_free * [W(X+dX) - W(X)] = Q_free * dW.

  // We will track Sig in this vector -- indices match indices of ChannelToUse.
  std::vector<double> UnshapedSignal(ChannelsToUse.size(), 0);

  for(size_t i = TimeIndex; i < fNSample*BANDWIDTH_FACTOR; i++) {

    // Get the normalized electric field at this point.
    double ex, ez;
    fEField.GetEField(Xpos, Zpos, ex, ez);
    double emag = std::sqrt(ex*ex + ez*ez);
    if(emag <= 1.0e-4*keV/cm) {
      LogEXOMsg("In a region of very small electric field -- stopping trace", EEWarning);
      return;
    }
    ex /= emag;
    ez /= emag;

    // Step along the trajectory.
    double drift_velocity = fDriftVelocity;
    if(fDriftVelocityCollection > 0.0 and Zpos <= APDPLANE_UPLANE_DISTANCE + UPLANE_VPLANE_DISTANCE) {
      drift_velocity = fDriftVelocityCollection;
    }

    // Simulate diffusion during tracking
    double randomWalkX = 0.;
    double randomWalkZ = 0.;
    if(fDiffusionDuringDrifting){
      // sigma_r = sqrt(2 D_trans t)
      // because the transverse diffusion coefficient is defined in terms of r
      // we need do divide by a factor of sqrt(2) to get the sigma of x (and y)
      // ==> sigma_x = sqrt(D_trans t)
      if(fTransverseDiffusionCoeff > 0.){
        double sigmaX = sqrt(fTransverseDiffusionCoeff * SAMPLE_TIME_HIGH_BANDWIDTH);
        randomWalkX = gRandom->Gaus(0.,sigmaX);
      }
      if(fLongitudinalDiffusionCoeff > 0.){
        double sigmaZ = sqrt(2 * fLongitudinalDiffusionCoeff * SAMPLE_TIME_HIGH_BANDWIDTH);
        randomWalkZ = gRandom->Gaus(0.,sigmaZ);
      }
    }

    double dX = -1.0*ex * drift_velocity * SAMPLE_TIME_HIGH_BANDWIDTH + randomWalkX;
    double dZ = -1.0*ez * drift_velocity * SAMPLE_TIME_HIGH_BANDWIDTH + randomWalkZ;

    // How much free charge is still drifting at this point?
    double Q_free = Energy * std::exp(-1.0 * (i-TimeIndex) * SAMPLE_TIME_HIGH_BANDWIDTH / fElectronLifetime);

    // Check if we've hit a wire.
    // When a wire gets hit, its unshaped signal jumps up to the full amount of charge that deposits, Q_free, and stays there.
    // Other wires have unshaped signals drop down to zero -- ie. don't touch them, they're already correct.
    // Wires are at 0.15*cm, 0.45cm, etc.  We should be efficient and check all with a modulo.
    double Xmod = std::fmod(Xpos + dX, WIRE_PITCH);
    if(Xmod < 0) Xmod += WIRE_PITCH; // because of how fmod handles negative numerators.
    if(Xmod >= WIRE_PITCH) {
      LogEXOMsg("Our remainder prescription seems to have failed -- how?", EEAlert);
      return;
    }
    if(std::sqrt(std::pow(Xmod - WIRE_PITCH/2, 2) +
       std::pow(Zpos + dZ  - APDPLANE_UPLANE_DISTANCE, 2)) < WIRE_DIAMETER/2) {
      // We hit a u-wire.
      if(Pixel) {
        Pixel->fWireHitTime = i*SAMPLE_TIME_HIGH_BANDWIDTH;
        if(ChannelType == EXOMiscUtil::kUWire) {
          // Channel numbers only make sense on the plane being studied.
          Pixel->fDepositChannel = int((Xpos+dX)/CHANNEL_WIDTH) + ReferenceChannel;
          if(Xpos+dX < 0) Pixel->fDepositChannel--;
          if(EXOMiscUtil::TypeOfChannel(Pixel->fDepositChannel) != EXOMiscUtil::TypeOfChannel(ReferenceChannel) or
             not EXOMiscUtil::OnSameDetectorHalf(Pixel->fDepositChannel, ReferenceChannel)) {
            // Use +999 to indicate charge deposited on the other grid.
            Pixel->fDepositChannel = 999;
          }
        }
        else {
          // Use +999 to indicate charge deposited on the other grid.
          Pixel->fDepositChannel = 999;
        }
      }
      // Now fill in the rest of the signals with appropriate values.
      bool HasFoundHitChannel = false; // Make sure we only hit one channel.
      for(size_t isig = 0; isig < UnshapedSignal.size(); isig++) {
        double NextWeightPotential = 0.0; // Unless the charge just deposited in this channel...
        if((not HasFoundHitChannel) and ChannelType == EXOMiscUtil::kUWire and
           std::fabs(Xpos+dX - ChannelsToUse[isig].fPosition) <= CHANNEL_WIDTH/2) {
          NextWeightPotential = 1.0;
          HasFoundHitChannel = true;
        }
        UnshapedSignal[isig] += Q_free * (NextWeightPotential -
          fWeightField.GetWeightPotential(ChannelType, Xpos - ChannelsToUse[isig].fPosition + CHANNEL_WIDTH/2, Zpos)/keV);
        for(size_t j = i; j < fNSample*BANDWIDTH_FACTOR; j++) {
          (*(ChannelsToUse[isig].fWaveform))[j] += UnshapedSignal[isig];
        }
      }
      return; // We've done everything necessary if there was a hit -- now just leave.
    }
    if(std::sqrt(std::pow(Xmod - WIRE_PITCH/2 - fVShift, 2) +
       std::pow(Zpos + dZ  - (APDPLANE_UPLANE_DISTANCE+UPLANE_VPLANE_DISTANCE), 2)) < WIRE_DIAMETER/2) {
      // We hit a v-wire.
      if(Pixel) {
        Pixel->fWireHitTime = i*SAMPLE_TIME_HIGH_BANDWIDTH;
        if(ChannelType == EXOMiscUtil::kVWire) {
          // Channel numbers only make sense on the plane being studied.
          Pixel->fDepositChannel = int((Xpos+dX)/CHANNEL_WIDTH) + ReferenceChannel;
          if(Xpos+dX < 0) Pixel->fDepositChannel--;
          if(EXOMiscUtil::TypeOfChannel(Pixel->fDepositChannel) != EXOMiscUtil::TypeOfChannel(ReferenceChannel) or
             not EXOMiscUtil::OnSameDetectorHalf(Pixel->fDepositChannel, ReferenceChannel)) {
            // Use +999 to indicate charge deposited on the other grid.
            Pixel->fDepositChannel = 999;
          }
        }
        else {
          // Use +999 to indicate charge deposited on the other grid.
          Pixel->fDepositChannel = 999;
        }
      }
      // Now fill in the rest of the signals with appropriate values.
      bool HasFoundHitChannel = false; // Make sure we only hit one channel.
      for(size_t isig = 0; isig < UnshapedSignal.size(); isig++) {
        double NextWeightPotential = 0.0; // Unless the charge just deposited in this channel...
        if((not HasFoundHitChannel) and ChannelType == EXOMiscUtil::kVWire and
           std::fabs(Xpos+dX - ChannelsToUse[isig].fPosition) <= CHANNEL_WIDTH/2) {
          NextWeightPotential = 1.0;
          HasFoundHitChannel = true;
        }
        UnshapedSignal[isig] += Q_free * (NextWeightPotential -
          fWeightField.GetWeightPotential(ChannelType, Xpos - ChannelsToUse[isig].fPosition + CHANNEL_WIDTH/2, Zpos)/keV);
        for(size_t j = i; j < fNSample*BANDWIDTH_FACTOR; j++) {
          (*(ChannelsToUse[isig].fWaveform))[j] += UnshapedSignal[isig];
        }
      }
      return; // We've done everything necessary if there was a hit -- now just leave.
    }
    // Done handling the situation where a wire was hit.
    // If we've reached this point, then no wire was hit -- we don't continue through a hit wire.

    // Generate unshaped signals for this time.
    for(size_t isig = 0; isig < UnshapedSignal.size(); isig++) {
      UnshapedSignal[isig] += Q_free *
        (fWeightField.GetWeightPotential(ChannelType,Xpos+dX - ChannelsToUse[isig].fPosition + CHANNEL_WIDTH/2, Zpos + dZ) -
         fWeightField.GetWeightPotential(ChannelType,Xpos    - ChannelsToUse[isig].fPosition + CHANNEL_WIDTH/2, Zpos     ))/keV;
      (*(ChannelsToUse[isig].fWaveform))[i] += UnshapedSignal[isig];
    }

    // Update Xpos, Zpos
    Xpos += dX;
    Zpos += dZ;

  } // End iteration through time.  We've reached the end of the trace, apparently without hitting a wire.
  // Log a warning noting that we never hit a wire.
  LogEXOMsg("It looks like a waveform ended before charge ever deposited -- bad Efields or too short waveforms?", EEWarning);
}

double EXODigitizeWires::GetScalingOnChannel(const size_t channel) const
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
