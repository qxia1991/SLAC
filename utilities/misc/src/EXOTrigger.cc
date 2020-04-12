#include "EXOUtilities/EXOTrigger.hh"
#include "EXOUtilities/EXOTriggerConfig.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include <iostream>

EXOTrigger::EXOTrigger(const EXOTriggerConfig* conf)
:fConfig(conf), fNAverages(0), fMaxChannelValue(0), fChannelSum(0),
 fRunningSum(0), fMaxChannel(-1), fWhichIndivTriggerPassed(-1),
 fWhichSumTriggerPassed(-1), fIndividualTriggered(false), fSumTriggered(false)
{

}
 

void EXOTrigger::Initialize()
{
  if(fConfig->fBackgroundCount <= 0){
    fNAverages = 0;
  }
  else{
    fNAverages = 1 << (fConfig->fBackgroundCount-1); // this gives fNaverages = 2^(fConfig->fBackgroundCount-1)
  }
  for(EXOTriggerConfig::TriggerSet::iterator it = fConfig->fIndividualTriggerSet.begin(); it != fConfig->fIndividualTriggerSet.end(); it++){
    fIndividualThresholds.push_back(EXOTrigThresh(it->fPrescale,it->fThreshold));
    //fIndividualThresholds.push_back(EXOTriggerSimpleThresh(it->fPrescale,it->fThreshold));
  }
  for(EXOTriggerConfig::TriggerSet::iterator it = fConfig->fSumTriggerSet.begin(); it != fConfig->fSumTriggerSet.end(); it++){
    fSumThresholds.push_back(EXOTrigThresh(it->fPrescale,it->fThreshold));
    //fSumThresholds.push_back(EXOTriggerSimpleThresh(it->fPrescale,it->fThreshold));
  }
  fRunningSum = 0;
}

void EXOTrigger::Reset()
{
  for(size_t i=0; i<fIndividualThresholds.size(); i++){
    fIndividualThresholds[i].Reset();
  }
  for(size_t i=0; i<fSumThresholds.size(); i++){
    fSumThresholds[i].Reset();
  }
  fLastChannelSums.clear();
  fRunningSum = 0;
  fWhichIndivTriggerPassed = fWhichSumTriggerPassed = -1;
  fIndividualTriggered = false, fSumTriggered = false;
}

bool EXOTrigger::Trigger(std::vector<int> &frame, std::vector<int> &channels, int trigIn)
{
  fChannelSum = 0;
  int maxvalue = 0;
  fMaxChannel = -1;
  fMaxChannelValue = 0;
  fIndividualTriggered = false;
  fSumTriggered = false;

  //int ndeactivated = 0;
  for(size_t i=0; i<frame.size(); i++){
    if(IsDeactivated(channels[i])){
      //ndeactivated++;
    }
    else{
      fChannelSum += frame[i];
      int value = frame[i] - GetOffset(channels[i]);
      if(value > maxvalue){
        fMaxChannel = channels[i];
        maxvalue = value;
        fMaxChannelValue = frame[i];
      }
    }
  }
  fLastChannelSums.push_back(fChannelSum);
  fRunningSum += fChannelSum;
  if(fLastChannelSums.size() > fNAverages){
    fRunningSum -= fLastChannelSums[0];
    fLastChannelSums.pop_front();
  }
  fChannelSum -= GetBackgroundAverage();
  
  int trigEn = 1;
  int trigSumEn = 1;
  int trigOut = 0;
  int trigAct = 0;
  //Individual
  //for(int i=0; i<fIndividualThresholds.size(); i++){
  for(int i=fIndividualThresholds.size(); i>0; i--){
    if(fIndividualThresholds[i-1].Enabled()){
      fIndividualThresholds[i-1].State(trigIn,
                                     trigEn,
                                     trigSumEn,
                                     maxvalue + fConfig->fIndividualHysteresis,
                                     maxvalue,
                                     trigAct,
                                     trigOut);
      trigIn = trigOut;
      if(trigAct){
        fWhichIndivTriggerPassed = i-1;
        //std::cout << "value/+hysteresis " << maxvalue << "/" << maxvalue + fConfig->fIndividualHysteresis << " in channel " << fMaxChannel << " passed threshold " << fIndividualThresholds[i-1].trigTHold << std::endl;
        trigEn = 0;
        fIndividualTriggered = 1;
      }
    }
    
  }
  //Sum
  if(fLastChannelSums.size() == fNAverages){
    for(int i=fSumThresholds.size(); i>0; i--){
      if(fSumThresholds[i-1].Enabled()){
        fSumThresholds[i-1].State(trigIn,
                                trigEn,
                                trigSumEn,
                                fChannelSum + fConfig->fSumHysteresis,
                                fChannelSum,
                                trigAct,
                                trigOut);
        trigIn = trigOut;
        if(trigAct){
          fWhichSumTriggerPassed = i-1;
          //std::cout << "value/+hysteresis " << fChannelSum << "/" << fChannelSum + fConfig->fSumHysteresis << " passed threshold " << fSumThresholds[i-1].trigTHold << std::endl;
          trigSumEn = 0;
          fSumTriggered = 1;
        }
      }
    }
  }
  return 2*fSumTriggered + fIndividualTriggered;
}

