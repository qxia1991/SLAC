//______________________________________________________________________________
//                                                                        
// EXOWaveform                                                           
//                                                                        
// Encapsulates waveform data 
//  

#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXODimensions.hh"

#include <cassert>

ClassImp( EXOWaveform )

//______________________________________________________________________________
// initial definitions of compression functions to be used later
int data_to_qdata( int *data, int nsample, unsigned short *qdata, int maxqdata );
int qdata_to_data( unsigned short *qdata, int nqele, int *data, int maxdata  );
//______________________________________________________________________________
EXOWaveform::EXOWaveform() : EXOIntWaveform()
{
  EXOWaveform::Clear();
}

//______________________________________________________________________________
EXOWaveform::EXOWaveform( const Int_t* aData, size_t length ) : EXOIntWaveform() 
{
  EXOWaveform::Clear();
  SetData( aData, length );
}

//______________________________________________________________________________
EXOWaveform::~EXOWaveform()
{
}

//______________________________________________________________________________
void  EXOWaveform::SetCompressedData( const UShort_t* aData, size_t numberOfValues ) 
{
  // Set compressed data.  This will remove any data already in the waveform,
  // including any uncompressed data.  numberOfValues must be the length of 
  // the UShort_t array passed in.  This will be in the same format as the old
  // EXOEventData qdata, e.g.:
  //
  // UShort_t* aData;
  // aData[0] = channel | compression bit
  // aData[1] = len of compressed data
  // aData[2] = compressed data 
  // ...
  // aData[2+len] = compressed data 
  //
  // numberOfValues is a sanity check, and should equal 2 + len

  if (!aData || numberOfValues < 2) return;
  if (numberOfValues != (size_t)(aData[1] + 2)) {
    std::cout << "Malconstructed compressed data" << std::endl;
    return;
  }
  fChannel = aData[0] & 0xFFF;
  SetLength(0);
  fIsCompressed = true;
  // This is a guess in the size, but it just needs to be much
  // larger
  fLastVectorSize = numberOfValues*5;
  SetCompressedLength( numberOfValues );
  fCompressedData.assign( aData, aData + numberOfValues ); 
}

//______________________________________________________________________________
void EXOWaveform::Clear( Option_t* )
{
  // Reset state of waveform, does *not* deallocate arrays
  fLastVectorSize = 0;
  fChannel = -999;
  fIsCompressed = false;
  fData.resize(0);
  fCompressedData.resize(0);
}

//______________________________________________________________________________
void EXOWaveform::Compress(bool safe)
{
  // Compress the waveform.  The safe variable defines whether to make sure
  // that this data can be losslessly compressed.  That is, it checks to ensure
  // that the data packed inside the 32-bit integer is in fact 12-bits wide.
  // Default is not to check.
  //
  // It is a non-op (safe) to call this on an already compressed waveform

  if ( fIsCompressed || GetLength() == 0 ) return;    
  if (safe) {
      // Check the data to ensure that it is 12-bits
      for (size_t i=0;i<fData.size();i++) {
          if ( (fData[i] & 0xFFF ) != 0 ) {
              std::cout << 
                "Data in waveform greater than 12-bits width, aborting" 
                << std::endl;
              return;
          }
      }
  }
  SetCompressedLength( GetLength()*2 );
  int result = data_to_qdata( &(fData)[0], GetLength(), 
                 &(fCompressedData)[0], GetCompressedLength() ); 
  if ( result < 0 ) {
      std::cout << "Problem compressing..." << std::endl;
  } else {
      fLastVectorSize = GetLength();
      SetCompressedLength( result ); 
      SetLength( 0 ); 
      fIsCompressed = true;
  } 
}

//______________________________________________________________________________
void EXOWaveform::Decompress()
{
  // Decompress the waveform.
  //
  // It is a non-op (safe) to call this on an already decompressed waveform
  if ( !fIsCompressed || GetCompressedLength() == 0 ) return;    
  SetLength( fLastVectorSize );
  int result = qdata_to_data( &(fCompressedData)[0], GetCompressedLength(), &(fData)[0], GetLength() ); 
  if ( result < 0 ){
      std::cout << "Problem decompressing..." << std::endl;
  } else {
      SetLength( result ); 
      SetCompressedLength( 0 ); 
      fIsCompressed = false;
      fLastVectorSize = 0;
  } 

}

