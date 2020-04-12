#ifndef EXOPassiveMaterialHit_h
#define EXOPassiveMaterialHit_h 1

#include "EXOSim/EXOVHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <cstddef> //for size_t

class EXOPassiveMaterialHit : public EXOVHit 
{
  public:

      EXOPassiveMaterialHit();

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:

    G4int    materialID; // 0 = salt
                         // 1 = lead shielding
                         // 2 = outer cryostat
                         // 3 = inner cryostat
                         // 4 = HFE
                         // 5 = teflon vessel
    G4double etotal;     // total energy deposit for this hit
    G4String particleName;
  
};


// vector collection of one type of hits
typedef G4THitsCollection<EXOPassiveMaterialHit> EXOPassiveMaterialHitsCollection;

extern G4Allocator<EXOPassiveMaterialHit> EXOPassiveMaterialHitsAllocator;

inline void* EXOPassiveMaterialHit::operator new(size_t) {
  void* aHit;
  aHit = (void*) EXOPassiveMaterialHitsAllocator.MallocSingle();
  return aHit;
}

inline void EXOPassiveMaterialHit::operator delete(void* aHit) {
  EXOPassiveMaterialHitsAllocator.FreeSingle((EXOPassiveMaterialHit*) aHit);
}

#endif

