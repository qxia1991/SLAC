#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "EXOSim/EXOEmPenelopePhysics.hh"

#include "G4Version.hh"
#if 0+G4VERSION_NUMBER >= 960
// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(EXOEmPenelopePhysics);
#endif
#if 0+G4VERSION_NUMBER >= 950
#include "G4BuilderType.hh"
#endif
#if 0+G4VERSION_NUMBER >= 1000
#include "G4EmProcessOptions.hh"
#include "G4UAtomicDeexcitation.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOEmPenelopePhysics::EXOEmPenelopePhysics(G4int ver)
  : G4VPhysicsConstructor("EXOEmPenelope")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bElectromagnetic);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOEmPenelopePhysics::EXOEmPenelopePhysics(G4int ver, const G4String&)
  : G4VPhysicsConstructor("EXOEmPenelope")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bElectromagnetic);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOEmPenelopePhysics::~EXOEmPenelopePhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleTypes.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void EXOEmPenelopePhysics::ConstructParticle()
{
// gamma
  G4Gamma::GammaDefinition();

// leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
//  G4NeutrinoE::NeutrinoEDefinition();
//  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
//  G4NeutrinoMu::NeutrinoMuDefinition();
//  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();

// mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

// baryons
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
//  G4Neutron::NeutronDefinition();
//  G4AntiNeutron::AntiNeutronDefinition();

// ions
  G4Deuteron::DeuteronDefinition();
  G4Triton::TritonDefinition();
  G4He3::He3Definition();
  G4Alpha::AlphaDefinition();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4EmExtraPhysics.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4ionIonisation.hh"
#include "G4RayleighScattering.hh"

// Penelope Model
#include "G4PenelopeRayleighModel.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4PenelopePhotoElectricModel.hh"
#include "G4PenelopeGammaConversionModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4PenelopeBremsstrahlungModel.hh"
#include "G4PenelopeAnnihilationModel.hh"

void EXOEmPenelopePhysics::ConstructProcess()
{
  if(verboseLevel > 1) {
    G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl;
  }
#if 0+G4VERSION_NUMBER >= 950
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
#endif

  //const G4RegionStore* theRegionStore = G4RegionStore::GetInstance();
  //G4Region* InnerRegion = theRegionStore->GetRegion("InnerRegion");
  
  //G4cout << "EXOEmPenelopePhysics: this physics is obsolete. Use G4EmPenelopePhysics instead." << G4endl;
    // Low Energy Electromagnetic Physics, Penelope Model

#if 0+G4VERSION_NUMBER >= 1000
  G4ParticleTable::G4PTblDicIterator* theParticleIterator = aParticleIterator;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){

    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();


    if        (particleName == "gamma") {
      // gamma
      G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
      G4PenelopePhotoElectricModel* thePenelopePhotoElectricModel = new G4PenelopePhotoElectricModel();
#if 0+G4VERSION_NUMBER < 950
    // it became default in new Penelope model 2008
//    thePenelopePhotoElectricModel->SetVerbosityLevel(2);
      thePenelopePhotoElectricModel->ActivateAuger(true);
#endif
#if 0+G4VERSION_NUMBER < 960
      thePhotoElectricEffect->SetModel(thePenelopePhotoElectricModel);
#else
      thePhotoElectricEffect->SetEmModel(thePenelopePhotoElectricModel);
#endif
      pmanager->AddDiscreteProcess(thePhotoElectricEffect);
      G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
#if 0+G4VERSION_NUMBER < 960
      theComptonScattering->SetModel(new G4PenelopeComptonModel());
#else
      theComptonScattering->SetEmModel(new G4PenelopeComptonModel());
#endif
      pmanager->AddDiscreteProcess(theComptonScattering);
      G4GammaConversion* theGammaConversion = new G4GammaConversion();
#if 0+G4VERSION_NUMBER < 960
      theGammaConversion->SetModel(new G4PenelopeGammaConversionModel());
#else
      theGammaConversion->SetEmModel(new G4PenelopeGammaConversionModel());
#endif
      pmanager->AddDiscreteProcess(theGammaConversion);
      G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
#if 0+G4VERSION_NUMBER < 960
      theRayleighScattering->SetModel(new G4PenelopeRayleighModel());
#else
      theRayleighScattering->SetEmModel(new G4PenelopeRayleighModel());
#endif
      pmanager->AddDiscreteProcess(theRayleighScattering);

    } else if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
      G4eIonisation* theIonisation = new G4eIonisation();
      G4PenelopeIonisationModel* thePenelopeIonisationModel = new G4PenelopeIonisationModel();
#if 0+G4VERSION_NUMBER < 950
      thePenelopeIonisationModel->ActivateAuger(true);
#endif
      theIonisation->SetEmModel(thePenelopeIonisationModel);
      pmanager->AddProcess(theIonisation, -1, 2, 2);
      G4eBremsstrahlung* theBremsstrahlung = new G4eBremsstrahlung();
      theBremsstrahlung->SetEmModel(new G4PenelopeBremsstrahlungModel());
      pmanager->AddProcess(theBremsstrahlung, -1, -3, 3);

    } else if (particleName == "e+") {
      //positron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
      G4eIonisation* theIonisation = new G4eIonisation();
      G4PenelopeIonisationModel* thePenelopeIonisationModel = new G4PenelopeIonisationModel();
#if 0+G4VERSION_NUMBER < 950
      thePenelopeIonisationModel->ActivateAuger(true);
#endif
      theIonisation->SetEmModel(thePenelopeIonisationModel);
      pmanager->AddProcess(theIonisation,        -1, 2, 2);
      G4eBremsstrahlung* theBremsstrahlung = new G4eBremsstrahlung();
      theBremsstrahlung->SetEmModel(new G4PenelopeBremsstrahlungModel());
      pmanager->AddProcess(theBremsstrahlung,    -1, 3, 3);
      G4eplusAnnihilation* theAnnihilation = new G4eplusAnnihilation();
#if 0+G4VERSION_NUMBER < 960
      theAnnihilation->SetModel(new G4PenelopeAnnihilationModel());
#else
      theAnnihilation->SetEmModel(new G4PenelopeAnnihilationModel());
#endif
      pmanager->AddProcess(theAnnihilation,   0,-1, 4);

    } else if( particleName == "mu+" ||
               particleName == "mu-"    ) {
      //muon
      pmanager->AddProcess(new G4MuMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4MuIonisation,       -1, 2, 2);
      pmanager->AddProcess(new G4MuBremsstrahlung,   -1, 3, 3); // -1,-1, 3
      pmanager->AddProcess(new G4MuPairProduction,   -1, 4, 4); // -1,-1, 4

    } else if( particleName == "proton" ||
               particleName == "pi-" ||
               particleName == "pi+" ||
               particleName == "K-"  ||
               particleName == "K+"  ) {
      //proton
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
      pmanager->AddProcess(new G4hBremsstrahlung,     -1, 3, 3);
      pmanager->AddProcess(new G4hPairProduction,     -1, 4, 4);

    } else if( particleName == "deuteron" ||
               particleName == "triton" ||
               particleName == "He3" ||
               particleName == "alpha" )     {
      //alpha
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);

    } else if( particleName == "GenericIon" ) {
      //Ions
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);

    } else if ((!particle->IsShortLived()) &&
               (particle->GetPDGCharge() != 0.0) &&
               (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);
    }
  }

#if 0+G4VERSION_NUMBER >= 1000
  // Activate atomic deexcitation: fluorescence and Auger                                                                        
  G4VAtomDeexcitation* de = 0;
  de = G4LossTableManager::Instance()->AtomDeexcitation();
  if (!de) {
      de = new G4UAtomicDeexcitation();
      G4LossTableManager::Instance()->SetAtomDeexcitation(de);
  }
  de->SetFluo(true);
  de->SetAuger(true);
#endif

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
