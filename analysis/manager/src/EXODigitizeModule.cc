//______________________________________________________________________________
//
// EXODigitizeModule
//
//
// Module that takes an input EXOEventData object with filled monte carlo data
// and fills the waveform data of that object.  This is specifically to allow
// MC data to have digitization re-run on top of it after updates to digitizer
// have been performed.
//
// A note about sample times:  EXOEventAction as of the writing of this module
// fills the variable EXOMonteCarloData::fEventGroupingTime with the time it
// uses to group the events.  The digitize will *try* to use this value, unless
// it's 0 (meaning it was never set, or it's old data) or if
// SetDigitizationTime (directly or via /digitizer/setDigitizationTime) was
// called with to define the time. 
//
// Written September 2011, M. Marino
//______________________________________________________________________________
#include "EXOAnalysisManager/EXODigitizeModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOAddNoise.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOVWireGains.hh"
#include "TRandom.h"
#include <sstream>
#include <cmath>
using CLHEP::microsecond;
using CLHEP::cm;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXODigitizeModule, "digitizer" )

EXODigitizeModule::EXODigitizeModule() : 
  fForceUseInternalSamples(false),
  fNumSamples(512),
  fWireNoiseMagnitude(0.0),
  fAPDNoiseMagnitude(0.0),
  fUnixTimeOfEvent(0),
  fElectronicsDatabaseFlavor("vanilla"),
  fDoIDigitizeAPDs(true),
  fDoIDigitizeWires(true),
  fMCScalingDatabaseFlavor("vanilla"),
  fUWireDatabaseFlavor("source_calibration"),
  fVWireDatabaseFlavor("vanilla"),
  fACSmearSigma(0.0)  
{
  fTimingInfo.SetName("DigitizerStatistics");
  RegisterSharedObject(fTimingInfo.GetName(), fTimingInfo);
  fSmearMCIonizationEnergy.SetLXeEnergyResolution(0.02);
  fDigWires.set_electron_lifetime(CLHEP::second);
  fDigWires.set_drift_velocity(2.8*CLHEP::mm/CLHEP::microsecond);
  fDigWires.set_collection_drift_velocity(0.0*cm/microsecond);
  fDigWires.set_digitize_induction(true);
  SetTriggerTime(TRIGGER_TIME/microsecond);
}

//______________________________________________________________________________
EXODigitizeModule::~EXODigitizeModule()
{
  RetractObject(fTimingInfo.GetName());
}

