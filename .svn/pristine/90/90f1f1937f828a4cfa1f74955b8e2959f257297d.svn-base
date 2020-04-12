#ifndef EXOAPDSD_h
#define EXOAPDSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

#include "EXOAPDHit.hh"
#include "EXOAPDInternalHit.hh"
#include "EXORunAction.hh"

class EXODetectorConstruction;
class G4Step;
class G4HCofThisEvent;

class EXOAPDSD : public G4VSensitiveDetector {
  
public:
  
  EXOAPDSD(G4String, EXODetectorConstruction*);
  ~EXOAPDSD();
  
  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist);
  G4bool ProcessFinalHit(const G4Step* aStep);
  void   EndOfEvent(G4HCofThisEvent*);

  G4bool RemovedAPDs(const G4Step *aStep);
private:

  G4int GetGangNo(const G4Step* aStep);
  EXOAPDHitsCollection*       APDHitsCollection;
  EXOAPDInternalHitsCollection* APDInternalHitsCollection;
  EXODetectorConstruction*    EXODetector;
  G4int                       HitID;

};

#endif
