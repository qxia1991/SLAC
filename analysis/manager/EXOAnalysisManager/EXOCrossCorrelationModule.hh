#ifndef EXOCrossCorrelationModule_hh
#define EXOCrossCorrelationModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOCorrelationCollection.hh"
#include "EXOUtilities/EXOWFCrossProduct.hh"
#include <string>
#include <cstddef>

class TFile;

class EXOCrossCorrelationModule : public EXOAnalysisModule 
{
public :

  EXOCrossCorrelationModule();
  ~EXOCrossCorrelationModule();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetMaxEventNumber(int val){fMaxEventNumber = val;}
  void SetMinEventNumber(int val){fMinEventNumber = val;}
  void SetNumSamples(size_t val);
  void SetFilename(std::string val){fFilename = val;}

protected :
  int fMaxEventNumber; //The maximum event number that is considered
  int fMinEventNumber; //The minimum event number that is considered
  int fMinchannel;
  int fMaxchannel;
  size_t fNSamples; //The number of samples to consider 
  int fEventCounter;
  EXOCorrelationCollection fCollection;
  EXOWFCrossProduct fCrossProduct;
  std::string fFilename;
  TFile* fFile;

  DEFINE_EXO_ANALYSIS_MODULE( EXOCrossCorrelationModule )
};

inline void EXOCrossCorrelationModule::SetNumSamples(size_t val)
{
  //Set the number of samples to be used (starting at the beginning of the waveform)
  fNSamples = val;
}

#endif

  