//______________________________________________________________________________
int EXODigitizeModule::Initialize()
{

  // fDigAPDs.SetTimingStatisticInfo(&fTimingInfo); I haven't bothered to produce timing info within EXODigitizeAPDs.
  fDigWires.SetTimingStatisticInfo(&fTimingInfo);
  fTimingInfo.Clear();
  return 0;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXODigitizeModule::BeginOfRun(EXOEventData *ED)
{
  if (fUnixTimeOfEvent != 0) {
    // Set the trigger time to use if the user has requested it.
    ED->fEventHeader.fTriggerSeconds = fUnixTimeOfEvent;
    ED->fEventHeader.fTriggerMicroSeconds = 0;
  }
  return kOk;
}
//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXODigitizeModule::ProcessEvent(EXOEventData *ED)
{
  EXOMonteCarloData& mc = ED->fMonteCarloData;

  fTimingInfo.Reset();

  // Smear the ionization energies to account for LXe energy resolution.
  fTimingInfo.StartTimerForTag("SmearMCIonizationEnergy");
  fSmearMCIonizationEnergy.ApplySmear(mc);
  fTimingInfo.StopTimerForTag("SmearMCIonizationEnergy");

  if (fForceUseInternalSamples or
      mc.fEventGroupingTime == 0.0) {
    fDigWires.set_nsample(fNumSamples);
    fDigAPDs.set_nsample(fNumSamples);
    ED->fEventHeader.fSampleCount = fNumSamples - 1;
  } else {
    int the_samples = (int)(mc.fEventGroupingTime/SAMPLE_TIME);
    fDigWires.set_nsample(the_samples);
    fDigAPDs.set_nsample(the_samples);
    ED->fEventHeader.fSampleCount = the_samples - 1;
  } 
  if (fUnixTimeOfEvent != 0) {
    // Set the trigger time to use if the user has requested it.
    ED->fEventHeader.fTriggerSeconds = fUnixTimeOfEvent;
    ED->fEventHeader.fTriggerMicroSeconds = 0;
  }
  EXOElectronicsShapers* electronicsShapers = GetCalibrationFor(EXOElectronicsShapers, 
                                                                EXOElectronicsShapersHandler, 
                                                                fElectronicsDatabaseFlavor, 
                                                                ED->fEventHeader);
  if ( not electronicsShapers ) {
    LogEXOMsg("Unable to get shaper information from DB", EEError);
    return kDrop;
  } 

  EXOMCChannelScaling* ScalingFromDatabase = GetCalibrationFor(EXOMCChannelScaling, 
							       EXOMCChannelScalingHandler, 
							       fMCScalingDatabaseFlavor, 
							       ED->fEventHeader);
  
  if ( not ScalingFromDatabase ) {
    LogEXOMsg("Unable to get channel scaling information from DB", EEError);
    return kDrop;
  } 

  EXOUWireGains* UGainsFromDatabase = GetCalibrationFor(EXOUWireGains,
                                                        EXOUWireGainsHandler,
                                                        fUWireDatabaseFlavor,
                                                        ED->fEventHeader);
  if ( not UGainsFromDatabase) {
    LogEXOMsg("Unable to get U channel gain information from DB", EEError);
    return kDrop;
  }

  EXOVWireGains* VGainsFromDatabase = GetCalibrationFor(EXOVWireGains,
                                                        EXOVWireGainsHandler,
                                                        fVWireDatabaseFlavor,
                                                        ED->fEventHeader);
  if ( not VGainsFromDatabase) {
    LogEXOMsg("Unable to get V channel gain information from DB", EEError);
    return kDrop;
  }
  
  ED->GetWaveformData()->Clear();
  
  //If adding in by hand smearing
  double ACSmearFactor = 0.0;
  double smear_energy = mc.fTotalEnergyInLiquidXe/1000.; // LXe energy in MeV
  double fACSmearSigma_formula =  0;
  double fACTanTheta           =  0;

  if (fACSmearSigma>0){
    if (fUnixTimeOfEvent>0 && fUnixTimeOfEvent<1451700000) {
       //Phase-1
       double p0p = 9.51; double p0t = 0.967;
       double p1p = 1.65; double p1t = 0.779;
       double p2p = 3.20; double p2t = 1.664;
       
       fACSmearSigma_formula = p0p*exp(-p1p*smear_energy) + p2p; // probability of recombination in charge channel [in %]
       fACTanTheta = p0t*exp(-p1t*smear_energy) + p2t; // correction for scint channel  = 1/tan(theta)
       ACSmearFactor =  gRandom->Gaus(0.0, fACSmearSigma_formula/100.0);
    }
    else if(fUnixTimeOfEvent>0 && fUnixTimeOfEvent>1451700000){
        //Phase-2
        double p0p = 6.49; double p0t = 2.07;
        double p1p = 1.30; double p1t = 1.50;
        double p2p = 2.62; double p2t = 1.68;

        fACSmearSigma_formula = p0p*exp(-p1p*smear_energy) + p2p; // probability of recombination in charge channel [in %]
        fACTanTheta = p0t*exp(-p1t*smear_energy) + p2t; // correction for scint channel  = 1/tan(theta)
        ACSmearFactor =  gRandom->Gaus(0.0, fACSmearSigma_formula/100.0);
    }
    //std::cout<< "Time Stamp " << fUnixTimeOfEvent << " energy " << smear_energy << " formula " << fACSmearSigma_formula << " tan " << fACTanTheta << std::endl;
  }


  if(fDoIDigitizeAPDs) {
    electronicsShapers->SetNoiseAmplitudeForAPDs(fAPDNoiseMagnitude*ADC_BITS/APD_ADC_FULL_SCALE_ELECTRONS);
    fDigAPDs.SetElectronics(electronicsShapers);
    fDigAPDs.SetScaling(ScalingFromDatabase->GetScalingChannelMap());
    fDigAPDs.SetACSmearFactor(ACSmearFactor*fACTanTheta);
    fTimingInfo.StartTimerForTag("DigitizeAPDs");
    fDigAPDs.Digitize(*ED->GetWaveformData(), ED->fMonteCarloData);
    fTimingInfo.StopTimerForTag("DigitizeAPDs");
  }

  if(fDoIDigitizeWires) {
    electronicsShapers->SetNoiseAmplitudeForWires(fWireNoiseMagnitude*W_VALUE_LXE_EV_PER_ELECTRON);
    fDigWires.SetElectronics(electronicsShapers);
    fDigWires.SetScaling(ScalingFromDatabase->GetScalingChannelMap());
    fDigWires.SetUGains(UGainsFromDatabase->GetGainsChannelMap());
    fDigWires.SetVGains(VGainsFromDatabase->GetGainsChannelMap());
    fDigWires.SetACSmearFactor(ACSmearFactor);
    fTimingInfo.StartTimerForTag("DigitizeWires");
    fDigWires.Digitize(*ED->GetWaveformData(), ED->fMonteCarloData);
    fTimingInfo.StopTimerForTag("DigitizeWires");
  }

  // Trim any saturated waveforms.
  fTimingInfo.StartTimerForTag("TrimSaturatedSignals");
  fTrimWaveforms.TrimWaveforms(*ED->GetWaveformData());
  fTimingInfo.StopTimerForTag("TrimSaturatedSignals");

  ED->fEventHeader.fFrameRevision = 0;
  if(fDigWires.get_trigger_time() != fDigAPDs.get_trigger_time()) LogEXOMsg("Trigger times are mismatched", EEAlert);
  ED->fEventHeader.fTriggerOffset = (int)(fDigWires.get_trigger_time()/SAMPLE_TIME);
  if(ED->GetWaveformData()->GetNumWaveforms() > 0) {
    ED->GetWaveformData()->fNumSamples = ED->GetWaveformData()->GetWaveform(0)->GetLength();
    for(size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++) {
      if((Int_t)ED->GetWaveformData()->GetWaveform(i)->GetLength() != ED->GetWaveformData()->fNumSamples) {
        LogEXOMsg("The lengths of waveforms are mismatched", EEAlert);
      }
    }
  }
  return kOk;
}

//______________________________________________________________________________
int EXODigitizeModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/digitizer/wireNoise",
           "RMS electrons of noise to be added to the wire signals", 
           this, fWireNoiseMagnitude, &EXODigitizeModule::SetWireNoise);

  talktoManager->CreateCommand("/digitizer/APDNoise",
           "RMS electrons of noise to be added to the APD signals", 
           this, fAPDNoiseMagnitude, &EXODigitizeModule::SetAPDNoise);

  talktoManager->CreateCommand("/digitizer/LXeEnergyRes",
           "LXe energy resolution constant",
           this, fSmearMCIonizationEnergy.GetLXeEnergyResolution(), &EXODigitizeModule::SetLXeEnergyResolution);

  talktoManager->CreateCommand("/digitizer/electronLifetime",
           "Set the free electron lifetime, in units of microseconds",
           this, fDigWires.get_electron_lifetime()/CLHEP::microsecond, &EXODigitizeModule::SetElectronLifetime);

  talktoManager->CreateCommand("/digitizer/driftVelocity",
           "Set the drift velocity, in units of cm/microseconds",
           this, fDigWires.get_drift_velocity()/(CLHEP::cm/CLHEP::microsecond), &EXODigitizeModule::SetDriftVelocity);

  talktoManager->CreateCommand("/digitizer/collectionDriftVelocity",
           "Set the collection drift velocity (once the charge has passed v wires) (0.0 to disable)",
           this, fDigWires.get_collection_drift_velocity()/(CLHEP::cm/CLHEP::microsecond),
                 &EXODigitizeModule::SetCollectionDriftVelocity);

  talktoManager->CreateCommand("/digitizer/digitizeInduction",
           "Choose whether to digitize induction signals on neighbouring channels",
           this, fDigWires.get_digitize_induction(), &EXODigitizeModule::SetDigitizeInduction);

  talktoManager->CreateCommand("/digitizer/digitizeWires",
           "Choose whether to digitize wires",
           this, fDoIDigitizeWires, &EXODigitizeModule::SetDigitizeWires);

  talktoManager->CreateCommand("/digitizer/digitizeAPDs",
           "Choose whether to digitize APDs",
           this, fDoIDigitizeAPDs, &EXODigitizeModule::SetDigitizeAPDs);

  talktoManager->CreateCommand("/digitizer/setTriggerTime",
           "Time at which event is triggered, in microseconds",
           this, fTriggerTime/microsecond, &EXODigitizeModule::SetTriggerTime);

  talktoManager->CreateCommand("/digitizer/setDigitizationTime",
           "Digitization time, in microseconds",
           this, fNumSamples*microsecond/SAMPLE_TIME, &EXODigitizeModule::SetDigitizationTime);

  talktoManager->CreateCommand("/digitizer/setDatabaseTime",
           "Sets the time (unix/epoch time, in seconds) of the events." 
           "  Digitizer will use this to query the database.  0 (default) to reset.",
           this, fUnixTimeOfEvent, &EXODigitizeModule::SetUnixTimeOfEvent);

  talktoManager->CreateCommand("/digitizer/ElectronicsDBFlavor",
           "Set the flavor string used to query the database for electronics shaping times",
           this,
           fElectronicsDatabaseFlavor,
           &EXODigitizeModule::SetElectronicDatabaseFlavor );

  talktoManager->CreateCommand("/digitizer/setWeightPotentialFiles",
           "Sets the files to be used for weight potential.  "
           "[uwire_file] [vwire_file]  ."
           "This also likely requires setting V shift!  ",
           this, "", &EXODigitizeModule::SetWeightPotentialFiles);

  talktoManager->CreateCommand("/digitizer/setElectricFieldFile",
           "Sets the file to be used for the drift field.  "
           "[efield_file]",
           this, "", &EXODigitizeModule::SetElectricFieldFile);

  talktoManager->CreateCommand("/digitizer/setNumberDigitizedVWireNeighbors",
           "Sets number of v wire neighbors to digitize.  "
           "This *should* be used with appropriate bin files.",
           &fDigWires, (unsigned int)1, &EXO3DDigitizeWires::SetNumberDigitizerNeighborVSignals);


  talktoManager->CreateCommand("/digitizer/setNumberDigitizedUWireNeighbors",
          "Sets number of u wire neighbors to digitize.  "
          "This *should* be used with appropriate bin files.",
          &fDigWires, (unsigned int)1, &EXO3DDigitizeWires::SetNumberDigitizerNeighborUSignals);

  talktoManager->CreateCommand("/digitizer/setWValue_ev_per_electron",
           "Sets a non-default w-value to use within the digitizer.",
           &fDigWires, W_VALUE_LXE_EV_PER_ELECTRON/CLHEP::eV, &EXO3DDigitizeWires::SetWValueEVperElectron);

  talktoManager->CreateCommand("/digitizer/setManualYieldFactor",
           "Scales photon yield by the given factor (should normally be left at 1).",
           &fDigAPDs, 1., &EXODigitizeAPDs::set_manualyieldfactor);

  talktoManager->CreateCommand("/digitizer/applyEmpiricalScalingAPDs",
           "Scale the APDs by an emprical factor to match data",
           &fDigAPDs, true, &EXODigitizeAPDs::set_applyscaling);

  talktoManager->CreateCommand("/digitizer/applyEmpiricalScalingWires",
           "Scale the U/V wires by an emprical factor to match data",
           &fDigWires, true, &EXO3DDigitizeWires::SetApplyScaling);
    
  talktoManager->CreateCommand("/digitizer/applyGainScalingWires",
            "Scale the U/V wires by the gains in the Databse.",
            &fDigWires, false, &EXO3DDigitizeWires::SetApplyGainScaling);

  talktoManager->CreateCommand("/digitizer/MCScalingDBFlavor",
            "Set the flavor string used to query the database for MC scaling factors",
            this,
            fMCScalingDatabaseFlavor,
            &EXODigitizeModule::SetMCScalingDatabaseFlavor );

  talktoManager->CreateCommand("/digitizer/MCExtraScale",
          "Set a constant scaling for all the U-wire signals (Specifically for Low-Field MC)",
          &fDigWires, fDigWires.GetMCExtraScale(), &EXO3DDigitizeWires::SetMCExtraScale);
  
  talktoManager->CreateCommand("/digitizer/transDiffusionCoeff",
           "Set the transverse diffusion coefficient (mm^2 / nanosec)",
           &fDigWires, fDigWires.GetTransverseDiffusionCoeff(), &EXO3DDigitizeWires::SetTransverseDiffusionCoeff);
 
  talktoManager->CreateCommand("/digitizer/longDiffusionCoeff",
           "Set the longitudinal diffusion coefficient (mm^2 / nanosec)",
           &fDigWires, fDigWires.GetLongitudinalDiffusionCoeff(), &EXO3DDigitizeWires::SetLongitudinalDiffusionCoeff);
 
  talktoManager->CreateCommand("/digitizer/numdiffusePCDs",
           "Set into how many parts each PCD should be split. Set <= 1 to turn off",
           &fDigWires, fDigWires.GetNumDiffusePCDs(), &EXO3DDigitizeWires::SetNumDiffusePCDs);
 
  talktoManager->CreateCommand("/digitizer/diffuseDuringDrift",
           "do diffusion (random walk) while drifting (instead of before drifting)",
          &fDigWires, fDigWires.GetDiffusionDuringDrifting(), &EXO3DDigitizeWires::SetDiffusionDuringDrifting);

  talktoManager->CreateCommand("/digitizer/UWireGainDatabaseFlavor",
          "Set the flavor string used to query the database for U-Wire gains used to scale MC WFs.",
          this,
          fUWireDatabaseFlavor,
          &EXODigitizeModule::SetUWireDatabaseFlavor);
 
  talktoManager->CreateCommand("/digitizer/ACSmearSigma",
         "Set the smearing factor to smear the charge/light signals. This is sigma used to sample the per event flucuation.",
         this,
         fACSmearSigma,
         &EXODigitizeModule::SetACSmearSigma);

  talktoManager->CreateCommand("/digitizer/VWireGainDatabaseFlavor",
         "Set the flavor string used to query the database for V-Wire gains used to scale MC WFs.",
         this,
         fVWireDatabaseFlavor,
         &EXODigitizeModule::SetVWireDatabaseFlavor);

  return 0;
}

