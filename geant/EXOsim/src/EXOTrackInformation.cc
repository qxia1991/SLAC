#include "EXOSim/EXOTrackInformation.hh"

G4Allocator<EXOTrackInformation> EXOTrackInformationAllocator;

EXOTrackInformation::EXOTrackInformation() : 
  G4VUserTrackInformation(),
  fEventMirrorNumber(0),
  fAncestorParticleType(0),
  fAncestorParticleEnergy(0.0)
{
}

EXOTrackInformation::EXOTrackInformation(const EXOTrackInformation& info) :
  G4VUserTrackInformation(info),
  fEventMirrorNumber(info.fEventMirrorNumber),
  fAncestorParticleType(info.fAncestorParticleType),
  fAncestorParticleEnergy(info.fAncestorParticleEnergy)
{
}
