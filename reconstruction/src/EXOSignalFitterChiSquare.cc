//______________________________________________________________________________
// 
// EXOSignalFitterChiSquare is a class that handles fitting to either one or
// several waveforms.  Users can add a set of signals on a channel to fit via
// AddSignalsWithFitModel.  The class is meant to generically fit one or more
// waveforms.  
// 
// An example usage is given below:
//   
//   EXOSignalFitterChiSquare chi;
//   // In the following, 
//   // : sigs is a set of signals on the channel (EXOChannelSignals), it also
//   //     includes a pointer to the data waveform on this channel.
//   // : model is the signal model of the channel (EXOSignalModel),
//   // : EXOWireFitRanges() is a class defining the parameter range behavior
//   //     for wire signals,
//   // : boolWF is an EXOBoolWaveform which defines which points of the input
//   //     waveform should be used in the fit.
//    
//   chi.AddSignalsWithFitModel( sigs, model, EXOWireFitRanges(), boolWF, 1.0 );
//   chi.Minimize();
//   for(size_t i=0;i<chi.GetNumberOfFitChannels();i++) {
//     EXOChannelSignals fitSignals = chi.GetFitChannelSignalsAt(i); 
//     .... // Process the signals somehow
//   }
// 
//______________________________________________________________________________
#include "EXOReconstruction/EXOSignalFitterChiSquare.hh"
#include "EXOReconstruction/EXOSignalModel.hh"
#include "EXOReconstruction/EXOFitRanges.hh"
#include "TString.h"
#include "TMinuit.h"
#ifdef HAVE_ROOT_ROOFIT
#include "RooPlot.h"
#include "TH1D.h"
#include "TF1.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TList.h"
#endif
#include <cassert> 
#include <algorithm> 

using namespace std;

//______________________________________________________________________________
EXOSignalFitterChiSquare::EXOSignalFitterChiSquare() :
  fDimension(0)
{
  fMinimizer.SetPrintLevel(0);
  // We explicitly set the tolerance to be the Minuit default, 0.1.  Otherwise
  // TMinuitMinimizer uses 0.0001.
  fMinimizer.SetTolerance(0.1); 
}

//______________________________________________________________________________
void EXOSignalFitterChiSquare::Minimize()
{
  // Perform the fit and save the results
  PrepareMinimizer();
  fMinimizer.Minimize();
  SaveResults();
}

//______________________________________________________________________________
void EXOSignalFitterChiSquare::PrepareMinimizer()
{
  // Prepare the minimizer with what has been filled using
  // AddSignalsWithFitModel.  This sets the proper parameter ranges/limits and
  // initial values with Minuit.

  // Prepare everything now
  fDimension = 0;
  for (size_t i=0;i<fAllSignals.size();i++) {
    ChiSquareSignals& sigs = fAllSignals[i];
    fDimension += 1 + 2*sigs.fSigs.GetNumSignals();
  } 

  // Set to call through to our class function
  fFunc = ROOT::Math::Functor(this, 
            &EXOSignalFitterChiSquare::CalculateChiSquare, 
            fDimension);

  // Set the chi-square function for the minimizer
  fMinimizer.SetFunction(fFunc);

  // This is to ensure that the print level is set.  Earlier vesions of
  // TMinuitMinimizer do not properly set the print level of Minuit.
  // *However*, TMinuitMinimizer does use gMinuit which is a global and so we
  // can call the print level on the global.  This is unfortunate, but
  // necessary to remove a lot of warning messages from the output.
  if (gMinuit) gMinuit->SetPrintLevel(-1);

  // Now set the parameter names, ranges
  Int_t ptr = 0;
  for (size_t i=0;i<fAllSignals.size();i++) {
    ChiSquareSignals& sigs = fAllSignals[i];
    
    assert(sigs.fSigs.GetNumSignals() == sigs.fParameterSettings.size());

    fMinimizer.SetFixedVariable(ptr++, 
               Form("Number of Signals Channel: %d", sigs.fSigs.GetChannel()), 
               sigs.fSigs.GetNumSignals());  

    sigs.fSigs.ResetIterator();
    const EXOSignal* signal; 
    ParamVec::iterator iter = sigs.fParameterSettings.begin();
    Int_t j = 0;
    while (((signal = sigs.fSigs.Next()) != NULL) && 
            (iter != sigs.fParameterSettings.end())) {
      // Get references to the variables
      EXOFitRanges::EXORanges& range = *iter; 

      // Make their names
      std::string energy_name = Form("Energy of signal %u", j);
      std::string time_name = Form("Time of signal %u", j);
      
      // First the magnitude
      if (range.fMagUnc == 0.0) {
        fMinimizer.SetFixedVariable(ptr++, energy_name, signal->fMagnitude); 
      } else {
        double sigMagnitude = signal->fMagnitude;
        fMinimizer.SetLimitedVariable(ptr++, energy_name, 
                                      sigMagnitude,
                                      range.fMagUnc, 
                                      range.fMagMin, 
                                      range.fMagMax); 
      }

      // Then the time
      if (range.fTimeUnc == 0.0) {
        fMinimizer.SetFixedVariable(ptr++, time_name, signal->fTime); 
      } else {
        double sigTime = signal->fTime;
        fMinimizer.SetLimitedVariable(ptr++, time_name, 
                                      sigTime,
                                      range.fTimeUnc, 
                                      range.fTimeMin, 
                                      range.fTimeMax); 
      }
      j++;
      iter++;
    }
       
  }

}