//______________________________________________________________________________
void EXODigitizeModule::SetWireNoise(double noise)
{
  // Set the Wire Noise in units of Electron RMS
  fWireNoiseMagnitude = noise;
}
//______________________________________________________________________________
void EXODigitizeModule::SetAPDNoise(double noise)
{
  // Set the APD Noise in units of Electron RMS
  fAPDNoiseMagnitude = noise;
}

//______________________________________________________________________________
void EXODigitizeModule::SetLXeEnergyResolution(double res)
{
  // Set energy resoluction of LXe
  fSmearMCIonizationEnergy.SetLXeEnergyResolution(res);
}

//______________________________________________________________________________
void EXODigitizeModule::SetElectronLifetime( double lifetime)
{
  // Set the electron livetime, input in units of microseconds 
  fDigWires.set_electron_lifetime(lifetime*microsecond);
}

//______________________________________________________________________________
void EXODigitizeModule::SetDriftVelocity( double drift_vel)
{
  // Set the drift velocity, input in cm/mus
  fDigWires.set_drift_velocity(drift_vel*cm/microsecond);
}
//______________________________________________________________________________
void EXODigitizeModule::SetCollectionDriftVelocity( double drift_vel)
{
  // Set the collection drift velocity (betwen v and u-wires, input in cm/mus
  fDigWires.set_collection_drift_velocity(drift_vel*cm/microsecond);
}


