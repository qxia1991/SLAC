#include "G4ios.hh"
#include "G4Version.hh"

#include "EXOSim/EXOTrackingAction.hh"
#include "EXOUtilities/EXOMCTrackInfo.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOSim/EXOTrackInformation.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4VProcess.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4ProcessTable.hh"

#ifdef HAVE_ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#else
class TTree { ; }; // dummy
#endif // HAVE_ROOT

#include "G4SystemOfUnits.hh" // confuses TString

EXOTrackingAction::EXOTrackingAction()
  :
  fMessenger(this), 
  fNavigator(0),
  fCheckForImportanceProcesses(true)
{
  fVerbose = 0;
  fCacheSel = kNotCached;
  fStoreSel = kNotCached;
  fMinimalEnergy = 1.*keV;
  fMaxGeneration = 1;
  fOurVolume = "ActiveLXe";
  fDumpStore = false;
  fEventID = -1; // means not set
  fTracks.clear();
  fStore.Clear();
  fTree = 0;
  fTreeFailed = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOTrackingAction::~EXOTrackingAction()
{
  fTracks.clear();
  fStore.Clear();
  delete fNavigator;
}

G4int EXOTrackingAction::SelectionFromString(const G4String &str)
{
  G4int mode = 0;
  std::vector<std::string> tokens;
  EXOMiscUtil::stringTokenize(str, " \t\v", tokens);
  for (unsigned i=0;i<tokens.size();i++) {
    if      (tokens[i]=="kNotCached") mode |= kNotCached;
    else if (tokens[i]=="kNoFlags") mode |= kNoFlags;
    else if (tokens[i]=="kOpticalPhotons") mode |= kOpticalPhotons;
    else if (tokens[i]=="kGammas") mode |= kGammas;
    else if (tokens[i]=="kNeutrons") mode |= kNeutrons;
    else if (tokens[i]=="kUncharged") mode |= kUncharged;
    else if (tokens[i]=="kElectrons") mode |= kElectrons;
    else if (tokens[i]=="kHadronElastic") mode |= kHadronElastic;
    else if (tokens[i]=="kLowEnergy") mode |= kLowEnergy;
    else if (tokens[i]=="kHiGeneration") mode |= kHiGeneration;
    else if (tokens[i]=="kOutsideVolume") mode |= kOutsideVolume;
    else 
      G4cerr << __func__ << ": unknown flag '" << tokens[i] << "'." << G4endl;
  }
  return mode;
}
G4String EXOTrackingAction::SelectionToString(G4int mode)
{
  G4String str;
  if (mode & kNotCached) str += "kNotCached ";
  if (mode & kNoFlags) str += "kNoFlags ";
  if (mode & kOpticalPhotons) str += "kOpticalPhotons ";
  if (mode & kGammas) str += "kGammas ";
  if (mode & kNeutrons) str += "kNeutrons ";
  if (mode & kUncharged) str += "kUncharged ";
  if (mode & kElectrons) str += "kElectrons ";
  if (mode & kHadronElastic) str += "kHadronElastic ";
  if (mode & kLowEnergy) str += "kLowEnergy ";
  if (mode & kHiGeneration) str += "kHiGeneration ";
  if (mode & kOutsideVolume) str += "kOutsideVolume ";
  return str;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>

bool EXOTrackingAction::IsSelected(const G4Track* aTrack, G4int mode) const
{
  //G4TrackStatus status = aTrack->GetTrackStatus();
  const G4ParticleDefinition *particle = aTrack->GetDefinition();
  const TrackCacheValue *track = GetTrack(aTrack->GetTrackID());

  if (mode & kNotCached) {
    if (!track) return false;
  }
  if (mode & kNoFlags) {
    if (track && track->fFlags==0) return false;
  }
  if (mode & kOpticalPhotons) {
    if (particle==G4OpticalPhoton::Definition()) return false;
  }
  if (mode & kGammas) {
    if (particle==G4Gamma::Definition()) return false;
  }
  if (mode & kNeutrons) {
    if (particle==G4Neutron::Definition()) return false;
  }
  if (mode & kUncharged) {
    if (particle->GetPDGCharge()==0.) return false;
  }
  if (mode & kElectrons) {
    if (particle==G4Electron::Definition() || 
	particle==G4Positron::Definition())
      return false;
  }
  if (mode & kHadronElastic) {
#if 0+G4VERSION_NUMBER < 960
    if (aTrack->GetCreatorProcess() && 
	( aTrack->GetCreatorProcess()->GetProcessName()=="hElastic" ||
	  aTrack->GetCreatorProcess()->GetProcessName()=="HadronElastic" ) ) // actually unreliable
      return false;
#else
    if (aTrack->GetCreatorProcess() && 
	( aTrack->GetCreatorProcess()->GetProcessName()=="hadElastic" ||
	  aTrack->GetCreatorProcess()->GetProcessName()=="CoulombScat" ) ) // actually unreliable
      return false;
#endif
  }
  if (mode & kLowEnergy) {
    if (aTrack->GetVertexKineticEnergy() < fMinimalEnergy) return false;
  }
  if (mode & kHiGeneration) {
    if (track && track->fGeneration > fMaxGeneration) return false;
  }
  if (mode & kOutsideVolume) {
    G4TouchableHandle hist;
    const G4ThreeVector &r0 = aTrack->GetVertexPosition();
    const G4ThreeVector &d0 = aTrack->GetVertexMomentumDirection();
    if (fNavigator)
      fNavigator->LocateGlobalPointAndUpdateTouchableHandle(r0, d0, hist, false);
    bool res = false; // found?
    for (int i=0;i<hist->GetHistoryDepth();i++) {
      //G4cout << "volu " << i <<" "<< hist->GetVolume(i)->GetName() << G4endl;
      if (hist->GetVolume(i)->GetName()==fOurVolume) { res = true; break; }
    }
    if (!res) return false;
  }
  return true;
}

void EXOTrackingAction::PreUserTrackingAction( const G4Track* aTrack)
{
  if(aTrack->GetParentID()==0 && aTrack->GetUserInformation()==0)
  {
    fMirrorNumber = 0;
    
    if (fCheckForImportanceProcesses && 
          fImportanceProcesses.size() == 0) {
      G4ProcessVector* vec = G4ProcessTable::GetProcessTable()->FindProcesses("ImportanceProcess");
      for(G4int i = 0;i<vec->size();i++) fImportanceProcesses.insert((*vec)[i]); 
      delete vec;
      if (vec->size() == 0) {
        // Means we are not running with importance processes, don't check anymore
        fCheckForImportanceProcesses = false;
      }
    }
  }

  // The following sets the appropriate ancestor particle energy according to whether
  // or not the particle is a product of radioactive decay
  
  // Ease-of-reading variables
  G4String creatorProcess;
  if (aTrack->GetCreatorProcess()){
      creatorProcess = aTrack->GetCreatorProcess()->GetProcessName();
  }
  // Checks if the particle is either the first one generated (w/ a parent
  // ID of 0) or is a product of radioactive decay. If so, this particle
  // is an ancestor.
  if (aTrack->GetParentID() == 0 or creatorProcess == "RadioactiveDecay"){
      // Name-to-number correlation taken directly from 
      // EXOStackingAction.cc. This just checks the name of the particle
      // and sets its type to a corresponding integer
      
      G4String particleName = aTrack->GetDefinition()->GetParticleName();
      G4String particleType = aTrack->GetDefinition()->GetParticleType();
      int ancestorParticleType;
      if (particleType == "nucleus" && particleName != "alpha"){
          ancestorParticleType = 0;
      }
      else if (particleName == "e-") {ancestorParticleType = 1;}
      else if (particleName == "e+") {ancestorParticleType = 2;}
      else if (particleName == "gamma") {ancestorParticleType = 3;}
      else if (particleName == "alpha") {ancestorParticleType = 4;}
      else if (particleName == "mu-") {ancestorParticleType = 5;}
      else if (particleName == "mu+") {ancestorParticleType = 6;}
      else {ancestorParticleType = 7;}

      G4double ancestorParticleEnergy = aTrack->GetKineticEnergy();

      // Actually sets the info in the track
      EXOTrackInformation* anInfo = new EXOTrackInformation();
      anInfo->SetAncestorParticleType(ancestorParticleType);
      anInfo->SetAncestorParticleEnergy(ancestorParticleEnergy);
      anInfo->SetMirrorNumber(fMirrorNumber);
      const_cast<G4Track*>(aTrack)->SetUserInformation(anInfo);

      // Statements printed if verbosity is set to > 2
      if (fVerbose > 2) {
          G4cout << "New ancestor created. Ancestor information changed:" << G4endl
          << "Name: " << particleName << " -- Energy: " << ancestorParticleEnergy
          << " -- Track ID: " << aTrack->GetTrackID() << G4endl;
      }
  }
  //------------

  if (!fNavigator) {
    G4Navigator *g4nav = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    fNavigator = new G4Navigator();
    fNavigator->SetWorldVolume(g4nav->GetWorldVolume());
  }
  G4TrackStatus status = aTrack->GetTrackStatus();
  if (1) { // select on status
    bool sel = IsSelected(aTrack,fCacheSel);
    if (sel) {
      TrackCache::const_iterator iter = fTracks.find(aTrack->GetTrackID());
      if (iter==fTracks.end()) { // new
	TrackCacheValue track;
	track.fParticle = aTrack->GetDefinition();
	if (aTrack->GetParentID()) {
	  iter = fTracks.find(aTrack->GetParentID());
	  if (iter!=fTracks.end())
	    track.fGeneration = iter->second.fGeneration + 1;
	  else
	    track.fGeneration = -1;
	} else
	  track.fGeneration = 0;
	track.fFlags = 0;
	fTracks[aTrack->GetTrackID()] = track;
      } // else ?[
    }
    if (fVerbose > 2) {
      const G4Event *anEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
      const TrackCacheValue *track = GetTrack(aTrack->GetTrackID());
      G4cout << "==track_info++";
      G4cout <<"\tiev=" << anEvent->GetEventID()
	     <<"\titr=" << aTrack->GetTrackID() 
	     <<"\t" << aTrack->GetParentID()
	     <<"\tst=" << aTrack->GetTrackStatus()
	     <<"\tpart=" << aTrack->GetDefinition()->GetParticleName();
      G4cout << "\tc=" << track <<" "<< sel << G4endl;
    }
  }


}

void EXOTrackingAction::PostUserTrackingAction( const G4Track* aTrack)
{

  // Make sure we copy to the secondaries
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if (secondaries) {
    EXOTrackInformation* info = (EXOTrackInformation*)(aTrack->GetUserInformation());
    size_t nSeco = secondaries->size();
    for(size_t i=0;i<nSeco;i++)
    { 
      // This places the ancestor information (set in PreUserTrackingAction above)
      // into all secondaries. If a secondary happens to be an ancestor, this self-corrects
      // when that secondary is simulated
      EXOTrackInformation* infoNew = new EXOTrackInformation(*info);
      infoNew->SetAncestorParticleType(info->GetAncestorParticleType());
      infoNew->SetAncestorParticleEnergy(info->GetAncestorParticleEnergy());
      // Statements for verbose > 2 setting
      if (fVerbose > 2) {
          G4String secParID = (*secondaries)[i]->GetParentID();
          G4cout << "Passing ancestor information to secondary. Parent ID: " << secParID << G4endl;
      }

      
      if (fImportanceProcesses.find((*secondaries)[i]->GetCreatorProcess()) != fImportanceProcesses.end()) {
        // This was created by the importance process, set the mirror number to increment
        fMirrorNumber++;
        infoNew->SetMirrorNumber(fMirrorNumber);
      }
      (*secondaries)[i]->SetUserInformation(infoNew);
    }
  }

  G4TrackStatus status = aTrack->GetTrackStatus();

  if (status==fStopAndKill || status==fKillTrackAndSecondaries) {
    StoreTrack(aTrack);
  }
}

bool EXOTrackingAction::StoreTrack( const G4Track* aTrack)
{
  // Sometimes this is called from StackingAction and fNavigator may be uninitialized
  if (!fNavigator) {
    G4Navigator *g4nav = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    fNavigator = new G4Navigator();
    fNavigator->SetWorldVolume(g4nav->GetWorldVolume());
  }
    bool sel = IsSelected(aTrack,fStoreSel);
    if (sel) {
      const TrackCacheValue *track = GetTrack(aTrack->GetTrackID());
      const TrackCacheValue *parent = GetTrack(aTrack->GetParentID());
      EXOMCTrackInfo &info = fStore;
      const G4Event *anEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
      info.fEventID = fEventID ? fEventID : anEvent->GetEventID();
      info.fTrackID = aTrack->GetTrackID();
      info.fParticle = aTrack->GetDefinition()->GetParticleName();
      //float ParticleMass; // 
      //int ParticleCharge; //
      info.fGeneration = track ? track->fGeneration : -1;
      info.fFlags = track ? track->fFlags : 0;
      info.fStatus = aTrack->GetTrackStatus();
      info.fParentID = aTrack->GetParentID();
      if (parent && parent->fParticle)
	info.fParentParticle = parent->fParticle->GetParticleName(); 
      else
	info.fParentParticle = "-";
      info.fProcess = ((aTrack->GetCreatorProcess())?(aTrack->GetCreatorProcess()->GetProcessName()):"-");
      const G4Step *aStep = aTrack->GetStep();
      if (aTrack->GetCurrentStepNumber()) { // track was processed, all fine
	const G4ThreeVector &r0 = aTrack->GetVertexPosition();
	const G4VPhysicalVolume *aVertexVolume = fNavigator->LocateGlobalPointAndSetup(r0, 0, false);
	const G4Material *aVertexMaterial = aTrack->GetLogicalVolumeAtVertex()->GetMaterial();
	info.fVertexX = r0.x()/mm, info.fVertexY = r0.y()/mm, info.fVertexZ = r0.z()/mm;
	info.fVertexMaterial = (aVertexMaterial ? aVertexMaterial->GetName() : "-");
	info.fVertexVolume = (aVertexVolume ? aVertexVolume->GetName() : "-");
	info.fVertexVolumeCopy = (aVertexVolume ? aVertexVolume->GetCopyNo() : -1);
	const G4ThreeVector &d0 = aTrack->GetVertexMomentumDirection();
	info.fVertexDirectionX = d0.x(), info.fVertexDirectionY = d0.y(), info.fVertexDirectionZ = d0.z();
	info.fVertexEnergy = aTrack->GetVertexKineticEnergy()/keV;
	//const G4ThreeVector &r1 = aTrack->GetPosition(); // this may be prestep
	const G4ThreeVector &r1 = (aStep)?aStep->GetPostStepPoint()->GetPosition():aTrack->GetPosition();
	//const G4VPhysicalVolume *aLastVolume = fNavigator->LocateGlobalPointAndSetup(r1, 0, false); // this one is more correct, but misbehaves on enter of new volume
	const G4VPhysicalVolume *aLastVolume = aTrack->GetNextTouchableHandle()->GetVolume(0);
	const G4Material *aLastMaterial = (aLastVolume)?aLastVolume->GetLogicalVolume()->GetMaterial():0;
	info.fLastX = r1.x()/mm, info.fLastY = r1.y()/mm, info.fLastZ = r1.z()/mm;
	info.fLastMaterial = (aLastMaterial ? aLastMaterial->GetName() : "-");
	info.fLastVolume = (aLastVolume ? aLastVolume->GetName() : "-");
	info.fLastVolumeCopy = (aLastVolume ? aLastVolume->GetCopyNo() : -1);
	const G4ThreeVector &d1 = aTrack->GetMomentumDirection();
	info.fLastDirectionX = d1.x(), info.fLastDirectionY = d1.y(), info.fLastDirectionZ = d1.z();
	info.fLastEnergy = aTrack->GetKineticEnergy()/keV;
      } else { // it wasn't, many things are not filled
	const G4ThreeVector &r0 = aTrack->GetPosition();
	const G4ThreeVector &d0 = aTrack->GetMomentumDirection();
	G4TouchableHistory _hist;
	G4VTouchable *hist = &_hist;
	fNavigator->LocateGlobalPointAndUpdateTouchable(r0, hist, false);
	const G4VPhysicalVolume *aVertexVolume = hist->GetVolume(0);
	const G4Material *aVertexMaterial = (aVertexVolume)?aVertexVolume->GetLogicalVolume()->GetMaterial():0;
	info.fVertexX = r0.x()/mm, info.fVertexY = r0.y()/mm, info.fVertexZ = r0.z()/mm;
	info.fVertexMaterial = (aVertexMaterial ? aVertexMaterial->GetName() : "-");
	info.fVertexVolume = (aVertexVolume ? aVertexVolume->GetName() : "-");
	info.fVertexVolumeCopy = (aVertexVolume ? aVertexVolume->GetCopyNo() : -1);
	info.fVertexDirectionX = d0.x(), info.fVertexDirectionY = d0.y(), info.fVertexDirectionZ = d0.z();
	info.fVertexEnergy = aTrack->GetKineticEnergy()/keV;
	info.fLastX = info.fVertexX, info.fLastY = info.fVertexY, info.fLastZ = info.fVertexZ;
	info.fLastMaterial = info.fVertexMaterial;
	info.fLastVolume = info.fVertexVolume;
	info.fLastVolumeCopy = info.fVertexVolumeCopy;
	info.fLastDirectionX = info.fVertexDirectionX, info.fLastDirectionY = info.fVertexDirectionY, info.fLastDirectionZ = info.fVertexDirectionZ;
	info.fLastEnergy = info.fVertexEnergy;
      }
      const G4VProcess *aProcess = (aStep)?aStep->GetPostStepPoint()->GetProcessDefinedStep():0;
      info.fLastProcess = (aProcess ? aProcess->GetProcessName() : "-"); // not reliable
      info.fLength = aTrack->GetTrackLength()/mm;
      info.fGlobalTime = aTrack->GetGlobalTime()/s;
      info.fLocalTime = aTrack->GetLocalTime()/s;
      info.fNsteps = aTrack->GetCurrentStepNumber();
      if (fDumpStore) info.Print();
      FillTree();
    }
    if (fVerbose > 2) {
      const TrackCacheValue *track = GetTrack(aTrack->GetTrackID());
      G4cout << "==track_info--";
      G4cout <<"\tiev=" << fEventID
	     <<"\titr=" << aTrack->GetTrackID() 
	     <<"\t" << aTrack->GetParentID()
	     <<"\tst=" << aTrack->GetTrackStatus()
	     <<"\tpart=" << aTrack->GetDefinition()->GetParticleName();
      G4cout << "\tc=" << track <<" "<< fVerbose <<" "<< sel << G4endl;
    }
    return sel;
}

int EXOTrackingAction::FillTree()
{
#ifdef HAVE_ROOT
  if (fTreeFailed) return -1;
  if (!fTree) {
    // tree name
    if (!gDirectory->IsWritable()) {
      G4cerr << "EXOTrackingAction: The current ROOT directory is not writeable. " << G4endl;
      fTreeFailed = true;
      return -1;
    }
    fTree = new TTree(EXOMiscUtil::GetMCTrackTreeName().c_str(),EXOMiscUtil::GetMCTrackTreeDescription().c_str());
    fTree->Branch(EXOMiscUtil::GetMCTrackBranchName().c_str(), &fStore);
  }
  int res = fTree->Fill();
  if (res==-1) {
    G4cerr << "EXOTrackingAction: TTree::Fill() failed. Stop further saving. " << G4endl;
    fTreeFailed = true;
  }
  return res;
#else
  return -1;
#endif
}
