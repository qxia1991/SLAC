#include "EXOSim/EXOTrackingActionMessenger.hh"
#include "EXOSim/EXOTrackingAction.hh"


EXOTrackingActionMessenger::EXOTrackingActionMessenger(EXOTrackingAction* ta) :
  fTrackingAction(ta)
{
  // use G4 /tracking directory
  fDirectory = new G4UIdirectory("/tracking/info/");
  fDirectory->SetGuidance("Saving track information control commands.");

  fVerboseLevelCmd = new G4UIcmdWithAnInteger("/tracking/verbose_user", this);
  fVerboseLevelCmd->SetGuidance("Set verbose level for TrackingAction.");
  fVerboseLevelCmd->SetParameterName("verbose_level", true);
  fVerboseLevelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fCacheSelectionCmd = new G4UIcmdWithAString("/tracking/info/cache_mode", this);
  fCacheSelectionCmd->SetGuidance("Set selection flags for TrackingAction cache.");
  fCacheSelectionCmd->SetParameterName("cache_mode", true);
  fCacheSelectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fStoreSelectionCmd = new G4UIcmdWithAString("/tracking/info/store_mode", this);
  fStoreSelectionCmd->SetGuidance("Set selection flags for TrackingAction save.");
  fStoreSelectionCmd->SetParameterName("store_mode", true);
  fStoreSelectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fMinimalEnergyCmd = new G4UIcmdWithADoubleAndUnit("/tracking/info/min_energy", this);
  fMinimalEnergyCmd->SetGuidance("Set min energy for selection in TrackingAction.");
  fMinimalEnergyCmd->SetParameterName("min_energy", true);
  fMinimalEnergyCmd->SetDefaultUnit("keV");
  fMinimalEnergyCmd->SetUnitCategory("Energy");
  fMinimalEnergyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fMaxGenerationCmd = new G4UIcmdWithAnInteger("/tracking/info/max_generation", this);
  fMaxGenerationCmd->SetGuidance("Set max generation number for selectin in TrackingAction.");
  fMaxGenerationCmd->SetParameterName("max_generation", true);
  fMaxGenerationCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fOurVolumeCmd = new G4UIcmdWithAString("/tracking/info/our_volume", this);
  fOurVolumeCmd->SetGuidance("Set volume mane for selection in TrackingAction.");
  fOurVolumeCmd->SetParameterName("volume_name", true);
  fOurVolumeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fDumpStoreCmd = new G4UIcmdWithABool("/tracking/info/dump_stored", this);
  fDumpStoreCmd->SetGuidance("Dump track info for each saved track in TrackingAction.");
  fDumpStoreCmd->SetParameterName("dump", true);
  fDumpStoreCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

void EXOTrackingActionMessenger::SetNewValue(G4UIcommand* command, G4String value)
{
  if (value=="") {
    G4cout << GetCurrentValue(command) << G4endl;
    return;
  }
  if        (command==fVerboseLevelCmd) {
    fTrackingAction->SetVerboseLevel(fVerboseLevelCmd->GetNewIntValue(value));
  } else if (command==fCacheSelectionCmd) {
    fTrackingAction->SetCacheSelection(EXOTrackingAction::SelectionFromString(value));
  } else if (command==fStoreSelectionCmd) {
    fTrackingAction->SetStoreSelection(EXOTrackingAction::SelectionFromString(value));
  } else if (command==fMinimalEnergyCmd) {
    fTrackingAction->SetMinimalEnergy(fMinimalEnergyCmd->GetNewDoubleValue(value));
  } else if (command==fMaxGenerationCmd) {
    fTrackingAction->SetMaxGeneration(fMaxGenerationCmd->GetNewIntValue(value));
  } else if (command==fOurVolumeCmd) {
    fTrackingAction->SetOurVolume(value);
  } else if (command==fDumpStoreCmd) {
    fTrackingAction->SetDumpStore(fDumpStoreCmd->GetNewBoolValue(value));
  }
}

G4String EXOTrackingActionMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String res;
  if        (command==fVerboseLevelCmd) {
    res = fVerboseLevelCmd->ConvertToString(fTrackingAction->GetVerboseLevel());
  } else if (command==fCacheSelectionCmd) {
    res = EXOTrackingAction::SelectionToString(fTrackingAction->GetCacheSelection());
  } else if (command==fStoreSelectionCmd) {
    res = EXOTrackingAction::SelectionToString(fTrackingAction->GetStoreSelection());
  } else if (command==fMinimalEnergyCmd) {
    res = fMinimalEnergyCmd->ConvertToString(fTrackingAction->GetMinimalEnergy(), "keV");
  } else if (command==fMaxGenerationCmd) {
    res = fMinimalEnergyCmd->ConvertToString(fTrackingAction->GetMaxGeneration());
  } else if (command==fOurVolumeCmd) {
    res = fTrackingAction->GetOurVolume();
  } else if (command==fDumpStoreCmd) {
    res = (fTrackingAction->GetDumpStore() ? "true" : "false");
  }
  return res;
}
