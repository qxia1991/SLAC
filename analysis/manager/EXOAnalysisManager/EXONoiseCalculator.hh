#ifndef EXONoiseCalculator_hh
#define EXONoiseCalculator_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOEventInfo.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include "TObject.h"
#include <string>
#include <set>

class EXOEventData;
class EXOTalkToManager;
class EXOEventInfo;
class TFile;
class TTree;

class EXONoiseCalculator : public EXOAnalysisModule 
{

private :
  EXOEventInfo* fEventInfo;

  std::string fFilenameBase;
  TFile *fFile;
  TTree *fTree;
  EXOBaselineAndNoiseCalculator fBaselineCalculator;
  int fNevents;
  std::set<int> fChannels;

public :

  EXONoiseCalculator();
  ~EXONoiseCalculator();

  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *talktoManager);

  void SetFilenameBase(std::string fn){fFilenameBase = fn;}

  DEFINE_EXO_ANALYSIS_MODULE( EXONoiseCalculator )
};
#endif
