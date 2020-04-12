#include "EXOCalibUtilities/EXOEventSummary.hh"
#include "EXOCalibUtilities/EXOFiducialVolume.hh"
#include "EXOCalibUtilities/EXODiagonalCut.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOXe137Veto.hh"
#include "EXOUtilities/EXOCoordinates.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"

#include <limits>
#include <algorithm>
#include <cmath>
#include <iostream>

ClassImp(EXOEventSummary)

using namespace std;

string EXOEventSummary::fDiagonalCutDBFlavor = "2013-0nu-denoised";
string EXOEventSummary::fFiducialCutDBFlavor = "2013-0nu";
string EXOEventSummary::fXe137VetoDBFlavor = "Run2abcNonDenoisedCosmo";

EXOEventSummary::EXOEventSummary()
{
  Clear();
}

void EXOEventSummary::SetDiagonalCutDBFlavor(const string flavor)
{
  fDiagonalCutDBFlavor = flavor;
}

void EXOEventSummary::SetFiducialVolumeDBFlavor(const std::string flavor)
{
  fFiducialCutDBFlavor = flavor;
}

void EXOEventSummary::SetXe137VetoDBFlavor(const std::string flavor)
{
  fXe137VetoDBFlavor = flavor;
}

void EXOEventSummary::Clear()
{
  ClearScint();

  time_sec = 0;
  runNum = 0;
  eventNum = 0;
  nsc = 0;
  numCCsWithoutScint = 0;

  weight = 0;
  primary_x = 0;
  primary_y = 0;
  primary_z = 0;
 
  event_sizeR = 0;
  event_sizeU = 0;
  event_sizeV = 0;
  event_sizeZ = 0;

  pcd_x.clear();
  pcd_y.clear();
  pcd_z.clear();
  pcd_energy.clear();
  pcd_num_ancestors.clear();
  pcd_num_ancestor_types.clear();
  pcd_ancestor_type.clear();
  pcd_ancestor_energy.clear();

  isTriggeredEvent = true;

  isVetoed = false;
  isBlankedout = false;
  isPanelMuonVeto = false;
  isTPCMuonVeto = false;
  isTPCEvent = false;
  hasSaturatedChannel = false; 

  isNoise = false;
  isSolicitedTrigger = false;
  isReconNoise = false;

  veto_plus = 0;
  veto_minus = 0;
  muon_plus = 1e20;
  muon_minus = 1e20;
  veto_multiplicity = 0;
}

void EXOEventSummary::ClearScint()
{
  energy = 0;
  energy_ss = 0;
  energy_ms = 0;
  standoff_distance = 0;
  u_mst_metric = 0;
  v_mst_metric = 0;
  cluster_mst_metric = 0;
  u_cluster_mst_metric = 0;
  u_and_v_mst_metric = 0;

  energy_mc = 0;
  energy_rec_mc = 0;

  rotated_ss = 0;
  rotated_ms = 0;
  e_scint = 0;
  e_charge = 0;
  e_charge_ind = 0;
  multiplicity = 0;
  t_scint = 0;
  num_coll_wires = 0;
  num_ind_wires = 0; 
  num_vwires    = 0;  
  frac3d        = 0;
  max_ind       = 0;
  dnn_var_raw       = 0;
  dnn_var_recon       = 0;

  e2d_scint = 0; 
  e2d_charge = 0; 
  e2d_rotated = 0; 
  e2d_dist = 0; 

  isCulled = false;
  isMissingPosition = false;
  isWithinDriftTime = true;
  //is137XeVeto = false;

  pcd_cc_metric = 0;

  doesSurviveEqualization = true;

  isNearOtherScintCluster = false;
    
  // Clear out charge clusters, so charge clusters correspond to their scint clusters
  cluster_x.clear();
  cluster_y.clear();
  cluster_z.clear();
  cluster_energy.clear();
  cluster_ind_energy.clear();
  cluster_risetime.clear();
  cluster_numwires.clear();
  cluster_maxUfrac.clear();
  cluster_maxVfrac.clear();
  cluster_maxVfrac12.clear();
  cluster_vrms.clear();
  cluster_maxVratio.clear();
}

