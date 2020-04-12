#ifndef EXOUWireMixedSignalModule_hh
#define EXOUWireMixedSignalModule_hh

#include "EXOAnalysisModule.hh"
#include "TF1.h"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOUWireMixedSignalModule : public EXOAnalysisModule 
{

private :

  // These are example parameters for this module 

  double        doubleparam;
  int           intparam;
  bool          boolparam;
  std::string   stringparam;
  TF1* func_coll;
  TF1* func_ind;
  TF1* function_combined;
  std::string                fElectronicsDatabaseFlavor;
  std::string                fUWireDatabaseFlavor;

protected:

public :

  EXOUWireMixedSignalModule();
  ~EXOUWireMixedSignalModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  double CombineFit(double* x, double* p);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetDoubleParam(double aval) { doubleparam = aval; }
  void SetIntParam(int aval) { intparam = aval; }
  void SetBoolParam(bool aval) { boolparam = aval; }
  void SetStringParam(std::string aval) { stringparam = aval; }
  void CallBack();

  void SetElectronicDatabaseFlavor(std::string aval) { fElectronicsDatabaseFlavor = aval; }
  void SetUWireDatabaseFlavor(std::string aval)      {fUWireDatabaseFlavor = aval;}

  DEFINE_EXO_ANALYSIS_MODULE( EXOUWireMixedSignalModule )

};
#endif

  


