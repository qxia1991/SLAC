//_____________________________________________________________________________
//                                                                        
// EXOMuonData                                                          
//                                                                        
// Encapsulates the muon data of an EXO event.
//
// It contains arrays of the following:
//
//   EXOMuonTrack          // the track details


#include "EXOUtilities/EXOMuonData.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"

ClassImp( EXOMuonData )

//_____________________________________________________________________________
EXOMuonData::EXOMuonData() : 
  TObject(),
  fMuonTracks(0)
{
  // Default constructor, clear and setup arrays
  EXOMuonData::Clear();
  InitializeArrays();
}

//_____________________________________________________________________________
void EXOMuonData::InitializeArrays()
{
  InitializeMuonTracks();
}

//_____________________________________________________________________________
void EXOMuonData::InitializeMuonTracks()
{
  // Initialize muon track array, gives an initial *guess* of MAXCOLL_TOT size
  fMuonTracks = 
    new EXOTClonesArray( EXOMuonTrack::Class(), MAXCOLL_TOT ); 
}

//_____________________________________________________________________________
EXOMuonData::~EXOMuonData()
{
  delete fMuonTracks;
}

//_____________________________________________________________________________
void EXOMuonData::Clear( Option_t* /*option*/ )
{
  // Clear Data, reset to initial state.  Clear resets all the arrays.  It is
  // important to note that no destructors are called, instead all the TObject
  // components of the TClonesArrays are Cleared.  This is to avoid excessive
  // new/deletes during Fill.  See the documentation for TClonesArrays for more
  // information.

  fMonteCarloTheta                  = 0;
  fMonteCarloPhi                    = 0;

  if ( fMuonTracks ) fMuonTracks->Clear( "C" );
}

//_____________________________________________________________________________
EXOMuonData::EXOMuonData(const EXOMuonData& other) : TObject(other) 
{
  // Copy constructor

  fMonteCarloTheta = other.fMonteCarloTheta;
  fMonteCarloPhi = other.fMonteCarloPhi;

  fMuonTracks = new EXOTClonesArray(*other.fMuonTracks);
}

//_____________________________________________________________________________
EXOMuonData& EXOMuonData::operator=(const EXOMuonData& other)
{
  // Copy constructor
  if (this == &other) return *this;

  fMonteCarloTheta = other.fMonteCarloTheta;
  fMonteCarloPhi = other.fMonteCarloPhi;

  *static_cast<EXOTClonesArray*>(fMuonTracks) = *other.fMuonTracks;

  return *this;
}
//______________________________________________________________________________
bool EXOMuonData::operator==(const EXOMuonData& other) const
{
  // Do deep check of equivalence.

  bool retVal = 
    fMonteCarloTheta == other.fMonteCarloTheta and
    fMonteCarloPhi == other.fMonteCarloPhi and
    GetNumMuonTracks() == other.GetNumMuonTracks();

  if (not retVal) return false;

  for (size_t i = 0; i < GetNumMuonTracks(); ++i) {
    if (not (*GetMuonTrack(i) == *other.GetMuonTrack(i))) {
      return false;
    }
  }

  return true;
}

//_____________________________________________________________________________
Bool_t EXOMuonData::ContainsMuon() const
{
  for(size_t i = 0; i < GetNumMuonTracks(); ++i) {
    if (GetMuonTrack(i)->IsMuon()) {
      return true;
    }
  }
  return false;
}

//_____________________________________________________________________________
Bool_t EXOMuonData::ContainsPossibleMuon() const
{
  for(size_t i = 0; i < GetNumMuonTracks(); ++i) {
    if (GetMuonTrack(i)->IsPossibleMuon()) {
      return true;
    }
  }
  return false;
}
