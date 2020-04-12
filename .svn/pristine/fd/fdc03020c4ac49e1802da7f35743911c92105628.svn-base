#ifndef EXOVerifyDAQ_hh
#define EXOVerifyDAQ_hh

#include "EXOAnalysisModule.hh"
#include "Rtypes.h"
#include <map>

class EXOVerifyDAQ : public EXOAnalysisModule
{
private:
  std::map<Int_t, size_t> fChannelNumSamples;
  std::map<Int_t, Long64_t> fChannelSum;
  std::map<Int_t, Long64_t> fChannelSumSquare;

public:
  EXOVerifyDAQ();
  EventStatus BeginOfRun(EXOEventData*);
  EventStatus ProcessEvent(EXOEventData* ED);
  EventStatus EndOfRun(EXOEventData* ED);
  DEFINE_EXO_ANALYSIS_MODULE(EXOVerifyDAQ)
};
#endif
