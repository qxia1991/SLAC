#ifndef EXOTrackInformation_h
#define EXOTrackInformation_h 1

#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"
#include <cstddef> //for size_t


class EXOTrackInformation : public G4VUserTrackInformation 
{
  public:

    EXOTrackInformation();
    EXOTrackInformation(const EXOTrackInformation& track);

    inline void* operator new(size_t);
    inline void  operator delete(void*);

  private:
    G4int fEventMirrorNumber; // Mirror number is incremented when particles are split
    G4int fAncestorParticleType; // Integer corresponding to type of Ancestor Particle
    G4double fAncestorParticleEnergy; // Energy of Ancestor Particle in MeV
 
  public:
    void SetMirrorNumber(G4int mirror) { fEventMirrorNumber = mirror; }
    G4int GetMirrorNumber() const { return fEventMirrorNumber; }

    void SetAncestorParticleEnergy(G4double energy) { fAncestorParticleEnergy = energy; }
    G4double GetAncestorParticleEnergy() { return fAncestorParticleEnergy; }

    void SetAncestorParticleType(G4int name) { fAncestorParticleType = name; }
    G4int GetAncestorParticleType() { return fAncestorParticleType; }

};


// vector collection of one type of hits

extern G4Allocator<EXOTrackInformation> EXOTrackInformationAllocator;

inline void* EXOTrackInformation::operator new(size_t) 
{
  return EXOTrackInformationAllocator.MallocSingle();
}


inline void EXOTrackInformation::operator delete(void* info) 
{
  EXOTrackInformationAllocator.FreeSingle((EXOTrackInformation*) info);
}

#endif /* EXOTrackInformation_h */

