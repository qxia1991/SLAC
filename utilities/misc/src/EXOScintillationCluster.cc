//______________________________________________________________________________
//                                                                        
// EXOScintillationCluster 
//
// encapsulates information for a scintillation cluster.  Also includes
// associated class EXOGangInfo which encapsulates information for different
// gangs for a given scintillation cluster event. 
//
// WARNING:  The pointer fGangInfo nominally points to an TClonesArray;
// in fact, it is assumed (eg. by the GetNum functions) that it points
// to an EXOTClonesArray, which is guaranteed to always be compressed.
// See the class description for EXOTClonesArray, which explains caveats.

#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOAPDSignal.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"
#include "TROOT.h"
#include "TMath.h"
#include <string>
#include <iostream>
#include <cmath>

ClassImp( EXOScintillationCluster )

//______________________________________________________________________________
void EXOScintillationCluster::Clear( Option_t* ) 
{ 
  // Clear event, reset to initial state
  fX = 0.0; 
  fY = 0.0; 
  fZ = 0.0; 
  fTime = 0.0; 
  fRefChargeClusters.Clear();
  fRefAPDSignals.Clear();
  fRefRecommendedSignal = 0;
  fRawEnergy = 0.0; 
  fWeightedAPDEnergy = 0.0;
  fEnergy = 0.0;
  fEnergyError = 0.0;
  fDenoisedEnergy = 0.0;
  fDenoisedEnergyError = 0.0;
  fDenoisingInternalCode = -1;
  fDNNVarRaw   = -1;
  fDNNVarRecon = -1;
  fDNNChargeEnergy = 0;
}

//______________________________________________________________________________
EXOScintillationCluster::EXOScintillationCluster() : 
  TObject()
{ 
  EXOScintillationCluster::Clear(); 
  gROOT->GetListOfCleanups()->Add(this);
}

//______________________________________________________________________________
EXOScintillationCluster::~EXOScintillationCluster() 
{
  gROOT->GetListOfCleanups()->Remove(this);
}
//______________________________________________________________________________
void EXOScintillationCluster::InsertChargeCluster( EXOChargeCluster* obj )
{
  // Insert an associated charge cluster into this object.
  if(fRefChargeClusters.IsEmpty()) fRefChargeClusters = EXOTRefArray(TProcessID::GetProcessWithUID(obj)); 
  fRefChargeClusters.Add(obj);
}
//______________________________________________________________________________
EXOChargeCluster* EXOScintillationCluster::GetChargeClusterAt(size_t index)
{
  // Get an associated charge cluster from this object. 
  return static_cast<EXOChargeCluster*>(fRefChargeClusters.At(index));
}
//______________________________________________________________________________
const EXOChargeCluster* EXOScintillationCluster::GetChargeClusterAt(size_t index) const
{
  // Get an associated charge cluster from this object. 
  return static_cast<const EXOChargeCluster*>(fRefChargeClusters.At(index));
}
//______________________________________________________________________________
void EXOScintillationCluster::RecursiveRemove(TObject* obj) 
{
  // Try to remove object from TRefArray.  This function is automatically
  // called by TObject's destructor when a reference object is deleted.

  // Unfortunately, it's possible for the process ID of the TRefArray to get
  // deleted*out from under* the TRefArray without notifying it.  This is a bug
  // in ROOT, but this provides a workaround by checking if it was deleted.  
  // The *first* check is to see if TProcessID::Cleanup has been called,
  // because TProcessID has no internal checking to avoid dereferencing a NULL
  // pointer!
  TProcessID* id = fRefChargeClusters.GetPID();
  if (gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
      || !TProcessID::IsValid(id)) {
    fRefChargeClusters.Clear(); // Avoid future problems by clearing. 
    return;
  }
  if(!fRefChargeClusters.Remove(obj)) {
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefChargeClusters.GetLast();i++) {
      if(!fRefChargeClusters.AtWithoutTable(i)) {
        fRefChargeClusters.RemoveAt(i); 
      }
    }
  } 
  fRefChargeClusters.Compress();
  //
  id = fRefAPDSignals.GetPID();
  if (gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
      || !TProcessID::IsValid(id)) {
    fRefAPDSignals.Clear(); // Avoid future problems by clearing. 
    return;
  }
  if(!fRefAPDSignals.Remove(obj)) {
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefAPDSignals.GetLast();i++) {
      if(!fRefAPDSignals.AtWithoutTable(i)) {
        fRefAPDSignals.RemoveAt(i); 
      }
    }
  } 
  fRefAPDSignals.Compress();
}
//______________________________________________________________________________
double EXOScintillationCluster::GetRadius() const { return hypot(fX,fY); }
double EXOScintillationCluster::GetTheta() const { return std::atan2(fY,fX); }
double EXOScintillationCluster::GetEnergy() const { return GetRecommendedSignal()->GetEnergy(); }
double EXOScintillationCluster::GetEnergyError() const { return 0; }

