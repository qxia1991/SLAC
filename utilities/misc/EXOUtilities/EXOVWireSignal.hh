#ifndef EXOVWireSignal_hh
#define EXOVWireSignal_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOTRefArray_hh
#include "EXOUtilities/EXOTRefArray.hh"
#endif

class EXOChargeCluster;

class EXOVWireSignal : public TObject {
  public:
    Int_t          fChannel;                 //V-wire signal channel number.
    Double_t       fMagnitude;               //V-wire signal magnitude. 
    Double_t       fMagnitudeError;          //V-wire signal magnitude uncertainty.
    Double_t       fCorrectedMagnitude;      //V-wire signal (gain) corrected magnitude.
    Double_t       fCorrectedMagnitudeError; //V-wire signal (gain) corrected magnitude uncertainty.
    Double_t       fTime;                    //V-wire signal time in nanoseconds.
    Double_t       fTimeError;               //V-wire signal time uncertainty.
    Double_t       fBaseline;                //V-wire baseline ADC counts.
    Double_t       fBaselineError;           //V-wire baseline uncertainty.
    Double_t       fChiSquare;               //V-wire chi^2 from fit (or something approximating it).
    Double_t       fChiSquareRestr;          //V-wire chi^2 from fit, over restricted window 
                                             //(which can differ from the fitting window) */

    // Get/Set associated charge cluster
    void InsertChargeCluster( EXOChargeCluster* aValue );
    EXOChargeCluster*  GetChargeClusterAt( size_t index );
    const EXOChargeCluster*  GetChargeClusterAt( size_t index ) const;
    size_t GetNumChargeClusters() const;

  public:
    EXOVWireSignal();
    ~EXOVWireSignal();
    virtual void Clear( Option_t* opt = "" ); 
    virtual void RecursiveRemove(TObject* obj);   //Auto remove objects from Wire signals
    virtual void Print( Option_t* opt = "" ) const;
    bool operator==(const EXOVWireSignal& sig) const;

  protected:
    EXOTRefArray           fRefChargeClusters; // cluster this signal went into.
  ClassDef( EXOVWireSignal, 5)

};

inline size_t EXOVWireSignal::GetNumChargeClusters() const
{
  // number of charge clusters this v-wire signal went into
  return fRefChargeClusters.GetLast() + 1;
}

#endif
