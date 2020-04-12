#include "G4Event.hh"
#include "G4EventManager.hh"
#include "TTimeStamp.h"
#include "G4HCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"

#include "EXOSim/EXOEventAction.hh"
#include "EXOSim/EXORunAction.hh"
#include "EXOSim/EXOStackingAction.hh"
#include "EXOSim/EXOSteppingAction.hh"

#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOEventData.hh"

#include <math.h>
#include <vector>

using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXOEventAction::EXOEventAction(EXORunAction* run,
                               EXOStackingAction* stack,
                               EXOSteppingAction* step)
  :
   fEvtMsg(this),
   fCurrentEvents(EXOEventData::Class()),
   fCurrentIter(&fCurrentEvents),
   fTimeZero(0.0),
   fRunAction(run),
   fStackingAction(stack),
   fStepAction(step),
   fLXeHitCollectionID(-1),
   fAPDHitCollectionID(-1),
   fAPDInternalHitCollectionID(-1),
   fPassiveMaterialHitCollectionID(-1),
   fVetoPanelHitCollectionID(-1),
   fProcessLXeEventsOnly(false),
   fProcessVetoPanelEventsOnly(false),
   fProcessLXeOrVetoPanelEventsOnly(false),
   fProcessLXeAndVetoPanelEventsOnly(false),
   fTruthTest(-1),
   fDoAntiCorrelationATeam(false),
   fPrintParticleInfo(false),
   fDoStaticWindowing(false),
   fSaveParticleInfo(true),
   fTotalEventWindowTime(0.0),
   fUnixTimeOfEvent(0),
   fEventCountg4(0),
   fEventCountexo(0),
   fEventCountlxe(0),
   fEventCountVetoPanels(0),
   fLXeHC(NULL),
   fAPDHC(NULL),
   fAPDInternalHC(NULL),
   fPassiveMaterialHC(NULL),
   fVetoPanelHC(NULL)
{

  set_total_event_window_time( 2.0*microsecond );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXOEventAction::BeginOfEventAction(const G4Event* /*evt*/)
{

  static bool first_call = true;
  // Import trigger time from EXODigitizer
  // FixME need to deal with the trigger time.
  //fTriggerTime = fDig.get_trigger_time();


  G4SDManager *SDman = G4SDManager::GetSDMpointer();
  // Get the LXe hits collection ID

  if (fLXeHitCollectionID==-1) {
    fLXeHitCollectionID = SDman->GetCollectionID("LXeHitsCollection");
  }

  // Get the APD hits collection ID

  if (fAPDHitCollectionID==-1) {
    fAPDHitCollectionID = SDman->GetCollectionID("APDHitsCollection");
  }

  // Get the Passive Material hits collection ID

  if (fPassiveMaterialHitCollectionID==-1) {
    fPassiveMaterialHitCollectionID = SDman->GetCollectionID("PassiveMaterialHitsCollection");
  }

  // Get the veto panel hits collection ID
  if (fVetoPanelHitCollectionID == -1) {
    fVetoPanelHitCollectionID = SDman->GetCollectionID("VetoPanelHitsCollection");
  }

  // Get the APD Internal hits collection ID

  if (fAPDInternalHitCollectionID==-1) {
    fAPDInternalHitCollectionID = SDman->GetCollectionID("APDInternalHitsCollection");
  }

  // Initialize the list of step times in the SteppingAction
  // Reset the stepping action in general.
  fStepAction->Reset();

  // Print stuff out on the first call

  if ( first_call == true ) {
    G4cout << "*************************************************************" << G4endl;
    if ( fDoStaticWindowing == true ) {
      G4cout << "EXOEventAction:: total event window time will be "<< G4endl;
      G4cout << fTotalEventWindowTime/microsecond 
             << " microseconds in every event" << G4endl;
    }
    else {
      G4cout << "EXOEventAction: Using dynamic event window time" << G4endl;
      G4cout << "Bi214 decays will have event window time of 2048 microseconds" << G4endl;
      G4cout << "All others will have 512 microseconds" << G4endl;
    }
    G4cout << "*************************************************************" << G4endl;

    first_call = false;
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXOEventAction::EndOfEventAction(const G4Event* evt)
{
  // Performing EndOfEventAction, this function collects all the Geant4 events
  // and builds them into separate events according to the event window and
  // charge collection.  Each of these separate EXOEventData objects is stored
  // in local array and is accessed by GetNextEvent.  See
  // EXOGeant4Module::GetNextEvent for an example of how this is done.  


  //incrementing event counter for anticorrelation work:
  if(fRunAction->returnAlphaAnticorr()) fRunAction->countIncrement();

  // increment the G4 event count

  fEventCountg4++;

  // First see if there is any energy in the LXe

  fLXeHC = NULL;
  // See if the hit collection has been defined
  if( fLXeHitCollectionID < 0 ) {
    G4cout << "no charge info hit found in this event" << G4endl;
  }
  else {
    // address hits collections
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(HCE) {
      fLXeHC = (EXOLXeHitsCollection*)(HCE->GetHC(fLXeHitCollectionID));
    }
  }

  // Get the veto panel, APD, and passive material hit collections also

  fVetoPanelHC = NULL;
  // See if the hit collection has been defined
  if( fVetoPanelHitCollectionID < 0 ) {
    G4cout << "no veto panel hit info found in this event" << G4endl;
  }
  else {
    // address hits collections
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(HCE) {
      fVetoPanelHC = (EXOVetoPanelHitsCollection*)(HCE->GetHC(fVetoPanelHitCollectionID));
    }
  }

  fAPDHC = NULL;
  // See if the hit collection has been defined
  if( fAPDHitCollectionID < 0 ) {
    G4cout << "no APD hit info found in this event" << G4endl;
  }
  else {
    // address hits collections
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(HCE) {
      fAPDHC = (EXOAPDHitsCollection*)(HCE->GetHC(fAPDHitCollectionID));
    }
  }

  fAPDInternalHC = NULL;
  // See if the hit collection has been defined
  if( fAPDInternalHitCollectionID < 0 ) {
    G4cout << "no nternal APD hit info found in this event" << G4endl;
  }
  else {
    // address hits collections
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(HCE) {
      fAPDInternalHC = (EXOAPDInternalHitsCollection*)(HCE->GetHC(fAPDInternalHitCollectionID));
    }
  }

  fPassiveMaterialHC = NULL;
  // See if the hit collection has been defined
  if( fPassiveMaterialHitCollectionID < 0 ) {
    G4cout << "no PassiveMaterial hit info found in this event" << G4endl;
  }
  else {
    // address hits collections
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(HCE) {      fPassiveMaterialHC = (EXOPassiveMaterialHitsCollection*)
             (HCE->GetHC(fPassiveMaterialHitCollectionID));
    }
  }

  // Clear previous events
  fCurrentEvents.Clear("C");
  fCurrentIter.Reset();
  
  // Get step times, these are automatically ordered in time.
  const EXOSteppingAction::StepMirrors& stepmirror = fStepAction->get_stepmirrors();

  // The following builds the events in segments of event window times.  It
  // begins with the first time, and selects events that are within the
  // event window window.  (Each EXO event has a time equal to the event window
  // time).
  EXOSteppingAction::StepMirrors::const_iterator thisMir = stepmirror.begin();
  const G4double sec2usec = 1.e+6;  
  for( ; thisMir!= stepmirror.end(); thisMir++) {
    const EXOSteppingAction::StepTimes& steptime = thisMir->second;
    EXOSteppingAction::StepTimes::const_iterator thisTime = steptime.begin();
    fCurrentMirrorNumber = thisMir->first;
    TTimeStamp RealTime; // initializes to current time.
    while( thisTime != steptime.end() ){
    
      // record the trigger time
      EXOEventData& ed = 
        *static_cast<EXOEventData*>(fCurrentEvents.GetNewOrCleanedObject(fCurrentEvents.GetEntriesFast())); 
    
      // Initialize the unix time of the new event.  This is important, eg., for database fetching.
      if(fUnixTimeOfEvent != 0) {
        // unix time was set by the user.
        //   but also add GEANT time
        G4double t = (*thisTime)/s;
        G4double ts = floor(t);
        unsigned tu = (unsigned)floor((t - ts)*sec2usec);
        ed.fEventHeader.fTriggerSeconds = fUnixTimeOfEvent + (time_t)ts;
        ed.fEventHeader.fTriggerMicroSeconds = tu;
      }
      else {
        // unix time was not set; use the current time (which will change through a run).
        //   but also add GEANT time
        G4double t = (*thisTime)/s + RealTime;
        G4double ts = floor(t); 
        unsigned tu = (unsigned)floor((t - ts)*sec2usec);
        ed.fEventHeader.fTriggerSeconds = (time_t)ts;
        ed.fEventHeader.fTriggerMicroSeconds = tu;
      }
    
      fTimeZero = *thisTime; 
      
      // record the GEANT event numbers
    
      ed.fEventHeader.fGeant4EventNumber = evt->GetEventID();
      ed.fEventHeader.fGeant4SubEventNumber = fCurrentEvents.GetEntriesFast()-1;
      
      // Set the total event window time and charge collection time, if we are
      // not using fDoStaticWindowing
    
      if ( fDoStaticWindowing == false ) {
        
        G4double t0 = fTimeZero; 
        G4double t1 = t0 + 2048.0*microsecond;
        G4bool bi214event = fStackingAction->Bi214Po214_event(t0,t1);
        
        if ( bi214event == true ) set_total_event_window_time(2048.0*microsecond);
        else set_total_event_window_time(512.0*microsecond);
    
      }
      ed.fMonteCarloData.fEventGroupingTime = fTotalEventWindowTime;
    
      // Get generated particle information from EXOStackingAction
    
      if ( fPrintParticleInfo == true ) {
        G4cout << "***************************************************************" << G4endl;
        G4cout << Form("GEANT event number %d, subevent number = %d, EXO event number %d",
           evt->GetEventID(), (int)m, ed.fEventNumber) << G4endl;
        G4cout << "---------------------------------------------------------------" << G4endl;
      }
    
      // This function calls stacking actions which have "slop" in the time.  Can
      // we remove this?  Otherwise, it's very confusing which G4 events are built
      // in and it's important to make sure included G4 events are all consistent.  M. Marino
      fill_primary_event_info(ed.fMonteCarloData);
      
      // Add up the total energy in the LXE
    
      G4double etotallxe = get_lxe_energy_info(ed.fMonteCarloData);

      // Add up the total energy in the veto panels
      G4double eTotalVetoPanels = GetVetoPanelInfo(ed.fMonteCarloData);

      // The variables fProcess*Only handle all combinations of LXe and veto
      // events.  Note that "ProcessOnlyX" really means "disregard any event
      // that does not have deposited energy in X" (essentially "keep only
      // events deemed useful w/rt X").

      // First, make sure that there are not two or more selection criteria active
      // simultaneously.  Perform this test only once (on the first event).
      if (fTruthTest == -1) {
          if (fProcessLXeEventsOnly == true) fTruthTest++;
          if (fProcessVetoPanelEventsOnly == true) fTruthTest++;
          if (fProcessLXeOrVetoPanelEventsOnly == true) fTruthTest++;
          if (fProcessLXeAndVetoPanelEventsOnly == true) fTruthTest++;

          // More than one condition applied
          // Move to individual "Set" routines in this class?
          if (fTruthTest > 0) {
              G4cout << "*** More than one LXe/veto event selection criterion in place! ***" << G4endl;
              G4cout << "*** Please check your macro and try again. ***" << G4endl;
              fTruthTest = -1;
          }
      }
      
      // Check to see if we have any energy in the relevant volumes; stop 
      // processing the event if not.
      // LXE ONLY
      if ((fProcessLXeEventsOnly == true) && (etotallxe > 1.0*keV)) {
          // Increment the number of LXe events
          fEventCountlxe++;

          // Potentially increment the number of veto panel events
          if ( eTotalVetoPanels > 1.0*keV ) fEventCountVetoPanels++;

          // Put the passive material energy deposit info into the n-tuple
          get_passive_material_energy(ed.fMonteCarloData);

          do_tpc_event_grouping(ed.fMonteCarloData);

          // Get the monte carlo APD and internal APD info; A-team anticorrelation, if on
          get_APD_info(ed.fMonteCarloData);
          get_internal_APD_energy(ed.fMonteCarloData);
		  // GetVetoPanelInfo(ed.fMonteCarloData);
      }
	  // VETO PANELS ONLY
      else if ((fProcessVetoPanelEventsOnly == true) && (eTotalVetoPanels > 1.0*keV)) {
          fEventCountVetoPanels++;

          if ( etotallxe > 1.0*keV ) fEventCountlxe++;

          get_passive_material_energy(ed.fMonteCarloData);
          do_tpc_event_grouping(ed.fMonteCarloData);

          get_APD_info(ed.fMonteCarloData);
          get_internal_APD_energy(ed.fMonteCarloData);
		  // GetVetoPanelInfo(ed.fMonteCarloData);
      }
	  // LXE OR VETO PANELS
      else if (fProcessLXeOrVetoPanelEventsOnly == true && (etotallxe > 1.0*keV || eTotalVetoPanels > 1.0*keV)) {
          if ( etotallxe > 1.0*keV ) fEventCountlxe++;
          if ( eTotalVetoPanels > 1.0*keV ) fEventCountVetoPanels++;

          get_passive_material_energy(ed.fMonteCarloData);
          do_tpc_event_grouping(ed.fMonteCarloData);

          get_APD_info(ed.fMonteCarloData);
          get_internal_APD_energy(ed.fMonteCarloData);
		  // GetVetoPanelInfo(ed.fMonteCarloData);
      }
	  // LXE AND VETO PANELS
      else if (fProcessLXeAndVetoPanelEventsOnly == true && (etotallxe > 1.0*keV && eTotalVetoPanels > 1.0*keV)) {
          fEventCountlxe++;
          fEventCountVetoPanels++;

          get_passive_material_energy(ed.fMonteCarloData);
          do_tpc_event_grouping(ed.fMonteCarloData);

          get_APD_info(ed.fMonteCarloData);
          get_internal_APD_energy(ed.fMonteCarloData);
		  // GetVetoPanelInfo(ed.fMonteCarloData);
      }
	  // JUST RECORD ALL EVENTS, NO MATTER WHAT
      else {
          if ( etotallxe > 1.0*keV ) fEventCountlxe++;
         if ( eTotalVetoPanels > 1.0*keV ) fEventCountVetoPanels++;

          get_passive_material_energy(ed.fMonteCarloData);
          do_tpc_event_grouping(ed.fMonteCarloData);

          get_APD_info(ed.fMonteCarloData);
          get_internal_APD_energy(ed.fMonteCarloData);
		  // GetVetoPanelInfo(ed.fMonteCarloData);
      }

    
      thisTime = steptime.upper_bound( fTimeZero + fTotalEventWindowTime ); 
    } // end loop over events found by count_events
  } // End loop over mirror times

  // Add up all the events generated

  fEventCountexo += fCurrentEvents.GetEntriesFast(); 

  // Done with this event

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::set_keeplxeonly( G4bool value )
  
{
  // Function to define whether to write all events, or only those in lxe.
  if ( value == false ) {
    // G4cout << "SetKeepLXeOnly(): writing all events (lxe or not)" << G4endl;
    G4cout << "SetKeepLXeOnly(): I don't care" << G4endl;
    fProcessLXeEventsOnly = false;
    return;
  }

  G4cout << "SetKeepLXeOnly(): writing only LXe events" << G4endl;
  fProcessLXeEventsOnly = true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::SetKeepVetoPanelOnly( G4bool value )
{
  // Function to define whether to write all events, or only those in the veto panels.
  if ( value == false ) {
    G4cout << "SetKeepVetoPanelOnly(): I don't care" << G4endl;
    fProcessVetoPanelEventsOnly = false;
    return;
  }

  G4cout << "SetKeepVetoPanelOnly(): writing only veto panel events" << G4endl;
  fProcessVetoPanelEventsOnly = true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::SetKeepLXeOrVetoPanel( G4bool value )
{
  // Function to define whether to write all events, or only the union of events
  // with energy deposit in either the LXe or veto panels.
  if ( value == false ) {
    G4cout << "SetKeepLXeOrVetoPanel(): I don't care" << G4endl;
    fProcessLXeOrVetoPanelEventsOnly = false;
    return;
  }

  G4cout << "SetKeepLXeOrVetoPanel(): writing only events with energy deposit in either LXe or veto panels (possibly both)" << G4endl;
  fProcessLXeOrVetoPanelEventsOnly = true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::SetKeepLXeAndVetoPanel( G4bool value )
{
  // Function to define whether to write all events, or only the union of events
  // with energy deposit in either the LXe or veto panels.
  if ( value == false ) {
    G4cout << "SetKeepLXeAndVetoPanel(): I don't care" << G4endl;
    fProcessLXeAndVetoPanelEventsOnly = false;
    return;
  }

  G4cout << "SetKeepLXeAndVetoPanel(): writing only events with energy deposit in both LXe and veto panels" << G4endl;
  fProcessLXeAndVetoPanelEventsOnly = true;
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::set_anticorrelationATeam( G4bool value )
{
  // Use the anti-correlation by the ATeam.  FixME: This should be removed
  // eventually.
  G4cout << "***************************************************" << G4endl;
  if ( value == false ) {
    G4cout << "turning off anticorrelation" << G4endl;
    fDoAntiCorrelationATeam = false;
    return;
  }

  G4cout << "turning on anticorrelation" << G4endl;
  fDoAntiCorrelationATeam = true;
  G4cout << "***************************************************" << G4endl;

}
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::force_static_event_window_time( G4double value )
{
  fDoStaticWindowing = true;
  set_total_event_window_time( value);
}
  


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::set_total_event_window_time( G4double value )
{

  // Internal function to set the total event window time.  This is called by
  // force_static_event_window_time, or dynamically updated by the class during
  // processing of events, for example, when a Bi214 event is found.
  if ( fTotalEventWindowTime == value ) return;

  if ( value < 1.0*microsecond ) {
    G4cout << "fTotalEventWindowTime cannot be less than 1 microsecond" << G4endl;
    return;
  }

  fTotalEventWindowTime = value;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::set_print_particle_info( G4bool value )
{
  fPrintParticleInfo = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::set_save_particle_info( G4bool value )
{
  fSaveParticleInfo = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4double EXOEventAction::GetCurrentEventWindowBeginTime()
{
  // Internal function, returns the current event beginning time for
  // event-building purposes.
  return fTimeZero - 1.0*microsecond; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4double EXOEventAction::GetCurrentEventWindowEndTime()
{
  // Internal function, returns the current event end event window time. 
  return fTimeZero + fTotalEventWindowTime; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4bool EXOEventAction::HitInTimeWindow(const EXOVHit& hit)
{
  // Internal function, returns whether the time is in the hit window for
  // charge collection.  Internal functions should call this to determine if a
  // charge deposit is to be accepted in an event. 
  // This function NOW just uses GetCurrentEventWindowEndTime(), to avoid
  // hidden dependencies with digitizer and drift times.  
  return ( hit.mirrorNumber == fCurrentMirrorNumber &&
           hit.time > GetCurrentEventWindowBeginTime() && 
           hit.time < GetCurrentEventWindowEndTime()); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4bool EXOEventAction::ChargeHitInTimeWindow(const EXOVHit& hit)
{
  // Internal function, returns whether the time is in the hit window to be
  // accepted in an event. 

  return HitInTimeWindow(hit); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::fill_primary_event_info( EXOMonteCarloData& mc_data )
{
  // Get the primary event info from the StackingAction, and put it in the
  // event data object.  This function gets all primary events between
  // fTimeZero and GetCurrentEventWindowEndTime, delegating some event
  // information collection to the stacking Action.

  mc_data.fBetaDecayQValue = fStackingAction->get_qvalue(fTimeZero,GetCurrentEventWindowEndTime())/keV;


  G4int particle_type[100];
  G4double particle_energy[100];
  G4double particle_charge[100];
  G4int particle_atomicmass[100];
  G4double particle_excitedenergy[100];
  G4int numParticle = 0;
  
  G4double event_location[3];

  if (fSaveParticleInfo) {
    numParticle = 100;
    fStackingAction->get_part_info(fTimeZero, 
            GetCurrentEventWindowEndTime(), &numParticle,
            particle_type, particle_energy, particle_charge,
            particle_atomicmass, particle_excitedenergy,
            event_location);
    
    //FixME TODO
    //if ( numParticle > MAXPART ) numParticle = MAXPART;
    
    for ( G4int i = 0; i < numParticle; i++ ) {
      EXOMCParticleInformation* part_info =
          mc_data.GetNewParticleInformation(); 
      part_info->fID = particle_type[i];
      part_info->fKineticEnergykeV = particle_energy[i]/keV;
      part_info->fCharge = particle_charge[i];
      part_info->fAtomicNumber = particle_atomicmass[i];
      part_info->fExcitedStatekeV = particle_excitedenergy[i]/keV;
    }
    
    mc_data.fPrimaryEventX = event_location[0];
    mc_data.fPrimaryEventY = event_location[1];
    mc_data.fPrimaryEventZ = event_location[2];

  }

  // Get the number of photons created between eventT0 and the
  // end of the digitized event.

  G4int nphotons = fStackingAction->get_photoncount(fTimeZero,GetCurrentEventWindowEndTime());
  mc_data.fTotalPhotons = nphotons;

  if ( fPrintParticleInfo == true ) {

    G4cout << "flavor\tenergy (keV)\tcharge\tA\texcitation energy (keV)" << G4endl;
    G4cout << "---------------------------------------------------------------" << G4endl;

    std::string flavor; 
    for ( G4int i = 0; i < numParticle; i++ ) {

      if ( particle_type[i] == 0 ) flavor = "ion";
      if ( particle_type[i] == 1 ) flavor = "e-";
      if ( particle_type[i] == 2 ) flavor = "e+";
      if ( particle_type[i] == 3 ) flavor = "gamma";
      if ( particle_type[i] == 4 ) flavor = "alpha";
      if ( particle_type[i] == 5 ) flavor = "nu";
      if ( particle_type[i] == 6 ) flavor = "muon";
      if ( particle_type[i] == 7 ) flavor = "other";

      G4cout << Form("%s\t%.1f\t\t%.1f\t%d\t\t%.1f",
         flavor.c_str(),
         particle_energy[i]/keV,
         particle_charge[i],
         particle_atomicmass[i],
         particle_excitedenergy[i]/keV ) << G4endl;
    }

    if ( mc_data.fBetaDecayQValue > 0.0 ) {
      G4cout << "---------------------------------------------------------------" << G4endl;
      G4cout << Form("beta decay Q value = %.1f keV",mc_data.fBetaDecayQValue/keV) << G4endl;
    }

  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4double EXOEventAction::get_lxe_energy_info( EXOMonteCarloData& mc_data )
{
  // Add up the total energy and ionization energy in the LXe
  // and put it into the  event data. Returns the total LXe energy.
  // Accepts events which are within the ChargeHit time window, see
  // ChargeHitInTimeWindow. 

  G4double etotallxe = 0.0;
  G4double echargelxe = 0.0;
  G4double etotallxe_photons = 0.0;
  G4bool didBrem  = false;
  G4bool didCompton = false;

  // Delete energy distribution Tree in preparation for creating a new
  // one (for Alpha Team's recombination)
  if(fRunAction->returnAlphaAnticorr()){
    if(fRunAction->returnIncrement()%2==0){
      if(fRunAction->returnIncrement()!=0){
        fRunAction->DeleteTree();
      }
    }
  }

 
  if(fLXeHC) {
    G4int numberLXehits = fLXeHC->entries();

    G4double x;
    G4double y;
    G4double z;
    G4double energy;


    for (G4int i=0; i<numberLXehits; i++) {

      //Creating an energy distribution tree to use for recombination
      if(fRunAction->returnAlphaAnticorr()){        
        if(fRunAction->returnIncrement()%2==0){
          x = (*fLXeHC)[i]->position[0] / mm;
          y = (*fLXeHC)[i]->position[1] / mm;
          z = (*fLXeHC)[i]->position[2] / mm;
          energy = (*fLXeHC)[i]->etotal / eV;
          fRunAction->FillEnergyTree(x,y,z,energy);
        }    
      }


      G4double hit_etotal          = (*fLXeHC)[i]->etotal;
      G4double hit_echarge         = (*fLXeHC)[i]->echarge;
      G4String hit_name            = (*fLXeHC)[i]->particleName;
      if ((*fLXeHC)[i]->ptype == 1) {
        didBrem = true;
      }
      else if ((*fLXeHC)[i]->ptype == 2) {
        didCompton = true;
      }
    

      if ( hit_etotal == 0.0 ) continue;
      if ( hit_name != "opticalphoton" ) {
        if (ChargeHitInTimeWindow(*(*fLXeHC)[i])) {
          etotallxe += hit_etotal;
          echargelxe += hit_echarge;
        }

      }  else {                        
        etotallxe_photons += hit_etotal;  
      }

    } // end loop over hits

  } // LXe hit collection found
 
  mc_data.fTotalIonizationEnergyInLiquidXe = echargelxe/keV;
  mc_data.fTotalEnergyInLiquidXe = etotallxe/keV;
  mc_data.fTotalIonizationEnergyInLiquidXe += etotallxe_photons/keV; // sums active and inactive    
  
  mc_data.fDidBrem    = didBrem;
  mc_data.fDidCompton = didCompton;

  return etotallxe;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::get_passive_material_energy( EXOMonteCarloData& mc_data )
{
  // Get the energy deposited in the passive shielding within the charge hit
  // window ( see ChargeHitInTimeWindow ) and put it into the  event data.

  G4double EnergySalt = 0.0;
  G4double EnergyShield = 0.0;
  G4double EnergyOuterCryo = 0.0;
  G4double EnergyInnerCryo = 0.0;
  G4double EnergyHFE = 0.0;
  G4double EnergyLXeVessel = 0.0;
  G4double EnergyCathode_photons = 0.0;   
  G4double EnergyAnodes_photons = 0.0;   
  G4double EnergyWireSupports = 0.0;   
  G4double EnergyCathodeRing = 0.0;   
  G4double EnergyReflector = 0.0;   
  G4double EnergyAPDFrames = 0.0;   
  G4double EnergyLXe_photons = 0.0; 
  G4double EnergyLXeVessel_photons = 0.0; 
  G4double Energy_Field_Rings = 0.0;

  if(fPassiveMaterialHC) {
    G4int nhits = fPassiveMaterialHC->entries();
    for (G4int i=0; i<nhits; i++) {

      G4int    hit_material        = (*fPassiveMaterialHC)[i]->materialID;
      G4double hit_etotal          = (*fPassiveMaterialHC)[i]->etotal;
      G4String hit_name            = (*fPassiveMaterialHC)[i]->particleName;

      if ( hit_etotal == 0.0 ) continue;
      if ( !ChargeHitInTimeWindow(*((*fPassiveMaterialHC)[i])) ) continue;

      if ( hit_material == 0 ) EnergySalt += hit_etotal;
      if ( hit_material == 2 ) EnergyShield += hit_etotal;
      if ( hit_material == 3 ) EnergyOuterCryo += hit_etotal;
      if ( hit_material == 4 ) EnergyInnerCryo += hit_etotal;
      if ( hit_material == 5 ) EnergyHFE += hit_etotal;
      if ( hit_material == 6 ) { 
          if (hit_name != "opticalphoton"){ EnergyLXeVessel += hit_etotal; }
          else{ EnergyLXeVessel_photons += hit_etotal; }
      }
      if ( hit_material == 11 ) EnergyCathode_photons += hit_etotal; 
      if ( hit_material == 12 ) EnergyAnodes_photons += hit_etotal; 
      if ( hit_material == 13 ) EnergyAnodes_photons += hit_etotal; 
      if ( hit_material == 14 ) EnergyWireSupports += hit_etotal; 
      if ( hit_material == 15 ) EnergyWireSupports += hit_etotal; 
      if ( hit_material == 16 ) EnergyCathodeRing += hit_etotal; 
      if ( hit_material == 10 ) EnergyReflector += hit_etotal; 
      if ( hit_material == 8 ) EnergyAPDFrames += hit_etotal; 
      if ( hit_material == 9 ) EnergyAPDFrames += hit_etotal; 
      if ( hit_material == 7 ) EnergyLXe_photons += hit_etotal;  //inactive LXe 
      if ( hit_material == 17 ) Energy_Field_Rings += hit_etotal; 
      

    } // end loop over hits

  } // passive material hit collection found

  mc_data.fTotalEnergyInSalt = EnergySalt/keV;
  mc_data.fTotalEnergyInShield = EnergyShield/keV;
  mc_data.fTotalEnergyInOuterCryostat = EnergyOuterCryo/keV;
  mc_data.fTotalEnergyInInnerCryostat = EnergyInnerCryo/keV;
  mc_data.fTotalEnergyInHFE = EnergyHFE/keV;
  mc_data.fTotalEnergyInVessel = EnergyLXeVessel/keV;
  mc_data.fTotalEnergyOptPhotonsInCathode = EnergyCathode_photons/keV;   
  mc_data.fTotalEnergyOptPhotonsInBothAnodes = EnergyAnodes_photons/keV;   
  mc_data.fTotalEnergyOptPhotonsInBothWireSupports = EnergyWireSupports/keV;   
  mc_data.fTotalEnergyOptPhotonsInCathodeRing = EnergyCathodeRing/keV;   
  mc_data.fTotalEnergyOptPhotonsInTeflonReflector = EnergyReflector/keV;   
  mc_data.fTotalEnergyOptPhotonsInBothAPDFrames = EnergyAPDFrames/keV;   
  mc_data.fTotalEnergyOptPhotonsInLiquidXe += EnergyLXe_photons/keV; // sums inactive and active
  mc_data.fTotalEnergyOptPhotonsInLiquidXeVessel = EnergyLXeVessel_photons/keV;   
  mc_data.fTotalEnergyOptPhotonsInRemovedAPDs = fStepAction->get_photon_energy_removed_apds()/keV;
  mc_data.fTotalEnergyOptPhotonsInFieldShapingRings = Energy_Field_Rings/keV;   
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4double EXOEventAction::GetVetoPanelInfo( EXOMonteCarloData& mc_data )
{
  // Get the energy deposited in the veto panels within the charge hit
  // window ( see ChargeHitInTimeWindow ) and put it into the event data.

  // Reset the number of hits and the total hit energy for each event.
  G4double totalEnergyVetoPanels = 0.0;
  mc_data.fTotalHitsVetoPanels = 0;

  if(fVetoPanelHC) {
    // Get the total number of G4 hits (across all panels) for this event.
    G4int nHits = fVetoPanelHC->entries();

	// Now create an array of EXOMCVetoPanelHitInfo objects.  Make one for each panel,
	// but we don't write them out, if the panel was not hit in this event window.
    EXOMCVetoPanelHitInfo vetoPanelHitInfo[29];

	// Loop over all G4 hits recorded in the hit collection for this G4 event.
    for (G4int i=0; i<nHits; i++) {

	  // First, make sure that this hit falls within the EXO event time window.
      if ( !HitInTimeWindow(*((*fVetoPanelHC)[i])) ) continue;

      G4double hitEnergy  = (*fVetoPanelHC)[i]->hitEnergy;
      G4int    hitPanelID = (*fVetoPanelHC)[i]->panelID;
      G4double hitTime    = (*fVetoPanelHC)[i]->time;
      hitTime -= fTimeZero;

	  // Next, make sure that this hit has non-zero energy.
      if ( hitEnergy == 0.0 ) continue;

	  // Finally, ensure that the panel ID is valid. To each of the 29 panels, we
	  // assign an ID from the copy number (see detector construction).
      if ( !(hitPanelID > 0 && hitPanelID < 30) ) {
	    G4cout << "EXOEventAction:: Veto panel error: panelID = " << hitPanelID << G4endl;
		continue;
	  }


	  // Add this hit data to the appropriate EXOMCVetoPanelHitInfo object.
	  // For now, the time stored in each EXOMCVetoPanelHitInfo object will be the
      // (adjusted) time of the first hit on each panel.  Also write the panelID on
	  // the first hit for a each panel.
	  if ( vetoPanelHitInfo[hitPanelID].fNumHits == 0 ) {
        vetoPanelHitInfo[hitPanelID].fTime     = hitTime;
        vetoPanelHitInfo[hitPanelID].fPanelID  = (*fVetoPanelHC)[i]->panelID;
	  }
      vetoPanelHitInfo[hitPanelID].fEnergy    += (*fVetoPanelHC)[i]->hitEnergy;
      vetoPanelHitInfo[hitPanelID].fNumHits++;


	  // TO-DO: Produce an energy-weighted average position for hits in this panel.
      // G4ThreeVector hitPosition   = (*fVetoPanelHC)[i]->position;
      // vetoPanelHit.fPosition = (*fVetoPanelInternalHC)[i]->position;


      // Increment the total energy and number of hits (both summed over all panels)
      totalEnergyVetoPanels += hitEnergy;
      mc_data.fTotalHitsVetoPanels++;

    } // end loop over hits
	
	// Add each EXOMCVetoPanelHitInfo object to the encapsulating EXOMonteCarloData
	// object, but only if there was at least one hit on that panel for this event. 
    for (G4int i=0; i<29; i++) {
	
	  if ( !(vetoPanelHitInfo[i].fNumHits == 0) ) {
	    mc_data.AddVetoPanelHitInfo(vetoPanelHitInfo[i]);
	  }

    }  // done adding panel info to EXOMonteCarloData

  } // veto panel hit collection found

  mc_data.fTotalEnergyInVetoPanels = totalEnergyVetoPanels/keV;

  return totalEnergyVetoPanels;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::do_tpc_event_grouping( EXOMonteCarloData& MonteCarloData )
{
  // Performs event windowing.  Loops over LXe hits, accepting if the hit is in the
  // time window (see HitInTimeWindow).  APD info is already stored in the
  // EXOEventData within get_APD_info().

  // Get the digitization manager
  G4DigiManager* DM = G4DigiManager::GetDMpointer();
  
  // Get the hit collection ID
  G4int THCID = DM->GetHitsCollectionID("LXeHitsCollection");

  // Get the hits collection
  const EXOLXeHitsCollection* HC = 
    dynamic_cast<const EXOLXeHitsCollection*>(DM->GetHitsCollection(THCID));
  if(!HC) return;

  // Loop over the hits and add up the energy in each charge cluster
  G4int nhits = HC->entries();

  for (G4int i=0; i<nhits; i++) {

    G4ThreeVector hit_position  = (*HC)[i]->position;
    G4double hit_time           = (*HC)[i]->time;
    if (!HitInTimeWindow(*((*HC)[i]))) continue;

    // Get energy from the hit
    G4double hit_etotal          = (*HC)[i]->etotal;
    G4double hit_echarge         = (*HC)[i]->echarge;
    hit_time -= fTimeZero;

    // Get Ancestor Particle Info. from hit
    G4int ancestorParticleType = (*HC)[i]->ancestorParticleType;
    G4float ancestorParticleEnergy = floor((*HC)[i]->ancestorParticleEnergy*pow(10,6))/pow(10,6); // Truncate to nearest eV

    // Add in the process to the interaction list if it is a scatter type process
    if (    (*HC)[i]->depositProcess != "msc"
        and (*HC)[i]->depositProcess != "eIoni"
        and (*HC)[i]->depositProcess != "Transportation") {
    
        //std::cout << "+++++++++++++++++++++++++++++Adding in " << (*HC)[i]->depositProcess << std::endl ;

        EXOMCInteractionInfo* int_info =   MonteCarloData.GetNewInteraction();
        int_info->fProcessName = (*HC)[i]->depositProcess;
        int_info->fX = hit_position.getX();
        int_info->fY = hit_position.getY();
        int_info->fZ = hit_position.getZ();
    }


    // Ignore hits with non-positive ionization energy
    if(hit_echarge <= 0.0) continue;

    // Get the appropriate EXOMCPixelatedChargeDeposit from MonteCarloData
    EXOCoordinates HitCoord(EXOMiscUtil::kXYCoordinates,
                            hit_position.getX(),
                            hit_position.getY(),
                            hit_position.getZ(),
                            hit_time);
    EXOMCPixelatedChargeDeposit* PixelDeposit = MonteCarloData.FindOrCreatePixelatedChargeDeposit(HitCoord);
    
    if ((*HC)[i]->depositProcess == "phot"){
        PixelDeposit->fNumPhot += 1;
    }
    else if ((*HC)[i]->depositProcess == "compt"){
        PixelDeposit->fNumCompt += 1;
    }
    else if (   (*HC)[i]->depositProcess != "eBrem" 
            and (*HC)[i]->depositProcess != "msc" 
            and (*HC)[i]->depositProcess != "eIoni" 
            and (*HC)[i]->depositProcess != "Transportation"
            and (*HC)[i]->depositProcess != "annihil")
    {
        std::cout << "Untracked type of deposit process " << (*HC)[i]->depositProcess << std::endl;
    }


    // Add energy to pixel
    PixelDeposit->fTotalEnergy += hit_etotal;
    PixelDeposit->fTotalIonizationEnergy += hit_echarge;
    
    // Add ancestor particle types and energies to pixel
    // If the energy or type does not exist in the current PCD, or if the PCD's ancestor
    // information is empty, then add the current info. to it.
    
    bool pushFlag = false;
    bool ancestorMatch = false;
    if (PixelDeposit->fAncestorParticleType.empty()) { pushFlag = true; }
    else {
        for (unsigned int j=0; j < PixelDeposit->fAncestorParticleType.size(); j++) {

            if (PixelDeposit->fAncestorParticleType[j] == ancestorParticleType ){
                if( fabs(PixelDeposit->fAncestorParticleEnergy[j] - ancestorParticleEnergy) < .000001) {//Compare energies to meV
                ancestorMatch = true; // Flag that determines whether or not an ancestor match is already in the vector
                continue;
                }
            }
        }
    if (ancestorMatch == false) { pushFlag = true; }
    }
// Put data into PCD
    if (pushFlag == true) {
        PixelDeposit->fAncestorParticleType.push_back(ancestorParticleType);
        PixelDeposit->fAncestorParticleEnergy.push_back(ancestorParticleEnergy);
    }

    if (PixelDeposit->fWeight != 1.0 && PixelDeposit->fWeight != (*HC)[i]->weight) {
      G4cerr << "Pixels do not having matching weight!" << G4endl;
    }
    PixelDeposit->fWeight = (*HC)[i]->weight;
    

    // Assorted output -- it was here before, and surely doesn't hurt to keep.
    if ( fabs(HitCoord.GetU()) > REFLECTORINNERRAD ) {
      G4cout << "u coordinate of this hit = " << HitCoord.GetU()/cm << " cm " << G4endl;
      continue;
    }
    if ( fabs(HitCoord.GetV()) > REFLECTORINNERRAD ) {
      G4cout << "v coordinate of this hit = " << HitCoord.GetV()/cm << " cm " << G4endl;
      continue;
    }

  } // end the clustering of hits
  


}

void EXOEventAction::get_APD_info( EXOMonteCarloData& mc_data )
{
  // Get the APD info for this event, storing in the Monte Carlo Data.  This
  // function does either a *fast* simulation of the APD count hits or grabs
  // informaiton from the simulation about hits (not energy deposited) in the
  // APDs by optical photons.  See also get_internal_APD_energy().
  
  // The following two are used in this function only.
  static bool first_APD_call = true;

  // Reset the mc_data for counts in the two arrays
  mc_data.fTotalHitsArrayOne = 0;
  mc_data.fTotalHitsArrayTwo = 0;

  // Use the fast APD simulation if asked
  if ( fRunAction->returnFastScintAlpha() ) {

    G4int nphotons = fStackingAction->get_photoncount(fTimeZero,
                                                     GetCurrentEventWindowEndTime());

    if ( first_APD_call == true ) { 
      G4cout << "********************************************************" << G4endl;
      G4cout << "using fast APD simulation" << G4endl;
      if (nphotons != 0) G4cout << "warning: together with photons tracking!" << G4endl;
      G4cout << "********************************************************" << G4endl;
      if ( fDoAntiCorrelationATeam == true ) {
        G4cout << "********************************************************" << G4endl;
        G4cout << "using recombination model" << G4endl;
        G4cout << "********************************************************" << G4endl;    
      }
      first_APD_call = false;
    }

    // Get the number of hits on each APD plane

    G4int num_APD1_hits = 0;
    G4int num_APD2_hits = 0;

    if ( fDoAntiCorrelationATeam == true ) {    
      do_recombination( mc_data.fTotalPhotons, num_APD1_hits, num_APD2_hits,
                        mc_data.fTotalIonizationEnergyInLiquidXe );
      // Distribute those hits "evenly" over all the APD gangs on the two planes
      
      // Pull this guy here so that it's not created every time.
      EXOMCAPDHitInfo apd_hit;
      apd_hit.fTime = 100.0*nanosecond;
      apd_hit.fNumHits = 1;
      for ( G4int i = 0; i < num_APD1_hits; i++ ) {
        apd_hit.fGangNo = i%NUMBER_APD_CHANNELS_PER_PLANE;
        mc_data.AddAPDHitInfo(apd_hit);
      }
      for ( G4int i = 0; i < num_APD2_hits; i++ ) {
        apd_hit.fGangNo = NUMBER_APD_CHANNELS_PER_PLANE + i%NUMBER_APD_CHANNELS_PER_PLANE;
        mc_data.AddAPDHitInfo(apd_hit);
      }

    } else {
      fast_APD_simulation( mc_data, num_APD1_hits, num_APD2_hits );
    }

    mc_data.fTotalHitsArrayOne = num_APD1_hits;
    mc_data.fTotalHitsArrayTwo = num_APD2_hits;

  } // Done with the fast APD simulation

  // Get the APD info and put it into the  event data

  // Add up the number of APD hits

  // FixME: TODO Does the following code need to still exist?  M. Marino
  // It seems that fAPDHC is only used here?

  // Fill whatever else happens to be in fAPDHC
  if(fAPDHC) {

    G4int numberAPDhits = fAPDHC->entries();
    EXOMCAPDHitInfo apd_hit;
    apd_hit.fNumHits = 1;
    for (G4int i=0; i<numberAPDhits; i++) {

      if ( !HitInTimeWindow(*((*fAPDHC)[i])) ) continue;
      G4double hit_time            = (*fAPDHC)[i]->time;
      hit_time -= fTimeZero;

      apd_hit.fGangNo = (*fAPDHC)[i]->gangNo;
      apd_hit.fTime = hit_time;

      if ( apd_hit.fGangNo >= 0 && apd_hit.fGangNo < NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE ) {
        mc_data.AddAPDHitInfo(apd_hit);
        // Increment the total number in the array
        if (apd_hit.fGangNo < NUMBER_APD_CHANNELS_PER_PLANE) mc_data.fTotalHitsArrayOne++;
        else mc_data.fTotalHitsArrayTwo++;
      } else {
        G4cout << "EXOEventAction:: APD error: gangNo = " << 
          apd_hit.fGangNo << G4endl;
      }

    } // End loop over APD hits

  } // found APD hit collection

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::get_internal_APD_energy( EXOMonteCarloData& mc_data )
{
  // Get the energy deposited internally in the APDs by optical photons and puts
  // it into the event data.  See also get_APD_info().

  if(fAPDInternalHC) {
    G4int nhits = fAPDInternalHC->entries();
    for (G4int i=0; i<nhits; i++) {

      if ( !HitInTimeWindow(*((*fAPDInternalHC)[i])) ) continue;
      G4double      hit_time     = (*fAPDInternalHC)[i]->time;
      hit_time -= fTimeZero;
      EXOMCAPDHitInfo apd_hit;
      apd_hit.fTime   = hit_time;
      apd_hit.fEnergy = (*fAPDInternalHC)[i]->etotal;
      apd_hit.fGangNo = (*fAPDInternalHC)[i]->gangNo;

      G4ThreeVector hit_position = (*fAPDInternalHC)[i]->position;
      G4double gain = get_gain(hit_position[2]);
      apd_hit.fCharge = apd_hit.fEnergy*gain; 
 
      mc_data.AddAPDHitInfo(apd_hit);

    } // end loop over hits

  } // internal APD hit collection found
  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::fast_APD_simulation( EXOMonteCarloData &mc, 
                      G4int &num_APD1_hits, 
                      G4int &num_APD2_hits )
{
  // Fast APD simulation, returns the total photons and the number of hits on
  // the two APD planes.
  // Coefficients are normalized so that the acceptance is between 0 and 1
  // parameters = {a1, a2, b1, b2, c1, c2, d1, d2, d3, d4, e, f1, f2, f3, f4, g1, g2, g3, g4}

  G4double tpc_1[19] = {-3.04490419e-06,  -1.14552389e-05,   4.06603102e-04,   1.10250692e-03,
			-1.00402797e-06,   2.58881357e-04,   2.49008046e-03,   9.80160799e-03,
  			 1.72719919e-03,   1.32738693e-03,   5.42197108e-01,   1.23273173e-08,
			-4.55571441e-07,   7.53887444e-08,   1.43239443e-08,  -2.71258542e-10,
			-1.32014091e-08,   1.03377322e-09,   1.10489232e-10};

  G4double tpc_2[19] = {-5.08213328e-06,  -2.17202349e-06,   4.09534631e-04,   4.73603365e-04,
			-2.88758584e-06,  -2.25485917e-04,  -8.65997007e-04,  -9.74660326e-03,
			-3.45256397e-03,  -3.30521012e-03,   5.15677779e-01,   7.05382849e-09,
			 3.73849026e-07,  -1.00465687e-07,  -2.99732744e-08,   7.08318613e-11,
			 1.24872754e-08,  -1.25193358e-09,  -3.48909121e-10};

  // scintillation yield rescaled to fit the data
  G4double scint_yield = 1521.74; // in arbitrary units

  num_APD1_hits = 0;
  num_APD2_hits = 0;

  G4double total_energy = 0.0;
    
  size_t numberLXehits = mc.GetNumPixelatedChargeDeposits(); 
  for (size_t i=0; i<numberLXehits; i++) { 

    const EXOMCPixelatedChargeDeposit* pcd = mc.GetPixelatedChargeDeposit(i);
    G4double hit_etotal = pcd->fTotalEnergy;
    if ( hit_etotal == 0.0 ) continue;

    EXOCoordinates coords  = pcd->GetPixelCenter();
    G4double hit_time      = coords.GetT();
    G4double r = sqrt(coords.GetX()*coords.GetX() + coords.GetY()*coords.GetY())/mm;
    G4double z = coords.GetZ()/mm;
    G4double acceptance1 = 0.0, acceptance2 = 0.0;
    
    // new fitting function with four sets of parameters for different values of z
    // fit_1 = a1 * r * r + b1 * r + c1 * z * z + d1 * z + e  + f1 * r * r * z +  g1 * z * z * r * r (>0)
    // fit_2 = a2 * r * r + b2 * r + c2 * z * z + d2 * z + e  + f2 * r * r * z +  g2 * z * z * r * r [-25, 0)
    // fit_3 = a1 * r * r + b1 * r + c1 * z * z + d3 * z + e  + f3 * r * r * z +  g3 * z * z * r * r [-85, -25)
    // fit_4 = a1 * r * r + b1 * r + c1 * z * z + d4 * z + e  + f4 * r * r * z +  g4 * z * z * r * r (< -85)
    // {a1, a2, b1, b2, c1, c2, d1, d2, d3, d4, e, f1, f2, f3, f4, g1, g2, g3, g4}


    if (z > 0.) {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[6] * z + tpc_1[10] + 
	tpc_1[11] * r * r * z + tpc_1[15] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[6] * z + tpc_2[10] + 
	tpc_2[11] * r * r * z + tpc_2[15] * r * r * z * z;

    } else if (z < 0. && z >= -25.){
      acceptance1 = tpc_1[1] * r * r + tpc_1[3] * r + tpc_1[5] * z * z + tpc_1[7] * z + tpc_1[10] + 
	tpc_1[12] * r * r * z + tpc_1[16] * r * r * z * z;

      acceptance2 = tpc_2[1] * r * r + tpc_2[3] * r + tpc_2[5] * z * z + tpc_2[7] * z + tpc_2[10] + 
	tpc_2[12] * r * r * z + tpc_2[16] * r * r * z * z;

    } else if (z < -25. && z >= -85.) {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[8] * z + tpc_1[10] + 
	tpc_1[13] * r * r * z + tpc_1[17] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[8] * z + tpc_2[10] + 
	tpc_2[13] * r * r * z + tpc_2[17] * r * r * z * z;

    } else {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[9] * z + tpc_1[10] + 
	tpc_1[14] * r * r * z + tpc_1[18] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[9] * z + tpc_2[10] + 
	tpc_2[14] * r * r * z + tpc_2[18] * r * r * z * z;
    }

    total_energy += hit_etotal;
    G4int tempNum_APD1_hits = (G4int)(scint_yield * hit_etotal * acceptance1/MeV);
    G4int tempNum_APD2_hits = (G4int)(scint_yield * hit_etotal * acceptance2/MeV);
    num_APD1_hits += tempNum_APD1_hits; 
    num_APD2_hits += tempNum_APD2_hits; 
     
    // Now add these on to the different planes 
    EXOMCAPDHitInfo apdHit;
    apdHit.fTime = hit_time;
    if ( tempNum_APD1_hits > 0) {
      double APD1_smear = G4RandGauss::shoot(tempNum_APD1_hits,sqrt((G4double)(tempNum_APD1_hits)));
      apdHit.fCharge = APD1_smear/NUMBER_APD_CHANNELS_PER_PLANE; 
      for ( G4int iApd = 0; iApd < NUMBER_APD_CHANNELS_PER_PLANE; iApd++ ) {
        apdHit.fGangNo = iApd;
        mc.AddAPDHitInfo(apdHit);
      }
    }
    if ( tempNum_APD2_hits > 0) {
      double APD2_smear = G4RandGauss::shoot(tempNum_APD2_hits,sqrt((G4double)(tempNum_APD2_hits)));
      apdHit.fCharge = APD2_smear/NUMBER_APD_CHANNELS_PER_PLANE; 
      for ( G4int iApd = 0; iApd < NUMBER_APD_CHANNELS_PER_PLANE; iApd++ ) {
        apdHit.fGangNo = NUMBER_APD_CHANNELS_PER_PLANE + iApd;
        mc.AddAPDHitInfo(apdHit);
      }
    }

  }
    mc.fTotalPhotons = (int)(total_energy * scint_yield/MeV);

  // Add in a gaussian smearing term

  G4double APD1_smear = 0.0;
  G4double APD2_smear = 0.0;

  if ( num_APD1_hits > 0 ) {
    APD1_smear = G4RandGauss::shoot(0.0,sqrt((G4double)(num_APD1_hits)));
  }
  if ( num_APD2_hits > 0 ) {
    APD2_smear = G4RandGauss::shoot(0.0,sqrt((G4double)(num_APD2_hits)));
  }

  num_APD1_hits += (G4int)(APD1_smear);
  num_APD2_hits += (G4int)(APD2_smear);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::get_acceptance( G4int hit_number, G4double &pacceptance1, G4double &pacceptance2 )
{
  // Get the acceptance on the two APD planes for a hit hit_number in fLXeHC.
  // This (internal) function assumes that fLXeHC is not NULL and so other
  // calling functions in this class must take care of this. 

  G4double p0_pf[7] = {4.86378e-02, 4.72671e-02, 4.66348e-02, 4.58761e-02,   //"pf" for far plane parameters
                        4.02597e-02, 1.36732e-02, 0.0};
  G4double p1_pf[7] = {1.99446e-04, 1.75336e-04, 1.71054e-04, 1.81581e-04, 
                        1.14810e-04, -8.98073e-04, 0.0};
  G4double p2_pf[7] = {3.67577e-07, 2.52844e-07, 2.31886e-07, 3.10647e-07, 
                        9.31609e-08, -1.50258e-05, 0.0};
    
  G4double p3_pf = -9.82903e-08; //for events inside cathode ring only
  G4double p4_pf = -2.30009e-10;
    

  G4double p0_pn[7] = {5.16537e-02, 5.07639e-02, 5.06046e-02, 5.04272e-02,    //"pn" for near plane
                        4.70461e-02, 3.61464e-02, 0.0};
  G4double p1_pn[7] = {2.64649e-04, 2.27776e-04, 1.96460e-04, 2.03429e-04, 
                        3.28585e-04, 1.08898e-03, 0.0};
  G4double p2_pn[7] = {9.85131e-07, 1.12548e-06, 1.16181e-06, 8.88561e-07, 
                        -7.98202e-08, -1.75686e-05, 0.0};

  G4double p3_pn = 1.50961e-07; //for events inside cathode ring only
  G4double p4_pn = -4.35878e-10;
    
  G4double p0_avg[7] = {0.050146,   0.049016,   0.048620,   0.048152,   0.043653,   0.024910,   0.0};
  G4double p1_avg[7] = {2.3205e-04,   2.0156e-04,   1.8376e-04,   1.9250e-04,   2.2170e-04,   9.5454e-05,   0.0};
  G4double p2_avg[7] = {6.7635e-07,   6.8916e-07,   6.9685e-07,   5.9960e-07,   6.6704e-09,  -1.6297e-05,   0.0};
  
  G4double p3_avg = 2.6335e-08; //for events inside cathode ring only
  G4double p4_avg = -3.3294e-10;
  
  
  G4ThreeVector hit_position  = (*fLXeHC)[hit_number]->position;
  G4double hit_radius     =    sqrt(hit_position[0]*hit_position[0] +
                    hit_position[1]*hit_position[1]);
  G4int iradius = 0;
  G4double hit_z_mm = hit_position[2]/mm;
  
  if ( hit_radius < 7.732*cm ) {
    iradius = 0;
  }
  else if ( hit_radius < 10.7282*cm ) {
    iradius = 1;
  }
  else if ( hit_radius < 13.054*cm ) {
    iradius = 2;
  }
  else if ( hit_radius < 15.024*cm ) {
    iradius = 3;
  }
  else if ( hit_radius < 16.764*cm ) {
    iradius = 4;
  }
  else if ( hit_radius < 18.34*cm ) {
    iradius = 5;
  }
  else {
    iradius = 6;
  }
  
  G4double acceptance1 = 0.0, acceptance2 = 0.0;
  
  if(fabs(hit_z_mm) <= 0.1 )
  {//center
    if(iradius == 5)
    {//events inside cathode ring
      acceptance1 = p0_avg[iradius]+p1_avg[iradius]*hit_z_mm+p2_avg[iradius]*pow(hit_z_mm,2)+p3_avg*pow(hit_z_mm,3)+p4_avg*pow(hit_z_mm,4);
      acceptance2 = acceptance1;
    }
    else{
      acceptance1 = p0_avg[iradius]+p1_avg[iradius]*hit_z_mm+p2_avg[iradius]*pow(hit_z_mm,2);
      acceptance2 = acceptance1;
    }
  }
  else if(hit_z_mm > 0.1)
  {//plane 1
    if(iradius == 5)
    {//events inside cathode ring
      acceptance1 = p0_pn[iradius]+p1_pn[iradius]*hit_z_mm+p2_pn[iradius]*pow(hit_z_mm,2)+p3_pn*pow(hit_z_mm,3)+p4_pn*pow(hit_z_mm,4);
      acceptance2 = p0_pf[iradius]-p1_pf[iradius]*hit_z_mm+p2_pf[iradius]*pow(hit_z_mm,2)-p3_pf*pow(hit_z_mm,3)+p4_pf*pow(hit_z_mm,4);
      
    }
    else{
      acceptance1 = p0_pn[iradius]+p1_pn[iradius]*hit_z_mm+p2_pn[iradius]*pow(hit_z_mm,2);
      acceptance2 = p0_pf[iradius]-p1_pf[iradius]*hit_z_mm+p2_pf[iradius]*pow(hit_z_mm,2);
    }
  }
  else if(hit_z_mm < -0.1 )
  {//plane 2
    if(iradius == 5)
    {//events inside cathode ring
      acceptance1 = p0_pf[iradius]+p1_pf[iradius]*hit_z_mm+p2_pf[iradius]*pow(hit_z_mm,2)+p3_pf*pow(hit_z_mm,3)+p4_pf*pow(hit_z_mm,4);
      acceptance2 = p0_pn[iradius]-p1_pn[iradius]*hit_z_mm+p2_pn[iradius]*pow(hit_z_mm,2)-p3_pn*pow(hit_z_mm,3)+p4_pn*pow(hit_z_mm,4);
      
    }
    else{
      acceptance1 = p0_pf[iradius]+p1_pf[iradius]*hit_z_mm+p2_pf[iradius]*pow(hit_z_mm,2);
      acceptance2 = p0_pn[iradius]-p1_pn[iradius]*hit_z_mm+p2_pn[iradius]*pow(hit_z_mm,2);
  
    }
  }
  else{
    G4cout << "\nError: scinitllation parameterization not defined for event location specified, setting to 0.0\n";
    acceptance1 = 0.0;
    acceptance2 = 0.0;
  }

  pacceptance1 = acceptance1;
  pacceptance2 = acceptance2;
      
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void EXOEventAction::do_recombination( G4int& total_photons, G4int &num_APD1_hits, 
                                       G4int &num_APD2_hits, G4double& total_ionization )
{
  // Does the recombination, returning the total photons, number of APD hits in
  // both planes and the total ionization.

  num_APD1_hits = 0;
  num_APD2_hits = 0;

  // fast recombination

  G4double grid_size = 20*um;

  if (fLXeHC) {

    G4int numberLXehits = fLXeHC->entries();

    // Undo alpha quenching. The recombination model
    // should take care of this.
    
    for (G4int i=0; i<numberLXehits; i++) { 
      G4double hit_etotal  = (*fLXeHC)[i]->etotal; 
      (*fLXeHC)[i]->echarge = hit_etotal;
    }

    // put entries into array

    std::vector<G4int> outside_grid(numberLXehits);
    std::vector<G4int> hit_list(numberLXehits);
    G4int sub_event_hit_number = 0;

    for (G4int i=0; i<numberLXehits; i++) {  

      if (!ChargeHitInTimeWindow(*(*fLXeHC)[i])) continue;
      hit_list[sub_event_hit_number] = i;
      sub_event_hit_number++;
    }
    G4int remaining_hits = sub_event_hit_number;

    while (remaining_hits != 0) {

      // get first entry and make grid with this hit at the center

      G4int count = 0;
      std::vector<int> grid[100][100];

      G4ThreeVector first_hit_position = (*fLXeHC)[hit_list[0]]->position;
      G4double x_center = first_hit_position[0];
      G4double y_center = first_hit_position[1];
      grid[50][50].push_back(hit_list[0]);

      // Put all other hits in grid. Put aside hits outside grid for now.
      
      for (G4int i=1; i<remaining_hits; i++) {
    
        G4ThreeVector hit_position = (*fLXeHC)[hit_list[i]]->position;
        G4int grid_position_x = int((hit_position[0] - x_center)/grid_size);
        G4int grid_position_y = int((hit_position[1] - y_center)/grid_size);
        if (grid_position_x < -50 ||
            grid_position_x >= 50 ||
            grid_position_y < -50 ||
            grid_position_y >= 50) {
          outside_grid[count] = hit_list[i];
          count++;
        }
        else {
          grid[grid_position_x+50][grid_position_y+50].push_back(hit_list[i]);
        }
      }

      // do recombination on grid

      for (G4int i = 0; i < 100; i++) {
        for (G4int j = 0; j < 100; j++) {
          if (grid[i][j].size() != 0) {
            total_photons += recombine_on_grid(num_APD1_hits, num_APD2_hits, grid[i][j]);
          }
        }
      }
     
      // move outside_grid to hit_list

      for (G4int i = 0; i < count; i++) {
        hit_list[i] = outside_grid[i];
      }

      remaining_hits = count;
      

    }
    // Add in a gaussian smearing term
    
    G4double APD1_smear = 0.0;
    G4double APD2_smear = 0.0;
    if ( num_APD1_hits > 0 ) {
      APD1_smear = G4RandGauss::shoot(0.0,sqrt((G4double)(num_APD1_hits)));
    }
    if ( num_APD2_hits > 0 ) {
      APD2_smear = G4RandGauss::shoot(0.0,sqrt((G4double)(num_APD2_hits)));
    }
    
    num_APD1_hits += (G4int)(APD1_smear);
    num_APD2_hits += (G4int)(APD2_smear);

    // Reset total ionization energy in the LXe since this has changed
    // with recombination

    G4double echargelxe = 0.0;

    for (G4int i=0; i<numberLXehits; i++) { 
      if (!ChargeHitInTimeWindow(*(*fLXeHC)[i])) continue;
      echargelxe += (*fLXeHC)[i]->echarge; 
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4int EXOEventAction::recombine_on_grid( G4int &num_APD1_hits, 
                                         G4int &num_APD2_hits, 
                                         const std::vector<int>& hit_list )
{
  // Returns total number of photons.  Do recombination on this cell

  G4int number_hits = hit_list.size();
  G4double total_photons = 0.0;

  // get APD acceptance for this location

  G4double acceptance1, acceptance2;
  get_acceptance(hit_list[0], acceptance1, acceptance2);

  // initial scintillation from excitons

  for (G4int i=0; i<number_hits; i++) {

    G4double hit_etotal         = (*fLXeHC)[hit_list[i]]->etotal;
    G4String hit_name           = (*fLXeHC)[hit_list[i]]->particleName;
    G4double hit_charge         = (*fLXeHC)[hit_list[i]]->echarge;
    G4double nex;
    G4double w_value = W_VALUE_IONIZATION;

    if ( hit_name == "opticalphoton") continue;
    if ( !HitInTimeWindow(*((*fLXeHC)[i])) ) continue; 

    if ( hit_name == "e-" || 
         hit_name == "e+" || 
         hit_name == "mu+" ||
         hit_name == "mu-" ||
         hit_name == "gamma" ) {
      nex = N_EX_RATIO_EXCIMERS_TO_IONS;
    }
    
    else if ( hit_name == "alpha" ) {
      nex = N_EX_RATIO_EXCIMERS_TO_IONS_FOR_IONS;
      (*fLXeHC)[hit_list[i]]->echarge = hit_charge;    
      
    }
    
    else {
      //      G4cout << "Unexpected particle - " << hit_name <<
      //    ". Setting xenon recombination to the same as for electrons." << G4endl;
      nex = N_EX_RATIO_EXCIMERS_TO_IONS;
    }

    total_photons += hit_charge/w_value*nex;
  }
    

  for (G4int i=0; i<number_hits; i++) { 
    
    G4double hit_etotal         = (*fLXeHC)[hit_list[i]]->etotal;
    G4ThreeVector hit_position  = (*fLXeHC)[hit_list[i]]->position;
    G4double hit_radius         = sqrt(hit_position[0]*hit_position[0] +
                                       hit_position[1]*hit_position[1]);
    G4String hit_name            = (*fLXeHC)[hit_list[i]]->particleName;
    G4int iradius = 0;
    G4double hit_z_mm = hit_position[2]/mm;
    
    if ( hit_etotal == 0.0 ) continue;
    if ( hit_name == "opticalphoton") continue;
    if ( !HitInTimeWindow(*((*fLXeHC)[i])) ) continue; 
    
    // Calculate recombination for anticorrelation. Scan through hit list
    // for other nearby energy deposits
    
    G4double hit_charge = (*fLXeHC)[hit_list[i]]->echarge;
    G4ThreeVector position_other;
    G4double charge_other;
    G4double total_charge;
    G4double k2;
    G4double recombination_ratio;
    G4double hit_photons;
    G4double k1 = ANTICORRELATION_PARAMETER;
    G4double w_value = W_VALUE_IONIZATION;
    G4double delta_x;
    G4double delta_y;
    G4double delta_z;
    G4double overlap;

    // Calculate field dependent anticorrelation radius

    G4double field = fRunAction->returnAlphaEField();

    G4double r = ANTICORRELATION_RADIUS_CONSTANT*
      exp(ANTICORRELATION_RADIUS_EXP_TERM*log(field*1000));
    
    // self recombination within LXe hit
    k2 = hit_charge/w_value*k1;
    recombination_ratio = 1 - 1/k2*log(1+k2);
    hit_photons = recombination_ratio*hit_charge/w_value;
    hit_charge *= 1-recombination_ratio;
    
    // Recombination with other LXe hits. If another hit is within 2r in x and y,
    // there is some recombination between the two charge clusters
    
    for (int j = i+1; j < number_hits; j++) {    
      position_other = (*fLXeHC)[hit_list[j]]->position;
      delta_x = fabs(position_other[0] - hit_position[0]);
      if (delta_x < 2*r) {
        delta_y = fabs(position_other[1] - hit_position[1]);
        if (delta_y < 2*r) {
          if (fabs(position_other[2] - hit_position[2]) < 1*mm) {
            // calculate recombination
            charge_other = (*fLXeHC)[hit_list[j]]->echarge;
            total_charge = charge_other + hit_charge;
            overlap = (2*r-delta_x)*(2*r-delta_y)/4/r/r;
            k2 = total_charge*overlap/w_value*k1;
            recombination_ratio = 1 - 1/k2*log(1+k2);
            hit_photons += recombination_ratio*total_charge*overlap/w_value;
            // subtract charge from the two hits to compensate for photons created
            hit_charge *= (1-recombination_ratio*overlap);
            (*fLXeHC)[hit_list[j]]->echarge = 
              charge_other*(1-recombination_ratio*overlap);
          }
        }
      }
    }
    (*fLXeHC)[hit_list[i]]->echarge = hit_charge;
    
    total_photons += hit_photons;

  }

  //    G4cout << "total_photons " << total_photons << G4endl;
  
  num_APD1_hits += (G4int)(total_photons*acceptance1);
  num_APD2_hits += (G4int)(total_photons*acceptance2);
  
  // Re-set the total number of photons generated
  
  return (int)(total_photons);

      
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4double EXOEventAction::get_gain( G4double zpos )
{
  // Get the gain according the the zpos.

  G4double gain = 0;
  G4double depth = (APDHEIGHT + zpos) / um;
  G4double gain_profile[5] = {APD_GAIN/2,APD_GAIN/2,1.,1.,0.};
  G4double depth_profile[5] = {0,30.,100.,170.,171.};
  for (int i=0; i<4; i++) {
    if (depth > depth_profile[i] && depth <= depth_profile[i+1]) {
      gain = gain_profile[i]*(depth_profile[i+1]-depth)/(depth_profile[i+1]-depth_profile[i]) + gain_profile[i+1]*(depth-depth_profile[i])/(depth_profile[i+1]-depth_profile[i]);
    }
  }

  // factor to convert from energy to VUV photon equivalent
  G4double factor = 1.0/(7*eV * APD_GAIN/2);
  return gain*factor;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
EXOEventData* EXOEventAction::GetNextEvent() 
{ 
  // Returns an available event that was filled since the last
  // time.  This will automatically return NULL when no more events
  // are available in the buffer held internally by fCurrentEvents.
  return static_cast<EXOEventData*>(fCurrentIter.Next()); 
}

