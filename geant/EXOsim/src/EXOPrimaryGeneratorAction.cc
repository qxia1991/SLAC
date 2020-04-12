//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//

// **********************************************************************

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "EXOSim/EXOPrimaryGeneratorAction.hh"
#include "Randomize.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOSim/EXORunAction.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "EXOSim/EXOFortGen.hh"

#ifdef HAVE_ROOT
#include "TFile.h"
#include "TH2.h"
#include "TGraph.h"
#endif

#include <fstream>
#include <sstream>
#include <vector>

using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXOPrimaryGeneratorAction::EXOPrimaryGeneratorAction() :
    gunMessenger(this),
    isotope("xe136"),
    generator("generalParticleSource"),
    q_value(DOUBLE_BETA_DECAY_Q_VALUE),
    ff_factor(1),
    norm_calculated(false),
    normalization(1),
    K_spectral_max(1),
    CuIsotope(63),
    CuComponent("LXeVessel"),
    XeCaptureMode(3),
    atomic_number(54),
    majoron_spectral_index(1),
    use_graph_ff_values(false),
    radius_of_ff_calc("r0"),
    generate_gamma(false),
    n_gamma(0),
    mwe_depth(1585),
    fTrackSource(),
    fTreeName(EXOMiscUtil::GetMCTrackTreeName()), fTree(0), fTreeFailed(false),
    fTrackSelect(), fList(0),
    fStore(0), fIEntry(-1), fEventID(-1), fEventNum(0),
    fEventSource(), fFile(0)

{
  gammaE[0] = 0.0;
  gammaE[1] = 0.0;

  generalParticleSource = new G4GeneralParticleSource ();
  particleGun  = new G4ParticleGun(1);

  // Initialize particle definitions

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  aBeta = particleTable->FindParticle(particleName="e-");
  aBetaPlus = particleTable->FindParticle(particleName="e+");
  aMuonMinus = particleTable->FindParticle(particleName="mu-");
  aMuonPlus = particleTable->FindParticle(particleName="mu+");
  aGamma = particleTable->FindParticle(particleName="gamma");
    
  // default Q-value and particle used for Xe-136 double beta decay
  particle_used = aBeta;

  fLoopMode = -1, fModeLast = false;

      

}

EXOPrimaryGeneratorAction::~EXOPrimaryGeneratorAction()
{
  delete generalParticleSource;
  delete particleGun;
}

void EXOPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  EXORunAction* runAction = (EXORunAction*)G4RunManager::GetRunManager()->GetUserRunAction();

  //Alpha Team Anticorrelation - Contact Derek for Questions
  if(runAction->returnAlphaAnticorr()){

    //Get ready to save random seed file
    G4double seedint = runAction->returnRunID(); 
    //Put the random number here.

    G4String s = G4UIcommand::ConvertToString(seedint); 
    G4String fileN ="currentEvent.rndm"+s; 

    if(runAction->returnIncrement()%2!=0){ 
      //Reload Random Seed Engine Status to recreate previous event
      
      HepRandom::restoreEngineStatus(fileN); 
    }else{
      //Save Random Seed Engine Status  
      HepRandom::saveEngineStatus(fileN); 
    }
  }

  G4bool bb_generator = false;

  if ( generator == "gps" || generator == "generalParticleSource" ) {
    generalParticleSource->GeneratePrimaryVertex(anEvent);
  }
  else if ( generator == "bb0n" ) {
    bb_generator = true;
    calc_norm(0);
    generate_bb_spectralIndex(anEvent, 0);
  }
  else if ( generator == "bb2n" ) {
    bb_generator = true;
    calc_norm(5);
    generate_bb_spectralIndex(anEvent, 5);
  }
  else if ( generator == "bb2n_iachello" ) {
    bb_generator = true;
    generate_bb2n_iachello(anEvent);
  }
  else if ( generator == "bb0n_NRR" || generator == "bb0n_NRL") {
    bb_generator = true;
    generate_bb0n_NME(anEvent);
  }
  else if ( generator == "bb0nX" ) {
    bb_generator = true;
    calc_norm(majoron_spectral_index);
    generate_bb_spectralIndex(anEvent, majoron_spectral_index);
  }
  else if ( generator == "muon" ) {
    generate_muon( anEvent );
  }
  else if ( generator == "trackinfo" ) {
    generate_trackinfo( anEvent );
  }
  else if ( generator == "dicebox" ) {
    generate_dicebox( anEvent );
  }
  else if ( generator == "nCaptureXe136"){
    generate_nCaptureXe136( anEvent, XeCaptureMode);
  }
  else if ( generator == "nCaptureXe134"){
    generate_nCaptureXe134( anEvent );
  }
  else if ( generator == "nCaptureF19"){
    generate_nCaptureF( anEvent );
  }
  else if ( generator == "nCaptureCu"){
    generate_nCaptureCu(anEvent, CuIsotope, CuComponent);
  }
  else if ( FortGeneratorIsAvailable( generator ) ) {
    generate_fortWithName( generator, anEvent ); 
  }

  if (bb_generator && generate_gamma) {
    generate_excitedStateGammas( anEvent );
  }
}

void EXOPrimaryGeneratorAction::set_generator( G4String value )
{
  // Set the generator
  if (value == "gps"                   || 
      value == "generalParticleSource" || 
      value == "bb0n"                  ||
      value == "bb2n"                  || 
      value == "bb2n_iachello"         || 
      value == "bb0n_NRR"              || 
      value == "bb0n_NRL"              || 
      value == "bb0nX"                 || 
      value == "muon"                  ||
      value == "trackinfo"             ||
      value == "dicebox"               ||
      value == "nCaptureXe136"         ||
      value == "nCaptureXe134"         ||
      value == "nCaptureF19"           ||
      value == "nCaptureCu"            ||
      FortGeneratorIsAvailable(value)) {
    G4cout << "using " << value << " generator" << G4endl;
    generator = value;
  } else {
    G4cout << "generator " << value << " not recognized" << G4endl;
  }

  norm_calculated = false;

  if (value == "bb2n_iachello") {
    setup_iachello();
  }
  if (value == "bb0n_NRR" || value == "bb0n_NRL") {
    setup_NME(value);
  }


}

void EXOPrimaryGeneratorAction::set_atomic_number( G4int value )
{
 G4cout << "***************************************************" << G4endl;

 if ( value < 1 ) {
   G4cout << "Error: Atomic Number, Z, has to be > 0" << G4endl;
   G4cout << "Substituting default value of Z for xenon" << G4endl;
   atomic_number = 54;
 } else {
   G4cout << "setting the Atomic Number to " << value << G4endl;
   atomic_number = value;
 }
 G4cout << "***************************************************" << G4endl;
 norm_calculated = false;

}

void EXOPrimaryGeneratorAction::set_Cu_isotope( G4int value)
{
  G4cout << "***************************************************" << G4endl;
    
 if (value != 63 && value != 65){
    G4cout << "Error: Must set Cu isotope to 63 or 65" << G4endl;
    G4cout << "Substituting default value for Cu" << G4endl;
    CuIsotope = 63; 
    } else {
    G4cout << "setting the Cu Isotope for n-Capture to " << value << G4endl;
    CuIsotope = value;
    }
  G4cout << "***************************************************" << G4endl;
  norm_calculated = false;
}

void EXOPrimaryGeneratorAction::set_Cu_Component( G4String value)
{
  G4cout << "***************************************************" << G4endl;

  if (value != "LXeVessel" && value != "InnerCryo" && value != "OuterCryo"){
    G4cout << "Error: must be the LXeVessel, InnerCryo, or OuterCryo" << G4endl;}  else {
    G4cout << "setting the component to " << value << G4endl;
    CuComponent = value;
  }
  G4cout << "***************************************************" << G4endl;
}

void EXOPrimaryGeneratorAction::use_XeCaptureMode( G4int value )
{
  G4cout << "***************************************************" << G4endl;
  switch(value) {
    case 1:
      G4cout << "Using 'Exact copy' mode in n-Capture on Xe136" << G4endl;
      XeCaptureMode = value;
      break;
    case 2:
      G4cout << "Using 'Efficiency boost' mode in n-Capture on Xe136" << G4endl;
      XeCaptureMode = value;
      break;
    case 3:
      G4cout << "Using 'Random Gammas' mode in n-Capture on Xe136" << G4endl;
      XeCaptureMode = value;
      break;
    case 4:
      G4cout << "Using 'Internal conversion' mode in n-Capture on Xe136" << G4endl;
      XeCaptureMode = value;
      break;
    default:
      G4cerr << "Unknown mode for n-Capture on Xe136: " << value << G4endl;
  };
  G4cout << "***************************************************" << G4endl;  
}

void EXOPrimaryGeneratorAction::set_q_value( G4double value )
{
  G4cout << "***************************************************" << G4endl;

  if ( value <= 0 *keV ) {
    G4cout << "Error: Q-value has to be > 0" << G4endl;
    G4cout << "Substituting default value for Xe-136 " << G4endl;
    q_value = DOUBLE_BETA_DECAY_Q_VALUE;
  } else {
    G4cout << "setting the Q-value to " << value *keV << " keV" << G4endl;
    // this output may not factor in the unit correctly
    q_value = value;
  }
  G4cout << "***************************************************" << G4endl;
  norm_calculated = false;

}

void EXOPrimaryGeneratorAction::use_beta_plus( G4bool do_use_beta_plus )
{
  if( do_use_beta_plus ){
    ff_factor = -1;
    particle_used = aBetaPlus;
  }
  norm_calculated = false;
}

void EXOPrimaryGeneratorAction::set_majoron_spectral_index( G4int value )
{
  G4cout << "***************************************************" << G4endl;

  if( value < 0 ){
    G4cout << "Error: Spectral index has to be >= 0" << G4endl;
    G4cout << "Substituting default value of 1" << G4endl;
    G4cout << "***************************************************" << G4endl;
    majoron_spectral_index = 1;
  } else {
    G4cout << "setting spectral index to " << value << G4endl;
    majoron_spectral_index = value;
  }
  G4cout << "***************************************************" << G4endl;
  norm_calculated = false;
}

void EXOPrimaryGeneratorAction::set_use_graph_ff_values(G4bool value)
{
  G4cout << "***************************************************" << G4endl;
  if( value ){
    use_graph_ff_values = true;
    G4cout << "Using Fermi Function values from Graph of Peter Vogel calculated values" << G4endl;
    G4cout << "Radius of FF calc = " << radius_of_ff_calc << G4endl;
    setup_FF_GraphValues(radius_of_ff_calc);
  }
}


void EXOPrimaryGeneratorAction::set_radius_ff_calc(G4String value)
{
  G4cout << "***************************************************" << G4endl;
  if (value == "r0"         ||
      value == "rR"){
    G4cout << "Using Fermi Function calculation at " << value << G4endl;
  
    radius_of_ff_calc = value;
  }
  
  else {G4cout << "Radial value not recognized" << G4endl;}
    
}

void EXOPrimaryGeneratorAction::set_isotope( G4String value )
{
  // Allow user to choose between the double beta decays of 
  // the various xenon isotopes. Double beta decays for
  // Xe-136 (default) and Xe-134 and double beta+ (positron)
  // decay for Xe-124.
  // As of 03/09/12 - can generate decay (choose via the usual
  // method of generator) to excited state by providing gamma
  // information here. Xe-136 excited state is now available
  // Xe-134 excited added on 03/12/12
 
  if (value == "xe136"        || 
      value == "xe134"        || 
      value == "xe124"        || 
      value == "xe136excited" ||
      value == "xe134excited") {
    G4cout << "using isotope" << value << "" << G4endl;
    isotope = value;
    if ( isotope == "xe136"){
      particle_used = aBeta;
      q_value = DOUBLE_BETA_DECAY_Q_VALUE;
      ff_factor = 1; 
      atomic_number = 54;
      generate_gamma = false;
    }
    else if ( isotope == "xe134"){ 
      particle_used = aBeta;
      q_value = XE_134_Q_VALUE;
      ff_factor = 1;
      atomic_number = 54;
      generate_gamma = false;
    }
    else if ( isotope == "xe124"){
      particle_used = aBetaPlus;
      q_value = XE_124_Q_VALUE;
      ff_factor = -1;
      atomic_number = 54;
      generate_gamma = false;
    }
    else if ( isotope == "xe136excited"){
      particle_used = aBeta;
      ff_factor = 1;
      atomic_number = 54;
      generate_gamma = true;
      n_gamma = 2;
      gammaE[0] = 760.50*keV;
      gammaE[1] = 818.514*keV;
      // set gamma energies here
      // 760.50 keV and 818.514 keV
      q_value = DOUBLE_BETA_DECAY_Q_VALUE - 760.50*keV - 818.514*keV;
    }
    else if ( isotope == "xe134excited"){
      particle_used = aBeta;
      ff_factor = 1;
      atomic_number = 54;
      generate_gamma = true;
      n_gamma = 1;
      gammaE[0] = 604.705*keV;
      // set gamma energies here
      // 604.705 keV
      q_value = XE_134_Q_VALUE - 604.705*keV;
      
     // When expanding the generator to simulate decays to other excited state
     // of xenon isotopes, enter the Q-values and the desired (uncorrelated) 
     // gamma energies here
    }
  } else {
    G4cout << "isotope " << value << " not recognized" << G4endl;
  }
  norm_calculated = false;
}

void EXOPrimaryGeneratorAction::set_mwe_depth( G4double value )
{
  // Set the meter water equivalent vertical depth for muon simulations
  // (1 m.w.e. = 1 hg/cm^2)
  
  mwe_depth = value;
  muon_ang_max = -1;  // reset this so it's calculated anew

  G4cout << "Muon spectrum will be simulated with " << mwe_depth <<
    " m.w.e. vertical depth angular and energy distributions." << G4endl;
}

void EXOPrimaryGeneratorAction::setup_iachello()
{
  G4cout << "***************************************************" << G4endl;
  G4cout << "using the Iachello spectra " << G4endl;
  G4cout << "***************************************************" << G4endl;
  N_iachello = 245;
  iachello_bin_width = 9.99*keV;
  
  // Read in the differential Iachello spectrum from the file
  
  for ( int i = 0; i < N_iachello; i++ ) {
    for ( int j = 0; j < N_iachello; j++ ) {
      iachello_differential_spectrum_array[i][j] = 0.0;
    }
  }
  
  // This is a dummy spectrum .... only one non-zero bin.
  // The dummy spectrum is returned if the Iachello bb2n table file cannot
  // be found.
  
  iachello_differential_spectrum_array[100][100] = 1.0;
  
#ifdef HAVE_ROOT
  TFile *f = new TFile(EXOMiscUtil::SearchForFile("data/iachello/Iachello_bb2n_spectrum.root").c_str(), "READ" );
  TH2F *h1 = NULL;
  h1 = (TH2F*)f->Get("h1");
  if ( h1 == NULL ) {
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
    G4cout << "EXOPrimaryGeneratorAction::set_use_iachello: unable to retreive Iachello histogram from root file." << G4endl;
    G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  }
  
  else if ( h1->GetNbinsX() == N_iachello && h1->GetNbinsY() == N_iachello ) {
    for ( int i = 0; i < N_iachello; i++ ) {
      for ( int j = 0; j < N_iachello; j++ ) {
	iachello_differential_spectrum_array[i][j] = h1->GetBinContent(i+1,j+1);
      }
    }
  }
  
  else {
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
    G4cout << "EXOPrimaryGeneratorAction::set_use_iachello: Dimensions of Iachello histogram are wrong" << G4endl;
    G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  }
  
  f->Close();
#else
  G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  G4cout << "EXOPrimaryGeneratorAction::set_use_iachello: No support for ROOT files. " << G4endl;
  G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
  G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
#endif /* HAVE_ROOT */
  
  // Calculate the sum spectrum from the differential spectrum.
  // Also record the maximum of the sum spectrum.
  
  for ( int i = 0; i < N_iachello; i++ ) {
    G4double sum = 0.0;
    for ( int m = 0; m <= i; m++ ) {
      sum += iachello_differential_spectrum_array[i-m][m];
    }
    iachello_sum_spectrum_array[i] = sum;
    if ( sum > K_spectral_max_iachello ) {
      K_spectral_max_iachello = sum;
    }
  }
}

