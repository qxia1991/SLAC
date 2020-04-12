#include "EXOReconstruction/EXOSignalsPlot.hh"
#include "EXOReconstruction/EXOSignal.hh"
#include "EXOReconstruction/EXOSignalModel.hh"
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TF1.h"
#include "TLine.h"
#include "TList.h"

using namespace std;

EXOSignalsPlot::EXOSignalsPlot()
: fHist(0),
  fMinTime(0),
  fMaxTime(0)
{

}

EXOSignalsPlot::EXOSignalsPlot(const EXOSignalsPlot& other)
{
  fHist = new TH1D(*other.fHist);
  TIter next(other.fHist->GetListOfFunctions());
  TObject* obj = 0;
  while( (obj = next()) ){
    fHist->GetListOfFunctions()->Add(obj->Clone());
  }
  fMinTime = other.fMinTime;
  fMaxTime = other.fMaxTime;
}

EXOSignalsPlot& EXOSignalsPlot::operator=(const EXOSignalsPlot& other)
{
  delete fHist;
  fHist = new TH1D(*other.fHist);
  TIter next(other.fHist->GetListOfFunctions());
  TObject* obj = 0;
  while( (obj = next()) ){
    fHist->GetListOfFunctions()->Add(obj->Clone());
  }
  fMinTime = other.fMinTime;
  fMaxTime = other.fMaxTime;
  return *this;
}

EXOSignalsPlot::~EXOSignalsPlot()
{
  Clear();
}

void EXOSignalsPlot::Clear()
{
  fMinTime = 0;
  fMaxTime = 0;
  delete fHist;
}

void EXOSignalsPlot::AddThreshold(double thresh)
{
  if(not fHist){
    LogEXOMsg("Can not add signal to plot. Add waveform first",EEError);
    return;
  }
  TF1* thresholdFunc = new TF1("threshold",
                           "[0]",
                           fHist->GetXaxis()->GetXmin(),
                           fHist->GetXaxis()->GetXmax());
  thresholdFunc->SetParameter(0, thresh);
  thresholdFunc->SetLineColor(kRed);
  fHist->GetListOfFunctions()->Add(thresholdFunc);
}

void EXOSignalsPlot::AddSignal(const EXOSignal& sig, Color_t color)
{
  if(not fHist){
    LogEXOMsg("Can not add signal to plot. Add waveform first",EEError);
    return;
  }
  TLine* line = new TLine(sig.fTime/CLHEP::microsecond,
                          fHist->GetMinimum(),
                          sig.fTime/CLHEP::microsecond,
                          fHist->GetMaximum());
  line->SetLineColor(color);
  fHist->GetListOfFunctions()->Add(line);
}

void EXOSignalsPlot::AddSignals(const EXOChannelSignals& sigs, Color_t color)
{
  sigs.ResetIterator();
  const EXOSignal* sig = sigs.Next();
  while(sig){
    AddSignal(*sig,color);
    sig = sigs.Next();
  }
}

void EXOSignalsPlot::AddSignals(const EXOChannelSignals& sigs, const EXOSignalModel& model, Color_t color)
{
  if(not fHist){
    LogEXOMsg("Can not add signal to plot. Add waveform first",EEError);
    return;
  }
  //Rescale histogram x-axis from mus to ns
  TAxis* a = fHist->GetXaxis();
  a->Set(a->GetNbins(),fMinTime,fMaxTime);
  a->SetTitle("t [ns]");
  TF1* signalFunc = new TF1(model.GetFunction(fMinTime,fMaxTime,sigs.GetNumSignals()));
  signalFunc->SetLineColor(color);
  signalFunc->SetNpx(fHist->GetNbinsX());
  const EXOSignal* sig = NULL;
  sigs.ResetIterator();
  size_t j=0;
  while( (sig = sigs.Next()) != NULL){
    signalFunc->SetParameter(2*j+1,sig->fMagnitude);
    signalFunc->SetParameter(2*j+2,sig->fTime);
    j++;
  }
  fHist->GetListOfFunctions()->Add(signalFunc);
}

