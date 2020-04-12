//______________________________________________________________________________
//
// EXOChargeCluster
//
// Class encapsulating a charge cluster
//

#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TROOT.h"
#include <string>
#include <iostream>

ClassImp( EXOChargeCluster )

//______________________________________________________________________________
EXOChargeCluster::EXOChargeCluster() : TObject()
{
  EXOChargeCluster::Clear();
  gROOT->GetListOfCleanups()->Add(this);
}
//______________________________________________________________________________
EXOChargeCluster::~EXOChargeCluster() 
{
  gROOT->GetListOfCleanups()->Remove(this);
}
//______________________________________________________________________________
void EXOChargeCluster::Clear( Option_t* opt )
{ 
  // Clear charge info, reset to initial state
  fX = 0.0; 
  fY = 0.0; 
  fZ = 0.0; 
  fZRMSFoundSignals = 0.0;
  fCollectionTime = 0.0; 
  fRawEnergy = 0.0; 
  fRawEnergyError = 0.0;
  fCorrectedEnergy = 0.0; 
  fU                           = 0;
  fURMSFoundSignals            = 0;
  fV                           = 0;
  fVRMSFoundSignals            = 0;
  fDriftTime                   = 0;
  fPurityCorrectedEnergy       = 0;     
  fCorrectedEnergyError        = 0;
  fAmplitudeInVChannels        = 0;   
  fCorrectedAmplitudeInVChannels = 0;   
  fRefScintCluster             = NULL;    
  fRefUWireSignals.Clear();
  fRefVWireSignals.Clear();
  fRefUWireInductionSignals.Clear();
  fDetectorHalf                = 0;  
  fEnergyInVChannels           = 0;     
  fSumRisetime                 = 0;
  fPurityCorrectedEnergyMix    = 0;
  fCorrectedEnergyMix          = 0;
}

//______________________________________________________________________________
void EXOChargeCluster::SetScintillationCluster( EXOScintillationCluster* aValue ) 
{ 
  // Set associated charge cluster
  fRefScintCluster = aValue; 
}

//______________________________________________________________________________
EXOScintillationCluster*  EXOChargeCluster::GetScintillationCluster() 
{ 
  // Get associated scintillation cluster
  return static_cast< EXOScintillationCluster* >( 
    fRefScintCluster.GetObject() ); 
}

