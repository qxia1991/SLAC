#ifndef EXOTriggerModule_hh
#define EXOTriggerModule_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOTrigger.hh"
#include "EXOUtilities/EXOTriggerConfigReader.hh"
#include <map>

class EXOTriggerModule : public EXOAnalysisModule 
{

public :

  EXOTriggerModule();
  ~EXOTriggerModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  int ShutDown();
  void SetOverwrite(bool val){fOverwrite = val;}; //should an existing trigger output in the EXOEventHeader be overwritten?
  void SetConfigFile(std::string val){fConfigFile = val;}; //set the xml file that contains the physics trigger configuration (needs an EXO_PhysicsTrigger tag as root node)
  void SetOffsetsFile(std::string val){fOffsetsFile = val;}; //set the xml file that contains the baseline offset configuration (needs an EXO_BaselineOffest tag as root node)
  void SetVerbosity(int level){fVerbose = level;};
  int TalkTo(EXOTalkToManager *tm);

private :

  enum System{kAPD = 0,
              kVGD = 1,
              kHVD = 2,
              kUnknown};

  int GetTriggerInfo(EXOEventData *ED);
  int GetTotalChannelIndex(int individualchannelindex, System sys) const;
  int GetNChannels(System sys) const;
  void PrintIndividualWaveform(const EXOEventData *ED, int threshold, int channel) const;

  EXOTriggerConfigReader fConfigReader;
  EXOTrigger fAPDTrigger;
  EXOTrigger fVGDTrigger;
  EXOTrigger fHVDTrigger;

  bool fOverwrite; //should old trigger data be overwritten?
  bool fPrevtrig; //has a trigger previously been applied?
  int fVerbose; //verbosity level: 0 - no verbosity
                //                 1 - text only
                //                 2 - waveform plots

  std::string fConfigFile;
  std::string fOffsetsFile;

  typedef std::map<int,int> Intmap;
  Intmap fAPD_IndividualToTotalChannel;
  Intmap fVGD_IndividualToTotalChannel;
  Intmap fHVD_IndividualToTotalChannel;
  
  DEFINE_EXO_ANALYSIS_MODULE( EXOTriggerModule )
};
#endif

  


