#ifndef EXOVWireGainModule_hh
#define EXOVWireGainModule_hh

#include "EXOAnalysisModule.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOVWireGainModule : public EXOAnalysisModule
{
  public:
    EXOVWireGainModule();
    EventStatus ProcessEvent(EXOEventData* ED);
    void SetVWireDatabaseFlavor(std::string flavor);
    void SetUserGainCorrection(double val);
    int TalkTo(EXOTalkToManager* talktoManager);
    DEFINE_EXO_ANALYSIS_MODULE(EXOVWireGainModule)

  private:
    std::string fVWireDatabaseFlavor;
    double fUserGainCorrection;
};
#endif
