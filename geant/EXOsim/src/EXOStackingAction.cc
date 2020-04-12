#include "EXOSim/EXOStackingAction.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4Ions.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOSim/EXOTrackingKiller.hh"
#include "G4EventManager.hh"
#include "EXOSim/EXOTrackingAction.hh"
#include "G4SystemOfUnits.hh" // confuses TString

EXOStackingAction::EXOStackingAction() : 
  nphotontime(0),
  nbetadecay(0),
  kill_low_energy_particles(false),
  print_warning(true),
  fKiller(0)
{

}

EXOStackingAction::~EXOStackingAction()
{
}

G4ClassificationOfNewTrack EXOStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{

  // if the new particle is a primary or a daughter of a primary, record its
  // species and energy 

  G4String particleName = aTrack->GetDefinition()->GetParticleName();
  G4String particleType = aTrack->GetDefinition()->GetParticleType();
  G4double charge = aTrack->GetDefinition()->GetPDGCharge();
  G4int parentid = aTrack->GetParentID();
  G4int trackid = aTrack->GetTrackID();
  G4double etotal = aTrack->GetKineticEnergy();
  G4double time = aTrack->GetGlobalTime();
  G4int step = aTrack->GetCurrentStepNumber();
  G4ThreeVector location = aTrack->GetPosition();

  // Warn if we see geantinos, since they are often a symptom that gps was used
  // incorrectly. If /gps/source/add is used and not preceded by
  // /gps/source/clear, geantinos are emitted. This is an issue when we consider
  // efficiency per source decay. 
  static bool haveWarnedAboutGeantinos = false; // only warn once for geantinos
  if ((!haveWarnedAboutGeantinos) && (particleName == "geantino")) {
    G4Exception(
      "EXOStackingAction",
      "This is a geantino!!",
      JustWarning,
      "It is normal to see geantinos while checking for geometry overlaps, but this could indicate a problem with gps commands in the .mac file otherwise."
    );
    haveWarnedAboutGeantinos = true;
    LogEXOMsg("Some tracks were geantinos", EEWarning);
  }

  if ( fParticleInformation.size() < MAXPARTICLE && particleName != "opticalphoton" && 
       step == 0 ) {
    
    PartInfo new_part;
    new_part.particle_energy = etotal;
    new_part.particle_charge = charge;
    new_part.particle_parentid = parentid;
    new_part.particle_trackid = trackid;
    new_part.particle_time = time;
    new_part.particle_atomicmass = 0;
    new_part.particle_excitationenergy = 0.0;
    new_part.particle_x0 = location[0];
    new_part.particle_y0 = location[1];
    new_part.particle_z0 = location[2];

    if ( particleType == "nucleus" && particleName != "alpha" ) {
      new_part.particle_type = 0;
      G4ParticleDefinition const *partdef = aTrack->GetDefinition();
      G4Ions *ion = (G4Ions*)partdef;
      new_part.particle_atomicmass = ion->GetAtomicMass();
      new_part.particle_excitationenergy = ion->GetExcitationEnergy();
    }
    else if ( particleName == "e-" ) {
      new_part.particle_type = 1;
    }
    else if ( particleName == "e+" ) {
      new_part.particle_type = 2;
    }
    else if ( particleName == "gamma" ) {
      new_part.particle_type = 3;
    }
    else if ( particleName == "alpha" ) {
      new_part.particle_type = 4;
      new_part.particle_atomicmass = 4;
     }
    else if ( particleType == "lepton" && fabs(charge) < 0.001 ) {
      new_part.particle_type = 5;
    }
    else if ( particleName == "mu-" || particleName == "mu+" ) {
      new_part.particle_type = 6;
    }
    else {
      new_part.particle_type = 7;
    }
  
    fParticleInformation.push_back(new_part);
  

  }

  if ( fParticleInformation.size() == MAXPARTICLE && print_warning == true ) {
    G4cout << "EXOStackingAction: too many generated particles to record them all" 
	   << G4endl;
    print_warning = false;
  }

  // Look out for beta decays, and record the Q value

  if ( particleType == "lepton" && fabs(charge) < 0.001 &&
       beta_decay_candidate == false && nbetadecay < MAXBETADECAY ) {
    beta_decay_candidate = true;
    qvalue[nbetadecay] = etotal;
    betadecaytime[nbetadecay] = time;      
  }
  else if ( beta_decay_candidate == true &&
            (particleName == "e+" || particleName == "e-" ) ) {
    qvalue[nbetadecay] += etotal;
    nbetadecay++;
    beta_decay_candidate = false;
  }
  else {
    beta_decay_candidate = false;
    qvalue[nbetadecay] = 0.0;
    betadecaytime[nbetadecay] = 0.0;
  }

// Count the number of optical photons generated

  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()) { 

    // See if we have already counted a photon occuring at this time

    bool found_it = false;
    G4int i = 0;
    while ( i < nphotontime && found_it == false ) {
      if ( fabs(time - photontime[i]) < 1.0*microsecond ) {
	photoncount[i]++;
	found_it = true;
      }
      i++;
    }

    // if not, make a new entry in the list

    if ( found_it == false ) {
      if ( nphotontime < MAXPHOTONTIME ) {   
        photontime[nphotontime] = time;
        photoncount[nphotontime] = 1;
        nphotontime++;
      }
      else {
	G4cout << "photons found at too many times" << G4endl;
      }
    }

  } // particle was an optical photon


  if (fKiller && fKiller->ConsiderKill(aTrack,false)) {
    EXOTrackingAction *ta = (EXOTrackingAction*)G4EventManager::GetEventManager()->GetUserTrackingAction();
    ta->StoreTrack(aTrack);
    return fKill;
  }

  // See if we should kill this particle for being
  // low energy and far away from the LXe

  bool kill_it = false;
  G4double rpart = 0.0;
  G4double zpart_abs = 0.0;
  if ( kill_low_energy_particles == true ) {

    rpart = sqrt(location[0]*location[0] +
      location[1]*location[1]);
    zpart_abs = fabs(location[2]);

    if ( particleName == "alpha" &&
	 rpart > CUCRY2INNERRAD &&
	 zpart_abs > CUCRY2HEIGHT ) {
      kill_it = true;
    }



    else if ( particleName == "e-" ||
              particleName == "gamma" ) {


      if ( etotal < 40*keV &&
	   rpart > CUCRY2INNERRAD &&
	   zpart_abs > CUCRY2HEIGHT ) {
        kill_it = true;
      }

    }

  }



  if ( kill_it == true ) {
    //        G4cout << "killing this guy: particleName = " << particleName << 
    //              ", and etotal = " << etotal/keV  << " keV, and rpart = " << rpart/cm <<
    //              " cm and zpart_abs = " << zpart_abs/cm << " cm" << G4endl;
    return fKill;
  }
  else {


  // If a photon has been created track it at the end so that 2 identical events can be created
  // (Related to the Alpha team's anticorrelation work)
  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){ 
    return fWaiting;	
  }
   return fUrgent;
  }



}

