#ifndef EXOCopyDenoisedModule_hh
#define EXOCopyDenoisedModule_hh

#include <string>
#include <vector>
#include <map>
#include "TFile.h"
#include "TTree.h"

#include "EXOAnalysisModule.hh"

class EXOEventData;

class EXOCopyDenoisedModule : public EXOAnalysisModule
{
public:
  EXOCopyDenoisedModule();

  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData* ED);
  
  int TalkTo(EXOTalkToManager *tm);
  void SetDenoisedEnergyFlavor(std::string flavor);
  void SetDenoisedEnergyFile(std::string name);
  
  DEFINE_EXO_ANALYSIS_MODULE(EXOCopyDenoisedModule)

  struct EXODenoisedInfo
  {
    Int_t fRunNumber;
    Int_t fEventNumber, fNsc, fDenoisingInternalCode; 
    Double_t fDenoisedEnergy, fDenoisedError, fX, fY, fZ, fTime;
    
    EXODenoisedInfo(){}
    
    EXODenoisedInfo(Int_t run, Int_t event, Int_t nsc, Double_t denoised, Double_t error, Int_t code, Double_t x, Double_t y, Double_t z, Double_t time){
      fRunNumber = run;
      fEventNumber = event;
      fNsc = nsc;
      fDenoisedEnergy = denoised;
      fDenoisedError = error;
      fDenoisingInternalCode = code;
      fX = x; fY = y; fZ = z; fTime = time;
    }
  };
  
protected:

  void ClearDenoisedInfo();
  size_t GetDenoisedInfoIdx(int event, double time);
  
  std::string fDenoisedEnergyFlavor;
  std::vector<EXODenoisedInfo> fDenoisedInfo;  
};
#endif
