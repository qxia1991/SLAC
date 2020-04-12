#ifndef EXOWaveformData_hh
#define EXOWaveformData_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOTClonesArray_hh
#include "EXOUtilities/EXOTClonesArray.hh"
#endif
#include <map> 
#include <cstddef> //for size_t

class EXOWaveform;
class EXOWaveformData : public TObject
{

  public:


    Int_t fNumSamples;  /*nsample : convenience variable, number of samples in\
                                    the waveforms.*/ 
  public:

    EXOWaveformData(); 
    EXOWaveformData(const EXOWaveformData&);
    EXOWaveformData& operator=(const EXOWaveformData&);
    virtual ~EXOWaveformData(); 
    
    const EXOWaveform* GetWaveform( size_t i ) const; 
    const EXOTClonesArray* GetWaveformArray() const;

    EXOWaveform* GetWaveformToEdit( size_t i );
    EXOWaveform* GetWaveformWithChannelToEdit( Int_t channel ); 
    EXOWaveform* GetNewWaveform();

    const EXOWaveform* GetWaveformWithChannel( Int_t channel ) const; 
    size_t GetNumWaveforms() const; // nsig : Get number of waveforms in data
    
    
    virtual void SortByChannel();

    virtual void Compress(bool safe=false);
    virtual void Decompress();

    virtual void Remove(EXOWaveform* wf);
    virtual void Clear( Option_t* option = "");

    //EXOWaveformData& operator+=(const EXOWaveformData& other); 

    // Returns true when data has been modified from its original state.
    bool HasWFDataBeenModified() { return fWFsHaveBeenEdited; }

    bool ContainsAllWaveformsIn(const EXOWaveformData& comp) const;
    bool operator==(const EXOWaveformData& comp) const;
    
  protected:

    EXOTClonesArray* GetWaveformArray();
    void InitializeArrays();
    void InitializeWaveforms();
    Bool_t  fWFsHaveBeenEdited; /* editedwfs : A flag that returns            \
                                   true when these wf data have been modified \
                                   from the original binary data, for         \
                                   example through module processing. */ 

    size_t FindWaveformIndexWithChannel( Int_t channel ) const;

    // Raw Data
    typedef std::map<Int_t, size_t> WFMap;
    TClonesArray*     fWaveforms;      //->Array of all the waveforms
    mutable WFMap     fChanMap;        //! Channel map to give index of channels
    
  ClassDef( EXOWaveformData, 3 )

};

//---- inlines -----------------------------------------------------------------

inline EXOTClonesArray* EXOWaveformData::GetWaveformArray() 
{ 
  // Get the waveform array, the normal way to access is using GetWaveform
  // and GetNumWaveforms 
  return static_cast< EXOTClonesArray* >( fWaveforms ); 
}

inline const EXOTClonesArray* EXOWaveformData::GetWaveformArray() const 
{ 
  // Get the waveform array, the normal way to access is using GetWaveform
  // and GetNumWaveforms 
  return static_cast< const EXOTClonesArray* >( fWaveforms ); 
}

inline size_t EXOWaveformData::GetNumWaveforms() const
{ 
  // Get the number of waveforms in the data
  return ( fWaveforms ) ? fWaveforms->GetEntriesFast() : 0; 
}


#endif
