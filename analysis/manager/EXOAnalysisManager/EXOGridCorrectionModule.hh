#ifndef EXOGridCorrectionModule_hh
#define EXOGridCorrectionModule_hh

#include "EXOAnalysisModule.hh"
#include <string>

class EXOEventData;
class EXOChargeCluster;
class EXOGridCorrectionCalib;

class EXOGridCorrectionModule : public EXOAnalysisModule
{
public:
  EXOGridCorrectionModule();
  int TalkTo(EXOTalkToManager *tm);
  EventStatus ProcessEvent(EXOEventData* ED);
  void SetGridCorrDatabaseFlavor(std::string flavor);
  DEFINE_EXO_ANALYSIS_MODULE(EXOGridCorrectionModule)
protected:
  std::string fGridCorrDatabaseFlavor;
  double GetGridCorrection(EXOGridCorrectionCalib * aCalib,
			   EXOChargeCluster * ecc);
};
#endif
