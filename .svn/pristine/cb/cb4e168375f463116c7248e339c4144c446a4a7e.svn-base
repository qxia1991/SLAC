#ifndef EXOReconNoiseTagger_hh
#define EXOReconNoiseTagger_hh

#include <iostream>
#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include <cstddef> //for size_t
#include <vector>

class EXOReconNoiseTagger : public EXOAnalysisModule 
{

private :

  //unsigned int num_noise_events;
  int run_number;
  unsigned int num_removed_events;
  unsigned int num_adjust_events;
  unsigned int run_start_time;
  unsigned int run_end_time;
  unsigned int num_bad_chi2;
  unsigned int num_ignored;
  unsigned int num_good_events;

  std::vector<EXOUWireSignal*> GetListOfSuspicous(EXOEventData *ED);
  std::vector<EXOUWireSignal*> FindSigsToRemove(EXOEventData *ED, EXOUWireSignal* badSignal);

//protected:


public :

  EXOReconNoiseTagger() ;
  ~EXOReconNoiseTagger() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *eventdata);
  EventStatus ProcessEvent(EXOEventData *eventdata);
  EventStatus EndOfRun(EXOEventData *eventdata);

  int TalkTo(EXOTalkToManager *tm);

  //int ShutDown();

  void SetChiSquareCut(double val)  {fChiSquareCut=val;} 
  void SetEnergyCut(double val)     {fEnergyCut=val;}
  void SetMultCut(double val)       {fMultCut=val;}
  void SetTimeCut(double val)       {fTimeCut=val;}

  DEFINE_EXO_ANALYSIS_MODULE( EXOReconNoiseTagger )

protected:
 
 double fChiSquareCut;
 double fEnergyCut;
 double fMultCut;
 double fTimeCut; 

};
#endif
