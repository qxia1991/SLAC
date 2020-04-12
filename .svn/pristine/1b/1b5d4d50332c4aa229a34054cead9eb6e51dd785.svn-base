//______________________________________________________________________________
//                                                                        
// EXOWaveformData
//
// Class encapsulating all the waveforms in a data set See EXOWaveform for a
// description of the basic class encapsulating a single waveform. 
//
// WARNING:  The pointer fWaveforms nominally points to an TClonesArray;
// in fact, it is assumed (eg. by the GetNum functions) that it points
// to an EXOTClonesArray, which is guaranteed to always be compressed.
// See the class description for EXOTClonesArray, which explains caveats.

#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

ClassImp( EXOWaveformData )
//______________________________________________________________________________
EXOWaveformData::EXOWaveformData() : 
  TObject(),
  fWaveforms(0) 
{
  EXOWaveformData::Clear();
  InitializeArrays();
}

//______________________________________________________________________________
void EXOWaveformData::InitializeArrays()
{
  InitializeWaveforms();
}

//______________________________________________________________________________
void EXOWaveformData::InitializeWaveforms()
{
  fWaveforms = new EXOTClonesArray( EXOWaveform::Class(), MAXSIG ); 
}

//______________________________________________________________________________
EXOWaveformData::~EXOWaveformData()
{
  delete fWaveforms;
}

//______________________________________________________________________________
void EXOWaveformData::Clear( Option_t* /*option*/ )
{
  // Clear data, call Clear on associated waveforms
  fNumSamples = 0;
  fWFsHaveBeenEdited = false;
  if ( fWaveforms ) fWaveforms->Clear( "C" );
  fChanMap.clear();
}
//______________________________________________________________________________
size_t EXOWaveformData::FindWaveformIndexWithChannel( Int_t channel ) const
{
  // Internal function that returns the index of a waveform with a particular
  // channel.  If not found, returns std::string::npos.  

  if (fChanMap.size() != GetNumWaveforms()) {
    fChanMap.clear();
    for (size_t i=0;i<GetNumWaveforms();i++) fChanMap[GetWaveform(i)->fChannel] = i;
  }
  WFMap::iterator iter = fChanMap.find(channel);
  if (iter!=fChanMap.end()) return iter->second;
  return std::string::npos;
}
//______________________________________________________________________________
EXOWaveform* EXOWaveformData::GetWaveformWithChannelToEdit( Int_t channel )
{
  // Returns a waveform for a particular channel to edit.  If the channel cannot be
  // found, returns NULL;

  size_t index = FindWaveformIndexWithChannel(channel);
  if (index != std::string::npos) return GetWaveformToEdit(index);
  return NULL;
}
//______________________________________________________________________________
const EXOWaveform* EXOWaveformData::GetWaveformWithChannel( Int_t channel ) const 
{
  // Returns a waveform for a particular channel.  If the channel cannot be
  // found, returns NULL;

  size_t index = FindWaveformIndexWithChannel(channel);
  if (index != std::string::npos) return GetWaveform(index);
  return NULL;
}
//______________________________________________________________________________
void EXOWaveformData::SortByChannel() 
{ 
  // Sort waveforms by channel
  if (fWaveforms) {
    fWaveforms->Sort(); 
    fChanMap.clear();
    for (size_t i=0;i<GetNumWaveforms();i++) fChanMap[GetWaveform(i)->fChannel] = i;
  }
}
//______________________________________________________________________________
EXOWaveformData::EXOWaveformData(const EXOWaveformData& other) : TObject(other)
{
  fNumSamples = other.fNumSamples;
  fWaveforms = new EXOTClonesArray(*other.fWaveforms);
  fChanMap = other.fChanMap;
  fWFsHaveBeenEdited = other.fWFsHaveBeenEdited;
}
//______________________________________________________________________________
EXOWaveformData& EXOWaveformData::operator=(const EXOWaveformData& other) 
{
  if (this == &other) return *this;
  fNumSamples = other.fNumSamples;
  *static_cast<EXOTClonesArray*>(fWaveforms) = *other.fWaveforms;
  fChanMap = other.fChanMap;
  fWFsHaveBeenEdited = other.fWFsHaveBeenEdited;
  return *this;
}

