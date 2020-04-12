#ifndef EXOWeightedAPDEnergy_hh
#define EXOWeightedAPDEnergy_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include <string>
#include <map>
#include <vector>
class EXOTalkToManager;
class EXOEventData;
class TH3D;
class TGraph;
class TFile;

class EXOWeightedAPDEnergy : public EXOAnalysisModule
{

  TFile* fRootFile;
  std::vector<unsigned char> fAPDs;
  std::map<unsigned char, TH3D*> fLightMaps;
  std::map<unsigned char, TGraph*> fGainMaps;


 public:
  EXOWeightedAPDEnergy();
  int Initialize();
  int TalkTo(EXOTalkToManager *talktoManager);
  EventStatus ProcessEvent(EXOEventData *ED);
  ~EXOWeightedAPDEnergy();

  std::string fFilename;
  void SetFilename(std::string name) { fFilename = name; }

  DEFINE_EXO_ANALYSIS_MODULE(EXOWeightedAPDEnergy)
};
#endif
