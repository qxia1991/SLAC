//______________________________________________________________________________
//
// EXOIgnoreChannelsModule
//
// Module will remove ignored channels from any found U-, V-, or APD-signals in
// the event data object.  This is mainly for debugging and checking hardware
// issues.
//
//______________________________________________________________________________

#include "EXOAnalysisManager/EXOIgnoreChannelsModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOIgnoreChannelsModule, "ignore" )
  
//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOIgnoreChannelsModule::ProcessEvent(EXOEventData *ED)
{
  // Process event, removing ignored channels from the data object   

  #define FIND_AND_REMOVE(sig_type)                              \
  std::vector<EXO ## sig_type*> sig_type ## vec;                 \
  for (size_t i=0;i<ED->GetNum ## sig_type ## s();i++) {         \
    EXO ## sig_type *tmp = ED->Get ## sig_type (i);              \
    if (fIgnoredChannels.find(tmp->fChannel) !=                  \
        fIgnoredChannels.end() ) sig_type ## vec.push_back(tmp); \
  }                                                              \
  for (size_t i=0;i<sig_type ## vec.size();i++) ED->Remove(sig_type ## vec[i]); 
  
  FIND_AND_REMOVE(APDSignal)
  FIND_AND_REMOVE(UWireSignal)
  FIND_AND_REMOVE(VWireSignal)
 
  return kOk;
}

//______________________________________________________________________________
int EXOIgnoreChannelsModule::TalkTo(EXOTalkToManager *talktoManager)
{
  //  Add talk to command to add ignored channels 

  talktoManager->CreateCommand("/ignore/addchannels",
                               "add channels to ignore (white-space separated)",
                               this,
                               "",
                               &EXOIgnoreChannelsModule::AddIgnoredChannels);
  
  return 0;
}

//______________________________________________________________________________
void EXOIgnoreChannelsModule::AddIgnoredChannels(std::string ignChannels)
{
  // Add ignored channels
  std::istringstream is(ignChannels);
  Int_t tmp;
  while (is >> tmp) AddIgnoredChannel(tmp);
}
