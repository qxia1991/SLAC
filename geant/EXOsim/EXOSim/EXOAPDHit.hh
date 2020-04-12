#ifndef EXOAPDHit_h
#define EXOAPDHit_h 1

#include "EXOSim/EXOVHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <cstddef> //for size_t


class EXOAPDHit : public EXOVHit 
{
  public:

      EXOAPDHit();

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:

    G4int gangNo;
  
};


// vector collection of one type of hits
typedef G4THitsCollection<EXOAPDHit> EXOAPDHitsCollection;

extern G4Allocator<EXOAPDHit> EXOAPDHitsAllocator;

inline void* EXOAPDHit::operator new(size_t) {
  void* aHit;
  aHit = (void*) EXOAPDHitsAllocator.MallocSingle();
  return aHit;
}


inline void EXOAPDHit::operator delete(void* aHit) {
  EXOAPDHitsAllocator.FreeSingle((EXOAPDHit*) aHit);
}

#endif

