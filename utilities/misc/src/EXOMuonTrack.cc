//______________________________________________________________________________
//
// EXOMuonTrack
//
// Encapsulates information about reconstructed muon tracks.

#include <iostream>
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOMuonTrack.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"
#include "TROOT.h"

ClassImp( EXOMuonTrack )

//______________________________________________________________________________
void EXOMuonTrack::Clear( Option_t* )
{
  // Clear event, reset to initial state
  fTheta = 0.0;
  fPhi = 0.0;
  fMaximumDriftTime = 0.0;
  fNumUHits = 0;
  fNumUHitsOnTrack = 0;
  fNumVHits = 0;
  fNumVHitsOnTrack = 0;
  fTPCHalf = kUndefined;
}

//______________________________________________________________________________
EXOMuonTrack::EXOMuonTrack() :
  TObject()
{
  EXOMuonTrack::Clear();
  gROOT->GetListOfCleanups()->Add(this);
}

//______________________________________________________________________________
EXOMuonTrack::EXOMuonTrack(const EXOMuonTrack &other) : TObject(other)
{
  fTheta = other.fTheta;
  fPhi = other.fPhi;
  fMaximumDriftTime = other.fMaximumDriftTime;
  fNumUHits = other.fNumUHits;
  fNumUHitsOnTrack = other.fNumUHitsOnTrack;
  fNumVHits = other.fNumVHits;
  fNumVHitsOnTrack = other.fNumVHitsOnTrack;
  fTPCHalf = other.fTPCHalf;
}
//______________________________________________________________________________
EXOMuonTrack& EXOMuonTrack::operator=(const EXOMuonTrack &other)
{
  if (this == &other) return *this;
  fTheta = other.fTheta;
  fPhi = other.fPhi;
  fMaximumDriftTime = other.fMaximumDriftTime;
  fNumUHits = other.fNumUHits;
  fNumUHitsOnTrack = other.fNumUHitsOnTrack;
  fNumVHits = other.fNumVHits;
  fNumVHitsOnTrack = other.fNumVHitsOnTrack;
  fTPCHalf = other.fTPCHalf;
  return *this;
}
//______________________________________________________________________________
bool EXOMuonTrack::IsMuon() const
{
  // It's a muon if a track was reconstructed for it
  return (fTheta >= 0);
}
//______________________________________________________________________________
bool EXOMuonTrack::IsPossibleMuon() const
{
  return (IsMuon() || (fNumUHitsOnTrack >= 4 || fNumVHitsOnTrack >= 4));
}

//______________________________________________________________________________
void EXOMuonTrack::Print( Option_t *opt) const
{
  TString TPC_string;
  switch(fTPCHalf) {
  case kTPC1:
    TPC_string = "TPC 1";
    break;
  case kTPC2:
    TPC_string = "TPC 2";
    break;
  default:
    TPC_string = "Undefined TPC";
    break;
  }
  if (IsMuon()) {
    std::cout << "Muon Track in " << TPC_string << ":" << std::endl;
    std::cout << Form("  Zenith Angle:    %1.2f", fTheta) << std::endl;
    std::cout << Form("  Azimuthal Angle: %1.2f", fPhi) << std::endl;
  } else if (IsPossibleMuon()) {
    std::cout << "Possible Muon Track in " << TPC_string << ":" << std::endl;
  } else {
    std::cout << "Something unclassified in " << TPC_string << ":" << std::endl;
  }
  std::cout << Form("  %i of %i U-wire hits fell on the track.",
		    fNumUHitsOnTrack, fNumUHits) << std::endl;
  std::cout << Form("  %i of %i V-wire hits fell on the track.",
		    fNumVHitsOnTrack, fNumVHits) << std::endl;
  std::cout << Form("  Maximum drift time for charge on the track was %3.2f us.", fMaximumDriftTime/CLHEP::microsecond) << std::endl;
}

//______________________________________________________________________________
bool EXOMuonTrack::operator==( const EXOMuonTrack &other) const
{
  bool retVal = ((fTheta == other.fTheta) and
		 (fPhi == other.fPhi) and
		 (fMaximumDriftTime == other.fMaximumDriftTime) and
		 (fNumUHits == other.fNumUHits) and
		 (fNumUHitsOnTrack == other.fNumUHitsOnTrack) and
		 (fNumVHits == other.fNumUHits) and
		 (fNumVHitsOnTrack == other.fNumVHitsOnTrack) and
		 (fTPCHalf == other.fTPCHalf));

  return retVal;
}
//______________________________________________________________________________
EXOMuonTrack::~EXOMuonTrack()
{
  gROOT->GetListOfCleanups()->Remove(this);
}

//______________________________________________________________________________
void EXOMuonTrack::RecursiveRemove(TObject* obj) 
{
  // Not yet implemented

}
