#include "EXOCalibUtilities/EXOEventSummaryEDecay.hh"

EXOEventSummaryEDecay::EXOEventSummaryEDecay()
{
  Clear();
}

void EXOEventSummaryEDecay::Clear()
{
  EXOEventSummary::Clear();

  energy_no_z_info = 0;

  cluster_num_v_wires.clear();
  cluster_num_u_wires.clear();
  cluster_energy_no_z_info.clear();
  cluster_uwire_sum_energy.clear();
  cluster_vwire_sum_magnitude.clear();
}

void EXOEventSummaryEDecay::AddCluster(const EXOChargeCluster& cc, double energy)
{
  EXOEventSummary::AddCluster(cc, energy);

  cluster_num_v_wires.push_back(cc.GetNumVWireSignals());
  cluster_num_u_wires.push_back(cc.GetNumUWireSignals());
  cluster_energy_no_z_info.push_back(0.);

  double sumU = 0.;
  for(size_t i = 0; i < cc.GetNumUWireSignals(); i++)
    sumU += cc.GetUWireSignalAt(i)->fCorrectedEnergy;
  cluster_uwire_sum_energy.push_back(sumU);

  double sumV = 0.;
  for(size_t i = 0; i < cc.GetNumVWireSignals(); i++)
    sumV += cc.GetVWireSignalAt(i)->fCorrectedMagnitude;  
  cluster_vwire_sum_magnitude.push_back(sumV);
}
