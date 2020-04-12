#ifndef EXOTrackingActionMessenger_hh
#define EXOTrackingActionMessenger_hh

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

class EXOTrackingAction;

class EXOTrackingActionMessenger: public G4UImessenger
{
public:
  EXOTrackingActionMessenger(EXOTrackingAction *TA);
  void SetNewValue(G4UIcommand* command, G4String value);
  G4String GetCurrentValue(G4UIcommand* command);

private:
  EXOTrackingAction *fTrackingAction;
  G4UIdirectory *fDirectory;
  G4UIcmdWithAnInteger *fVerboseLevelCmd;
  G4UIcmdWithAString *fCacheSelectionCmd;
  G4UIcmdWithAString *fStoreSelectionCmd;
  G4UIcmdWithADoubleAndUnit *fMinimalEnergyCmd;
  G4UIcmdWithAnInteger *fMaxGenerationCmd;
  G4UIcmdWithAString *fOurVolumeCmd;
  G4UIcmdWithABool *fDumpStoreCmd;
};

#endif /* EXOTrackingActionMessenger_hh */
