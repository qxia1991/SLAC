#ifndef EXOEventSummary_hh
#define EXOEventSummary_hh

#include "TObject.h"
#include <vector>
#include <string>

#include <map>

class EXOChargeCluster;
class EXOMCPixelatedChargeDeposit;

class EXOChargeCluster;
class EXOMCPixelatedChargeDeposit;

class EXOEventSummary{

  public:
  EXOEventSummary();
  virtual ~EXOEventSummary(){;}

  //(potential) observables
  Double_t energy_ss; // if ss ->CalibratedRotatedEnergy else -999.9
  Double_t energy_ms; // if ms ->CalibratedRotatedEnergy else -999.9
  Double_t energy;    // if ss :  energy_ss ;  if ms : energy_ms
  Double_t standoff_distance; // min(REFLECTORINNERRAD - GetMaxRadius(),CATHODE_ANODE_y_DISTANCE - fabs(GetMaxZ()));
  // using MST_Metric -> default energy weighting is false
  Double_t u_mst_metric;         // MST for Xi(U,Z), 2D
  Double_t v_mst_metric;         // MST for Xi(V,Z), 2D
  Double_t u_and_v_mst_metric;   // u_mst_metric + v_mst_metric
  Double_t cluster_mst_metric;   // MST Xi(x,y,Z), 3D, over clusters (0 for ss)
  Double_t u_cluster_mst_metric; // MST Xi(U,y,Z), 3D, using V of parent charge cluster

  //(potential) observables not in MC
  Double_t veto_plus;   // time since last veto panel trigger
  Double_t veto_minus;  // time until next veto panel trigger
  Double_t muon_plus;   // positive time elapsed since last muon event -- used for isTPCMuonVeto 
  Double_t muon_minus;  // time remaining until next muon event
  Int_t    veto_multiplicity;  // number of veto panels triggered in most recent veto panel trigger


  //(potential) observables specific to MC
  Double_t energy_mc;     // unsmeared true energy (MC only)
  Double_t energy_rec_mc; // reconstructed MC energy (MC only)

  //other variables
  Double_t rotated_ss; //if ss ->RawRotatedEnergy else -999.9
  Double_t rotated_ms; //if ms ->RawRotatedEnergy else -999.9
  Double_t e_scint;    // scintillation energy. depends on fed flavor
  Double_t e_charge;  
  Double_t e_charge_ind;
  Double_t t_scint; // time of scintillation cluster relative to trigger
  Int_t time_sec;   // trigger timestamp of event
  Int_t runNum;     // run number
  Int_t eventNum;   // event number
  Int_t multiplicity; // event multiplicity
  Int_t nsc; // number of scintillation clusters
  Int_t numCCsWithoutScint; // number of charge clusters for which no scintillation cluster was found
  Int_t num_coll_wires; // total number of U-wires with colllection signals
  Int_t num_ind_wires; // total number of U-wires with induction signals 
  Int_t num_vwires;    // total number of V-wire signals
  Double_t frac3d;     // fraction of charge energy which is 3D reconed
  Double_t max_ind;    // magnitude of maxium U-Induction Signal
  Double_t dnn_var_raw;     //varaible from DNN Discrim (Raw WFs)
  Double_t dnn_var_recon;    //varaible from DNN Discrim (Recon WFs)

  //fit2d energy
  Double_t e2d_scint; // calibrated scintillation energy from MC-based 2D fits
  Double_t e2d_charge; // calibrated charge energy from MC-based 2D fits
  Double_t e2d_rotated; // calibrated rotated energy using MC-based 2D fit angle and both vars above
  Double_t e2d_dist; // distance between rotated e2d_rotated and (e2d_charge,e2d_scint) in units of orthogonal sigma

  //variables over the cluster.
  std::vector<Double_t> cluster_x; // charge cluster X position
  std::vector<Double_t> cluster_y; // charge cluster Y position
  std::vector<Double_t> cluster_z; // charge cluster Z position
  std::vector<Double_t> cluster_energy; // charge cluster energy
  std::vector<Double_t> cluster_ind_energy; // charge cluster energy of U-wire induction signals
  std::vector<Double_t> cluster_risetime; // charge cluster rise time
  std::vector<Int_t>    cluster_numwires; // number of wire signals in cluster
  std::vector<Double_t> cluster_maxUfrac; // fraction of charge energy in largest u-signal
  std::vector<Double_t> cluster_maxVfrac; // fraction of charge energy in largest v-signal
  std::vector<Double_t> cluster_maxVfrac12; // fraction of charge energy in largest v-signal
  std::vector<Double_t> cluster_vrms; // fraction of charge energy in largest v-signal
  std::vector<Double_t> cluster_maxVratio; //last try

  // Double_t disc ; // reserved variable for output of ML discriminator, default -2, and when filled [-1,1]  --> now no longer building discriminator in this, but rather just friending it over. 