//______________________________________________________________________________
void EXOSignalFitterChiSquare::SaveResults()
{
  // Save the results back in the EXOChannelSignals.  This function takes the
  // final results and saves back into channel signals.  These results are
  // available by using GetFitChannelSignalsAt.

  size_t ptr = 1;
  const double* vars = fMinimizer.X();
  const double* errors = fMinimizer.Errors();

  for (size_t i=0;i<fAllSignals.size();i++) {
    ChiSquareSignals& sigs = fAllSignals[i];

    assert(sigs.fSigs.GetNumSignals() == sigs.fParameterSettings.size());
    EXOChannelSignals newChannelSignals = sigs.fSigs;
    newChannelSignals.Clear();
    for (size_t j=0;j<sigs.fSigs.GetNumSignals();j++) {
      // Fill the results from the fit  
      assert(ptr < fMinimizer.NDim());
      EXOSignal signal; 
      signal.fMagnitude      = vars[ptr];
      signal.fMagnitudeError = errors[ptr++];
      signal.fTime           = vars[ptr];
      signal.fTimeError      = errors[ptr++];
      newChannelSignals.AddSignal(signal);
    }
    ptr++;
    sigs.fSigs = newChannelSignals;
           
  }

}

//______________________________________________________________________________
void EXOSignalFitterChiSquare::AddSignalsWithFitModel(
  const EXOChannelSignals& sigs,
  const EXOSignalModel& model,
  const EXOFitRanges& ranges,
  const std::vector<std::pair<size_t, size_t> >& includeInFit,
  double noise,
  double baseline)
{
  // Add signals from EXOChannelSignals to be fit.  EXOSignalModel contains
  // models for this particular channel, baseline contains the locked baseline
  // value (FixME?).  ranges is a class which tells us how to calculate the
  // range and uncertainty for the time and energy signals.  See documentation
  // for the EXOFitRanges class for more details.  This class depends that the
  // signal model lives at least through a call to Minimize() because it
  // retains a pointer to the signal model to avoid copying.

  if (sigs.GetWaveform() == 0) {
    LogEXOMsg("Signal has NULL waveform", EEError);
    return;
  }
  ChiSquareSignals chi = { 
    sigs,   // Copy the channel signals 
    &model, // Save a pointer to the channel signal model 
    EXODoubleWaveform(*sigs.GetWaveform()),
    includeInFit, // Ranges to include in evaluations.
    ParamVec(), // Initialize the parameter vector 
    noise      // Noise of signal
  };

  // Subtract baseline from fFitWaveform.
  // (Faster to do it once now than every time we evaluate chi-square.)
  chi.fFitWaveform -= baseline;

  sigs.ResetIterator();
  const EXOSignal* signal;
  while ((signal = sigs.Next()) != NULL) {
    chi.fParameterSettings.push_back(
      ranges.GetRange(signal->fMagnitude, 
                     signal->fTime));
  }

  fAllSignals.push_back(chi); 
  
}