void EXOPrimaryGeneratorAction::setup_NME( G4String generator )
{
  // Neutrinoless double-beta decay with nonstandard Majoron emission PRL 122, 181801 2019
  G4cout << "***************************************************" << G4endl;
  G4cout << "using the nonstandard majoron emission spectra " << G4endl;
  G4cout << "***************************************************" << G4endl;
  N_NME = 245;
  NME_bin_width = 9.99*keV;
  
  // Read in the differential NME spectrum from the file
  
  for ( int i = 0; i < N_NME; i++ ) {
    for ( int j = 0; j < N_NME; j++ ) {
      NME_differential_spectrum_array[i][j] = 0.0;
    }
  }
  
  // This is a dummy spectrum .... only one non-zero bin.
  // The dummy spectrum is returned if the bb0n NME table file cannot
  // be found.
  
  NME_differential_spectrum_array[100][100] = 1.0;
  
#ifdef HAVE_ROOT
  //use Iachello spectrum file for code test, will replace with correct spectrum later.
  TFile *f = new TFile(EXOMiscUtil::SearchForFile("data/newmajoron/nsi_2d.root").c_str(), "READ" );
  TH2F *h1 = NULL;
  // NME model has two spectra, need to set up separately.
  if(generator == "bb0n_NRR")
      h1 = (TH2F*)f->Get("nsi_rr");
  else
      h1 = (TH2F*)f->Get("nsi_rl");
  if ( h1 == NULL ) {
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
    G4cout << "EXOPrimaryGeneratorAction::set_use_NME : unable to retreive bb0n NME histogram from root file." << G4endl;
    G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  }
  
  else if ( h1->GetNbinsX() == N_NME && h1->GetNbinsY() == N_NME )  {
    for ( int i = 0; i < N_NME; i++ ) {
      for ( int j = 0; j < N_NME; j++ ) {
	NME_differential_spectrum_array[i][j] = h1->GetBinContent(i+1,j+1);
      }
    }
  }
  
  else {
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
    G4cout << "EXOPrimaryGeneratorAction::set_use_NME: Dimensions of NME histogram are wrong" << G4endl;
    G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  }
  
  f->Close();
#else
  G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
  G4cout << "EXOPrimaryGeneratorAction::set_use_NME: No support for ROOT files. " << G4endl;
  G4cout << "Using dummy spectrum with all probability at E1 = 1.0 MeV and E2 = 1.0 MeV" << G4endl;
  G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl;
#endif /* HAVE_ROOT */
  
  // Calculate the sum spectrum from the differential spectrum.
  // Also record the maximum of the sum spectrum.
  
  for ( int i = 0; i < N_NME; i++ ) {
    G4double sum = 0.0;
    for ( int m = 0; m <= i; m++ ) {
      sum += NME_differential_spectrum_array[i-m][m];
    }
    NME_sum_spectrum_array[i] = sum;
    if ( sum > K_spectral_max_NME) {
      K_spectral_max_NME = sum;
    }
  }
}


