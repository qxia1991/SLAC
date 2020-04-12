#ifndef EXOTClonesArray_hh
#define EXOTClonesArray_hh

#ifndef ROOT_TClonesArray
#include "TClonesArray.h"
#endif

class EXOTClonesArray : public TClonesArray {

  // An initial wrapper of TClonesArray, in case 
  // extra functionality is required.
  public:
    EXOTClonesArray() : TClonesArray() {}

    EXOTClonesArray( const TClonesArray& array ) : TClonesArray( array ) {}

    EXOTClonesArray( const char* classname, Int_t size = 1000, Bool_t call_dtor = false ) : 
      TClonesArray( classname, size, call_dtor ) {}

    EXOTClonesArray( const TClass* aClass, Int_t size = 1000, Bool_t call_dtor = false ) : 
      TClonesArray( aClass, size, call_dtor ) {}

    virtual EXOTClonesArray& operator=(const TClonesArray& array);
    virtual TObject *&operator[](Int_t idx);
    virtual TObject *operator[](Int_t idx) const { return TClonesArray::operator[](idx); }
    virtual TObject *GetNewOrCleanedObject(Int_t idx);
    virtual void Clear(Option_t *opt);
    virtual void RemoveAndCompress(TObject *obj);

  ClassDef( EXOTClonesArray, 1 )
};

#endif /* EXOTClonesArray_hh */
