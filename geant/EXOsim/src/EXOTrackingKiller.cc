#include "EXOSim/EXOTrackingKiller.hh"
#include "EXOSim/EXODetectorConstruction.hh"

#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
//#include <G4TrackStatus.hh>
#include <G4Step.hh>
#include <G4VProcess.hh>
#include <G4TouchableHistory.hh>

#include <G4ParticleTable.hh>
#include <G4IonTable.hh>
#include <G4ProcessTable.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOTrackingKiller::EXOTrackingKiller(EXODetectorConstruction* det ) : 
  fDetector(det),
  fTrackingKillerMessenger(this)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4bool EXOTrackingKiller::AddKiller(const G4String& value)
{
  G4bool ret = false;
  G4String::size_type i = 0, j = 0, k = 0, l = 0;
  ParticleKiller killer;
  killer.fParticle = 0;
  killer.fParticleInverted = false;
  killer.fVolume = 0;
  killer.fVolumeWithDaughters = false;
  killer.fVolumeInverted = false;
  killer.fProcess = 0;
  killer.fProcessInverted = false;
  killer.fEnergy = -1;
  killer.fEnergyInverted = false;
  killer.fEnergyCompare = 0;
  j = value.find_first_not_of(" ",0);
  while (j!=G4String::npos && k<999) {
    i = value.find_first_of(" ",j);
    // type [not] <particle> in [not] [deep] <volume> from [not] <process> keV (<|=|>|<=|!=|>=) <energy>
    switch(k) {
    case 0: // common entry
      if      (!value.compare(j,i-j,"type")) k = 1;
      else if (!value.compare(j,i-j,"in"))   k = 2;
      else if (!value.compare(j,i-j,"from")) k = 3;
      else if (!value.compare(j,i-j,"keV"))  k = 4;
      else {
	G4cerr << __func__ << ": extra symbols on the line: '" << value.substr(j) <<"'"<< G4endl;
	k = 999;
      }
      break;
    case 1: // particle
      if (!value.compare(j,i-j,"not"))
        killer.fParticleInverted = true;
      else if (!value.compare(j,i-j,"any"))
        killer.fParticle = 0, k = 0;
      else {
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String name = value.substr(j,i-j);
        bool is_ion = name.find('[')!=std::string::npos && !islower(name[0]); // definitely ion
        if (!is_ion) 
          killer.fParticle = particleTable->FindParticle(name); // maybe regular particle
        if (is_ion || killer.fParticle==0) { // ... maybe not
	  G4IonTable *ionTable = particleTable->GetIonTable();
	  G4String::size_type i1 = 0, j1 = 0;
	  int L = 0, A = 0; double E = 0;
	  // corresponds to G4IonTable::GetIonName()
	  while (name[i1]=='L') i1++; L = i1;
	  i1 = name.find_first_of("0123456789",i1);
	  if (i1!=G4String::npos) A = strtol(name.c_str()+i1,0,0);
	  j1 = name.find('[',i1+1);
	  if (j1!=G4String::npos) E = strtof(name.c_str()+j1,0);
	  for (int Z=1;Z<=ionTable->GetNumberOfElements();Z++) 
	    if (ionTable->GetIonName(Z,A,L,E)==name.c_str()) {
	      killer.fParticle = ionTable->GetIon(Z,A,L,E); break; }
	}
	if (killer.fParticle) k = 0;
	else {
	  G4cerr << __func__ << ": can't find particle '" << name <<"'"<< G4endl;
	  return false;
	}
      }
      break;
    case 2: // volume
      if (!value.compare(j,i-j,"not"))
        killer.fVolumeInverted = true;
      else if (!value.compare(j,i-j,"deep"))
        killer.fVolumeWithDaughters = true;
      else if (!value.compare(j,i-j,"any"))
        killer.fVolume = 0, k = 0;
      else {
	G4String name = value.substr(j,i-j);
	G4String::size_type i1 = name.find(':');
	G4int j1 = (i1!=G4String::npos) ? strtol(name.c_str()+i1+1,0,0) : 0;
        killer.fVolume = fDetector->FindVolume(name.substr(0,i1),j1);
        if (killer.fVolume) k = 0;
        else {
          G4cerr << __func__ << ": can't find volume '" << name <<"'"<< G4endl;
          return false;
        }
      }
      break;
    case 3: // process
      if (!value.compare(j,i-j,"not"))
        killer.fProcessInverted = true;
      else if (!value.compare(j,i-j,"any"))
        killer.fProcess = 0, k = 0;
      else {
	G4String name = value.substr(j,i-j);
	G4ProcessTable *ptable = G4ProcessTable::GetProcessTable(); // const
	const G4ProcessVector *plist = ptable->FindProcesses(name);
	if (plist->size()>1) {
          G4cerr << __func__ << ": warning: found " << plist->size() 
		 << " processes '" << name << "', using first one"<< G4endl;
	}
	if (plist->size()>0) killer.fProcess = (*plist)[0];
	if (killer.fProcess) k = 0;
        else {
          G4cerr << __func__ << ": can't find process '" << name <<"'"<< G4endl;
          return false;
        }
      }
      break;
    case 4: // energy
      if      (!value.compare(j,i-j,"="))
        killer.fEnergyInverted = false, killer.fEnergyCompare = 0;
      else if (!value.compare(j,i-j,"!="))
        killer.fEnergyInverted = true,  killer.fEnergyCompare = 0;
      else if (!value.compare(j,i-j,"<"))
        killer.fEnergyInverted = false, killer.fEnergyCompare = -1;
      else if (!value.compare(j,i-j,">="))
        killer.fEnergyInverted = true,  killer.fEnergyCompare = -1;
      else if (!value.compare(j,i-j,">"))
        killer.fEnergyInverted = false, killer.fEnergyCompare = 1;
      else if (!value.compare(j,i-j,"<="))
        killer.fEnergyInverted = true,  killer.fEnergyCompare = 1;
      else if (!value.compare(j,i-j,"any"))
        killer.fEnergy = -1, k = 0;
      else {
	G4String num = value.substr(j,i-j);
	G4double val = strtof(num.c_str(),0);
	if (val>=0)
	  killer.fEnergy = val, k = 0;
	else {
          G4cerr << __func__ << ": bad energy value '" << num <<"'"<< G4endl;
          return false;
	}
      }
      break;
    default:
      G4cerr << __func__ << ": extra symbols on the line: '" << value.substr(j) <<"'"<< G4endl;
      k = 999;
    }
    j = value.find_first_not_of(" ",i);
  }

  if (killer.fParticle || killer.fVolume || killer.fProcess || killer.fEnergy>=0.)
    fKillers.push_back(killer), ret = true;
  return ret;
}
G4String EXOTrackingKiller::GetKillerList() const
{
  G4String list;
  KillerList::const_iterator killer;
  for (killer=fKillers.begin();killer!=fKillers.end();killer++) {
    list += "  type ";
    if (killer->fParticleInverted) list += "not ";
    if (killer->fParticle) {
      list += killer->fParticle->GetParticleName(); list += ' ';
    } else list += "any ";
    list += "in ";
    if (killer->fVolumeInverted) list += "not ";
    if (killer->fVolume) {
      if (killer->fVolumeWithDaughters) list += "deep ";
      list += killer->fVolume->GetName();
      char buf[32]; snprintf(buf,32,":%d ",killer->fVolume->GetCopyNo()); list += buf;
    } else list += "any ";
    list += "from ";
    if (killer->fProcessInverted) list += "not ";
    if (killer->fProcess) {
      list += killer->fProcess->GetProcessName(); list += ' ';
    } else list += "any ";
    list += "keV ";
    if (killer->fEnergy>=0.) {
      list += ( (killer->fEnergyCompare < 0) ? 
		(killer->fEnergyInverted?">= ":"< ") :
		( (killer->fEnergyCompare == 0) ? 
		  (killer->fEnergyInverted?"!= ":"= ") :
		  (killer->fEnergyInverted?"<= ":"> ") ) );
      char buf[32]; snprintf(buf,32,"%lf ",killer->fEnergy); list += buf;
    } else list += "any ";
    list += "\n";
  }
  return list;
}

