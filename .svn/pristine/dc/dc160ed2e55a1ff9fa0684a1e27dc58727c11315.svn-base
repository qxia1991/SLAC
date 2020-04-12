//______________________________________________________________________________
//
// EXOControlRecordList is a class encapsulating a set of control records.
// Accessing records within the class is done by using the templatized
// GetPreviousRecord and GetNextRecord functions.  A usage example is below: 
//
//   EXOControlRecordList myList;
//   ...
//   // Get the previous begin record
//   EXOBeginRecord* begin = myList.GetPreviousRecord(runNumber, eventNumber);
//   if (begin == NULL) // something went wrong.
//
// GetNextRecord and GetPreviousRecord are also overloaded to simply take an
// EXOEventData object:
//
//   EXOEventData* event;
//   ...
//   EXOBeginRecord* begin = myList.GetPreviousRecord(*event);
//
// There are many control record types, the return type of the function defines
// what kind of record you will receive.
// For example:
//  
//   // Get the first record previous to this event 
//   EXOControlRecord* rec = myList.GetPreviousRecord(*event);
//   // Get a very specific type (e.g. a physics begin run 
//   EXOBeginPhysicsRunRecord* phys_begin = myList.GetPreviousRecord(*event);
//
// One can also step through the records like so:
// EXOControlRecord* rec = GetNextRecord(0,0);
// while(rec) {
//   rec->Print();
//   ..
//   rec = myList.GetNextRecord(*rec);
// }
//
// For a listing/tree of types, see the ROOT-generated documentation for the
// EXOControlRecord class.
//
// In python, one can iterate over the control record list like so:
//
//   cl = ROOT.EXOControlRecordList()
//   ...
//   for rec in cl: print rec 
//
// M. Marino
//______________________________________________________________________________

#include "EXOUtilities/EXOControlRecordList.hh"
#include "TClass.h"

//______________________________________________________________________________
EXOControlRecordList::EXOControlRecordList(const EXOControlRecordList& other)
{
  // Copy constructor
  ListMap::const_iterator iter = other.fRecords.begin();
  for (;iter != other.fRecords.end(); iter++) Add(**iter); 
} 

//______________________________________________________________________________
EXOControlRecordList::EXOControlRecordList(const EXOControlRecordList& other, std::vector<TClass*> FilterRecords)
{
  // Copy constructor.
  // Copy only the records which inherit from one of the TClasses in FilterRecords.
  ListMap::const_iterator iter = other.fRecords.begin();
  for (;iter != other.fRecords.end(); iter++) {
    for(size_t i = 0; i < FilterRecords.size(); i++) {
      if((*iter)->IsA()->InheritsFrom(FilterRecords[i])) {
        Add(**iter);
        break;
      }
    }
  }
}

//______________________________________________________________________________
EXOControlRecordList& EXOControlRecordList::operator=(const EXOControlRecordList& other)
{
  // Copy operator
  ListMap::const_iterator iter = other.fRecords.begin();
  for (;iter != other.fRecords.end(); iter++) Add(**iter); 
  return *this;
}

//______________________________________________________________________________
EXOControlRecordList::~EXOControlRecordList()
{
  // Destructor, we delete the objects which we allocated.
  ListMap::iterator iter = fRecords.begin();
  for (;iter != fRecords.end(); iter++) delete *iter; 
}

//______________________________________________________________________________
void EXOControlRecordList::Add(const EXOControlRecord& record)
{
  // Add a control record to the list
  const EXOControlRecord* newRecord = static_cast<const EXOControlRecord*>(record.Clone(""));
  fRecords.insert(newRecord);
}

//______________________________________________________________________________
void EXOControlRecordList::Add(const EXOControlRecordList& list)
{
  // Add a control record to the list.  This preserves all the information in
  // both control lists, provided the records aren't duplicated.  This is
  // because the control list distinguishes between records in different runs.
  ListMap::const_iterator iter = list.fRecords.begin();
  for (;iter != list.fRecords.end(); iter++) Add(*(*iter)); 
}


//______________________________________________________________________________
void EXOControlRecordList::Print(Option_t* opt) const
{
  // Print the contents of the control record list.  This loops (forward) in
  // the list and calls Print() on each record.
  ListMap::const_iterator iter = fRecords.begin();
  for (;iter != fRecords.end(); iter++) (*iter)->Print(opt); 
}

//______________________________________________________________________________
const EXOControlRecord* EXOControlRecordList::GetPreviousRecord(
  const EXOControlRecord& rec, TClass* ofType) const
{
  // Find the most recent previous record of the requested type defined by
  // TClass of Type, with relation to the input control record rec.  If no
  // record is found, NULL is returned. 
  ListMap::const_iterator iter = fRecords.lower_bound(&rec); 
  ListMap::key_compare comp = fRecords.key_comp();
  // We now have an iterator that is equal to or greater than rec, therefore we must
  // handle the case when it is equal.  This iterates down to the first record
  // which is less.
  while (iter != fRecords.begin() and
         (iter == fRecords.end() or not comp(*iter, &rec))) iter--;
  if (iter == fRecords.end() or not comp(*iter, &rec)) return NULL;
  while (true) {
    if ((*iter)->InheritsFrom(ofType)) {
      return *iter;
    }
    if (iter == fRecords.begin()) break;
    iter--;
  }
  return NULL;

} 


//______________________________________________________________________________
const EXOControlRecord* EXOControlRecordList::GetNextRecord(
  const EXOControlRecord& rec, TClass* ofType) const 
{
  // Find the next record of the requested type defined by TClass* ofType, with
  // relation to the input control record rec.  If no record is found, NULL is
  // returned. 
  ListMap::const_iterator iter = fRecords.upper_bound(&rec); 
  if (iter == fRecords.end()) return NULL;
  for (;iter != fRecords.end(); iter++) {
    if ((*iter)->InheritsFrom(ofType)) {
      return *iter;
    }
  }
  return NULL;
}
//______________________________________________________________________________
EXOControlRecordList::CRiterator EXOControlRecordList::begin() const
{
  // beginning iterator
  return CRiterator(this); 
}
//______________________________________________________________________________
EXOControlRecordList::CRiterator EXOControlRecordList::end() const
{
  // End iterator
  CRiterator tmp(this); 
  tmp._state = fRecords.end(); 
  return tmp;
}

