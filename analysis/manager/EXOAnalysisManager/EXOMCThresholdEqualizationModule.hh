#ifndef EXOMCThresholdEqualizationModule_hh
#define EXOMCThresholdEqualizationModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <string>
#include "TRandom3.h"

class EXOEventData;
class EXOEventHeader;
class EXOTalkToManager;
class EXOAPDSignal;

class EXOMCThresholdEqualizationModule : public EXOAnalysisModule 
{

private :
  mutable TRandom3 fRandGen;
  bool fEqualizeAPD;
  bool fEqualizeU;
  bool fEqualizeV;
  double fAPDMatchTime;

protected:
  bool ShouldSignalSurvive(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude, const EXOEventHeader& header) const;
  void RemoveAPDSignals(EXOEventData* ED, std::vector<EXOAPDSignal*>& sumSignals) const;

public :

  EXOMCThresholdEqualizationModule();
  ~EXOMCThresholdEqualizationModule() {};

  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);

  void SetSeed(unsigned int s){fRandGen.SetSeed(s);}
  void SetEqualizeAPD(bool val){fEqualizeAPD = val;}
  void SetEqualizeU(bool val){fEqualizeU = val;}
  void SetEqualizeV(bool val){fEqualizeV = val;}

  DEFINE_EXO_ANALYSIS_MODULE( EXOMCThresholdEqualizationModule )

};
#endif

  


