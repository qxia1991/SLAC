#include "EXOUtilities/EXOTriggerSimpleThresh.hh"

EXOTriggerSimpleThresh::EXOTriggerSimpleThresh(int prescale, int threshold)
: fPrescaleValue(prescale), fPrescale(prescale), fThreshold(threshold),
  fReducedPrescaleBefore(false), fTriggeredAtLeastOnce(false),
  fJustTriggered(false)
{

}

EXOTriggerSimpleThresh::EXOTriggerSimpleThresh()
: fPrescaleValue(0), fPrescale(0), fThreshold(0),
  fReducedPrescaleBefore(false), fTriggeredAtLeastOnce(false),
  fJustTriggered(false)
{

}

bool EXOTriggerSimpleThresh::Trigger(int value)
{
  fJustTriggered = false;
  if(fReducedPrescaleBefore){
    if(value+fHysteresis < fThreshold){
      fReducedPrescaleBefore = false;
      return false;
    }
    if(value < fThreshold){
      return false;
    }
  }

  if(value < fThreshold){
    return false;
  }
  if(fPrescale > 0){
    fPrescale--;
    fReducedPrescaleBefore = true;
    return false;
  }
  fPrescale = fPrescaleValue;
  fReducedPrescaleBefore = true;
  fTriggeredAtLeastOnce = true;
  fJustTriggered = true;
  return true;
}

void EXOTriggerSimpleThresh::Reset()
{
  fPrescale = fPrescaleValue;
  fReducedPrescaleBefore = false;
  fTriggeredAtLeastOnce = false;
  fJustTriggered = false;
}

void EXOTriggerSimpleThresh::IncrementPrescale()
{
  if(fPrescale < fPrescaleValue){
    fPrescale++;
  }
}

bool EXOTriggerSimpleThresh::JustTriggered()
{
  return fJustTriggered;
}
