#ifndef EXOComptonInfo_hh
#define EXOComptonInfo_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class EXOComptonInfo : public TObject {

  public:
    EXOComptonInfo() : TObject() { EXOComptonInfo::Clear(); }
  
    Double_t fEnergy;           //compton_e       : energy output from compton module (usuall sum[eccl])        
    Double_t fFOM;              //compton_fom     : figure of merit for compton reconstruction        
    Int_t fNumClustersUsed;     //compton_ncl     : number of clusters used for compton reconstruction (currently = 2 or 3 )        
    Double_t fHalfAnglePhi;     //compton_phi     : half angle phi of compton cone        
    Double_t fHalfAnglePhiErr;  //compton_phi_err : error of half-alngle of compton cone        
    Double_t fX1;               //compton_x1      : position of first point of compton vector        
    Double_t fY1;               //compton_y1      : position of first point of compton vector        
    Double_t fZ1;               //compton_z1      : position of first point of compton vector        
    Double_t fX1Err;            //compton_dx1     : error on position of first point of compton vector        
    Double_t fY1Err;            //compton_dy1     : error on position of first point of compton vector        
    Double_t fZ1Err;            //compton_dz1     : error on position of first point of compton vector        
    Double_t fX2;               //compton_x2      : position of second point of compton vector        
    Double_t fY2;               //compton_y2      : position of second point of compton vector        
    Double_t fZ2;               //compton_z2      : position of second point of compton vector        
    Double_t fX2Err;            //compton_dx2     : error on position of second point of compton vector        
    Double_t fY2Err;            //compton_dy2     : error on position of second point of compton vector        
    Double_t fZ2Err;            //compton_dz2     : error on position of second point of compton vector

    virtual void Clear( Option_t* = "" );

    bool operator==(const EXOComptonInfo& ci) const;
  ClassDef( EXOComptonInfo, 1 )

};

#endif /* EXOComptonInfo_hh */
