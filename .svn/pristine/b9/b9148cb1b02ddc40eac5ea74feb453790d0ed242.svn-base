#include "G4SystemOfUnits.hh"
//#include "G4VUserPhysicsList.hh"
//#include "G4PhysicsListHelper.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ParticleTable.hh"
#include "G4ios.hh"
#include "G4Tokenizer.hh"           

#include "EXOSim/EXOPhysicsList.hh"
#include "EXOSim/EXOPhysicsListMessenger.hh"

EXOPhysicsListMessenger::EXOPhysicsListMessenger(EXOPhysicsList* phys, const char *_dir)
  : fPhysicsList(phys)
{
  G4String dir(_dir);
  // /run/particle    directory
  fDirectory = new G4UIdirectory(dir);
  fDirectory->SetGuidance("Commands for EXOPhysicsList.");

  fDefCutCmd = new G4UIcmdWithADoubleAndUnit((dir+"defaultCutValue").c_str(),this);
  fDefCutCmd->SetGuidance("Set default cut value");
  fDefCutCmd->SetParameterName("cut",false);
  fDefCutCmd->SetDefaultValue(1.0);
  //fDefCutCmd->SetRange("cut >=0.0");
  fDefCutCmd->SetDefaultUnit("mm");
  fDefCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fVerboseCmd = new G4UIcmdWithAnInteger((dir+"SetVerboseLevel").c_str(),this);
  fVerboseCmd->SetGuidance("Set the Verbose level");
  fVerboseCmd->SetGuidance(" 0 : Silent (default)");
  fVerboseCmd->SetGuidance(" 1 : Display warning messages");
  fVerboseCmd->SetGuidance(" 2 : Display more");
  fVerboseCmd->SetParameterName("level",true);
  fVerboseCmd->SetDefaultValue(0);
  fVerboseCmd->SetRange("level >=0 && level <=3");

  fListPhysCmd = new G4UIcmdWithoutParameter((dir+"ListPhysics").c_str(),this);
  fListPhysCmd->SetGuidance("Print list of physics enabled");

  fClearPhysCmd = new G4UIcmdWithoutParameter((dir+"ClearPhysics").c_str(),this);
  fClearPhysCmd->SetGuidance("Clear list of physics builders");

  fRegisterPhysCmd = new G4UIcmdWithAString((dir+"RegisterPhysics").c_str(),this);
  fRegisterPhysCmd->SetGuidance("Add a builder to the list of physics");


  //messenger.DeclareProperty("defaultCutValue", this->defaultCutValue, "Default Cut Value");
  //messenger.DeclareMethod("SetVerboseLevel", &TG4GenericPhysicsList::SetVerboseLevel, "Verbose Level");
  //messenger.DeclareMethod("RegisterPhysics", &TG4GenericPhysicsList::RegisterPhysicsConstructor, "Register Physics Constructor");
  // /run/particle/Verbose command
  
}

EXOPhysicsListMessenger::~EXOPhysicsListMessenger()
{
  G4cout << __PRETTY_FUNCTION__ << G4endl;
  delete fRegisterPhysCmd;
  delete fClearPhysCmd;
  delete fListPhysCmd;
  delete fDefCutCmd;
  delete fVerboseCmd;
  delete fDirectory;
}

void EXOPhysicsListMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{

  if        ( command==fVerboseCmd ) {
    fPhysicsList->SetVerboseLevel(fVerboseCmd->GetNewIntValue(newValue)); 

  } else if ( command==fDefCutCmd ) {
    G4double newCut = fDefCutCmd->GetNewDoubleValue(newValue); 
    fPhysicsList->SetDefaultCutValue(newCut);
    fPhysicsList->SetCuts();

  } else if ( command==fListPhysCmd ) {
    fPhysicsList->ListPhysics();

  } else if ( command==fClearPhysCmd ) {
    fPhysicsList->ClearPhysics();

  } else if ( command==fRegisterPhysCmd ) {
    fPhysicsList->RegisterPhysicsConstructor(newValue);

  }

} 

G4String EXOPhysicsListMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;
  return cv;
}
