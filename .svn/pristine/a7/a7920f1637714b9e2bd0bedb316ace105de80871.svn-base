///////////////////////////////////////////////////////
// EXOScintEventSummary
// Data class filled by EXOClusterCull
// Contains and provides information about charge
// clusters that survived culling (and those that were culled).
//
////////////////////////////////////////////////////////


#include "EXOCalibUtilities/EXOScintEventSummary.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOCalibUtilities/EXOFiducialVolume.hh"
#include "EXOCalibUtilities/EXODiagonalCut.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "TMath.h"
#include <limits>
#include <set>
#include <iostream>

using namespace std;

string EXOScintEventSummary::fDiagonalCutDBFlavor = "2013-0nu-denoised";
string EXOScintEventSummary::fFiducialCutDBFlavor = "2013-0nu";

EXOScintEventSummary::EXOScintEventSummary()
  : fScintCluster(0),
    fEventHeader(0),
    fPCDMetric(0), fMultipleChargeOnlyEvents(false)
{

}

void EXOScintEventSummary::SetDiagonalCutDBFlavor(const string val)
{
  fDiagonalCutDBFlavor = val;
}

void EXOScintEventSummary::SetFiducialVolumeDBFlavor(const std::string flavor)
{
  fFiducialCutDBFlavor = flavor;
}

void EXOScintEventSummary::AssertInitialization(bool requireSC) const
{
  if( not fEventHeader and (requireSC and not fScintCluster) ){
    LogEXOMsg("ScintEventSummary was not properly filled", EEError);
  }
}

double EXOScintEventSummary::GetReconstructedChargeEnergy() const
{
  //Return the summed energy over ALL charge clusters that survived culling.

  AssertInitialization(false);
  double energy = 0.0;

  if(fChargeDeposits.size()){//Mean's we're MC
    for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
      // Use best available measurement of charge-energy
      if((*iter)->fPurityCorrectedEnergy > 0)
        energy += (*iter)->fPurityCorrectedEnergy;
      else if((*iter)->fCorrectedEnergy > 0)
        energy += (*iter)->fCorrectedEnergy; //Use corrected energy for MC to account for channel dependent scaling factors in digitizer
      else
        energy += (*iter)->fRawEnergy;
    }
    return energy;
  }
  
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
      // Use best available measurement of charge-energy
      if ((*iter)->fPurityCorrectedEnergy > 0)
        energy += (*iter)->fPurityCorrectedEnergy;
      else if ((*iter)->fCorrectedEnergy > 0)
        energy += (*iter)->fCorrectedEnergy;
      else
        energy += (*iter)->fRawEnergy;
  }
  return energy;
}

double EXOScintEventSummary::GetReconstructedMixChargeEnergy() const
{
    //Get the Charge Energy for the Mixed Collection/Induction Fitting Correction
    AssertInitialization(false);
    double energy = 0.0;
    for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
        if((*iter)->fPurityCorrectedEnergyMix > 0)
            energy += (*iter)->fPurityCorrectedEnergyMix;
        else if((*iter)->fCorrectedEnergyMix > 0)
            energy += (*iter)->fCorrectedEnergyMix; 
        else
            energy += (*iter)->fRawEnergy;
    }
    return energy;
}

double EXOScintEventSummary::GetChargeInductionEnergy() const
{
  //Return magnitude of U-wire induction signals summed over ALL charge clusters that survived culling.

  AssertInitialization();
  double energy = 0.0;

  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
    energy += (*iter)->fInductionEnergy; //Use uncorrected induction magnitude, at least so far
  };

  return energy;
}

double EXOScintEventSummary::GetEnergyOfAssociatedPCDs() const
{
  //For MC, return the summed energy over all PCDs that have an 'associated' charge cluster which survived culling.
  AssertInitialization(false);
  double energy = 0.0;
  for(map<const EXOChargeCluster*, set<const EXOMCPixelatedChargeDeposit*> >::const_iterator outeriter = fChargeDeposits.begin(); outeriter != fChargeDeposits.end(); outeriter++){
    for(set<const EXOMCPixelatedChargeDeposit*>::iterator inneriter = outeriter->second.begin(); inneriter != outeriter->second.end(); inneriter++){
      energy += (*inneriter)->fTotalEnergy*1000;
    }
  }
  return energy;
}

double EXOScintEventSummary::GetEnergyOfNonAssociatedPCDs() const
{
  //For MC, return the summed energy over all PCDs that do NOT have an 'associated charge cluster that survived culling.
  AssertInitialization(false);
  double energy = 0.0;
  for(set<const EXOMCPixelatedChargeDeposit*>::const_iterator iter = fNonAssociatedChargeDeposits.begin(); iter != fNonAssociatedChargeDeposits.end(); iter++){
    energy += (*iter)->fTotalEnergy*1000;
  }
  return energy;
}

