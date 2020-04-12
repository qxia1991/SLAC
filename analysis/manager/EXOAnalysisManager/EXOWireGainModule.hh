#ifndef EXOWireGainModule_hh
#define EXOWireGainModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
class EXOEventData;

class EXOWireGainModule : public EXOAnalysisModule
{
public:
  void SetUWireDatabaseFlavor(std::string flavor);

  EXOWireGainModule();
  int TalkTo(EXOTalkToManager *tm);
  EventStatus ProcessEvent(EXOEventData* ED);
  DEFINE_EXO_ANALYSIS_MODULE(EXOWireGainModule)
protected:
  std::string fUWireDatabaseFlavor;
};
#endif
