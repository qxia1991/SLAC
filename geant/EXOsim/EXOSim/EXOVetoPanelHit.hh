#ifndef EXOVetoPanelHit_h
#define EXOVetoPanelHit_h 1

#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <cstddef>	//for size_t
#include "EXOSim/EXOVHit.hh"


class EXOVetoPanelHit : public EXOVHit 
{
  public:

    EXOVetoPanelHit();

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void Draw() {}
    void Print() {}

  public:

    G4double hitEnergy; // total energy deposit for this hit
    G4ThreeVector position;
    G4String particleName;
    G4int panelID;
	
	// Inherits from EXOVHit:
	// G4double time;
    // G4double weight;
    // G4int mirrorNumber;

};


// vector collection of one type of hits
typedef G4THitsCollection<EXOVetoPanelHit> EXOVetoPanelHitsCollection;

extern G4Allocator<EXOVetoPanelHit> EXOVetoPanelHitsAllocator;

inline void* EXOVetoPanelHit::operator new(size_t) {
  void* aHit;
  aHit = (void*) EXOVetoPanelHitsAllocator.MallocSingle();
  return aHit;
}

inline void EXOVetoPanelHit::operator delete(void* aHit) {
  EXOVetoPanelHitsAllocator.FreeSingle((EXOVetoPanelHit*) aHit);
}

#endif

