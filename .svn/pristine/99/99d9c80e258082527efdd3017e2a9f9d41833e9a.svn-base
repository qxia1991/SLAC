#ifndef EXOControlRecordList_hh
#define EXOControlRecordList_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOControlRecord_hh
#include "EXOUtilities/EXOControlRecord.hh"
#endif
#ifndef EXOEventData_hh
#include "EXOUtilities/EXOEventData.hh"
#endif
#include <set>
#include <limits>
#include <vector>

class TClass;

class EXOControlRecordList : public TObject
{
 
  public:
    EXOControlRecordList() {}
    EXOControlRecordList(const EXOControlRecordList&);
    EXOControlRecordList(const EXOControlRecordList&, std::vector<TClass*> FilterRecords);
    EXOControlRecordList& operator=(const EXOControlRecordList&);
    virtual ~EXOControlRecordList();

    size_t size() const { return fRecords.size(); }

    const EXOControlRecord* 
      GetPreviousRecord(const EXOControlRecord& rec, TClass* ofType) const; 
    const EXOControlRecord* 
      GetNextRecord(const EXOControlRecord& rec, TClass* ofType) const; 
 
    template<class T>
    const T* GetPreviousRecord(const EXOControlRecord& rec) const;

    template<class T>
    const T* GetNextRecord(const EXOControlRecord& rec) const;


    template<class T>
    const T* GetPreviousRecord(Int_t runNumber = std::numeric_limits<Int_t>::max(), 
                               Int_t eventNumber = std::numeric_limits<Int_t>::max()) const;

    template<class T>
    const T* GetNextRecord(Int_t runNumber = 0, Int_t eventNumber = 0) const;

    template<class T>
    const T* GetPreviousRecord(const EXOEventData& event) const
    {
      // Get the previous record of the requested type with respect to the
      // EXOEventData object.
      return GetPreviousRecord<T>(event.fRunNumber, event.fEventNumber); 
    }

    template<class T>
    const T* GetNextRecord(const EXOEventData& event) const
    {
      // Get the next record of the requested type with respect to the
      // EXOEventData object.
      return GetNextRecord<T>(event.fRunNumber, event.fEventNumber); 
    }

    void Print(Option_t* opt = "") const;

    struct ControlRecordComp {
      // We make this public so other classes can check the ordering of control records.
      bool operator()(const EXOControlRecord* const& lhs, const EXOControlRecord* const& rhs) const 
      {
        // Ordering in the following manner:
        // run number, event number, time 
        if (lhs->GetRunNumber() != rhs->GetRunNumber()) { 
          return lhs->GetRunNumber() < rhs->GetRunNumber();
        }
        if (lhs->GetPreviousEventNumber() != rhs->GetPreviousEventNumber()) { 
          return lhs->GetPreviousEventNumber() < rhs->GetPreviousEventNumber();
        }
        return lhs->GetTimestamp() < rhs->GetTimestamp();
      }
    };
    
    typedef std::set<const EXOControlRecord*, ControlRecordComp> ListMap;

    class CRiterator : public std::iterator<std::input_iterator_tag, const EXOControlRecord> {
      public:
        CRiterator& operator++() 
          { ++_state; return *this; } 
        CRiterator operator++(int) 
          { CRiterator tmp(*this); ++(*this); return tmp; }
        bool operator==(const CRiterator& o) const 
          { return _state == o._state; }
        const EXOControlRecord& operator*()
          { return *(*_state); }

      private:
        friend class EXOControlRecordList;
        CRiterator(const EXOControlRecordList* ds) : 
          _state(ds->fRecords.begin()) {}
        ListMap::const_iterator _state;
    };


    CRiterator begin() const;
    CRiterator end() const;

  protected:
    friend class EXOBinaryFileInputModule;
    friend class EXOTreeInputModule;

    void Add(const EXOControlRecord& record); 
    void Add(const EXOControlRecordList& list); 


    ListMap fRecords;  // All Control records

  ClassDef(EXOControlRecordList, 1); // List of control records

};
template<class T>
const T* EXOControlRecordList::GetPreviousRecord(Int_t runNumber, Int_t eventNumber) const
{
  // Find the most recent previous record of the requested type.  If no record
  // is found, NULL is returned.  This function can also be used to get the
  // very last record in a list, by calling simply with default arguments. I.e.:
  //
  //   EXOControlRecord* rec = list->GetPreviousRecord<EXOControlRecord>();
  //
  // will return the very last record in the list.

  T test;
  test.fRunNumber   = runNumber;
  // We need to decrement the number here to make sure we get the record
  // *before* this event.
  test.fPreviousEventNumber = eventNumber-1;
  test.fTimestamp   = (ULong64_t)-1; 
  return GetPreviousRecord<T>(test);
}

template<class T>
const T* EXOControlRecordList::GetNextRecord(Int_t runNumber, Int_t eventNumber) const
{
  // Find the next record of the requested type.  If no record is found, NULL
  // is returned.  This function can also be used to get the very first record
  // in a list, by calling simply with default arguments. I.e.:
  //
  //   EXOControlRecord* rec = list->GetNextRecord<EXOControlRecord>();
  //
  // will return the very first record in the list.

  T test;
  test.fRunNumber   = runNumber;
  test.fPreviousEventNumber = eventNumber;
  test.fTimestamp   = 0; 
  return GetNextRecord<T>(test);
}


template<class T>
const T* EXOControlRecordList::GetPreviousRecord(const EXOControlRecord& rec) const
{
  // Find the most recent previous record of the requested type, with relation
  // to the input control record rec.  If no record is found, NULL is returned. 
  return static_cast<const T*>(GetPreviousRecord(rec, T::Class())); 
}

template<class T>
const T* EXOControlRecordList::GetNextRecord(const EXOControlRecord& rec) const
{
  // Find the next record of the requested type, with relation to the input
  // control record rec.  If no record is found, NULL is returned. 

  return static_cast<const T*>(GetNextRecord(rec, T::Class())); 
}


#endif /* EXOControlRecordList_hh */
