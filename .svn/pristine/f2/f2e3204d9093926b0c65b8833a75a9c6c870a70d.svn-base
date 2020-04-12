#ifndef EXOVetoEventHeader_hh
#define EXOVetoEventHeader_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TBits
#include "TBits.h"
#endif

class EXOVetoEventHeader : public TObject {

public:
  EXOVetoEventHeader() : TObject() { EXOVetoEventHeader::Clear(); }
  
  
  UInt_t fVetoVersion;		//veto version
  UInt_t fVetoRunNumber;        //the run in which this record occurred
  UInt_t fVetoEventNumber;	//number of veto events
  UInt_t fVetoUTime;		//unix time of veto event
  UInt_t fVetoMuTime;		//micro seconds of event
  Double_t fVetoTime;		//total time (rounding may occur at the sub-second level -- for precision, use fVetoUTime and fVetoMuTime)
  Bool_t fVetoTimeError;	//veto error
  Bool_t fIntegrityError;	//Integrity error
  TBits fVetoPanelHit;		//The veto panel that was hit.

  bool operator==(const EXOVetoEventHeader&) const;
  virtual void Clear( Option_t* = "" );
  ClassDef( EXOVetoEventHeader, 7 )

};

#endif /* EXOVetoEventHeader_hh */