double EXOEventSummary::GetMaxR() const
{
  double max = numeric_limits<double>::min();
  for(size_t i=0; i<cluster_x.size(); i++){
    double val = cluster_x[i]*cluster_x[i] + cluster_y[i]*cluster_y[i];
    if (val > max) max = val;
  }
  return sqrt(max);
}

bool EXOEventSummary::isFiducial(bool oldCCDef3d) const
{
  //Return whether all charge clusters after culling are fiducial,
  //ignoring clusters that do not have 3D positions (unless oldCCDef3d set true)
  const EXOFiducialVolume* fidVol = GetCalibrationFor(
      EXOFiducialVolume,
      EXOFiducialVolumeHandler,
      fFiducialCutDBFlavor,
      1348000000);

  for(size_t i=0; i<cluster_x.size(); i++){
    EXOCoordinates coord(EXOMiscUtil::kXYCoordinates,cluster_x[i],cluster_y[i],cluster_z[i],0.0);
    if(oldCCDef3d)
    {
      if(not fidVol->IsFiducial(coord))
        return false;
    }
    else
    {
      if( fabs(cluster_x[i])<900. and fabs(cluster_y[i])<900. and (not fidVol->IsFiducial(coord))){
        return false;
      }
    }
  }
  return true;
}

std::string EXOEventSummary::isFiducialStr(bool oldCCDef3d) const
{
  //Return whether all charge clusters after culling are fiducial,
  //ignoring clusters that do not have 3D positions (unless oldCCDef3d set true)
  const EXOFiducialVolume* fidVol = GetCalibrationFor(
      EXOFiducialVolume,
      EXOFiducialVolumeHandler,
      fFiducialCutDBFlavor,
      1348000000);

  if(oldCCDef3d)
  {
    return fidVol->GetFidVolString("Min$(abs(cluster_z))",
                                   "Max$(abs(cluster_z))",
                                   "Max$(abs(cluster_x))",
                                   "Max$(abs( 0.5*( ((cluster_z<=0) - (cluster_z>0))*cluster_x + sqrt(3.0)*cluster_y  ))",
                                   "Max$(abs( 0.5*( ((cluster_z>0) - (cluster_z<=0))*cluster_x + sqrt(3.0)*cluster_y  ))",
                                   "Max$(cluster_x*cluster_x + cluster_y*cluster_y)"                                   
                                   ); 
  }
  
  return fidVol->GetFidVolString("Min$(abs(cluster_z))",
                                 "Max$(abs(cluster_z))",
                                 "Max$(abs(cluster_x)*(abs(cluster_x)<900))",
                                 "Max$(abs( 0.5*( ((cluster_z<=0) - (cluster_z>0))*cluster_x*(abs(cluster_x)<900) + sqrt(3.0)*cluster_y*(abs(cluster_y)<900))  ))",
                                 "Max$(abs( 0.5*( ((cluster_z>0) - (cluster_z<=0))*cluster_x*(abs(cluster_x)<900) + sqrt(3.0)*cluster_y*(abs(cluster_y)<900))  ))",
                                 "Max$(cluster_x*cluster_x*(abs(cluster_x)<900) + cluster_y*cluster_y*(abs(cluster_y)<900))"
                                 );
}


bool EXOEventSummary::isDiagonallyCut() const
{
  //Return whether event is cut by the diagonal cut
  const EXODiagonalCut* diag = GetCalibrationFor(
      EXODiagonalCut,
      EXODiagonalCutHandler,
      fDiagonalCutDBFlavor,
      1348000000);

  if(multiplicity > 1){
    return !diag->SurvivesMultiSiteCut(e_scint,e_charge);
  }
  return !diag->SurvivesSingleSiteCut(e_scint,e_charge);
}

bool EXOEventSummary::randXe137VetoCut(double time_window, double spatial_ext, double e_min, double e_max, bool usez, bool usexy, bool req2d, bool ignoreH1) const
{
  // Return whether event is cut by the Xe137 veto
  // If set to use volume, by randomization a fraction of volume / FV is vetoed
  
  const EXOXe137Veto* xe137veto = GetCalibrationFor(
      EXOXe137Veto,
      EXOXe137VetoHandler,
      fXe137VetoDBFlavor,
      1348000000);

  EXOXe137Veto().SetEnergyWindow(e_min,e_max);
  EXOXe137Veto().Require2DCluster(req2d);
  EXOXe137Veto().SetXe137VetoCuts(time_window,spatial_ext,usez,usexy);    
  EXOXe137Veto().IgnoreH1Line(ignoreH1);

  const EXOFiducialVolume* fidVol = GetCalibrationFor(
    EXOFiducialVolume,
    EXOFiducialVolumeHandler,
    fFiducialCutDBFlavor,
    1348000000);

  return xe137veto->RandXe137Vetoed(time_sec,*fidVol);  
}