//*****************************************************
//        APD signals 
//*****************************************************
//______________________________________________________________________________
void EXOScintillationCluster::SetRecommendedSignal(EXOAPDSignal *sig)
{
  // Set recommended signal, i.e. signal to estimate energy
  fRefRecommendedSignal = sig;
  // FIXME update energy
}
//______________________________________________________________________________
const EXOAPDSignal* EXOScintillationCluster::GetRecommendedSignal() const
{
  // Get recommended signal
  return static_cast<const EXOAPDSignal*> (fRefRecommendedSignal.GetObject());
}
//______________________________________________________________________________
EXOAPDSignal* EXOScintillationCluster::GetRecommendedSignal()
{
  // Get recommended signal
  return static_cast<EXOAPDSignal*> (fRefRecommendedSignal.GetObject());
}

//______________________________________________________________________________
int EXOScintillationCluster::FindAPDSignalLocation(EXOAPDSignal::EXOAPDSignal_type type, int pos, int n) const
{ 
  // Find first APD signal of this type, associated to this cluster.
  if (pos<0) pos = 0;
  if (n==-1) n = GetNumAPDSignals();
  else n += pos;
  if (n>=(int)GetNumAPDSignals()) n = GetNumAPDSignals();
  for (int i=pos;i<n;i++) {
    const EXOAPDSignal* sig = GetAPDSignalAt(i);
    if (sig && sig->fType==type)
      return i;
  }
  return -1;
}
//______________________________________________________________________________
EXOAPDSignal* EXOScintillationCluster::GetAPDSignal(EXOAPDSignal::EXOAPDSignal_type type, int channel) 
{ 
  // Get APD signal associated to this cluster with type and channel.
  for (int i=0;i<=fRefAPDSignals.GetLast();i++) {
    EXOAPDSignal* sig = static_cast<EXOAPDSignal*>(fRefAPDSignals[i]);
    if (sig && sig->fType==type && sig->fChannel==channel)
      return sig;
  }
  return 0;
}
//______________________________________________________________________________
const EXOAPDSignal* EXOScintillationCluster::GetAPDSignal(EXOAPDSignal::EXOAPDSignal_type type, int channel) const
{ 
  // Get APD signal associated to this cluster with type and channel.
  for (int i=0;i<=fRefAPDSignals.GetLast();i++) {
    const EXOAPDSignal* sig = static_cast<const EXOAPDSignal*>(fRefAPDSignals[i]);
    if (sig && sig->fType==type && sig->fChannel==channel)
      return sig;
  }
  return 0;
}
//______________________________________________________________________________
EXOAPDSignal* EXOScintillationCluster::GetAPDSignal(const char *descr, int channel) 
{ 
  // Get APD signal associated to this cluster with type and channel.
  for (int i=0;i<=fRefAPDSignals.GetLast();i++) {
    EXOAPDSignal* sig = static_cast<EXOAPDSignal*>(fRefAPDSignals[i]);
    if (sig && sig->fChannel==channel && sig->fDescr==descr) // descr comparison is slow
      return sig;
  }
  return 0;
}
//______________________________________________________________________________
const EXOAPDSignal* EXOScintillationCluster::GetAPDSignal(const char *descr, int channel) const
{ 
  // Get APD signal associated to this cluster with type and channel.
  for (int i=0;i<=fRefAPDSignals.GetLast();i++) {
    const EXOAPDSignal* sig = static_cast<const EXOAPDSignal*>(fRefAPDSignals[i]);
    if (sig && sig->fChannel==channel && sig->fDescr==descr) // descr comparison is slow
      return sig;
  }
  return 0;
}
//______________________________________________________________________________
size_t EXOScintillationCluster::GetNumGangs() const 
{ 
  // Get number of individual gangs signals.
  unsigned int n = 0;
  for (unsigned int i=0;i<GetNumAPDSignals();i++) {
    const EXOAPDSignal* sig = GetAPDSignalAt(i);
    if (sig && sig->fType==EXOAPDSignal::kGangFit)
      n++;
  }
  return n;
}