//______________________________________________________________________________
void EXOWaveformData::Compress(bool safe) 
{ 
  // Utility functions to (de)compress all of the waveforms at once.  It is
  // safe to call this at any time as the waveform will not compress or
  // decompress if it's not in the appopriate state.  That is an
  // already-compressed waveform will not try to compress again.  The safe flag
  // checks the waveform before compression to ensure that it may be
  // compressed.  That is, it checks to see the waveform is below 12-bits
  // internal width per point.  Default to check is false.

  for(size_t i=0;i<GetNumWaveforms();i++) {
    static_cast<EXOWaveform*>(GetWaveformArray()->At(i))->Compress(safe); 
  }
}

//______________________________________________________________________________
void EXOWaveformData::Decompress() 
{ 
  // Utility functions to (de)compress all of the waveforms at once.  It is
  // safe to call this at any time as the waveform will not compress or
  // decompress if it's not in the appopriate state.  That is an
  // already-compressed waveform will not try to 
  for(size_t i=0;i<GetNumWaveforms();i++) {
    static_cast<EXOWaveform*>(GetWaveformArray()->At(i))->Decompress();
  }
}

//______________________________________________________________________________
const EXOWaveform* EXOWaveformData::GetWaveform( size_t i ) const 
{ 
  // Get Waveform at index i
  return static_cast<const EXOWaveform*>(GetWaveformArray()->At(i)); 
}

//______________________________________________________________________________
EXOWaveform* EXOWaveformData::GetWaveformToEdit( size_t i )
{ 
  // Get an editable waveform at index i.  This must be used when the waveform
  // you wish to get is already in the data set, but it needs to be edited or
  // changed in some way.  This sets a flag that ensures that other users
  // *know* the waveform data has been edited from its initial state. 
  fWFsHaveBeenEdited = true;
  return static_cast<EXOWaveform*>(GetWaveformArray()->At(i)); 
}

//______________________________________________________________________________
EXOWaveform* EXOWaveformData::GetNewWaveform()
{ 
  // Get a new waveform, this is to be used to load the EXOWaveformData object
  // with new waveforms.   
  return static_cast<EXOWaveform*>(
    GetWaveformArray()->GetNewOrCleanedObject( GetNumWaveforms() ) );
}

//______________________________________________________________________________
void EXOWaveformData::Remove(EXOWaveform* wf) 
{ 
  // Remove waveform from the data set 
  GetWaveformArray()->RemoveAndCompress(wf);
}

//______________________________________________________________________________
bool EXOWaveformData::ContainsAllWaveformsIn(const EXOWaveformData& comp) const
{
  // Returns true if this object contains all of the waveforms in comp.  This
  // object can have however more waveforms.
  size_t numWFs = comp.GetNumWaveforms();
  if ( GetNumWaveforms() < numWFs ) return false;
  if ( fNumSamples != comp.fNumSamples ) return false;
  for (size_t i=0;i<numWFs;i++) {
    const EXOWaveform* compwf = comp.GetWaveform(i);
    const EXOWaveform* thiswf = GetWaveformWithChannel(compwf->fChannel); 
    if ( not thiswf ) {
      LogEXOMsg(Form("Waveform at channel %i does not exist", compwf->fChannel), EEDebug);
      return false;
    }
    if ( not (*compwf == *thiswf) ) {
      LogEXOMsg(Form("Waveform at channel %i is not equivalent", compwf->fChannel), EEDebug);
      return false;
    }
  }
  return true;
}

//______________________________________________________________________________
bool EXOWaveformData::operator==(const EXOWaveformData& comp) const
{
  // This operator checks to see that all the waveforms in comp and this
  // waveform data are exactly the same.
  if ( GetNumWaveforms() != comp.GetNumWaveforms() ) return false;
  return ContainsAllWaveformsIn(comp);
}


