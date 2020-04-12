#ifndef EXORisetimeModule_hh
#define EXORisetimeModule_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOUtilities/EXORisetimeCalculation.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include "EXOUtilities/EXOExtremumFinder.hh"

class EXOEventData;

class EXORisetimeModule : public EXOAnalysisModule
{
public:
  EXORisetimeModule();
  EventStatus ProcessEvent(EXOEventData* ED);

private:
  EXORisetimeCalculation fRisetimeCalculation;
  EXOBaselineAndNoiseCalculator fBaselineCalculator;
  EXOExtremumFinder fExtremumFinder;
  DEFINE_EXO_ANALYSIS_MODULE(EXORisetimeModule)
};
#endif