void EXOStackingAction::NewStage()
{
}

void EXOStackingAction::PrepareNewEvent()
{ 

  fParticleInformation.clear(); 
  for ( G4int i = 0; i < MAXPHOTONTIME; i++ ) {
    photoncount[i] = 0;
    photontime[i] = -999.9*microsecond;
    nphotontime = 0;
  }
    
  nbetadecay = 0;
  beta_decay_candidate = false;
  for ( G4int i = 0; i < MAXBETADECAY; i++ ) {
    qvalue[i] = 0.0;
    betadecaytime[i] = 0.0;
  }


}


G4int EXOStackingAction::get_photoncount( G4double t0, G4double t1 )
{
  // loop through the photon counts and return the number of 
  // photons generated between t0 and t1

  G4double t0slop = t0 - 1.0*microsecond;
  G4double t1slop = t1 + 1.0*microsecond;

  G4int event_photon_count = 0;
  for ( G4int i = 0; i < nphotontime; i++ ) {
    if ( photontime[i] > t0slop &&
    	 photontime[i] < t1slop ) event_photon_count += photoncount[i];
  }

  return event_photon_count;
  
}

G4double EXOStackingAction::get_qvalue( G4double t0, G4double t1 )
{

  G4double t0slop = t0 - 1.0*microsecond;
  G4double t1slop = t1 + 1.0*microsecond;

  for ( G4int i = 0; i < nbetadecay; i++ ) {
    if ( betadecaytime[i] > t0slop &&
	 betadecaytime[i] < t1slop ) {
      return qvalue[i];
    }
  }

  return 0.0;
}

void EXOStackingAction::print_particle_info()
{
  G4cout << "ID#\tflavor\tparent\tcharge\tenergy\ttime" << G4endl;
  for ( size_t i = 0; i < fParticleInformation.size(); i++ ) {
    PartInfo& part = fParticleInformation[i];
    G4cout << part.particle_trackid <<"\t"
	   << part.particle_type <<"\t"
	   << part.particle_parentid <<"\t"
	   << part.particle_charge <<"\t"
	   << part.particle_energy/keV <<"\t"
	   << part.particle_time/second << G4endl;
  }
}

