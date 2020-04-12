#ifndef EXOPhysicsListMessenger_hh
#define EXOPhysicsListMessenger_hh 1

#include <G4UImessenger.hh>
//#include "EXOPhysicsList.hh"

//namespace EXO {

  class EXOPhysicsList;
  class G4UIdirectory;
  class G4UIcmdWithoutParameter;
  class G4UIcmdWithAnInteger;
  class G4UIcmdWithADoubleAndUnit;
  class G4UIcmdWithAString; 
  //class G4UIcommand;

  class EXOPhysicsListMessenger: public G4UImessenger
  {
  public:
    EXOPhysicsListMessenger(EXOPhysicsList *phys, const char *dir);
    ~EXOPhysicsListMessenger();

    void SetNewValue(G4UIcommand* command, G4String value);
    G4String GetCurrentValue(G4UIcommand* command);
  
  private:
    EXOPhysicsList            *fPhysicsList;
    G4UIdirectory             *fDirectory;
    G4UIcmdWithAnInteger      *fVerboseCmd;
    G4UIcmdWithADoubleAndUnit *fDefCutCmd;
    G4UIcmdWithoutParameter   *fListPhysCmd;
    G4UIcmdWithoutParameter   *fClearPhysCmd;
    G4UIcmdWithAString        *fRegisterPhysCmd;
  };


//} // namespace

#endif /* EXOPhysicsListMessenger_hh */
