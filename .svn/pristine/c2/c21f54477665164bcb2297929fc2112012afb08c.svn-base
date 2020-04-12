#ifndef EXODataSet_hh
#define EXODataSet_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#include <string>
#include <vector>
#include <iterator>

class EXORunInfo;
class EXODataSet : public TObject {
  public:
  
    size_t GetNumRuns() const { return size(); }
    const EXORunInfo& GetRunAt(size_t i) const;

    size_t size() const { return fRunList.size(); }
    const EXORunInfo& operator[](size_t i) const
      { return GetRunAt(i); }

    const std::string& GetFilterString() const { return fFilterString; }
    const std::string& GetDirectory() const { return fDirectory; }
    void Print(Option_t* opt = "") const;

    class DSiterator : public std::iterator<std::input_iterator_tag, const EXORunInfo> {
      public:
        DSiterator& operator++() 
          { ++_state; return *this; } 
        DSiterator operator++(int) 
          { DSiterator tmp(*this); ++(*this); return tmp; }
        bool operator==(const DSiterator& o) const 
          { return _ds == o._ds && _state == o._state; }
        const EXORunInfo& operator*()
          { return (*_ds)[_state]; }

      private:
        friend class EXODataSet;
        DSiterator(const EXODataSet* ds) : _ds(ds), _state(0) {}
        const EXODataSet* _ds;
        size_t _state;
    };

    DSiterator begin();
    DSiterator end();

  private:
    friend class EXORunInfoManager;
    EXODataSet(const std::string& dir, 
               const std::string& filter) : 
      fDirectory(dir),
      fFilterString(filter) {} 

    std::string fDirectory;
    std::string fFilterString;
    std::vector<Int_t> fRunList;
    
  ClassDef( EXODataSet, 0 ) // Object that encapsulates a data set 
};

#endif /* EXODataSet_hh */