void EXOStackingAction::get_part_info( G4double t0, G4double t1,
				       G4int *npart, 
				       G4int *parttype, 
				       G4double *partenergy, 
				       G4double *partcharge,
				       G4int *partA,
				       G4double *partexcited,
				       G4double *position )
{

  G4double t0slop = t0 - 1.0*microsecond;
  G4double t1slop = t1 + 1.0*microsecond;

  for ( G4int i = 0; i < *npart; i++ ) {
    parttype[i] = 0;
    partenergy[i] = 0.0;
    partcharge[i] = 0.0;
    partA[i] = 0;
    partexcited[i] = 0.0;
  }
  G4int maxnpart = *npart;
  *npart = 0;

  G4int primary_particle = -1;

  for ( size_t i = 0; i < fParticleInformation.size(); i++ ) {

    PartInfo& part = fParticleInformation[i];
    bool keepit = false;

    // See if we can find the parent
    PartInfo* jparent = NULL;
    for ( size_t j = 0; j < i; j++ ) {
      if ( fParticleInformation[j].particle_trackid == part.particle_parentid ) {
 	jparent = &fParticleInformation[j];
        break;
      }
    }

    // See if we can find a daughter

    PartInfo* jdaughter = NULL;
    for (size_t j=i+1;j<fParticleInformation.size();j++) {
      if ( fParticleInformation[j].particle_parentid == part.particle_trackid ) {
        jdaughter = &fParticleInformation[j];
        break;
      }
    }

    // Keep any particle in the time window with parent = 0

    if ( part.particle_time > t0slop &&
	 part.particle_time < t1slop &&
	 part.particle_parentid == 0 ) {
      keepit = true;
      primary_particle = i;
    }

    // Keep any particle in the time window
    // whose parent was born before the time window

    if ( jparent != NULL ) {
      if ( part.particle_time > t0slop && 
	   part.particle_time < t1slop &&
	   jparent->particle_time < t0slop ) {
	keepit = true;
      }
    }

    // Keep any particle born before the time window
    // whose daughter was born in the time window

    if ( jdaughter != NULL ) {
      if ( part.particle_time < t0slop &&
           jdaughter->particle_time > t0slop &&
	   jdaughter->particle_time < t1slop ) {
	keepit = true;
	primary_particle = i;
      }
    }

    // Keep any particle born in the time window
    // whose parent is a nucleus

    if ( jparent != NULL ) {
      if ( part.particle_time > t0slop &&
           part.particle_time < t1slop &&
	   jparent->particle_type == 0 ) {
	keepit = true;
      }
    }

    // If the particle is a nucleus, and it decays after
    // the time window, don't keep it

    if ( keepit == true && part.particle_type == 0 ) {
      if ( jdaughter != NULL ) {
	if ( jdaughter->particle_time > t1slop ) keepit = false;
      }
      else {
	keepit = false;
      }
    }

    // If we are keeping it, add it to the list

    if ( keepit == true && (*npart) < maxnpart ) { 
      parttype[*npart] = part.particle_type;
      partenergy[*npart] = part.particle_energy;
      partcharge[*npart] = part.particle_charge;
      partA[*npart] = part.particle_atomicmass;
      partexcited[*npart] = part.particle_excitationenergy;
      (*npart)++;
    }

  }

  if ( primary_particle >= 0 ) {
    position[0] = fParticleInformation[primary_particle].particle_x0;
    position[1] = fParticleInformation[primary_particle].particle_y0;
    position[2] = fParticleInformation[primary_particle].particle_z0;
  }
  else {
    position[0] = -999.0;
    position[1] = -999.0;
    position[2] = -999.0;
  }

  return;

}

G4bool EXOStackingAction::Bi214Po214_event( G4double t0, G4double t1 )
{

  G4double t0slop = t0 - 1.0*microsecond;
  G4double t1slop = t1 + 1.0*microsecond;

  for ( size_t i = 0; i < fParticleInformation.size(); i++ ) {
    PartInfo& part = fParticleInformation[i];
    if ( part.particle_time > t0slop &&
         part.particle_time < t1slop &&
	 fabs(part.particle_charge-84.0) < 0.01 &&
	 part.particle_atomicmass == 214 ) return true;
  }

  return false;

}

void EXOStackingAction::set_kill_low_energy_particles( G4bool val )
{
  if ( val == true ) {
    G4cout << "******************************************************" << G4endl;
    G4cout << "Alphas and low energy electrons and gammas outside the" << G4endl;
    G4cout << "TPC vessel will be killed" << G4endl;
    G4cout << "******************************************************" << G4endl;
  }
  else {
    //    G4cout << "***************************************************************" << G4endl;
    //    G4cout << "All particles will be tracked regardless of energy and location" << G4endl;
    //    G4cout << "***************************************************************" << G4endl;
  }

  kill_low_energy_particles = val;
    
}
      
