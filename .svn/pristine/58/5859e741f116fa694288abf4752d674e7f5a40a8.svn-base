#ifndef EXOEventSummaryChargeOnly_hh
#define EXOEventSummaryChargeOnly_hh

#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOCalibUtilities/EXOEventSummary.hh"

class EXOEventSummaryChargeOnly: public EXOEventSummary
{

public:
  EXOEventSummaryChargeOnly();
  virtual ~EXOEventSummaryChargeOnly(){;}
  
  //additional observables
  Double_t energy_no_z_info; // event energy w/o z corrections

  std::vector<Int_t> cluster_num_v_wires;  // cluster number of v-wires
  std::vector<Int_t> cluster_num_u_wires;  // cluster number of u-wires
  std::vector<Double_t> cluster_energy_no_z_info; // cluster energy w/o z corrections
  std::vector<Double_t> cluster_uwire_sum_energy; // cluster energy of u-wires after gain correction
  std::vector<Double_t> cluster_vwire_sum_magnitude; // cluster magnitude of v-wires after gain correction

  std::vector<Double_t> cluster_u; // charge cluster U position
  std::vector<Double_t> cluster_v; // charge cluster V position

  Bool_t hasMultipleChargeOnlyEvents;

  //functions
  virtual void Clear();
  virtual void AddCluster(const EXOChargeCluster& cc, double energy);

  bool IsU() const;
  bool IsV() const;
  bool IsZ() const;
  bool IsUV() const;
  bool IsUZ() const;
  bool IsVZ() const;
  bool IsUVZ() const;
  bool IsXY() const {return IsUV();};
  bool IsXYZ() const {return IsUVZ();};
  
protected:
  
  ClassDef(EXOEventSummaryChargeOnly,1)
};

#endif
