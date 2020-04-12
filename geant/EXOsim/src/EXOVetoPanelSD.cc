#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "EXOSim/EXORunAction.hh"
#include "EXOSim/EXOVetoPanelSD.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "EXOSim/EXOTrackInformation.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Using temporary designations for now; need to look up actual panel names in each
// mounting location on the clean room exterior.  Members of this table will be
// referenced by panelID, which is defined as the copy number of the panel placement.
// See EXODetectorConstruction.cc for placement details (long/short shells).
const G4String vetoPanelName[29] = 
      {
	   "Top 1", "Top 2", "Top 3", "Top 4", "Top 5", "Top 6", "Top 7",
       "Bottom 1", "Bottom 2", "Bottom 3", "Bottom 4",
       "West 1", "West 2", "West 3", "West 4", "West 5", "West 6",
       "South 1", "South 2", "South 3", "South 4", "South 5", "South 6",
	   "South 7", "South 8", "South 9", "South 10", "South 11", "South 12"
      };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
EXOVetoPanelSD::EXOVetoPanelSD(G4String name, EXODetectorConstruction* SD ) 
  : G4VSensitiveDetector(name), EXODetector(SD) 
{

  G4String HCname="VetoPanelHitsCollection";
  collectionName.insert(HCname);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOVetoPanelSD::Initialize(G4HCofThisEvent* HCE) 
{

  HitsCollection = new EXOVetoPanelHitsCollection
    (SensitiveDetectorName,collectionName[0]); 

  HitID = -1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOVetoPanelSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{ 
  G4double stepEnergy = aStep->GetTotalEnergyDeposit();

  return AddHitWithEnergy(aStep, stepEnergy);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOVetoPanelSD::ProcessFinalHit(const G4Step* aStep) 
{  
  // Processes a hit for a track that will subsequently be killed.  Therefore,
  // we take the kinetic energy of the track as the total energy in the hit.

  return AddHitWithEnergy(aStep, aStep->GetTrack()->GetKineticEnergy());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOVetoPanelSD::AddHitWithEnergy(const G4Step* aStep, G4double stepEnergy)
{
  G4ThreeVector endPos = aStep->GetPostStepPoint()->GetPosition();
  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();

  // For recording the name of the specific panel hit, use uniquely defined copy numbers
  // on the panel "shell" volumes (see physiVetoPanel[i] in detector construction).
  // Modify this assignment, so as to get the copy number of the parent volume;
  // only the shell volumes have unique copy numbers corresponding to the placement.
  // G4int panelID = aStep->GetTrack()->GetTouchable()->GetCopyNumber(1);
  G4int panelID = aStep->GetTrack()->GetVolume()->GetCopyNo();

  if (stepEnergy > 0.0) {
    EXOVetoPanelHit* aHit = new EXOVetoPanelHit();
    aHit->position = endPos;
    aHit->hitEnergy = stepEnergy;   
    aHit->time = time;
    aHit->particleName = particleName; 
    aHit->panelID = panelID;
    aHit->mirrorNumber = static_cast<EXOTrackInformation*>(aStep->GetTrack()->GetUserInformation())->GetMirrorNumber();
    aHit->weight = aStep->GetPreStepPoint()->GetWeight();
    HitID = HitsCollection->insert(aHit);
	
	return true;
  }
  else {
    return false;
  }

}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOVetoPanelSD::EndOfEvent(G4HCofThisEvent* HCE) 
{

  // Add this hit collection to the event
  G4String HCname = collectionName[0];

  // Why is the HCID get initialized, then tested and assigned?  It only gets
  // referenced once within the scope of the assignment...
  static G4int HCID = -1;
  if(HCID<0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(HCname);
  }

  HCE->AddHitsCollection(HCID, HitsCollection);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

