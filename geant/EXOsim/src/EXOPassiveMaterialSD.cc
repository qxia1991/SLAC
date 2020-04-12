#include "EXOSim/EXOPassiveMaterialSD.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "EXOSim/EXORunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOPassiveMaterialSD::EXOPassiveMaterialSD(G4String name, EXODetectorConstruction* SD ) 
  : G4VSensitiveDetector(name), EXODetector(SD) {

  G4String HCname="PassiveMaterialHitsCollection";
  collectionName.insert(HCname);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOPassiveMaterialSD::~EXOPassiveMaterialSD() {;}


void EXOPassiveMaterialSD::Initialize(G4HCofThisEvent* HCE) {

  HitsCollection = new EXOPassiveMaterialHitsCollection
    (SensitiveDetectorName,collectionName[0]); 

  HitID = -1;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOPassiveMaterialSD::ProcessFinalHit(const G4Step* aStep)
{
  // Called to process a hit for a track that will subsequently be killed.
  // Therefore we use the kinetic energy as the total energy of the hit.
  return AddHitWithTotalEnergy( aStep, aStep->GetTrack()->GetKineticEnergy() );
}

G4bool EXOPassiveMaterialSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
  // Process a normal hit 
  return AddHitWithTotalEnergy( aStep, aStep->GetTotalEnergyDeposit() );
}

G4bool EXOPassiveMaterialSD::AddHitWithTotalEnergy( const G4Step* aStep, G4double etotal)
{ 
  G4VPhysicalVolume* volume = aStep->GetPostStepPoint()->GetPhysicalVolume();
  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();
  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();

  G4int materialID = -1;
  if ( volume == EXODetector->GetSalt() ) {
    materialID = 0;
  }
  else if ( volume == EXODetector->GetAir() || volume == EXODetector->GetAirGap() ) {
    materialID = 1;
  }
  else if ( volume == EXODetector->GetSideShield() || volume == EXODetector->GetFrontShield() || volume == EXODetector->GetRearShield()|| volume == EXODetector->GetOuterShield() ) {
    materialID = 2;
  }
  else if ( volume == EXODetector->GetOuterCryo() ) {
    materialID = 3;
  }
  else if ( volume == EXODetector->GetInnerCryo() ) {
    materialID = 4;
  }
  else if ( volume == EXODetector->GetHFE() ) {
    materialID = 5;
  }
  else if ( volume == EXODetector->GetLXeVessel() ) {
    materialID = 6;
  }  
  else if ( volume == EXODetector->GetInactiveLXe() ) {
    materialID = 7;
  }  
  else if ( volume == EXODetector->GetAPDFrame1() ) {
    materialID = 8;
  }  
  else if ( volume == EXODetector->GetAPDFrame2() ) {
    materialID = 9;
  }  
  else if ( volume == EXODetector->GetReflector_pz() || volume == EXODetector->GetReflector_nz() ) {
    materialID = 10;
  }  

  if ( etotal > 0.0 ) {
    // make a hit and add it to the collection
    EXOPassiveMaterialHit* ahit = new EXOPassiveMaterialHit();
    ahit->materialID = materialID;
    ahit->etotal = etotal;
    ahit->time = time;
    ahit->particleName = particleName;
    HitID = HitsCollection->insert(ahit);
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOPassiveMaterialSD::EndOfEvent(G4HCofThisEvent* HCE) {


  // Add this hit collection to the event

  G4String HCname = collectionName[0];

  static G4int HCID = -1;
  if(HCID<0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(HCname);
  }

  HCE->AddHitsCollection(HCID,HitsCollection);
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