//______________________________________________________________________________
const EXOScintillationCluster*  EXOChargeCluster::GetScintillationCluster() const  
{ 
  // Get associated scintillation cluster
  return static_cast< const EXOScintillationCluster* >( 
    fRefScintCluster.GetObject() ); 
}
//______________________________________________________________________________
void EXOChargeCluster::RecursiveRemove(TObject* obj) 
{
  // Try to remove object from TRefArray.  This function is automatically
  // called by TObject's destructor when a reference object is deleted.

  // Unfortunately, it's possible for the process ID of the TRefArray to get
  // deleted*out from under* the TRefArray without notifying it.  This is a bug
  // in ROOT, but this provides a workaround by checking if it was deleted.  
  // The *first* check is to see if TProcessID::Cleanup has been called,
  // because TProcessID has no internal checking to avoid dereferencing a NULL
  // pointer!
  TProcessID* id = fRefUWireSignals.GetPID();
  if (gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
      || !TProcessID::IsValid(id)) {
    fRefUWireSignals.Clear(); // Avoid future problems by clearing. 
    return;
  }
  if(!fRefUWireSignals.Remove(obj)) {
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefUWireSignals.GetLast();i++) {
      if(!fRefUWireSignals.AtWithoutTable(i)) {
        fRefUWireSignals.RemoveAt(i); 
      }
    }
  } 
 
  // Now do the same thing also for v-wires.
  id = fRefVWireSignals.GetPID();
  if (gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
      || !TProcessID::IsValid(id)) {
    fRefVWireSignals.Clear(); // Avoid future problems by clearing. 
    return;
  }
  if(!fRefVWireSignals.Remove(obj)) {
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefVWireSignals.GetLast();i++) {
      if(!fRefVWireSignals.AtWithoutTable(i)) {
        fRefVWireSignals.RemoveAt(i); 
      }
    }
  } 

  // Now for the u-wire induction signals
  id = fRefUWireInductionSignals.GetPID();
  if (gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
      || !TProcessID::IsValid(id)) {
    fRefUWireInductionSignals.Clear(); // Avoid future problems by clearing. 
    return;
  }
  if(!fRefUWireInductionSignals.Remove(obj)) {
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefUWireInductionSignals.GetLast();i++) {
      if(!fRefUWireInductionSignals.AtWithoutTable(i)) {
        fRefUWireInductionSignals.RemoveAt(i); 
      }
    }
  } 

  // We have to try to compress because the above call might still return 0,
  // meaning the object is already removed from the TProcessID object.
  fRefUWireSignals.Compress();
  fRefVWireSignals.Compress();
  fRefUWireInductionSignals.Compress();
}
//______________________________________________________________________________
void EXOChargeCluster::Print( Option_t* opt /* = "" */ ) const
{
  // Outputs a summary of the contents of this charge cluster to stdout in human-readable form.
  // When argument is "R", this also calls the Print command of associated u,v-wire and scintillation signals.

  using namespace std;
  if( strcmp(opt, "R") == 0 ) cout<<string(100, '*')<<endl;
  else cout<<string(100, '-')<<endl;
  cout<<"Charge cluster summary:"<<endl;

  // Skip charge clusters with no energy -- why were they even written?
  if( fRawEnergy <= 0.0 ) {
    cout<<"There is no energy associated with this charge cluster.  Returning."<<endl;
    return;
  }

  // Position
  if( GetNumVWireSignals() > 0 ) {
    cout<<"X position:  "<<fX<<" mm."<<endl;
    cout<<"Y position:  "<<fY<<" mm."<<endl;
    cout << "RMS of V:" << fVRMSFoundSignals << endl;
  }
  else {
    cout<<"U position:  "<<fU<<" mm.  (No v-wire signal was observed.)"<<endl;
  }
  cout << "RMS of U:" << fURMSFoundSignals << endl;
  if( GetScintillationCluster() != NULL ) {
    cout<<"Z position:  "<<fZ<<" mm."
        <<"Time of interaction:  "<<fCollectionTime - fDriftTime<<" ns."
        <<"Charge collection time:  "<<fCollectionTime<<" ns. "<<endl;
  }
  else {
    cout<<"Charge collection time:  "<<fCollectionTime<<" ns.  (Scintillation was not observed.)"<<endl;
  }

  // Energy information
  cout<<"Raw energy = "<<fRawEnergy<<" +/- "<<fRawEnergyError<<" keV."<<endl;
  if( fCorrectedEnergy > 0.0 ) cout<<"Corrected energy = "<<fCorrectedEnergy<<" +/- "<<fCorrectedEnergyError<<" keV."<<endl;
  if( fPurityCorrectedEnergy > 0.0 ) cout<<"Purity-corrected energy = "<<fPurityCorrectedEnergy<<" keV."<<endl;
  
  if( fPurityCorrectedEnergyMix > 0.0 ) cout<<"Purity-corrected Mix Energy = "<<fPurityCorrectedEnergyMix<<" keV."<<endl;
  if( fCorrectedEnergyMix > 0.0 ) cout<<"Corrected Mix energy = "<<fCorrectedEnergyMix << " keV."<<endl;
  cout<<endl;
  cout<<"The total amplitude in the v-wire channels (no real units) was "<<fAmplitudeInVChannels<<"."<<endl;

  // Information about components of the cluster.
  cout<<GetNumUWireSignals()<<" u-wire signals were merged to form this cluster."<<endl;
  if( strcmp(opt, "R") == 0 ) {
    for( size_t i=0; i<GetNumUWireSignals(); i++) GetUWireSignalAt(i)->Print();
  }
  if( strcmp(opt, "R") == 0 ) {
    for( size_t i=0; i<GetNumVWireSignals(); i++) GetVWireSignalAt(i)->Print();
  }
  if( strcmp(opt, "R") == 0 ) {
    for( size_t i=0; i<GetNumUWireInductionSignals(); i++) GetUWireInductionSignalAt(i)->Print();
  }
  if( GetScintillationCluster() == NULL ) {
    cout<<"No scintillation was paired with the cluster."<<endl;
  }
  else {
    cout<<"A scintillation signal was paired with this cluster."<<endl;
    if( strcmp(opt, "R") == 0 ) GetScintillationCluster()->Print();
  }

  if( strcmp(opt, "R") == 0 ) cout<<string(100, '*')<<endl;
  else cout<<string(100, '-')<<endl;
}
//______________________________________________________________________________
bool EXOChargeCluster::operator==(const EXOChargeCluster& other) const
{
  bool retVal = fX == other.fX and 
          fY == other.fY and 
          fZ == other.fZ and 
          fZRMSFoundSignals == other.fZRMSFoundSignals and
          fCollectionTime == other.fCollectionTime and 
          fRawEnergy == other.fRawEnergy and 
          fRawEnergyError == other.fRawEnergyError and
          fCorrectedEnergy == other.fCorrectedEnergy and 
          fU == other.fU and
          fURMSFoundSignals == other.fURMSFoundSignals and
          fV == other.fV and
          fVRMSFoundSignals == other.fVRMSFoundSignals and
          fDriftTime == other.fDriftTime and
          fPurityCorrectedEnergy == other.fPurityCorrectedEnergy and     
          fCorrectedEnergyError == other.fCorrectedEnergyError and
          fPurityCorrectedEnergyMix == other.fPurityCorrectedEnergyMix and
          fCorrectedEnergyMix == other.fCorrectedEnergyMix and
          fAmplitudeInVChannels == other.fAmplitudeInVChannels and   
          fCorrectedAmplitudeInVChannels == other.fCorrectedAmplitudeInVChannels and   
          fDetectorHalf == other.fDetectorHalf and  
          fEnergyInVChannels == other.fEnergyInVChannels and     
          fSumRisetime == other.fSumRisetime and  
          GetNumUWireSignals() == other.GetNumUWireSignals() and
          GetNumVWireSignals() == other.GetNumVWireSignals() and
          GetNumUWireInductionSignals() == other.GetNumUWireInductionSignals();
  if (not retVal) return retVal;
 
  // We have to be careful here, we can't do a deep check because
  // EXOScintillationCluster could reference this Charge Cluster, therefore we
  // only do this if this object is not reference.
  const EXOScintillationCluster* mine = GetScintillationCluster();
  const EXOScintillationCluster* hers = other.GetScintillationCluster();
  if ( not TestBit(kIsReferenced) ) {
     if ((mine and hers == NULL) ||
         (hers and mine == NULL)) return false;
     if ( mine && not (*mine == *hers)) return false;
  } 

  for (size_t i=0;i<GetNumUWireSignals();i++) {
      if (not (*GetUWireSignalAt(i) == *other.GetUWireSignalAt(i))) return false; 
  }
  for (size_t i=0;i<GetNumVWireSignals();i++) {
      if (not (*GetVWireSignalAt(i) == *other.GetVWireSignalAt(i))) return false; 
  }
  for (size_t i=0;i<GetNumUWireInductionSignals();i++) {
      if (not (*GetUWireInductionSignalAt(i) == *other.GetUWireInductionSignalAt(i))) return false; 
  }
  return true;
}
//______________________________________________________________________________

Bool_t EXOChargeCluster::Is3DCluster() const
{
  if(GetNumUWireSignals() < 1){
    return false;
  }
  if(GetNumVWireSignals() < 1){
    return false;
  }
  if(not GetScintillationCluster()){
    return false;
  }
  return true;
}

//______________________________________________________________________________
Bool_t EXOChargeCluster::IsFiducial() const 
{
  // Return if the charge cluster is "fiducial".  Only for backwards compatibility.
  if (!Is3DCluster() or
      (fabs(fZ) > CATHODE_APDFACE_DISTANCE - FIDUCIAL_CUT) or 
      (fabs(fU) > ACTIVE_XENON_RADIUS - FIDUCIAL_CUT) or
      (fabs(fV) >= ACTIVE_XENON_RADIUS - FIDUCIAL_CUT) or
      (fabs(fX) >= ACTIVE_XENON_RADIUS - FIDUCIAL_CUT)) return false; 

  return true;
}
