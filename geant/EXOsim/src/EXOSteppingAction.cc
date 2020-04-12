#include "EXOSim/EXOSteppingAction.hh"
#include "EXOSim/EXOTrackInformation.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "EXOSim/EXOPassiveMaterialSD.hh" 
#include "EXOSim/EXOLXeSD.hh"
#include "EXOSim/EXOAPDSD.hh"
#include "EXOSim/EXOTrackingKiller.hh"

#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"

#include "Randomize.hh"  

#include "G4ProcessManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SDManager.hh"

////#include "G4RunManager.hh"
G4double transparency_cathode = 0.90;
G4double transparency_anode = 0.91667;
G4double prob_to_absorb_cathode = 1.0;
G4double prob_to_absorb_anode = 1.0;
G4double costheta_exp_cathode = 0.0;
G4double costheta_exp_anode = 0.0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOSteppingAction::EXOSteppingAction(EXODetectorConstruction* det ) : 
  G4UserSteppingAction(),
  fDetector(det),
  fSteppingActionMessenger(this),
  fPhotonEnergyRemovedAPDs(0),
  fIsReset(true),
  fKiller(0)
  
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void EXOSteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
   static EXOPassiveMaterialSD* PassiveSD = NULL; 
   static EXOLXeSD* ActiveSD = NULL; 
   static EXOAPDSD* APDSD = NULL; 

   if (fIsReset) {
  
      // Find the sensitive detectors, but only do this once!
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      G4String PasssdName="/EXODet/PassiveMaterial";
      G4String XesdName="/EXODet/Xenon";
      G4String APDsdName="/EXODet/APD";

      PassiveSD = dynamic_cast<EXOPassiveMaterialSD*>(
                    SDman->FindSensitiveDetector(PasssdName));
      ActiveSD  = dynamic_cast<EXOLXeSD*>(
                    SDman->FindSensitiveDetector(XesdName));
      APDSD     = dynamic_cast<EXOAPDSD*>(
                    SDman->FindSensitiveDetector(APDsdName));
      fIsReset = false;
    }


   G4Track* track = aStep->GetTrack();
   const G4StepPoint* presteppoint = aStep->GetPreStepPoint();
   const G4StepPoint* steppoint = aStep->GetPostStepPoint();
   G4VPhysicalVolume* prevolume = presteppoint->GetPhysicalVolume();
   G4VPhysicalVolume* volume = steppoint->GetPhysicalVolume();

   // particle killer
   if (fKiller && fKiller->ConsiderKill(track,true))
     track->SetTrackStatus(fStopAndKill);

   // specific handling for optical photons
   if (track->GetDefinition() == G4OpticalPhoton::Definition()) {

        static G4OpBoundaryProcess *CheckForAbsorption = NULL;
        G4OpBoundaryProcessStatus theEXOStatus = Undefined;
    
        if(!CheckForAbsorption){
            G4ProcessManager* pm= track->GetDefinition()->GetProcessManager();
            G4int nprocesses = pm->GetProcessListLength();
            G4ProcessVector* pv = pm->GetProcessList();
            G4int i;
            for( i=0; i<nprocesses; i++){
                if((*pv)[i]->GetProcessName()=="OpBoundary"){
                    CheckForAbsorption = (G4OpBoundaryProcess*)(*pv)[i];
                    break;
                }
            }
        }

        theEXOStatus = CheckForAbsorption->GetStatus();

        if(track->GetTrackStatus() == fStopAndKill) {
	    // if the track has been set to kill, force the status to be
	    // absorption and process the final hits as such.
	    theEXOStatus = Absorption;
            if((volume == fDetector->GetInactiveLXe()) || 
               (volume == fDetector->GetReflector_pz())||
               (volume == fDetector->GetReflector_nz())||
               (volume == fDetector->GetAPDFrame1())   || 
               (volume == fDetector->GetAPDFrame2())   || 
               (volume == fDetector->GetLXeVessel())   || 
               (volume->GetName() == "Field_Ring")     || 
               (volume == fDetector->GetWireSupport1())|| 
               (volume == fDetector->GetWireSupport2()) ) {

                if (PassiveSD) PassiveSD->ProcessFinalHit(aStep); 
            }
            if(volume == fDetector->GetActiveLXe() && ActiveSD) {
                ActiveSD->ProcessFinalHit(aStep); 
            } 
        }
 
       //currently only the absorption and spikereflection statuses are possible for APDs: 
       if(steppoint->GetStepStatus()==fGeomBoundary){
            switch(theEXOStatus){
                case Absorption:          break;
                case FresnelRefraction:        break;
                case FresnelReflection:        break;
                case Undefined:         break;
                case BackScattering:        break;
                case Detection:            break;
                case NotAtBoundary:        break;
                case SameMaterial:        break;
                case StepTooSmall:        break;
                case NoRINDEX:            break;
                case LambertianReflection:    break;
                case LobeReflection:        break;
                case TotalInternalReflection:    break;
                case SpikeReflection:        break;
                default:             break;
            }
       }
       //Check for correct volume for APD hit:
       if (volume == fDetector->GetAPD()) {    
       
           //Check for proper initialization:
           if (APDSD) {
               if (theEXOStatus == Absorption) {
                   APDSD->ProcessFinalHit(aStep); // aka: hits
                   track->SetTrackStatus(fStopAndKill);
               //kills hits on removed APDs:
               } else if (APDSD->RemovedAPDs(aStep) == true){ 
                   G4double etotal = track->GetKineticEnergy();
                   fPhotonEnergyRemovedAPDs += etotal; 
                   track->SetTrackStatus(fStopAndKill);
               }
               // Kill the track
           }
       }
       
       if((prevolume != fDetector->GetCathode()) && (volume == fDetector->GetCathode())) {
	   // Record and kill optical photons in the cathode according to the
	   // transparency_cathode probability and a power of cos(theta)
	   G4double cos_theta = std::fabs(aStep->GetTrack()->GetMomentumDirection().cosTheta());
	   if (G4UniformRand() > transparency_cathode*std::pow(cos_theta, costheta_exp_cathode)) {
	     // the photon interacts, is it absorbed?
	     if (G4UniformRand() <= prob_to_absorb_cathode) {
	         theEXOStatus = Absorption;
	     } else {
	         // reflect it
	         const G4ThreeVector oldVector = aStep->GetTrack()->GetMomentumDirection();
		 G4ThreeVector newVector(oldVector.x(), oldVector.y(), -oldVector.z());
		 aStep->GetTrack()->SetMomentumDirection(newVector);
	     }
	   }
	   if (theEXOStatus == Absorption) {
	       if (PassiveSD ) PassiveSD->ProcessFinalHit(aStep);  
	       track->SetTrackStatus(fStopAndKill);
	   }   
       }  
       if(((prevolume != fDetector->GetAnode1()) && (volume    == fDetector->GetAnode1()))   || 
          ((prevolume != fDetector->GetAnode2()) && (volume    == fDetector->GetAnode2()))) {
	   // Record and kill optical photons in the anodes according to the
	   // transparency_anode probability and a power of cos(theta)
	   G4double cos_theta = std::fabs(aStep->GetTrack()->GetMomentumDirection().cosTheta());
           if (G4UniformRand() > transparency_anode*std::pow(cos_theta, costheta_exp_anode)) {
	       // the photon interacts, is it absorbed?
	       if (G4UniformRand() <= prob_to_absorb_anode) {
	           theEXOStatus = Absorption;
	       } else {
	           // reflect it
	           const G4ThreeVector oldVector = aStep->GetTrack()->GetMomentumDirection();
	           G4ThreeVector newVector(oldVector.x(), oldVector.y(), -oldVector.z());
	           aStep->GetTrack()->SetMomentumDirection(newVector);
	       }
	   }
	   if (theEXOStatus == Absorption) {
	       if (PassiveSD ) PassiveSD->ProcessFinalHit(aStep);  
	       track->SetTrackStatus(fStopAndKill);
	   }   
       }
       if(volume == fDetector->GetCathodeRing()){
           // Record and kill optical photons in the cathode ring
           if (PassiveSD) PassiveSD->ProcessFinalHit(aStep);
           track->SetTrackStatus(fStopAndKill);
       }    
  }  

  // Keep a list of the times of all the steps, so that the Geant event
  // can be divided up into detector events in the EventAction

  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();

  // Don't record steps that occur at t = 0 with zero energy deposit

  if ( aStep->GetTotalEnergyDeposit() < 1.0*keV &&
       time < 1.0*nanosecond ) return;

  G4int mirror = 
    static_cast<EXOTrackInformation*>(aStep->GetTrack()->GetUserInformation())->GetMirrorNumber();

  fStepMirrors[mirror].insert(time);
}

void EXOSteppingAction::Reset()
{
  fStepMirrors.clear(); 
  fPhotonEnergyRemovedAPDs = 0; 
  fIsReset = true;
}
