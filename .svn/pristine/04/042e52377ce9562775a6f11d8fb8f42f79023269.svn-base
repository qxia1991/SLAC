#ifndef EXOTriggerConfig_hh
#define EXOTriggerConfig_hh

#include "EXOUtilities/EXODimensions.hh"
#include <utility>
#include <map>
#include <set>
#include <string>
#include <cstddef> //for size_t

class EXOTriggerConfig
{
  public:

    typedef std::pair<int,int> Intpair;

    enum System{
      APD,
      VGD,
      HVD
    };

    struct Trigger
    {
      int fIndex;
      int fThreshold;
      int fPrescale;
      std::string fName;
    };

    struct PairCompare{
      bool operator()(const Intpair &left, const Intpair &right) const{
        return CHANNELS_PER_DAQCARD*left.first+left.second < CHANNELS_PER_DAQCARD*right.first+right.second;
      }
    };

    struct TriggerCompare{
      bool operator()(const Trigger &left, const Trigger &right) const{
        return left.fThreshold < right.fThreshold;
      }
    };

    typedef std::map<Intpair,int,PairCompare> PairIntMap;
    typedef std::set<Intpair,PairCompare> PairIntSet;
    typedef std::set<Trigger,TriggerCompare> TriggerSet;

  public:

    EXOTriggerConfig();
    void PrintConfig();

    TriggerSet fIndividualTriggerSet;
    TriggerSet fSumTriggerSet;
    PairIntMap fChannelOffsets;
    PairIntSet fDisabledChannels;
    System fSystem;
    int fIndividualHysteresis;
    int fSumHysteresis;
    size_t fBackgroundCount;
    int fBaselineReference;
    int fBaselineComputedFromRun;
};

#endif
