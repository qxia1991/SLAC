#ifndef EXODigitizeWires_hh
#define EXODigitizeWires_hh

#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOElectricPotentialReader.hh"
#include "EXOUtilities/EXOWeightPotentialReader.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include <set>
#include <vector>
#include <map>
#include <cstddef> //for size_t

class EXOWaveformData;
class EXOMonteCarloData;
class EXOCoordinates;
class EXOMCPixelatedChargeDeposit;
class EXOElectronics;
class EXOMCChannelScaling;
class EXOTimingStatisticInfo;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class EXODigitizeWires
{
  public:

    EXODigitizeWires();
    
    void Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData);

    void set_electron_lifetime( double value );
    double get_electron_lifetime() const {return fElectronLifetime;}

    void set_drift_velocity(double value);
    double get_drift_velocity() const {return fDriftVelocity;}
    void set_collection_drift_velocity(double value);
    double get_collection_drift_velocity() const {return fDriftVelocityCollection;}

    void set_digitize_induction( bool choice = true)
      { fDigitizeInduction = choice; }
    bool get_digitize_induction() const {return fDigitizeInduction;}

    void set_nsample( size_t nsample );
    void set_trigger_time(double time) 
      { fTriggerTime = time; }
    double get_trigger_time()          
      { return fTriggerTime; }
    size_t get_nsample() 
      { return fNSample; }

    void SetUBaseline(double ubase) { fUBaseline = ubase; }
    void SetVBaseline(double vbase) { fVBaseline = vbase; }
    double GetUBaseline() const { return fUBaseline; }
    double GetVBaseline() const { return fVBaseline; }

    void SetTransverseDiffusionCoeff(double val){
      // Set the transverse diffusion coefficient in mm^2 / ns
      fTransverseDiffusionCoeff = val;
    }
    double GetTransverseDiffusionCoeff() const {
      // Get the transverse diffusion coefficient in mm^2 / ns
      return fTransverseDiffusionCoeff;
    }
    void SetLongitudinalDiffusionCoeff(double val){
      // Set the longitudinal diffusion coefficient in mm^2 / ns
      fLongitudinalDiffusionCoeff = val;
    }
    double GetLongitudinalDiffusionCoeff() const {
      // Get the longitudinal diffusion coefficient in mm^2 / ns
      return fLongitudinalDiffusionCoeff;
    }
    void SetNumDiffusePCDs(double val){fNumDiffusePCDs = val;}
    double GetNumDiffusePCDs() const {return fNumDiffusePCDs;}

    void SetDiffusionDuringDrifting(bool val){
      // Do diffusion during drifting, i.e. a random walk. More accurate than
      // the default which diffuses the PCDs before drifting.
      fDiffusionDuringDrifting = val;
    }
    bool GetDiffusionDuringDrifting() const {return fDiffusionDuringDrifting;}

    void SetVShift(double vshift) { fVShift = vshift; }

    void SetElectronics( const EXOElectronics* elec )
      { fElectronics = elec; }

    void SetScaling( std::map <size_t, double> scale )
      { fScaling = scale; }

    void SetTimingStatisticInfo(EXOTimingStatisticInfo* value);

    size_t get_data_size() const { return fDdata.size(); } 
    const EXODoubleWaveform& get_waveform(size_t i) const
      { return fDdata[i]; }

    void SetElectricFieldFile(const std::string& afile) 
      { if (afile != "") fEField.LoadFieldDataFromFile(afile); }
    void SetUWeightPotentialFile(const std::string& afile) 
      { if (afile != "") fWeightField.LoadUDataFromFile(afile); }
    void SetVWeightPotentialFile(const std::string& afile) 
      { if (afile != "") fWeightField.LoadVDataFromFile(afile); }
  
    void SetNumberDigitizerNeighborVSignals(unsigned int anum)
      { fDigitizeVNeighborSignals = anum; }

    void SetWValueEVperElectron(double w_value) { fWvalue_energy_per_electron = w_value * CLHEP::eV; }
    
    void SetApplyScaling(bool val) { fApplyEmpiricalScaling = val; }

  protected:

    void ResetWires();

    EXOElectricPotentialReader fEField;
    EXOWeightPotentialReader fWeightField;

    struct WireToDigitize {
      // This lets us pass a vector of waveforms to be tracked more easily.
      double fPosition; // Position relative to electrostatics field maps
      EXODoubleWaveform* fWaveform; // High-bandwidth waveform to fill
    };

    void GenerateUnshapedSignals(EXOMCPixelatedChargeDeposit* Pixel);
    void GenerateSignals(const std::vector<WireToDigitize>& ChannelsToUse,
                         EXOMiscUtil::EChannelType ChannelType, EXOMiscUtil::ETPCSide TPCSide,
                         int ReferenceChannel,
                         double Xpos, double Zpos, double Time, double Energy,
                         EXOMCPixelatedChargeDeposit* Pixel = NULL);

    void ScaleAndDigitizeWireSignals(EXOWaveformData& data);
    void AddNoiseToWireSignals();
    void DoADCSamplingWires();
    void ShapeWireSignals();
    double GetScalingOnChannel(const size_t channel) const;

    std::vector<EXODoubleWaveform> fDdata;
    std::vector<EXODoubleWaveform> fSampledData;
 
    double fElectronLifetime;
    double fDriftVelocity;
    double fDriftVelocityCollection;
    double fUBaseline;
    double fVBaseline;
    double fVShift;
    double fWvalue_energy_per_electron; // The W-value can change, eg with field, and this lets us set it differently
                                        // when necessary.  Probably better would be a database table.
    double fTransverseDiffusionCoeff;   // In mm^2 / ns
    double fLongitudinalDiffusionCoeff; // In mm^2 / ns
    double fNumDiffusePCDs;             // Into how many sub-PCDs should each PCD be split for diffusion
    bool fDiffusionDuringDrifting;      // Whether to do diffusion (random walk) during drifting (otherwise do it in advance)
 
    bool fDigitizeInduction;
 
    size_t fNSample;
 
    double fTriggerTime;

    unsigned int fDigitizeVNeighborSignals;

    bool fApplyEmpiricalScaling;

    std::set<int> fWireSignalChannels;  
    std::map <size_t, double> fScaling;

    const EXOElectronics* fElectronics;

    EXOTimingStatisticInfo* fTimingInfo;
};

#endif
