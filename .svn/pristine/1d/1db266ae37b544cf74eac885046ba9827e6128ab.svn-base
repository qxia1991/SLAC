#ifndef EXOLXeSD_h
#define EXOLXeSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

#include "EXOLXeHit.hh"
#include "EXORunAction.hh"

class EXODetectorConstruction;
class G4Step;
class G4HCofThisEvent;

class EXOLXeSD : public G4VSensitiveDetector {
  
public:
  
  EXOLXeSD(G4String, EXODetectorConstruction*);
  ~EXOLXeSD() {}
  
  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  G4bool ProcessFinalHit(const G4Step*);
  void   EndOfEvent(G4HCofThisEvent*);

private:

  G4bool AddHitWithEnergyAndCharge(const G4Step* aStep, G4double etotal, G4double echarge);
  EXOLXeHitsCollection*       HitsCollection;
  EXODetectorConstruction*  EXODetector;
  G4int                       HitID;

};

#endif