void EXOPrimaryGeneratorAction::calc_norm(G4int index)
{
  // Calculate the normalization factor (normalization) as well
  // as the maximum value of the K distribution (K_spectral_max) for
  // spectral index. This normalization varies for different isotopes.
  // This function should only run once in the beginning.

  if (norm_calculated) {
    return;
  }

  G4double T0 = q_value/ELECTRON_MASS;

  normalization = 1; // So we get the unnormalized value
    
  const G4int N = 100;
  G4double sum_spec[N+1];
  memset(sum_spec, 0, sizeof(sum_spec));

  G4double sum_max = 0;

  for (G4int i = 0; i <N + 1; ++i) {
    sum_spec[i] = sum_spectrum(i*T0/N, index);
   
    if (sum_spec[i] > sum_max) {
      sum_max = sum_spec[i];
    }
  }

  normalization = SimpsonsRule(0, T0, N, sum_spec);
  K_spectral_max = 1.01*sum_max/normalization;
  norm_calculated = true;

  G4cout << "normalization " << normalization << G4endl;
  G4cout << "K_spectral_max " << K_spectral_max << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXOPrimaryGeneratorAction::generate_fortWithName( const std::string& gen, G4Event *anEvent )
{
  // Use one of the available fortran generators to generate an event.
  // If the selected generator isn't available, get out.  These generators
  // only generate energy, momentum, and type.  Therefore, this function
  // uses the position distribution from the current G4SingleParticleSource
  // from the GPS to generate the event position.  

  // This check is done because this is a public function and could be
  // called by any user not knowing the correct name.
  if (!FortGeneratorIsAvailable(gen)) return;
  const EXOFortGen::ParVec& generatedParts = 
    EXOFortGen::GetParticlesWithGenerator( gen, 0.0 );

  // Use the Single Particle source to grab the position.
  G4ThreeVector position = generalParticleSource->GetCurrentSource()->GetPosDist()->GenerateOne();
  // set the particle gun position to source vertex
  particleGun->SetParticlePosition(position);

  for (size_t i=0;i<generatedParts.size();i++) {
    particleGun->SetParticleDefinition(generatedParts[i].fParticle);
    particleGun->SetParticleMomentumDirection(generatedParts[i].fMomentum);
    particleGun->SetParticleEnergy(generatedParts[i].fEnergy);
    particleGun->SetParticleTime(generatedParts[i].fDeltaT);
    particleGun->GeneratePrimaryVertex(anEvent);
  }
   
  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double EXOPrimaryGeneratorAction::D_spectrum(G4double K, G4double D, G4int index)
{
    // Returns dN/(dE1dE2) for a decay with spectral index
    // For bb0n, you'd use 0, for bb2n you'd use 5, etc.

    // Energies (E1, E2),  momenta (p1, p2), and kinetic energies (T1, T2)
    // are all divided by the electron mass.
    //
    // K = T1+T2 (Sum of kinetic energies; Range: 0 < K < T0 )
    // D = T1-T2 (their difference; Range:  -K < D < +K )
    //
    // T1 = (K+D)/2
    // T2 = (K-D)/2
 
    G4int Z = atomic_number;

    G4double T0 = q_value/ELECTRON_MASS; 

    G4double E1 = 0.5*(K + D) + 1.0;
    G4double E2 = 0.5*(K - D) + 1.0;

    G4double p1 = sqrt(E1*E1 - 1.0);
    G4double p2 = sqrt(E2*E2 - 1.0);

    G4double T1 = E1 - 1.0;
    G4double T2 = E2 - 1.0;

    if(GetStatusUseGraphFFValues())
      {
	return p1*E1*fermi_function_GraphValues(T1*ELECTRON_MASS)*p2*E2*fermi_function_GraphValues(T2*ELECTRON_MASS)*pow(T0 - K, index);
      }

    else
      {
	return p1*E1*fermi_function(Z,T1*ELECTRON_MASS)*p2*E2*fermi_function(Z,T2*ELECTRON_MASS)*pow(T0 - K, index);
    }
}

G4double EXOPrimaryGeneratorAction::D_spectrum_max(G4double K, G4int index)
{
  // This function calculates the maximum value of the D spectrum for
  // spectral index n.
 
    const G4int N = 100; // N is the # of bins.
    
    G4double a = -K;
    G4double b = K;
    G4double dD = (b - a)/N;

    G4double D_spectral_max = 0.;

    for (G4int i = 0; i < N + 1; i++) {

      G4double D;

      if (i != N) {   
        D = a + i*dD;
      } else {
        D = b;
      }

      G4double D_temp = D_spectrum(K, D, index);
      if (D_temp > D_spectral_max) {
	D_spectral_max = D_temp;
      }

    }

    return D_spectral_max; 
}

G4double EXOPrimaryGeneratorAction::sum_spectrum(G4double K, G4int index, G4double &D_spectral_max)
{
    // This function calculates the sum electron spectrum dN/dK for the
    // decay with spectral index n
    // Also stores the maximum of dN/dD since it's computing it anyway

    G4double phase_space_sum = 0;
    G4double last_sum;

    const G4int N = 100; // N is the # of bins.
    G4double phase_space[N + 1];
    memset(phase_space, 0, sizeof(phase_space));

    G4double a = -K;
    G4double b = K;
    G4double dD = (b - a)/N;

    G4double D = a;

    D_spectral_max = 0.;

    for (G4int i = 0; i < N + 1; ++i) {

      if (i != N) {
        D = a + i*dD;
      } else {
        D = b;
      }

      phase_space[i] = D_spectrum(K, D, index);
      if (phase_space[i] > D_spectral_max) {
	D_spectral_max = phase_space[i];
      }
    }
    G4double normalized_spectrum = SimpsonsRule(a, b, N, phase_space)/normalization;
    //G4cout << K*ELECTRON_MASS << "\t" << normalized_spectrum << G4endl;
    return normalized_spectrum;

}

G4double EXOPrimaryGeneratorAction::sum_spectrum(G4double K, G4int index)
{
    // This function calculates the sum electron spectrum dN/dK for the
    // decay with spectral index
    G4double dummy = 0;
    return sum_spectrum(K, index, dummy);
}

G4ThreeVector EXOPrimaryGeneratorAction::get_random_position(int mode)
{
  switch (mode) {
  case 0: {
  // Returns a random position in the active xenon

    G4double phi, r, x, y, z;
    G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4bool XenonVolume = false;
    while(XenonVolume == false){
      z = (2.0*G4UniformRand() - 1.0)*CATHODE_APDFACE_DISTANCE;
      // Uniform in a disc
      phi = 2*pi*G4UniformRand();
      r = ACTIVE_XENON_RADIUS*sqrt(G4UniformRand());
    
      x = r*cos(phi);
      y = r*sin(phi);

      G4VPhysicalVolume *volume = theNavigator->LocateGlobalPointAndSetup(G4ThreeVector(x, y, z));
      if (volume->GetName()=="ActiveLXe") XenonVolume = true;
    }

    return G4ThreeVector(x, y, z);
  } break;
  case 1: {
    // returns position in active or inactive xenon carefully

    G4double r0, x0, y0, z0, phi0;
    G4double INACTIVE_XENON_R = 22.7330; // FIXME EXODimensions.h // really, I think this is fine (jba)
    G4double INACTIVE_XENON_Z = 22.258;
    G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4bool XenonVolume = false;
    while(XenonVolume == false){
      phi0 = 2*pi*G4UniformRand();
      r0 = INACTIVE_XENON_R*sqrt(G4UniformRand());
      z0 = INACTIVE_XENON_Z*(2*(G4UniformRand() - 0.5));
      x0 = r0*cos(phi0);
      y0 = r0*sin(phi0);

      G4VPhysicalVolume *volume = theNavigator->LocateGlobalPointAndSetup(G4ThreeVector(x0*cm, y0*cm, z0*cm));
      if (volume->GetName()=="ActiveLXe" || volume->GetName()=="InactiveLXe") XenonVolume = true;
    }
    return G4ThreeVector(x0*cm, y0*cm, z0*cm);
  } break;
  default:
    ;
  }
  // should not happen
  return G4ThreeVector();
}

G4ThreeVector EXOPrimaryGeneratorAction::get_random_direction()
{
  G4double theta = acos(2*(G4UniformRand()-0.5));
  G4double phi   = 2*pi*G4UniformRand();

  G4double ux   = sin(theta)*cos(phi);
  G4double uy   = sin(theta)*sin(phi);
  G4double uz   = cos(theta);

  return G4ThreeVector(ux, uy, uz);
}

G4ThreeVector EXOPrimaryGeneratorAction::get_2nd_electron_direction(G4ThreeVector r0,
								    G4double T1,
								    G4double T2)
{
    // For the bb2n decay, the angular distribution of the electrons
    // is of the form
    // f(cos)= 1 - beta1*beta2*cos(theta12) with beta = p/E,
    // for the 0+ --> 0+ transition (cf. Boehm & Vogel p.146).
    //
    // This distribution favors a negative cos(theta12), i.e., when the
    // electrons are pointing at opposite hemispheres.
    //
    // The range of f(cos) is:
    // 1 - beta1*beta2 < f(cos) < 1 + beta1*beta2
    //
    // When the direction of the first electron is chosen, the direction
    // of the second electron has to be generated according to the above
    // angular distribution.

    G4double E1 = T1 + ELECTRON_MASS;
    G4double E2 = T2 + ELECTRON_MASS;
    G4double p1 = sqrt(E1*E1 - ELECTRON_MASS*ELECTRON_MASS);
    G4double p2 = sqrt(E2*E2 - ELECTRON_MASS*ELECTRON_MASS);
    G4double beta1 = p1/E1;
    G4double beta2 = p2/E2;
    G4double theta12 = 0;
    G4double phi12 =  2*pi*G4UniformRand();
    G4double y = 0;

    // 1 - beta1*beta2 < f(cos) < 1 + beta1*beta2
    do {
      theta12 = acos(2*(G4UniformRand()-0.5));
      y = 1.0 + beta1*beta2*(2*G4UniformRand()-1.0);
    } while (y > 1.0 - beta1*beta2*cos(theta12));

    G4double ux2 = sin(theta12)*cos(phi12);
    G4double uy2 = sin(theta12)*sin(phi12);
    G4double uz2 = cos(theta12);

    G4double phi = atan2(r0.y(), r0.x());
    G4double theta = acos(r0.z()/r0.mag());
    // TODO: The G4ThreeVector class has methods for these
    // I'm just not sure what convention it uses for which
    // is theta and which is phi.

    // Rotate second electron to detector coordinates

    G4double ct = cos(theta);
    G4double st = sin(theta);
    G4double cp = cos(phi);
    G4double sp = sin(phi);

    G4double uxp = ct*ux2 + st*uz2;
    G4double uyp = uy2;
    G4double uzp = -1.0*st*ux2 + ct*uz2;
    ux2 = uxp;
    uy2 = uyp;
    uz2 = uzp;

    uxp = cp*ux2 - sp*uy2;
    uyp = sp*ux2 + cp*uy2;
    uzp = uz2;
    ux2 = uxp;
    uy2 = uyp;
    uz2 = uzp;

    return G4ThreeVector(ux2, uy2, uz2);
}

void EXOPrimaryGeneratorAction::generate_excitedStateGammas( G4Event *anEvent )
{
  // For the decay to excited state, the two gammas are emitted
  // from the de-excitation sequence of 0+ -> 2+ -> 0+ which 
  // has an angular correlation of
  // f(theta) = 5/8 x (1 - 3 cos^2 (theta) + 4 cos^4(theta))
  // from Duerr, M. etc. Physical Review D 84, 093004 (2011)
  //
  // With this angular correlation, the preferred angle between
  // the two output gammas is 0 (same direction) or pi (back-to-back).
  // max value is 5/4 (theta = 0 or pi) and 
  // min value is 5/16 (theta = pi/4, 3pi/4...)
  //
  // When the direction of the first gamma is chosen, the direction of the
  // second gamma has to generated according to the above angular correlation
  //
  // This situation applies only for the case when n_gamma == 2

  particleGun->SetParticleDefinition(aGamma);

    if(n_gamma == 2){
      // first gamma
      particleGun->SetParticleEnergy(gammaE[0]);

      G4double thetag1 = acos(2*(G4UniformRand()-0.5));
      G4double phig1   = 2*pi*G4UniformRand();
      G4double uxg1   = sin(thetag1)*cos(phig1);
      G4double uyg1   = sin(thetag1)*sin(phig1);
      G4double uzg1   = cos(thetag1);

      // set the first gamma momentum direction
      particleGun->SetParticleMomentumDirection(G4ThreeVector(uxg1,uyg1,uzg1));
      particleGun->GeneratePrimaryVertex(anEvent); 
      
      // second gamma
      G4double thetag12 = 0;
      G4double phig2 = 2*pi*G4UniformRand();
      G4double costheta = 0;
      G4double y = 0.;

      do {
		thetag12 = acos(2*(G4UniformRand()-0.5)); // theta phase space now properly considered

	        y = 1.25*G4UniformRand(); // uniformly sample y variable (1.25 is the max of he distribution)
      } while ( y > 5./16
		&& y > (5./8*(1 - 3*pow(cos(thetag12), 2) +
			      4*pow(cos(thetag12), 4))));

      // I think the first condition is redundant, but I'm just copying code.
      
      G4double uxg2 = sin(thetag12)*cos(phig2);
      G4double uyg2 = sin(thetag12)*sin(phig2);
      G4double uzg2 = cos(thetag12);
  
      // Rotate second gamma to detector coordinates
      
      G4double ctg = cos(thetag1);
      G4double stg = sin(thetag1);
      G4double cpg = cos(phig1);
      G4double spg = sin(phig1);
      
      G4double uxpg = ctg*uxg2 + stg*uzg2;
      G4double uypg = uyg2;
      G4double uzpg = -1.0*stg*uxg2 + ctg*uzg2;
      uxg2 = uxpg;
      uyg2 = uypg;
      uzg2 = uzpg;
  
      uxpg = cpg*uxg2 - spg*uyg2;
      uypg = spg*uxg2 + cpg*uyg2;
      uzpg = uzg2;
      uxg2 = uxpg;
      uyg2 = uypg;
      uzg2 = uzpg;
      
      // set the particle momentum direction
      particleGun->SetParticleEnergy(gammaE[1]);
      
      particleGun->SetParticleMomentumDirection(G4ThreeVector(uxg2,uyg2,uzg2));
      particleGun->GeneratePrimaryVertex(anEvent);
    } else {
      for(int i = 0; i < n_gamma; ++i) {
	
	particleGun->SetParticleEnergy(gammaE[i]);

	// pick a direction uniformly
	G4double thetag1 = acos(2*(G4UniformRand()-0.5));
	G4double phig1 = 2*pi*G4UniformRand();
	G4double uxg1 = sin(thetag1)*cos(phig1);
	G4double uyg1 = sin(thetag1)*sin(phig1);
	G4double uzg1 = cos(thetag1);

	particleGun->SetParticleMomentumDirection(G4ThreeVector(uxg1,uyg1,uzg1));
	particleGun->GeneratePrimaryVertex(anEvent);
      }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void EXOPrimaryGeneratorAction::generate_bb_kineticEnergies( G4Event *anEvent, G4double T1, G4double T2) {
  // Generate a double beta decay event where the electrons have
  // energies given by T1 and T2

  // Beta source vertex----------------------------------------

  particleGun->SetParticlePosition(get_random_position());

  // Generate first electron------------------------------------

  particleGun->SetParticleEnergy(T1);
  G4ThreeVector direction0 = get_random_direction();
  particleGun->SetParticleMomentumDirection(direction0);
  particleGun->GeneratePrimaryVertex(anEvent);
  

  // Generate second electron------------------------------------
  particleGun->SetParticleEnergy(T2);
  particleGun->SetParticleMomentumDirection(get_2nd_electron_direction(direction0, T1, T2));
  particleGun->GeneratePrimaryVertex(anEvent);
}

void EXOPrimaryGeneratorAction::generate_bb_spectralIndex( G4Event *anEvent , G4int spectral_index)
{
  // Generate a double beta decay having the specified spectral index
  // This is a very general feature of double beta decay
  // bb0n has index 0, bb2n has index 5, Majoron modes have 2, 3, 5, 7
  //
  // K=T1+T2 (sum of kinetic energies T1 and T2; 0 < K < To)
  // D=T1-T2 (their difference; -K < D < +K; D=0 => T1=T2=To/2)
  //
  // Algorithm to generate the energy of the two electrons for the bb decay:
  // 1. Randomly generate K that satisfies the dN/dK spectrum.
  // 2. Making use of the symmetry between T1 and T2,
  //    randomly generate D that satisfies the phase space constraint
  //
  //    p1*E1*FermiFunction(Z,E1)*p2*E2*FermiFunction(Z,E2)*pow((T0-K), spectral_index)
  //
  // 3. With this set of K and D, calculate T1 and T2
  //    T1 = (K+D)/2
  //    T2 = (K-D)/2
  //
  // This set of K & D (or T1, T2) guarantees to satisfy both
  // the sum electron spectrum dN/dK and the single electron spectra dN/dTo.
  // (cf. Physics of Massive Neutrinos by Felix Boehm & Petr Vogel,pp.145-151.)
  //
  // To compare the spectra see PhysLettB vol 211, 1988, Mohapatra and Takasugi
  //
  // To generate an arbitrary probabilistic distribution function f(x),
  // use the Acceptance-Rejection Monte Carlo Method.
  // (cf. Frank Porter's lecture on the Acceptance-Rejection Method.)
  //
  // http://www.hep.caltech.edu/~fcp/statistics/sluo00/sluolec5.pdf
  //
  // Max-------------------
  //    |                 |
  //    |    *** f(x)     |
  //    |   *   **        |
  //    |  *      **      |
  //    | *         **    |
  //    |*            **  |
  //  0 *---------------***
  //    a                 b
  //

  particleGun->SetParticleDefinition(particle_used);

  // Choose electron energies

  G4double T0 = q_value/ELECTRON_MASS;
  G4double K = T0;
  G4double D = T0;

  G4double D_spectral_max = 0;

  G4double y = 0;

  if (spectral_index > 0) {

    do {
      // Acceptance-Rejection for the sum electron spectrum dN/dK
      K = T0*G4UniformRand();
      y = K_spectral_max*G4UniformRand();
    } while(y > sum_spectrum(K, spectral_index, D_spectral_max));
  } else {
    K = T0;
    D_spectral_max = D_spectrum_max(K, spectral_index);
  }
  
  do {
    // Acceptance-Rejection condition for the single electron spectrum dN/T0
    D = K*(2*G4UniformRand()-1.0);   // -K < D < +K
    y = D_spectral_max * G4UniformRand();
  } while (y > D_spectrum(K, D, spectral_index));

  G4double T1 = (0.5*(K + D))*ELECTRON_MASS;
  G4double T2 = (0.5*(K - D))*ELECTRON_MASS;

  generate_bb_kineticEnergies(anEvent, T1, T2);
}


void EXOPrimaryGeneratorAction::generate_bb2n_iachello( G4Event *anEvent )
{
  // Generate a double beta decay according to the iachello spectrum
  // see the documentation for generate_bb_spectralIndex to see how
  // the spectrum is sampled

  particleGun->SetParticleDefinition(particle_used);

  // Choose electron energies
  
  G4double T0 = q_value/ELECTRON_MASS;
  G4double K = T0;
  G4double D = T0;

  G4double D_spectral_max = 0;
  G4double y = 0;

  do {
    // Acceptance-Rejection for the sum electron spectrum dN/dK
    K = T0*G4UniformRand();
    y = K_spectral_max_iachello*G4UniformRand();
  } while(y > sum_spectrum_iachello(K, D_spectral_max));

  do {
    // Acceptance-Rejection condition for the single electron spectrum dN/T0
    D = K*(2*G4UniformRand()-1.0);   // -K < D < +K
    y = D_spectral_max * G4UniformRand();
  } while (y > D_spectrum_iachello(K, D));

    G4double T1 = (0.5*(K + D))*ELECTRON_MASS;
    G4double T2 = (0.5*(K - D))*ELECTRON_MASS;

    generate_bb_kineticEnergies(anEvent, T1, T2);
}

void EXOPrimaryGeneratorAction::generate_bb0n_NME( G4Event *anEvent )
{
  // Generate a double beta decay according to the NME spectrum
  // see the documentation for generate_bb_spectralIndex to see how
  // the spectrum is sampled

  particleGun->SetParticleDefinition(particle_used);

  // Choose electron energies
  
  G4double T0 = q_value/ELECTRON_MASS;
  G4double K = T0;
  G4double D = T0;

  G4double D_spectral_max = 0;
  G4double y = 0;

  do {
    // Acceptance-Rejection for the sum electron spectrum dN/dK
    K = T0*G4UniformRand();
    y = K_spectral_max_NME*G4UniformRand();
  } while(y > sum_spectrum_NME(K, D_spectral_max));

  do {
    // Acceptance-Rejection condition for the single electron spectrum dN/T0
    D = K*(2*G4UniformRand()-1.0);   // -K < D < +K
    y = D_spectral_max * G4UniformRand();
  } while (y > D_spectrum_NME(K, D));

    G4double T1 = (0.5*(K + D))*ELECTRON_MASS;
    G4double T2 = (0.5*(K - D))*ELECTRON_MASS;

    generate_bb_kineticEnergies(anEvent, T1, T2);
}

G4double EXOPrimaryGeneratorAction::sum_spectrum_iachello( G4double K, 
							   G4double &D_spectral_max )
{
  
  // Get the value of the Iachello sum spectrum for our value of K
  
  G4double iachello_full_energy = N_iachello*iachello_bin_width/ELECTRON_MASS;
  G4double half_bin_width = iachello_bin_width/(2.0*ELECTRON_MASS); 
  G4double ratio = (K-half_bin_width)/iachello_full_energy;
  G4int i = (int)(N_iachello*ratio);

  if ( i < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::bb2n_sum_spectrum_iachello:: i should not be less than zero!" << G4endl;
    i = 0;
  }
  if ( i >= N_iachello ) i = N_iachello - 1;

  double value = iachello_sum_spectrum_array[i]; 

  // Now look in the two-dimensional array to determine the maximum in the electron difference
  // spectrum for this particular value of K

  G4double maximum = -1.0;

  for ( int m = 0; m <= i; m++ ) {
    if ( iachello_differential_spectrum_array[i-m][m] > maximum ) {
      maximum = iachello_differential_spectrum_array[i-m][m];
    }
  }

  D_spectral_max = maximum;
 
  return value;

}

G4double EXOPrimaryGeneratorAction::D_spectrum_iachello(G4double K, G4double D)
{

  G4double T1, T2;
  
  T1 = 0.5*(K+D);
  T2 = 0.5*(K-D);

  G4double half_bin_width = iachello_bin_width/(2.0*ELECTRON_MASS); 

  // Since the very first entry in the iachello_differential_spectrum_array is 
  // for electron energies between 5 and 15 keV. We will output 0 if one of
  // the electron energy is less than 5 keV.

  if(T1 < half_bin_width || T2 < half_bin_width){    
    return 0.0;}
{
  G4double iachello_full_energy = N_iachello*iachello_bin_width/ELECTRON_MASS;
  G4double ratio1 = (T1-half_bin_width)/iachello_full_energy;
  G4int i1 = (int)(N_iachello*ratio1);

  G4double ratio2 = (T2-half_bin_width)/iachello_full_energy;
  G4int i2 = (int)(N_iachello*ratio2);

  if ( i1 < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::D_spectrum_iachello: i1 is less than zero" << G4endl;
    i1 = 0;
  }
  if ( i2 < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::D_spectrum_iachello: i2 is less than zero" << G4endl;
    i2 = 0;
  }
  if ( i1 > N_iachello - 1 ) {
    i1 = N_iachello - 1;
  }
  if ( i2 > N_iachello - 1 ) {
    i2 = N_iachello - 1;
  }

  return iachello_differential_spectrum_array[i1][i2];

  }

}


G4double EXOPrimaryGeneratorAction::sum_spectrum_NME( G4double K, 
							   G4double &D_spectral_max )
{
  
  // Get the value of the NME sum spectrum for our value of K
  
  G4double NME_full_energy = N_NME*NME_bin_width/ELECTRON_MASS;
  G4double half_bin_width = NME_bin_width/(2.0*ELECTRON_MASS); 
  G4double ratio = (K-half_bin_width)/NME_full_energy;
  G4int i = (int)(N_NME*ratio);

  if ( i < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::bb0n_sum_spectrum_NME:: i should not be less than zero!" << G4endl;
    i = 0;
  }
  if ( i >= N_NME ) i = N_NME - 1;

  double value = NME_sum_spectrum_array[i]; 

  // Now look in the two-dimensional array to determine the maximum in the electron difference
  // spectrum for this particular value of K

  G4double maximum = -1.0;

  for ( int m = 0; m <= i; m++ ) {
    if ( NME_differential_spectrum_array[i-m][m] > maximum ) {
      maximum = NME_differential_spectrum_array[i-m][m];
    }
  }

  D_spectral_max = maximum;
 
  return value;

}

G4double EXOPrimaryGeneratorAction::D_spectrum_NME(G4double K, G4double D)
{

  G4double T1, T2;
  
  T1 = 0.5*(K+D);
  T2 = 0.5*(K-D);

  G4double half_bin_width = NME_bin_width/(2.0*ELECTRON_MASS); 

  // Since the very first entry in the NME_differential_spectrum_array is 
  // for electron energies between 5 and 15 keV. We will output 0 if one of
  // the electron energy is less than 5 keV.

  if(T1 < half_bin_width || T2 < half_bin_width){    
    return 0.0;}
{
  G4double NME_full_energy = N_NME*NME_bin_width/ELECTRON_MASS;
  G4double ratio1 = (T1-half_bin_width)/NME_full_energy;
  G4int i1 = (int)(N_NME*ratio1);

  G4double ratio2 = (T2-half_bin_width)/NME_full_energy;
  G4int i2 = (int)(N_NME*ratio2);

  if ( i1 < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::D_spectrum_NME: i1 is less than zero" << G4endl;
    i1 = 0;
  }
  if ( i2 < 0 ) {
    G4cout << "EXOPrimaryGeneratorAction::D_spectrum_NME: i2 is less than zero" << G4endl;
    i2 = 0;
  }
  if ( i1 > N_NME - 1 ) {
    i1 = N_NME - 1;
  }
  if ( i2 > N_NME - 1 ) {
    i2 = N_NME - 1;
  }

  return NME_differential_spectrum_array[i1][i2];

  }

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// The following methods are used by multiple generators
// As of Sept, 2010, The functions below have been added to the EXOPrimaryGeneratorAction class

G4double EXOPrimaryGeneratorAction::SimpsonsRule(G4double x0, G4double xn, G4int N, G4double f[])
{
  //
  // Simpson's Rule:
  // Partition [a, b] into intervals all of the same width.
  // We must use an even number of intervals, so n will be even.
  // xk = a + kx = a + k (b-a)/n
  //
  // integral(x0, xn)
  // = (xn-x0)/3n *[f(x0) + 4f(x1) + 2f(x2) + 4f(x3) +
  //                         ... + 2f(xn-2) + 4f(xn-1) + f(xn)]
  //
  // Sum all the odd terms and then multiply by 4.
  // Sum all the even terms and then multiply by 2.
  //

    if ( N%2 != 0 ) G4cout << "SimpsonsRule: N is not even";

    G4double sum_odd=0.;
    for (G4int i=0; i<N/2; i++){
            sum_odd += f[2*i+1];
    }
    sum_odd = 4.0*sum_odd;

    G4double sum_even=0.;
    for (G4int j=1; j<N/2; j++){
            sum_even += f[2*j];
    }
    sum_even = 2.0*sum_even;

    return (xn-x0)*(f[0]+sum_odd+sum_even+f[N])/(3.0*N);
}


void EXOPrimaryGeneratorAction::setup_FF_GraphValues(G4String calcRadius)
{

  G4cout << "In setup_iachello_FF" << G4endl;
  G4cout << "Using radius " << calcRadius << " for calculation." << G4endl;
#ifdef HAVE_ROOT
   TFile *f = new TFile(EXOMiscUtil::SearchForFile("data/iachello/FF_Z56_Calculation_by_PVogel.root").c_str(), "READ" );
   TGraph *gr = NULL;

   if(calcRadius == "r0")
     {
       gr = (TGraph*)f->Get("gr0");
     }
   if(calcRadius == "rR")
     {
       gr = (TGraph*)f->Get("grR");
     }



#endif /* HAVE_ROOT */
    nFFGraphPoints = gr->GetN();

    //  Puts graph points into arrays. 
    GraphFFX = gr->GetX();
    GraphFFY = gr->GetY();
    
    f->Close();

}

G4double EXOPrimaryGeneratorAction::fermi_function_GraphValues(G4double KE )
{
  // P. Vogel gave us a table of values from his calculation of the Fermi
  // function. Two calculations were performed,  r = 0 and r = R  (R being 
  // the radius of the nucleus) for Z=56.  
  // You can choose whether to use r=0 or r=R with a talk-to command.
  // We will use linear interpolation between values.
  // -TJ, Apr 2014


  //G4cout << "Using FF values from graph " << G4endl;

  G4double total_energy, W, p, px1, px2, py1, py2, m, b;
  G4double result = 0;
  
  total_energy = KE + ELECTRON_MASS;
  W = total_energy/ELECTRON_MASS;
  p = sqrt(W*W-1);
  //We need to avoid p=0, if KE == 0 (D == +-K). Set p to a low value.
  if ( KE == 0 ){ p = 0.0009800; }
  
  //G4cout << "p = "<< p << G4endl;

  G4double * fx = GetFFGraphX();
  G4double * fy = GetFFGraphY();
  G4int n = GetnFFGraphPoints();
  G4double KinEn = KE;

  // If KE is < 0.01 MeV, just take the value of FF for 0.01 MeV
  if (KinEn <= fx[0] || KinEn == 0)
    {
      result = fy[0];
    } 

  for (G4int i = 0; i < n; i++)
    {
      //G4cout <<"i = "<<i<<"fx[i]= "<<fx[i]<<" fx[i+1]= "<<fx[i+1]<<G4endl;
      if (fx[i] <= KinEn && KinEn < fx[i+1])
	{
	  px1 = fx[i];
	  px2 = fx[i+1];
	  py1 = fy[i];
	  py2 = fy[i+1];

	  //G4cout << px1 << " < " << KinEn << " < " << px2 << G4endl;
	  //G4cout << "px1 px2 py1 py2 "<<px1<<" "<<px2<<" "<<py1<<" "<<py2<<G4endl;

	  m = (py2 - py1)/(px2 - px1);
	  b = py1 - m*px1;
	  result = m*KinEn + b; 
	  break;
	}

    }

  result = result * W/p;

  //G4cout << "KE = " << KE << " result = " << result << G4endl;
  //G4cout << "fx[0], fx[2] = " << fx[0] << " " << fx[2] << "\n";
  //G4cout << "fy[0], fy[2] = " << fy[0] << " " << fy[1] << "\n";
  //G4cout << "n = " << n << G4endl;
  //G4cout << "total_energy = " << total_energy << "\n";
  //G4cout << "W = " << W << "\n";
  return result;
}

G4double EXOPrimaryGeneratorAction::fermi_function( G4int Z, G4double KE )
{
  // From Schenter+Vogel in Nucl.Sci.Eng,83,393(1983)
  // W is full energy in electron mass units

  G4double total_energy, W, Z0, a, b, p, v, x, y, yy, result;

  G4double alpha = 7.2974e-3; // fine structure constant

  total_energy = KE + ELECTRON_MASS;
  W = total_energy/ELECTRON_MASS;

  Z0 = Z + ff_factor*2;

  if ( W<=1 ) {
    W = 1e-4+1;
  }
    a = -0.811+4.46e-2*Z0+1.08e-4*Z0*Z0;
    b = 0.673-1.82e-2*Z0+6.38e-5*Z0*Z0;
    
  if ( W>2.2 ) {
    a = -8.46e-2+2.48e-2*Z0+2.37e-4*Z0*Z0;
    b = 1.15e-2+3.58e-4*Z0-6.17e-5*Z0*Z0;
  }

  x = sqrt(W-1);
  p = sqrt(W*W-1);

  result = exp(a+b*x)*W/p;

  if (p<=0) result = 1; // just to be consistent with the old Fermi Function code

  if(ff_factor == -1){ // for double positron decays
  v=p/W;
  y=2*pi*Z0*alpha/v;
  yy=1./exp(y);
  result=result*yy;
  }

  return result;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double EXOPrimaryGeneratorAction::muon_angular_distribution(G4double theta)
{
  // based on dI/d(theta) = sin(theta)*(3.818e-7)*pow(cos(theta), 1.53)*exp(1585*-8e-4*(1/cos(theta)-1)) from S. Miyake
  // (but ignoring the overall normalization factor)

  // Compute the logarithm first, otherwise I think we run into
  // precision issues

  // The cos(theta) comes from the fact that we later pick a target 
  // for the muon from a horizontal disk, so the flux through it will
  // be cos(theta) less.
  G4double log_intensity = (-(8e-4*mwe_depth)/cos(theta)+
			    1.53*log(cos(theta))+(8e-4*mwe_depth));
  return cos(theta)*sin(theta)*exp(log_intensity);
}

void EXOPrimaryGeneratorAction::generate_muon( G4Event *anEvent )
{
    // Generate muons. The angular distribution is described
    // by S. Miyake in "Rapporteur Paper on Muons and Neutrinos" from the
    // proceedings of the 13th Interncational Cosmic Ray Conference
    // in 1973. The energy distribution is decribed by Gaisser in
    // "Cosmic Rays and Particle Physics", published 1990, and the
    // range is from 1 to 500 GeV
    //
    // After the incident angles are chosen, the muon is spawned 3 m
    // above the TPC, aimed at a random point in a disk below the
    // TPC, with those incident angles and energy. Only muons that
    // actually hit the TPC are used. This should give the correct
    // distribution out to about 80 degrees.
    //
    // Assumes R(mu+/mu-) is 1.25, which is what experimental data shows
    // below about 100 GeV.
    //
    // This does not include the contribution of neutrino-induced muons. 
    //
    // To generate an arbitrary probabilistic distribution function f(x), 
    // use the Acceptance-Rejection Monte Carlo Method.
    // (cf. Frank Porter's lecture on the Acceptance-Rejection Method.)
    //
    // http://www.hep.caltech.edu/~fcp/statistics/sluo00/sluolec5.pdf
    //   
    // Max------------------- 
    //    |                 |
    //    |    *** f(x)     |
    //    |   *   **        |
    //    |  *      **      |
    //    | *         **    |
    //    |*            **  |
    //  0 *---------------*** 
    //    a                 b
    // 
    // Generate randomly a point within the box bounded by (a->b, 0->Max). 
    // If the point falls under f(x), accept it, otherwise, reject.
    // If it is rejected, repeat the process until a point falls under f(x).

  // TYPE ------------------------------------------------------

  G4double plus_ratio = 1.25;
  if (G4UniformRand() < plus_ratio/(plus_ratio + 1)) {
    particleGun->SetParticleDefinition(aMuonPlus);
  } else {
    particleGun->SetParticleDefinition(aMuonMinus);
  }

  // Acceptance-Rejection loop
  G4double y_ang, intensity_ang;

  // DIRECTION ------------------------------------------
  // First figure out the maximum of the angular distribution
  if (muon_ang_max <= 0) {
    G4int N = 1000000;
    for (G4int i = 0; i < N; ++i) {
      G4double theta = i*(pi/2)/N;
      G4double a = muon_angular_distribution(theta);
      if (a > muon_ang_max) {
	muon_ang_max = a;
      }
    }
  }

  // Pick incident angles

  G4double theta = 0;
  do  {
    theta = pi/2*G4UniformRand();
    y_ang = muon_ang_max*G4UniformRand();
    intensity_ang = muon_angular_distribution(theta);
  } while (y_ang > intensity_ang);

  G4double phi = 6.283185*(G4UniformRand()-0.5);

  // ENERGY ----------------------------------------------------

  // Pick energy

  G4double E = 0;
  G4double y_en = 1;
  G4double intensity_en = 0;
  do {

    // generate between 1 and 500 GeV
    //G4double log_energy = G4UniformRand()*3;
    //E = pow(10, log_energy)
    E = 1+G4UniformRand()*499;

    // Let's assume standard rock
    // See Groom et al. "Muon Stopping Power and Range Tables"
    G4double eps = 693; // GeV
    G4double b = 1.0/(2.5e3); // 1e-2 g/cm^2
    
    // What is the average energy at the surface for muons that get
    // down here with that energy?
    G4double E_0 = (E + eps)*exp(b*mwe_depth/cos(theta)) - eps;

    // From Gaisser
    G4double surface_flux = (0.14*pow(E_0, -2.7)
			     *(1/(1+1.1*E_0*cos(theta)/115)
			       + 0.054/(1+1.1*E_0*cos(theta)/850)));
    
    intensity_en = surface_flux*exp(b*mwe_depth/cos(theta));

    // intensity maximum for E = 0
    G4double E_max = eps*(exp(b*mwe_depth/cos(theta)) - 1);
    y_en = G4UniformRand()*(exp(b*mwe_depth/cos(theta))*0.14*pow(E_max, -2.7)
			 *(1/(1+1.1*E_max*cos(theta)/115)
			   + 0.054/(1+1.1*E_max*cos(theta)/850)));
    
     // Test whether randomly generated point is under histogram
   } while (y_en > intensity_en);
  
  particleGun->SetParticleEnergy(E*GeV);

  // TARGET ---------------------------------------------
  // Pick a target in a circle under the detector
  // Throw out targets that don't have muons pass
  // through the TPC to save simulation time. Not
  // everyone may want this feature!
  G4double R = 300;
  G4double x1 = R + 1;
  G4double z1 = R + 1;
  G4double y1 = -(22.875-0.137);

  G4bool hit_TPC = false;
  G4double dY = 0.01;

  while (!hit_TPC) {

    x1 = R + 1;
    z1 = R + 1;

    while (x1*x1 + z1*z1 > R*R) {
      x1 = 2*R*(G4UniformRand()-0.5);
      z1 = 2*R*(G4UniformRand()-0.5);
    }

    for (G4double y = y1; y < -y1; y += dY) {
      G4double r = (y-y1)/cos(theta);
      G4double z = z1 + r*sin(theta)*cos(phi);
      if (fabs(z) > 20.44065) {
	continue;
      }
      G4double x = x1 - r*sin(theta)*sin(phi);
      if (x*x + y*y <= y1*y1) {
	hit_TPC = true;
	break;
      }
    }
  }

  
  // POSITION -------------------------------------------
  // Pick a start position based on angle and target
  // Start it 3 meters above the TPC. That should be sufficient.
  // The minus sign for x is so that N is phi=0, E is phi=pi/2, etc.
  G4double ell = 300/cos(theta);
  G4double x0 = x1 - 2*ell*sin(theta)*sin(phi);
  G4double y0 = y1 + ell*cos(theta);
  G4double z0 = z1 + 2*ell*sin(theta)*cos(phi);

  particleGun->SetParticlePosition(G4ThreeVector(x0*cm, y0*cm, z0*cm));
  
  // MOMENTUM -------------------------------------------
  
  G4double magnitude = sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)+(z0-z1)*(z0-z1));
  G4float ux = (x1-x0)/magnitude;
  G4float uy = (y1-y0)/magnitude;
  G4float uz = (z1-z0)/magnitude;

  particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));

  // GENERATE PARTICLE ---------------------------------

  particleGun->GeneratePrimaryVertex(anEvent);

}

#ifdef HAVE_ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TEntryList.h>
#include "EXOUtilities/EXOMCTrackInfo.hh"
#endif
void EXOPrimaryGeneratorAction::ReopenTrackSource(bool lazy)
{
#ifdef HAVE_ROOT
  if (fTree) {
    // clear old
    fIEntry = -1;
    fStore = 0;
    fList = 0;
    //delete fTree->GetCurrentFile();
    fTree = 0;
    fTreeFailed = false;
  }
  if (lazy) return; // do not hurry
  if (!fTree) {
    TDirectory *old = gDirectory;
    TObject *obj = 0;

    // lets find file or whatever
    TDirectory *file = 0;
    if (!fTrackSource.empty()) {
      obj = gROOT->FindObject(fTrackSource.c_str());
      //G4cout << "obj " << obj <<" "<< (obj?obj->IsA()->GetName():"(null)") << G4endl;
      if (obj) {
	if (obj->IsA()->InheritsFrom(TFile::Class())) { // found it 
	  file = dynamic_cast<TDirectory*>(obj);
	} else {
	  G4cerr << __func__ << ": found your source '" << fTrackSource 
		 << "', but it is of bad type '" << obj->IsA()->GetName() <<"'"<< G4endl;
	  fTreeFailed = true;
	  return;
	}
      } else {
	file = TFile::Open(fTrackSource.c_str());
	// one should close file at the end, but we don't care
	if (!file || file->IsZombie()) {
	  G4cerr << __func__ << ": can't open your source '" << fTrackSource <<"'"<< G4endl;
	  fTreeFailed = true;
	  delete file;
	  return;
	}
      }
      // now we found file
    } else {
      file = gDirectory; // use current by default
    }
    //G4cout << "file " << file <<" "<< file->IsA()->GetName() << G4endl;

    // lets find our tree
    TTree *tree = 0;
    obj = file->Get(fTreeName.c_str());
    if (obj && obj->IsA()->InheritsFrom(TTree::Class())) { // found it
      tree = dynamic_cast<TTree*>(obj);
    } else {
      G4cerr << __func__ << ": can't find tree '" << fTreeName << "' in the source" << G4endl;
      fTreeFailed = true;
      return;
    }
    // now we have tree

    // lets find or make selection
    if (!fTrackSelect.empty()) {
      // first check file
      obj = file->FindObject(fTrackSelect.c_str());
      //G4cout << "obj " << obj <<" "<< (obj?obj->IsA()->GetName():"(null)") << G4endl;
      if (obj && obj->IsA()->InheritsFrom("TEntryList")) { // found it 
	fList = dynamic_cast<TEntryList*>(obj);
      } else {
	// also check current directory
	obj = old->FindObject(fTrackSelect.c_str());
	//G4cout << "obj2 " << obj <<" "<< (obj?obj->IsA()->GetName():"(null)") << G4endl;
	if (obj && obj->IsA()->InheritsFrom("TEntryList")) { // found it 
	  fList = dynamic_cast<TEntryList*>(obj);
	} else {
	  // try to make
	  tree->Draw(">> EXOPGA_tmp_list",fTrackSelect.c_str(),"entrylist");
	  // actually one should verify that there is no such name already
	  fList = dynamic_cast<TEntryList*>(gDirectory->Get("EXOPGA_tmp_list"));
	  if (!fList) {
	    G4cerr << __func__ << ": failed to create selection" << G4endl;
	    fTreeFailed = true;
	    return;
	  }
	}
      }
    }

    // ok, final checks
    int nentries = 0;
    fEventID = -1;
    fEventNum = 0;
    if (fList) {
      tree->SetEntryList(fList);
      nentries = fList->GetN();
      fIEntry = fList->GetEntry(0);
    } else {
      nentries = tree->GetEntries();
      fIEntry = 0;
    }
    if (nentries) {
      tree->SetBranchAddress("MCTrackBranch",&fStore);
      fTree = tree;
      fTreeFailed = false;
      G4cout << "EXOPrimaryGeneratorAction: trackinfo: found "<< nentries << " tracks in your source" << G4endl;
    } else {
      G4cerr << "EXOPrimaryGeneratorAction: trackinfo: your source is empty"<< G4endl;
      fTreeFailed = true;
      return;
    }
    old->cd();
  }
#else
  // maybe someone volunteers to code read from text files?
  G4cerr << __func__ << ": not implemented, no particles will be generated" << G4endl;
#endif /* HAVE_ROOT */
}
void EXOPrimaryGeneratorAction::generate_trackinfo( G4Event *anEvent )
{
#ifdef HAVE_ROOT
  if (fTreeFailed) return;
  if (!fTree) ReopenTrackSource(false);
  if (fTreeFailed) return;
  // ok, now move on
  bool done = false; // done one event
  // main loop
  do {
    if (fEventID>=0) {
      // let's generate
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition *particle = 0;
      bool is_ion = fStore->fParticle.find('[')!=std::string::npos && !islower(fStore->fParticle[0]); // definitely ion
      if (!is_ion) 
        particle = particleTable->FindParticle(fStore->fParticle); // maybe regular particle
      if (is_ion || particle==0) { // ... maybe not
	G4IonTable *ionTable = particleTable->GetIonTable();
	std::string::size_type i = 0, j = 0;
	int L = 0, A = 0; double E = 0;
	// corresponds to G4IonTable::GetIonName()
	while (fStore->fParticle[i]=='L') i++; L = i;
	i = fStore->fParticle.find_first_of("0123456789",i);
	if (i!=std::string::npos) A = strtol(fStore->fParticle.c_str()+i,0,0);
	j = fStore->fParticle.find('[',i+1);
	if (j!=std::string::npos) E = strtof(fStore->fParticle.c_str()+j+1,0);
	for (int Z=1;Z<=ionTable->GetNumberOfElements();Z++) 
	  if (ionTable->GetIonName(Z,A,L,E)==fStore->fParticle.c_str()) {
	    particle = ionTable->GetIon(Z,A,L,E); break; }
      }
      if (particle) {
        particleGun->SetParticleDefinition(particle);
        if (fModeLast) {
          particleGun->SetParticleEnergy(fStore->fLastEnergy*keV);
          particleGun->SetParticlePosition(G4ThreeVector(fStore->fLastX*mm, fStore->fLastY*mm, fStore->fLastZ*mm));
          particleGun->SetParticleMomentumDirection(G4ThreeVector(fStore->fLastDirectionX, fStore->fLastDirectionY, fStore->fLastDirectionZ));
          particleGun->SetParticleTime(fStore->fGlobalTime*second);
        } else {
          particleGun->SetParticleEnergy(fStore->fVertexEnergy*keV);
          particleGun->SetParticlePosition(G4ThreeVector(fStore->fVertexX*mm, fStore->fVertexY*mm, fStore->fVertexZ*mm));
          particleGun->SetParticleMomentumDirection(G4ThreeVector(fStore->fVertexDirectionX, fStore->fVertexDirectionY, fStore->fVertexDirectionZ));
          particleGun->SetParticleTime((fStore->fGlobalTime-fStore->fLocalTime)*second);
        }
        //particleGun->Dump();
        particleGun->GeneratePrimaryVertex(anEvent);
      } else {
        G4cerr << __func__ << ": can't find required particle '" << fStore->fParticle << "': skipping" << G4endl;
      }
    }
    // load entry
    if (fTreeFailed) { // this is stop from lookup for new entry
      G4cerr << "EXOPrimaryGeneratorAction: trackinfo: reached maximum number of loops over source, stop generating new tracks" << G4endl;
      break;
    }
    int read = fTree->GetEntry(fIEntry);
    if        (read<-1) {
    } else if (read==-1) {
      G4cerr << __func__ << ": GetEntry() failed on entry #" << fIEntry << G4endl;
      fTreeFailed = true;
      return;
    } else if (read==0) {
      G4cerr << __func__ << ": GetEntry() can't find entry #" << fIEntry << G4endl;
      fEventID = -1;
    } else if (read>0) {
      // we have it
      //G4cout << __func__ <<" load " << fIEntry <<" "<< fStore->fEventID <<" "<< fStore->fTrackID << G4endl;
      // check EventID
      int id = fStore->fEventID;
      if (fEventID!=id) { // reached next event
	if (fEventID>=0) done = true; // done unless very first iteration
	fEventID = id; // cache ID
	if (fEventNum>=0) fEventNum++; // also account
      }
    }
    // where is new entry
    if (fList) {
      if (fIEntry>=0) fIEntry = fList->Next();
      else            fIEntry = fList->GetEntry(0);
      if (fIEntry<0) { // the end of selection
	if (fEventNum>=0) {
	  G4cout << "EXOPrimaryGeneratorAction: trackinfo: reached the end of your source"
		 << " after " << fEventNum << " events" << G4endl;
	  fEventNum = 0;
	}
	if      (fLoopMode==-1) fIEntry = fList->GetEntry(0); // reset forever
	else if (fLoopMode>0) { fIEntry = fList->GetEntry(0); fLoopMode--; } // reset fLoopMode times
	else                  { fTreeFailed = true; } // the end, but proceed to generation
      }
    } else {
      if (fIEntry<fTree->GetEntries()) fIEntry++;
      else {
	if      (fLoopMode==-1) fIEntry = 0; // reset forever
	else if (fLoopMode>0) { fIEntry = 0; fLoopMode--; } // reset fLoopMode times
	else                  { fTreeFailed = true; } // the end, but proceed to generation
      }
    }
    if (anEvent->GetNumberOfPrimaryVertex()>100) {
      G4cerr << "EXOPrimaryGeneratorAction: trackinfo: something really nasty goes on"
	     << " since i reached "<< anEvent->GetNumberOfPrimaryVertex() 
             << " of primary tracks for an event. Breaking down processing" << G4endl;
      fTreeFailed = true;
      break;
    }
  } while (!done);
#else
  // maybe someone volunteers to code read from text files?
#endif /* HAVE_ROOT */
}

void EXOPrimaryGeneratorAction::SetEventSourceName(const std::string& v)
{
  if (fFile) {
    fFile->close();
    delete fFile;
  }
  fEventSource = v;
  fFile = new ifstream(v.c_str());
  if (!fFile->good()) {
    G4cerr << "EXOPrimaryGeneratorAction: dicebox: can't open source file '" << v << "'" << G4endl;
    fFile = 0;
  }
}

void EXOPrimaryGeneratorAction::generate_dicebox( G4Event *anEvent )
{
  if (fFile==0) return;
    // neutron capture on Xe136
    int n = 1;//floor(1-log(G4UniformRand())/1.2354);
    for (int i=0;i<n;i++) {
      particleGun->SetParticlePosition(get_random_position(1));
      double time = -3.1e-3+sqrt(9.61e-6-log(G4UniformRand())/86555);
      particleGun->SetParticleTime(time*second);

    double spin; int parity; unsigned nsteps; double e0;
    std::vector<double> level; std::vector<int> type;

    // read event from file
    std::string line;
    std::stringstream ss;
    double e; int l;
    getline(*fFile,line); ss.str(line); ss.clear();
    ss >> spin >> parity >> nsteps >> e0;
    getline(*fFile,line); ss.str(line); ss.clear();
    while(!ss.eof()) { ss >> e; level.push_back(e); }
    getline(*fFile,line); ss.str(line); ss.clear();
    while(!ss.eof()) { ss >> l; type.push_back(l); }
    fEventNum++;
    fFile->peek(); // required to catch EOF
    if (fFile->eof()) {
      G4cout << "EXOPrimaryGeneratorAction: dicebox: reached the end of your source"
             << " after " << fEventNum << " events" << G4endl;
      fEventNum = 0;
      fFile->clear();
      fFile->seekg(0);
    }

      // now make particles
      if (nsteps==level.size() && nsteps==type.size()) {
        for (unsigned j=0;j<nsteps;j++) {
          e = e0 - level[j]; e0 = level[j];
//          G4cout << j <<" "<< e <<" "<< type[j] << G4endl;
          switch (type[j]) {
          case 0:
            // just gamma
            particleGun->SetParticleDefinition(aGamma);
            particleGun->SetParticleEnergy(e*MeV);
            particleGun->SetParticleMomentumDirection(get_random_direction());
            particleGun->GeneratePrimaryVertex(anEvent);
            break;
          case 1:
            // internal conversion
            particleGun->SetParticleDefinition(aBeta);
            particleGun->SetParticleEnergy(e*MeV - 34.5*keV);
            particleGun->SetParticleMomentumDirection(get_random_direction());
            particleGun->GeneratePrimaryVertex(anEvent);
            particleGun->SetParticleDefinition(aGamma);
            particleGun->SetParticleEnergy(34.5*keV);
            particleGun->SetParticleMomentumDirection(get_random_direction());
            particleGun->GeneratePrimaryVertex(anEvent);
            break;
          default:
            ;
          }
        }
      } else {
      }
    }
}

void EXOPrimaryGeneratorAction::generate_nCaptureXe134( G4Event *anEvent )
{
  // Unlike the more sophisticated 136Xe capture generator, here we do not have
  // the luxury of good data.  As a substitute, we will use a so-so study
  // by the FLUKA gang for the ALICE TRT.  Read about it at:
  // https://www.oecd-nea.org/science/pubs/2004/3828-SATIF-6.pdf
  //
  // This model has been implemented in FLUKA, and so we just simply use the
  // capture cascades which FLUKA simulates.  We know, from looking at the 136Xe
  // capture cascades, that this very challenging assignment task was not done.
  // flawlesssly.  There are gammas assigned to thermal capture on 136Xe which we know
  // do not occur.  However, total Q-values are obeyed, and we are undoubtedly in the
  // "better than nothing" regime here.  I wouldn't try to measure 134Xe capture
  // with this, but as a small background PDF, it shouldn't be too bad.
  //
  // We will select one of 14 possible decay cascades, accounting for nearly all
  // decay cascades FLUKA showed, with under 0.02% of cascades not following one
  // of these patterns. That extra 0.02% is given to the most common n=4 cascade

  // of course, we will use gammas
  particleGun->SetParticleDefinition(aGamma);


  //******** Set Vertex of Capture + Excited State Decay *************//
  particleGun->SetParticlePosition(get_random_position(1));

  G4double decay_gammas[4];
  G4int n_gammas = 0;
  // Choose a random number to determine the decay
  G4double decaynum = G4UniformRand();

  // Check candidate decays (values in keV)
  if (decaynum < 0.631097) {
    decay_gammas[0] =  4230.6; decay_gammas[1] = 2151.4;
    decay_gammas[2] = 0; decay_gammas[3] = 0; n_gammas = 2;}
  else if (decaynum < 0.656151) {
    decay_gammas[0] = 3904.3; decay_gammas[1] = 2477.7;
    decay_gammas[2] = 0; decay_gammas[3] = 0; n_gammas = 2;}
  else if (decaynum < 0.672370) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 2255.4;
    decay_gammas[2] = 0; decay_gammas[3] = 0; n_gammas = 2;}
  else if (decaynum < 0.894955) {
    decay_gammas[0] = 3904.3; decay_gammas[1] = 2189.3;
    decay_gammas[2] = 288.4; decay_gammas[3] = 0; n_gammas = 3;}
  else if (decaynum < 0.986309) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 1123.9;
    decay_gammas[2] = 1131.4; decay_gammas[3] = 0; n_gammas = 3;}
  else if (decaynum < 0.990367) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 995.1;
    decay_gammas[2] = 1260.4; decay_gammas[3] = 0; n_gammas = 3;}
  else if (decaynum < 0.994292) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 797.9;
    decay_gammas[2] = 1457.6; decay_gammas[3] = 0; n_gammas = 3;}
  else if (decaynum < 0.995290) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 807.1;
    decay_gammas[2] = 1448.3; decay_gammas[3] = 0; n_gammas = 3;}
  else if (decaynum < 0.998763) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 690.2;
    decay_gammas[2] = 1038.7; decay_gammas[3] = 526.5; n_gammas = 4;}
  else if (decaynum < 0.999188) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 797.9;
    decay_gammas[2] = 1169.1; decay_gammas[3] = 288.5; n_gammas = 4;}
  else if (decaynum < 0.999494) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 807.1;
    decay_gammas[2] = 1159.9; decay_gammas[3] = 288.5; n_gammas = 4;}
  else if (decaynum < 0.999734) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 162.5;
    decay_gammas[2] = 1566.4; decay_gammas[3] = 526.5; n_gammas = 4;}
  else if (decaynum < 0.999880) {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 995.1;
    decay_gammas[2] = 971.9; decay_gammas[3] = 288.5; n_gammas = 4;}
  else {
    decay_gammas[0] = 4126.7; decay_gammas[1] = 690.2;
    decay_gammas[2] = 433.8; decay_gammas[3] = 1131.5; n_gammas = 4;}
    
  // Now, loop over decaying gammas and throw them
  for (G4int j = 0; j != n_gammas; ++j){
    particleGun->SetParticleEnergy(decay_gammas[j]*keV);
    particleGun->SetParticleMomentumDirection(get_random_direction());
    particleGun->GeneratePrimaryVertex(anEvent);
  } // end generation loop

}

