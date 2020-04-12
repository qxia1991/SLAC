#ifndef EXOATeamComptonModule_hh
#define EXOATeamComptonModule_hh

#include "EXOAnalysisManager/EXOTreeSaverModule.hh"
#include "EXOUtilities/EXOComptonInfo.hh"
#include <TVector3.h>
class TTree;
class EXOATeamComptonModule : public EXOTreeSaverModule 
{

public :

  EXOATeamComptonModule();
  ~EXOATeamComptonModule();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);

  int ShutDown();

protected:
  std::string GetTreeName() { return "compton"; }
  std::string GetTreeDescription() { return "Tree for Compton Data"; }
  void SetupBranches(TTree* tree);

private :
  EXOComptonInfo fCompton;  


  double ecl[3];
  double xcl[3];
  double ycl[3];
  double zcl[3];
  double mec;
  bool fidcl[3];
  bool tdcl[3];
  bool ghcl[3];
  double E_tot;
  double FOM;
  int ncl;
  TVector3 U;
  TVector3 V;
  int get_ncl();
  double get_E();
  double get_ecl(int i);
  double get_xcl(int i);
  double get_ycl(int i);
  double get_zcl(int i);
  bool get_fidcl(int i);
  bool get_tdcl(int i);
  bool get_ghcl(int i);
  void set_ncl(int i);
  void set_ecl(int i, double E);
  void set_xcl(int i, double X);
  void set_ycl(int i, double Y);
  void set_zcl(int i, double Z);
  void set_fidcl(int i, bool FID);
  void set_tdcl(int i, bool TD);
  void set_ghcl(int i, bool GH);
  void add_cluster(double E, double X, double Y, double Z, bool FID, bool TD, bool GH);
  int next(int v[]);
  double delta_E(double E);
  double get_phi_geo();
  double get_cos_phi_geo();
  double get_phi_com(int i);
  double get_cos_phi_com(int i);
  double get_sigma_geo();
  double get_sigma_com();
  double delta_phi_com();
  double delta_phi_geo();
  double delta_phi();
  double seq_FOM(int v[]);
  void Sequence(int v[]);
  void UnSequence(int v[]);
  void Order();
  int num_events;
  int num_2_sites;
  int num_3_sites;
  int pass_2_sites;
  int pass_3_sites;

  DEFINE_EXO_ANALYSIS_MODULE( EXOATeamComptonModule )

};
#endif

  


