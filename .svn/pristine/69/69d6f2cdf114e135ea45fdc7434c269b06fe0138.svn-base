//______________________________________________________________________________
//
// EXOMCTrackInfo
//
// Class that holds information about selected tracks processed by Monte-Carlo 
// simulation.  This is stored in a 'mctrack' TTree.  In particular, it stores
// the following information: 
//
//   1. EventID and TrackID
//   2. Particle name, generation, last status, flags and its parent
//   3. Location and parameters of particle at first (birth) point
//   4. Location and parameters of particle at last (death) point
//   5. Track length, time and number of steps
// 
//
#include "EXOUtilities/EXOMCTrackInfo.hh"
#include <iostream>


ClassImp( EXOMCTrackInfo )

//______________________________________________________________________________
EXOMCTrackInfo::EXOMCTrackInfo(const EXOMCTrackInfo& r) : 
  TObject(r)
{
  // Copy constructor
  fEventID = r.fEventID;
  fTrackID = r.fTrackID;
  fParticle = r.fParticle;
  //float ParticleMass;
  //int ParticleCharge;
  fGeneration = r.fGeneration;
  fFlags = r.fFlags;
  fStatus = r.fStatus;
  fParentID = r.fParentID;
  fParentParticle = r.fParentParticle;
  fProcess = r.fProcess;
  fVertexX = r.fVertexX, fVertexY = r.fVertexY, fVertexZ = r.fVertexZ;
  fVertexMaterial = r.fVertexMaterial;
  fVertexVolume = r.fVertexVolume;
  fVertexVolumeCopy = r.fVertexVolumeCopy;
  fVertexDirectionX = r.fVertexDirectionX, fVertexDirectionY = r.fVertexDirectionY, fVertexDirectionZ = r.fVertexDirectionZ;
  fVertexEnergy = r.fVertexEnergy;
  fLastX = r.fLastX, fLastY = r.fLastY, fLastZ = r.fLastZ;
  fLastMaterial = r.fLastMaterial;
  fLastVolume = r.fLastVolume;
  fLastVolumeCopy = r.fLastVolumeCopy;
  fLastDirectionX = r.fLastDirectionX, fLastDirectionY = r.fLastDirectionY, fLastDirectionZ = r.fLastDirectionZ;
  fLastEnergy = r.fLastEnergy;
  fLastProcess = r.fLastProcess;
  fLength = r.fLength;
  fGlobalTime = r.fGlobalTime, fLocalTime = r.fLocalTime;
  fNsteps = r.fNsteps;
}

//______________________________________________________________________________
EXOMCTrackInfo& EXOMCTrackInfo::operator=(const EXOMCTrackInfo& r)
{
  // Copy operator 
  fEventID = r.fEventID;
  fTrackID = r.fTrackID;
  fParticle = r.fParticle;
  //float ParticleMass;
  //int ParticleCharge;
  fGeneration = r.fGeneration;
  fFlags = r.fFlags;
  fStatus = r.fStatus;
  fParentID = r.fParentID;
  fParentParticle = r.fParentParticle;
  fProcess = r.fProcess;
  fVertexX = r.fVertexX, fVertexY = r.fVertexY, fVertexZ = r.fVertexZ;
  fVertexMaterial = r.fVertexMaterial;
  fVertexVolume = r.fVertexVolume;
  fVertexVolumeCopy = r.fVertexVolumeCopy;
  fVertexDirectionX = r.fVertexDirectionX, fVertexDirectionY = r.fVertexDirectionY, fVertexDirectionZ = r.fVertexDirectionZ;
  fVertexEnergy = r.fVertexEnergy;
  fLastX = r.fLastX, fLastY = r.fLastY, fLastZ = r.fLastZ;
  fLastMaterial = r.fLastMaterial;
  fLastVolume = r.fLastVolume;
  fLastVolumeCopy = r.fLastVolumeCopy;
  fLastDirectionX = r.fLastDirectionX, fLastDirectionY = r.fLastDirectionY, fLastDirectionZ = r.fLastDirectionZ;
  fLastEnergy = r.fLastEnergy;
  fLastProcess = r.fLastProcess;
  fLength = r.fLength;
  fGlobalTime = r.fGlobalTime, fLocalTime = r.fLocalTime;
  fNsteps = r.fNsteps;
  return *this;
}

