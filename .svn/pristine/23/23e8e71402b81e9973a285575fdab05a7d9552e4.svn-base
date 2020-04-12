#ifndef EXOAPDPosCorrModule_hh
#define EXOAPDPosCorrModule_hh

#include <string>
#include <vector>
#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "TString.h"

class EXOScintillationCluster;
class EXOEventHeader;
class TH3F;
class TFile;

class EXOAPDPosCorrModule : public EXOAnalysisModule 
{

private :

  void normalize_corrections();

  Int_t get_low_r_neighbor(Double_t r);
  Int_t get_low_phi_neighbor(Double_t phi);
  Int_t get_low_z_neighbor(Double_t z);

  Double_t get_r_weighting(Double_t r, Int_t n_r_low);
  Double_t get_phi_weighting(Double_t phi, Int_t n_phi_low);
  Double_t get_z_weighting(Double_t z, Int_t n_z_low);

  Double_t get_signal(EXOScintillationCluster *sc, Double_t z);
  Double_t get_signal_error(EXOScintillationCluster *sc, Double_t z);

  //Double_t get_bin_correction(Int_t n_r, Int_t n_z, Int_t n_phi);
  Double_t get_correction(Double_t r, Double_t z, Double_t phi, Double_t &erro);

  Double_t get_multicluster_correction(EXOScintillationCluster *sc, Double_t &error);

  void compute_z_axis_lightmap();
  void load_lightmap_from_file(std::string filename);
  void load_lightmap_from_database(const EXOEventHeader &header);


  TFile *lm_file;
  TH3F *h_lightmap;
  TH3F *h_errors;
  TH3F *h_counts;
  std::vector<Double_t> *z_axis_lightmap;
  std::vector<Double_t> *z_axis_errors;

  TString signal_to_use;


  std::string lightmap_filename;
  std::string lightmap_database_flavor;
  bool lightmap_skip_correction;

protected:

public :
  EXOAPDPosCorrModule();
  int Initialize();
  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *talktoManager);
  int ShutDown();

  void set_lightmap_filename(std::string filename);
  void set_lightmap_database_flavor(std::string flavor);
  void set_lightmap_skip_correction(bool val);

  DEFINE_EXO_ANALYSIS_MODULE( EXOAPDPosCorrModule )

};
#endif
