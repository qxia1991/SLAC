#ifndef EXOATeamFilterModule_hh
#define EXOATeamFilterModule_hh

#include "EXOAnalysisModule.hh"

class EXOATeamFilterModule : public EXOAnalysisModule 
{

public :

  EXOATeamFilterModule();
  ~EXOATeamFilterModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  DEFINE_EXO_ANALYSIS_MODULE( EXOATeamFilterModule )

};
#endif

  


