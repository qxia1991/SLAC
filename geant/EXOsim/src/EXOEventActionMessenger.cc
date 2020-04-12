//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "EXOSim/EXOEventActionMessenger.hh"
#include "EXOSim/EXOEventAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXOEventActionMessenger::EXOEventActionMessenger(EXOEventAction* EvAct) : 
  eventAction(EvAct),
  AnticorrelationACmd("/event/anticorrelationATeam",this),
  LXeEventsOnly("/event/LXeEventsOnly",this),
  VetoPanelEventsOnly("/event/VetoPanelEventsOnly",this),
  LXeOrVetoPanelEventsOnly("/event/LXeOrVetoPanelEventsOnly",this),
  LXeAndVetoPanelEventsOnly("/event/LXeAndVetoPanelEventsOnly",this),
  PrintParticleInfo("/event/printParticleInfo",this),
  SaveParticleInfo("/event/saveParticleInfo",this),
  TotalEventWindowTimeCmd("/event/totalEventWindowTime",this),
  UnixTimeOfEventCmd("/event/setUnixTime", this)
{ 

  AnticorrelationACmd.SetGuidance("Turn anticorrelation on or off");
  AnticorrelationACmd.SetParameterName("anticorrelationATeam",false);
  AnticorrelationACmd.AvailableForStates(G4State_Idle);      

  LXeEventsOnly.SetGuidance("Write only LXe hits");
  LXeEventsOnly.SetParameterName("writeLXe",false);
  LXeEventsOnly.AvailableForStates(G4State_Idle);

  VetoPanelEventsOnly.SetGuidance("Write only veto panel hits");
  VetoPanelEventsOnly.SetParameterName("writeVetoPanel",false);
  VetoPanelEventsOnly.AvailableForStates(G4State_Idle);

  LXeOrVetoPanelEventsOnly.SetGuidance("Write only (LXe || veto panel) hits");
  LXeOrVetoPanelEventsOnly.SetParameterName("writeLXeOrVetoPanel",false);
  LXeOrVetoPanelEventsOnly.AvailableForStates(G4State_Idle);

  LXeAndVetoPanelEventsOnly.SetGuidance("Write only (LXe && veto panel) hits");
  LXeAndVetoPanelEventsOnly.SetParameterName("writeLXeAndVetoPanel",false);
  LXeAndVetoPanelEventsOnly.AvailableForStates(G4State_Idle);

  PrintParticleInfo.SetGuidance("Print out generated particle info");
  PrintParticleInfo.SetParameterName("printPart",false);
  PrintParticleInfo.AvailableForStates(G4State_Idle); 

  SaveParticleInfo.SetGuidance("Saves generated particle info");
  SaveParticleInfo.SetParameterName("savePart",true);
  SaveParticleInfo.AvailableForStates(G4State_Idle); 


  TotalEventWindowTimeCmd.SetGuidance("Set the total event window time to a static value");
  TotalEventWindowTimeCmd.SetParameterName("totaleventwindowtime",false,false);
  TotalEventWindowTimeCmd.SetRange("totaleventwindowtime>=0.0");
  TotalEventWindowTimeCmd.SetDefaultUnit("microsecond");
  TotalEventWindowTimeCmd.SetUnitCategory("Time");
  TotalEventWindowTimeCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  UnixTimeOfEventCmd.SetGuidance("Set a specific time for simulated events (UTC unix time in seconds); this will guide the database");
  UnixTimeOfEventCmd.SetParameterName("unixtime", false);
  UnixTimeOfEventCmd.SetRange("unixtime>0");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXOEventActionMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{ 
    

  if(command == &LXeEventsOnly)
    {eventAction->set_keeplxeonly(LXeEventsOnly.GetNewBoolValue(newValue));}

  else if(command == &VetoPanelEventsOnly)
    {eventAction->SetKeepVetoPanelOnly(VetoPanelEventsOnly.GetNewBoolValue(newValue));}

  else if(command == &LXeOrVetoPanelEventsOnly)
    {eventAction->SetKeepLXeOrVetoPanel(LXeOrVetoPanelEventsOnly.GetNewBoolValue(newValue));}

  else if(command == &LXeAndVetoPanelEventsOnly)
    {eventAction->SetKeepLXeAndVetoPanel(LXeAndVetoPanelEventsOnly.GetNewBoolValue(newValue));}

  else if(command == &PrintParticleInfo)
    {eventAction->set_print_particle_info(PrintParticleInfo.GetNewBoolValue(newValue));}

  else if(command == &SaveParticleInfo)
    {eventAction->set_save_particle_info(SaveParticleInfo.GetNewBoolValue(newValue));}

  else if(command == &AnticorrelationACmd)
    {eventAction->set_anticorrelationATeam(AnticorrelationACmd.GetNewBoolValue(newValue));}

  else if(command == &TotalEventWindowTimeCmd)
    {eventAction->force_static_event_window_time(TotalEventWindowTimeCmd.
					      GetNewDoubleValue(newValue));}

  else if(command == &UnixTimeOfEventCmd)
    {eventAction->SetUnixTimeOfEvents(UnixTimeOfEventCmd.GetNewIntValue(newValue));}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
