//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef EXOEventActionMessenger_h
#define EXOEventActionMessenger_h

#include "G4UImessenger.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class EXOEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class EXOEventActionMessenger: public G4UImessenger
{
  public:
    EXOEventActionMessenger(EXOEventAction*);
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    EXOEventAction* eventAction;   
    G4UIcmdWithABool  AnticorrelationACmd;       
    G4UIcmdWithABool  LXeEventsOnly;
    G4UIcmdWithABool  VetoPanelEventsOnly;
    G4UIcmdWithABool  LXeOrVetoPanelEventsOnly;
    G4UIcmdWithABool  LXeAndVetoPanelEventsOnly;
    G4UIcmdWithABool  PrintParticleInfo;
    G4UIcmdWithABool  SaveParticleInfo;
    G4UIcmdWithADoubleAndUnit  TotalEventWindowTimeCmd;
    G4UIcmdWithAnInteger  UnixTimeOfEventCmd;

};

#endif
