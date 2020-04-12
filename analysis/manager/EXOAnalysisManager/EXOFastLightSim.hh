#ifndef EXOFastLightSim_hh
#define EXOFastLightSim_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include <string>
#include <map>
#include <vector>

class EXOTalkToManager;
class EXOEventData;
class EXOMonteCarloData;
class TH3D;
class TGraph;
class TFile;


class EXOFastLightSim : public EXOAnalysisModule
{

 private:
  void ClearAPDHits(EXOMonteCarloData& MonteCarloData); 
  void FastAPDSim(EXOMonteCarloData& MonteCarloData);
  void FullLightMapSim(EXOMonteCarloData& MonteCarloData);
  void GetAcceptance(double z, double r, double& acceptance1, double& acceptance2);
  bool IsChannelMissing(unsigned char gang);
  std::vector<unsigned char> fAPDs;
  std::map<unsigned char, TH3D*> fLightMaps;
  std::map<unsigned char, TGraph*> fGainMaps;
 
  //These are the Talk To THings.  The order matters or it will fail when you commit. 
  TFile* fRootFile;
  double fScintYield;
  bool   fDoFastSim;
  unsigned int fDBTime;
  double fLMAvg;
  bool fAddShotNoise;
  bool fSkipLightMap;

 public:
  EXOFastLightSim();
  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *talktoManager);
  EventStatus ProcessEvent(EXOEventData *ED);
  ~EXOFastLightSim();

  std::string fFilename;
  void SetFilename(std::string name) { fFilename = name; }
  void SetScintYield(double val) {fScintYield = val;}
  void SetDoFastSim(bool val) {fDoFastSim = val;}
  void SetDBTime(unsigned int atime){fDBTime = atime;}
  void SetLMAvg(double val){fLMAvg = val;}
  void SetAddShotNoise(bool val) {fAddShotNoise = val;}
  void SetSkipLightMap(bool val) {fSkipLightMap = val;}

  DEFINE_EXO_ANALYSIS_MODULE(EXOFastLightSim)
};
#endif