void EXOPrimaryGeneratorAction::generate_nCaptureXe136( G4Event *anEvent , G4int mode)
{
    struct nCapture_Xe136Info{
    int jump;
    double level, gamma;
    double ratio;
    };

  particleGun->SetParticleDefinition(aGamma);


  //******** Set Vertex of Capture + Excited State Decay *************//
  particleGun->SetParticlePosition(get_random_position(1));
  {

    nCapture_Xe136Info Decay[] = {
	{  3, 4025,    0.00, 100/135. }, // skip to seen lines
	{ 32, 4025,-3424.47,  23/135. }, // special to 601
	{ 30, 4025,-3039.33,   1. }, // special to 986
	//
	{ 30, 4025, 3424.47, 46/87. }, // 601
	{ 28, 4025, 3039.33, 15/87. }, // 986
	{ 17, 4025, 2184.04, 12/87. }, // 1842
	{ 11, 4025, 2088.93,  5/87. }, // 1936
	{  9, 4025, 1829.38,  1/87. }, // 2196
	{  4, 4025, 1535.15,  7/87. }, // 2490
	{  1, 4025, 1416.68,  1. }, // 2609
	{ 23, 2609, 2007.80, 10/110. }, // 601
	{ 13, 2609,  893.30, 1. }, // 1716
	{ 22, 2490, 2490.38, 84/267. }, // 0
	{ 20, 2490, 1889.21, 25/267. }, // 601
	{ 18, 2490, 1504.30, 100/267. }, // 986
	{ 14, 2490, 1187.55, 1. }, // 1303
	{ 13, 2196,  893.42, 1. }, // 1303
	{ 17, 1936, 1936.05, 63/340. }, // 0
	{ 15, 1936, 1335.00, 71/340. }, // 601
	{ 13, 1936,  949.85, 100/340. }, // 986
	{  9, 1936,  633.32, 46/340. }, // 1303
	{  6, 1936,  267.92, 1. }, // 1668
	{ 12, 1842, 1841.49, 30/130. }, // 0
	{  6, 1842,  538.76, 1. }, // 1303
	{ 10, 1716, 1715.55, 100/170. }, // 0
	{  8, 1716, 1114.50, 46/170. }, // 601
	{  3, 1716,  412.82, 1. }, // 1303
	{  6, 1668, 1067.08, 100/167. }, // 601
	{  4, 1668,  681.93, 1. }, // 986
	{  5, 1303, 1302.73, 100/112. }, // 0
	{  3, 1303,  701.68, 10/112. }, // 601
	{  1, 1303,  316.53, 1. }, // 986
	{  1,  986,  385.15, 1. }, // 601
	{  1,  601,  601.05, 1. }, // 0
	{  0,    0,    0.00, 1. },
      };

    int decay_num = sizeof(Decay)/sizeof(Decay[0]);
    //    int decay_num = GetNumArrayElements();

    G4double energy = 0.; 
    G4double p = G4UniformRand();
    //G4cout << "p = " << p << G4endl;
    for (G4int j = 0; 0<= j && j < decay_num;){
      if (p < Decay[j].ratio){
          energy = Decay[j].gamma;
	  //G4cout << "Energy = " << energy << G4endl;
	if (energy > 0.){
	  particleGun->SetParticleEnergy(energy*keV);
          particleGun->SetParticleMomentumDirection(get_random_direction());
	  particleGun->GeneratePrimaryVertex(anEvent);
	}
	if ( energy < 0.) {
	  switch (mode) {
	  case 1:
	    // nothing to do
	    break;
	  case 2:
	    // force gamma
	    particleGun->SetParticleEnergy(-energy*keV);
	    particleGun->SetParticleMomentumDirection(get_random_direction());
	    particleGun->GeneratePrimaryVertex(anEvent);
	    break;
	  case 3:
	  { // 2 random gamma approximation
	    double level = (G4UniformRand() + G4UniformRand() +1.)*1000.;
	    double target = Decay[j].level+Decay[j].gamma;
	    if (target > 610.) level += 500.;
	    energy = Decay[j].level - level;
	    particleGun->SetParticleEnergy(energy*keV);
	    particleGun->SetParticleMomentumDirection(get_random_direction());
	    particleGun->GeneratePrimaryVertex(anEvent);
	    energy = level - target;
	    particleGun->SetParticleEnergy(energy*keV);
	    particleGun->SetParticleMomentumDirection(get_random_direction());
	    particleGun->GeneratePrimaryVertex(anEvent);
	  }
	    break;
	  case 4:
	    // 100% internal conversion
	    particleGun->SetParticleDefinition(aBeta);
	    energy = -Decay[j].gamma;
	    particleGun->SetParticleEnergy(energy*keV);
	    particleGun->SetParticleMomentumDirection(get_random_direction());
	    particleGun->GeneratePrimaryVertex(anEvent);
	    particleGun->SetParticleDefinition(aGamma);
	    break;
	  }
	}
        if (Decay[j].jump == 0) break;
	else j += Decay[j].jump;
	p = G4UniformRand();
      } else {
        p -= Decay[j].ratio;
	j++;
      }
    }
  }
}