bool EXOTrigger::IsDeactivated(int physicalchannel) const
{
  int module = -1, channel = -1, address = -1;
  fChannelMap->get_card_and_channel(physicalchannel,module,channel);
  if(module >= 2*NUM_DAQCARDS_PER_SECTION){
    module -= 2*NUM_DAQCARDS_PER_SECTION;
  }
  else if(module >= NUM_DAQCARDS_PER_SECTION){
    module -= NUM_DAQCARDS_PER_SECTION;
  }
  EXOTriggerConfig::Intpair key(module,channel);
  EXOTriggerConfig::PairIntSet::const_iterator it = fConfig->fDisabledChannels.find(key);
  if(it == fConfig->fDisabledChannels.end()){
    return false;
  }
  return true;
}

int EXOTrigger::GetTotalOffset(int physicalchannel) const
{
  return GetOffset(physicalchannel) + fConfig->fBaselineReference;
}

int EXOTrigger::GetOffset(int physicalchannel) const
{
  int module = -1, channel = -1, address = -1;
  fChannelMap->get_card_and_channel(physicalchannel,module,channel);
  if(module >= 2*NUM_DAQCARDS_PER_SECTION){
    module -= 2*NUM_DAQCARDS_PER_SECTION;
  }
  else if(module >= NUM_DAQCARDS_PER_SECTION){
    module -= NUM_DAQCARDS_PER_SECTION;
  }
  EXOTriggerConfig::Intpair key(module,channel);
  EXOTriggerConfig::PairIntMap::const_iterator it = fConfig->fChannelOffsets.find(key);
  if(it == fConfig->fChannelOffsets.end()){
    std::cout << "EXOTrigger: no offset for channel " << physicalchannel << "(" << module << "|" << channel << ") found!" << std::endl;
    return 0;
  }
  //std::cout << "EXOTrigger: offset for channel " << physicalchannel << "(" << module << "|" << channel << "): " << it->second << std::endl;
  return it->second;
}

int EXOTrigger::GetBackgroundAverage() const
{
  size_t size = fLastChannelSums.size();
  if(size != fNAverages || size == 0){
    return 0;
  }
  return fRunningSum / size;
}

int EXOTrigger::GetPassedIndivThreshold() const
{
  if(!IndividualTriggered()){
    return 0;
  }
  EXOTriggerConfig::TriggerSet::iterator it = fConfig->fIndividualTriggerSet.begin();
  for(int i=0; i<fWhichIndivTriggerPassed; i++){
    it++;
  }
  return it->fThreshold;
}

int EXOTrigger::GetPassedSumThreshold() const
{
  if(!SumTriggered()){
    return 0;
  }
  EXOTriggerConfig::TriggerSet::iterator it = fConfig->fSumTriggerSet.begin();
  for(int i=0; i<fWhichSumTriggerPassed; i++){
    it++;
  }
  return it->fThreshold;
}

int EXOTrigger::GetLowestIndivThreshold() const
{
  EXOTriggerConfig::TriggerSet::iterator it = fConfig->fIndividualTriggerSet.begin();
  if(it == fConfig->fIndividualTriggerSet.end()){
    return 0;
  }
  return it->fThreshold;
}

int EXOTrigger::GetLowestSumThreshold() const
{
  EXOTriggerConfig::TriggerSet::iterator it = fConfig->fSumTriggerSet.begin();
  if(it == fConfig->fSumTriggerSet.end()){
    return 0;
  }
  return it->fThreshold;
}

int EXOTrigger::GetNSumThresholds() const
{
  return fConfig->fSumTriggerSet.size();
}

int EXOTrigger::GetNIndividualThresholds() const
{
  return fConfig->fIndividualTriggerSet.size();
}