//______________________________________________________________________________
double EXOSignalFitterChiSquare::CalculateChiSquare(const double *x) const
{
  // Function that actually calls through to calculate the chi-square of
  // signals.  The value is of the chi-square is returned.  The input pointer x
  // is so formatted: 
  //
  //  x[0] : number of signals for first wf, n
  //  x[1] : energy of first signal  
  //  x[2] : time of first signal
  //  ...
  //  x[2*n-1] : energy of nth signal
  //  x[2*n]   : time of nth signal
  //    And now on to the next signal
  //  x[2n+1]  : number of signals for second wf, m
  //  x[2n+2]  : etc...

  //calculate chisquare

  double f = 0;
  const double *par = x;
  for (size_t j=0;j<fAllSignals.size();j++) {
    const ChiSquareSignals& sig = fAllSignals[j];
    const EXOSignalModel* model = sig.fModel;

    double chanval = 0.0;
    double period = sig.fFitWaveform.GetSamplingPeriod();

    for(size_t k = 0; k < sig.fIncludeRanges.size(); k++) {
      const size_t start = sig.fIncludeRanges[k].first;
      const size_t end = sig.fIncludeRanges[k].second;
      double time = sig.fFitWaveform.GetTimeAtIndex(start) + period/2;

      fTmp.assign(end - start + 1, 0);
      const Int_t nstep = (Int_t)par[0];
      for(Int_t isig = 0; isig < nstep; isig++) {
        // For each signal, add it to tmp to build a model for this range.
        model->AddSignalToArray(fTmp.begin(), fTmp.end(),
                                time, period,
                                par[1 + 2*isig], par[2 + 2*isig]);
      }

      // Loop through the range and increment chi-square appropriately.
      const double* fitwf_start = &sig.fFitWaveform[start];
      for(size_t index = 0; index < fTmp.size(); index++) {
        double delta = fitwf_start[index] - fTmp[index];
        chanval += delta*delta;
      }
    }

    par += 1 + 2*sig.fSigs.GetNumSignals();
    f += chanval/(sig.fNoise*sig.fNoise);
  }

  return f;

}

//______________________________________________________________________________
vector<EXOSignalsPlot> EXOSignalFitterChiSquare::GetPlotOfResults() const
{
  vector<EXOSignalsPlot> plotVec;
  for(size_t i=0; i<fAllSignals.size(); i++){
    const ChiSquareSignals& sig = fAllSignals[i];
    stringstream title;
    title << "Signal " << i << ", Channel " << sig.fModel->GetChannelOrTag(); 
    EXOSignalsPlot plot(sig.fFitWaveform,title.str());
    plot.AddSignals(sig.fSigs,*sig.fModel);
    plotVec.push_back(plot);
  }
  return plotVec;
}

//______________________________________________________________________________
void EXOSignalFitterChiSquare::GetHistPlotOfResults(TList& returnVec) const
{
  // fills a TList with RooPlots of the current results.  The caller *owns* the
  // returned pointers and is therefore responsible to delete them.  The
  // returnVec is set to be the owner so the objects will be deleted when the
  // TList object is deleted/deconstructed. 
#ifndef HAVE_ROOT_ROOFIT
  LogEXOMsg("Unable to return plot of results, ROOT not compiled with RooFit", EEError);
  return;
#else

  returnVec.SetOwner();
  // We don't want to associate these objects with what directory might be
  // open.
  TDirectory* tmpDirectory = gDirectory;
  gROOT->cd();
  for (size_t i=0;i<fAllSignals.size();i++) {
    const ChiSquareSignals& sig = fAllSignals[i];

    TH1D* newHist = new TH1D(Form("Signal_%i", (int)i), 
                             Form("Channel %i, Signal_%i", sig.fModel->GetChannelOrTag(), (int)i),
                             sig.fFitWaveform.GetLength(),
                             sig.fFitWaveform.GetMinTime(), sig.fFitWaveform.GetMaxTime());
    newHist->SetFillColor(0);
    // Copy the data into the histogram 
    std::copy(sig.fFitWaveform.GetVectorData().begin(),
              sig.fFitWaveform.GetVectorData().end(),
              newHist->GetArray() + 1);
    newHist->SetFillColor(0);

    // Copy the function from the signal model
    TF1* newFunction = new TF1(sig.fModel->GetFunction(sig.fFitWaveform.GetMinTime(),
                               sig.fFitWaveform.GetMaxTime(), 
                               sig.fSigs.GetNumSignals()));
    newFunction->SetTitle(Form("Channel %i, Fit_Signal_%i", sig.fModel->GetChannelOrTag(), (int)i));
    newFunction->SetNpx(newHist->GetNbinsX());

    // Add the current signals to the function.
    size_t j = 0;
    const EXOSignal* signal = NULL;
    sig.fSigs.ResetIterator();
    while ((signal = sig.fSigs.Next()) != NULL) {
      newFunction->SetParameter(2*j+1, signal->fMagnitude); 
      newFunction->SetParameter(2*j+2, signal->fTime); 
      j++;
    }

    RooPlot* returnPlot = new RooPlot(newHist->GetBinLowEdge(1),
           newHist->GetBinLowEdge(newHist->GetNbinsX()));
           returnPlot->SetName("signal_fit");
    newFunction->SetLineColor(2);
    newHist->SetLineColor(1);
    returnPlot->addTH1(newHist);
    returnPlot->addObject(newFunction, "same");
    returnPlot->SetTitle(Form("Channel: %i", sig.fModel->GetChannelOrTag()));
    returnPlot->SetXTitle("Time [ns]");
    returnVec.Add(returnPlot);
  }
  tmpDirectory->cd();
#endif
  
}

