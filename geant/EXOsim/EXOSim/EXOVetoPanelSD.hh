#ifndef EXOVetoPanelSD_h
#define EXOVetoPanelSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

#include "EXOVetoPanelHit.hh"
#include "EXORunAction.hh"

class EXODetectorConstruction;
class G4Step;
class G4HCofThisEvent;


class EXOVetoPanelSD : public G4VSensitiveDetector {
  
  public:
  
    EXOVetoPanelSD(G4String, EXODetectorConstruction*);
    ~EXOVetoPanelSD() {}

    void   Initialize(G4HCofThisEvent* HCE);
    G4bool ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist);
    G4bool ProcessFinalHit(const G4Step* aStep);
    void   EndOfEvent(G4HCofThisEvent* HCE);

  private:

    G4int GetPanelID(const G4Step* aStep);
    G4bool AddHitWithEnergy(const G4Step* aStep, G4double eTotal);

    EXOVetoPanelHitsCollection*	HitsCollection;
    EXODetectorConstruction*  	EXODetector;
    G4int						HitID;

};

#endif

