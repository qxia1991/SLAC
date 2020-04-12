#ifndef EXOExampleModule_hh
#define EXOExampleModule_hh

#include "EXOAnalysisModule.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOExampleModule : public EXOAnalysisModule 
{

private :

  // These are example parameters for this module 

  double        doubleparam;
  int           intparam;
  bool          boolparam;
  std::string   stringparam;

protected:

public :

  EXOExampleModule();
  ~EXOExampleModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetDoubleParam(double aval) { doubleparam = aval; }
  void SetIntParam(int aval) { intparam = aval; }
  void SetBoolParam(bool aval) { boolparam = aval; }
  void SetStringParam(std::string aval) { stringparam = aval; }
  void CallBack();


  DEFINE_EXO_ANALYSIS_MODULE( EXOExampleModule )

};
#endif

  


