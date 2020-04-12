#ifndef EXOPassiveMaterialSD_h
#define EXOPassiveMaterialSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

#include "EXOPassiveMaterialHit.hh"
#include "EXORunAction.hh"

class EXODetectorConstruction;
class G4Step;
class G4HCofThisEvent;

class EXOPassiveMaterialSD : public G4VSensitiveDetector {
  
public:
  
  EXOPassiveMaterialSD(G4String, EXODetectorConstruction*);
  ~EXOPassiveMaterialSD();
  
  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  G4bool ProcessFinalHit(const G4Step* aStep);
  void   EndOfEvent(G4HCofThisEvent*);

private:

  G4bool AddHitWithTotalEnergy( const G4Step* step, G4double energy ); 
  EXOPassiveMaterialHitsCollection*       HitsCollection;
  EXODetectorConstruction*  EXODetector;
  G4int                       HitID;

};

#endif
