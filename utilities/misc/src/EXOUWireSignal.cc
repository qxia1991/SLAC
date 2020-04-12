//______________________________________________________________________________
//                                                                        
// EXOUWireSignal                                                           
//
// Encapsulation of U-wire signal
// 

#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "TROOT.h"
#include <string>
#include <iostream>

ClassImp( EXOUWireSignal )

//______________________________________________________________________________
EXOUWireSignal::EXOUWireSignal() : TObject()
{
  EXOUWireSignal::Clear();
  gROOT->GetListOfCleanups()->Add(this);
}
//______________________________________________________________________________
EXOUWireSignal::~EXOUWireSignal() 
{
  gROOT->GetListOfCleanups()->Remove(this);
}
//______________________________________________________________________________
void EXOUWireSignal::Clear( Option_t* ) 
{
    fChannel              = -999;		
    fRawEnergy            = 0;			
    fRawEnergyError       = 0;
    fCorrectedEnergy      = 0;
    fCorrectedEnergyError = 0;
    fTime                 = 0;			
    fTimeError            = 0;		
    fBaseline             = 0;		
    fBaselineError        = 0;	
    fChiSquare            = 0;
    fChiSquareRestr       = 0;
    fChiSquareInd         = 0;
    fChiSquareIndRestr    = 0;
    fRisetime             = -999.9;
    fUnshapedInteg        = -999.9;
    fChiSqRatio           = -999.9;
    fMaxMinTime           = -999.9;
    fMaxNeighEng          = -999.9;
    fIsInduction          = false;
    fAPDFilteredWFPeakMagnitude = -999;
    fAPDFilteredWFPeakTime  = -999;
    fAPDFilteredWFThreshold = -999;
	fMixSigFitChi2 = -999;								// Chi Squared result of the mixed signal fit
	fMixSigColPar = 0;								//Mixed Signal Collection Parameter
	fMixSigColErr = 0;									//Mixed Signal Collection Error
	fMixSigColTime = 0;								//Mixed Signal Collection Time Par
	fMixSigColErrTime = 0;						//Mixed Signal Collection Time Error
	fMixSigIndPar = 0;								//Mixed Signal Induction Parameter
	fMixSigIndErr = 0;									//Mixed Signal Induction Error
	fMixSigIndTime = 0;								//Mixed Signal Induction Time Par
	fMixSigIndErrTime = 0;							//MixedSignal Ind Time Error
	fMixSigClusterFound = -1;						//Index of cluster IsInd tag was added to. Set by EXOUWireMixSigAddModule.
	fMixSigUinClustFound = -1;						//Index of the Uwire in the Cluster the IsInd tag is closest to.
	fMixSigDuplicateTag = false;					//Set in EXOUWireMisSigAddModule. Says if duplicated IsInd Tag.
    fRefChargeClusters.Clear();
}
//______________________________________________________________________________
void EXOUWireSignal::RecursiveRemove(TObject* obj) 
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
      if(!fRefChargeClusters.At(i)) {
        fRefChargeClusters.RemoveAt(i); 
      }
    }
  } 
 
  // We have to try to compress because the above call might still return 0,
  // meaning the object is already removed from the TProcessID object.
  fRefChargeClusters.Compress();
}
//______________________________________________________________________________
void EXOUWireSignal::InsertChargeCluster( EXOChargeCluster* aValue ) 
{ 
  //Add a charge cluster that this u-wire signal is associated to 
  if(fRefChargeClusters.IsEmpty()) fRefChargeClusters = EXOTRefArray(TProcessID::GetProcessWithUID(aValue));
  fRefChargeClusters.Add(aValue);
}
//______________________________________________________________________________
EXOChargeCluster* EXOUWireSignal::GetChargeClusterAt( size_t index ) 
{ 
  // Get an associated charge cluster
  return static_cast< EXOChargeCluster* >( fRefChargeClusters.At(index) ); 
}
//______________________________________________________________________________
const EXOChargeCluster* EXOUWireSignal::GetChargeClusterAt( size_t index ) const 
{ 
  // Get an associated charge cluster
  return static_cast< const EXOChargeCluster* >( fRefChargeClusters.At(index) ); 
}
//______________________________________________________________________________
void EXOUWireSignal::Print( Option_t* /*opt*/ ) const
{
  // Output a summary of the contents of this charge cluster to stdout in human-readable form.

  using namespace std;
  cout<<string(100, '-')<<endl;
  cout<<"U-wire signal summary:"<<endl;
  cout<<"Channel number:  "<<fChannel<<endl;
  cout<<"Collection time:  "<<fTime<<" +/- "<<fTimeError<<" ns."<<endl;
  cout<<"Raw energy:  "<<fRawEnergy<<" +/- "<<fRawEnergyError<<" keV."<<endl;
  if( fCorrectedEnergy > 0.0 ) cout<<"Corrected energy:  "<<fCorrectedEnergy<<" +/- "<<fCorrectedEnergyError<<" keV."<<endl;
  cout<<"Channel baseline:  "<<fBaseline<<" +/- "<<fBaselineError<<" ADC counts."<<endl;
  cout<<"Chi^2 of the fit to this channel:  "<<fChiSquare<<endl;
  cout<<"Chi^2 of the fit to this channel (restricted window):  "<<fChiSquareRestr<<endl;
  cout<<"Chi^2 of the fit to this channel (induction template):  "<<fChiSquareInd<<endl;
  cout<<"Chi^2 of the fit to this channel (induction template, restricted window):  "<<fChiSquareIndRestr<<endl;
  cout<<"The risetime of the signal is:  "<<fRisetime<<endl;
  cout<<"The normalized integral of the unshaped pulse is: "<<fUnshapedInteg<<endl;
  cout<<"log_10( Deposition fit chi^2/Induction fit chi^2 ): "<<fChiSqRatio<<endl;
  cout<<"Time between maximum and minimum: "<<fMaxMinTime<<endl;
  cout<<"Max raw energy on neighboring channel: "<<fMaxNeighEng<<endl;
  cout<<"Flag tagging event as induction-like is: "<<fIsInduction<<endl;
  cout<<"APD Filtered WF Peak Value is: "<<fAPDFilteredWFPeakMagnitude<<endl;
  cout<<"APD Filtered WF Peak Time is: "<<fAPDFilteredWFPeakTime<<endl;
  cout<<"APD Filtered WF Threshold is: "<<fAPDFilteredWFThreshold<<endl;

  cout<<string(100, '-')<<endl;
}
//______________________________________________________________________________
bool EXOUWireSignal::operator==( const EXOUWireSignal& other ) const
{
   bool retVal = fChannel == other.fChannel and		
          fRawEnergy == other.fRawEnergy and			
          fRawEnergyError == other.fRawEnergyError and
          fCorrectedEnergy == other.fCorrectedEnergy and
          fCorrectedEnergyError == other.fCorrectedEnergyError and
          fTime == other.fTime and			
          fTimeError == other.fTimeError and		
          fBaseline == other.fBaseline and		
          fBaselineError == other.fBaselineError and
          fChiSquare == other.fChiSquare and
          fChiSquareRestr == other.fChiSquareRestr and
          fChiSquareInd == other.fChiSquareInd and
          fChiSquareIndRestr == other.fChiSquareIndRestr and
          fRisetime == other.fRisetime and
          fUnshapedInteg == other.fUnshapedInteg and
          fChiSqRatio == other.fChiSqRatio and
          fMaxMinTime == other.fMaxMinTime and
          fMaxNeighEng == other.fMaxNeighEng and
          fIsInduction == other.fIsInduction and
          fRisetime == other.fRisetime and
          fAPDFilteredWFPeakMagnitude == other.fAPDFilteredWFPeakMagnitude and
          fAPDFilteredWFPeakTime  == other.fAPDFilteredWFPeakTime  and
          fAPDFilteredWFThreshold == other.fAPDFilteredWFThreshold and
          GetNumChargeClusters() == other.GetNumChargeClusters() and
		  fMixSigFitChi2 == other.fMixSigFitChi2 and								// Chi Squared result of the mixed signal fit
	      fMixSigColPar == other.fMixSigColPar and								//Mixed Signal Collection Parameter
	      fMixSigColErr == other.fMixSigColErr and									//Mixed Signal Collection Error
		  fMixSigColTime == other.fMixSigColTime and								//Mixed Signal Collection Time Par
		  fMixSigColErrTime == other.fMixSigColErrTime and						//Mixed Signal Collection Time Error
		  fMixSigIndPar == other.fMixSigIndPar and								//Mixed Signal Induction Parameter
		  fMixSigIndErr == other.fMixSigIndErr and									//Mixed Signal Induction Error
		  fMixSigIndTime == other.fMixSigIndTime and								//Mixed Signal Induction Time Par
		  fMixSigIndErrTime == other.fMixSigIndErrTime;
   if (not retVal) return retVal;
   if (not TestBit(kIsReferenced)) {
     // We can test the charge clusters
     for(size_t i=0; i<GetNumChargeClusters(); i++){
       if(not (*GetChargeClusterAt(i) == *other.GetChargeClusterAt(i))){
         return false;
       }
     }
   }
   return true;
}
