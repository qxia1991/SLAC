#ifndef EXOMuonTrack_hh
#define EXOMuonTrack_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif
#include <cstddef>

class EXOMuonTrack : public TObject {
public:

  EXOMuonTrack();
  EXOMuonTrack(const EXOMuonTrack &other);
  EXOMuonTrack& operator=(const EXOMuonTrack&);
  virtual ~EXOMuonTrack();
  virtual void Clear( Option_t* opt = "" );
  virtual void Print( Option_t* opt = "" ) const;

  bool operator==(const EXOMuonTrack& other) const;

  virtual void RecursiveRemove(TObject *obj); // Auto remove object from TRefArray

  double fTheta; // The angle from the zenith
  double fPhi;   // The azimuthal angle. 0 is N (+z), pi/2 is E (-x), etc.
  double fMaximumDriftTime; // Maximum drift time between light and charge signals
  int fNumUHits; // Number of u wire hits found
  int fNumUHitsOnTrack; // Number of u wire hits actually lying on the track
  int fNumVHits; // Number of u wire hits found
  int fNumVHitsOnTrack; // Number of u wire hits actually lying on the track

  enum TPCHalf { kUndefined, kTPC1, kTPC2 } fTPCHalf;
  
  bool IsMuon() const;
  bool IsPossibleMuon() const;

  ClassDef( EXOMuonTrack, 1 )
}; 

#endif /* EXOMuonTrack_hh */
