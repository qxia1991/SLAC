#ifndef EXOWaveform_hh
#define EXOWaveform_hh

#ifndef EXOTemplWaveform_hh
#include "EXOUtilities/EXOTemplWaveform.hh" 
#endif

#include "EXOUtilities/EXOMiscUtil.hh"
#include <cstddef> //for size_t

class EXOWaveform : public EXOIntWaveform {

  /* Class encapsulating an waveform */
  public:
  
    // Set the data by inputting an array
    // The following handles all basic types.
    template<typename T>
    void SetData( const T* aData, size_t numberOfValues ); 

    const UShort_t* GetCompressedData() const 
      { return (fCompressedData.size() > 0) ? &(fCompressedData)[0] : NULL; }
    void   SetCompressedData( const UShort_t* aData, size_t numberOfValues ); 
    size_t GetCompressedLength() const;

    // Compress/Decompress the waveform
    void   Compress(bool safe = false);
    void   Decompress();

    UShort_t GetCompressedDataPoint(size_t i) const 
      { return fCompressedData[i]; }

    Bool_t IsCompressed() const { return fIsCompressed; }

    virtual Int_t Compare( const TObject* obj ) const; 
    virtual Bool_t IsSortable() const; 
    virtual const char* GetName() const;

    EXOMiscUtil::ETPCSide GetTPCSide() const;
    EXOMiscUtil::EChannelType GetChannelType() const;

    bool operator==(const EXOWaveform& comp) const;

  public:
    EXOWaveform();
    EXOWaveform( const Int_t* aData, size_t length );
    virtual ~EXOWaveform();
    virtual void Clear( Option_t* = "" );
 
  public:
    Int_t                 fChannel;           // wfchan : Channel of waveform 

  protected:
    Bool_t                fIsCompressed;      // wfcompress : if waveform is compressed or not 
    std::vector< UShort_t >  fCompressedData; // qdata : compressed data
    ULong64_t             fLastVectorSize;    // Last vector size, saved for compression
    mutable std::string   fNameOfWaveform;    //! Name of Waveform, don't save
    void   SetCompressedLength( size_t length );
    void   RemoveCompressedData() { SetCompressedLength(0); fIsCompressed = false; }

  ClassDef( EXOWaveform, 4 )
};

//---- inlines -----------------------------------------------------------------

template<typename T>
inline void EXOWaveform::SetData( const T* aData, size_t numberOfValues ) 
{ 
  // Set data using an array.  This function clears out any data including
  // previously compressed data
  RemoveCompressedData();
  EXOTemplWaveform<Int_t>::SetData<T>(aData, numberOfValues);
} 

inline void EXOWaveform::SetCompressedLength( size_t length ) 
{ 
  // Set compressed length of waveform 
  fCompressedData.resize( length );
}

inline size_t EXOWaveform::GetCompressedLength() const 
{ 
  // Get length of compressed waveform
  return fCompressedData.size();
}

inline Bool_t EXOWaveform::IsSortable() const 
{
  // This object is sortable and has defined Compare().
  return true;
}

#endif /* EXOWaveform_hh */
