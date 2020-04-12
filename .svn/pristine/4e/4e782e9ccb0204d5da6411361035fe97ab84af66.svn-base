#ifndef EXOYMatchExtractor_hh
#define EXOYMatchExtractor_hh

#include "EXOReconstruction/EXOVSignalParameterExtractor.hh"
#include "EXOUtilities/EXOBaselineAndNoiseCalculator.hh"
#include <vector>
#include <utility>
#include <map>

class EXOVWireThresholds;

class EXOYMatchExtractor: public EXOVSignalParameterExtractor
{
  public:
    EXOYMatchExtractor(); virtual EXOSignalCollection Extract(
      const EXOReconProcessList& processList,
      const EXOSignalCollection& inputSignals) const;
    void SetWireMatchTime(double val){fWireMatchTime = val;}
    void SetThresholdFactor(double val){fThresholdFactor = val;}
    void SetThreshold(double val){fThreshold = val;}
    void SetThresholds(const EXOVWireThresholds* val){fThresholds = val;}

  protected:
    void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

    struct SignalWithChannel
    {
      bool operator==(const SignalWithChannel &other) const {return fSignal == other.fSignal;}
      bool operator!=(const SignalWithChannel &other) const {return not(*this == other);}
      int fChannel;
      const EXOSignal *fSignal;
    };

    struct EnergySorter
    {
      bool operator()(const SignalWithChannel &lhs, const SignalWithChannel &rhs) const {return lhs.fSignal->fMagnitude > rhs.fSignal->fMagnitude;}
    };

    struct ChannelSorter
    {
      bool operator()(const SignalWithChannel &lhs, const SignalWithChannel &rhs) const {return lhs.fChannel < rhs.fChannel;}
    };

    void NotifySignalModelHasChanged(int signalOrTag, const EXOSignalModel& model);

  private:
    double ApplyYMatch(const EXOWaveform& wf, double uTime, double baseline) const;
    void FindAndAddVSignal(int uChannel, double uTime, const EXOReconProcessList& processList, EXOSignalCollection& returnCollection) const;
    std::vector< std::pair<int,double> > FakeCluster(const EXOSignalCollection& inputSignals) const;

    double fWireMatchTime; //The clustering time for u-wire signals.
    double fThresholdFactor;
    double fThreshold;
    const EXOVWireThresholds* fThresholds;
    mutable std::map<int,double> fYMatchNormalization;
    mutable EXOBaselineAndNoiseCalculator fBaselineCalculator;
};

#endif
