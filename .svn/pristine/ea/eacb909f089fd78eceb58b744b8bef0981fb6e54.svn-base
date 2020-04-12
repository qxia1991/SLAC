#include "EXOCalibUtilities/EXOEventSummaryChargeOnly.hh"

EXOEventSummaryChargeOnly::EXOEventSummaryChargeOnly()
{
  Clear();
}

void EXOEventSummaryChargeOnly::Clear()
{
  EXOEventSummary::Clear();

  energy_no_z_info = 0;

  cluster_num_v_wires.clear();
  cluster_num_u_wires.clear();
  cluster_energy_no_z_info.clear();
  cluster_uwire_sum_energy.clear();
  cluster_vwire_sum_magnitude.clear();

  cluster_u.clear();
  cluster_v.clear();
}

void EXOEventSummaryChargeOnly::AddCluster(const EXOChargeCluster& cc, double energy)
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

  cluster_u.push_back(cc.fU);
  cluster_v.push_back(cc.fV);
}

bool EXOEventSummaryChargeOnly::IsU() const
{
  for(size_t cc = 0; cc < cluster_energy.size(); cc++)
    if(fabs(cluster_u.at(cc)) > 900)
      return false;
  return true;
}

bool EXOEventSummaryChargeOnly::IsV() const
{
  for(size_t cc = 0; cc < cluster_energy.size(); cc++)
    if(fabs(cluster_v.at(cc)) > 900)
      return false;
  return true;
}

bool EXOEventSummaryChargeOnly::IsZ() const
{
  for(size_t cc = 0; cc < cluster_energy.size(); cc++)
    if(fabs(cluster_z.at(cc)) > 900)
      return false;
  return true;
}

bool EXOEventSummaryChargeOnly::IsUV() const
{
  return IsU() and IsV();
}

bool EXOEventSummaryChargeOnly::IsUZ() const
{
  return IsU() and IsZ();
}

bool EXOEventSummaryChargeOnly::IsVZ() const
{
  return IsV() and IsZ();
}

bool EXOEventSummaryChargeOnly::IsUVZ() const
{
  return IsUV() and IsZ();
}
