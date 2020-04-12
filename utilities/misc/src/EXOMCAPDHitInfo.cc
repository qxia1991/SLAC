//______________________________________________________________________________
//                                                                        
// EXOMCAPDHitInfo 
//                                                                        
// class to encapsulate all EXO Monte Carlo APD Hit Info 

#include "EXOUtilities/EXOMCAPDHitInfo.hh"
ClassImp( EXOMCAPDHitInfo )

//______________________________________________________________________________
Int_t EXOMCAPDHitInfo::Compare(const TObject* obj) const
{ 
  // Compare to APD Hit infos by first gangNo and then time Returns 0 when
  // equal, -1 when this object is smaller than obj, +1 when bigger.
  if ( obj == this ) return 0;
  const EXOMCAPDHitInfo* other = static_cast<const EXOMCAPDHitInfo*>(obj);
  if (other->fGangNo != fGangNo) return ( fGangNo < other->fGangNo ) ? -1 : 1;  
  if (other->fTime != fTime) return ( fTime < other->fTime ) ? -1 : 1;  
  return 0;
}
   
