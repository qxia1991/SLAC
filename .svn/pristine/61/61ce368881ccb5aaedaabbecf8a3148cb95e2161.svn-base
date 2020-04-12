#ifndef EXOTRefArray_hh
#define EXOTRefArray_hh

#ifndef ROOT_TRefArray
#include "TRefArray.h"
#endif

class EXOTRefArray : public TRefArray {

  // An initial wrapper of TClonesArray, in case 
  // extra functionality is required.
  public:

    EXOTRefArray(TProcessID *pid = 0) : TRefArray(pid) {}
    EXOTRefArray(Int_t s, TProcessID *pid) : TRefArray(s, pid) {}
    EXOTRefArray(Int_t s, Int_t lowerBound = 0, TProcessID *pid = 0) : TRefArray(s, lowerBound, pid) {}
    EXOTRefArray(const EXOTRefArray &a) : TRefArray(a) {}
    EXOTRefArray& operator=(const EXOTRefArray &a);

    TObject *operator[](Int_t i) const;
    TObject *At(Int_t idx) const;
    TObject *AtWithoutTable(Int_t idx) const;
    void Compress();

  ClassDef( EXOTRefArray, 1 )
};

#endif /* EXOTClonesArray_hh */
