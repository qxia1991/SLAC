#include "EXOSim/EXOAPDSD.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "EXOSim/EXORunAction.hh"
#include "EXOUtilities/EXODimensions.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  const G4int APDPlane1_gang_table[NUMBER_APDS_PER_PLANE] = 
                                       {1,1,
                    1,1,1,2,2,
                    34,34,1,1,2,2,2,3,3,
                    34,34,34,4,4,2,2,3,3,3,7,7,
                    33,33,34,34,4,4,4,5,5,3,3,7,7,7,
                    33,33,33,36,36,4,4,5,5,5,10,10,7,7,
                    32,32,33,33,36,36,36,6,6,5,5,10,10,10,8,8,
                    32,32,32,35,35,36,36,6,6,6,12,12,10,10,8,8,8,
                    32,32,35,35,35,37,37,6,6,12,12,12,11,11,8,8,
                    27,27,35,35,37,37,37,31,31,12,12,11,11,11,9,9,
                    27,27,27,29,29,37,37,31,31,31,18,18,11,11,9,9,9,
                    27,27,29,29,29,30,30,31,31,18,18,18,16,16,9,9,
                    26,26,29,29,30,30,30,24,24,18,18,16,16,16,13,13,
                    26,26,26,28,28,30,30,24,24,24,17,17,16,16,13,13,13,
                    26,26,28,28,28,23,23,24,24,17,17,17,14,14,13,13,
                    25,25,28,28,23,23,23,22,22,17,17,14,14,14,
                    25,25,25,21,21,23,23,22,22,22,15,15,14,14,
                    25,25,21,21,21,20,20,22,22,15,15,15,
                    21,21,20,20,20,19,19,15,15,
                    20,20,19,19,19,
                    19,19};
  

  G4int APDPlane2_gang_table[NUMBER_APDS_PER_PLANE] = 
                                       {20,20,
                    20,20,20,21,21,
                    16,16,20,20,21,21,21,22,22,
                    16,16,16,23,23,21,21,22,22,22,26,26,
                    15,15,16,16,23,23,23,24,24,22,22,26,26,26,
                    15,15,15,18,18,23,23,24,24,24,29,29,26,26,
                    14,14,15,15,18,18,18,25,25,24,24,29,29,29,27,27,
                    14,14,14,17,17,18,18,25,25,25,31,31,29,29,27,27,27,
                    14,14,17,17,17,19,19,25,25,31,31,31,30,30,27,27,
                    9,9,17,17,19,19,19,13,13,31,31,30,30,30,28,28,
                    9,9,9,11,11,19,19,13,13,13,37,37,30,30,28,28,28,
                    9,9,11,11,11,12,12,13,13,37,37,37,35,35,28,28,
                    8,8,11,11,12,12,12,6,6,37,37,35,35,35,32,32,
                    8,8,8,10,10,12,12,6,6,6,36,36,35,35,32,32,32,
                    8,8,10,10,10,5,5,6,6,36,36,36,33,33,32,32,
                    7,7,10,10,5,5,5,4,4,36,36,33,33,33,
                    7,7,7,3,3,5,5,4,4,4,34,34,33,33,
                    7,7,3,3,3,2,2,4,4,34,34,34,
                    3,3,2,2,2,1,1,34,34,
                    2,2,1,1,1,
                    1,1};
  
