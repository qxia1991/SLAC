#ifndef EXOMCTrackInfo_hh
#define EXOMCTrackInfo_hh

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <string>

class EXOMCTrackInfo : public TObject
{
  // class to encapsulate Geant4 track information
public:
  EXOMCTrackInfo(): TObject() { EXOMCTrackInfo::Clear(); }
  //EXOMCTrackInfo(const G4Track *);
  EXOMCTrackInfo(const EXOMCTrackInfo &r);
  //~EXOMCTrackInfo();
  EXOMCTrackInfo& operator=(const EXOMCTrackInfo &r);
  bool operator==(const EXOMCTrackInfo &r) const;
  virtual void Clear(Option_t* = "");
  virtual void Print(Option_t* = "") const;

  //Get/Set

  //private:
  // -1 or '-' usually means undefined
  // generally produce 80 bytes/track in file, compressed
  Int_t       fEventID;          // Geant4 event ID, feventHeader.fGeant4EventNumber
  Int_t       fTrackID;          // Geant4 track ID
  std::string fParticle;         // Particle name
  //float ParticleMass; // 
  //int ParticleCharge; //
  Int_t       fGeneration;       // generation = (parent)? parent->generation+1 : 0;
  Int_t       fFlags;            // user flags
  Int_t       fStatus;           // Geant4 track last status
  Float_t     fLength;           // Track final length in mm
  Float_t     fGlobalTime;       // Geant4 global time at the beginning of track, s
  Float_t     fLocalTime;        // Time of particle travel, s
  Int_t       fNsteps;           // Number of Geant4 steps along track
  // parent
  Int_t       fParentID;         // Geant4 track ID of creator particle
  std::string fParentParticle;   // Name of creator particle
  std::string fProcess;          // Name of creator process
  // starting point
  Float_t     fVertexX;          // Starting position X, mm
  Float_t     fVertexY;          // Starting position Y, mm
  Float_t     fVertexZ;          // Starting position Z, mm
  std::string fVertexMaterial;   // Name of material at starting point
  std::string fVertexVolume;     // Name of physical volume at starting point
  Int_t       fVertexVolumeCopy; // Copy number of physical volume at starting point
  Float_t     fVertexDirectionX; // Direction along X at starting point
  Float_t     fVertexDirectionY; // Direction along Y at starting point
  Float_t     fVertexDirectionZ; // Direction along Z at starting point
  Float_t     fVertexEnergy;     // Kinetic energy at stating point, keV
  // end point
  Float_t     fLastX;            // Final position X, mm
  Float_t     fLastY;            // Final position Y, mm
  Float_t     fLastZ;            // Final position Z, mm
  std::string fLastMaterial;     // Name of material at final point
  std::string fLastVolume;       // Name of physical volume at final point
  Int_t       fLastVolumeCopy;   // Copy number of physical volume at final point
  Float_t     fLastDirectionX;   // Direction along X at final point
  Float_t     fLastDirectionY;   // Direction along Y at final point
  Float_t     fLastDirectionZ;   // Direction along Z at final point
  Float_t     fLastEnergy;       // Kinetic energy at final point, keV
  std::string fLastProcess;      // Name of process at final point, unreliable as reason of stop
  //

  ClassDef( EXOMCTrackInfo, 1 )
};

#endif /* EXOMCTrackInfo_hh */
