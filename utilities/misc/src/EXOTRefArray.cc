//______________________________________________________________________________
//
// EXOTRefArray
//  
//   Provides a wrapper class for TRefArray to deal with several
//   bugs/limitations not dealt with in our stock version of ROOT.
//
//______________________________________________________________________________

#include "EXOUtilities/EXOTRefArray.hh"

//______________________________________________________________________________
EXOTRefArray& EXOTRefArray::operator=(const EXOTRefArray& a)
{
  TRefArray::operator=(a);
  return *this;
}

//______________________________________________________________________________
TObject *EXOTRefArray::operator[](Int_t at) const
{
  return At(at);
}

//______________________________________________________________________________
TObject *EXOTRefArray::At(Int_t at) const
{
   // Return the object at position i. Returns 0 if i is out of bounds.
   int j = at-fLowerBound;
   if (j >= 0 && j < fSize) {
      // This properly returns 0 if the table is not available
      TObject* obj = GetFromTable(j);
      if (obj) return obj;
      if (!fPID) return 0;
      obj = fPID->GetObjectWithID(fUIDs[j]);
      return obj;
   }
   BoundsOk("At", at);
   return 0;
}

//______________________________________________________________________________
TObject *EXOTRefArray::AtWithoutTable(Int_t at) const
{
   // Return the object at position i. Returns 0 if i is out of bounds.
   // Does *not* check the ref table, which is useful if trying to call
   // recursive remove functions.
   int j = at-fLowerBound;
   if (j >= 0 && j < fSize) {
      // This properly returns 0 if the table is not available
      if (!fPID) return 0;
      return fPID->GetObjectWithID(fUIDs[j]);
   }
   BoundsOk("At", at);
   return 0;
}

//______________________________________________________________________________
void EXOTRefArray::Compress()
{
  // Ensure that everything is zeroed out above the last, compress loops up to
  // the size of the array, not the last value.
  memset(&fUIDs[fLast+1], 0, (fSize - fLast - 1)*sizeof(fUIDs[0]));
  TRefArray::Compress();
}

//______________________________________________________________________________
void EXOTRefArray::Streamer(TBuffer &R__b)
{
  TRefArray::Streamer(R__b);
  if (R__b.IsReading()) memset(&fUIDs[fLast+1], 0, (fSize - fLast - 1)*sizeof(fUIDs[0]));
}
