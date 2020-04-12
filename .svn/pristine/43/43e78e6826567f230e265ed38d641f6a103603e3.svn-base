#ifndef EXOSignalFitterChiSquare_hh
#define EXOSignalFitterChiSquare_hh

#include "TMinuitMinimizer.h"
#include "Math/Functor.h"
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXOFitRanges.hh"
#include "EXOReconstruction/EXOSignalsPlot.hh"
#include <cstddef> //for size_t

class EXOFitRanges;
class EXOSignalModel;
class TList;

class EXOSignalFitterChiSquare 
{
  public:
    EXOSignalFitterChiSquare();

    virtual ~EXOSignalFitterChiSquare() {}

    virtual void Minimize();

    virtual void AddSignalsWithFitModel(const EXOChannelSignals& sigs,
                                        const EXOSignalModel& model,
                                        const EXOFitRanges& ranges,
                                        const std::vector<std::pair<size_t, size_t> >& includeInFit,
                                        double noise,
                                        double baseline
                                        );

    // Reset to a pristine state
    virtual void Reset() { fAllSignals.clear(); }

    // Return the minimizer
    const TMinuitMinimizer& GetMinimizer() const
      { return fMinimizer; }

    // Clear the signals added by AddSignalsWithFitModel; this lets us specify new ranges for a chi-square.
    // Existing fits are left in place.
    void ClearSignals() {fAllSignals.clear();}

    double CalculateChiSquare(const double *x) const;

    // Get the results of the fit
    size_t GetNumberOfFitChannels() const 
      { return fAllSignals.size(); }
    const EXOChannelSignals& GetFitChannelSignalsAt(size_t i) const
      { return fAllSignals[i].fSigs; }

    std::vector<EXOSignalsPlot> GetPlotOfResults() const;
    void GetHistPlotOfResults(TList& alist) const;

  protected:
    virtual void PrepareMinimizer();
    virtual void SaveResults();

    // Struct to describe the fitting
    typedef std::vector<EXOFitRanges::EXORanges> ParamVec;
    struct ChiSquareSignals {
        EXOChannelSignals     fSigs;        // Signals on the channel
        const EXOSignalModel* fModel;       // Signal model
        EXODoubleWaveform     fFitWaveform; // Double Waveform
        std::vector<std::pair<size_t, size_t> > fIncludeRanges; // ranges to include in evaluations.
        ParamVec              fParameterSettings; 
        double                fNoise;
    };

    TMinuitMinimizer              fMinimizer; 
    std::vector<ChiSquareSignals> fAllSignals;
    size_t                        fDimension;
    ROOT::Math::Functor           fFunc;
  private:
    mutable std::vector<double> fTmp; // To avoid reallocation of space every time we evaluate a chi-square.
};

#endif /* EXOSignalFitterChiSquare_hh */
