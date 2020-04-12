#ifndef EXOTreeOutputModule_hh
#define EXOTreeOutputModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXODimensions.hh"

class TTree;
class TFile;
class EXOTreeOutputModule : public EXOAnalysisModule 
{

private :

  std::string fOutputFilename;

  bool   fWriteSignals;
  bool   fOnlyUWires;
  bool   fTrimWFs;
  bool   fOnlyDNNEvents;
  double fEnergySaveCut;
  bool   fWriteMCCharge;
  bool   fCompressSignals;
  double fMaxFileSize;

  TTree *fRootTree;
  TTree *fStatisticsTree;
  TFile *fRootFile;
  EXOEventData* fLastEvent;

public :

  EXOTreeOutputModule();
  ~EXOTreeOutputModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED) { fLastEvent = 0; return kOk; }
  EventStatus ProcessEvent(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetOutputFilename(std::string aval) 
    { fOutputFilename = aval; }
  void SetWriteSignals(bool aval) { fWriteSignals = aval; }
  void SetOnlyUWires(bool aval) { fOnlyUWires = aval; }
  void SetTrimWFs(bool aval)  {fTrimWFs=aval;}
  void SetOnlyDNNEvents(bool aval) {fOnlyDNNEvents=aval;}
  void SetEnergySaveCut(double aval) {fEnergySaveCut=aval;}
  void SetWriteMCCharge(bool aval) { fWriteMCCharge = aval; }
  void SetCompressSignals(bool aval) { fCompressSignals = aval; }
  void SetMaxFileSize(double aval) { fMaxFileSize = aval; }

  DEFINE_EXO_ANALYSIS_MODULE( EXOTreeOutputModule )

};
#endif



  


