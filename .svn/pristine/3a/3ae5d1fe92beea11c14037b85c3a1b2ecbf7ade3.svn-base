#include "EXOSim/EXORunActionMessenger.hh"
#include "EXOSim/EXORunAction.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXORunActionMessenger::EXORunActionMessenger(EXORunAction* run) :
  EXORun(run),
  KillLowEnergyCmd("/run/killLowEnergyParticles",this),
  AnticorrAlphaCmd("/run/anticorrelationAlphaTeam",this),
  FastScintAlphaCmd("/run/fastscintAlphaTeam",this),
  AlphaFieldCmd("/run/efieldAlphaTeam",this),
  NexAlphaCmd("/run/nexfracAlphaTeam",this)
{ 

  KillLowEnergyCmd.SetGuidance("kills low energy particles far from the LXe");
  KillLowEnergyCmd.SetParameterName("killlowenergy",false);
  KillLowEnergyCmd.AvailableForStates(G4State_Idle);   

  AnticorrAlphaCmd.SetGuidance("Turns on Alpha Team's Anticorrelation");
  AnticorrAlphaCmd.SetParameterName("alphaanticorrelation",false);
  AnticorrAlphaCmd.AvailableForStates(G4State_Idle);      

  FastScintAlphaCmd.SetGuidance("Turns on Fast Scintillation");
  FastScintAlphaCmd.SetParameterName("alphafastscint",false);
  FastScintAlphaCmd.AvailableForStates(G4State_Idle); 

  AlphaFieldCmd.SetGuidance("Set the Electric Field");
  AlphaFieldCmd.SetParameterName("efieldalpha",true);
  AlphaFieldCmd.SetDefaultValue(2.0);
  AlphaFieldCmd.SetRange("efieldalpha>0.0");
  AlphaFieldCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  NexAlphaCmd.SetGuidance("Set Nex/Ne");
  NexAlphaCmd.SetParameterName("nexalpha",true);
  NexAlphaCmd.SetDefaultValue(0.13);
  NexAlphaCmd.SetRange("nexalpha >= 0.0");
  NexAlphaCmd.AvailableForStates(G4State_PreInit,G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXORunActionMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{ 
  if(command == &KillLowEnergyCmd)
    EXORun->set_kill_low_energy_particles(KillLowEnergyCmd.GetNewBoolValue(newValue));

  else if(command == &AnticorrAlphaCmd)
    EXORun->set_anticorr_alpha(AnticorrAlphaCmd.GetNewBoolValue(newValue));

  else if(command == &AlphaFieldCmd)
    EXORun->set_efield_alpha(AlphaFieldCmd.GetNewDoubleValue(newValue));

  else if(command == &NexAlphaCmd)
    EXORun->set_nex_alpha(NexAlphaCmd.GetNewDoubleValue(newValue));

  else if(command == &FastScintAlphaCmd)
    EXORun->set_fastscint_alpha(FastScintAlphaCmd.GetNewBoolValue(newValue));

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....





