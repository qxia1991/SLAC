#include "EXOSim/EXORunAction.hh"
#include "EXOSim/EXOStackingAction.hh"
#include "TTree.h"
#include "Randomize.hh"
#include "EXOUtilities/EXODimensions.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXORunAction::EXORunAction( EXOStackingAction * sa) :
  runActionMessenger(this),
  stackAction(sa),
  kill_low_energy_particles(false),
  anticorr_alpha(false),
  fastscint_alpha(true),
  e_field_alpha(2.0),
  run_id(1000*G4UniformRand()),  
  nex_alpha (.13),
  increment(0)
{
  // Must initialize all pointers declared in header file. 
  // Doing this in function declaration instead of body is 
  // faster, for some reason (and is recommended).
  energy_tree = new TTree("energy_tree","energy_tree");
  energy_tree->Branch("x",&x,"x/D");
  energy_tree->Branch("y",&y,"y/D");
  energy_tree->Branch("z",&z,"z/D");
  energy_tree->Branch("energy",&energy,"energy/D");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EXORunAction::BeginOfRunAction(const G4Run* aRun)
{

  //  G4cout << "Starting run number " << aRun->GetRunID() << G4endl;

  stackAction->set_kill_low_energy_particles( kill_low_energy_particles );




}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXORunAction::FillEnergyTree(G4double e_x, G4double e_y, G4double e_z, G4double e_e){

  x=e_x;
  y=e_y;
  z=e_z;
  energy=e_e;
  energy_tree->Fill();
}



void EXORunAction::EndOfRunAction(const G4Run* aRun)
{  

}

void EXORunAction::set_kill_low_energy_particles( G4bool val )
{
  kill_low_energy_particles = val;
}

void EXORunAction::set_anticorr_alpha( G4bool val )
{

  G4cout << "**** Using Alpha Team's Anticorrelation Algorithm - Alpha Team >> A Team ****" << G4endl;

   anticorr_alpha = val;
}

void EXORunAction::set_efield_alpha( G4double val )
{

  G4cout << "**** Using " << val << "kV for Team Alpha's Anticorrelation ****" << G4endl;

   e_field_alpha = val;
}


void EXORunAction::set_nex_alpha( G4double val )
{

  G4cout << "**** Using " << val << " for Team Alpha's Nex/Nel ****" << G4endl;

   nex_alpha = val;
}

void EXORunAction::set_fastscint_alpha( G4bool val )
{
  if (val) G4cout << "**** Using Fast Scintillation for Team Alpha's Anticorrelation **** " << G4endl;
   fastscint_alpha = val;
}
    
void EXORunAction::DeleteTree()
{
  energy_tree->Reset();
}
