//______________________________________________________________________________
//
// EXOSignalCollection is a collection of (signals) EXOChannelSignals that have
// been found for an event.
//
//______________________________________________________________________________
#include "EXOReconstruction/EXOSignalCollection.hh"

using EXOMiscUtil::TypeOfChannel;

//______________________________________________________________________________
void EXOSignalCollection::Clear(Option_t *)
{
  // Clears the contents of the collection.
  fChannelSignals.clear();
}

//______________________________________________________________________________
void EXOSignalCollection::AddChannelSignal(const EXOChannelSignals &cs)
{
  // Adds a EXOChannelSignal to the collection.
  fChannelSignals[cs.GetChannel()].AddAndSaveChannelData(cs);
}

//______________________________________________________________________________
void EXOSignalCollection::AddSignalOnWaveform(const EXOSignal& sig, const EXOReconProcessList::WaveformWithType* wfWithType)
{
  // Adds a EXOSignal to the collection
  int channel = wfWithType->fWf->fChannel;
  fChannelSignals[channel].SetChannel(channel);
  fChannelSignals[channel].SetBehaviorType(wfWithType->fType);
  fChannelSignals[channel].SetWaveform(wfWithType->fWf);
  fChannelSignals[channel].AddSignal(sig);
}

//______________________________________________________________________________
void EXOSignalCollection::SetCacheInformationForChannel(std::string infoName, double info, int channel)
{
  // Sets cache information on the specified channel
  fChannelSignals[channel].SetCacheInformationFor(infoName,info);
}

//______________________________________________________________________________
size_t EXOSignalCollection::GetNumChannelSignals() const
{
  return fChannelSignals.size();
}

//______________________________________________________________________________
const EXOChannelSignals* EXOSignalCollection::GetSignalsForChannel(int channel) const
{
  // Returns ChannelSignals of a specific channel. If no Signals are found on
  // this channel, NULL is returned.

  ChannelSignalsMap::const_iterator iter = fChannelSignals.find(channel);
  if(iter == fChannelSignals.end()) return NULL;
  return &(iter->second);
}

//______________________________________________________________________________
EXOSignalCollection EXOSignalCollection::GetCollectionForType(
  EXOMiscUtil::EChannelType type) const
{
  // Get a collection that only has a particular channel type in it.
  EXOSignalCollection returnCollection;
  for(ChannelSignalsMap::const_iterator iter = fChannelSignals.begin(); 
      iter!=fChannelSignals.end(); iter++) {

    if(not (TypeOfChannel(iter->first) == type)) continue;
    returnCollection.AddChannelSignal(iter->second);
  }
  return returnCollection;
}

//______________________________________________________________________________
void EXOSignalCollection::ResetIterator() const
{
  // Reset iterator to beginning to iterate using Next().
  fIter = fChannelSignals.begin();
}
//______________________________________________________________________________
const EXOChannelSignals* EXOSignalCollection::Next() const
{
  // Get the next available channel signals in the iteration.
  // EXOChannelSignals are ordered by channel/tag number, lower number come
  // earlier.
  if (fIter == fChannelSignals.end()) return NULL;
  const EXOChannelSignals* retVal = &fIter->second; 
  fIter++;
  return retVal;
}

//______________________________________________________________________________
const EXOChannelSignals* EXOSignalCollection::NextOfType(
  EXOReconUtil::ESignalBehaviorType type) const
{
  // Get the next available channel signals in the iteration that is of the
  // specified type.
  if (fIter == fChannelSignals.end()) return NULL;
  while(fIter != fChannelSignals.end() and fIter->second.GetBehaviorType() != type){
    fIter++;
  }
  if (fIter == fChannelSignals.end()) return NULL;
  const EXOChannelSignals* retVal = &fIter->second; 
  fIter++;
  return retVal;
}

//______________________________________________________________________________
void EXOSignalCollection::Add(const EXOSignalCollection& other)
{
  // Append another signal collection to this one.
  const EXOChannelSignals* sigs;
  other.ResetIterator();
  while ((sigs = other.Next()) != NULL) AddChannelSignal(*sigs);
}
