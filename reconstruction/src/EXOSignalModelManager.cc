//______________________________________________________________________________
// Signal Model manager, manages a set of EXOSignalModel objects.
// EXOSignalModelRegistrants can be registered with this class to be notified
// for example when an EXOSignalModel changes.  This is useful because it allows
// classes which must update cached values dependent on the signal model to do
// so.  For this notification functionality, see
// BuildSignalModelForChannelOrTag and NotifySignalModelHasChanged.
//______________________________________________________________________________
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOSignalModelRegistrant.hh"
#include "EXOReconstruction/EXOVSignalModelBuilder.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TString.h"

//______________________________________________________________________________
void EXOSignalModelManager::BuildSignalModelForChannelOrTag(int channel, 
  const EXOVSignalModelBuilder& builder)
{
  // Set the signal model for a channel.  EXOVSignalModelBuilder is used to
  // build the signal model, see it's documentation for more details.
  // Initialization is only performed if necessary, this behavior is defined by
  // the EXOVSignalModelBuilder class.

  EXOSignalModel& mod = fSignalModelMap[channel];
  if (builder.InitializeSignalModelIfNeeded(mod, channel)) {
    // This means that the initialization has been run, so we must notify our
    // registrants that this signal model has changed.
    NotifyRegistrantsOfChange(channel, mod);
  } 
}
    
//______________________________________________________________________________
const EXOSignalModel* EXOSignalModelManager::GetSignalModelForChannelOrTag(
  int chanOrTag) const
{
  SignalModelMap::const_iterator iter = fSignalModelMap.find(chanOrTag);
  if (iter == fSignalModelMap.end()) return NULL;
  return &iter->second;
}

//______________________________________________________________________________
void EXOSignalModelManager::AddDerivedSignalModelForChannelOrTag(int chanOrTag, 
  const EXOVSignalModelBuilder& builder) const
{
  // Add a derived signal model for a channel/tag.  This is a mechanism for
  // EXOVDefineReconProcessList types to add signal models for derived
  // waveforms (APD sums, etc.). These channels/tags are handled specially
  // since already in place channels are not allowed to be overwritten but
  // other channels/tag types which have been set with this function are.    
  // EXOVSignalModelBuilder is used to build the signal model, see it's
  // documentation for more details.

  if (fSignalModelMap.find(chanOrTag) != fSignalModelMap.end() && 
      fDerivedChannelNumbers.find(chanOrTag) == fDerivedChannelNumbers.end()) {
    // This means a user tried to add a channel/tag that already exists and was
    // not previously added with this function.
    LogEXOMsg(TString::Format("Cannot add a channel/tag that already exists (%d)", 
      chanOrTag).Data(), EEError);
    return;
  } 
  fDerivedChannelNumbers.insert(chanOrTag);
  const_cast<EXOSignalModelManager*>(this)->
    BuildSignalModelForChannelOrTag(chanOrTag, builder);
}

//______________________________________________________________________________
void EXOSignalModelManager::NotifyRegistrantsOfChange(int chanOrTag, 
  const EXOSignalModel& mod)
{
  // Notify the registered objects of a change in a signal model for a
  // particular channel or tag.
  RegObjList::iterator iter = fRegisteredObjs.begin();
  for (;iter!=fRegisteredObjs.end();iter++) { 
    (*iter)->NotifySignalModelHasChanged(chanOrTag, mod);
  }
}

//______________________________________________________________________________
void EXOSignalModelManager::AddRegisteredObject(EXOSignalModelRegistrant* reg)
{ 
  // Add a registered object.  These objects are notified whenever a signal
  // model is changed so that they can update their caches.
  fRegisteredObjs.insert(reg); 
  reg->RegisterSignalModels(this);
}

//______________________________________________________________________________
void EXOSignalModelManager::RemoveRegisteredObject(EXOSignalModelRegistrant* reg)
{
  // Remove any previously registered object.  This is called in the destructor
  // of EXOSignalModelRegistrants.
  fRegisteredObjs.erase(reg);
}

//______________________________________________________________________________
EXOSignalModelManager::~EXOSignalModelManager()
{
  // When the signal manager destructs, remove all registered objects 
  RegObjList::iterator iter = fRegisteredObjs.begin();
  for (;iter!=fRegisteredObjs.end();iter++) { 
    (*iter)->RegisterSignalModels(NULL);
    RemoveRegisteredObject(*iter);
  }

}