//______________________________________________________________________________
void EXODigitizeModule::SetDigitizeInduction( bool dig_induction)
{
  // Set to digitize the induction signals, this is mainly for debugging
  // purposes.
  fDigWires.set_digitize_induction(dig_induction);
}

//______________________________________________________________________________
void EXODigitizeModule::SetTriggerTime( double trig_time)
{
  // Set the trigger time, input in microseconds
  fTriggerTime = trig_time*microsecond;
  fDigWires.set_trigger_time(fTriggerTime);
  fDigAPDs.set_trigger_time(fTriggerTime);
}
//______________________________________________________________________________
void EXODigitizeModule::SetDigitizeAPDs(bool apds)
{
  // Set whether to digitize the APDs
  fDoIDigitizeAPDs = apds;
}
//______________________________________________________________________________
void EXODigitizeModule::SetDigitizeWires(bool wires)
{
  // Set whether to digitize the wires
  fDoIDigitizeWires = wires;
}

//______________________________________________________________________________
void EXODigitizeModule::SetDigitizationTime(double digi_time)
{
  // Set digitization time, in microseconds.  This will force the digitizer to
  // use this time and not anything from the input EXOMonteCarloData.
  int the_samples = (int)(digi_time*microsecond/SAMPLE_TIME);

  // if we are the same, don't bother changing anything
  if (fNumSamples == the_samples) return; 
  fNumSamples = the_samples;
  fForceUseInternalSamples = true;
}

//______________________________________________________________________________
void EXODigitizeModule::SetWeightPotentialFiles(std::string files)
{
  // Takes a space-separated string, e.g. "uwire_file vwire_file" to set the
  // weight potential files in the wire digitizer.
  if (files == "") return;
  std::string uwire_file;
  std::string vwire_file;
  std::istringstream is(files);
  is >> uwire_file >> vwire_file;
  if (is.fail()) {
    LogEXOMsg("Improper formatting: " + files, EEError);
    return;
  }
  fDigWires.SetUWeightPotentialFile(uwire_file);
  fDigWires.SetVWeightPotentialFile(vwire_file);
}

//______________________________________________________________________________
void EXODigitizeModule::SetElectricFieldFile(std::string afile)
{
  // Set the file to be used for the dift field
  if (afile == "") return;
  fDigWires.SetElectricFieldFile(afile);
}

