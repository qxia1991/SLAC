#ifndef EXOSteppingActionMessenger_h
#define EXOSteppingActionMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

class EXOSteppingAction;

class EXOSteppingActionMessenger: public G4UImessenger
{
  public:
    EXOSteppingActionMessenger(EXOSteppingAction *SA);
    void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    EXOSteppingAction*   SteppingAction;
    G4UIdirectory        stepDirectory;
};

#endif
