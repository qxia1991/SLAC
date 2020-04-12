#ifndef EXOChargeInjectionSignal_hh
#define EXOChargeInjectionSignal_hh

#include "TObject.h"

class EXOChargeInjectionSignal : public TObject {
  public:
    Int_t          fChannel;              //: Charge Injection signal channel number.
    Double_t       fMagnitude;            //: Charge Injection signal energy in keV. 
    Double_t       fMagnitudeError;       //: Charge Injection signal energy uncertainty.  
    Double_t       fTime;                 //: Charge Injection signal time in nanoseconds.     
    Double_t       fTimeError;            //: Charge Injection signal time uncertainty.
    Double_t       fBaseline;             //: Charge Injection baseline ADC counts.
    Double_t       fBaselineError;        //: Charge Injection baseline uncertainty.
    Double_t       fChiSquare;            /*: Charge Injection chi^2 from fit (or something approximating it).\
                                              Note that the same chi^2 is used for all u-signals on the channel.*/

  public:
    EXOChargeInjectionSignal() : TObject() { EXOChargeInjectionSignal::Clear(); }
    virtual void Clear( Option_t* opt = "" ); 
    virtual void Print( Option_t* opt = "" ) const;
    bool operator==(const EXOChargeInjectionSignal& sig) const;

  ClassDef( EXOChargeInjectionSignal, 1 )

};

#endif /* EXOChargeInjectionSignal_hh */
