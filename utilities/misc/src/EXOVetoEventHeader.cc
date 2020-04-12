//______________________________________________________________________________
//
// EXOVetoEventHeader
//
// Encapsulates information belonging to the veto event header.
//
#include "EXOUtilities/EXOVetoEventHeader.hh"
#include <limits>
ClassImp( EXOVetoEventHeader )

//______________________________________________________________________________
  void EXOVetoEventHeader::Clear( Option_t* )
{
  // Reset header to initial state. 
  TObject::Clear();
  
  //veto events
  fVetoVersion = 0;
  fVetoRunNumber = 0;
  fVetoEventNumber = 0;
  fVetoUTime = 0;
  fVetoMuTime = 0;
  fVetoTime = -std::numeric_limits<Double_t>::infinity(); // Negative infinity -- to make sure it stands out among the big numbers.
  fVetoTimeError = false;
  fIntegrityError = false;
  fVetoPanelHit.Clear();
}

bool EXOVetoEventHeader::operator==(const EXOVetoEventHeader& rec) const
{
  // compare two event headers
  return ( fVetoVersion == rec.fVetoVersion and
           fVetoRunNumber == rec.fVetoRunNumber and
           fVetoEventNumber == rec.fVetoEventNumber and
           fVetoUTime == rec.fVetoUTime and
           fVetoMuTime == rec.fVetoMuTime and
           fVetoTime == rec.fVetoTime and 
           fVetoTimeError == rec.fVetoTimeError and 
           fIntegrityError == rec.fIntegrityError and
           fVetoPanelHit == rec.fVetoPanelHit
         ); 

}