double EXOScintEventSummary::GetStandoff() const
{
  //Return the minimum standoff distance from the detector walls of all charge clusters that survived culling.
  //The detector walls in this case are the teflon reflector and the V-wire plane
  AssertInitialization();
  return min(REFLECTORINNERRAD - GetMaxRadius(), CATHODE_ANODE_y_DISTANCE - fabs(GetMaxZ()));
}

double EXOScintEventSummary::GetMaxRadius() const
{
  //Return the maximum radial position of all charge clusters that survived culling.
  AssertInitialization();
  double Rad = 0.0;
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){

    //ignore clusters with missing positions so that the standoff distance
    //calculation remains well-behaved for events with missing positions
    if( abs((*iter)->fX) > 900 || abs((*iter)->fY) > 900 ) continue;

     Rad = max(Rad, sqrt((*iter)->fX*(*iter)->fX + (*iter)->fY*(*iter)->fY));
  }
  return Rad;
}

double EXOScintEventSummary::GetMinZ() const
{
  // Return the Z position of the charge cluster with minimum |Z| which survived culling.
  // Note that the result is signed; if charge clusters are at +150mm and -170mm, we return +150.
  AssertInitialization();
  double signZ = 999.9;
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){

    //ignore clusters with missing positions so that the standoff distance
    //calculation remains well-behaved for events with missing positions
    if( abs((*iter)->fZ) > 900 ) continue;

    if(fabs(signZ) > fabs((*iter)->fZ))
     signZ = (*iter)->fZ;
  }
  return signZ;
}

double EXOScintEventSummary::GetMaxZ() const
{
  // Return the Z position of the charge clusters with maximum |Z| which survived culling.
  // Note that the result is signed; if charge clusters are at +150mm and -170mm, we return -170.
  AssertInitialization();
  double signZ = 0.;
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){

    //ignore clusters with missing positions so that the standoff distance
    //calculation remains well-behaved for events with missing positions
    if( abs((*iter)->fZ) > 900 ) continue;

    if(fabs(signZ) < fabs((*iter)->fZ))
     signZ = (*iter)->fZ;
  }
  return signZ;
}

int EXOScintEventSummary::GetNumSite() const
{
  //Return the number of charge clusters that survived culling with one exception:
  //When there is only one such charge cluster, and this cluster's number of U-wire signals
  //on different channels is > 2, then return 2 instead of one.
  AssertInitialization(false);
  if(fChargeClusters.size() > 1){
    return fChargeClusters.size();
  }
  if(fChargeClusters.size() == 0){
    return 0;
  }
  const EXOChargeCluster* cc = (*(fChargeClusters.begin()));
  //A std::set keeps only different elements.
  //Inserting a channel number that is already in the set won't increase its size.
  set<int> UChannels;  
  size_t nUWS = cc->GetNumUWireSignals();
  for(size_t i=0; i<nUWS; i++){
    UChannels.insert(cc->GetUWireSignalAt(i)->fChannel);
  }
  if(UChannels.size() > 2){
    return 2;
  }
  //ensure the U-wire channels are neighboring to eliminate cathode split events
  if(UChannels.size() == 2) {
    if(TMath::Abs(*(UChannels.begin()) - *(UChannels.rbegin())) > 1.1)  return 2;
  }
  return 1;
}
int EXOScintEventSummary::GetNumCollWires() const
{
  // Return the total number of U-wires with found signals (including induction)
  AssertInitialization(false);

  //A std::set keeps only different elements.
  //Inserting a channel number that is already in the set won't increase its size.
  set<int> UChannels;  
  
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++) {
    size_t nUWS = (*iter)->GetNumUWireSignals();
    for(size_t i=0; i<nUWS; i++) {
      UChannels.insert((*iter)->GetUWireSignalAt(i)->fChannel);
    }
  }
  // now return total number of unique channels found
  return UChannels.size();
}

int EXOScintEventSummary::GetNumVWires() const
{
  // Return the total number of V-wires with found signals
  AssertInitialization(false);

  // A std::set keeps only different elements.
  // Inserting a channel number that is already in the set won't increase its size.
  set<int> VChannels;

  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++) {
    size_t nVWS = (*iter)->GetNumVWireSignals();
    for(size_t i=0; i<nVWS; i++) {
      VChannels.insert((*iter)->GetVWireSignalAt(i)->fChannel);
    }
  }
  // now return total number of unique channels found
  return VChannels.size();
}

