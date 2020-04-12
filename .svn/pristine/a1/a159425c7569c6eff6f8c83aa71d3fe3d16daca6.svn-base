// Class to encapsulate all EXO Monte Carlo veto panel hit info.

#include "EXOUtilities/EXOMCVetoPanelHitInfo.hh"
ClassImp( EXOMCVetoPanelHitInfo )


Int_t EXOMCVetoPanelHitInfo::Compare(const TObject* obj) const
{ 
  // Compare two veto panel hit infos by first panel number and then time. Returns
  // 0 when equal, -1 when this object is smaller than obj, +1 when bigger.
  if ( obj == this ) return 0;

  const EXOMCVetoPanelHitInfo* other = static_cast<const EXOMCVetoPanelHitInfo*>(obj);

  if (other->fPanelID != fPanelID) return ( fPanelID < other->fPanelID ) ? -1 : 1;  
  if (other->fTime != fTime) return ( fTime < other->fTime ) ? -1 : 1;

  // This should not be done for absolute clarity, since return value 0
  // indicates equality of compared objects, which is already tested.
  // Zero should be returned only for equality of objects.
  return 0;
}

