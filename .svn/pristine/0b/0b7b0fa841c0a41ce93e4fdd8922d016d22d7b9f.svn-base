#ifndef EXORealNoiseModule_hh
#define EXORealNoiseModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
class EXOAnalysisManager;
class EXOEventData;
class EXOTalkToManager;
class TFile;
class TTree;
class EXOEventData;

class EXORealNoiseModule : public EXOAnalysisModule 
{

private :

  bool fMakeNoiseFile;
  bool fUseNoiseFile;
  int fNumRandomTrigs;
  int fNumNoiseTraces;
  std::string fNoiseFilenameParam;
  int fOffset;
  int fNoiseIndex;
  int fRequestedLength;
  bool fSkipAPDs;
  bool fSkipVWires;
  bool fSkipUWires;

  TFile* fNoiseFile;
  TTree* fNoiseTree;
  EXOEventData* fNoiseEventData;

protected:

public :

  EXORealNoiseModule();
  ~EXORealNoiseModule();

  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);

  void SetMakeNoiseFile(bool aval);
  void SetUseNoiseFile(bool aval) { fUseNoiseFile = aval; }
  void SetNoiseFilename(std::string aval) {fNoiseFilenameParam = aval;}
  void SetRequestedLength(int aval) {fRequestedLength = aval;}
  void SetSkipAPDs(bool aval) {fSkipAPDs = aval;}
  void SetSkipVWires(bool aval) {fSkipVWires = aval;}
  void SetSkipUWires(bool aval) {fSkipUWires = aval;}



  DEFINE_EXO_ANALYSIS_MODULE( EXORealNoiseModule )


};
#endif

  


