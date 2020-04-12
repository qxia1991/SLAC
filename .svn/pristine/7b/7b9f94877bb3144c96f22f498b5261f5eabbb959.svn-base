#ifndef EXOSummedWaveformFitter_hh
#define EXOSummedWaveformFitter_hh

#include "EXOAnalysisModule.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOSignalFitter.hh"
#include <string>

class EXOEventData;
class EXOTalkToManager;
class TFile;
class TTree;

class EXOSummedWaveformFitter : public EXOAnalysisModule 
{

public :

  EXOSummedWaveformFitter();

  int Initialize();
  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager*);

  void SetFilename(std::string name) { fFilename = name; }
  void SetShapingDatabaseFlavor(std::string flavor) { fElectronicsDatabaseFlavor = flavor; }
  void SetGainDatabaseFlavor(std::string flavor) { fUWireDatabaseFlavor = flavor; }
  void SetClusteringTime(double val) { fClusteringTime = val; }
  void SetUWireScaling(double val) { fUWireScalingFactor = val; }

protected:

  std::string fFilename;
  std::string fElectronicsDatabaseFlavor;
  std::string fUWireDatabaseFlavor;
  double fClusteringTime;
  double fUWireScalingFactor;

  EXOSignalModelManager fSignalModelManager;
  EXOSignalFitter fSignalFitter;

  DEFINE_EXO_ANALYSIS_MODULE( EXOSummedWaveformFitter )
};
#endif