void EXOPrimaryGeneratorAction::generate_nCaptureF(G4Event *anEvent)
{

  struct nCapture_FInfo{
    int jump;
    double level, gamma;
    double ratio;
  };

  particleGun->SetParticleDefinition(aGamma);


  //******** Generate vertex in the HFE ***************//
  G4double r0, x0, y0, z0, phi0;

  particleGun->SetParticleDefinition(aGamma);
  //
  G4double MAXR = 85;
  G4double MAXZ = 85;
  G4bool CorrectVolume = false;
  while(CorrectVolume == false){
    x0 = MAXZ*(2*(G4UniformRand() - 0.5));
    y0 = MAXZ*(2*(G4UniformRand() - 0.5));
    z0 = MAXZ*(2*(G4UniformRand() - 0.5));

    G4ThreeVector point = G4ThreeVector(x0*cm, y0*cm, z0*cm);
    G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4VPhysicalVolume *volume = theNavigator->LocateGlobalPointAndSetup(point);
    
    if (volume->GetName()=="HFE"){
	particleGun->SetParticlePosition(G4ThreeVector(x0*cm, y0*cm, z0*cm));
	CorrectVolume = true;
    }
  }
  //**** Start choosing energies of gammas from f19ng cascade ****//
  nCapture_FInfo Decay[] = {
    {168,      6601.352,          6600.08,    0.0985118423811},
    {163,      6601.352,          5616.82,    0.0144623768602},
    {161,      6601.352,          5543.67,    0.0429679312513},
    {156,      6601.352,           5291.4,    0.0247327604276},
    {152,      6601.352,          4757.02,    0.0198071683085},
    {147,      6601.352,           4630.6,  0.000628798993922},
    {143,      6601.352,          4556.81,    0.0547055124712},
    {121,      6601.352,          3112.72,    0.0251519597569},
    {119,      6601.352,          3074.81,    0.0198071683085},
    {110,      6601.352,          3014.58,    0.0424439320897},
    {101,      6601.352,          2921.01,   0.00985118423811},
    {96,       6601.352,          2636.11,    0.0101655837351},
    {91,       6601.352,          2519.05,   0.00733598826242},
    {87,       6601.352,          2324.11,    0.0122615803815},
    {84,       6601.352,           2229.8,   0.00544959128065},
    {81,       6601.352,          2009.52,   0.00492559211905},
    {77,       6601.352,          1708.52,   0.00272479564033},
    {75,       6601.352,           1375.2,  0.000523999161601},
    {72,       6601.352,          1318.52,   0.00241039614337},
    {66,       6601.352,          1282.14,   0.00901278557954},
    {64,       6601.352,          1135.38,  0.000943198490882},
    {58,       6601.352,           1046.0,    0.0185495703207},
    {53,       6601.352,           978.19,   0.00639278977154},
    {51,       6601.352,            791.2,  0.000419199329281},
    {33,       6601.352,            665.2,     0.156151750157},
    {27,       6601.352,           662.24,    0.0106895828967},
    {9,        6601.352,           583.55,     0.377279396353},
    {3,        6601.352,           556.41,    0.0211695661287},
    {1,        6601.352,            302.2,                1.0},
    {139,        6299.1,           6299.1,                1.0},
    {129,       6044.92,          4735.22,     0.276923076923},
    {125,       6044.92,          4200.56,     0.553846153846},
    {96,        6044.92,          2556.35,    0.0820512820513},
    {86,        6044.92,           2458.0,    0.0307692307692},
    {73,        6044.92,          2079.72,                1.0},
    {133,      6017.784,          6016.72,     0.260099612618},
    {131,      6017.784,          5360.93,    0.0329275041505},
    {127,      6017.784,           5033.5,     0.171555063641},
    {125,      6017.784,           4960.3,   0.00747094631987},
    {120,      6017.784,          4708.19,    0.0143884892086},
    {116,      6017.784,          4173.54,     0.046209186497},
    {111,      6017.784,          4046.71,   0.00996126175982},
    {107,      6017.784,          3973.47,   0.00664084117322},
    {103,      6017.784,          3823.05,    0.0293303818484},
    {94,       6017.784,           3152.1,   0.00387382401771},
    {89,       6017.784,          3051.43,    0.0821804095185},
    {82,       6017.784,           2529.2,     0.160486995019},
    {72,       6017.784,          2431.08,    0.0968456004427},
    {68,       6017.784,          2427.83,    0.0525733259546},
    {62,       6017.784,          2337.58,   0.00387382401771},
    {57,       6017.784,           2052.8,   0.00138350857775},
    {52,       6017.784,           1935.5,                1.0},
    {116,        5939.1,           5938.1,     0.123595505618},
    {111,        5939.1,           4954.5,      0.23595505618},
    {102,        5939.1,          4095.01,     0.314606741573},
    {94,         5939.1,           3894.2,     0.134831460674},
    {63,         5939.1,          2352.44,                1.0},
    {111,      5936.128,           5935.1,    0.0660762942779},
    {109,      5936.128,          5279.27,     0.287465940054},
    {105,      5936.128,          4951.91,    0.0401907356948},
    {103,      5936.128,           4878.8,   0.00613079019074},
    {98,       5936.128,           4626.5,   0.00544959128065},
    {94,       5936.128,           4092.2,    0.0115803814714},
    {89,       5936.128,          3964.85,     0.300408719346},
    {85,       5936.128,          3891.39,    0.0122615803815},
    {81,       5936.128,          3741.44,    0.0395095367847},
    {72,       5936.128,           3070.9,    0.0136239782016},
    {67,       5936.128,           2969.7,    0.0108991825613},
    {60,       5936.128,          2447.58,    0.0960490463215},
    {50,       5936.128,          2349.55,    0.0211171662125},
    {46,       5936.128,           2346.3,    0.0143051771117},
    {40,       5936.128,          2255.82,    0.0592643051771},
    {35,       5936.128,          1970.95,   0.00681198910082},
    {30,       5936.128,          1853.96,                1.0},
    {94,         5810.1,           5810.1,                1.0},
    {93,        5623.13,           5622.5,     0.137931034483},
    {88,        5623.13,           4639.0,     0.396551724138},
    {82,        5623.13,          4313.29,     0.310344827586},
    {71,        5623.13,           3578.6,                1.0},
    {89,        5555.34,          5554.59,     0.305882352941},
    {87,        5555.34,           4899.2,    0.0411764705882},
    {78,        5555.34,          4245.65,     0.547058823529},
    {74,        5555.34,           3711.0,    0.0705882352941},
    {55,        5555.34,           2690.5,                1.0},
    {54,        5465.89,           2600.3,                1.0},
    {83,        5319.17,          5318.32,      0.22619047619},
    {78,        5319.17,          4335.09,     0.559523809524},
    {76,        5319.17,           4262.5,    0.0357142857143},
    {71,        5319.17,           4009.3,     0.119047619048},
    {67,        5319.17,           3475.3,                1.0},
    {78,        5282.79,           5282.1,     0.571428571429},
    {72,        5282.79,           4225.8,                1.0},
    {76,         5226.1,           5226.1,                1.0},
    {72,        4892.76,           4070.0,               0.35},
    {52,        4892.76,           2697.9,                0.2},
    {24,        4892.76,           1306.2,                1.0},
    {68,        4591.72,           3607.8,                0.6},
    {66,        4591.72,           3534.4,                1.0},
    {66,        4371.47,          3387.56,     0.938461538462},
    {12,        4371.47,            691.4,                1.0},
    {64,        4277.09,          3293.23,     0.240740740741},
    {62,        4277.09,          3219.89,     0.564814814815},
    {47,        4277.09,           2232.9,                1.0},
    {65,        4082.17,          4081.77,     0.355263157895},
    {60,        4082.17,           3098.1,    0.0460526315789},
    {58,        4082.17,           3025.1,                0.5},
    {43,        4082.17,          2038.08,                1.0},
    {57,        3965.07,          2981.25,     0.261194029851},
    {51,        3965.07,          2655.74,     0.582089552239},
    {47,        3965.07,          2120.95,      0.10447761194},
    {23,        3965.07,           793.36,                1.0},
    {57,        3680.17,          3679.91,     0.465240641711},
    {55,        3680.17,           3023.9,     0.171122994652},
    {50,        3680.17,          2623.18,     0.235294117647},
    {45,        3680.17,          2370.88,    0.0427807486631},
    {41,        3680.17,           1836.5,                1.0},
    {52,         3589.8,          3589.47,     0.831775700935},
    {50,         3589.8,          2933.76,     0.107476635514},
    {31,         3589.8,          1545.87,                1.0},
    {49,       3586.545,          3586.23,     0.328798185941},
    {47,       3586.545,          2930.31,    0.0975056689342},
    {43,       3586.545,          2602.75,    0.0396825396825},
    {41,       3586.545,          2529.55,     0.102040816327},
    {33,       3586.545,           1742.7,   0.00680272108844},
    {25,       3586.545,           1542.5,     0.310657596372},
    {21,       3586.545,          1392.22,    0.0884353741497},
    {8,        3586.545,           620.44,                1.0},
    {36,        3526.31,          2469.34,                1.0},
    {40,       3488.409,          3488.13,     0.725806451613},
    {35,       3488.409,          2504.54,    0.0383064516129},
    {33,       3488.409,          2431.43,     0.070564516129},
    {28,       3488.409,          2179.09,    0.0917338709677},
    {24,       3488.409,           1644.5,                1.0},
    {31,        3171.69,          2187.96,                1.0},
    {34,       2966.109,           2965.9,     0.270833333333},
    {32,       2966.109,          2309.96,     0.122023809524},
    {29,       2966.109,          2143.26,     0.583333333333},
    {9,        2966.109,           771.71,                1.0},
    {30,        2864.86,          2864.68,     0.380952380952},
    {28,        2864.86,           2208.5,     0.047619047619},
    {25,        2864.86,           2042.0,     0.119047619048},
    {18,        2864.86,           1555.0,     0.119047619048},
    {14,        2864.86,           1020.9,    0.0714285714286},
    {9,         2864.86,            894.1,    0.0714285714286},
    {5,         2864.86,            820.9,     0.119047619048},
    {1,         2864.86,            670.1,                1.0},
    {22,       2194.301,          2194.16,     0.469964664311},
    {18,       2194.301,          1371.53,     0.512367491166},
    {11,       2194.301,            885.0,                1.0},
    {19,       2043.982,          2043.89,    0.0752212389381},
    {17,       2043.982,           1387.9,      0.91814159292},
    {8,        2043.982,           734.84,                1.0},
    {16,        1970.83,          1970.73,     0.176817288802},
    {12,        1970.83,          1148.05,     0.518664047151},
    {10,        1970.83,            987.2,   0.00785854616896},
    {4,         1970.83,           661.63,                1.0},
    {12,       1843.802,          1843.74,     0.913173652695},
    {10,       1843.802,           1187.7,    0.0673652694611},
    {1,        1843.802,            534.6,                1.0},
    {9,        1309.195,          1309.17,     0.916767189385},
    {7,        1309.195,            653.2,    0.0241254523522},
    {3,        1309.195,           325.73,    0.0494571773221},
    {1,        1309.195,           252.65,                1.0},
    {5,        1056.821,          1056.78,                1.0},
    {4,          983.59,           983.53,                1.0},
    {3,         822.734,           822.69,     0.332321699545},
    {1,         822.734,           166.78,                1.0},
    {1,         656.018,            656.0,                1.0},
    {0,             0.0,              0.0,                1.0},
  };  



  int decay_num = sizeof(Decay)/sizeof(Decay[0]);
  G4double energy = 0.;
  if(1){
    G4double p = G4UniformRand();
        //G4cout << "p = " << p << G4endl;
    for (G4int j = 0; 0<= j && j < decay_num;){
      if (p < Decay[j].ratio){
        energy = Decay[j].gamma;
	//G4cout << "energy = " << energy << G4endl;
        if (energy > 0.){
	  //G4cout << "Setting energy " << energy << G4endl;
          particleGun->SetParticleEnergy(energy*keV);
          particleGun->SetParticleMomentumDirection(get_random_direction());
          // If energy > 0, a gamma is generated, but that doesn't mean
          // the cascade is finished!
          particleGun->GeneratePrimaryVertex(anEvent);
                //G4cout <<"After generation" << G4endl;
	}
        // Only break out of loop when 'jump' == 0
        if (Decay[j].jump == 0) break;
        else j += Decay[j].jump;
        p = G4UniformRand();
      } else {
	p -= Decay[j].ratio;
	j++;
      }
    }
  }
   
}

