#include "EXOSim/EXOSteppingActionMessenger.hh"
#include "EXOSim/EXOSteppingAction.hh"


EXOSteppingActionMessenger::EXOSteppingActionMessenger(EXOSteppingAction* SA) :
  SteppingAction(SA),
  stepDirectory("/step/")
{
  stepDirectory.SetGuidance("Step draw control command.");

}

void EXOSteppingActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
}
