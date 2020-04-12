#ifndef EXOStackingAction_H
#define EXOStackingAction_H 1

#define MAXPARTICLE 100000
#define MAXPHOTONTIME 100
#define MAXBETADECAY 100

#include "G4UserStackingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>

class EXOTrackingKiller;

class EXOStackingAction : public G4UserStackingAction
{
  public:
    EXOStackingAction();
    virtual ~EXOStackingAction();
   public:

    class PartInfo {
     public:
      G4int particle_type;
      G4double particle_energy;
      G4int particle_trackid;
      G4int particle_parentid;
      G4double particle_charge;
      G4double particle_time;  
      G4int particle_atomicmass;
      G4double particle_excitationenergy;
      G4double particle_x0;
      G4double particle_y0;
      G4double particle_z0;
    };
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

    G4int get_photoncount( G4double t0, G4double t1 );
    G4double get_qvalue( G4double t0, G4double t1 );
    void print_particle_info();
    void get_part_info( G4double t0, G4double t1, G4int *npart, 
		        G4int *parttype, G4double *partenergy,
			G4double *partcharge, G4int *partA,
			G4double *partexcited, G4double *position );
    G4bool Bi214Po214_event( G4double t0, G4double t1 );
    void set_kill_low_energy_particles( bool val );

    void SetTrackingKiller(EXOTrackingKiller *killer) { fKiller = killer; }

  private:

    std::vector<PartInfo> fParticleInformation;

    G4int nphotontime;
    G4double photontime[MAXPHOTONTIME];
    G4int photoncount[MAXPHOTONTIME];

    G4bool beta_decay_candidate;
    G4int nbetadecay;
    G4double qvalue[MAXBETADECAY];
    G4double betadecaytime[MAXBETADECAY];

    G4bool kill_low_energy_particles;
    G4bool print_warning;

    EXOTrackingKiller *fKiller; // we don't own it
};
#endif




