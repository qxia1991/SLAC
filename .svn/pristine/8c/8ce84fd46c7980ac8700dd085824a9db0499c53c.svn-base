///////////////////////////////////////////////////////
// EXOClusterCull
// Selects charge clusters associated to a scintillation
// and culls charge clusters that lie outside a specific
// radius.
////////////////////////////////////////////////////////

#include "EXOCalibUtilities/EXOClusterCull.hh"
#include "EXOUtilities/EXOCoordinates.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOEnergyCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <limits>


using namespace std;

//_____________________________________________________________________________
EXOClusterCull::EXOClusterCull()
: fChargeClusterThreshold(0.0),
  //fRadialCull(REFLECTORINNERRAD),
  //fRadialCullMC(REFLECTORINNERRAD)
  fRadialCull(183.),
  fRadialCullMC(183.),
  //fClusterAcceptanceRadius(50.),
  fExtraUChannelAcceptance(1.),
  fExtraVChannelAcceptance(1.),
  fMaxDriftSamples(120),
  fMaxPCDTime(3.5),
  fDebugFilename("ClusterCullDebug.root"),
  fFillDebugTree(false),
  fDebugFile(0),
  fDebugTree(0),
  fDebugPCD(0),
  fDebugDistance(0),
  fDebugMissingCCEnergy(0)
{

}
//_____________________________________________________________________________
EXOClusterCull::~EXOClusterCull()
{
  if(fDebugFile){
    fDebugFile->Close(); //Closing the file deletes the tree if i remember correctly
    fDebugTree = 0;
  }
  delete fDebugFile;
  fDebugPCD = 0;
}
//_____________________________________________________________________________
EXOScintEventSummary EXOClusterCull::ClusterCull(const EXOScintillationCluster *sc, const EXOEventData *ED, bool oldDef3d) const
{
  // Select charge clusters for the given scintillation cluster
  // and decide which charge clusters need to be culled.
  // Return an EXOScintEventSummary object which stores references
  // to the selected clusters (and knows which charge clusters were culled).
  // For Monte Carlo data this function also associates PCDs with
  // charge clusters that survived culling.
  // If no scintillation cluster is given, then consider it a charge-only "scintillation" event:
  // ie. as if all CCs were associated to an inexistent scintillation cluster

  //Create the return object
  EXOScintEventSummary returnCollection;
  returnCollection.fScintCluster = sc;
  returnCollection.fEventHeader = &ED->fEventHeader;
  
  //Find charge clusters that survive culling
  size_t ncl = sc ? sc->GetNumChargeClusters() : ED->GetNumChargeClusters();
  for (size_t i=0; i<ncl; i++) {
    
    const EXOChargeCluster* cc = sc ? sc->GetChargeClusterAt(i) : ED->GetChargeCluster(i);
    
    if(RadialCulling(cc->fX,cc->fY,fRadialCull) and ((oldDef3d) || (fabs(cc->fX)<900. and fabs(cc->fY)<900.))) {
      returnCollection.fCulledChargeClusters.insert(cc);
      continue;
    }

    if(!bool(sc) && fabs(cc->fCollectionTime/1000. - ED->fEventHeader.fTriggerOffset) > fMaxDriftSamples) // we need to check the trigger and max drift time
    {
      returnCollection.SetMultipleChargeOnlyEvents();
      continue;
    }
    
    if(ED->fEventHeader.fIsMonteCarloEvent and cc->fRawEnergy < fChargeClusterThreshold) { //should we apply threshold to individual U-wire energies instead? -I.O.
      continue;
    }
    returnCollection.fChargeClusters.insert(cc);
  }
    
  if(ED->fEventHeader.fIsMonteCarloEvent){
    //Associate PCDs with the charge cluster that survived culling
    returnCollection.SetPCDMetric(SelectPCDs(ED,returnCollection.fChargeClusters,returnCollection.fChargeDeposits,returnCollection.fNonAssociatedChargeDeposits));
  }
  returnCollection.AssertInitialization(bool(sc));
  return returnCollection;
}

