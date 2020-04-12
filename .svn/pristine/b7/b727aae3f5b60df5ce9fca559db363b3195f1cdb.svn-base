#ifndef EXOMCAPDHitInfo_hh
#define EXOMCAPDHitInfo_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class EXOMCAPDHitInfo : public TObject 
{

  public:
    Int_t          fNumHits;    //apd_hits : Number of hits deposited on an APD as calculated using fast APD simulation  
    Int_t          fGangNo;     //chanapd  : Gang number of hit 
    Double_t       fEnergy;     //eapd     : Energy deposited in APD
    Double_t       fCharge;     /*qapd     : Charge deposited in APD (energy
                                             multiple by gain), but assumed to have the same units as fNumHits.*/
    Double_t       fTime;       //tapd     : Time of APD event

  public:
    EXOMCAPDHitInfo() : TObject() { EXOMCAPDHitInfo::Clear(); }
    EXOMCAPDHitInfo(Int_t gang, Double_t time, Double_t charge) : 
      TObject(), fGangNo(gang), fCharge(charge), fTime(time){}

    virtual Bool_t IsSortable() const { return true; }
    virtual Int_t Compare(const TObject* obj) const;
    virtual void Clear( Option_t* = "" ) 
    { fCharge = 0.0; fGangNo = 0; fTime = 0.0; fEnergy = 0.0; fNumHits = 0; }
    EXOMCAPDHitInfo& operator+=(const EXOMCAPDHitInfo& other)
    { fEnergy += other.fEnergy; fNumHits += other.fNumHits; 
      fCharge += other.fCharge; return *this; }

  ClassDef( EXOMCAPDHitInfo, 2 )
};

#endif /* EXOMCAPDHitInfo_hh */
