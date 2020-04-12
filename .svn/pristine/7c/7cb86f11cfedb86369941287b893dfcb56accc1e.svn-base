#include "EXOSim/EXOPrimaryGeneratorMessenger.hh"

#include "EXOSim/EXOPrimaryGeneratorAction.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOPrimaryGeneratorMessenger::EXOPrimaryGeneratorMessenger(
                      EXOPrimaryGeneratorAction* EXOGun) :
  EXOAction(EXOGun),
  fDirectory("/generator/"),
  GenCmd("/generator/setGenerator",this),
  IsoCmd("/generator/setIsotope",this),
  XeCaptureCmd("/generator/setXeCaptureMode", this),
  CuIsoCmd("/generator/setCuIsotope",this),
  CuComponentCmd("/generator/setCuComponent", this),
  SpectralIndexCmd("/generator/setSpectralIndex",this),
  GraphFFValuesCmd("/generator/setUseGraphFFValues", this),
  RadiusFFCalcCmd("/generator/setRadiusOfFFCalc", this),
  AtomicNumberCmd("/generator/AtomicNumber",this),
  QValueCmd("/generator/QValue",this),
  MweDepthCmd("/generator/depthInMwe",this),
  UseBetaPlusCmd("/generator/useBetaPlus",this),
  fTrackSourceCmd("/generator/TrackInfo_source",this),
  fTrackTreeCmd("/generator/TrackInfo_tree",this),
  fTrackSelectCmd("/generator/TrackInfo_select",this),
  fTrackModeLastCmd("/generator/TrackInfo_modelast",this),
  fEventSourceCmd("/generator/Dicebox_source",this)
{
   
  fDirectory.SetGuidance("Generator commands");

  GenCmd.SetGuidance("Choose the primary event generator");
  GenCmd.SetGuidance(("  Choices : generalParticleSource(default),bb0n,bb2n,bb2n_iachello,bb0n_NRR, bb0n_NRL,bb0nX,muon,nCaptureXe136, nCaptureXe134, nCaptureF19, nCaptureCu" 
                      + EXOAction->GetListOfAvailableFortranGens()).c_str());
  GenCmd.SetParameterName("choice",true);
  GenCmd.SetDefaultValue("generalParticleSource");
  GenCmd.SetCandidates(("gps generalParticleSource bb0n bb2n bb2n_iachello bb0n_NRR bb0n_NRL bb0nX muon trackinfo dicebox nCaptureXe136 nCaptureXe134 nCaptureF19 nCaptureCu" 
                        + EXOAction->GetListOfAvailableFortranGens()).c_str());
  GenCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  SpectralIndexCmd.SetGuidance("Enter the spectral index for Majoron (bb0nX) decays (default=1)");
  SpectralIndexCmd.SetParameterName("spectralIndex",false,false);
  SpectralIndexCmd.SetDefaultValue(1);
  SpectralIndexCmd.SetRange("spectralIndex>=1");
  SpectralIndexCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  GraphFFValuesCmd.SetGuidance("Choose whether to use the P.Vogel 1983 curve or the Graph Values to calculate the Fermi Function.  Must be called *after* /generator/setRadiusOfFFCalc");
  GraphFFValuesCmd.SetParameterName("useGraphFFValues", true);
  GraphFFValuesCmd.SetDefaultValue(false);
  GraphFFValuesCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  RadiusFFCalcCmd.SetGuidance("Enter the radial point of the Fermi Function calculation from Graph Values.  Must be set *before* /generator/setUseGraphFFValues");
  RadiusFFCalcCmd.SetGuidance(" Choices: r0 (r=0), rR (r=R) (default: r=0)");
  RadiusFFCalcCmd.SetParameterName("RadiusOfFFCalc", true);
  RadiusFFCalcCmd.SetDefaultValue("r0");
  RadiusFFCalcCmd.SetCandidates("r0 rR");
  RadiusFFCalcCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  XeCaptureCmd.SetGuidance("Choose how to simulate missing intensities in n-Capture on Xe136");
  XeCaptureCmd.SetParameterName("mode",true);
  XeCaptureCmd.SetDefaultValue(3);
  XeCaptureCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  CuIsoCmd.SetGuidance("Enter the Cu isotope (63 or 65) for n-Capture (default = 63)");
  CuIsoCmd.SetParameterName("isotope", false, false);
  CuIsoCmd.SetDefaultValue(63);
  CuIsoCmd.AvailableForStates(G4State_PreInit, G4State_Idle);

  CuComponentCmd.SetGuidance("Enter the copper component to confine n-capture decays (default = TPC)");
  CuComponentCmd.SetGuidance("Choices : LXeVessel, InnerCryo, OuterCryo");
  CuComponentCmd.SetParameterName("choice", true);
  CuComponentCmd.SetDefaultValue("LXeVessel");
  CuComponentCmd.SetCandidates("LXeVessel InnerCryo OuterCryo");
  CuComponentCmd.AvailableForStates(G4State_PreInit, G4State_Idle);
  
  IsoCmd.SetGuidance("Choose the xenon isotope");
  IsoCmd.SetGuidance("  Choices : xe136(default),xe134,xe124,xe136excited,xe134excited");
  IsoCmd.SetParameterName("choice",true);
  IsoCmd.SetDefaultValue("xe136");
  IsoCmd.SetCandidates("xe136 xe134 xe124 xe136excited xe134excited");
  IsoCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  AtomicNumberCmd.SetGuidance("Enter the Atomic Number of the double beta decay parent nucleus");
  AtomicNumberCmd.SetParameterName("atomicNumber",false,false);
  AtomicNumberCmd.SetRange("atomicNumber>=1");
  AtomicNumberCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  QValueCmd.SetGuidance("Enter the Q-value of the double beta decay");
  QValueCmd.SetParameterName("qvalue",false,false);
  QValueCmd.SetRange("qvalue>=0.0");
  QValueCmd.SetDefaultUnit("keV");
  QValueCmd.SetUnitCategory("Energy");
  QValueCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  MweDepthCmd.SetGuidance("Enter the vertical depth in meters water equivalent (hg/cm^2) for muon simulations.");
  MweDepthCmd.SetParameterName("mwe_depth",false,false);
  MweDepthCmd.SetRange("mwe_depth>=0.0");
  MweDepthCmd.SetDefaultValue(1585.0);
  MweDepthCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  UseBetaPlusCmd.SetGuidance("Choose whether to simulate double beta plus");
  UseBetaPlusCmd.SetParameterName("usebetaplus",true);
  UseBetaPlusCmd.SetDefaultValue(false);
  UseBetaPlusCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  //fTrackSourceCmd("/generator/TrackInfo_source",this),
  fTrackSourceCmd.SetGuidance("Set a place where a TrackInfo tree is, it could be file name or TObject name.");
  fTrackSourceCmd.SetParameterName("source",true);
  fTrackSourceCmd.SetDefaultValue("");
  fTrackSourceCmd.AvailableForStates(G4State_PreInit,G4State_Idle);
  //fTrackTreeCmd("/generator/TrackInfo_tree",this),
  fTrackTreeCmd.SetGuidance("Set a name of TrackInfo TTree.");
  fTrackTreeCmd.SetParameterName("tree",true);
  fTrackTreeCmd.SetDefaultValue(EXOMiscUtil::GetMCTrackTreeName().c_str());
  fTrackTreeCmd.AvailableForStates(G4State_PreInit,G4State_Idle);
  //fTrackSelectCmd("/generator/TrackInfo_select",this),
  fTrackSelectCmd.SetGuidance("Set a name of TEventList or selection string to use.");
  fTrackSelectCmd.SetParameterName("source",true);
  fTrackSelectCmd.SetDefaultValue("");
  fTrackSelectCmd.AvailableForStates(G4State_PreInit,G4State_Idle);
  //fTrackModeLastCmd("/generator/TrackInfo_modelast",this)
  fTrackModeLastCmd.SetGuidance("Use track parameters at end of track instead of at vertex");
  fTrackModeLastCmd.SetParameterName("trackinfo_modelast",true);
  fTrackModeLastCmd.SetDefaultValue(false);
  fTrackModeLastCmd.AvailableForStates(G4State_PreInit,G4State_Idle);
  //fEventSourceCmd("/generator/Dicebox_source",this),
  fEventSourceCmd.SetGuidance("Set a name of file with dicebox output.");
  fEventSourceCmd.SetParameterName("source",true);
  fEventSourceCmd.SetDefaultValue("");
  fEventSourceCmd.AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOPrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue)
{ 
  if( command == &GenCmd )
   { EXOAction->set_generator(newValue);}

  else if( command == &IsoCmd )
   { EXOAction->set_isotope(newValue);}

  else if( command == &XeCaptureCmd )
    { EXOAction->use_XeCaptureMode(XeCaptureCmd.GetNewIntValue(newValue));}

  else if( command == &CuIsoCmd )
    { EXOAction->set_Cu_isotope(CuIsoCmd.GetNewIntValue(newValue));}
  
  else if(command == &CuComponentCmd )
    { EXOAction->set_Cu_Component(newValue);}

  else if( command == &AtomicNumberCmd )
    { EXOAction->set_atomic_number(AtomicNumberCmd.GetNewIntValue(newValue));}

  else if( command == &SpectralIndexCmd )
    { EXOAction->set_majoron_spectral_index(SpectralIndexCmd.GetNewIntValue(newValue));}
  else if( command == &RadiusFFCalcCmd )
    { EXOAction->set_radius_ff_calc(newValue);}

  else if( command == &GraphFFValuesCmd )
    { EXOAction->set_use_graph_ff_values(GraphFFValuesCmd.GetNewBoolValue(newValue));}

  else if( command == &QValueCmd )
    { EXOAction->set_q_value(QValueCmd.GetNewDoubleValue(newValue));}

  else if( command == &MweDepthCmd )
    { EXOAction->set_mwe_depth(MweDepthCmd.GetNewDoubleValue(newValue));}

  else if( command == &UseBetaPlusCmd )
    { EXOAction->use_beta_plus(UseBetaPlusCmd.GetNewBoolValue(newValue));}

  else if( command == &fTrackSourceCmd )
    { EXOAction->SetTrackSourceName(newValue); }

  else if( command == &fTrackTreeCmd )
    { EXOAction->SetTrackTreeName(newValue); }

  else if( command == &fTrackSelectCmd )
    { EXOAction->SetTrackSelection(newValue); }

  else if( command == &fTrackModeLastCmd )
    { EXOAction->SetTrackModeLast(fTrackModeLastCmd.GetNewBoolValue(newValue)); }

  else if( command == &fEventSourceCmd )
    { EXOAction->SetEventSourceName(newValue); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