//_____________________________________________________________________________
int EXOClusterCull::SelectPCDs(const EXOEventData* ED, const set<const EXOChargeCluster*>& ccs, map<const EXOChargeCluster*, set<const EXOMCPixelatedChargeDeposit*> >& pcds, set<const EXOMCPixelatedChargeDeposit*> &nonAssociatedPcds) const
{
  //Find all pixelated charge deposits in ED that have a corresponding
  //charge cluster in ccs and add them to pcds
  
  if(fFillDebugTree){
    fDebugEventNumber = ED->fEventNumber;
    fDebugRunNumber = ED->fRunNumber;
    fDebugSkippedByClustering = ED->fSkippedByClustering;
  }

  //Set metric to associate PCDs
  size_t noU(0), noV(0), noZ(0);
  for(set<const EXOChargeCluster*>::const_iterator iter=ccs.begin(); iter!=ccs.end(); iter++)
  {
    if(fabs((*iter)->fU) > 900) noU += 1;
    if(fabs((*iter)->fV) > 900) noV += 1;//hasV = false;
    if(fabs((*iter)->fZ) > 900) noZ += 1;//hasZ = false;
  }
  int pcdMetric = 0;
  if(noZ == ccs.size()) // scintillation-less event
  {
    if(noU > noV) pcdMetric = 1; // Use V-metric if more CCs with V-position than U-position
    else if(noU < noV) pcdMetric = 2; // Use U-metric if more CCs with U-position than V-position
    else pcdMetric = 3; // Use UV-metric if number of CCs with U-position equals that with V-position
  }
  else // event with at least one z = scintillation event
  {
    if(noU > noV) pcdMetric = 4; // Use VZ-metric if more CCs with V-position associated to scintillation cluster
    else pcdMetric = 5; // Use UZ-metric if more or equal CCs with U-position than V-position
  }      
  
  size_t nPCDs = ED->fMonteCarloData.GetNumPixelatedChargeDeposits();
  for (size_t i = 0; i < nPCDs; i++) {
    const EXOMCPixelatedChargeDeposit* pcd = ED->fMonteCarloData.GetPixelatedChargeDeposit(i);
    const EXOChargeCluster* nearestCC = 0;
    double nearestDistance = numeric_limits<double>::max();
    for(set<const EXOChargeCluster*>::const_iterator iter=ccs.begin(); iter!=ccs.end(); iter++){
      if(not TimeCompatible(*(*iter),*pcd)) {
        continue;
      }
      double distance = CalculateDistance(*(*iter),*pcd,pcdMetric);
      if(distance < nearestDistance){
        nearestCC = *iter;
        nearestDistance = distance;
      }
    }

    //Associate pcd with nearestCC
    if(pcdMetric == 2 or pcdMetric == 3 or pcdMetric == 5)
    {
      if(nearestCC && IsUdistanceAcceptable(*nearestCC,*pcd)) 
        pcds[nearestCC].insert(pcd);
    }
    else if(pcdMetric == 1 or pcdMetric == 3 or pcdMetric == 4)
    {
      if(nearestCC && IsVdistanceAcceptable(*nearestCC,*pcd))
        pcds[nearestCC].insert(pcd);
    }
    else{
      //No corresponding charge cluster found for pcd
      nonAssociatedPcds.insert(pcd);
      if(fFillDebugTree){
        fDebugMissingCCEnergy = 0;
        for(set<const EXOChargeCluster*>::const_iterator iter=ccs.begin(); iter!=ccs.end(); iter++){
          if( fabs((*iter)->fX) > 900 or fabs((*iter)->fY) > 900 ){
            fDebugMissingCCEnergy += (*iter)->fRawEnergy;
          }
        }
        fDebugPCD = pcd;
        fDebugDistance = nearestDistance;
        fDebugTree->Fill();
      }
    }
  }
  return pcdMetric;
}

