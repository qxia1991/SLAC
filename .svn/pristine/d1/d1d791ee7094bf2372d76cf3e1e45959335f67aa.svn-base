#include "EXOSim/EXOLXeSD.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "EXOSim/EXOTrackInformation.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "EXOSim/EXORunAction.hh"
#include "G4RunManager.hh"
#include "EXOUtilities/EXODimensions.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOLXeSD::EXOLXeSD(G4String name, EXODetectorConstruction* SD ) 
  : G4VSensitiveDetector(name), EXODetector(SD) 
{

  G4String HCname="LXeHitsCollection";
  collectionName.insert(HCname);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOLXeSD::Initialize(G4HCofThisEvent* HCE) 
{

  HitsCollection = new EXOLXeHitsCollection
    (SensitiveDetectorName,collectionName[0]); 

  HitID = -1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EXOLXeSD::ProcessFinalHit(const G4Step* aStep) 
{  
  // Processes a hit for a track that will subsquently be killed.  Therefore,
  // we take the kinetic energy of the track as the total energy in the hit. 
  return AddHitWithEnergyAndCharge(aStep, aStep->GetTrack()->GetKineticEnergy(), 0.0);
}

G4bool EXOLXeSD::AddHitWithEnergyAndCharge(const G4Step* aStep, G4double etotal, G4double echarge)
{
  G4ThreeVector endpos = aStep->GetPostStepPoint()->GetPosition();
  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();

  EXOTrackInformation* info = (EXOTrackInformation*)(aStep->GetTrack()->GetUserInformation());

  // Keep track of Processes seen
  // This may double count since no check against trackID.
  G4String creatorProcess;
  if ((aStep->GetTrack()->GetCreatorProcess())){
      creatorProcess = (aStep->GetTrack()->GetCreatorProcess())->GetProcessName();
      //G4cout << "Process Type " << (aStep->GetTrack()->GetCreatorProcess())->GetProcessName() << G4endl;
  }

  G4int processType;
  if      (creatorProcess=="eBrem")   {processType=1;}
  else if (creatorProcess=="compt")   {processType=2;}
  else if (creatorProcess=="phot")    {processType=3;}
  else                                {processType=0;}
  
  G4String depositProcess;
  if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) {
    depositProcess = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  }
  else{
      depositProcess = "none";
  }

  if (etotal > 0.0 or (depositProcess=="conv" or depositProcess=="phot" or depositProcess=="compt" or depositProcess=="eBrem")) {
    EXOLXeHit* ahit = new EXOLXeHit();
    
    //std:: cout << "********************MJ Energy in SD " << etotal << "  " << depositProcess << std::endl;

    ahit->ancestorParticleType = info->GetAncestorParticleType(); 
    ahit->ancestorParticleEnergy = info->GetAncestorParticleEnergy();
    ahit->position = endpos;
    ahit->etotal = etotal;   
    ahit->time = time;
    ahit->echarge = echarge;
    ahit->particleName = particleName; 
    ahit->mirrorNumber = static_cast<EXOTrackInformation*>(aStep->GetTrack()->GetUserInformation())->GetMirrorNumber();
    ahit->weight = aStep->GetPreStepPoint()->GetWeight();
    ahit->ptype  = processType;
    ahit->depositProcess = depositProcess;
    HitID = HitsCollection->insert(ahit);
  }
  /*else if(aStep->GetPostStepPoint()->GetProcessDefinedStep()) {
    G4String pname = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    if (pname != "Transportation" and pname != "Scintillation" and pname != "Rayl" and pname != "conv" and pname != "msc" 
         and pname != "eIoni" and pname != "eBrem")
    {
        std::cout << "**************************>>>>>>>>>>>>>>>>>>>MJ No Energy here " ;
        if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) {                                 
            std::cout << " End Proc: "<< aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << " " ;
        }
        std::cout << (aStep->GetTrack()->GetTrackID())  << " " ;
        std::cout << (aStep->GetTrack()->GetParentID()) << " " ;
        std::cout << particleName << " "  ;
        std::cout<< std::endl;
      }
  } */

  return true;


}

G4bool EXOLXeSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{ 
  G4double etotal = aStep->GetTotalEnergyDeposit();
  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double echarge = etotal;
  //Deplete hit energy if Alpha Team's Anticorrelation algorithm is being used. The fraction to
  //deplete is determined in EXOElectronConversion.cc
  EXORunAction* runAction = (EXORunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
  if(runAction->returnAlphaAnticorr()) {
    if(runAction->returnIncrement()%2!=0) echarge = etotal*runAction->returnFrac();
  } else {
    if ( particleName == "alpha" ) echarge = etotal*ALPHA_QUENCH_FACTOR;
  }
  return AddHitWithEnergyAndCharge(aStep, etotal, echarge);
}
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOLXeSD::EndOfEvent(G4HCofThisEvent* HCE) 
{

  // Add this hit collection to the event

  G4String HCname = collectionName[0];

  static G4int HCID = -1;
  if(HCID<0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(HCname);
  }

  HCE->AddHitsCollection(HCID,HitsCollection);
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

