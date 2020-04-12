#ifndef EXODefineCrossProductProcessList_hh
#define EXODefineCrossProductProcessList_hh

#include "EXOReconstruction/EXOVDefineReconProcessList.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOFrequencyPeakFilter.hh"
#include <map>
#include <string>

class EXODefineCrossProductProcessList: public EXOVDefineReconProcessList
{
  public:
    EXODefineCrossProductProcessList();
    typedef std::map<int,EXOWaveform> WFMap;
    typedef std::map<int,EXOFrequencyPeakFilter> FilterMap;
    virtual EXOReconProcessList GetProcessList(const EXOReconProcessList& inputList) const;
    void SetFile(const std::string filename);
    void SetNSigma(double nsigma);
    void SetNBins(int nbins);
    void SetCollectionName(std::string val);
    void UpdateFilters() const;
    void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);
    void ProcessVerbosity(const EXOWaveform& raw, const EXOWaveform& filtered) const;

  protected:
    mutable WFMap fWFMap;
    mutable FilterMap fFilterMap;
    std::string fFilename;
    double fNSigma;
    int fNBins;
    std::string fCollectionName;
    mutable bool fNeedUpdate;
};

#endif