//______________________________________________________________________________
void EXOScintillationCluster::AddAPDSignalsOfTypeToList(EXOAPDSignal::EXOAPDSignal_type type, TList& retList) const 
{ 
  // Get a list of all signals of type 
  for (size_t i=0;i<GetNumAPDSignals();i++) {
    const EXOAPDSignal* sig = GetAPDSignalAt(i);
    if (sig && sig->fType==type) retList.Add(const_cast<EXOAPDSignal*>(sig));
  }
}
//______________________________________________________________________________
EXOScintillationCluster::EXOScintillationCluster(const EXOScintillationCluster& other) : TObject(other)
{
  fX = other.fX; 
  fY = other.fY; 
  fZ = other.fZ; 
  fTime = other.fTime; 
  fRefChargeClusters.Clear();
  fRefAPDSignals.Clear();
  fRefRecommendedSignal = 0;
  fRawEnergy = other.fRawEnergy; 
  fWeightedAPDEnergy = other.fWeightedAPDEnergy;
  fEnergy = other.fEnergy; 
  fDenoisedEnergy = other.fDenoisedEnergy;
  fDenoisedEnergyError = other.fDenoisedEnergyError;
  fDenoisingInternalCode = other.fDenoisingInternalCode;
  fDNNVarRaw  = other.fDNNVarRaw;
  fDNNVarRecon= other.fDNNVarRecon;
  fDNNChargeEnergy = other.fDNNChargeEnergy;
}
//______________________________________________________________________________
EXOScintillationCluster& EXOScintillationCluster::operator=(const EXOScintillationCluster& other)
{
  if (this == &other) return *this;
  fX = other.fX; 
  fY = other.fY; 
  fZ = other.fZ; 
  fTime = other.fTime; 
  fRefChargeClusters.Clear();
  fRefAPDSignals.Clear();
  fRefRecommendedSignal = 0;
  fRawEnergy = other.fRawEnergy; 
  fWeightedAPDEnergy = other.fWeightedAPDEnergy;
  fEnergy = other.fEnergy; 
  fDenoisedEnergy = other.fDenoisedEnergy;
  fDenoisedEnergyError = other.fDenoisedEnergyError;
  fDenoisingInternalCode = other.fDenoisingInternalCode;
  fDNNVarRaw = other.fDNNVarRaw;
  fDNNVarRecon = other.fDNNVarRecon;
  fDNNChargeEnergy = other.fDNNChargeEnergy;
  return *this;
}
//______________________________________________________________________________
void EXOScintillationCluster::Print( Option_t* opt /* = false */ ) const
{
  // Output a summary of the contents of this charge cluster to stdout in human-readable form.
  // When argument is "R", this also calls the Print command of associated charge clusters.

  using namespace std;
  if( strcmp(opt, "R") == 0 ) cout<<string(100, '*')<<endl;
  else cout<<string(100, '-')<<endl;
  cout<<"Scintillation cluster summary:"<<endl;
  
  // Time
  cout<<"Time of signal:  "<<fTime<<" ns."<<endl;

  // Magnitude and type
  Int_t algoUsed = GetAlgorithmUsed();
  if( algoUsed == 0 ) {
    cout<<"This scintillation cluster was observed on both TPC planes."<<endl;
    cout<<"Counts on TPC plane 1:  "<<GetCountsOnAPDPlane(EXOMiscUtil::kNorth)
        <<" +/- "<<GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth)<<" ADC counts."<<endl;
    cout<<"Counts on TPC plane 2:  "<<GetCountsOnAPDPlane(EXOMiscUtil::kSouth)
        <<" +/- "<<GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth)<<" ADC counts."<<endl;
  }
  else if ( algoUsed == 4 ) {
    cout<<"This scintillation cluster was only observed on TPC plane 1."<<endl;
    cout<<"Counts on TPC plane 1:  "<<GetCountsOnAPDPlane(EXOMiscUtil::kNorth)
        <<" +/- "<<GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth)<<" ADC counts."<<endl;
  }
  else if ( algoUsed == 5 ) {
    cout<<"This scintillation cluster was only observed on TPC plane 2."<<endl;
    cout<<"Counts on TPC plane 2:  "<<GetCountsOnAPDPlane(EXOMiscUtil::kSouth)
        <<" +/- "<<GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth)<<" ADC counts."<<endl;
  }
  else {
    cout<<"Oops -- this fAlgorithmUsed option was not programmed into EXOScintillationCluster::Print."<<endl;
  }

  // Summed gang results
  cout<<"Fitting gangs individually with fixed time, then summing, gave:"<<endl;
  cout<<"\tOn plane 1:  "<<GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth)
      <<" +/- "<<GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth)<<" ADC counts."<<endl;
  cout<<"\tOn plane 2:  "<<GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth)
      <<" +/- "<<GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth)<<" ADC counts."<<endl;
  cout<<endl;

  // Output information on charge clusters.
  cout<<GetNumChargeClusters()<<" charge clusters are associated with this scintillation event."<<endl;
  if( strcmp(opt, "R") == 0 ) {
    for( size_t i=0; i<GetNumChargeClusters(); i++) {
      GetChargeClusterAt(i)->Print();
    }
  }

  if( strcmp(opt, "R") == 0 ) cout<<string(100, '*')<<endl;
  else cout<<string(100, '-')<<endl;
}

