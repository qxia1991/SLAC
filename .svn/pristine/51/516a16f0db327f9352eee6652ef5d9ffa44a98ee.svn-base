
#include <globals.hh>
#include <G4ios.hh>
#include <G4Version.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>
#if 0+G4VERSION_NUMBER >= 960
#include <G4GenericPhysicsList.hh>
#endif
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>

#include <G4Material.hh>
#include <G4MaterialTable.hh>
//#include <iomanip>   
#include <G4StateManager.hh>
#include <G4ApplicationState.hh>

//#include "G4DataQuestionaire.hh"
#include "EXOSim/EXOPhysicsList.hh"
#include "EXOSim/EXOPhysicsListMessenger.hh"


EXOPhysicsList::EXOPhysicsList(G4int ver)
 : G4VModularPhysicsList()
{
  SetVerboseLevel(ver);
  //DeclareProperties();
  fMessenger = new EXOPhysicsListMessenger(this, "/PhysicsList/");
  defaultCutValue = 10.0*CLHEP::mm;

  //Default cuts are applied everywhere, then A G4Region is defined below to set
  //cuts for innerCryostat and all daughters to much finer cuts. 
  defaultCutValue  = 10.0*CLHEP::mm;
  cutForGamma      = defaultCutValue;
  cutForElectron   = defaultCutValue;
  cutForPositron   = defaultCutValue;
  cutForMuon       = 0.0;
  cutForProton     = defaultCutValue;
  cutForNeutron    = 0.0;
  cutForAlpha      = 0.0;
  cutForGenericIon = 0.0;
  cutInsideTPC     = 0.1*CLHEP::mm;
  cutInnerRegion   = 1.0*CLHEP::mm;

  //G4DataQuestionaire it(photon);
  G4cout << "<<< Geant4 Physics List simulation engine: EXOPhysicsList" << G4endl;
  G4cout << G4endl;

  RegisterPhysicsConstructor("EXOEmPenelopePhysics");
  //RegisterPhysicsConstructor("G4EmExtraPhysics");
  RegisterPhysicsConstructor("G4OpticalPhysics");
  RegisterPhysicsConstructor("EXONeutronPhysics");
//  RegisterPhysicsConstructor("G4HadronElasticPhysics");
//  RegisterPhysicsConstructor("G4StoppingPhysics");
#if 0+G4VERSION_NUMBER >= 1000
//  RegisterPhysicsConstructor("G4HadronPhysicsQGSP_BERT");
#else
//  RegisterPhysicsConstructor("HadronPhysicsQGSP_BERT");
#endif
//  RegisterPhysicsConstructor("HadronPhysicsLHEP_BERT_HP");
//  RegisterPhysicsConstructor("G4IonPhysics");
  RegisterPhysicsConstructor("G4DecayPhysics");
  RegisterPhysicsConstructor("G4RadioactiveDecayPhysics");
  RegisterPhysicsConstructor("EXOSpecialPhysics");
}

EXOPhysicsList::~EXOPhysicsList()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <G4BosonConstructor.hh>
#include <G4LeptonConstructor.hh>
#include <G4MesonConstructor.hh>
#include <G4BaryonConstructor.hh>
#include <G4IonConstructor.hh>
#include <G4ShortLivedConstructor.hh>
void EXOPhysicsList::ConstructParticle()
{
  if (verboseLevel>0) G4cout << __PRETTY_FUNCTION__ << G4endl;
  // As of version 4.10 all used particles MUST be created in here,
  // so we are firing anything just for the case
  G4Geantino::GeantinoDefinition();
  G4BosonConstructor::ConstructParticle();
  G4LeptonConstructor::ConstructParticle();
  G4MesonConstructor::ConstructParticle();
  G4BaryonConstructor::ConstructParticle();
  G4IonConstructor::ConstructParticle();
  G4ShortLivedConstructor::ConstructParticle();
  G4VModularPhysicsList::ConstructParticle();
}

