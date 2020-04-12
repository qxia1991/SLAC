#ifndef EXOUneighbors_hh
#define EXOUneighbors_hh

#include "EXOAnalysisModule.hh"


class EXOUWireSignal;
class EXOVWireSignal;
class EXOEventData;

class EXOUneighbors : public EXOAnalysisModule 
{

protected:
  void ApplyUneighborCorrection(EXOEventData* ED);
  void FindPulseParams(EXOEventData* ED, double t0, int channel, double& P);
  double GetUneighborsCorrection(double fP1, double fP2);
  void InitVars(); 

public :

  EXOUneighbors();
  ~EXOUneighbors();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);
  void SetNsigma(int val) {nsig = val;};
  int GetNsigma()	  {return nsig;};

protected:

  int nsig; //if Pp/m is smaller than nsig RMS above baseline, assume noise fluctuation


  DEFINE_EXO_ANALYSIS_MODULE( EXOUneighbors )
};
#endif
