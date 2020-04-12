#ifndef EXOPrimaryGeneratorMessenger_h
#define EXOPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"

class EXOPrimaryGeneratorAction;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOPrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    EXOPrimaryGeneratorMessenger(EXOPrimaryGeneratorAction*);
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    EXOPrimaryGeneratorAction* EXOAction;
    G4UIdirectory                fDirectory;
    G4UIcmdWithAString           GenCmd;
    G4UIcmdWithAString           IsoCmd;
    G4UIcmdWithAnInteger         XeCaptureCmd;
    G4UIcmdWithAnInteger         CuIsoCmd;
    G4UIcmdWithAString           CuComponentCmd;
    G4UIcmdWithAnInteger         SpectralIndexCmd;
    G4UIcmdWithABool             GraphFFValuesCmd;
    G4UIcmdWithAString           RadiusFFCalcCmd;
    G4UIcmdWithAnInteger         AtomicNumberCmd;
    G4UIcmdWithADoubleAndUnit    QValueCmd;
    G4UIcmdWithADouble           MweDepthCmd;
    G4UIcmdWithABool             UseBetaPlusCmd;

    G4UIcmdWithAString           fTrackSourceCmd;
    G4UIcmdWithAString           fTrackTreeCmd;
    G4UIcmdWithAString           fTrackSelectCmd;
    G4UIcmdWithABool             fTrackModeLastCmd;
    G4UIcmdWithAString           fEventSourceCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