inline int compare(const G4double &a, const G4double &b) { return ((a<b)?-1:((a==b)?0:1)); }
inline bool compare(const G4double &a, const G4double &b, int mode) { return ((mode<0)?(a<b):((mode==0)?(a==b):(a>b))); }

G4bool EXOTrackingKiller::ConsiderKill(const G4Track *track, G4bool post_point) const
{
  // vertex created particles are well undefined untill first step
  G4bool kill = false;
  KillerList::const_iterator killer;
  for (killer=fKillers.begin();killer!=fKillers.end();killer++) {
    if ( ( killer->fParticleInverted != // xor
	   (!killer->fParticle || track->GetDefinition() == killer->fParticle) ) && 
	 ( killer->fProcessInverted != // xor
	   (!killer->fProcess || track->GetCreatorProcess() == killer->fProcess) ) && 
	 ( killer->fEnergyInverted != // xor
	   (killer->fEnergy==-1. || compare(track->GetKineticEnergy()/keV,killer->fEnergy,killer->fEnergyCompare) ) ) ) {

      if (killer->fVolume && killer->fVolumeWithDaughters) {
        G4TouchableHistory _hist;
	const G4VTouchable *hist = (post_point)?track->GetNextTouchable():track->GetTouchable();
        if (!hist) {
          // some f***ing processes does not set track properly, workaround that
          hist = &_hist;
          fDetector->LocateGlobalPoint(track->GetPosition(),&_hist);
        }
	G4bool res = false; // found?
	if (hist)
	  for (int i=0;i<hist->GetHistoryDepth();i++) {
	    //G4cout << "volu " << i <<" "<< hist->GetVolume(i)->GetName() << G4endl;
	    if (hist->GetVolume(i) == killer->fVolume) { res = true; }
	  }
	if ( killer->fVolumeInverted != res ) { // xor
	  kill = true;
	}
      } else {
	const G4VTouchable *hist = (post_point)?track->GetNextTouchable():track->GetTouchable();
	const G4VPhysicalVolume* volume = (hist)?hist->GetVolume(0):0;
	if ( killer->fVolumeInverted != // xor
	     (!killer->fVolume || volume == killer->fVolume) ) {
	  kill = true;
	}
      }
    }
  }
  if (0) {
    const G4VTouchable *hist = track->GetTouchable();
    const G4VPhysicalVolume* volume = (hist)?hist->GetVolume(0):0;
    G4cout << __func__ <<" type "<< track->GetDefinition()->GetParticleName() <<" in "<< (volume?volume->GetName():"(null)") <<" from "<< ((track->GetCreatorProcess())?track->GetCreatorProcess()->GetProcessName():"(null)") << " keV " << (track->GetKineticEnergy()/keV) << " = " << kill <<" "<< 0 << G4endl;
  }
  return kill;
}