bool EXOEventSummary::isXe137VetoCut(double time_window, double spatial_ext, double e_min, double e_max, bool usez, bool usexy, bool req2d, bool ignoreH1) const
{
  // Return whether event is cut by the Xe137 veto
  
  const EXOXe137Veto* xe137veto = GetCalibrationFor(
      EXOXe137Veto,
      EXOXe137VetoHandler,
      fXe137VetoDBFlavor,
      1348000000);

  EXOXe137Veto().SetEnergyWindow(e_min,e_max);
  EXOXe137Veto().Require2DCluster(req2d);
  EXOXe137Veto().SetXe137VetoCuts(time_window,spatial_ext,usez,usexy);    
  EXOXe137Veto().IgnoreH1Line(ignoreH1);
  
  // will not include missing positions (though this is useless if this cut is applied to data)
  std::vector<double> cluster_x_2d;
  std::vector<double> cluster_y_2d;

  for(size_t i = 0; i < cluster_x.size(); i++)
  {
    double x = cluster_x.at(i);
    double y = cluster_y.at(i);
    if(x > -900 && y > -900)
    {
      cluster_x_2d.push_back(x);
      cluster_y_2d.push_back(y);
    }
  }

  return xe137veto->IsXe137Vetoed(time_sec,cluster_z,cluster_y_2d,cluster_x_2d);
}

bool EXOEventSummary::mcXe137VetoCut(double prob, double spatial_ext, double e_min, double e_max, bool usez, bool usexy, bool req2d, bool ignoreH1) const
{
  // Return whether MC event is cut by the Xe137 veto, prob represent probability to pass time cut
  
  const EXOXe137Veto* xe137veto = GetCalibrationFor(
    EXOXe137Veto,
    EXOXe137VetoHandler,
    fXe137VetoDBFlavor,
    1348000000);

  EXOXe137Veto().SetEnergyWindow(e_min,e_max);
  EXOXe137Veto().Require2DCluster(req2d);
  EXOXe137Veto().SetXe137VetoSpatialCuts(spatial_ext,usez,usexy);
  EXOXe137Veto().IgnoreH1Line(ignoreH1);
  
  // will not include missing positions (though this is useless if this cut is applied to MC)
  std::vector<double> cluster_x_2d;
  std::vector<double> cluster_y_2d;

  for(size_t i = 0; i < cluster_x.size(); i++)
  {
    double x = cluster_x.at(i);
    double y = cluster_y.at(i);
    if(x > -900 && y > -900)
    {
      cluster_x_2d.push_back(x);
      cluster_y_2d.push_back(y);
    }
  }
  
  return xe137veto->MCXe137Vetoed(prob,cluster_z,cluster_y_2d,cluster_x_2d);  
}

void EXOEventSummary::AddPCDAncestor(const EXOMCPixelatedChargeDeposit& pcd)
{
  std::map<Int_t,Int_t> numAncestorType;
  std::map<Int_t,Double_t> energyAncestorType;

  size_t ancestorTypeLength = pcd.fAncestorParticleType.size();
  size_t ancestorEnergyLength = pcd.fAncestorParticleEnergy.size();
  size_t length = std::min(ancestorTypeLength,ancestorEnergyLength);
  
  Int_t maxNum = 0;
  for(size_t i = 0; i < length; i++)
  {
    Int_t ancestorType = pcd.fAncestorParticleType.at(i);
    Double_t ancestorEnergy = pcd.fAncestorParticleEnergy.at(i);
    
    if(numAncestorType.count(ancestorType) == 0)
      numAncestorType.insert(std::pair<Int_t,Int_t>(ancestorType,0));
    if(energyAncestorType.count(ancestorType) == 0)
      energyAncestorType.insert(std::pair<Int_t,Double_t>(ancestorType,0.));

    numAncestorType.at(ancestorType) += 1;
    energyAncestorType.at(ancestorType) += ancestorEnergy;

    if(numAncestorType.at(ancestorType) > maxNum)
      maxNum = numAncestorType.at(ancestorType);
  }

  bool detAncestor = false;
  Int_t ancestor = 0;
  Double_t maxEnergy = 0.;
  for(std::map<Int_t,Int_t>::iterator nat = numAncestorType.begin(); nat != numAncestorType.end(); nat++)
  {
    if(nat->second == maxNum)
    {
      if(energyAncestorType.at(nat->first) > maxEnergy)
      {
        detAncestor = true;
        ancestor = nat->first;
        maxEnergy = energyAncestorType.at(nat->first);
      }
    }
  }

  if(detAncestor)
  {
    pcd_ancestor_type.push_back(ancestor);
    pcd_ancestor_energy.push_back(maxEnergy);
    pcd_num_ancestors.push_back(pcd.fAncestorParticleType.size());
    pcd_num_ancestor_types.push_back(numAncestorType.size());
  }
}

