#ifndef EXOMCVetoPanelHitInfo_hh
#define EXOMCVetoPanelHitInfo_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif


class EXOMCVetoPanelHitInfo : public TObject 
{

  public:
    Int_t          fPanelID;    //panelid  : Panel ID number
    Int_t          fNumHits;    //veto_hits: Number of hits in this veto panel
    Double_t       fTime;       //tveto    : Time of first hit in this veto panel
    Double_t       fEnergy;     //epanel   : Total energy deposited in this veto panel
	
	// Not yet implemented:
    // vector<Double_t> fPosition; //position : Energy-averaged position from hits in time window

  public:
    EXOMCVetoPanelHitInfo() : TObject() { EXOMCVetoPanelHitInfo::Clear(); }
    EXOMCVetoPanelHitInfo(Int_t panelID, Double_t time) : 
      TObject(), fPanelID(panelID), fTime(time) {}

    virtual Bool_t IsSortable() const { return true; }
    virtual Int_t Compare(const TObject* obj) const;
    virtual void Clear( Option_t* = "" )
      { fPanelID = -1; fTime = 0.0; fEnergy = 0.0; fNumHits = 0; }

    EXOMCVetoPanelHitInfo& operator+=(const EXOMCVetoPanelHitInfo& other)
      { fEnergy += other.fEnergy; fNumHits += other.fNumHits; return *this; }

  ClassDef( EXOMCVetoPanelHitInfo, 1 )

};

#endif