//______________________________________________________________________________
Int_t EXOWaveform::Compare( const TObject* obj ) const
{
  // Compare two waveforms.  At the moment, this compares the channel numbers.
  // This function is used to sort a TCollection of these objects.
  const EXOWaveform* wf = static_cast< const EXOWaveform* >( obj );
  if ( !wf ) return 0;
  if ( fChannel == wf->fChannel ) return 0;
  if ( fChannel < wf->fChannel ) return -1;
  return 1;
}

const char* EXOWaveform::GetName() const
{
  // Returns the name, defined by the channel
  std::stringstream ss;
  ss << "Channel " << fChannel;
  fNameOfWaveform = ss.str();
  return fNameOfWaveform.c_str(); 
}
//__________________________________________________________________________________
EXOMiscUtil::ETPCSide EXOWaveform::GetTPCSide() const
{
  // Based on the channel number, return the half of the TPC on which the channel lies.
  // Invalid channels (negative or too large) currently cause a prompt exit.

  // Negative channel numbers don't belong to any particular plane.
  // In some parts of the code, we identify North as -1 and South as -2; these should be replaced by use of the
  // correct enum, rather than identified as valid channel numbers (nothing technically wrong, just a matter of style).
  assert(fChannel >= 0);

  return EXOMiscUtil::GetTPCSide(fChannel);
}
//__________________________________________________________________________________
EXOMiscUtil::EChannelType EXOWaveform::GetChannelType() const
{
  // Based on the channel number, return the half of the TPC on which the channel lies.
  // In case it is not a correct channel number (ie. -1 or too large), return kNoPlane.
  return EXOMiscUtil::TypeOfChannel(fChannel);
}
//______________________________________________________________________________
bool EXOWaveform::operator==(const EXOWaveform& comp) const
{
  // Compare two EXOIntWaveforms.  The channel, compression status (including
  // compression data), and included data must all be the same.
  return ( fChannel == comp.fChannel and
           IsCompressed() == comp.IsCompressed() and 
           EXOIntWaveform::operator==(comp) and
           GetCompressedLength() == comp.GetCompressedLength() and
           std::equal( fCompressedData.begin(), fCompressedData.end(), comp.fCompressedData.begin() ) ); 
                 
}
//______________________________________________________________________________
int data_to_qdata( int *data, int nsample, unsigned short *qdata, int maxqdata )
{

  // Don't compress the summed APD signals
  const int offset = 2;
  int len3 = 0;
  int len5 = 0;

  // Try compressing with 5 bits

  len5 = EXOMiscUtil::delta_compression( data, nsample, 
      		      qdata+offset, maxqdata - offset, 5 );
  if ( len5 < 0 ) return -1;

  // Now try 3 bits

  len3 = EXOMiscUtil::delta_compression( data, nsample, 
      		      qdata+offset, maxqdata - offset, 3 );
  if ( len3 < 0 ) return -1;

  // Pick the better one

  if ( len5 < len3 ) {

    // In this case re-do five bit compression

    len5 = EXOMiscUtil::delta_compression( data, nsample, 
                      qdata+offset, maxqdata - offset, 5 );
    if ( offset + len5 >= maxqdata ) return -1;
    
    qdata[0] = 0; 
    qdata[1] = len5;

  }

  else {

    // In this case keep the 3 bit compression

    if ( offset + len3 >= maxqdata ) return -1;
    
    qdata[0] = 0x8000; // A one in the 8th bit signifies 3-bit compression
    qdata[1] = len3; 

  }

  return qdata[1]+offset;

}

/*********************************/
/* qdata_to_data returns nsample */
/*********************************/

//______________________________________________________________________________
int qdata_to_data( unsigned short *qdata, int nqele, int *data, int maxdata  )
{

  const int offset = 2;
  int len;
  int nsample;
  int nbit;

  nsample = 0;

  len = qdata[1];

  // A one in the 15th bit means 3-bit compression.

  nbit = ( (0x8000 & qdata[0]) == 0x8000 ) ? 3 : 5;

  nsample = EXOMiscUtil::delta_uncompression( qdata+offset, len, data, maxdata, nbit );

  return nsample;

}