EXOAPDSD::EXOAPDSD(G4String name, EXODetectorConstruction* SD ) 
  : G4VSensitiveDetector(name), EXODetector(SD) {

  G4String HCname="APDHitsCollection";
  collectionName.insert(HCname);

  HCname="APDInternalHitsCollection";
  collectionName.insert(HCname);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOAPDSD::~EXOAPDSD() {}


void EXOAPDSD::Initialize(G4HCofThisEvent* HCE) {

  APDHitsCollection = new EXOAPDHitsCollection
    (SensitiveDetectorName,collectionName[0]); 

  APDInternalHitsCollection = new EXOAPDInternalHitsCollection
    (SensitiveDetectorName,collectionName[1]); 

  HitID = -1;


}

G4int EXOAPDSD::GetGangNo(const G4Step* aStep)
{

   G4int apd_id = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(0);
   G4int APDPlaneNumber = apd_id/1000;
   G4int APDnumber = apd_id%1000;
   if ( APDPlaneNumber < 0 || APDPlaneNumber >= NAPDPLANE ) {
     G4cout << "EXOAPDSD::ProcessHits: APD plane number is " << APDPlaneNumber << G4endl;
     return -1;
   }

   if ( APDnumber < 0 || APDnumber >= NUMBER_APDS_PER_PLANE ) {
     G4cout << "EXOAPDSD::ProcessHits: APD replica number is " << APDnumber << G4endl;
     return -1;
   }
      
   // Get the gang number from the lookup table
     
   if ( APDPlaneNumber == 0 ) return APDPlane1_gang_table[APDnumber] - 1;
   else return APDPlane2_gang_table[APDnumber] - 1 + NUMBER_APD_CHANNELS_PER_PLANE;
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOAPDSD::ProcessFinalHit(const G4Step* aStep)
{

  // Fill in the hit info and store the hit for steps that will be killed.
  if ( RemovedAPDs(aStep) ) return true;

  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();
  G4int gangNo = GetGangNo(aStep);
  if (gangNo < 0) return true;

  EXOAPDHit *ahit = new EXOAPDHit();
  ahit->gangNo = gangNo;
  ahit->time = time;
  HitID = APDHitsCollection->insert(ahit);

  // Kills photons transmitted into the APD, or photons which hit a removed APD
  aStep->GetTrack()->SetTrackStatus(fStopAndKill);

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOAPDSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{ 

  // Fill in the hit info and store the hit
  if ( RemovedAPDs(aStep) ) return true;
      
  G4int gangNo = GetGangNo(aStep);
  if (gangNo < 0) return true;
 
  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();
  G4double etotal = aStep->GetTotalEnergyDeposit();
  G4ThreeVector endpos = aStep->GetPostStepPoint()->GetPosition();
  G4String particleName = 
    aStep->GetTrack()->GetDefinition()->GetParticleName();
  
  if ( etotal > 0.0 ) {
    // make a hit and add it to the collection
    EXOAPDInternalHit* ahit = new EXOAPDInternalHit();
    //    ahit->set_position(aStep->GetPostStepPoint()->GetPosition());
    ahit->etotal = etotal;
    ahit->time = time;
    ahit->particleName = particleName;
    ahit->echarge = etotal;
    ahit->gangNo = gangNo;
  
    // Get position of the event in the local APD co-ordinates
  
    G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
    G4TouchableHandle theTouchable = postStepPoint->GetTouchableHandle();
    G4ThreeVector worldPosition = postStepPoint->GetPosition();
    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
  
    ahit->position = localPosition;
  
    HitID = APDInternalHitsCollection->insert(ahit);
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOAPDSD::EndOfEvent(G4HCofThisEvent* HCE) {


  // Add this hit collection to the event

  G4String HCname = collectionName[0];

  static G4int HCID = -1;
  if(HCID<0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(HCname);
  }

  HCE->AddHitsCollection(HCID,APDHitsCollection);

  HCname = collectionName[1];

  static G4int  HCIDInt = -1;
  if(HCIDInt<0) {
    HCIDInt = G4SDManager::GetSDMpointer()->GetCollectionID(HCname);
  }

  HCE->AddHitsCollection(HCIDInt,APDInternalHitsCollection);
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool EXOAPDSD::RemovedAPDs(const G4Step *aStep)
{

    G4int APDPlaneNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
    G4int APDnumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(0);

    // APD_no is a APD numbering system desined to match the convention: left to right, top to bottom when facing the front of the APDplane
    int APD_no = 0;
    if (APDPlaneNumber == 0) APD_no = APDnumber + 1;
    if (APDPlaneNumber == 1) APD_no = 518 - APDnumber;
    
    // This is used to kill hits off the 50 removed apds:             
    if(APD_no == 1 || APD_no == 2 || APD_no == 7 || APD_no == 8
        || APD_no == 16 || APD_no == 28 || APD_no == 29 || APD_no == 42
        || APD_no == 57 || APD_no == 73 || APD_no == 89 || APD_no == 122
        || APD_no == 138 || APD_no == 171 || APD_no == 187 || APD_no == 203
        || APD_no == 218 || APD_no == 231 || APD_no == 232 || APD_no == 244
        || APD_no == 252 || APD_no == 253 || APD_no == 258 || APD_no == 259
        || APD_no == 260 || APD_no == 261 || APD_no == 266 || APD_no == 267
        || APD_no == 275 || APD_no == 287 || APD_no == 288 || APD_no == 301
        || APD_no == 316 || APD_no == 332 || APD_no == 348 || APD_no == 381
        || APD_no == 397 || APD_no == 430 || APD_no == 446 || APD_no == 462
        || APD_no == 477 || APD_no == 490 || APD_no == 491 || APD_no == 503
        || APD_no == 511 || APD_no == 512 || APD_no == 517 || APD_no == 518 )
    {
        return true;
    }

    else return false;
}


