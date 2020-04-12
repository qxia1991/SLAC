#ifndef EXOSteppingAction_h
#define EXOSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "EXOSim/EXOSteppingActionMessenger.hh"
#include "globals.hh"
#include <set>
#include <map>

class EXODetectorConstruction;
class EXOSteppingActionMessenger;
class EXOTrackingKiller;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOSteppingAction : public G4UserSteppingAction
{
  public:
    EXOSteppingAction(EXODetectorConstruction*);
    typedef std::set<G4double> StepTimes;
    typedef std::map<G4int, StepTimes> StepMirrors;

  protected:
    EXODetectorConstruction*    fDetector;
    EXOSteppingActionMessenger  fSteppingActionMessenger; 
    G4double  			fPhotonEnergyRemovedAPDs;
    StepMirrors                 fStepMirrors;
    bool                        fIsReset;

  public:
    void UserSteppingAction(const G4Step*);
    void Reset(); 
    const StepMirrors&  get_stepmirrors() const 
      { return fStepMirrors; }
    
    inline G4double get_photon_energy_removed_apds() const 
      { return fPhotonEnergyRemovedAPDs; }

  void SetTrackingKiller(EXOTrackingKiller *killer) { fKiller = killer; }
private:
  EXOTrackingKiller *fKiller; // we don't own it
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
