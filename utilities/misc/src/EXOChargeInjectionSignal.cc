//______________________________________________________________________________
//                                                                        
// EXOChargeInjectionSignal                                                           
//
// Encapsulation of charge injection signal
// 

#include "EXOUtilities/EXOChargeInjectionSignal.hh"
#include <iostream>
#include <string>

ClassImp( EXOChargeInjectionSignal )

//______________________________________________________________________________
void EXOChargeInjectionSignal::Clear( Option_t* ) 
{
    fChannel              = -999;		
    fMagnitude            = 0;			
    fMagnitudeError       = 0;
    fTime                 = 0;			
    fTimeError            = 0;		
    fBaseline             = 0;		
    fBaselineError        = 0;	
    fChiSquare            = 0;
}
//______________________________________________________________________________
void EXOChargeInjectionSignal::Print( Option_t* /*opt*/ ) const
{
  // Output a summary of the contents of this charge cluster to stdout in human-readable form.

  using namespace std;
  cout<<string(100, '-')<<endl;
  cout<<"Charge injection signal summary:"<<endl;
  cout<<"Channel number:  "<<fChannel<<endl;
  cout<<"Signal time:  "<<fTime<<" +/- "<<fTimeError<<" ns."<<endl;
  cout<<"Magnitude energy:  "<<fMagnitude<<" +/- "<<fMagnitudeError<<" ADC counts."<<endl;
  cout<<"Channel baseline:  "<<fBaseline<<" +/- "<<fBaselineError<<" ADC counts."<<endl;
  cout<<"Chi^2 of the fit to this channel:  "<<fChiSquare<<endl;
  cout<<string(100, '-')<<endl;
}
//______________________________________________________________________________
bool EXOChargeInjectionSignal::operator==( const EXOChargeInjectionSignal& other ) const
{
   bool retVal = fChannel == other.fChannel and		
          fMagnitude == other.fMagnitude and			
          fMagnitudeError == other.fMagnitudeError and
          fTime == other.fTime and			
          fTimeError == other.fTimeError and		
          fBaseline == other.fBaseline and		
          fBaselineError == other.fBaselineError and
          fChiSquare == other.fChiSquare;
   return retVal;
}
