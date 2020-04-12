#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "EXOSim/EXOTrackingKillerMessenger.hh"
#include "EXOSim/EXOTrackingKiller.hh"


EXOTrackingKillerMessenger::EXOTrackingKillerMessenger(EXOTrackingKiller* p) :
  fTrackingKiller(p)
{
  // use G4 /tracking directory
  fKillerDirectory = new G4UIdirectory("/tracking/killer/");
  fKillerDirectory->SetGuidance("Particle killer control commands.");

  fKillerAddCmd = new G4UIcmdWithAString("/tracking/killer/add", this);
  fKillerAddCmd->SetGuidance("Add killer target in format:\n  type [not] <particle> in [not] [deep] <volume> from [not] <process> keV (<|=|>|<=|!=|>=) <energy>");
  fKillerAddCmd->SetParameterName("rule", true);
  fKillerAddCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fKillerListCmd = new G4UIcmdWithoutParameter("/tracking/killer/list", this);
  fKillerListCmd->SetGuidance("List registered particle killer targets.");
  fKillerListCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fKillerClearCmd = new G4UIcmdWithoutParameter("/tracking/killer/clear", this);
  fKillerClearCmd->SetGuidance("Clear registered particle killer targets.");
  fKillerClearCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void EXOTrackingKillerMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if        (command == fKillerAddCmd) {
    fTrackingKiller->AddKiller(newValue);
  } else if (command == fKillerListCmd) {
    G4cout << "List of particle killers:\n" << fTrackingKiller->GetKillerList() << G4endl;
  } else if (command == fKillerClearCmd) {
    G4cout << "Cleared " << fTrackingKiller->GetNumOfKillers() << " particle killers" << G4endl;
    fTrackingKiller->ClearKillers();
  }
}