void EXOEventSummary::AddPCD(const EXOMCPixelatedChargeDeposit& pcd)
{
  pcd_x.push_back(pcd.GetPixelCenter().GetX());
  pcd_y.push_back(pcd.GetPixelCenter().GetY());
  pcd_z.push_back(pcd.GetPixelCenter().GetZ());
  pcd_energy.push_back(pcd.fTotalIonizationEnergy*1000.);

  AddPCDAncestor(pcd);  
}

void EXOEventSummary::UpdateSizeInfo(const EXOMCPixelatedChargeDeposit& pcd_i, const EXOMCPixelatedChargeDeposit& pcd_j)
{
    Double_t xi = pcd_i.GetPixelCenter().GetX();
    Double_t yi = pcd_i.GetPixelCenter().GetY();
    Double_t zi = pcd_i.GetPixelCenter().GetZ();
    Double_t ui = pcd_i.GetPixelCenter().GetU();
    Double_t vi = pcd_i.GetPixelCenter().GetV();

    Double_t xj = pcd_j.GetPixelCenter().GetX();
    Double_t yj = pcd_j.GetPixelCenter().GetY();
    Double_t zj = pcd_j.GetPixelCenter().GetZ();
    Double_t uj = pcd_j.GetPixelCenter().GetU();
    Double_t vj = pcd_j.GetPixelCenter().GetV();


    Double_t pcd_sep = (xi - xj)*(xi-xj) + (yi - yj)*(yi-yj) + (zi - zj)*(zi-zj);
    pcd_sep  = sqrt(pcd_sep);
    
    Double_t pcd_sepU = abs(ui - uj);
    Double_t pcd_sepV = abs(vi - vj);
    Double_t pcd_sepZ = abs(zi - zj);

    if (pcd_i.fTotalIonizationEnergy*1.e3 > 20 and pcd_j.fTotalIonizationEnergy*1.e3 > 20){
        //High Enough Energy to include it
        if(pcd_i.fDepositChannel >= 0 and pcd_j.fDepositChannel >= 0){
            //Also actually hit a channel so could have detected it
            if(pcd_sep > event_sizeR){
                event_sizeR = pcd_sep;
            }      
            if (pcd_sepU > event_sizeU){
                event_sizeU = pcd_sepU;
            } 
            if (pcd_sepV > event_sizeV){
                event_sizeV = pcd_sepV;
            }
            if (pcd_sepZ > event_sizeZ){
                event_sizeZ = pcd_sepZ;
            }
        }
    }

}

