#ifndef EXOEventSummaryEDecay_hh
#define EXOEventSummaryEDecay_hh

#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOCalibUtilities/EXOEventSummary.hh"

class EXOEventSummaryEDecay: public EXOEventSummary
{

public:
  EXOEventSummaryEDecay();
  virtual ~EXOEventSummaryEDecay(){;}
  
  //additional observables
  Double_t energy_no_z_info; // event energy w/o z corrections

  std::vector<Int_t> cluster_num_v_wires;  // cluster number of v-wires
  std::vector<Int_t> cluster_num_u_wires;  // cluster number of u-wires
  std::vector<Double_t> cluster_energy_no_z_info; // cluster energy w/o z corrections
  std::vector<Double_t> cluster_uwire_sum_energy; // cluster energy of u-wires after gain correction
  std::vector<Double_t> cluster_vwire_sum_magnitude; // cluster magnitude of v-wires after gain correction

  //functions
  virtual void Clear();
  virtual void AddCluster(const EXOChargeCluster& cc, double energy);

protected:
  
  ClassDef(EXOEventSummaryEDecay,1)
};

#endif
