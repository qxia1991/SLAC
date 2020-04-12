
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


//
// **********************************************************************

#ifndef EXOPrimaryGeneratorAction_h
#define EXOPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "EXOSim/EXOPrimaryGeneratorMessenger.hh"
#include <set>
#include <string>

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;
class EXOPrimaryGeneratorMessenger;
class G4ParticleDefinition;
class TTree;
class TEntryList;
class EXOMCTrackInfo;
class TGraph;
#ifdef HAVE_ROOT
#include <Rtypes.h> // for Long64_t and Int_t
#else
// substitute something reasonable
typedef long Long64_t;
typedef int Int_t;
#endif

class EXOPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  EXOPrimaryGeneratorAction();
  ~EXOPrimaryGeneratorAction();

public:
  void GeneratePrimaries(G4Event* anEvent);

  void set_generator( G4String value );
  void generate_bb_spectralIndex(G4Event *anEvent, G4int index);
  void generate_bb_kineticEnergies(G4Event *anEvent, G4double T1, G4double T2);
  void generate_bb2n_iachello( G4Event *anEvent );
  void generate_bb0n_NME( G4Event *anEvent );
  void generate_excitedStateGammas( G4Event *anEvent );
  void generate_fortWithName( const std::string &genName, G4Event *anEvent );
  void generate_muon( G4Event *anEvent );
  void generate_trackinfo( G4Event *anEvent );
  void generate_dicebox( G4Event *anEvent );
  void generate_nCaptureXe134(G4Event *anEvent);
  void generate_nCaptureXe136(G4Event *anEvent, G4int mode);
  void generate_nCaptureCu(G4Event *anEvent, G4int isotope, G4String);
  void generate_nCaptureF(G4Event *anEvent);

  void calc_norm(G4int index);
  void setup_iachello();
  void setup_NME(G4String generator);
  void setup_FF_GraphValues(G4String calcRadius);
  G4double * GetFFGraphX(){ return GraphFFX;}
  G4double * GetFFGraphY(){ return GraphFFY;}
  G4int GetnFFGraphPoints() { return nFFGraphPoints; }
  G4bool GetStatusUseGraphFFValues(){return use_graph_ff_values;}

  void set_isotope( G4String value );
  void set_atomic_number( G4int value );
  void set_majoron_spectral_index( G4int value);
  void set_use_graph_ff_values(G4bool value);
  void set_radius_ff_calc(G4String value);
  void set_q_value( G4double value );
  void use_beta_plus( G4bool value );
  void set_mwe_depth( G4double value );
  void set_Cu_isotope( G4int value);
  void set_Cu_Component( G4String value);
  void use_XeCaptureMode( G4int value );

  void SetTrackSourceName(const std::string& v) { fTrackSource = v; ReopenTrackSource(); }
  const std::string GetTrackSourceName() const { return fTrackSource; }
  void SetTrackTreeName(const std::string& v) { fTreeName = v; ReopenTrackSource(); }
  const std::string GetTrackTreeName() const { return fTreeName; }
  void SetTrackSelection(const std::string& v) { fTrackSelect = v; ReopenTrackSource(); }
  const std::string GetTrackSelection() const { return fTrackSelect; }
  void SetTrackModeLast(G4bool v) { fModeLast = v; }
  G4bool GetTrackModeLast() const { return fModeLast; }
  void SetEventSourceName(const std::string& v);
  const std::string GetEventSourceName() const { return fEventSource; }
  
  std::string GetListOfAvailableFortranGens();

protected:

  G4double D_spectrum ( G4double K, G4double D, G4int index);
  G4double D_spectrum_iachello ( G4double K, G4double D);
  G4double D_spectrum_NME( G4double K, G4double D);
  G4double D_spectrum_max(G4double K, G4int index);
  G4double sum_spectrum( G4double K, G4int index, G4double &D_spectral_max );
  G4double sum_spectrum_iachello( G4double K, G4double &D_spectral_max );
  G4double sum_spectrum_NME( G4double K, G4double &D_spectral_max );
  G4double sum_spectrum( G4double K, G4int index );

  G4double muon_angular_distribution(G4double theta);

  G4ThreeVector get_random_position(int mode = 0);
  G4ThreeVector get_random_direction();
  G4ThreeVector get_2nd_electron_direction(G4ThreeVector r0,
					   G4double T1,
					   G4double T2);

  G4double fermi_function( G4int Z, G4double KE );
  G4double fermi_function_GraphValues( G4double KE );

  G4double SimpsonsRule(G4double x0, G4double xn, G4int N, G4double f[]);
  bool     FortGeneratorIsAvailable(const std::string& gen);
  void     ReopenTrackSource(bool lazy = true);



private:
  G4GeneralParticleSource* generalParticleSource;
  G4ParticleGun*           particleGun; 
  EXOPrimaryGeneratorMessenger  gunMessenger; //messenger of this class
  G4String isotope;
  G4String generator; 
  G4ParticleDefinition *particle_used;
  G4ParticleDefinition *aBeta; 
  G4ParticleDefinition *aBetaPlus;
  G4ParticleDefinition *aMuonPlus;
  G4ParticleDefinition *aMuonMinus;
  G4ParticleDefinition *aGamma;
  G4double q_value; 
  G4int ff_factor;
  G4bool norm_calculated;
  G4double normalization;
  G4double K_spectral_max;
  G4int CuIsotope;
  G4String CuComponent;
  G4int XeCaptureMode;
  G4int atomic_number;
  G4int majoron_spectral_index;
  G4bool use_graph_ff_values;
  G4String radius_of_ff_calc;
  G4bool generate_gamma;
  G4int n_gamma;
  G4double gammaE[2]; //currently set to 2 for 2 gammas in xe-136 decay to excited states 
  G4double iachello_differential_spectrum_array[247][247];
  G4double iachello_sum_spectrum_array[247];
  G4double K_spectral_max_iachello;
  G4int N_iachello;
  G4double iachello_bin_width;
  G4double NME_differential_spectrum_array[247][247];
  G4double NME_sum_spectrum_array[247];
  G4double K_spectral_max_NME;
  G4int N_NME;
  G4double NME_bin_width;
  G4double mwe_depth;
  G4double muon_ang_max;
  G4double * GraphFFX;
  G4double * GraphFFY;
  G4int nFFGraphPoints;
  
  // trackinfo generator
  std::string fTrackSource; // Name of a place where MCTrackInfo tree is
  std::string fTreeName;    // Name of that tree
  TTree*      fTree;        // ROOT tree handle
  bool        fTreeFailed;  // do not try to find it
  std::string fTrackSelect; // Tree selection criteria, as for Draw() and such
  TEntryList *fList;        // Selection list if any
  EXOMCTrackInfo *fStore;   // pointer to current entry, we don't own it
  Long64_t    fIEntry;      // current entry number within tree
  Int_t       fEventID;     // cached event ID, used to distinguish events
  int         fEventNum;    // used to calculate number of unique events in source
  int         fLoopMode;    // -1
  bool        fModeLast;    // Use track parameters at last point instead of at vertex

  // dicebox generator
  std::string fEventSource; // Name of a text file with events dump from dicebox
  std::ifstream *fFile;
  //int fEventNum;             // shared with trackinfo
  
};

#endif



