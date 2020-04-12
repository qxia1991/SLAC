#ifndef EXOUWireInductionModule_hh
#define EXOUWireInductionModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include "EXOReconstruction/EXOMatchedFilterFinder.hh"
#include "EXOReconstruction/EXOYMatchExtractor.hh"
#include "EXOReconstruction/EXOMultipleSignalFinder.hh"
#include "EXOReconstruction/EXOSignalFitter.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;

class EXOUWireInductionModule : public EXOAnalysisModule 
{

private :

  EXOBaselineAndNoiseCalculator fBaselineCalculator;


protected:

  EventStatus CalculatePulseIntegral(EXOEventData* ED);
  EventStatus CalculatePulseTiming(EXOEventData* ED);
  EventStatus CalculateFitChi2Ratio(EXOEventData* ED);
  EventStatus CalculateMaxNeighEng(EXOEventData* ED);

  void UnshapeSignal(EXODoubleWaveform& filtered_signal, const EXOTransferFunction& tf) const;  

  void SetShapingPrePulse(size_t p) {fShapingPrePulseLength = p;}
  void SetShapingPostPulse(size_t p) {fShapingPostPulseLength = p;}
  void SetIntegralPrePulse(size_t p) {fIntegLowerBound = p;}
  void SetIntegralPostPulse(size_t p) {fIntegUpperBound = p;}
  void SetMaxMinPrePulse(size_t p) {fMaxMinLowerBound = p;}
  void SetMaxMinPostPulse(size_t p) {fMaxMinUpperBound = p;}
  void SetElectronicDatabaseFlavor(std::string aval) { fElectronicsDatabaseFlavor = aval; }
  void SetMaxMinInductionCut(Double_t dval) {fMaxMinInductionBound = dval;}
  void SetIntegInductionCut(Double_t dval) {fIntegInductionBound = dval;}
  void SetChi2RatInductionCut(Double_t dval) {fChi2RatInductionBound = dval;}
  void SetNeighEngInductionCut(Double_t dval) {fNeighEngInductionBound = dval;}

  size_t fShapingPrePulseLength;  // Number of samples before signal to start pulse unshaping
  size_t fShapingPostPulseLength;   // Number of samples after signal to end pulse unshaping
  std::string fElectronicsDatabaseFlavor;
  Double_t fChargeMatchTime;  // Time in ns to look for neighboring u-wire signal
  size_t fIntegLowerBound;  // Number of samples before signal time to start pulse integral
  size_t fIntegUpperBound;  // Number of samples after signal time to end pulse integral
  size_t fMaxMinLowerBound;  // Number of samples before signal time to start searching for pulse max/min
  size_t fMaxMinUpperBound;  // Number of samples after signal time to start searching for pulse max/min
  Double_t fMaxMinLowerFrac;  // Fraction of pulse height used to set pulse minimum
  Double_t fMaxMinUpperFrac;  // Fraction of pulse height used to set pulse maximum
  Double_t fMaxMinInductionBound; // Cut on min-to-max time used to identify induction signals
  Double_t fIntegInductionBound; // Cut on normalized integral used to identify induction signals
  Double_t fChi2RatInductionBound; // Cut on ratio of deposition to induction fit chi^2 
  Double_t fNeighEngInductionBound; // Cut on nearest neighbor energy used to identify induction signals

public :

  EXOUWireInductionModule();
  ~EXOUWireInductionModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);


  void CallBack();


  DEFINE_EXO_ANALYSIS_MODULE( EXOUWireInductionModule )

};
#endif

  


