#ifndef EXOTrigger_hh
#define EXOTrigger_hh

#include <deque>
#include <vector>
#include "EXOUtilities/EXOTrigThresh.hh"
#include <cstddef> //for size_t

class EXOTriggerConfig;
class EXOChannelMap;

class EXOTrigger
{
  public:
    EXOTrigger(const EXOTriggerConfig* conf);
    void Initialize();
    void Reset();
    bool Trigger(std::vector<int> &frame, std::vector<int> &channels, int trigIn);
    bool IsDeactivated(int physicalchannel) const;
    int GetTotalOffset(int physicalchannel) const;
    int GetOffset(int physicalchannel) const;
    int GetBackgroundAverage() const;
    bool IndividualTriggered() const {return fIndividualTriggered;};
    int GetWhichIndivTriggerPassed() const {return fWhichIndivTriggerPassed;};
    int GetPassedIndivThreshold() const;
    int GetLowestIndivThreshold() const;
    bool SumTriggered() const {return fSumTriggered;};
    int GetWhichSumTriggerPassed() const {return fWhichSumTriggerPassed;};
    int GetPassedSumThreshold() const;
    int GetLowestSumThreshold() const;
    int GetMaxChannelValue() const {return fMaxChannelValue;};
    int GetMaxChannel() const {return fMaxChannel;};
    int GetChannelSum() const {return fChannelSum;};
    void SetChannelMap(const EXOChannelMap& cm)
      { fChannelMap = &cm; }
    int GetNSumThresholds() const;
    int GetNIndividualThresholds() const;

  private:
    EXOTrigger();
    const EXOTriggerConfig* fConfig;
    const EXOChannelMap* fChannelMap;
    std::vector<EXOTrigThresh> fIndividualThresholds;
    std::vector<EXOTrigThresh> fSumThresholds;
    //std::vector<EXOTriggerSimpleThresh> fIndividualThresholds;
    //std::vector<EXOTriggerSimpleThresh> fSumThresholds;
    size_t fNAverages;
    int fMaxChannelValue;
    int fChannelSum;
    int fRunningSum;
    int fMaxChannel;
    int fWhichIndivTriggerPassed, fWhichSumTriggerPassed;
    bool fIndividualTriggered, fSumTriggered;
    std::deque<int> fLastChannelSums;
};

#endif
