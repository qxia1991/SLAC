#ifndef EXOUWireMixSigAddModule_hh
#define EXOUWireMixSigAddModule_hh

#include "EXOAnalysisModule.hh"
#include "TF1.h"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOUWireMixSigAddModule : public EXOAnalysisModule 
{

private :

  // These are example parameters for this module 

  double        fSignalTimeDiffLimit;
  struct infotup{int count; int channel; double MixColPar; double MixColParEr; double MixColTime; double MixColTimeEr; int ClusterBelong; double DiffTimeCluster; bool IsDuplicate; int UInCclose;} ;
  double           fTagEnergyCut;
  bool          boolparam;
  std::string   stringparam;
  TF1* func_coll;
  TF1* func_ind;
  TF1* function_combined;

protected:

public :

  EXOUWireMixSigAddModule();
  ~EXOUWireMixSigAddModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  void FindCluster(EXOEventData* ED, infotup *t);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetSignalTimeDiffLimit(double aval) { fSignalTimeDiffLimit = aval; }
  void SetTagEnergyCut(double aval) { fTagEnergyCut = aval; }
  void SetBoolParam(bool aval) { boolparam = aval; }
  void SetStringParam(std::string aval) { stringparam = aval; }
  void CallBack();


  DEFINE_EXO_ANALYSIS_MODULE( EXOUWireMixSigAddModule )

};
#endif

  


