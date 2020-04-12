#ifndef EXOAPDSignalFinderModule_hh
#define EXOAPDSignalFinderModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOMatchedFilterFinder.hh"
#include "EXOReconstruction/EXODefineAPDSumProcessList.hh"
#include "EXOUtilities/EXOControlRecord.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;
class EXOChannelMap;

class EXOAPDSignalFinderModule : public EXOAnalysisModule 
{

private :

  // These are example parameters for this module 

  //double        doubleparam;
  //int           intparam;
  //bool          boolparam;
  //std::string   stringparam;

  //double        fAPDSearchWindow;
  EXOSignalModelManager      fSignalModelManager;
  EXOMatchedFilterFinder     fMatchedFilterFinder;
  EXODefineAPDSumProcessList fDefineAPDSums;
  const EXOChannelMap*       fChannelMap;
  std::string fElectronicsDatabaseFlavor;
  //EXOBeginRecord::RunFlavor fRunFlavor;

protected:

public :

  EXOAPDSignalFinderModule();
  ~EXOAPDSignalFinderModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  //void SetDoubleParam(double aval) { doubleparam = aval; }
  //void SetIntParam(int aval) { intparam = aval; }
  //void SetBoolParam(bool aval) { boolparam = aval; }
  //void SetStringParam(std::string aval) { stringparam = aval; }
  void CallBack();

  //void SetAPDSearchWindow(double aval) { fAPDSearchWindow = aval; }

  DEFINE_EXO_ANALYSIS_MODULE( EXOAPDSignalFinderModule )

};
#endif

