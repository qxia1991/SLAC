#ifndef EXOVWaveformExtractor_hh
#define EXOVWaveformExtractor_hh

#include <string>
#include <map>
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOMiscUtil.hh"

class EXOVWaveformExtractor
{
  public:
    EXOVWaveformExtractor(const std::string& defaultParameter);
    virtual ~EXOVWaveformExtractor(){};

    virtual double Extract(const EXODoubleWaveform& wf) const;
    virtual const EXOMiscUtil::ParameterMap& ExtractAll(const EXODoubleWaveform& wf) const;

  protected:
    virtual void DoExtractAll(const EXODoubleWaveform& wf) const = 0;
    EXOMiscUtil::ParameterMap& GetParameterMapForUpdates() const;

  private:
    EXOVWaveformExtractor(){}; //Forces derived classes to use non-default constructor
    std::string fDefaultParameter;
    mutable EXOMiscUtil::ParameterMap fParameterMap;
};

#endif
