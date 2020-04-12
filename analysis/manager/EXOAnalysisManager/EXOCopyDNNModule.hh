#ifndef EXOCopyDNNModule_hh
#define EXOCopyDNNModule_hh

#include <string>
#include <vector>
#include <map>
#include "TFile.h"
#include "TTree.h"

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOControlRecord.hh"


class EXOEventData;

class EXOCopyDNNModule : public EXOAnalysisModule
{
public:
  
  EXOCopyDNNModule();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData* ED);

  int TalkTo(EXOTalkToManager *tm);

  DEFINE_EXO_ANALYSIS_MODULE(EXOCopyDNNModule)

  struct EXODNNInfo
  {
        Int_t fRunNumber;
        Int_t fEventNumber;
        Double_t fDNNVarRaw, fDNNVarRecon, fDNNChargeEnergy;
        
        EXODNNInfo(){}
        
        EXODNNInfo(Int_t run, Int_t event, Double_t dnn_raw, Double_t dnn_recon, Double_t dnn_charge){
            fRunNumber      = run;
            fEventNumber    = event;
            fDNNVarRaw      = dnn_raw;
            fDNNVarRecon    = dnn_recon;
            fDNNChargeEnergy= dnn_charge;
        }

  };

protected:

  void ClearDNNInfo();
  size_t GetDNNInfoIdx(int event);
  std::string GetDNNFileName(int runNumber);
  EXOBeginRecord::RunFlavor fRunFlavor;
  std::vector<EXODNNInfo> fDNNInfo;

};
#endif

