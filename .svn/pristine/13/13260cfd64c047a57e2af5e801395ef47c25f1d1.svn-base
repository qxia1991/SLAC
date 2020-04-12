#ifndef EXOTrackingKillerMessenger_hh
#define EXOTrackingKillerMessenger_hh

#include "G4UImessenger.hh"

class EXOTrackingKiller;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

class EXOTrackingKillerMessenger: public G4UImessenger
{
  public:
    EXOTrackingKillerMessenger(EXOTrackingKiller *p);
    void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    EXOTrackingKiller*   fTrackingKiller;
    G4UIdirectory*       fKillerDirectory;
    G4UIcmdWithAString*  fKillerAddCmd;
    G4UIcmdWithoutParameter* fKillerListCmd;
    G4UIcmdWithoutParameter* fKillerClearCmd;
};

#endif
