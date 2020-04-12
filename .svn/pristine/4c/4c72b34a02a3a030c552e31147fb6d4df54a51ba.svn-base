
#ifndef EXOTrackingAction_h
#define EXOTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include <set>
#include <map>
#include "EXOUtilities/EXOMCTrackInfo.hh"
#include "EXOSim/EXOTrackingActionMessenger.hh"

class G4VProcess;
class G4ParticleDefinition;
class TTree;
class G4Navigator;

class EXOTrackingAction : public G4UserTrackingAction {

  public:

    EXOTrackingAction();
    ~EXOTrackingAction();
    inline void Clear(const char *opt = "") { fTracks.clear(); } // do that on new event
   
    void PreUserTrackingAction( const G4Track*);
    void PostUserTrackingAction( const G4Track*);

  enum  Selection_t {
    // this is actually exclusion selection, you choose what to drop
    // if you want to disable things completely just set both selections to kNotCached 
    //
    kNotCached      = ( 1 << 0 ), // not found in cache
    kNoFlags        = ( 1 << 1 ), // have fFlags==0
    kOpticalPhotons = ( 1 << 2 ), // 
    kGammas         = ( 1 << 10 ), //
    kNeutrons       = ( 1 << 3 ), // 
    kUncharged      = ( 1 << 4 ), // gammas, neutrinos ... 
    kElectrons      = ( 1 << 5 ), // also positrons
    kHadronElastic  = ( 1 << 6 ), // produced by hadron elastic, neutrons produce tons of this
    kLowEnergy      = ( 1 << 7 ), // with fVertexEnergy < fMinimalEnergy
    kHiGeneration   = ( 1 << 8 ), // with fGeneration > fMaxGeneration
    kOutsideVolume  = ( 1 << 9 )  // born outside of fOurVoulume and its daughters
  };
  static G4int SelectionFromString(const G4String &v);
  static G4String SelectionToString(G4int v);
  bool IsSelected(const G4Track*, G4int mode) const; // return whether track passed selection, mode is OR of Selection_t
  bool StoreTrack(const G4Track*); // try (and store) track info

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() const { return fVerbose; }
  void SetCacheSelection(G4int v) { fCacheSel = v; }
  G4int GetCacheSelection() const { return fCacheSel; }
  void SetStoreSelection(G4int v) { fStoreSel = v; }
  G4int GetStoreSelection() const { return fStoreSel; }
  void SetMinimalEnergy(G4double v) { fMinimalEnergy = v; }
  G4double GetMinimalEnergy() const { return fMinimalEnergy; }
  void SetMaxGeneration(G4int v) { fMaxGeneration = v; }
  G4int GetMaxGeneration() const { return fMaxGeneration; }
  void SetOurVolume(const G4String &v) { fOurVolume = v; }
  const G4String& GetOurVolume() const { return fOurVolume; }
  void SetDumpStore(G4bool v) { fDumpStore = v; }
  G4bool GetDumpStore() const { return fDumpStore; }
  void SetEventID(G4int v) { fEventID = v; }
  G4int GetEventID() const { return fEventID; }


private:
  G4int       fVerbose;  // 
  G4int       fCacheSel; // drop mode for entering cache, OR of flags from Selection_t
  G4int       fStoreSel; // drop mode for saving to file, OR of flags from Selection_t
  G4double    fMinimalEnergy; // for selection
  G4int       fMaxGeneration; // for selection
  G4String    fOurVolume;     // for selection
  G4bool      fDumpStore;  // dump each saved TrackInfo

  G4int       fEventID;  // cache value, need this when run under control of exosim_module
  struct TrackCacheValue {
    const G4ParticleDefinition *fParticle;
    G4int fFlags;
    G4int fGeneration;
  };
  typedef std::map<G4int, TrackCacheValue> TrackCache; // 20-30 bytes per track
  TrackCache  fTracks;   // cache of seen tracks, G4Track's for them may be alreday deleted

  inline const TrackCacheValue* GetTrack(G4int id) const {
    TrackCache::const_iterator iter = fTracks.find(id);
    return ((iter!=fTracks.end())?(&iter->second):0); }

  EXOMCTrackInfo fStore; // persistent object that is attached to tree
  TTree*      fTree;     // ROOT tree itself, whoever opened file is responsible for close
  G4bool      fTreeFailed; // do not try to use tree
  int FillTree();

  EXOTrackingActionMessenger fMessenger; // be user friendly
  mutable G4Navigator *fNavigator; // should not interfere with G4

  G4bool      fCheckForImportanceProcesses; 

  std::set<const G4VProcess*> fImportanceProcesses;
  G4int fMirrorNumber;
  G4int fAncestorParticleType; // For ancestor particle info.
  G4double fAncestorParticleEnergy; // " " 
};

#endif