//______________________________________________________________________________
bool EXOScintillationCluster::operator==( const EXOScintillationCluster& other ) const
{
  // Deeply compare two scintillation clusters.
  bool retVal = fX == other.fX and 
          fY == other.fY and 
          fZ == other.fZ and 
          fTime == other.fTime and 
          fRawEnergy == other.fRawEnergy and 
          fWeightedAPDEnergy == other.fWeightedAPDEnergy and
          fEnergy == other.fEnergy and 
          fDenoisedEnergy == other.fDenoisedEnergy and
          fDenoisedEnergyError == other.fDenoisedEnergyError and
          fDenoisingInternalCode == other.fDenoisingInternalCode and
          GetNumChargeClusters() == other.GetNumChargeClusters() and
          GetNumAPDSignals() == other.GetNumAPDSignals() and
          fDNNVarRaw   == other.fDNNVarRaw and
          fDNNVarRecon == other.fDNNVarRecon and
          fDNNChargeEnergy == other.fDNNChargeEnergy;
  if (not retVal) return retVal;

  // Else check the rest, this is very deep, we try to compare all the referenced
  // clusters. 
  for (size_t i=0;i<GetNumChargeClusters();i++) {
      if (not (*GetChargeClusterAt(i) == *other.GetChargeClusterAt(i))) return false; 
  }
  for (size_t i=0;i<GetNumAPDSignals();i++) {
      if (not (*GetAPDSignalAt(i) == *other.GetAPDSignalAt(i))) return false; 
  }
  if (not GetRecommendedSignal() and other.GetRecommendedSignal()) return false;
  if (GetRecommendedSignal() and not other.GetRecommendedSignal()) return false;
  if ((GetRecommendedSignal() and other.GetRecommendedSignal()) and 
      not (*GetRecommendedSignal() == *other.GetRecommendedSignal())) return false; 
  return true;

}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const
{
  // Return the counts from an APD plane
  return GetCountsOnAPDPlane(aplane, false);
}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsErrorOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const
{
  // Return the error on counts from an APD plane
  return GetCountsOnAPDPlane(aplane, true);
}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsSumOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const
{
  // Return the counts from the sum on individual fits an APD plane
  return GetCountsSumOnAPDPlane(aplane, false);
}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsSumErrorOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const
{
  // Return the error on sum counts from individual fits from an APD plane
  return GetCountsSumOnAPDPlane(aplane, true);
}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsOnAPDPlane(EXOMiscUtil::ETPCSide aplane, bool error) const
{
  // Return the counts from the APD fits on a given plane 

  const EXOAPDSignal* sig = NULL;
  switch(aplane) {
    case EXOMiscUtil::kNorth: sig = GetPlaneOneSignal(); break;
    case EXOMiscUtil::kSouth: sig = GetPlaneTwoSignal(); break;
  }
  if (!sig) return 0.0;
  return (error) ? sig->fCountsError : sig->fRawCounts;
}

//______________________________________________________________________________
Double_t EXOScintillationCluster::GetCountsSumOnAPDPlane(EXOMiscUtil::ETPCSide aplane, bool error) const
{
  // Return the counts sum on given APD Plane (sum over Gang signals)
  size_t anum = GetNumAPDSignals();
  double counts = 0.0;
  double counts_error = 0.0;
  for (size_t j=0; j<anum; j++) {
    const EXOAPDSignal* sig = GetAPDSignalAt(j);
    if (!sig || sig->fType != EXOAPDSignal::kGangFit || 
        aplane != EXOMiscUtil::GetTPCSide(sig->fChannel)) continue;
    counts += sig->fRawCounts;
    counts_error += sig->fCountsError*sig->fCountsError;;
  }
  return (error) ? TMath::Sqrt(counts_error) : counts;
}