  //other variables specific to MC (MC only)
  Double_t weight;    // event weight (MC only)
  Double_t primary_x; // primary event vertex (MC only)
  Double_t primary_y; // primary event vertex (MC only)
  Double_t primary_z; // primary event vertex (MC only)
  Double_t event_sizeR; // true event size (full distance)
  Double_t event_sizeU; // true event size (just U-Plane)
  Double_t event_sizeV; // true event size (just V-Plane)
  Double_t event_sizeZ; // true event size (just Z-Plane)
  std::vector<Double_t> pcd_x; // PCD X position (MC only)
  std::vector<Double_t> pcd_y; // PCD Y position (MC only)
  std::vector<Double_t> pcd_z; // PCD Z position (MC only)
  std::vector<Double_t> pcd_energy; // PCD true energy (MC only)
  std::vector<Int_t> pcd_num_ancestors; // Number of ancestors that contributed to this PCD (MC only)
  std::vector<Int_t> pcd_num_ancestor_types; // Number of ancestor types that contributed to this PCD (MC only)
  std::vector<Int_t> pcd_ancestor_type; // Ancestor id/type that contributed to this PCD. If more than one ancestor with same type, this type is store. If more than one ancestor type, that type in more number is stored. Again, if more than one here, then that with highest summed energy. (This info is probably not so useful in this case.) (MC only)
  std::vector<Double_t> pcd_ancestor_energy; // Sum of the energies from the same type of ancestor corresponding to the id stored in the 'pcd_ancestor_type' variabl (MC only)
  Int_t pcd_cc_metric; // metric used to associate CC and PCDs: U, V, UV, VZ or UZ (MC only)

  //flags
  Bool_t isCulled;
  Bool_t isMissingPosition;

  // flags not reproduced in MC
  Bool_t isVetoed; // isPanelMuonVeto && isTPCMuonVeto && isBlankedout && isTPCEvent (default -> can reduce to a subset) see EXOCoincidences 
  Bool_t isBlankedout;   // bad times veto (environment, fridge, run boundaries)
  Bool_t isPanelMuonVeto;// panel muon veto
  Bool_t isTPCMuonVeto;  // TPC muon veto
  Bool_t isTPCEvent;     // too close in time to another TPC event, default is symmetric +- 1 second 
  Bool_t hasSaturatedChannel; // poll this as part of what makes TPC events vetoable on if they have nsc == 0 

  Bool_t isSolicitedTrigger; // in physics data 0.1Hz solicited -> used for livetime calculation

  Bool_t isNoise; //as tagged by EXOAlphaNoiseTagger
  Bool_t isWithinDriftTime;
  Bool_t isNearOtherScintCluster; // is this scint cluster within drift length of another?
  Bool_t isReconNoise;  //did the recon noise tagger touch this event

  //Bool_t is137XeVeto; //137Xe Veto

  //flags specific to MC
  Bool_t doesSurviveEqualization; 
  Bool_t isTriggeredEvent; // if event was triggered, ie if at least 1 charge or scint cluster was found even if it does not pass culling

  //functions
  virtual void Clear();
  virtual void ClearScint();
  double GetMaxR() const;
  bool isFiducial(bool oldCCDef3d=false) const;
  bool isDiagonallyCut() const;
  std::string isFiducialStr(bool oldCCDef3d=false) const;
  bool isXe137VetoCut(double time_window, double spatial_ext, double e_min, double e_max, bool usez, bool usexy = false, bool req2d = true, bool ignoreH1 = false) const;
  bool randXe137VetoCut(double time_window, double spatial_ext, double e_min, double e_max, bool usez, bool usexy = false, bool req2d = true, bool ignoreH1 = false) const;
  bool mcXe137VetoCut(double prob, double spatial_ext, double e_min, double e_max, bool usez, bool usexy = false, bool req2d = true, bool ignoreH1 = false) const;

  virtual void AddPCD(const EXOMCPixelatedChargeDeposit& pcd);
  virtual void AddPCDAncestor(const EXOMCPixelatedChargeDeposit& pcd);
  virtual void AddCluster(const EXOChargeCluster& cc, double energy);
  virtual void UpdateSizeInfo(const EXOMCPixelatedChargeDeposit& pcd_i, const EXOMCPixelatedChargeDeposit& pcd_j);

  static void SetDiagonalCutDBFlavor(const std::string flavor);
  static void SetFiducialVolumeDBFlavor(const std::string flavor);
  static void SetXe137VetoDBFlavor(const std::string flavor);

  static std::string GetDiagonalCutDBFlavor() {return fDiagonalCutDBFlavor;}
  static std::string GetFiducialVolumeDBFlavor() {return fFiducialCutDBFlavor;}
  static std::string GetXe137VetoDBFlavor() {return fXe137VetoDBFlavor;}

  double GetLightWeightedSumPCDEnergy(double weight, double maxr = 173., double maxz = 192.);
  
  private:
  static std::string fDiagonalCutDBFlavor; //! database flavor for diagonal cut. Do not serialize.
  static std::string fFiducialCutDBFlavor; //! database flavor for fiducial volume cut. Do not serialize.
  static std::string fXe137VetoDBFlavor; //! database flavor for Xe137 veto cut. Do not serialize.

  ClassDef(EXOEventSummary,34) // increment whenever contents of what saved to disk is changed.
};

#endif
