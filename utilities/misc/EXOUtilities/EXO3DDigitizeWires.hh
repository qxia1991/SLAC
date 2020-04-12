#ifndef EXO3DDigitizeWires_hh
#define EXO3DDigitizeWires_hh


#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXO3DElectricFieldFinder.hh"
#include "EXOUtilities/EXO3DWeightPotentialFinder.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <cstddef> //for size_t

class EXOWaveformData;
class EXOMonteCarloData;
class EXOCoordinates;
class EXOMCPixelatedChargeDeposit;
class EXOElectronics;
class EXOUWireGains;
class EXOMCChannelScaling;
class EXOTimingStatisticInfo;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
class EXO3DDigitizeWires
{
    public:

        EXO3DDigitizeWires();

        ~EXO3DDigitizeWires();

        void Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData);

        //Same as before
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

        void SetMCExtraScale(double val){
            // Set a constant scaling for all the U-wire signals (Specifically for Low-Field MC)
            fMCExtraScale = val;
        }
        double GetMCExtraScale() const {
            // Get the constant scaling for all the U-wire signals (Specifically for Low-Field MC)
            return fMCExtraScale;
        }

        void SetACSmearFactor(double val){
            fACSmearFactor = val;
        }
        double GetACSmearFactor(){
            return fACSmearFactor;
        }

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


        void SetElectronics( const EXOElectronics* elec )
            { fElectronics = elec; }
        void SetScaling( std::map <size_t, double> scale )
            { fScaling = scale; }
        void SetUGains(std::map <size_t, double> gains)
            {fUGains = gains;}
        void SetVGains(std::map <size_t, double> gains)
            {fVGains = gains;}

        void SetTimingStatisticInfo(EXOTimingStatisticInfo* value);

        size_t get_data_size() const { return fDdata.size(); }

        const EXODoubleWaveform& get_waveform(size_t i) const
            { return fDdata[i]; }

        void SetElectricFieldFile(const std::string& afile)
            { if (afile != "") fEField.LoadFieldDataFromFile(afile); }
        void SetUWeightPotentialFile(const std::string& afile)
            { if (afile != "") fWeightFieldU.LoadUDataFromFile(afile); }
        void SetVWeightPotentialFile(const std::string& afile)
            { if (afile != "") fWeightFieldV.LoadVDataFromFile(afile); }


        void CloseElectricFieldFile()
            { fEField.CloseFieldDataFromFile(); }
        void CloseUWeightPotentialFile()
            { fWeightFieldU.CloseDataFromFile(); }
        void CloseVWeightPotentialFile()
            { fWeightFieldV.CloseDataFromFile(); }    

        void SetWValueEVperElectron(double w_value) { fWvalue_energy_per_electron = w_value * CLHEP::eV; }

        void SetApplyScaling(bool val) { fApplyEmpiricalScaling = val; }
        void SetApplyGainScaling(bool val) { fApplyGainScaling = val; }

        void SetNumberDigitizerNeighborVSignals(unsigned int anum)
            { fDigitizeVNeighborSignals = anum; }
        
        void SetNumberDigitizerNeighborUSignals(unsigned int anum)
            { fDigitizeUNeighborSignals = anum; }


    protected:
        void ResetWires();

        //Changed to the 3D versions
        EXO3DElectricFieldFinder fEField;
        EXO3DWeightPotentialFinder fWeightFieldU;
        EXO3DWeightPotentialFinder fWeightFieldV;
        //end

        struct WireToDigitize {
            //This lets us pass a vector of waveforms to be tracked more easily.
            double fPosition; // Position relative to electrostatics field maps
            EXODoubleWaveform* fWaveform; // High-bandwidth waveform to fill
         };

        void GenerateUnshapedSignals(EXOMCPixelatedChargeDeposit* Pixel);
        
        //Added 3D position in x-y-z plane (need to edit more)
        void GenerateSignals(const std::vector<WireToDigitize>& UChannelsToUse,
                             const std::vector<WireToDigitize>& VChannelsToUse,
                             int URefCh, int VRefCh, double Xpos, double Ypos,double Zpos,
                             double Time, double Energy,
                             EXOMCPixelatedChargeDeposit* Pixel);
        
        
        //End

        void ScaleAndDigitizeWireSignals(EXOWaveformData& data);
        void AddNoiseToWireSignals();
        void DoADCSamplingWires();
        void ShapeWireSignals();
        void PlotWireSignals();

        double GetScalingOnChannel(const size_t channel) const;
        double GetGainOnChannel(const size_t channel) const;

        std::vector<EXODoubleWaveform> fDdata;  //Waveforms for all channels
        std::vector<EXODoubleWaveform> fSampledData; //

        double fElectronLifetime;
        double fDriftVelocity;
        double fDriftVelocityCollection;
        double fUBaseline;
        double fVBaseline;
        double fWvalue_energy_per_electron; // The W-value can change, eg with field, and this lets us set it differently
                                            // when necessary.  Probably better would be a database table.
        double fMCExtraScale;               // Scale Factor (Only Wire Channels)
        double fTransverseDiffusionCoeff;   // In mm^2 / ns
        double fLongitudinalDiffusionCoeff; // In mm^2 / ns
        double fNumDiffusePCDs;             // Into how many sub-PCDs should each PCD be split for diffusion
        bool fDiffusionDuringDrifting;

        bool fDigitizeInduction;
 
        size_t fNSample;

        double fTriggerTime;

        unsigned int fDigitizeVNeighborSignals;
        unsigned int fDigitizeUNeighborSignals;
        
        int fRefCh;

        bool fApplyEmpiricalScaling;
        bool fApplyGainScaling;

        std::set<int> fWireSignalChannels; //Channel #s to use
        std::map <size_t, double> fScaling;
        std::map <size_t, double> fUGains;
        std::map <size_t, double> fVGains;

        const EXOElectronics* fElectronics;

        EXOTimingStatisticInfo* fTimingInfo;
        double fACSmearFactor;             //  Event leveling smearing of charge signal passed from Digitize Module

        //given a upos or vpos find closest wire and relative position to that wire
        void GetClosestChannel(double Xpos, double Ypos, double Zpos, int &Uch, int &Vch, double& xrel, double& yrel);
       
};

#endif
