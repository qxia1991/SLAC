#ifndef EXOTrimWaveforms_hh
#define EXOTrimWaveforms_hh

class EXOWaveformData;

class EXOTrimWaveforms
{
  public:
    EXOTrimWaveforms();
    void TrimWaveforms(EXOWaveformData& wfData) const;
    void SetMinValue(int minValue);
    void SetMaxValue(int maxValue);
    int GetMinValue() const;
    int GetMaxValue() const;
  private:
    int fMinValue; // inclusive
    int fMaxValue; // exclusive
};

#endif
