#ifndef EXORunActionMessenger_h
#define EXORunActionMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"

class EXORunAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class EXORunActionMessenger: public G4UImessenger
{
  public:
    EXORunActionMessenger(EXORunAction*);
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    EXORunAction*   EXORun;

    G4UIcmdWithABool  KillLowEnergyCmd;    
    G4UIcmdWithABool  AnticorrAlphaCmd;  
    G4UIcmdWithABool  FastScintAlphaCmd;  
    G4UIcmdWithADouble  AlphaFieldCmd;  
    G4UIcmdWithADouble  NexAlphaCmd;  

};

#endif

