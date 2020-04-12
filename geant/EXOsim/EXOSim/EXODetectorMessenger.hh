#ifndef EXODetectorMessenger_h
#define EXODetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"



class EXODetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXODetectorMessenger: public G4UImessenger
{
  public:
    EXODetectorMessenger(EXODetectorConstruction* );

    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    EXODetectorConstruction* EXODetector;
    
    G4UIdirectory              fDirectory;
    G4UIcmdWithADoubleAndUnit  XenonABSLCmd;
    G4UIcmdWithADoubleAndUnit  XenonRayleighCmd;
    G4UIcmdWithADouble         XenonScintYieldCmd;
    G4UIcmdWithADouble         XenonScintFanoCmd;
    G4UIcmdWithoutParameter    fDumpMaterialsCmd;
    G4UIcmdWithAnInteger       fCheckOverlapsCmd;
    G4UIcmdWithAnInteger       fDumpGeometryCmd;
    G4UIcmdWith3VectorAndUnit  fTestPointCmd;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

#endif

