#ifndef EXOLXeHit_h
#define EXOLXeHit_h 1

#include "EXOSim/EXOVHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <cstddef> //for size_t


class EXOLXeHit : public EXOVHit 
{
  public:

      EXOLXeHit();

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw() {}
      void Print() {}

  public:

    G4ThreeVector position;
    G4double etotal; // total energy deposit for this hit
    
    G4double echarge; // ionization energy deposit
                      // for betas and gammas, echarge = etotal
                      // for alphas, echarge = etotal*ALPHA_QUENCH_FACTOR
    G4String particleName;
    G4int ancestorParticleType; // stores integer-type of ancestor particle
    G4double ancestorParticleEnergy; // stores the energy of ancestor particle in MeV
    G4int ptype; //Type of process (defined in tracking action)
    G4String depositProcess;  // How did this hit deposit energy 
};


// vector collection of one type of hits
typedef G4THitsCollection<EXOLXeHit> EXOLXeHitsCollection;

extern G4Allocator<EXOLXeHit> EXOLXeHitsAllocator;

inline void* EXOLXeHit::operator new(size_t) {
  void* aHit;
  aHit = (void*) EXOLXeHitsAllocator.MallocSingle();
  return aHit;
}


inline void EXOLXeHit::operator delete(void* aHit) {
  EXOLXeHitsAllocator.FreeSingle((EXOLXeHit*) aHit);
}

#endif

