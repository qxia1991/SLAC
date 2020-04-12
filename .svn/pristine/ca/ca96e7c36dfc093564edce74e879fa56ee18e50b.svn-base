#ifndef EXOATeamSoftwareTrigModule_hh
#define EXOATeamSoftwareTrigModule_hh

#include "EXOAnalysisModule.hh"

class EXOEventData;
class EXOTalkToManager;

class EXOATeamSoftwareTrigModule : public EXOAnalysisModule 
{

private :

  double threshold;

public :

  EXOATeamSoftwareTrigModule();
  ~EXOATeamSoftwareTrigModule() {};

  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);
  double Baseline(double signal[], int length, double trigger);
  double Max(double signal[], int length);
  double Min(double signal[], int length);
  double Amplitude(double signal[], int length, double trigger);
  void Setthreshold(double aval) { threshold = aval; }

  DEFINE_EXO_ANALYSIS_MODULE( EXOATeamSoftwareTrigModule )

};
#endif