//______________________________________________________________________________
bool EXOMCTrackInfo::operator==(const EXOMCTrackInfo& r) const
{
  // Comparison operator 
  if (fEventID == r.fEventID && 
      fTrackID == r.fTrackID && 
      fParticle == r.fParticle
      ) return true;
  return false;
}

//______________________________________________________________________________
void EXOMCTrackInfo::Clear(Option_t * opt)
{
  // Clear object ID and all strings
  fEventID = fTrackID = -1;
  fParticle.clear();
  fParentParticle.clear();
  fProcess.clear();
  fVertexMaterial.clear();
  fVertexVolume.clear();
  fLastMaterial.clear();
  fLastVolume.clear();
  fLastProcess.clear();
}

//______________________________________________________________________________
void EXOMCTrackInfo::Print(Option_t * opt) const
{
  // Dump out information to the screen.
  std::ostream &os = std::cout;
  const EXOMCTrackInfo &o = *this;
  os << "{\n"
     << "  .fEventID = " << o.fEventID << ",\n"
     << "  .fTrackID = " << o.fTrackID << ",\n"
     << "  .fParticle = \"" << o.fParticle << "\",\n"
     << "  .fGeneration = " << o.fGeneration << ",\n"
     << "  .fFlags = " << o.fFlags << ",\n"
     << "  .fStatus = " << o.fStatus << ",\n"
     << "  .fParentID = " << o.fParentID << ",\n"
     << "  .fParentParticle = \"" << o.fParentParticle << "\",\n"
     << "  .fProcess = \"" << o.fProcess << "\",\n"
     << "  .fVertexX = " << o.fVertexX << ", .fVertexY = " << o.fVertexY << ", .fVertexZ = " << o.fVertexZ << ",\n"
     << "  .fVertexMaterial = \"" << o.fVertexMaterial << "\",\n"
     << "  .fVertexVolume = \"" << o.fVertexVolume << "\",\n"
     << "  .fVertexVolumeCopy = " << o.fVertexVolumeCopy << ",\n"
     << "  .fVertexDirectionX = " << o.fVertexDirectionX << ", .fVertexDirectionY = " << o.fVertexDirectionY << ", .fVertexDirectionZ = " << o.fVertexDirectionZ << ",\n"
     << "  .fVertexEnergy = " << o.fVertexEnergy << ",\n"
     << "  .fLastX = " << o.fLastX << ", .fLastY = " << o.fLastY << ", .fLastZ = " << o.fLastZ << ",\n"
     << "  .fLastMaterial = \"" << o.fLastMaterial << "\",\n"
     << "  .fLastVolume = \"" << o.fLastVolume << "\",\n"
     << "  .fLastVolumeCopy = " << o.fLastVolumeCopy << ",\n"
     << "  .fLastDirectionX = " << o.fLastDirectionX << ", .fLastDirectionY = " << o.fLastDirectionY << ", .fLastDirectionZ = " << o.fLastDirectionZ << ",\n"
     << "  .fLastEnergy = " << o.fLastEnergy << ",\n"
     << "  .fLastProcess = \"" << o.fLastProcess << "\",\n"
     << "  .fLength = " << o.fLength << ",\n"
     << "  .fGlobalTime = " << o.fGlobalTime << ",\n"
     << "  .fLocalTime = " << o.fLocalTime << ",\n"
     << "  .fNsteps = " << o.fNsteps << ",\n"
     << "}" << std::endl;
}

