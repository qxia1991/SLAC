//______________________________________________________________________________
//                                                                        
// EXOUWireInductionSignal                                                           
//
// Encapsulation of u-wire induction signal.
// 

#include "EXOUtilities/EXOUWireInductionSignal.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "TROOT.h"
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

ClassImp( EXOUWireInductionSignal )

//______________________________________________________________________________
EXOUWireInductionSignal::EXOUWireInductionSignal() :  TObject()
{
  EXOUWireInductionSignal::Clear();
  gROOT->GetListOfCleanups()->Add(this);
}
//______________________________________________________________________________
EXOUWireInductionSignal::~EXOUWireInductionSignal() 
{
  gROOT->GetListOfCleanups()->Remove(this);
}
//______________________________________________________________________________
void EXOUWireInductionSignal::Clear( Option_t* ) 
{
    fChannel        = -999;		
    fMagnitude      =    0;			
    fMagnitudeError =    0;
    fTime           =    0;			
    fTimeError      =    0;		
    fBaseline       =    0;		
    fBaselineError  =    0;	
    fChiSquare      =    0;
    fChiSquareRestr =    0;
    fRefChargeClusters.Clear();
}
//______________________________________________________________________________
void EXOUWireInductionSignal::RecursiveRemove(TObject *obj) 
{
  // Try to remove object from TRefArray.  This function is automatically
  // called by TObject's destructor when a reference object is deleted.

  // Unfortunately, it's possible for the process ID of the TRefArray to get
  // deleted *out from under* the TRefArray without notifying it.  This is a bug
  // in ROOT, but this provides a workaround by checking if it was deleted.
  // The *first* check is to see if TProcessID::Cleanup has been called,
  // because TProcessID has no internal checking to avoid dereferencing a NULL
  // pointer!
  TProcessID *id = fRefChargeClusters.GetPID();
  if(gROOT->GetListOfCleanups()->IndexOf(TProcessID::GetPIDs()) < 0 
     || !TProcessID::IsValid(id)){
    fRefChargeClusters.Clear();//avoid future problems by clearing
    return;
  }
  if(!fRefChargeClusters.Remove(obj)){
    // Unfortunately, it's possible that the TProcessID removed this already,
    // so we need to loop through to find a NULL value.  
    for (Int_t i=0;i<=fRefChargeClusters.GetLast();i++){
      if(!fRefChargeClusters.At(i))
        fRefChargeClusters.RemoveAt(i); 
    }
  } 
 
  // We have to try to compress because the above call might still return 0,
  // meaning the object is already removed from the TProcessID object.
  fRefChargeClusters.Compress();
}
//______________________________________________________________________________
void EXOUWireInductionSignal::InsertChargeCluster( EXOChargeCluster *aValue ) 
{ 
  // Add a charge cluster that this u-wire signal is associated to.
  if(fRefChargeClusters.IsEmpty())
    fRefChargeClusters = EXOTRefArray(TProcessID::GetProcessWithUID(aValue));
  fRefChargeClusters.Add(aValue);
}
//______________________________________________________________________________
EXOChargeCluster* EXOUWireInductionSignal::GetChargeClusterAt( size_t index ) 
{ 
  // Get an associated charge cluster.
  return static_cast< EXOChargeCluster* >( fRefChargeClusters.At(index) ); 
}
//______________________________________________________________________________
const EXOChargeCluster* EXOUWireInductionSignal::GetChargeClusterAt( size_t index ) const 
{ 
  // Get a const associated charge cluster.
  return static_cast< const EXOChargeCluster* >( fRefChargeClusters.At(index) ); 
}
//______________________________________________________________________________
void EXOUWireInductionSignal::Print( Option_t* /*opt*/ ) const
{
  // Output a summary of the contents of this charge cluster to <stdout> in
  // human-readable form.
  cout<<string(100, '-')<<endl;
  cout<<"U-wire induction signal summary"<<endl;
  cout<<"Channel number....................:  "<<fChannel<<endl;
  cout<<"Collection time...................:  "
      <<fixed<<setprecision(1)<<fTime<<" +/- "<<fTimeError<<" ns."<<endl;
  cout<<"Magnitude.........................:  "
      <<fixed<<setprecision(1)<<setw(6)<<fMagnitude<<" +/- "<<fMagnitudeError<<" ADC counts."<<endl;
  cout<<"Channel baseline..................:  "
      <<fixed<<setprecision(1)<<fBaseline<<" +/- "<<fBaselineError<<" ADC counts."<<endl;
  cout<<"Chi^2 of the fit to this signal...:  "<<fixed<<setprecision(1)<<fChiSquare<<endl;
  cout<<"Chi^2 of the fit to this signal"<<endl;
  cout<<" with restricted window...........:  "<<fixed<<setprecision(1)<<fChiSquareRestr<<endl;
  cout<<string(80, '-')<<endl;
}
//______________________________________________________________________________
bool EXOUWireInductionSignal::operator==( const EXOUWireInductionSignal& other ) const
{
   bool retVal =
     (fChannel               == other.fChannel and		
      fMagnitude             == other.fMagnitude and			
      fMagnitudeError        == other.fMagnitudeError and
      fTime                  == other.fTime and			
      fTimeError             == other.fTimeError and		
      fBaseline              == other.fBaseline and		
      fBaselineError         == other.fBaselineError and
      fChiSquare             == other.fChiSquare and
      fChiSquareRestr        == other.fChiSquareRestr and
      GetNumChargeClusters() == other.GetNumChargeClusters());
   if (not retVal) return retVal;
   if (not TestBit(kIsReferenced)){//then we can test the charge clusters
     for(size_t i=0; i<GetNumChargeClusters(); i++){
       if(not (*GetChargeClusterAt(i) == *other.GetChargeClusterAt(i)))
         return false;
     }
   }
   return true;
}

