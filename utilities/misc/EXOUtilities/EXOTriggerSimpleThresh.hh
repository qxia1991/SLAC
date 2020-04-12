#ifndef EXOTriggerSimpleThresh_hh
#define EXOTriggerSimpleThresh_hh

class EXOTriggerSimpleThresh
{
  public:
    EXOTriggerSimpleThresh(int prescale, int threshold);
    EXOTriggerSimpleThresh();
    bool Trigger(int value);
    void Reset();
    void IncrementPrescale();
    bool JustTriggered();
    int GetThreshold(){return fThreshold;};
    void SetPrescaleValue(int value){fPrescaleValue = value;};
    void SetThreshold(int value){fThreshold = value;};
    void SetHysteresis(int value){fHysteresis = value;};

  private:
    int fPrescaleValue;
    int fPrescale;
    int fThreshold;
    int fHysteresis;

    bool fReducedPrescaleBefore;
    bool fTriggeredAtLeastOnce;
    bool fJustTriggered;
};

#endif