void EXOEventSummary::AddCluster(const EXOChargeCluster& cc, double e)
{
  cluster_x.push_back(cc.fX);
  cluster_y.push_back(cc.fY);
  cluster_z.push_back(cc.fZ);
  cluster_energy.push_back(e);
  cluster_ind_energy.push_back(cc.fInductionEnergy);
  cluster_risetime.push_back(cc.fSumRisetime);
  std::set<Int_t> channel_set;
  cluster_vrms.push_back(cc.fVRMSFoundSignals);

  std::map<Int_t,Double_t> ch_energy_set;

  for (size_t i = 0; i < cc.GetNumUWireSignals(); i++)
  {
      channel_set.insert(cc.GetUWireSignalAt(i)->fChannel);
    
      //std::cout << "iterate before end " << cc.GetUWireSignalAt(i)->fChannel << "  " 
      //          << cc.GetUWireSignalAt(i)->fIsInduction << "  " << cc.GetUWireSignalAt(i)->fCorrectedEnergy << std::endl;

      if (ch_energy_set.count(cc.GetUWireSignalAt(i)->fChannel) == 0){
        ch_energy_set.insert(std::pair<Int_t,Double_t>(cc.GetUWireSignalAt(i)->fChannel,0.));
      }
      if (not cc.GetUWireSignalAt(i)->fIsInduction){
        ch_energy_set.at(cc.GetUWireSignalAt(i)->fChannel) += cc.GetUWireSignalAt(i)->fCorrectedEnergy;
      }

  }

  //Find the largest U-Signal and the sum U-Energy (pre corrections)
  double sigSum    = 0;
  double maxSig    = 0;
  for(std::map<Int_t,Double_t>::iterator ces = ch_energy_set.begin(); ces != ch_energy_set.end(); ces++)
  {
      //std::cout << "iterate after end " << ces->first  << "  " << ces->second << std::endl;
      sigSum += ces->second;
      if (ces->second > maxSig){
        maxSig = ces->second;
      }
  }
  
  cluster_numwires.push_back(channel_set.size());
  if (cc.GetNumUWireSignals() > 0){
    cluster_maxUfrac.push_back(maxSig/sigSum);
  }
  else{
    cluster_maxUfrac.push_back(0.0);
  }

  //-----------------------------------------------------------------
  //Now do the V-Wire stuff
  std::map<Int_t,Double_t> ch_venergy_set;   
  for (size_t i = 0; i < cc.GetNumVWireSignals(); i++)
  {
      if (ch_venergy_set.count(cc.GetVWireSignalAt(i)->fChannel) == 0){
        ch_venergy_set.insert(std::pair<Int_t,Double_t>(cc.GetVWireSignalAt(i)->fChannel,0.));
      }
      ch_venergy_set.at(cc.GetVWireSignalAt(i)->fChannel) += cc.GetVWireSignalAt(i)->fCorrectedMagnitude;

  }
  double sigSumV    = 0;
  double maxSigV    = 0;
  int    maxCh      = 0;
  for(std::map<Int_t,Double_t>::iterator ces = ch_venergy_set.begin(); ces != ch_venergy_set.end(); ces++)
  {
      //std::cout << "iterate after end " << ces->first  << "  " << ces->second << std::endl;
      sigSumV += ces->second;
      if (ces->second > maxSigV){
        maxSigV = ces->second;
        maxCh   = ces->first;
      }
  }
 
  double maxSigV2   = 0; 
  for(std::map<Int_t,Double_t>::iterator ces = ch_venergy_set.begin(); ces != ch_venergy_set.end(); ces++)
  {
      if (ces->second > maxSigV2 and ces->first != maxCh){
          maxSigV2 = ces->second;
      }
  }


  if (cc.GetNumVWireSignals() > 0){
    cluster_maxVfrac.push_back(maxSigV/sigSumV);
    cluster_maxVfrac12.push_back(maxSigV/(maxSigV + maxSigV2));
    if (cc.GetNumUWireSignals() > 0){
        //use best energy
        if (cc.fPurityCorrectedEnergy>0)
        {
            cluster_maxVratio.push_back(maxSigV/cc.fPurityCorrectedEnergy); //ratio of largest V signal normalized by cluster_energy
        }
        else{
            cluster_maxVratio.push_back(maxSigV/cc.fCorrectedEnergy);
        }
    }
    else{
        cluster_maxVratio.push_back(0.0);
    }
  }
  else{
     cluster_maxVfrac.push_back(0.0);
     cluster_maxVfrac12.push_back(0.0);
     cluster_maxVratio.push_back(0.0);
  }

  

}

double EXOEventSummary::GetLightWeightedSumPCDEnergy(double weight, double maxr, double maxz)
{
  double total = 0.;
  for(size_t i = 0; i < pcd_energy.size(); i++)
  {
    double r = sqrt(pcd_x[i]*pcd_x[i]+pcd_y[i]*pcd_y[i]);
    double z = pcd_z[i];
    double ene = pcd_energy[i];

    if(r > maxr || std::abs(z) > maxz)
      ene *= weight;

    total += ene;
  }
  return total;
}
