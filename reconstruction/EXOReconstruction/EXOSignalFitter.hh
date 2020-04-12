#ifndef EXOSignalFitter_hh
#define EXOSignalFitter_hh

#include "EXOReconstruction/EXOVSignalParameterExtractor.hh"
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include <map>
#include <cstddef> //for size_t

class EXOWaveform;
class EXOSignalFitterChiSquare;
class EXOSignalFitter : public EXOVSignalParameterExtractor 
{
  public:
    EXOSignalFitter();
    virtual EXOSignalCollection Extract(
      const EXOReconProcessList& processList, 
      const EXOSignalCollection& inputSignals) const;

    void SetTriggerSample(size_t trig_sample)
      { fTriggerSample = trig_sample; }

    void SetChannelFitChiSquareCut(double val)
      { fChannelFitChiSquareCut = val; }
    void SetUpperFitBoundWireMicroseconds(double val);
    void SetLowerFitBoundWireMicroseconds(double val);
    void SetUpperFitBoundWireRestrMicroseconds(double val);
    void SetLowerFitBoundWireRestrMicroseconds(double val);
    void SetUpperFitBoundAPDMicroseconds(double val);
    void SetLowerFitBoundAPDMicroseconds(double val);
    void SetUpperFitBoundVWireMicroseconds(double val);
    void SetLowerFitBoundVWireMicroseconds(double val);

  protected:
    double fChannelFitChiSquareCut;
    double fUpperFitBoundaryWire;
    double fLowerFitBoundaryWire;
    double fUpperFitBoundaryWireRestr;  //upper boundary for chi^2 calculation on restricted window
    double fLowerFitBoundaryWireRestr;  //lower boundary for chi^2 calculation on restricted window
    double fUpperFitBoundaryVWire;
    double fLowerFitBoundaryVWire;
    double fUpperFitBoundaryAPD;
    double fLowerFitBoundaryAPD;
    size_t fTriggerSample;

    mutable EXOMiscUtil::ChannelInfoMap fChannelInfoCache;
    mutable bool fAPDSumSignalsHaveBeenCollected;
    mutable EXOChannelSignals fAPDFoundSignals;

    mutable int fNumberFitCycles;
    mutable int fNumberTotalMinuitCalls;

    EXOBaselineAndNoiseCalculator fBaselineCalculator;

    void ResetCaches() const;
    std::pair<double,double> FitAndGetChiSquare(EXOChannelSignals& sigs) const;
    std::pair<double,double> FitAndGetChiSquareInd(EXOChannelSignals& sigs) const;

    void AddSignalsToFitter(const EXOChannelSignals& sigs,
                            EXOSignalFitterChiSquare& fitter,
                            double t_min,
                            double t_max,
                            const std::vector<std::pair<size_t, size_t> >& includeInFit) const;

    double GetOrCalculateNoise(const EXOWaveform& wf) const;
    double GetOrCalculateBaseline(const EXOWaveform& wf) const;
    const EXOChannelSignals& GetSignalsFromAPDSumFits(const EXOSignalCollection& collect) const; 

    void HandleVerbosity(const EXOSignalFitterChiSquare& fitter) const;

    void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

  private:
    size_t ConsolidateIncludeInFitVector(std::vector<std::pair<size_t, size_t> >& includeInFit) const;
};

#endif /* EXOSignalFitter_hh */
