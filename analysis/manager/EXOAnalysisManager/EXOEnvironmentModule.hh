#ifndef EXOEnvironmentModule_hh
#define EXOEnvironmentModule_hh

#include "EXOAnalysisModule.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOEnvironmentModule : public EXOAnalysisModule 
{

private :

  // flavor is a characteristic of calibration readers, not handlers
  std::string          m_flavor;

public :

  EXOEnvironmentModule();
  ~EXOEnvironmentModule() {};

  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);
  void SetFlavor(std::string aval) { m_flavor = aval; }
  
  DEFINE_EXO_ANALYSIS_MODULE( EXOEnvironmentModule )

};
#endif
