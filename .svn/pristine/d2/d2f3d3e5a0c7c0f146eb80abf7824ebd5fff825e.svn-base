#ifndef EXOAlphaNoiseTagger_hh
#define EXOAlphaNoiseTagger_hh

#include <iostream>
#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <cstddef> //for size_t

class EXOAlphaNoiseTagger : public EXOAnalysisModule 
{

private :

  EXOWaveform* GetAPDSumWaveform(EXOEventData *eventdata);
  EXOWaveform* GetWireSumWaveform(EXOEventData *eventdata);

  size_t FindTimeWithIndivWireAboveThresh(EXOEventData *eventdata, double threshold);
  bool ChannelMaxesOut(EXOEventData *eventdata, int num);

  bool isRingingNoise(EXOEventData *eventdata);
  bool isSumNegBeforePosNoise(EXOEventData *eventdata);
  int NumMaxoutChannelGlitch(EXOEventData *eventdata);
  bool isAPDBounceNoise(EXOEventData *eventdata);
  bool isRealUSig(EXOEventData *eventdata);

  unsigned int num_noise_events;
  unsigned int num_noise_overlap;
  int run_number;
  unsigned int run_start_time;
  unsigned int run_end_time;

  bool verbose;
  bool check_apd_ringing;
  bool check_sumwire;
  bool check_glitch;
  bool check_apd_bounce;

protected:

  void SetVerbose(bool v) {verbose = v;};
  void SetCheckAPDRinging(bool c) {check_apd_ringing = c;};
  void SetCheckSumWire(bool c) {check_sumwire = c;}
  void SetCheckGlitch(bool c) {check_glitch = c;}
  void SetCheckAPDBounce(bool c) {check_apd_bounce = c;}

public :

  EXOAlphaNoiseTagger() {}
  ~EXOAlphaNoiseTagger() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *eventdata);
  EventStatus ProcessEvent(EXOEventData *eventdata);
  EventStatus EndOfRun(EXOEventData *eventdata);

  int TalkTo(EXOTalkToManager *tm);

  //int ShutDown();

  DEFINE_EXO_ANALYSIS_MODULE( EXOAlphaNoiseTagger )

};
#endif