int EXOScintEventSummary::GetNumIndWires() const
{
  // Return the total number of U-wires with found signals (including induction)
  AssertInitialization(false);

  //A std::set keeps only different elements.
  //Inserting a channel number that is already in the set won't increase its size.
  set<int> UChannels;  
  
  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++) {
    size_t nUWI = (*iter)->GetNumUWireInductionSignals();
    for(size_t i=0; i<nUWI; i++) {
      Int_t cchan = (*iter)->GetUWireInductionSignalAt(i)->fChannel;
      // Remove offset added by reconstruction
      UChannels.insert( EXOReconUtil::kUWireIndOffset - cchan );
    }
  }
  // now return total number of unique channels found
  return UChannels.size();
}

double EXOScintEventSummary::GetMaxInd() const
{
   std::map<Int_t,Double_t> ind_sig;
   for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++) {
        size_t nUWI = (*iter)->GetNumUWireInductionSignals();
        for(size_t i=0; i<nUWI; i++) {
            Int_t cchan = (*iter)->GetUWireInductionSignalAt(i)->fChannel;
            cchan = EXOReconUtil::kUWireIndOffset - cchan;
            if (ind_sig.count(cchan) == 0){
                ind_sig.insert(std::pair<Int_t,Double_t>(cchan,0.));
            }
            ind_sig.at(cchan) += (*iter)->GetUWireInductionSignalAt(i)->fMagnitude;
        }
   }
   double maxSig    = 0;
   for(std::map<Int_t,Double_t>::iterator is = ind_sig.begin(); is != ind_sig.end(); is++){
        if (is->second > maxSig){
            maxSig = is->second;
        }
   }
   return maxSig;

}

double EXOScintEventSummary::GetFrac3DRecon() const
{

    if (fChargeClusters.size() == 0){
        return 0.0;
    }

    double totalE3D = 0;
    for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++) 
    {
        if ((*iter)->Is3DCluster()){
            totalE3D += (*iter)->fPurityCorrectedEnergy;
        }
    }
    double totalE = GetReconstructedChargeEnergy();
    double frac3d = totalE3D/totalE;
    return frac3d;
}

bool EXOScintEventSummary::IsFiducial() const
{
  //Return whether ALL of the charge clusters that survived culling are inside
  //the fiducial volume
  AssertInitialization();
  const EXOFiducialVolume* fidVol = GetCalibrationFor(
      EXOFiducialVolume,
      EXOFiducialVolumeHandler,
      fFiducialCutDBFlavor,
      *fEventHeader);

  for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
    if( not fidVol->IsFiducial(*(*iter))){
      return false;
    }
  }
  return true;
}

bool EXOScintEventSummary::HasMissingPositions(bool oldCCDef3d) const
{
  //Return whether a charge cluster was culled because of missing 2D reconstruction.
  AssertInitialization(false);
  if(oldCCDef3d)
  {
    for(set<const EXOChargeCluster*>::iterator iter = fCulledChargeClusters.begin(); iter != fCulledChargeClusters.end(); iter++){
      if( fabs((*iter)->fX) > 900 or fabs((*iter)->fY) > 900){
        return true;
      }
    }
  }
  else
  {
    for(set<const EXOChargeCluster*>::iterator iter = fChargeClusters.begin(); iter != fChargeClusters.end(); iter++){
      if( fabs((*iter)->fX) > 900 or fabs((*iter)->fY) > 900){
        return true;
      }
    }
  }
  return false;
}

bool EXOScintEventSummary::WasSomethingCulled() const
{
  //Return whether a charge cluster was culled
  AssertInitialization(false);
  return fCulledChargeClusters.size();
}

bool EXOScintEventSummary::PassesDiagonalCut() const
{
  //Return whether this scintillation event passes the diagonal cut
  AssertInitialization();
  const EXODiagonalCut* diag = GetCalibrationFor(
      EXODiagonalCut,
      EXODiagonalCutHandler,
      fDiagonalCutDBFlavor,
      *fEventHeader);

  double charge = GetReconstructedChargeEnergy();
  if(GetNumSite() == 1){
    return diag->SurvivesSingleSiteCut(fScintCluster->fRawEnergy,charge);
  }
  return diag->SurvivesMultiSiteCut(fScintCluster->fRawEnergy,charge);
}

double EXOScintEventSummary::GetSimulatedEnergyForCluster(const EXOChargeCluster* cc) const
{
  map<const EXOChargeCluster*, set<const EXOMCPixelatedChargeDeposit*> >::const_iterator element = fChargeDeposits.find(cc);
  if(element == fChargeDeposits.end()){
    return 0;
  }
  double energy = 0;
  for(set<const EXOMCPixelatedChargeDeposit*>::iterator iter=element->second.begin(); iter != element->second.end(); iter++){
    energy += (*iter)->fTotalEnergy*1000;
  }
  return energy;
}

