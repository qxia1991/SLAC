#ifndef EXOUWireInductionSignal_hh
#define EXOUWireInductionSignal_hh
#ifndef EXOTRefArray_hh
#include "EXOUtilities/EXOTRefArray.hh"
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class EXOChargeCluster;
class EXOUWireInductionSignal : public TObject {
 public:
  Int_t    fChannel;              // signal channel number
  Double_t fMagnitude;            // signal energy in keV
  Double_t fMagnitudeError;       // signal energy uncertainty
  Double_t fTime;                 // signal time in nanoseconds
  Double_t fTimeError;            // signal time uncertainty
  Double_t fBaseline;             // baseline ADC counts
  Double_t fBaselineError;        // baseline uncertainty
  Double_t fChiSquare;            // chi^2 of signal fit
  Double_t fChiSquareRestr;       // chi^2 of signal fit to restricted range

  // Get/Set associated charge cluster
  void InsertChargeCluster( EXOChargeCluster *aValue );
  EXOChargeCluster *GetChargeClusterAt( size_t index );
  const EXOChargeCluster *GetChargeClusterAt( size_t index ) const;
  size_t GetNumChargeClusters() const;

 public:
  EXOUWireInductionSignal();
  ~EXOUWireInductionSignal();
  virtual void Clear( Option_t *opt = "" ); 
  virtual void RecursiveRemove(TObject *obj);//remove objects from wire signals
  virtual void Print( Option_t *opt = "" ) const;
  bool operator==(const EXOUWireInductionSignal& sig) const;

 protected:
  EXOTRefArray fRefChargeClusters;//clusters this signal went into
  ClassDef(EXOUWireInductionSignal,1)
};

inline size_t EXOUWireInductionSignal::GetNumChargeClusters() const
{
  // Number of charge clusters this u-wire signal went into.
  return fRefChargeClusters.GetLast() + 1;
}

#endif /* EXOUWireInductionSignal_hh */

