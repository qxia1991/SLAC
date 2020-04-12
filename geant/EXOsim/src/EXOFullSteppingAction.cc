//______________________________________________________________________________
//
// EXOFullSteppingAction
//
// Stepping action for the full EXO detector.  Currently, this just saves
// times, but it will allow for future updates to save information in the
// EXOFull simulations.
//
// Added information to save (hopefully) energy deposited in the LXe cells.
// - AS 1/19/12
//______________________________________________________________________________
#include "EXOSim/EXOFullSteppingAction.hh"
#include "EXOSim/EXOFullDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"

#include "G4ProcessManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void EXOFullSteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
  G4Track* fTrack = aStep->GetTrack();

  G4int StepNo = fTrack->GetCurrentStepNumber();

//  if (StepNo == 1) {
//      EXOAnalysisManager* analysis = EXOAnalysisManager::getInstance();
//    }  
//  This part collects energy deposited when in one of the LXe cells.
//  Originally, in the old EXO Full framework, an Analysis Manager was used.
//  That is commented out for now until we figure out how to output.
//  Also, there are placeholders for gathering information about weight & position.
  if (fTrack->GetTrackID() != 1 ) {
      if (aStep->GetTotalEnergyDeposit() ) {
        G4double time = aStep->GetPreStepPoint()->GetGlobalTime() ;
        G4double edep = aStep->GetTotalEnergyDeposit();
        G4double weight = aStep->GetPreStepPoint()->GetWeight() ;
        G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();
//  Get energy deposition for everything that goes into the LXe
        if(fTrack->GetMaterial()->GetName()=="LiquidXe"){
//      EXOAnalysisManager::getInstance()->AddEnergy(edep,weight,time);
        }
      }
  }

  // Keep a list of the times of all the steps, so that the Geant event
  // can be divided up into detector events in the EventAction

  G4double time = aStep->GetPostStepPoint()->GetGlobalTime();

  // Don't record steps that occur at t = 0 with zero energy deposit

  if ( aStep->GetTotalEnergyDeposit() < 1.0*keV &&
       time < 1.0*nanosecond ) return;

  fStepMirrors[0].insert(time);
}