void EXOPhysicsList::ConstructProcess()
{
  if (verboseLevel>0) G4cout << __PRETTY_FUNCTION__ << G4endl;
  G4VModularPhysicsList::ConstructProcess();
  
  if (verboseLevel>2) {
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->DumpInfo();
    }
  }
}

#include <G4RegionStore.hh>
void EXOPhysicsList::SetCuts()
{
  if (verboseLevel >0){
    G4cout << "EXOPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  //G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 100*MeV);
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  //

  //this->SetCutsWithDefault();   
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(50*CLHEP::eV, 100*CLHEP::MeV);
  if (cutForGamma>0.) SetCutValue(cutForGamma, "gamma");
  if (cutForElectron>0.) SetCutValue(cutForElectron, "e-");
  if (cutForPositron>0.) SetCutValue(cutForPositron, "e+");
  if (cutForMuon>0.) SetCutValue(cutForMuon, "mu-");
  if (cutForMuon>0.) SetCutValue(cutForMuon, "mu+");
  if (cutForProton>0.) SetCutValue(cutForProton, "proton");
  if (cutForNeutron>0.) SetCutValue(cutForNeutron, "neutron");
  if (cutForAlpha>0.) SetCutValue(cutForAlpha, "alpha");
  if (cutForGenericIon>0.) SetCutValue(cutForGenericIon, "GenericIon");
  if (cutInsideTPC>0.) {
    G4Region* region = G4RegionStore::GetInstance()->GetRegion("TPC");
    G4ProductionCuts* cuts = new G4ProductionCuts;
    cuts->SetProductionCut(cutInsideTPC);
    region->SetProductionCuts(cuts);
  }
  if (cutInnerRegion>0.) {
    G4Region* region;
    G4ProductionCuts* cuts;

    // Set cuts for inner detector stuff with finer tracking
    region = G4RegionStore::GetInstance()->GetRegion("InnerRegion");
    cuts = new G4ProductionCuts;
    cuts->SetProductionCut(0.1*cutInnerRegion,G4ProductionCuts::GetIndex("gamma"));
    cuts->SetProductionCut(cutInnerRegion,G4ProductionCuts::GetIndex("e-"));
    cuts->SetProductionCut(cutInnerRegion,G4ProductionCuts::GetIndex("e+"));
    region->SetProductionCuts(cuts);
    G4cout <<"InnerRegion cuts are set to:" << G4endl;
    G4cout <<"  gamma cut    = "<< cuts->GetProductionCut("gamma")/CLHEP::mm <<" mm"<< G4endl;
    G4cout <<"  electron cut = "<< cuts->GetProductionCut("e-")/CLHEP::mm    <<" mm"<< G4endl;
    G4cout <<"  positron cut = "<< cuts->GetProductionCut("e+")/CLHEP::mm    <<" mm"<< G4endl;
  }
  if (cutInsideTPC>0.){
    G4Region* region;
    G4ProductionCuts* cuts;
        // Set cuts for inner detector stuff with finer tracking
    region = G4RegionStore::GetInstance()->GetRegion("TPC");
    cuts = new G4ProductionCuts;
    cuts->SetProductionCut(0.1*cutInsideTPC,G4ProductionCuts::GetIndex("gamma"));
    cuts->SetProductionCut(cutInsideTPC,G4ProductionCuts::GetIndex("e-"));
    cuts->SetProductionCut(cutInsideTPC,G4ProductionCuts::GetIndex("e+"));
    region->SetProductionCuts(cuts);

    G4cout <<"InsideTPC cuts are set to:" << G4endl;
    G4cout <<"  gamma cut    = "<< cuts->GetProductionCut("gamma")/CLHEP::mm <<" mm"<< G4endl;
    G4cout <<"  electron cut = "<< cuts->GetProductionCut("e-")/CLHEP::mm    <<" mm"<< G4endl;
    G4cout <<"  positron cut = "<< cuts->GetProductionCut("e+")/CLHEP::mm    <<" mm"<< G4endl;
  }

  if (verboseLevel>0) DumpCutValuesTable();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#if 0+G4VERSION_NUMBER >= 950
#include "G4BuilderType.hh"
namespace {
  static const G4String typeUnknown = "Unknown";
  static const G4String typeTransportation = "Transportation";
  static const G4String typeElectromagnetic = "Electromagnetic";
  static const G4String typeEmExtra = "EmExtra";
  static const G4String typeDecay = "Decay";
  static const G4String typeHadronElastic = "HadronElastic";
  static const G4String typeHadronInelastic = "HadronInelastic";
  static const G4String typeStopping = "Stopping";
  static const G4String typeIons = "Ions";
  static const G4String noType = "------";
}

const G4String& G4VPhysicsConstructor__GetPhysicsTypeName(G4BuilderType aType)
{
  switch (aType) {
  case bUnknown:            return typeUnknown;            break;
  case bTransportation:     return typeTransportation;     break;
  case bElectromagnetic:    return typeElectromagnetic;    break;
  case bEmExtra:            return typeEmExtra;            break;
  case bDecay:              return typeDecay;              break;
  case bHadronElastic:      return typeHadronElastic;      break;
  case bHadronInelastic:    return typeHadronInelastic;    break;
  case bStopping:           return typeStopping;           break;
  case bIons:               return typeIons;               break;
  default: ;
  }
  return noType;  
}
#endif

void EXOPhysicsList::ListPhysics()
{
  G4cout << "Registered physics constructors" << G4endl;
  const G4VPhysicsConstructor *phys = 0;
  for(int i=0;i<100;i++) {
    phys = GetPhysics(i);
    if (phys==0) break;
    G4cout << "  [" << i <<"] '" << phys->GetPhysicsName() <<"'";
#if 0+G4VERSION_NUMBER >= 950
    G4cout << " (" << G4VPhysicsConstructor__GetPhysicsTypeName((G4BuilderType)phys->GetPhysicsType()) <<")";
#endif
    if (verboseLevel > 2)
      G4cout <<" "<< (void*)phys;
    G4cout << G4endl;
  }
}
void EXOPhysicsList::ClearPhysics()
{
  G4StateManager*    stateManager = G4StateManager::GetStateManager();
  G4ApplicationState currentState = stateManager->GetCurrentState();
  if(!(currentState==G4State_PreInit)){
    G4Exception("EXOPhysicsList::ClearPhysics",
		"Run0203", JustWarning,
                "Geant4 kernel is not PreInit state : Method ignored.");
    return;
  }

#if 0+G4VERSION_NUMBER >= 1000
  ((G4VMPLsubInstanceManager.offset[g4vmplInstanceID]).physicsVector)->clear();
#else
  physicsVector->clear();
#endif
}

void EXOPhysicsList::RegisterPhysics(G4VPhysicsConstructor* phys)
{
  if (verboseLevel > 0 )
    G4cout << "EXOPhysicsList::RegisterPhysics: " <<  phys->GetPhysicsName()  << "  is added" << G4endl;
  G4VModularPhysicsList::RegisterPhysics(phys);
}
void EXOPhysicsList::ReplacePhysics(G4VPhysicsConstructor* phys)
{
#if 0+G4VERSION_NUMBER >= 950
  G4VModularPhysicsList::ReplacePhysics(phys);
#else
  // Can't implement this without G4VPhysicsContructor::typePhysics
  G4cerr << "EXOPhysicsList::ReplacePhysics(phys) call is not available in this version of Geant4." << G4endl;
#endif
}
void EXOPhysicsList::RemovePhysics(G4VPhysicsConstructor* phys)
{
#if 0+G4VERSION_NUMBER >= 950
  G4VModularPhysicsList::RemovePhysics(phys);
#else
  G4StateManager*    stateManager = G4StateManager::GetStateManager();
  G4ApplicationState currentState = stateManager->GetCurrentState();
  if(!(currentState==G4State_PreInit)){
    G4Exception("EXOPhysicsList::RemovePhysics",
		"Run0205", JustWarning,
                "Geant4 kernel is not PreInit state : Method ignored.");
    return;
  }

  for (G4PhysConstVector::iterator itr = physicsVector->begin(); itr != physicsVector->end(); itr++) {
    if ( phys == (*itr)) {
      if (verboseLevel > 0 )
        G4cout << "EXOPhysicsList::RemovePhysics: " <<  (*itr)->GetPhysicsName()  << "  is removed" << G4endl;
      physicsVector->erase(itr);
      break;
    }
  }
#endif
}

#if 0+G4VERSION_NUMBER >= 1000
#include <G4PhysicsConstructorRegistry.hh>
void EXOPhysicsList::RegisterPhysicsConstructor(const G4String& phys_name)
{
  G4VPhysicsConstructor *phys = G4PhysicsConstructorRegistry::Instance()->GetPhysicsConstructor(phys_name);
  if (phys)
    RegisterPhysics(phys);
  else
    G4cerr << "Can't find physics constructor '" << phys_name <<"'"<< G4endl;
}
#else
// simulate if it's not provided
#include "EXOSim/EXOEmPenelopePhysics.hh"
#include "EXOSim/EXONeutronPhysics.hh"
#include "EXOSim/EXOSpecialPhysics.hh"
//
#include <G4DecayPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4OpticalPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#if 0+G4VERSION_NUMBER >= 940
#include <G4HadronElasticPhysicsHP.hh>
#endif
#include <G4IonPhysics.hh>
#if 0+G4VERSION_NUMBER < 1000
#include <G4QStoppingPhysics.hh>
#endif
#if 0+G4VERSION_NUMBER >= 960
#include <G4StoppingPhysics.hh>
#endif
#include <HadronPhysicsFTFP_BERT.hh>
#if 0+G4VERSION_NUMBER >= 960
#include <HadronPhysicsFTFP_BERT_HP.hh>
#endif
#include <HadronPhysicsQGSP_BERT.hh>
#include <HadronPhysicsQGSP_BERT_HP.hh>
#include <HadronPhysicsLHEP.hh>
#if 0+G4VERSION_NUMBER <= 940
#include <HadronPhysicsLHEP_BERT_HP.hh>
#endif
#if 0+G4VERSION_NUMBER >= 940
#include <HadronPhysicsShielding.hh>
#endif
//#include <G4Physics.hh>
void EXOPhysicsList::RegisterPhysicsConstructor(const G4String& phys_name)
{
//  G4cout << __PRETTY_FUNCTION__ <<" "<< phys_name << G4endl;

  // EXO
  if        (phys_name == "EXOSpecial") {
//    EXOSpecialPhysics();

  } else if (phys_name == "EXOEmPenelopePhysics") {
    RegisterPhysics(   new EXOEmPenelopePhysics());

  } else if (phys_name == "EXONeutronPhysics") {
    RegisterPhysics(   new EXONeutronPhysics());

  } else if (phys_name == "EXOSpecialPhysics") {
    RegisterPhysics(   new EXOSpecialPhysics());

  // decay
  } else if (phys_name == "G4DecayPhysics") {
    RegisterPhysics(   new G4DecayPhysics());

  } else if (phys_name == "G4RadioactiveDecayPhysics") {
    RegisterPhysics(   new G4RadioactiveDecayPhysics());

  // electromagnetic
  } else if (phys_name == "G4EmStandardPhysics") {
    RegisterPhysics(   new G4EmStandardPhysics());

  } else if (phys_name == "G4EmPenelopePhysics") {
    RegisterPhysics(   new G4EmPenelopePhysics());

  } else if (phys_name == "G4OpticalPhysics") {
    RegisterPhysics(   new G4OpticalPhysics());

  // gamma_lepto_buclear
  } else if (phys_name == "G4EmExtraPhysics") {
    RegisterPhysics(   new G4EmExtraPhysics());

  // hadron_elastic
  } else if (phys_name == "G4HadronElasticPhysics") {
    RegisterPhysics(   new G4HadronElasticPhysics());

  } else if (phys_name == "G4HadronElasticPhysicsHP") {
#if 0+G4VERSION_NUMBER >= 940
    RegisterPhysics(   new G4HadronElasticPhysicsHP());
#else
    RegisterPhysics(   new G4HadronElasticPhysics("elastic",0,true));
#endif

  // hadron_inelastic
  } else if (phys_name == "HadronPhysicsFTFP_BERT") {
    RegisterPhysics(   new HadronPhysicsFTFP_BERT());

#if 0+G4VERSION_NUMBER >= 960
  } else if (phys_name == "HadronPhysicsFTFP_BERT_HP") {
    RegisterPhysics(   new HadronPhysicsFTFP_BERT_HP());
#endif

  } else if (phys_name == "HadronPhysicsQGSP_BERT") {
    RegisterPhysics(   new HadronPhysicsQGSP_BERT());

  } else if (phys_name == "HadronPhysicsQGSP_BERT_HP") {
    RegisterPhysics(   new HadronPhysicsQGSP_BERT_HP());

  } else if (phys_name == "HadronPhysicsLHEP") {
    RegisterPhysics(   new HadronPhysicsLHEP());

#if 0+G4VERSION_NUMBER <= 940
  } else if (phys_name == "HadronPhysicsLHEP_BERT_HP") {
    RegisterPhysics(   new HadronPhysicsLHEP_BERT_HP());
#endif

#if 0+G4VERSION_NUMBER >= 940
  } else if (phys_name == "HadronPhysicsShielding") {
    RegisterPhysics(   new HadronPhysicsShielding());
#endif

  // ions
  } else if (phys_name == "G4IonPhysics") {
    RegisterPhysics(   new G4IonPhysics());

  // limiters
//  } else if (phys_name == "G4NeutronTrackingCut") {
//    RegisterPhysics(   new G4NeutronTrackingCut());

  // stopping
  } else if (phys_name == "G4StoppingPhysics" || 
             phys_name == "G4QStoppingPhysics") {
#if 0+G4VERSION_NUMBER >= 960
    RegisterPhysics(   new G4StoppingPhysics());
#else
    RegisterPhysics(   new G4QStoppingPhysics());
#endif

  } else
    G4cerr << "Can't find physics constructor '" << phys_name <<"'"<< G4endl;
}
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#if 0+G4VERSION_NUMBER >= 960
G4VModularPhysicsList* EXOPhysicsList::GetGenericPhysicsList(const G4String& list_names)
{
  std::vector<G4String> list;
  for (str_size i = 0, j = list_names.index(' ',0); true; i = j+1, j = list_names.index(' ',j+1)) {
    G4String str;
    str.assign(list_names,i,((j!=std::string::npos)?j-i:-1));
    if (!str.empty()) list.push_back(str);
    if (j == std::string::npos) break;
  }
  G4GenericPhysicsList* physics = new G4GenericPhysicsList(&list);
  return physics;
}
#else
G4VModularPhysicsList* EXOPhysicsList::GetGenericPhysicsList(const G4String& list_names)
{
  //G4Exception("EXOPhysicsList","1",FatalException,"Call GetGenericPhysicsList(name) is not implemented in this version of Geant4.");
  G4cerr << "EXOPhysicsList::GetGenericPhysicsList(name) call is not available in this version of Geant4." << G4endl;
  return 0;
}
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#if 0+G4VERSION_NUMBER >= 922
#include <G4PhysListFactory.hh>
G4VModularPhysicsList* EXOPhysicsList::GetReferencePhysicsList(const G4String& list_name)
{
  G4PhysListFactory physicsFactory;
  return physicsFactory.GetReferencePhysList(list_name);
}
#else
G4VModularPhysicsList* EXOPhysicsList::GetReferencePhysicsList(const G4String& list_name)
{
  G4cerr << "EXOPhysicsList::GetReferencePhysicsList(name) call is not available in this version of Geant4." << G4endl;
  return 0;
}
#endif