bool EXOClusterCull::IsUdistanceAcceptable(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const
{
  //Calculate whether cc and pcd are near enough in U dimension to be associated.
  //The acceptance distance depends on the size of the charge cluster, i.e. the
  //number of U-wire signals on different channels 

  set<int> Uchannels;
  size_t nuws = cc.GetNumUWireSignals();
  for(size_t i=0; i<nuws; i++){
    Uchannels.insert(cc.GetUWireSignalChannelAt(i));
  }
  double Uacceptance = (static_cast<double>(Uchannels.size())/2. + fExtraUChannelAcceptance) * CHANNEL_WIDTH;
  double Udist = fabs(cc.fU - pcd.GetPixelCenter().GetU());
  if(Udist > Uacceptance){
    return false;
  }
  return true;
}

bool EXOClusterCull::IsVdistanceAcceptable(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const
{
  //Calculate whether cc and pcd are near enough in V dimension to be associated.
  //The acceptance distance depends on the size of the charge cluster, i.e. the
  //number of V-wire signals on different channels
  set<int> Vchannels;
  size_t nvws = cc.GetNumVWireSignals();
  for(size_t i=0; i<nvws; i++){
    Vchannels.insert(cc.GetVWireSignalChannelAt(i));
  }
  double Vacceptance = (static_cast<double>(Vchannels.size())/2. + fExtraVChannelAcceptance) * CHANNEL_WIDTH;
  double Vdist = fabs(cc.fV - pcd.GetPixelCenter().GetV());
  if(Vdist > Vacceptance){
    return false;
  }
  return true;
}

//_____________________________________________________________________________
bool EXOClusterCull::ChannelCompatible(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const
{
  // Check whether pcd could have possibly contributed to cc by only looking at channel numbers
  size_t nuws = cc.GetNumUWireSignals();
  bool sameDeposit = false;
  for(size_t i=0; i<nuws; i++) {
    if(cc.GetUWireSignalAt(i)->fChannel == pcd.fDepositChannel) {
      sameDeposit = true;
      break;
    }
  }
  if(not sameDeposit) {
    return false;
  }

  size_t nvws = cc.GetNumVWireSignals();
  for(size_t i=0; i<nvws; i++) {
    if(pcd.fWireChannelsAffected.count(cc.GetVWireSignalAt(i)->fChannel)) {
      return true;
    }
  }
  return false;
}

double EXOClusterCull::CalculateDistance(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd, int metric) const
{
  //Calculate the distance between the charge cluster and the pixelated charge deposit given pcd-cc metric
  
  if(metric == 1) return CalculateDistance(cc,pcd,"V");
  if(metric == 2) return CalculateDistance(cc,pcd,"U");
  if(metric == 3) return CalculateDistance(cc,pcd,"UV");
  if(metric == 4) return CalculateDistance(cc,pcd,"VZ");
  if(metric == 5) return CalculateDistance(cc,pcd,"UZ");

  LogEXOMsg("Distance to associate CC <--> PCD not defined.", EEError);
  return -1;
}

double EXOClusterCull::CalculateDistance(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd, TString dim) const
{
  //Calculate the distance between the charge cluster and the pixelated charge deposit given dimensions to include
  
  double dist2 = 0;
  EXOCoordinates pixel = pcd.GetPixelCenter();
  if(dim.Contains("U"))
    dist2 += pow(cc.fU - pixel.GetU(),2);
  if(dim.Contains("V"))
    dist2 += pow(cc.fV - pixel.GetV(),2);
  if(dim.Contains("Z"))
    dist2 += pow(cc.fZ - pixel.GetZ(),2);
  
  return sqrt(dist2);
}

double EXOClusterCull::CalculateUZDistance(const EXOChargeCluster&cc, const EXOMCPixelatedChargeDeposit& pcd) const
{
  //Calculate the 2D distance (in U- and Z-dimension) between the Charge cluster and the PCD
  EXOCoordinates pixel = pcd.GetPixelCenter();
  double du = cc.fU - pixel.GetU();
  double dz = cc.fZ - pixel.GetZ();
  return sqrt(du*du + dz*dz);
}

//______________________________________________________________________________
bool EXOClusterCull::TimeCompatible(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const
{
  //Check whether pcd could have possibly contributed to cc by only looking at timing.
  if( fabs(cc.fCollectionTime - pcd.fWireHitTime) < fMaxPCDTime*CLHEP::microsecond) {
    return true;
  }
  return false;
}

//_____________________________________________________________________________
void EXOClusterCull::SetChargeClusterThreshold(double thresh)
{
  //Set the charge cluster threshold for PCD association
  fChargeClusterThreshold = thresh;
}

//_____________________________________________________________________________
bool EXOClusterCull::HexagonalCulling(double x, double y, double z, double RHex) const
{
  //Return true, if (x,y) coordinate needs to be culled
  double u;
  double v;

  //WE NEED z! (x,y) --> (u,v) depends on detector half!!
  EXOMiscUtil::XYToUVCoords(u,v,x,y,z);

  //cull outside choosen hexagon
  if ( fabs(u) > RHex ||
       fabs(v) > RHex ||
       fabs(x) > RHex ) {
    return true;
  }

  return false;
}

//_____________________________________________________________________________
Bool_t EXOClusterCull::RadialCulling(Double_t x, Double_t y, Double_t Rmax) const
{
  //Return true, if (x,y) coordinate needs to be culled
  Double_t r2= x*x+y*y;

  if(r2 > Rmax*Rmax) {
    return true;
  }
  return false;
}

void EXOClusterCull::FillDebugTree(bool val)
{
  if(val == false){
    if(fDebugFile){
      fDebugFile->Close();
      fDebugTree = 0;
    }
    delete fDebugFile;
    fFillDebugTree = false;
    return;
  }

  fFillDebugTree = true;
  if(fDebugTree && fDebugFile){
    return;
  }

  fDebugFile = new TFile(fDebugFilename.c_str(),"RECREATE");
  fDebugTree = new TTree("ClusterCullDebug","ClusterCullDebug");
  fDebugTree->Branch("PCD",&fDebugPCD);
  fDebugTree->Branch("missingCCenergy",&fDebugMissingCCEnergy,"missingCCenergy/D");
  fDebugTree->Branch("distance",&fDebugDistance,"distance/D");
  fDebugTree->Branch("fEventNumber",&fDebugEventNumber,"fEventNumber/I");
  fDebugTree->Branch("fRunNumber",&fDebugRunNumber,"fRunNumber/I");
  fDebugTree->Branch("fSkippedByClustering",&fDebugSkippedByClustering,"fSkippedByClustering/O");
}

void EXOClusterCull::WriteDebugTree()
{
  fDebugFile->Write();
  fDebugFile->Close();
  fDebugTree = 0;
  delete fDebugFile;
  fDebugFile = 0;
}


//______________________________________________________________________________
EXOClusterCull::VecList
EXOClusterCull::GetListForEventData(const EXOEventData& ed) const
{
  // Get a vector list given an event data object.
  const EXOMonteCarloData& mc = ed.fMonteCarloData;
  VecList retList;
  for (size_t i=0; i<mc.GetNumPixelatedChargeDeposits(); i++) {
    const EXOMCPixelatedChargeDeposit& pcd =
      *mc.GetPixelatedChargeDeposit(i);
    EXOCoordinates coord = pcd.GetPixelCenter();

    PCDVal newVal = {
      TVector3( coord.GetX(), coord.GetY(), coord.GetZ() ),
      pcd.fTotalEnergy*1000.0
    };
    retList.push_back(newVal);
  }
  return retList;
}

//______________________________________________________________________________
void EXOClusterCull::PerformClusteringOnList(
  VecList& listToCluster,
  double clusterDiameter) const
{
  // Perform clustering on a list.  This function iteratively clusters the
  // list, combining clusters that fall within a certain diameter.  This is a
  // translation of python code.

  typedef std::map<double,
          std::pair< VecList::iterator, VecList::iterator > > RadMapType;

  // Return if the cluster list is either empty or has a size of 1
  if (listToCluster.size() <= 1) {
    return;
  }

  RadMapType radMap;
  VecList::iterator first, second;

  // First calculate the distance between all clusters
  for (first = listToCluster.begin(); first != listToCluster.end(); first++) {
    second = first;
    for (second++; second != listToCluster.end(); second++) {
      double r = (first->position - second->position).Mag();
      radMap[ r ] = std::make_pair(first, second);
    }
  }
  if ( radMap.begin()->first < clusterDiameter ) {
    // We need to cluster this.  Add the two vectors together.
    first = radMap.begin()->second.first;
    second = radMap.begin()->second.second;
    PCDVal newVal;
    newVal.energy = first->energy + second->energy;
    newVal.position = (first->energy*first->position +
                       second->energy*second->position)*(1./newVal.energy);

    // Now remove the old two clusters and add the new
    listToCluster.erase(first);
    listToCluster.erase(second);
    listToCluster.push_back(newVal);

    // Cycle
    PerformClusteringOnList(listToCluster, clusterDiameter);
  }

}
