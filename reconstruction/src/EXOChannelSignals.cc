//______________________________________________________________________________
// EXOChannelSignals
//
//  Collection class containing all the signals found on a particular channel
//  or waveform.  This class saves the channel number/tag as well as a
//  pointer to the waveform of the channel.  It is also possible to store
//  generic cache information in this class using SetCacheInformationFor.
//  This can be later retrieved by using GetCacheInformationFor, or
//  GetAllCacheInformation.
//
//______________________________________________________________________________
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <cmath>
#include <cassert>

ClassImp( EXOChannelSignals )

//______________________________________________________________________________
EXOChannelSignals::EXOChannelSignals()
: fChannel(0),
  fBehaviorType(EXOReconUtil::kUndefined),
  fSignals(),
  fIter(fSignals.begin()),
  fRefWf(0)
{

}

//______________________________________________________________________________
void EXOChannelSignals::Clear(Option_t *)
{
  // Clears all signals, does not clear the waveform or channel/tag information
  fSignals.clear();
  ResetIterator();
}

//______________________________________________________________________________
void EXOChannelSignals::AddSignal(const EXOSignal &sig)
{
  // Adds an EXOSignal.  Also resets the iterator. 
  fSignals.insert(sig);
  ResetIterator();
}

//______________________________________________________________________________
size_t EXOChannelSignals::GetNumSignals() const
{
  return fSignals.size();
}

//______________________________________________________________________________
void EXOChannelSignals::SetWaveform(const EXOWaveform * wf)
{
  // Set associated waveform.
  fRefWf = wf;
}

//______________________________________________________________________________
const EXOWaveform* EXOChannelSignals::GetWaveform() const
{
  // Get associated waveform, can be NULL!
  return fRefWf;
}

//______________________________________________________________________________
void EXOChannelSignals::RemoveSignal(const EXOSignal& sig)
{
  // Removes signal from the collection, this also reset the iterator using
  // ResetIterator().
  fSignals.erase(sig);
  ResetIterator();
}

//______________________________________________________________________________
void EXOChannelSignals::ResetIterator() const
{
  // Reset the iterator to the beginning of the collection.
  fIter = fSignals.begin();
}

//______________________________________________________________________________
const EXOSignal* EXOChannelSignals::Next() const
{
  // Get the next signal in time
  if (fIter == fSignals.end()) return NULL;
  const EXOSignal* sig = &(*fIter);
  fIter++;
  return sig;
}
//______________________________________________________________________________
void EXOChannelSignals::AddAndSaveChannelData(const EXOChannelSignals& other) 
{
  // Add another set of channel signals to this one.  Save the channel and
  // waveform data as well.  The cache information of the other
  // signal is added to this one.  If there are duplicates, the cache is
  // overwritten by the new data.  
  MakeSimilarTo(other);
  const InformationCache& cache = other.GetAllCacheInformation();
  InformationCache::const_iterator iter = cache.begin();
  for (;iter != cache.end();iter++) fCacheInformation[iter->first] = iter->second;
  Add(other);
}
//______________________________________________________________________________
void EXOChannelSignals::Add(const EXOChannelSignals& other) 
{
  // Add another set of channel signals to this one.  No checking is done if
  // the channesl or waveforms match so that one can add signals from other,
  // arbitrary channels. 
  const EXOSignal* sig;
  other.ResetIterator();
  while ((sig = other.Next()) != NULL) AddSignal(*sig);
}
//______________________________________________________________________________
void EXOChannelSignals::SetCacheInformationFor(std::string infoName, double info)
{
  // Set a cached, named value.  This value can be used by other objects and
  // can be queried using HasCacheInformationFor().  
  fCacheInformation[infoName] = info;
}

//______________________________________________________________________________
bool EXOChannelSignals::HasCacheInformationFor(std::string infoName) const
{
  // Checks to see if the cached information exists for a given infoName.
  return fCacheInformation.find(infoName) != fCacheInformation.end();
}

//______________________________________________________________________________
double EXOChannelSignals::GetCacheInformationFor(std::string infoName) const
{
  // Gets cache information for a given infoName.  Returns 0 when the cache
  // info doesn't exist. 
  if (not HasCacheInformationFor(infoName)) return 0.0;
  return fCacheInformation.find(infoName)->second;
}

//______________________________________________________________________________
void EXOChannelSignals::Print(Option_t* opt) const
{
  // Print out the information for this signal 
  TObject::Print(opt);
  std::cout << "  Channel/Tag number: " << GetChannel() << std::endl; 
  std::cout << "  Found signals: " << GetNumSignals() << std::endl; 
  const EXOSignal* sig;
  ResetIterator();
  while ((sig = Next()) != NULL) sig->Print(opt);
}

//______________________________________________________________________________
double EXOChannelSignals::GetTimeToSignal(double Time) const
{
  // Locate the signal closest in time (forward or backward) to Time, and return the absolute time difference.
  // If EXOChannelSignals is empty, return a negative number.

  // Implementation note:  remember that std::set is ordered by our comparison operator, which for us is the time of the signal.
  EXOSignal signal;
  signal.fTime = Time;
  std::set<EXOSignal>::const_iterator iter = fSignals.lower_bound(signal);
  double LeastDiff = -1.0;
  if(iter != fSignals.end()) LeastDiff = std::fabs(iter->fTime - Time);
  if(iter != fSignals.begin()) {
    iter--;
    if(LeastDiff >= 0.0) LeastDiff = std::min(LeastDiff, std::fabs(iter->fTime - Time));
    else LeastDiff = std::fabs(iter->fTime - Time);
  }
  assert((GetNumSignals() == 0) xor (LeastDiff >= 0));
  return LeastDiff;
}

//_____________________________________________________________________________
void EXOChannelSignals::MakeSimilarTo(const EXOChannelSignals& other)
{
  // Set the channel, behavior type, and waveform pointer to those of other.
  // Do not copy the signals, information cache, or (of course) iterator.
  SetChannel(other.GetChannel());
  SetBehaviorType(other.GetBehaviorType());
  SetWaveform(other.GetWaveform());
}

//_____________________________________________________________________________
bool EXOChannelSignals::IsSimilarTo(const EXOChannelSignals& other) const
{
  // Check that channel, behavior type, and waveform pointer are equal.
  return GetChannel() == other.GetChannel() and 
         GetBehaviorType() == other.GetBehaviorType() and
         GetWaveform() == other.GetWaveform();
}
