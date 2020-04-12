#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "EXOSim/EXOSpecialPhysics.hh"

#include "G4Version.hh"
#if 0+G4VERSION_NUMBER >= 960
// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(EXOSpecialPhysics);
#endif
#if 0+G4VERSION_NUMBER >= 950
#include "G4BuilderType.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOSpecialPhysics::EXOSpecialPhysics(G4int ver)
  : G4VPhysicsConstructor("EXOSpecial")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bUnknown);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOSpecialPhysics::EXOSpecialPhysics(G4int ver, const G4String&)
  : G4VPhysicsConstructor("EXOSpecial")
{
  G4LossTableManager::Instance();
  SetVerboseLevel(ver);
#if 0+G4VERSION_NUMBER >= 950
  SetPhysicsType(bUnknown);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOSpecialPhysics::~EXOSpecialPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleTypes.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void EXOSpecialPhysics::ConstructParticle()
{
  /*
    G4DecayTable* MuonPlusDecayTable = new G4DecayTable();
    MuonPlusDecayTable -> Insert(new
                           G4MuonDecayChannelWithSpin("mu+",0.986));
    MuonPlusDecayTable -> Insert(new
                           G4MuonRadiativeDecayChannelWithSpin("mu+",0.014));
    G4MuonPlus::MuonPlusDefinition() -> SetDecayTable(MuonPlusDecayTable);

    G4DecayTable* MuonMinusDecayTable = new G4DecayTable();
    MuonMinusDecayTable -> Insert(new
                            G4MuonDecayChannelWithSpin("mu-",0.986));
    MuonMinusDecayTable -> Insert(new
                            G4MuonRadiativeDecayChannelWithSpin("mu-",0.014));
    G4MuonMinus::MuonMinusDefinition() -> SetDecayTable(MuonMinusDecayTable);
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <G4ProcessTable.hh>
#include <G4Scintillation.hh>
#include <G4EmProcessOptions.hh>
#include <G4UImanager.hh>

void EXOSpecialPhysics::ConstructProcess()
{
  if(verboseLevel > 1) {
    G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl;
  }
#if 0+G4VERSION_NUMBER >= 950
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
#endif
  G4UImanager *ui = G4UImanager::GetUIpointer();

  // The old way to tune
  if (1) {
    
    G4ProcessTable *processTable = G4ProcessTable::GetProcessTable();
    G4ProcessVector *processList = 0;
    
    processList = processTable->FindProcesses("Scintillation");
    for (int i=0;i<processList->size();i++) {
//      G4VProcess *process = (*processList)[i];
      G4Scintillation *process = dynamic_cast<G4Scintillation*>((*processList)[i]);
      if (!process) break;
      process->SetScintillationYieldFactor(1.);
      process->SetTrackSecondariesFirst(true);
    }

    //theCerenkovProcess->SetMaxNumPhotonsPerStep(300);
    //theCerenkovProcess->SetTrackSecondariesFirst(true);
  }

  // The new way
  /*
    G4EmProcessOptions emOptions;
    emOptions.SetFluo(true);  // Activate deexcitation and fluorescence
    emOptions.SetAuger(true); // Activate Auger effect if deexcitation
                              // is activated
    emOptions.SetPIXE(true);  // Activate Particle Induced X-Ray Emission
                              // (PIXE)
  */
  /*

    ui->ApplyCommand("/optics_engine/setWLSTimeProfile delta");

    ui->ApplyCommand("/optics_engine/setScintillationYieldFactor 1.0");
//    opticalPhysics->SetScintillationExcitationRatio(0.0);

    ui->ApplyCommand("/optics_engine/setCerencovMaxPhotons 100");
    ui->ApplyCommand("/optics_engine/setCerencovMaxBetaChange 10.0");

    ui->ApplyCommand("/optics_engine/selectOpProcess Cerenkov");
    ui->ApplyCommand("/optics_engine/setTrackSecondariesFirst true");
    ui->ApplyCommand("/optics_engine/selectOpProcess Scintillation");
    ui->ApplyCommand("/optics_engine/setTrackSecondariesFirst true");
  */

  // this sets default (?) reflectivity model
  //G4OpticalSurfaceModel themodel = unified;
  //theBoundaryProcess->SetModel(themodel);
  // actually this is improper because code anyway use 'glisur' for any surface 
  // without G4OpticalSurface

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
