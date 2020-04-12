#ifndef EXOWireGainDummyModule_hh
#define EXOWireGainDummyModule_hh

#include "EXOAnalysisModule.hh"
class EXOEventData;

class EXOWireGainDummyModule : public EXOAnalysisModule
{
public:
  EXOWireGainDummyModule();
  EventStatus ProcessEvent(EXOEventData* ED);
  DEFINE_EXO_ANALYSIS_MODULE(EXOWireGainDummyModule)
};
#endif