void EXOPrimaryGeneratorAction::generate_nCaptureCu(G4Event *anEvent,G4int iso, G4String component)
{
  //  G4cout << "In Copper Generator " << G4endl;

  struct nCapture_CuInfo{
    int jump;
    double level, gamma;
    double ratio;
  };

  particleGun->SetParticleDefinition(aGamma);


  //******** Generate vertex in the Copper Vessel ***************//
  G4double r0, x0, y0, z0, phi0;

  particleGun->SetParticleDefinition(aGamma);
  // Get Maximum of Vessel parameters from EXODetectorConstruction
  /*
  if (component == "LXeVessel"){
    G4double COPPER_VESSEL_MAXR = 23.;
    G4double COPPER_VESSEL_MAXZ = 54.;
    G4bool CopperVolume = false;
    while(CopperVolume == false){
      phi0 = 2*pi*G4UniformRand();
      r0 = COPPER_VESSEL_MAXR*sqrt(G4UniformRand());
      z0 = COPPER_VESSEL_MAXZ*(2*(G4UniformRand() - 0.5));
      x0 = r0*cos(phi0);
      y0 = r0*sin(phi0);
      
      G4ThreeVector point = G4ThreeVector(x0*cm, y0*cm, z0*cm);
      G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4VPhysicalVolume *volume = theNavigator->LocateGlobalPointAndSetup(point);
      
      if (volume->GetName()==component){
        //G4cout << component << " chosen" << G4endl;
	particleGun->SetParticlePosition(G4ThreeVector(x0*cm, y0*cm, z0*cm));
	CopperVolume = true;
      }
    }
  }
  */
    G4double MAXR = 88.0;
    G4double MAXZ = 91.0;
    G4bool CorrectVolume = false;
    while(CorrectVolume == false){
      x0 = MAXZ*(2*(G4UniformRand() - 0.5));
      y0 = MAXZ*(2*(G4UniformRand() - 0.5));
      z0 = MAXZ*(2*(G4UniformRand() - 0.5));


      //      phi0 = 2*pi*G4UniformRand();

      /*      if ( component == "LXeVessel" ){
      r0 = MAXR*sqrt(G4UniformRand());
      z0 = MAXZ*(2*(G4UniformRand() - 0.5));
      x0 = r0*cos(phi0);
      y0 = r0*sin(phi0);
      }
      else {
      r0 = MAXR*sqrt(G4UniformRand());
      z0 = r0*cos(phi0);
      x0 = MAXZ*(2*(G4UniformRand() - 0.5));
      y0 = r0*sin(phi0);
      }*/

      G4ThreeVector point = G4ThreeVector(x0*cm, y0*cm, z0*cm);
      G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4VPhysicalVolume *volume = theNavigator->LocateGlobalPointAndSetup(point);
      
      if (volume->GetName()==component){
	///	G4cout << component << " chosen" << G4endl;
	//G4cout << "x,y,z = " << x0 << " " << y0 << " " << z0 << G4endl;
	particleGun->SetParticlePosition(G4ThreeVector(x0*cm, y0*cm, z0*cm));
	CorrectVolume = true;
      }
    }
  



  if (iso == 63){
        //G4cout << "Decay cascade 63 being defined" << G4endl;


    nCapture_CuInfo  Decay[] = {
      {306,      7916.419,          7916.26,     0.333437427596},
      {304,      7916.419,          7756.91,    0.0159163485076},
      {302,      7916.419,           7638.0,     0.163192940394},
      {299,      7916.419,          7572.32,    0.0175281306349},
      {296,      7916.419,           7555.1,  0.000846185616859},
      {290,      7916.419,          7307.31,    0.0902597991317},
      {285,      7916.419,          7253.05,    0.0418055989282},
      {279,      7916.419,          7177.07,    0.0257885140376},
      {277,      7916.419,           7170.1,   0.00305231240367},
      {274,      7916.419,          7037.83,   0.00390857165883},
      {267,      7916.419,          6988.96,    0.0353584704188},
      {262,      7916.419,          6674.85,    0.0200465402089},
      {259,      7916.419,           6628.9,  0.000997290191298},
      {256,      7916.419,          6618.15,    0.0113832112744},
      {254,      7916.419,          6595.63,   0.00635646576474},
      {248,      7916.419,           6553.0,   0.00100736382959},
      {243,      7916.419,          6477.15,    0.0017326657869},
      {238,      7916.419,           6416.9,  0.000956995638115},
      {232,      7916.419,          6394.86,     0.014022504508},
      {230,      7916.419,           6365.6,   0.00106780565937},
      {225,      7916.419,          6321.54,   0.00363658342484},
      {223,      7916.419,          6308.61,   0.00168229759542},
      {221,      7916.419,           6233.0,   0.00109802657426},
      {213,      7916.419,           6166.9,   0.00167222395713},
      {209,      7916.419,          6136.05,    0.0019845067443},
      {206,      7916.419,          6063.65,   0.00611469844564},
      {204,      7916.419,           6015.7,   0.00230686316977},
      {201,      7916.419,          6010.83,    0.0160170848905},
      {197,      7916.419,          5866.16,   0.00145060391462},
      {194,      7916.419,           5824.0,   0.00302209148878},
      {188,      7916.419,          5771.48,   0.00512748189264},
      {182,      7916.419,          5636.18,   0.00413019170134},
      {180,      7916.419,          5615.01,    0.0049763773182},
      {178,      7916.419,           5600.5,  0.000977142914707},
      {174,      7916.419,          5555.78,   0.00276017689309},
      {170,      7916.419,           5528.2,    0.0012491311487},
      {167,      7916.419,          5450.75,   0.00159163485076},
      {158,      7916.419,          5418.49,    0.0186362308475},
      {156,      7916.419,          5408.88,   0.00401938168008},
      {154,      7916.419,           5385.0,  0.000110810021255},
      {152,      7916.419,           5321.3,  0.000554050106277},
      {150,      7916.419,          5280.67,   0.00251840957399},
      {148,      7916.419,           5269.4,  0.000805891063675},
      {143,      7916.419,          5258.67,    0.0103859210831},
      {135,      7916.419,          5190.09,   0.00672919038169},
      {132,      7916.419,          5183.89,   0.00368695161632},
      {129,      7916.419,          5152.11,   0.00270980870161},
      {125,      7916.419,          5139.86,   0.00180318125497},
      {122,      7916.419,           5085.3,   0.00330415336107},
      {119,      7916.419,           5023.2,   0.00184347580816},
      {115,      7916.419,           5019.5,   0.00191399127623},
      {113,      7916.419,          4983.51,   0.00203487493578},
      {110,      7916.419,          4903.08,   0.00152111938269},
      {108,      7916.419,           4883.0,  0.000392871893542},
      {103,      7916.419,           4835.1,  0.000302209148878},
      {100,      7916.419,           4803.8,   0.00119876295722},
      {98,       7916.419,           4790.7,   0.00107787929767},
      {94,       7916.419,           4708.9,  0.000856259255155},
      {91,       7916.419,          4658.53,   0.00775670148788},
      {88,       7916.419,          4603.07,    0.0054699855947},
      {85,       7916.419,           4572.5,   0.00100736382959},
      {81,       7916.419,          4562.95,   0.00315304878663},
      {77,       7916.419,          4504.04,   0.00490586185012},
      {75,       7916.419,          4475.66,   0.00481519910546},
      {72,       7916.419,          4450.86,    0.0017326657869},
      {69,       7916.419,           4440.9,   0.00196435946771},
      {67,       7916.419,          4423.12,   0.00141030936143},
      {63,       7916.419,           4405.0,   0.00306238604197},
      {61,       7916.419,           4391.9,   0.00133979389336},
      {58,       7916.419,          4320.24,    0.0135994116995},
      {54,       7916.419,           4312.8,   0.00282061872286},
      {52,       7916.419,          4286.62,   0.00339481610573},
      {50,       7916.419,          4204.37,   0.00255870412717},
      {46,       7916.419,          4133.08,   0.00383805619075},
      {38,       7916.419,          4089.11,   0.00255870412717},
      {31,       7916.419,           3883.0,   0.00147075119121},
      {25,       7916.419,          3844.44,   0.00493608276501},
      {22,       7916.419,          3775.27,   0.00203487493578},
      {19,       7916.419,           3651.6,  0.000624565574348},
      {15,       7916.419,          3588.52,   0.00342503702062},
      {11,       7916.419,           3482.9,   0.00115846840403},
      {7,        7916.419,           3472.2,   0.00184347580816},
      {5,        7916.419,           3366.8,   0.00145060391462},
      {1,        7916.419,          3153.05,                1.0},
      {188,       4763.39,          3521.02,     0.385650224215},
      {183,       4763.39,           3442.6,     0.210762331839},
      {30,        4763.39,           960.63,                1.0},
      {47,        4549.86,          1074.49,                1.0},
      {218,       4444.48,          4444.35,     0.676549865229},
      {215,       4444.48,           4166.7,     0.137466307278},
      {201,       4444.48,           3781.8,                1.0},
      {186,       4432.92,           3506.7,     0.214067278287},
      {179,       4432.92,           3145.4,     0.357798165138},
      {152,       4432.92,           2838.2,                1.0},
      {201,       4327.67,           3718.1,     0.141414141414},
      {185,       4327.67,           3431.6,     0.401515151515},
      {57,        4327.67,          1136.59,                1.0},
      {186,       4264.15,          3385.73,     0.631386861314},
      {175,       4264.15,           3022.8,                1.0},
      {192,       4140.83,           3478.0,     0.544973544974},
      {57,        4140.83,          1059.95,                1.0},
      {204,       4071.59,           3911.8,     0.170289855072},
      {200,       4071.59,           3729.6,    0.0634057971014},
      {177,       4071.59,          3175.26,     0.278985507246},
      {151,       4071.59,          2572.03,      0.38768115942},
      {105,       4071.59,           1747.3,                1.0},
      {200,       4034.03,           4033.4,     0.170138888889},
      {198,       4034.03,           3874.7,            0.15625},
      {169,       4034.03,           3108.0,     0.121527777778},
      {128,       4034.03,          2291.42,                1.0},
      {194,       3990.85,           3714.0,     0.248868778281},
      {189,       3990.85,          3628.36,                1.0},
      {193,       3826.92,           3667.6,     0.138936535163},
      {187,       3826.92,          3464.55,     0.445969125214},
      {144,       3826.92,          2365.32,     0.265866209262},
      {95,        3826.92,          1501.94,                1.0},
      {190,       3802.74,           3802.0,     0.346570397112},
      {174,       3802.74,           3140.0,      0.56678700361},
      {109,       3802.74,           1826.2,                1.0},
      {186,       3783.14,           3623.1,              0.352},
      {163,       3783.14,           2904.6,                0.1},
      {120,       3783.14,          2082.45,                1.0},
      {183,       3711.92,           3552.9,                1.0},
      {54,        3629.42,           897.06,                1.0},
      {182,       3603.09,           3603.9,     0.154696132597},
      {170,       3603.09,          2993.91,     0.596685082873},
      {157,       3603.09,           2724.8,                1.0},
      {86,        3596.04,          1316.24,     0.164133738602},
      {81,        3596.04,           1241.5,                1.0},
      {85,         3524.7,          1250.45,                1.0},
      {176,       3511.19,           3510.5,      0.33064516129},
      {173,       3511.19,           3232.3,     0.177419354839},
      {154,       3511.19,           2772.2,                1.0},
      {162,       3493.37,           2885.3,                1.0},
      {171,       3475.52,          3316.58,     0.828571428571},
      {156,       3475.52,           2811.1,                1.0},
      {80,        3465.57,          1198.75,      0.28870292887},
      {27,        3465.57,           532.94,                1.0},
      {63,        3440.28,           946.64,                1.0},
      {166,       3412.19,           3253.2,     0.366847826087},
      {164,       3412.19,           3133.9,     0.326086956522},
      {122,       3412.19,           2048.9,                1.0},
      {162,       3352.84,           3074.9,     0.203389830508},
      {85,        3352.84,          1447.69,     0.545197740113},
      {40,        3352.84,           634.78,                1.0},
      {157,       3343.92,           3001.4,     0.537735849057},
      {109,       3343.92,          1844.67,                1.0},
      {159,       3313.11,           3312.4,     0.431034482759},
      {11,        3313.11,           261.33,                1.0},
      {157,       3257.61,          3257.26,     0.710526315789},
      {91,        3257.61,          1556.84,                1.0},
      {126,       3207.59,          2280.36,     0.348525469169},
      {54,        3207.59,          831.176,                1.0},
      {64,        3191.07,           937.01,                1.0},
      {152,        3125.5,           3125.5,                1.0},
      {151,       3111.82,           3111.6,     0.201030927835},
      {9,         3111.82,           214.97,                1.0},
      {44,        3080.84,            587.0,     0.226519337017},
      {37,        3080.84,           583.22,                1.0},
      {147,       3051.77,           3052.2,     0.853658536585},
      {112,       3051.77,           1808.5,                1.0},
      {79,        3033.76,          1293.92,                1.0},
      {140,       3013.07,           2666.6,     0.152091254753},
      {96,        3013.07,          1574.36,                1.0},
      {142,       2932.48,          2932.06,                1.0},
      {70,        2896.84,          1127.84,     0.668555240793},
      {57,        2896.84,           804.29,      0.13597733711},
      {37,        2896.84,           440.13,                1.0},
      {104,       2892.35,          1649.52,     0.360869565217},
      {47,        2892.35,           625.35,                1.0},
      {136,       2830.54,           2830.1,     0.391156462585},
      {93,        2830.54,           1476.1,                1.0},
      {134,       2776.56,           2776.8,     0.308988764045},
      {127,       2776.56,           2413.7,     0.216292134831},
      {112,       2776.56,          2037.53,                1.0},
      {130,       2764.19,           2605.2,     0.416666666667},
      {48,        2764.19,           689.08,                1.0},
      {129,       2732.32,          2732.13,     0.738461538462},
      {117,       2732.32,          2123.06,                1.0},
      {94,        2726.19,          1484.85,    0.0974025974026},
      {89,        2726.19,          1428.17,     0.125541125541},
      {78,        2726.19,           1287.4,    0.0757575757576},
      {53,        2726.19,           957.27,     0.318181818182},
      {10,        2726.19,           192.53,                1.0},
      {93,        2717.97,           1790.3,                1.0},
      {95,        2695.22,          1799.48,                1.0},
      {120,       2657.34,           2656.8,     0.161160354553},
      {118,       2657.34,          2497.89,     0.660757453666},
      {92,        2657.34,          1761.01,    0.0668815471394},
      {54,        2657.34,           974.17,                1.0},
      {78,        2647.91,          1327.62,                1.0},
      {14,        2635.53,           247.58,                1.0},
      {114,        2594.9,           2594.9,                1.0},
      {113,       2533.65,          2533.53,                1.0},
      {112,       2507.29,          2507.29,                1.0},
      {107,       2497.59,          2153.71,     0.246933769419},
      {95,        2497.59,          1834.22,    0.0678659035159},
      {86,        2497.59,          1619.24,     0.200327064595},
      {57,        2497.59,           998.28,     0.169255928046},
      {45,        2497.59,           890.26,     0.131643499591},
      {43,        2497.59,           814.45,                1.0},
      {98,        2493.57,          1918.69,     0.529100529101},
      {89,        2493.57,          1830.34,                1.0},
      {103,       2465.47,          2465.43,                1.0},
      {76,        2456.69,          1560.94,                1.0},
      {80,        2387.95,           1641.7,                1.0},
      {79,        2376.41,           1630.1,                0.5},
      {41,        2376.41,           782.29,                1.0},
      {65,        2360.47,          1119.55,                1.0},
      {35,        2354.62,           747.34,                1.0},
      {96,        2324.75,          2324.75,                1.0},
      {91,        2316.49,          1972.59,                1.0},
      {93,        2301.09,          2141.73,                1.0},
      {82,        2279.75,          1670.92,                1.0},
      {88,        2274.27,           1929.5,     0.218543046358},
      {53,        2274.27,           953.97,                1.0},
      {84,        2267.01,           1904.8,                1.0},
      {85,        2254.06,          1910.18,                1.0},
      {82,        2144.53,           1782.2,     0.480487804878},
      {80,        2144.53,          1570.22,     0.109756097561},
      {75,        2144.53,           1535.7,     0.129268292683},
      {70,        2144.53,          1481.75,     0.146341463415},
      {9,         2144.53,           291.71,                1.0},
      {77,        2092.26,           1729.7,                1.0},
      {39,        2075.12,           711.94,                1.0},
      {60,         2050.0,           1303.9,              0.528},
      {38,         2050.0,           695.41,                1.0},
      {72,        1976.33,          1401.66,                1.0},
      {57,       1905.093,         1158.831,     0.800643086817},
      {51,       1905.093,          1009.35,                1.0},
      {76,        1900.28,          1900.25,                1.0},
      {75,        1852.65,          1852.64,     0.784240150094},
      {70,        1852.65,          1508.68,                1.0},
      {69,        1779.54,           1435.3,    0.0721649484536},
      {66,        1779.54,          1417.27,                1.0},
      {65,        1768.99,          1407.08,                1.0},
      {70,         1749.2,           1749.2,                1.0},
      {69,        1742.59,          1742.83,     0.301694915254},
      {64,        1742.59,           1398.7,     0.281355932203},
      {41,        1742.59,           846.87,                1.0},
      {59,        1739.85,          1165.21,                1.0},
      {64,        1700.65,          1541.56,     0.337579617834},
      {41,        1700.65,           822.33,                1.0},
      {63,       1683.122,          1683.09,                1.0},
      {55,        1607.31,          1032.68,                1.0},
      {61,       1594.315,          1594.42,     0.284382284382},
      {54,       1594.315,          1232.13,     0.715617715618},
      {52,       1594.315,          1019.59,                1.0},
      {51,        1594.23,          1019.59,                1.0},
      {56,        1550.54,          1391.25,                1.0},
      {56,       1521.147,           1521.2,               0.36},
      {54,       1521.147,          1361.76,     0.424545454545},
      {50,       1521.147,          1177.04,               0.03},
      {42,       1521.147,           912.37,    0.0881818181818},
      {37,       1521.147,           858.09,                1.0},
      {51,        1499.18,          1499.54,     0.249128919861},
      {49,        1499.18,          1339.88,     0.393728222997},
      {47,        1499.18,          1220.84,                1.0},
      {22,        1461.35,           565.43,                1.0},
      {47,        1438.69,          1438.75,     0.214516129032},
      {45,        1438.69,          1279.41,     0.312903225806},
      {39,        1438.69,          1076.35,     0.443548387097},
      {29,        1438.69,            775.9,                1.0},
      {43,        1363.21,          1363.21,                1.0},
      {42,        1354.25,          1354.68,    0.0986717267552},
      {40,        1354.25,          1194.89,     0.552182163188},
      {34,        1354.25,           992.11,     0.161290322581},
      {32,        1354.25,           779.65,                1.0},
      {38,       1320.329,         1320.315,                1.0},
      {37,       1298.123,         1298.134,     0.331437855402},
      {35,       1298.123,         1138.821,                1.0},
      {29,        1287.15,           924.91,                1.0},
      {34,        1242.65,          1242.56,                1.0},
      {32,       1241.091,          1081.74,     0.193140794224},
      {30,       1241.091,           962.68,     0.256317689531},
      {20,       1241.091,           632.34,     0.153429602888},
      {9,        1241.091,          494.852,                1.0},
      {29,        927.079,           927.05,    0.0855297157623},
      {27,        927.079,          767.795,     0.175452196382},
      {25,        927.079,            648.8,                1.0},
      {25,        895.714,           736.52,    0.0752256770311},
      {23,        895.714,          617.433,     0.744232698094},
      {17,        895.714,            320.7,                1.0},
      {23,        878.277,          878.277,     0.585253456221},
      {18,        878.277,           534.11,                1.0},
      {19,        746.245,          467.992,                1.0},
      {20,        739.051,           739.12,    0.0679389312977},
      {18,        739.051,          579.753,     0.631043256997},
      {16,        739.051,          460.792,    0.0959287531807},
      {13,        739.051,           395.28,    0.0269720101781},
      {10,        739.051,          376.851,                1.0},
      {15,          663.0,           663.06,      0.37247706422},
      {13,          663.0,           503.65,     0.293577981651},
      {11,          663.0,           384.74,     0.278899082569},
      {8,           663.0,            318.9,                1.0},
      {11,        608.784,           608.75,     0.785498489426},
      {9,         608.784,          449.512,      0.10775427996},
      {7,         608.784,           330.47,    0.0251762336354},
      {4,         608.784,          264.882,                1.0},
      {1,         574.629,          212.388,                1.0},
      {6,         362.231,            362.3,    0.0231259968102},
      {4,         362.231,          202.948,                1.0},
      {4,         343.898,           343.94,     0.968801313629},
      {2,         343.898,          184.612,                1.0},
      {2,         278.257,          278.244,                1.0},
      {1,         159.282,           159.28,                1.0},
      {0,             0.0,              0.0,                1.0},
    };

  int decay_num = sizeof(Decay)/sizeof(Decay[0]);
  G4double energy = 0.;
  if(1){
    G4double p = G4UniformRand();
        //G4cout << "p = " << p << G4endl;
    for (G4int j = 0; 0<= j && j < decay_num;){
      if (p < Decay[j].ratio){
        energy = Decay[j].gamma;
	//G4cout << "energy = " << energy << G4endl;
        if (energy > 0.){
	  //G4cout << "Setting energy " << energy << G4endl;
          particleGun->SetParticleEnergy(energy*keV);
          particleGun->SetParticleMomentumDirection(get_random_direction());
          // If energy > 0, a gamma is generated, but that doesn't mean
          // the cascade is finished!
          particleGun->GeneratePrimaryVertex(anEvent);
                //G4cout <<"After generation" << G4endl;
	}
        // Only break out of loop when 'jump' == 0
        if (Decay[j].jump == 0) break;
        else j += Decay[j].jump;
        p = G4UniformRand();
      } else {
	p -= Decay[j].ratio;
	j++;
      }
    }
  }
   
  }

  else if (iso == 65){
    //**** Start choosing energies of gammas from cu65ng cascade ****//
     nCapture_CuInfo Decay[] = {
      {346,       7066.22,          7065.72,    0.0240776531306},
      {344,       7066.22,          6879.82,   0.00378980100845},
      {342,       7066.22,           6828.1,  0.000237537385145},
      {339,       7066.22,          6790.64,    0.0283965146787},
      {336,       7066.22,          6680.01,     0.149000723409},
      {332,       7066.22,          6600.66,     0.155479015731},
      {328,       7066.22,          6336.16,   0.00155479015731},
      {323,       7066.22,          6243.19,    0.0263450554434},
      {317,       7066.22,          6048.84,    0.0184631331181},
      {313,       7066.22,          6013.46,   0.00129565846443},
      {309,       7066.22,           5907.9,  0.000377900385458},
      {306,       7066.22,          5853.41,    0.0120064351037},
      {301,       7066.22,          5722.03,    0.0041353099323},
      {298,       7066.22,           5626.9,  0.000485871924161},
      {294,       7066.22,          5518.73,   0.00711532440049},
      {291,       7066.22,          5505.56,   0.00102572961767},
      {289,       7066.22,          5488.65,   0.00100413530993},
      {284,       7066.22,          5388.12,   0.00574408585897},
      {276,       7066.22,          5320.23,     0.066402496302},
      {270,       7066.22,          5245.88,    0.0788192232528},
      {264,       7066.22,          5138.98,   0.00283965146787},
      {258,       7066.22,          5047.98,    0.0377900385458},
      {248,       7066.22,           5043.0,    0.0632713216796},
      {245,       7066.22,          4941.94,   0.00401654123973},
      {243,       7066.22,          4903.09,   0.00831380848008},
      {241,       7066.22,          4900.15,   0.00345508923848},
      {237,       7066.22,           4805.3,    0.0025049396979},
      {235,       7066.22,           4703.4,  0.000550654847382},
      {233,       7066.22,           4671.2,   0.00211624215857},
      {231,       7066.22,           4617.8,   0.00185711046568},
      {227,       7066.22,          4612.55,   0.00412451277843},
      {224,       7066.22,          4562.96,   0.00361704654653},
      {222,       7066.22,           4545.2,  0.000475074770291},
      {219,       7066.22,           4506.9,  0.000593843462863},
      {215,       7066.22,          4479.82,    0.0115529546412},
      {211,       7066.22,          4468.58,   0.00288284008335},
      {209,       7066.22,          4457.73,   0.00240776531306},
      {206,       7066.22,           4435.2,  0.000593843462863},
      {201,       7066.22,          4401.69,    0.0111210684863},
      {193,       7066.22,          4385.13,    0.0377900385458},
      {190,       7066.22,          4377.75,   0.00831380848008},
      {187,       7066.22,          4326.66,   0.00913439217422},
      {184,       7066.22,          4298.14,   0.00245095392854},
      {180,       7066.22,          4266.54,   0.00124167269508},
      {176,       7066.22,          4251.95,   0.00138203569539},
      {173,       7066.22,           4221.2,   0.00377900385458},
      {170,       7066.22,          4198.32,    0.0117688977186},
      {167,       7066.22,          4122.91,   0.00356306077718},
      {162,       7066.22,          4117.22,   0.00950149540581},
      {156,       7066.22,          4113.25,   0.00669423539955},
      {153,       7066.22,           4078.2,   0.00144681861861},
      {149,       7066.22,          4055.62,   0.00267769415982},
      {146,       7066.22,          4039.82,   0.00809786540268},
      {143,       7066.22,           4020.3,   0.00106891823315},
      {141,       7066.22,          4016.95,   0.00356306077718},
      {135,       7066.22,          3988.21,   0.00139283284926},
      {131,       7066.22,          3974.97,   0.00422168716326},
      {126,       7066.22,          3966.99,   0.00462118185646},
      {123,       7066.22,           3955.1,   0.00104732392541},
      {121,       7066.22,           3923.8,  0.000399494693199},
      {118,       7066.22,           3914.0,  0.000712612155436},
      {114,       7066.22,          3900.18,     0.013064556183},
      {111,       7066.22,           3857.0,   0.00451321031776},
      {109,       7066.22,          3818.14,   0.00545256270447},
      {106,       7066.22,          3778.38,   0.00520422816546},
      {103,       7066.22,          3732.12,   0.00264530269821},
      {101,       7066.22,          3724.08,   0.00507466231901},
      {98,        7066.22,          3695.05,   0.00157638446506},
      {95,        7066.22,          3668.23,   0.00100413530993},
      {92,        7066.22,          3633.54,   0.00153319584957},
      {89,        7066.22,           3586.2,  0.000809786540268},
      {83,        7066.22,           3578.0,   0.00125246984895},
      {78,        7066.22,          3556.99,    0.0045348046255},
      {73,        7066.22,          3530.56,   0.00766597924787},
      {68,        7066.22,          3482.51,   0.00356306077718},
      {63,        7066.22,           3465.0,   0.00691017847695},
      {55,        7066.22,           3429.4,    0.0113370115638},
      {52,        7066.22,          3361.11,   0.00164116738828},
      {46,        7066.22,          3316.04,   0.00356306077718},
      {43,        7066.22,          3285.85,   0.00356306077718},
      {37,        7066.22,          3251.26,   0.00259131692886},
      {33,        7066.22,           3169.4,   0.00215943077405},
      {30,        7066.22,           3131.7,   0.00186790761955},
      {27,        7066.22,          3052.91,   0.00248334539016},
      {23,        7066.22,          3009.32,   0.00320675469946},
      {20,        7066.22,           2949.5,   0.00205145923535},
      {16,        7066.22,           2766.2,   0.00129565846443},
      {14,        7066.22,           2603.3,   0.00248334539016},
      {10,        7066.22,           2539.3,  0.000755800770917},
      {5,         7066.22,          2215.88,    0.0175993608085},
      {1,         7066.22,          1988.92,                1.0},
      {209,       5077.21,           3383.6,     0.283752860412},
      {89,        5077.21,          1966.29,     0.443935926773},
      {61,        5077.21,          1589.79,                1.0},
      {211,       4850.76,           3275.2,    0.0656275635767},
      {206,       4850.76,          3172.95,     0.598851517637},
      {202,       4850.76,          3137.18,     0.246103363413},
      {93,        4850.76,           1773.5,                1.0},
      {208,       4527.91,           2968.8,     0.163982430454},
      {190,       4527.91,           2708.2,     0.190336749634},
      {123,       4527.91,          1728.01,                1.0},
      {244,        4462.7,          4276.58,                1.0},
      {240,        4300.2,           4024.9,     0.283950617284},
      {234,        4300.2,           3835.8,     0.234567901235},
      {226,        4300.2,           3476.9,                1.0},
      {225,       4116.41,          3293.49,     0.837209302326},
      {114,       4116.41,          1303.03,                1.0},
      {238,       4056.98,          3871.04,     0.523926380368},
      {234,       4056.98,          3782.18,     0.344785276074},
      {151,       4056.98,          1607.34,                1.0},
      {230,       4013.69,           3627.9,     0.244851258581},
      {214,       4013.69,          2996.78,                1.0},
      {228,       3934.58,           3548.6,     0.551724137931},
      {166,       3934.58,          1916.35,                1.0},
      {232,       3896.38,           3896.3,     0.148648648649},
      {182,       3896.38,           2160.4,     0.175675675676},
      {130,       3896.38,          1298.87,                1.0},
      {229,       3814.66,           3814.2,     0.112456747405},
      {219,       3814.66,           3348.5,     0.195501730104},
      {211,       3814.66,           2991.1,     0.207612456747},
      {162,       3814.66,          1843.71,     0.250865051903},
      {42,        3814.66,           417.03,                1.0},
      {140,       3780.19,          1416.38,     0.578475336323},
      {77,        3780.19,           770.64,                1.0},
      {221,        3750.3,           3563.7,     0.188679245283},
      {169,        3750.3,          2004.42,      0.38679245283},
      {153,        3750.3,          1732.27,     0.199685534591},
      {116,        3750.3,           1120.8,    0.0896226415094},
      {55,         3750.3,            651.1,                1.0},
      {187,       3705.08,          2457.66,     0.561349693252},
      {140,       3705.08,           1682.0,                1.0},
      {214,       3636.56,           3450.3,    0.0828138913624},
      {204,       3636.56,          3045.46,     0.204808548531},
      {192,       3636.56,          2619.14,     0.249332146037},
      {188,       3636.56,           2584.3,     0.080142475512},
      {182,       3636.56,          2423.94,     0.170080142476},
      {158,       3636.56,          1890.61,     0.124666073019},
      {117,       3636.56,          1115.48,                1.0},
      {196,        3601.0,           2870.7,     0.110014104372},
      {166,        3601.0,          2023.55,     0.662905500705},
      {154,        3601.0,           1854.3,    0.0761636107193},
      {13,         3601.0,           114.36,                1.0},
      {200,       3583.53,           3307.4,     0.206378986867},
      {174,       3583.53,           2371.4,     0.243902439024},
      {167,       3583.53,          2144.22,     0.348968105066},
      {51,        3583.53,           537.55,                1.0},
      {165,       3535.49,          2095.76,     0.223068552775},
      {159,       3535.49,          1975.26,     0.203482045702},
      {149,       3535.49,          1799.84,      0.14907508161},
      {41,        3535.49,            457.6,                1.0},
      {195,       3508.84,           3322.2,    0.0969696969697},
      {193,       3508.84,          3270.78,     0.690909090909},
      {184,       3508.84,           2918.0,    0.0787878787879},
      {172,       3508.84,           2492.0,                1.0},
      {192,       3487.05,           3486.2,     0.126036484245},
      {182,       3487.05,          3021.72,     0.358208955224},
      {180,       3487.05,           2896.9,     0.144278606965},
      {172,       3487.05,           2478.2,      0.28192371476},
      {124,       3487.05,           1515.3,                1.0},
      {185,       3479.48,           3241.9,     0.590717299578},
      {60,        3479.48,            665.6,                1.0},
      {161,       3432.37,          2380.34,     0.823529411765},
      {108,       3432.37,           1408.4,                1.0},
      {179,       3397.63,           3121.3,               0.44},
      {172,       3397.63,           2806.9,                1.0},
      {169,       3371.23,           2641.3,     0.342465753425},
      {77,        3371.23,           741.94,                1.0},
      {175,       3342.06,           3067.2,                1.0},
      {81,        3333.77,           747.48,     0.762845849802},
      {54,        3333.77,           533.96,                1.0},
      {170,       3287.36,           2901.2,     0.140164899882},
      {166,       3287.36,          2821.76,                1.0},
      {133,       3247.83,          1670.32,                1.0},
      {173,       3208.95,           3208.1,     0.268292682927},
      {92,        3208.95,          1042.86,                1.0},
      {171,       3165.77,           3165.8,     0.198113207547},
      {161,       3165.77,          2700.26,     0.679245283019},
      {157,       3165.77,           2436.7,                1.0},
      {164,       3151.97,          2876.19,     0.516616314199},
      {83,        3151.97,           788.42,                1.0},
      {162,       3141.74,          2866.61,                1.0},
      {165,       3110.86,           3111.4,              0.192},
      {100,       3110.86,          1139.65,                1.0},
      {162,       3099.08,          2912.57,     0.359908883827},
      {156,       3099.08,           2713.3,     0.227790432802},
      {151,       3099.08,           2508.3,     0.250569476082},
      {139,       3099.08,           2082.6,                1.0},
      {159,       3091.37,          3090.95,     0.208010335917},
      {134,       3091.37,          2039.33,     0.107235142119},
      {122,       3091.37,          1652.01,                1.0},
      {155,       3077.29,           2890.6,     0.205930807249},
      {146,       3077.29,           2612.1,     0.444810543657},
      {138,       3077.29,           2254.5,     0.148270181219},
      {132,       3077.29,           2059.9,     0.110378912685},
      {60,        3077.29,           556.46,                1.0},
      {150,       3048.82,          2862.63,                1.0},
      {146,       3045.95,           2770.7,     0.275862068966},
      {137,       3045.95,           2315.7,                1.0},
      {148,       3026.09,          3025.77,     0.674318507891},
      {95,        3026.09,           1280.2,                1.0},
      {145,       3010.18,           2824.8,     0.321715817694},
      {136,       3010.18,           2545.2,     0.214477211796},
      {111,       3010.18,          1666.15,                1.0},
      {143,       2987.96,           2986.9,             0.4625},
      {78,        2987.96,           1017.1,                1.0},
      {141,       2953.35,          2952.64,      0.36954087346},
      {131,       2953.35,           2488.4,      0.26875699888},
      {123,       2953.35,          2131.25,     0.127659574468},
      {114,       2953.35,          1901.52,     0.170212765957},
      {30,        2953.35,            289.2,                1.0},
      {132,       2948.76,          2673.42,     0.440771349862},
      {100,       2948.76,          1509.64,     0.241046831956},
      {96,        2948.76,          1401.26,     0.101928374656},
      {32,        2948.76,           340.19,                1.0},
      {131,       2943.33,           2758.8,     0.617283950617},
      {125,       2943.33,           2557.8,                1.0},
      {128,       2867.69,          2629.61,     0.763440860215},
      {120,       2867.69,          2402.83,                1.0},
      {124,       2844.72,          2569.26,     0.525373134328},
      {115,       2844.72,          2114.78,                1.0},
      {102,       2813.84,           1761.6,     0.163265306122},
      {90,        2813.84,          1374.41,     0.614795918367},
      {67,        2813.84,           993.48,                1.0},
      {122,       2799.85,           2615.2,     0.146341463415},
      {92,        2799.85,           1553.1,     0.475609756098},
      {69,        2799.85,          1053.88,                1.0},
      {104,       2767.86,          1944.97,     0.655172413793},
      {81,        2767.86,           1220.4,                1.0},
      {117,       2739.16,           2553.1,     0.354609929078},
      {84,        2739.16,           1394.9,                1.0},
      {116,       2688.22,          2687.88,     0.641975308642},
      {113,       2688.22,          2450.91,                1.0},
      {114,       2681.16,          2680.38,      0.20278833967},
      {89,        2681.16,           1629.2,    0.0760456273764},
      {85,        2681.16,           1523.0,    0.0228136882129},
      {82,        2681.16,          1468.56,     0.219687367976},
      {64,        2681.16,           987.18,     0.220532319392},
      {52,        2681.16,           860.85,    0.0599915504858},
      {46,        2681.16,           753.91,                1.0},
      {107,       2664.44,          2664.17,     0.322274881517},
      {102,       2664.44,           2389.3,    0.0947867298578},
      {84,        2664.44,          1647.33,     0.369668246445},
      {77,        2664.44,          1506.57,                1.0},
      {87,        2629.29,          1806.57,     0.473684210526},
      {64,        2629.29,          1081.95,                1.0},
      {101,        2608.5,           2608.5,                1.0},
      {99,        2597.49,          2411.58,     0.490463215259},
      {66,        2597.49,          1253.28,      0.25068119891},
      {14,        2597.49,           234.26,                1.0},
      {96,        2586.27,           2400.0,     0.143497757848},
      {69,        2586.27,          1428.18,     0.681614349776},
      {60,        2586.27,          1146.79,                1.0},
      {94,        2560.43,           2561.3,     0.338308457711},
      {63,        2560.43,          1313.29,                1.0},
      {82,        2520.77,          1929.63,                1.0},
      {81,         2503.0,          1912.13,     0.498207885305},
      {66,         2503.0,           1450.6,                1.0},
      {85,        2453.05,          2177.87,     0.348195329087},
      {76,        2453.05,          1723.07,     0.354564755839},
      {30,        2453.05,           632.67,                1.0},
      {86,        2449.19,           2448.7,                1.0},
      {81,        2394.93,          2120.25,                1.0},
      {72,        2363.63,          1633.89,                1.0},
      {79,        2260.66,          1985.73,     0.639566395664},
      {76,        2260.66,          1874.41,     0.224932249322},
      {57,        2260.66,           1208.8,                1.0},
      {79,        2166.01,          1980.01,                1.0},
      {79,        2163.12,          2163.22,                1.0},
      {78,        2124.09,          2123.95,     0.472140762463},
      {73,        2124.09,           1849.4,                1.0},
      {75,       2023.315,          1837.44,     0.134291658786},
      {71,       2023.315,           1748.0,     0.111594477019},
      {68,       2023.315,          1637.49,      0.44826933989},
      {64,       2023.315,           1557.4,    0.0794401361831},
      {60,       2023.315,          1293.71,    0.0766029884623},
      {42,       2023.315,           810.47,     0.049177227161},
      {37,       2023.315,            679.3,    0.0313977681105},
      {34,       2023.315,           583.62,     0.038963495366},
      {9,        2023.315,           111.93,                1.0},
      {66,        2018.36,          1832.39,     0.335533553355},
      {62,        2018.36,           1743.4,     0.643564356436},
      {53,        2018.36,          1288.63,                1.0},
      {35,        1971.18,           758.83,     0.241525423729},
      {33,        1971.18,           723.99,                1.0},
      {50,        1927.19,          1197.21,     0.482009504413},
      {40,        1927.19,           909.99,    0.0291921249151},
      {31,        1927.19,           714.66,                1.0},
      {47,        1911.31,           1180.8,     0.224043715847},
      {42,        1911.31,          1088.64,                1.0},
      {57,       1820.352,          1820.21,     0.128453038674},
      {54,       1820.352,          1582.51,     0.220994475138},
      {46,       1820.352,          1355.18,     0.310773480663},
      {38,       1820.352,          997.648,     0.201657458564},
      {29,       1820.352,          768.305,                1.0},
      {52,        1745.89,           1746.2,     0.145161290323},
      {50,        1745.89,          1559.86,     0.752688172043},
      {46,        1745.89,          1471.05,                1.0},
      {37,        1735.96,          1006.19,                1.0},
      {36,         1713.2,           983.21,               0.64},
      {23,         1713.2,           661.22,                1.0},
      {17,        1694.07,           482.69,                1.0},
      {45,         1678.0,          1678.19,     0.533515731874},
      {32,         1678.0,           948.09,     0.261285909713},
      {10,         1678.0,           334.03,     0.109439124487},
      {1,          1678.0,           100.15,                1.0},
      {29,        1577.34,           847.42,                1.0},
      {38,        1560.15,          1322.16,     0.873015873016},
      {6,         1560.15,            217.0,                1.0},
      {34,        1547.39,          1272.32,     0.494488188976},
      {31,        1547.39,          1161.63,     0.088188976378},
      {26,        1547.39,           956.74,                1.0},
      {35,       1439.408,          1439.37,     0.951417004049},
      {13,       1439.408,           422.01,                1.0},
      {33,       1344.012,           1343.4,    0.0241477272727},
      {26,       1344.012,           958.25,     0.255681818182},
      {22,       1344.012,          878.816,                1.0},
      {26,       1247.152,          972.108,                1.0},
      {29,       1212.515,          1212.52,     0.608963393774},
      {24,       1212.515,          937.507,                1.0},
      {23,        1158.09,           883.03,      0.46556122449},
      {16,        1158.09,           567.35,      0.21556122449},
      {9,         1158.09,           335.73,                1.0},
      {24,       1052.082,          1052.19,     0.639254738195},
      {21,       1052.082,           814.27,     0.253774494057},
      {13,       1052.082,           586.79,                1.0},
      {20,       1017.138,          831.196,     0.600088573959},
      {11,       1017.138,          551.953,     0.107617360496},
      {9,        1017.138,          426.372,     0.124003542958},
      {2,        1017.138,           194.47,                1.0},
      {11,        1008.49,           622.69,                1.0},
      {16,        822.691,          822.676,      0.60032772233},
      {14,        822.691,           636.68,    0.0436466557426},
      {8,         822.691,          436.912,     0.283032921198},
      {4,         822.691,          357.561,                1.0},
      {11,        729.824,          543.852,     0.904967602592},
      {7,         729.824,            454.8,                1.0},
      {6,          590.75,          315.711,                1.0},
      {9,         465.165,          465.152,     0.949744463373},
      {7,         465.165,           279.33,    0.0149063032368},
      {3,         465.165,            190.1,                1.0},
      {6,         385.782,          385.781,     0.969387755102},
      {4,         385.782,            199.9,                1.0},
      {4,          275.03,           274.92,    0.0137531668476},
      {2,          275.03,            89.18,                1.0},
      {2,         237.822,          237.821,                1.0},
      {1,         185.953,           186.01,                1.0},
      {0,             0.0,              0.0,                1.0},
    };

  int decay_num = sizeof(Decay)/sizeof(Decay[0]);
  //G4cout << "Decay Num = " << decay_num << G4endl;
  G4double energy = 0.;
  if(1){
    G4double p = G4UniformRand();
    //    G4cout << "p = " << p << G4endl;
    for (G4int j = 0; 0<= j && j < decay_num;){
      if (p < Decay[j].ratio){
        energy = Decay[j].gamma;
        //      G4cout << "energy = " << energy << G4endl;
        if (energy > 0.){
	  //       G4cout << "Setting energy " << energy << G4endl;
          particleGun->SetParticleEnergy(energy*keV);
          particleGun->SetParticleMomentumDirection(get_random_direction());
          // If energy > 0, a gamma is generated, but that doesn't mean
          // the cascade is finished!
          particleGun->GeneratePrimaryVertex(anEvent);
          //      G4cout <<"After generation" << G4endl;
	}
        // Only break out of loop when 'jump' == 0
        if (Decay[j].jump == 0) break;
        else j += Decay[j].jump;
        p = G4UniformRand();
      } else {
	p -= Decay[j].ratio;
	j++;
      }
    }
  }

  }

  else {
    G4cout << "No correct isotope specified." << G4endl;
  }



}

std::string EXOPrimaryGeneratorAction::GetListOfAvailableFortranGens()
{
  // Return a list of available fortran generators.  This is intended to
  // be used by the messenger.
  std::string retString = "";
  const EXOFortGen::GenMap& amap = EXOFortGen::GetFortranGenerators(); 
  EXOFortGen::GenMap::const_iterator iter = amap.begin();
  for (;iter != amap.end();iter++) retString += iter->first + " ";
  return retString;
  
}

bool EXOPrimaryGeneratorAction::FortGeneratorIsAvailable(const std::string& gen)
{
  // Return a list of available fortran generators.  This is intended to
  // be used by the messenger.
  const EXOFortGen::GenMap& amap = EXOFortGen::GetFortranGenerators(); 
  return (amap.find(gen) != amap.end());
  
}
