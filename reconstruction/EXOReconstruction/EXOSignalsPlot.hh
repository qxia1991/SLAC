#ifndef EXOSignalsPlot_hh
#define EXOSignalsPlot_hh

#include "EXOUtilities/EXOTemplWaveform.hh"
#include "TH1D.h"
#include <vector>
#include <string>

class EXOSignal;
class EXOSignalModel;
class EXOChannelSignals;
class TF1;
class TLine;

class EXOSignalsPlot
{
  public:
    EXOSignalsPlot();
    EXOSignalsPlot(const EXOSignalsPlot& other);

    template <typename _T>
    EXOSignalsPlot(const EXOTemplWaveform< _T > &wf, const std::string title = "");

    ~EXOSignalsPlot();

    template <typename _T>
    void UseWaveform(const EXOTemplWaveform< _T > &wf, const std::string title = "");

    void AddSignal(const EXOSignal& sig, Color_t color=kBlue);
    void AddSignals(const EXOChannelSignals& sigs, Color_t color=kBlue);
    void AddSignals(const EXOChannelSignals& sigs, const EXOSignalModel& model, Color_t color=kRed);
    void AddThreshold(double thresh);
    void Clear();
    TH1D* GetHist() const {return fHist;}
    EXOSignalsPlot& operator=(const EXOSignalsPlot& other);

  private:
    TH1D* fHist;
    double fMinTime;
    double fMaxTime;
};

template <typename _T>
EXOSignalsPlot::EXOSignalsPlot(const EXOTemplWaveform< _T > &wf, const std::string title)
: fHist(0)
{
  UseWaveform(wf,title);
}

template <typename _T>
void EXOSignalsPlot::UseWaveform(const EXOTemplWaveform< _T > &wf, const std::string title)
{
  delete fHist;
  fHist = wf.GimmeHist();
  fHist->SetTitle(title.c_str());
  fMinTime = wf.GetMinTime();
  fMaxTime = wf.GetMaxTime();
}

#endif
