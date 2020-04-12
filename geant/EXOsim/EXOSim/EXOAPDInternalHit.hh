#ifndef EXOAPDInternalHit_h
#define EXOAPDInternalHit_h 1

#include "EXOSim/EXOVHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <cstddef> //for size_t


class EXOAPDInternalHit : public EXOVHit 
{
  public:

      EXOAPDInternalHit();

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:

    G4ThreeVector position;
    G4double etotal; // total energy deposit for this hit
    
    G4double echarge; // ionization energy deposit
    G4String particleName;
    G4int gangNo;
  
};


// vector collection of one type of hits
typedef G4THitsCollection<EXOAPDInternalHit> EXOAPDInternalHitsCollection;

extern G4Allocator<EXOAPDInternalHit> EXOAPDInternalHitsAllocator;

inline void* EXOAPDInternalHit::operator new(size_t) {
  void* aHit;
  aHit = (void*) EXOAPDInternalHitsAllocator.MallocSingle();
  return aHit;
}


inline void EXOAPDInternalHit::operator delete(void* aHit) {
  EXOAPDInternalHitsAllocator.FreeSingle((EXOAPDInternalHit*) aHit);
}

#endif

