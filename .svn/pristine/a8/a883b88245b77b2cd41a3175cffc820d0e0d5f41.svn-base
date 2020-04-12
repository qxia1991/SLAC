#ifndef EXODigitizeAPDs_hh
#define EXODigitizeAPDs_hh

#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include <vector>
#include <set>

class EXOWaveformData;
class EXOMonteCarloData;
class EXOElectronics;
class EXOMCChannelScaling;

class EXODigitizeAPDs
{
public:

  EXODigitizeAPDs();

  void Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData);

  void set_trigger_time(double time) { trigger_time = time; }
  double get_trigger_time() { return trigger_time; }

  void set_nsample( size_t nsample );
  size_t get_nsample() { return nsample; }

  void set_manualyieldfactor(double factor) { fManualYieldFactor = factor; }
  void set_applyscaling(bool val) { fApplyEmpiricalScaling = val; }

  void SetElectronics( const EXOElectronics* elec ) { fElectronics = elec; }
  void SetScaling( std::map <size_t, double> scale ) { fScaling = scale; }
  void SetACSmearFactor(double val){fACSmearFactor = val;}


private:
  double trigger_time;
  size_t nsample;
  double fManualYieldFactor; // energy->photons conversion is set in EXOEventAction, unfortunately; this factor lets me scale it in the digitizer without regenerating monte carlo.  By default this must be 1.

  // fDdata and fSampledData are indexed by the gang number, ie software channel - NWIREPLANE*NCHANNEL_PER_WIREPLANE
  std::vector<EXODoubleWaveform> fDdata;
  std::vector<EXODoubleWaveform> fSampledData;
  // fAPDSignalChannels is a set of gang numbers.
  std::set<int> fAPDSignalChannels;

  const EXOElectronics* fElectronics;
  std::map <size_t, double> fScaling;

  bool fApplyEmpiricalScaling;
  double fACSmearFactor;

  void ResetAPDs();
  void AddAPDHitToSignals(Int_t GangNo, Double_t Time, Double_t Magnitude);
  void ShapeAPDSignals();
  void DoADCSamplingAPDs();
  void ApplyAPDGain();
  void AddNoiseToAPDSignals();
  void ScaleAndDigitizeAPDSignals(EXOWaveformData& data);
  double GetScalingOnChannel(const size_t channel) const;

};
#endif
