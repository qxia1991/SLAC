/*
EXOVWireSignalModelBuilder

Builds a model waveform for a v-wire by plugging into the same code that generates v-wire signals in MC.
*/
#include "EXOReconstruction/EXOVWireSignalModelBuilder.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOElectronics.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXOCoordinates.hh"
#include "EXOUtilities/EXODigitizeWires.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
//#include "EXOUtilities/EXO3DDigitizeWires.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXOWaveformData.hh"

//______________________________________________________________________________
bool EXOVWireSignalModelBuilder::InitializationIsNeeded(const EXOSignalModel& model) const
{
  // Returns whether or not initialization with the given signal model is
  // necessary.  Only checks if the transfer functions of the given
  // EXOSignalModel and in this Builder are equivalent.
  // FixME:  should also be able to check drift velocities, in case they've changed.
  return (model.GetTransferFunction() != fTransferFunction);
}

void EXOVWireSignalModelBuilder::InitializeSignalModel(
    EXODoubleWaveform& shapeModel, 
    EXOTransferFunction& transfer) const
{
  // Plug into EXODigitizeWires and create a v-wire signal.

  // Save the transfer function
  transfer = fTransferFunction;

  // Make an EXOElectronics object.
  // We'll make every channel have the same transfer function, for now.
  // (Since we only care about the shaped waveform on the channel being built.)
  // This may need to be reconsidered when we move to 3D fields -- nothing should break, but it could probably be done better.
  EXOElectronics electronics;
  for(size_t i = 0; i < NUMBER_READOUT_CHANNELS; i++) {
    electronics.SetTransferFunctionForChannel(i, transfer);
  }
  electronics.SetNoiseAmplitudeForWires(0.0);
  electronics.SetNoiseAmplitudeForAPDs(0.0);

  // Create the EXOMonteCarloData object
  // This position is selected because it mimics the old behavior well,
  // but it also seems to mimic an averaging of v-wire signals (at many positions) from data.
  // It is probably possible to get better position information by looking at the variations, sometimes large,
  // in v-wire signal shape as position changes.
  // We create two coordinates (separated in z) to mimic a spread in z-coordinate -- 
  // if the spread is zero, they'll just add to each other like one pixel.
  EXOCoordinates coord1(EXOMiscUtil::kXYCoordinates,
                        0.5*CHANNEL_WIDTH, // directly over a u-wire
                        0.0*CHANNEL_WIDTH,
                        0.5*(CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE) + fZ_Separation/2, // half-way btwn u-wires and cathode
                        0.0*CLHEP::microsecond);
  EXOCoordinates coord2(EXOMiscUtil::kXYCoordinates,
                        0.5*CHANNEL_WIDTH, // directly over a u-wire
                        0.0*CHANNEL_WIDTH,
                        0.5*(CATHODE_APDFACE_DISTANCE - APDPLANE_UPLANE_DISTANCE) - fZ_Separation/2, // half-way btwn u-wires and cathode
                        0.0*CLHEP::microsecond);
  EXOMonteCarloData mcData;
  EXOMCPixelatedChargeDeposit* pixel1 = mcData.FindOrCreatePixelatedChargeDeposit(coord1);
  pixel1->fTotalEnergy += 100.0*CLHEP::MeV;                // Doesn't really matter, since we won't trim saturated signals.
  pixel1->fTotalIonizationEnergy += pixel1->fTotalEnergy;
  EXOMCPixelatedChargeDeposit* pixel2 = mcData.FindOrCreatePixelatedChargeDeposit(coord2);
  pixel2->fTotalEnergy += 100.0*CLHEP::MeV;                // Doesn't really matter, since we won't trim saturated signals.
  pixel2->fTotalIonizationEnergy += pixel2->fTotalEnergy;

  EXODigitizeWires dig;
  //EXO3DDigitizeWires dig;
  dig.set_electron_lifetime(10.0*CLHEP::second); // very long
  dig.set_drift_velocity(fDriftVelocity);
  dig.set_collection_drift_velocity(fCollectionDriftVelocity);
  dig.set_nsample(2048);
  dig.set_trigger_time(1024.0*CLHEP::microsecond);
  dig.SetUBaseline(0.0);
  dig.SetVBaseline(0.0);
  dig.SetElectronics(&electronics);
  dig.SetApplyScaling( false ); // Don't need to apply the scalings when generating
                                // the V-wire signal model since they're normalized out anyway

  EXOWaveformData wfData;
  dig.Digitize(wfData, mcData);
  const EXOWaveform& wf = *wfData.GetWaveformWithChannel(57); // Channel 57 is the one nearest to the point created above.
  // What feature should we use to normalize the template function?
  // Normally, we expect that the most distinctive feature (ie the one most strongly relied on by the finder)
  // will be the peak-to-trough drop, which should always be roughly the same.
  // So, that's the feature we should use to normalize the template function;
  // that way, the threshold used in the database should be as constant as possible when the template function changes.
  int PeakToPeak = wf.GetMaxValue() - wf.GetMinValue();
  double WireHitTime = (pixel1->fWireHitTime+pixel2->fWireHitTime)/2;

  //shapeModel.SetLength(93);
  shapeModel.SetLength(300);
  shapeModel.SetSamplingPeriod(CLHEP::microsecond);
  //shapeModel.SetTOffset(-28.0*CLHEP::microsecond);
  shapeModel.SetTOffset(-70.0*CLHEP::microsecond);
  for(size_t i = 0; i < shapeModel.GetLength(); i++) {
    shapeModel[i] = double(wf.InterpolateAtPoint(WireHitTime + shapeModel.GetTimeAtIndex(i)))/PeakToPeak;
  }
}
