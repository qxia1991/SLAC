#ifndef EXOTrackingKiller_hh
#define EXOTrackingKiller_hh

#include "EXOSim/EXOTrackingKillerMessenger.hh"
#include "globals.hh"
#include <vector>

class EXODetectorConstruction;
class G4Track;
class G4ParticleDefinition;
class G4VPhysicalVolume;
class G4VProcess;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOTrackingKiller
{
public:
  EXOTrackingKiller(EXODetectorConstruction*);
  //~EXOTrackingKiller();

  void ClearKillers() { fKillers.clear(); }
  G4bool AddKiller(const G4String& value);
  G4int GetNumOfKillers() const { return fKillers.size(); }
  G4String GetKillerList() const;
  //G4bool ConsiderKill(const G4ParticleDefinition *particle, const G4StepPoint* pt) const;
  G4bool ConsiderKill(const G4Track *t, G4bool post_step = true) const;

private:
  EXODetectorConstruction*    fDetector;
  EXOTrackingKillerMessenger  fTrackingKillerMessenger; 
  struct ParticleKiller {
    G4bool fParticleInverted;
    const G4ParticleDefinition *fParticle;
    G4bool fVolumeInverted;
    const G4VPhysicalVolume *fVolume;
    G4bool fVolumeWithDaughters;
    G4bool fProcessInverted;
    const G4VProcess *fProcess;
    G4bool fEnergyInverted;
    G4int fEnergyCompare;
    G4double fEnergy;
  };
  typedef std::vector<ParticleKiller> KillerList;
  KillerList fKillers; 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
