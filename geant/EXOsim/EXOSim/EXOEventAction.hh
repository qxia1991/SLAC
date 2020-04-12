//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef EXOEventAction_h
#define EXOEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "EXOUtilities/EXOTClonesArray.hh"
#include "EXOSim/EXOEventActionMessenger.hh"
#include "EXOSim/EXOLXeHit.hh"
#include "EXOSim/EXOAPDHit.hh"
#include "EXOSim/EXOAPDInternalHit.hh"
#include "EXOSim/EXOPassiveMaterialHit.hh"
#include "EXOSim/EXOVetoPanelHit.hh"

class EXORunAction;
class EXOStackingAction;
class EXOSteppingAction;
class EXOEventData;
class EXOMonteCarloData;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class EXOEventAction : public G4UserEventAction
{
public:
  EXOEventAction(EXORunAction*,
                 EXOStackingAction*,
                 EXOSteppingAction*);
  
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  
  void set_anticorrelationATeam( G4bool value );
  void force_static_event_window_time( G4double value );
  void set_keeplxeonly( G4bool value );
  void SetKeepVetoPanelOnly( G4bool value );
  void SetKeepLXeOrVetoPanel( G4bool value );
  void SetKeepLXeAndVetoPanel( G4bool value );
  void set_print_particle_info( G4bool value );
  void set_save_particle_info( G4bool value );
  void SetUnixTimeOfEvents(G4int value) {fUnixTimeOfEvent = value;}

  EXOEventData* GetNextEvent(); 

protected:
  
  void set_total_event_window_time( G4double value );
  G4double get_lxe_energy_info( EXOMonteCarloData& mc );
  G4double GetVetoPanelInfo( EXOMonteCarloData& mc );
  void fill_primary_event_info( EXOMonteCarloData& mc );
  void get_passive_material_energy( EXOMonteCarloData& mc );
  void get_APD_info( EXOMonteCarloData& mc );
  void get_internal_APD_energy( EXOMonteCarloData& mc );
  void do_tpc_event_grouping( EXOMonteCarloData& mc );

  void fast_APD_simulation( EXOMonteCarloData &mc, 
                            G4int &num_APD1_hits, 
			    G4int &num_APD2_hits );
  void do_recombination( G4int &sum_photons, 
                         G4int &num_APD1_hits, 
			 G4int &num_APD2_hits, 
                         G4double& total_ionization );
  void get_acceptance( G4int hit_number, G4double &pacceptance1,
		       G4double &pacceptance2 ); 
  G4int recombine_on_grid( G4int &num_APD1_hits, 
			   G4int &num_APD2_hits, 
                           const std::vector<int>& hit_list );
  G4double get_gain( G4double zpos);

  G4double GetCurrentEventWindowBeginTime();
  G4double GetCurrentEventWindowEndTime();
  G4bool ChargeHitInTimeWindow(const EXOVHit& hit);
  G4bool HitInTimeWindow(const EXOVHit& hit);


  EXOEventActionMessenger     fEvtMsg;
  EXOTClonesArray             fCurrentEvents;
  TObjArrayIter               fCurrentIter;
  G4double                    fTimeZero;

  EXORunAction*               fRunAction;
  EXOStackingAction*          fStackingAction;
  EXOSteppingAction*          fStepAction;

  G4int                       fLXeHitCollectionID;
  G4int                       fAPDHitCollectionID;
  G4int                       fAPDInternalHitCollectionID;
  G4int                       fPassiveMaterialHitCollectionID;  
  G4int                       fVetoPanelHitCollectionID;

  G4bool                      fProcessLXeEventsOnly;
  G4bool                      fProcessVetoPanelEventsOnly;
  G4bool                      fProcessLXeOrVetoPanelEventsOnly;
  G4bool                      fProcessLXeAndVetoPanelEventsOnly;
  G4int                       fTruthTest;

  G4bool                      fDoAntiCorrelationATeam;
  G4bool                      fPrintParticleInfo;
  G4bool                      fDoStaticWindowing;
  G4bool                      fSaveParticleInfo;
  G4double                    fTotalEventWindowTime;
  unsigned int                fUnixTimeOfEvent; // If not set (0), use current time for each event

  G4int                       fEventCountg4;
  G4int                       fEventCountexo;
  G4int                       fEventCountlxe;
  G4int                       fEventCountVetoPanels;

  EXOLXeHitsCollection*             fLXeHC;
  EXOAPDHitsCollection*             fAPDHC;
  EXOAPDInternalHitsCollection*     fAPDInternalHC;
  EXOPassiveMaterialHitsCollection* fPassiveMaterialHC;
  EXOVetoPanelHitsCollection*       fVetoPanelHC;

  G4int fCurrentMirrorNumber;

};

#endif

    
