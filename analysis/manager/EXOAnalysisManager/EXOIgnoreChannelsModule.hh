#ifndef EXOIgnoreChannelsModule_hh
#define EXOIgnoreChannelsModule_hh

#include "EXOAnalysisModule.hh"
#include <set>

class EXOEventData;
class EXOTalkToManager;

class EXOIgnoreChannelsModule : public EXOAnalysisModule 
{

  protected:

    typedef std::set<int>  ChanSet;
    ChanSet          fIgnoredChannels;

  public:

    void AddIgnoredChannel(int chan) { fIgnoredChannels.insert(chan); }
    void AddIgnoredChannels(std::string ignChannels);

    EventStatus ProcessEvent(EXOEventData *ED);
    int TalkTo(EXOTalkToManager *tm);
    
    DEFINE_EXO_ANALYSIS_MODULE( EXOIgnoreChannelsModule )

};
#endif
