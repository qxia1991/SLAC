#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "EXOSim/EXONeutronPhysics.hh"

#include "G4Version.hh"
#if 0+G4VERSION_NUMBER >= 960
// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(EXONeutronPhysics);
#endif
#if 0+G4VERSION_NUMBER >= 950
#include "G4BuilderType.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXONeutronPhysics::EXONeutronPhysics(G4int ver)
  : G4VPhysicsConstructor("EXONeutron")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bHadronInelastic);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXONeutronPhysics::EXONeutronPhysics(G4int ver, const G4String&)
  : G4VPhysicsConstructor("EXONeutron")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bHadronInelastic);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXONeutronPhysics::~EXONeutronPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleTypes.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void EXONeutronPhysics::ConstructParticle()
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
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

// ions
  G4Deuteron::DeuteronDefinition();
  G4Triton::TritonDefinition();
  G4He3::He3Definition();
  G4Alpha::AlphaDefinition();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#if 0+G4VERSION_NUMBER < 1000

#include "G4NeutronCaptureAtRest.hh"

#include "G4HadronElasticProcess.hh"
#include "G4LElastic.hh"
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4HENeutronInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPInelasticData.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4LCapture.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4HadronFissionProcess.hh"
#include "G4NeutronHPFission.hh"
#include "G4NeutronHPFissionData.hh"

void EXONeutronPhysics::ConstructProcess()
{
  if(verboseLevel > 1) {
    G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl;
  }

  bool enableHPNeutron = true;
    G4ProcessManager* pmanager = G4Neutron::NeutronDefinition()->GetProcessManager();

  {
      // neutrons
      if (enableHPNeutron) {

	// elastic scattering
	G4HadronElasticProcess* theElasticProcess = 
	  new G4HadronElasticProcess;
	G4LElastic *theElasticModel = new G4LElastic;
	theElasticModel->SetMinEnergy(19*MeV);
	G4NeutronHPElastic *theElasticNeutronHP = new G4NeutronHPElastic;
	theElasticNeutronHP->SetMaxEnergy(19*MeV);
	theElasticProcess->RegisterMe(theElasticModel);
	theElasticProcess->RegisterMe(theElasticNeutronHP);
	//G4NeutronHPElasticData * theNeutronData = new G4NeutronHPElasticData;
	theElasticProcess->AddDataSet(new G4NeutronHPElasticData());
	pmanager->AddDiscreteProcess(theElasticProcess);

	// inelastic scattering
	G4NeutronInelasticProcess* theInelasticProcess =
	  new G4NeutronInelasticProcess("inelastic");
	G4HENeutronInelastic* theInelasticModel2 = new G4HENeutronInelastic;
	theInelasticModel2->SetMinEnergy(55*GeV);
	theInelasticProcess->RegisterMe(theInelasticModel2);
	G4LENeutronInelastic* theInelasticModel = new G4LENeutronInelastic;
	theInelasticModel->SetMinEnergy(19*MeV);
	theInelasticProcess->RegisterMe(theInelasticModel);
	G4NeutronHPInelastic * theInelasticNeutronHP =
	  new G4NeutronHPInelastic;
	theInelasticNeutronHP->SetMaxEnergy(19*MeV);
	theInelasticProcess->RegisterMe(theInelasticNeutronHP);
	theInelasticProcess->AddDataSet(new G4NeutronHPInelasticData());
	pmanager->AddDiscreteProcess(theInelasticProcess);

	// capture
	G4HadronCaptureProcess* theCaptureProcess =
	  new G4HadronCaptureProcess;
	G4LCapture* theCaptureModel = new G4LCapture;
	theCaptureModel->SetMinEnergy(19*MeV);
	theCaptureProcess->RegisterMe(theCaptureModel);
	G4NeutronHPCapture * theCaptureNeutronHP = new G4NeutronHPCapture;
	theCaptureNeutronHP->SetMaxEnergy(19*MeV);
	theCaptureProcess->RegisterMe(theCaptureNeutronHP);
	theCaptureProcess->AddDataSet(new G4NeutronHPCaptureData());
	pmanager->AddDiscreteProcess(theCaptureProcess);

	// fission
	G4HadronFissionProcess* theFissionProcess =
	  new G4HadronFissionProcess;
	G4LFission* theFissionModel = new G4LFission;
	theFissionModel->SetMinEnergy(19*MeV);
	theFissionProcess->RegisterMe(theFissionModel);
	G4NeutronHPFission * theFissionNeutronHP = new G4NeutronHPFission;
	theFissionNeutronHP->SetMaxEnergy(19*MeV);
	theFissionProcess->RegisterMe(theFissionNeutronHP);
	theFissionProcess->AddDataSet(new G4NeutronHPFissionData());
	pmanager->AddDiscreteProcess(theFissionProcess);

      } else {

        // Elastic scattering
        G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess();
        G4LElastic* protelMod = new G4LElastic();
        theElasticProcess->RegisterMe(protelMod);
        pmanager->AddDiscreteProcess(theElasticProcess);
        G4NeutronCaptureAtRest *theCaptureProcess = new G4NeutronCaptureAtRest;
        pmanager->AddProcess(theCaptureProcess);
        pmanager->SetProcessOrdering(theCaptureProcess, idxAtRest);
      }
  }
}
#else
void EXONeutronPhysics::ConstructProcess() {
  G4cerr << "EXONeutronPhysics: This physics list is deprecated. It makes no use." << G4endl; }
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
