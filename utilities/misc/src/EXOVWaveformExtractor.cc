#include "EXOUtilities/EXOVWaveformExtractor.hh"

using namespace std;

EXOVWaveformExtractor::EXOVWaveformExtractor(const string& defaultParameter)
: fDefaultParameter(defaultParameter)
{

}

EXOMiscUtil::ParameterMap& EXOVWaveformExtractor::GetParameterMapForUpdates() const
{
  fParameterMap.clear();
  return fParameterMap;
}

double EXOVWaveformExtractor::Extract(const EXODoubleWaveform& wf) const
{
  DoExtractAll(wf);
  EXOMiscUtil::ParameterMap::iterator iter = fParameterMap.find(fDefaultParameter);
  if(iter == fParameterMap.end()){
    LogEXOMsg("Implementation does not contain default parameter!",EECritical);
  }
  return iter->second;
}

const EXOMiscUtil::ParameterMap& EXOVWaveformExtractor::ExtractAll(const EXODoubleWaveform& wf) const
{
  DoExtractAll(wf);
  return fParameterMap;
}
