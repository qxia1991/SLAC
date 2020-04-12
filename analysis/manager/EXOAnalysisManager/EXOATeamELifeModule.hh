#ifndef EXOATeamELifeModule_hh
#define EXOATeamELifeModule_hh
#include "EXOAnalysisModule.hh"
#include "TH1.h"
#include "TROOT.h"
#include "TChain.h"

#define i_life_min 0
#define i_life_max 450

class EXOATeamELifeModule : public EXOAnalysisModule 
{

private :
  // TTree  *fChain;   //!pointer to the analyzed TTree or TChain
  //Int_t           fCurrent; //!current Tree number in a TChain

  TFile *f;

  // Declare common histograms
  TH1F * hsesum[i_life_max];
  TH1F * hsecl[i_life_max];
  TH1F * hsesum_chan[114][i_life_max]; // cover all u-channels
  TH1F * hsecl_chan[114][i_life_max]; // ditto
  TH1F * hma;
  TH1F * hsesum_z[40][i_life_max];
  TH1F * hsecl_z[40][i_life_max];

  // Declare lifetimeguesses 
  float lifetime[i_life_max];
  
  // Declare some common housekeeping variables
  double  fcut;
  double  zcut_l;
  double  zcut_u;
  double  pfft_low;
  double  pfft_high;
  bool    ecl_bool;
  
  double    fft_low;
  double    fft_high; 
  double    default_low;
  double    default_high;
  float     binwidth;
  std::string root_char;
  std::string iso_char;

  //Declare parameters
  Long64_t nbytes;
  Int_t nb;
  Int_t ientry;
  //  Int_t jentry;
  //this value is temporary until measured t0 is carried with the data (tsc?)
  float t0; //microseconds.
  float ecevent;
  float exp_arg;
  float e_cut;
  int bin_cut;
  int bin_low;
  int bin_high;
  int nbins;
  float h_low;
  float h_high;
  float hf_power[i_life_max];
  char fft_char[200];
  TH1F * hfft[i_life_max];
  int hist_int;
  //  int entries_max;

  // Declare FFT accessory functions
  void     fft(TH1 *h1);
  void     hcopy(char * h1name, char * h2name);

protected:

public :

  EXOATeamELifeModule();
  ~EXOATeamELifeModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);


  int TalkTo(EXOTalkToManager *tm);
  int ShutDown();

  void SetRootFile(std::string aval) { root_char = aval; }
  void SetSource(std::string aval) { iso_char = aval; }
  void Setfcut(double aval) { fcut = aval; }
  void Setzcut_l(double aval) { zcut_l = aval; }
  void Setzcut_u(double aval) { zcut_u = aval; }
  void Setpfft_low(double aval) { pfft_low = aval; }
  void Setpfft_high(double aval) { pfft_high = aval; }
  void Setecl_bool(bool aval) { ecl_bool = aval; }
 
  DEFINE_EXO_ANALYSIS_MODULE( EXOATeamELifeModule )

};
#endif

  


