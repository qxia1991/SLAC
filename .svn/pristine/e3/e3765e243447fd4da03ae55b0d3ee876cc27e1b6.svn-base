//______________________________________________________________________________
//
// EXOSignalFitter fits signals using the signal models.  It then returns the
// information from the fits in a EXOSignalCollection object.  For more
// information, see the documentaiton for Extract(). 
// 
//______________________________________________________________________________
#include "EXOReconstruction/EXOSignalFitter.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOSignalFitterChiSquare.hh"
#include <vector>
#include <cassert>
#include "TList.h"

using EXOMiscUtil::TypeOfChannel;
using EXOMiscUtil::ChannelIsUWire;
using EXOMiscUtil::ChannelIsVWire;
using EXOMiscUtil::ChannelIsAPD;
using EXOMiscUtil::ChannelInfoMap;
using EXOMiscUtil::ParameterMap;
using namespace std;

//______________________________________________________________________________
EXOSignalFitter::EXOSignalFitter() : 
  EXOVSignalParameterExtractor(),
  fChannelFitChiSquareCut(-1.),
  fUpperFitBoundaryWire(40*CLHEP::microsecond),
  fLowerFitBoundaryWire(40*CLHEP::microsecond),
  fUpperFitBoundaryWireRestr(40*CLHEP::microsecond),
  fLowerFitBoundaryWireRestr(40*CLHEP::microsecond),
  fUpperFitBoundaryVWire(40*CLHEP::microsecond),
  fLowerFitBoundaryVWire(40*CLHEP::microsecond),
  fUpperFitBoundaryAPD(40*CLHEP::microsecond),
  fLowerFitBoundaryAPD(40*CLHEP::microsecond),
  fTriggerSample((size_t)TRIGGER_SAMPLE),
  fAPDSumSignalsHaveBeenCollected(false)
{}

//______________________________________________________________________________
EXOSignalCollection EXOSignalFitter::Extract(
  const EXOReconProcessList& processList, 
  const EXOSignalCollection& inputSignals) const
{
  // Extract information about the found signals in inputSignals by fitting to
  // them.  There is an iterative process that is done when fitting to a
  // particular channel.  The following basic algorithm is used: 
  //
  // 1.  Fit the waveform.
  // 2.  Look for signals that are within 10 microseconds of one another and
  //     where one signal has a fit error of at least 15. These signals are
  //     combined, and the processing goes then back to 1. 
  // 3.  Look for the signal with the worst (Fit Magnitude)/(Fit Magnitude
  //     Error), remove the signal if the value of this parameter is less than
  //     6.0 (5.0) for U-Wires (APDs).  If any signals are removed, go back to
  //     step 1.
  // 4.  Look for the signal with the smallest Fit Magnitude.  If this is less
  //     than 5, remove this signal and return to step 1.
  // 5.  Finally, if the chi-square of the fit is worse than a particular
  //     threshold, remove all signals.  This cut is only made if
  //     fChannelFitChiSquareCut is > 0 which is *not* the default. 
  // 6.  The found signals are then added to the EXOSignalCollection which is
  //     returned from this function.

  //create the object that we want to return
  EXOSignalCollection outputSignals;

  ResetTimer("FitAndGetChiSquare");

  ResetCaches();

  const EXOChannelSignals* channelSigs;
  inputSignals.ResetIterator();
  while ( (channelSigs = inputSignals.Next()) != NULL) {

    // Copy the signals for this channel because we need to edit them 
    EXOChannelSignals ChannelFit = *channelSigs; 

    // Set cache information, in particular the baseline for the channel.
    double baseline = GetOrCalculateBaseline(*ChannelFit.GetWaveform());
    ChannelFit.SetCacheInformationFor("Baseline", baseline);
    double noisecounts = GetOrCalculateNoise(*ChannelFit.GetWaveform());
    ChannelFit.SetCacheInformationFor("BaselineError", noisecounts);
    //////////////////////////////////////////////////////////////////////////
    // If we are an APD Gang (and behaving as such), get the most recent signals from the fit APD sums
    // *if* they are available
    if (TypeOfChannel(ChannelFit.GetChannel()) == EXOMiscUtil::kAPDGang and
        ChannelFit.GetBehaviorType() == EXOReconUtil::kAPD) {
      ChannelFit.Clear();
      ChannelFit.Add(GetSignalsFromAPDSumFits(outputSignals));

    } 
    else{
      // We loop over the found signals and subtract the baseline.
      ChannelFit.Clear();

      const EXOSignal* sig;
      channelSigs->ResetIterator();
      while ((sig = channelSigs->Next()) != NULL) {
        EXOSignal temp = *sig;
        temp.fMagnitude -= baseline;
        ChannelFit.AddSignal(temp);
      }

    } 
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Threshold for magnitude / magnitude error ratio
    Double_t energy_thresh = 0;
    if (ChannelIsUWire(TypeOfChannel(ChannelFit.GetChannel()))) energy_thresh = 6.0;
    else if (ChannelIsVWire(TypeOfChannel(ChannelFit.GetChannel()))) energy_thresh = 3.0;
    else energy_thresh = 5.0; 
    //////////////////////////////////////////////////////////////////////////

    while(ChannelFit.GetNumSignals() > 0) {
 
      // Perform fit (returns pair of chi squares over fitting window and restricted window)
      StartTimer("FitAndGetChiSquare", false);
      std::pair<Double_t,Double_t> ChannelFitChiSquarePair = FitAndGetChiSquare(ChannelFit); 
      StopTimer("FitAndGetChiSquare");
      Double_t ChannelFitChiSquare = ChannelFitChiSquarePair.first; //Full fitting window
      Double_t ChannelFitChiSquareRestr = ChannelFitChiSquarePair.second; //Restricted window
      ChannelFit.SetCacheInformationFor("ChiSquare", ChannelFitChiSquare);
      ChannelFit.SetCacheInformationFor("ChiSquareRestr", ChannelFitChiSquareRestr);

      // If we are an APD gang (and behaving as such), we keep the results as is. 
      if (TypeOfChannel(ChannelFit.GetChannel()) == EXOMiscUtil::kAPDGang and
          ChannelFit.GetBehaviorType() == EXOReconUtil::kAPD) break;
 
      ////////////////////////////////////////////////////////////////////
      // Look for any pairs of steps with a small time difference and large
      // errors.  EXOChannelSignals are always ordered in time
      ChannelFit.ResetIterator();
      const EXOSignal* first_signal = ChannelFit.Next();
      const EXOSignal* second_signal;
      bool didMerge = false;
      while ((second_signal = ChannelFit.Next()) != NULL) {
 
        if ( (second_signal->fTime - first_signal->fTime 
                < 10.0*CLHEP::microsecond) and
             (first_signal->fMagnitudeError > 15.0 || 
              second_signal->fMagnitudeError > 15.0) ) {

          // The combine the signals
          if(fVerbose.ShouldPrintTextForChannel(ChannelFit.GetChannel())){
            cout << "Fitting stage: Combining degenerate signals on channel " << ChannelFit.GetChannel() << endl;
          }
          EXOSignal newSignal = *first_signal;

          newSignal.fMagnitude += second_signal->fMagnitude;
          newSignal.fTime = 0.5*(first_signal->fTime + second_signal->fTime);
          // Remove the old signals
          ChannelFit.RemoveSignal(*first_signal); 
          ChannelFit.RemoveSignal(*second_signal); 

          // Add the new one
          ChannelFit.AddSignal(newSignal);
          didMerge = true;
          break;
        }
        first_signal = second_signal;
      }
      // If we did merge, then go back to the beginning and refit.
      if (didMerge) continue;
      ////////////////////////////////////////////////////////////////////
 
      ////////////////////////////////////////////////////////////////////
      // Cut on Magnitude/MagnitudeError
      ChannelFit.ResetIterator();
      const EXOSignal* WorstIter = ChannelFit.Next();
      if (WorstIter == NULL) break;
      while ((second_signal = ChannelFit.Next()) != NULL) { 
        if(fabs(second_signal->fMagnitude/second_signal->fMagnitudeError) < 
           fabs(WorstIter->fMagnitude/WorstIter->fMagnitudeError)) {
          WorstIter = second_signal;
        }
      }
      if (fabs(WorstIter->fMagnitude/WorstIter->fMagnitudeError) < energy_thresh ) {
        // Remove signal and go back to the fit
        if(fVerbose.ShouldPrintTextForChannel(ChannelFit.GetChannel())){
          cout << "Fitting stage: Removing signal due to small amplitude / amplitude error ratio on channel " << ChannelFit.GetChannel() << endl;
        }
        ChannelFit.RemoveSignal( *WorstIter );
        continue;
      }
      ////////////////////////////////////////////////////////////////////
      
      ////////////////////////////////////////////////////////////////////
      // Cut on Magnitude
      // For Charge injection signals, we cut on absolute value; otherwise, we cut small or negative signals.
      ChannelFit.ResetIterator();
      WorstIter = ChannelFit.Next();
      if (WorstIter == NULL) break;
      while ((second_signal = ChannelFit.Next()) != NULL ) { 
        switch(ChannelFit.GetBehaviorType()) {
          case EXOReconUtil::kChargeInjection: {
            if(fabs(second_signal->fMagnitude) < fabs(WorstIter->fMagnitude)) WorstIter = second_signal;
            break;
          }
          default: {
            if(second_signal->fMagnitude < WorstIter->fMagnitude) WorstIter = second_signal;
            break;
          }
        }
      }
      if ( (ChannelFit.GetBehaviorType() == EXOReconUtil::kChargeInjection ?
            fabs(WorstIter->fMagnitude) :
            WorstIter->fMagnitude) <= 5 ) {
        if(fVerbose.ShouldPrintTextForChannel(ChannelFit.GetChannel())){
          cout << "Fitting stage: Removing signal due to small amplitude on channel " << ChannelFit.GetChannel() << endl;
        }
        // For charge injection runs, we keep large negative signals.
        // Go to the beginning
        ChannelFit.RemoveSignal( *WorstIter );
        continue;
      }
      ////////////////////////////////////////////////////////////////////
      
 
      // Drop the entire channel and break if chi^2 is bad (only if
      // ChannelFitChiSquareCut is positive).
      if(fChannelFitChiSquareCut > 0 and 
         ChannelFitChiSquare > fChannelFitChiSquareCut) {
        if(fVerbose.ShouldPrintTextForChannel(ChannelFit.GetChannel())){
          cout << "Fitting stage: Dropping fit due to bad chi^2 on channel " << ChannelFit.GetChannel() << endl;
        }
        ChannelFit.Clear();
        break;
      }
 
      // Nothing was culled -- so we're done.
      break;
    }
    outputSignals.AddChannelSignal(ChannelFit);
  }

  SetStatistic("TotalMinuitCalls", (double)fNumberTotalMinuitCalls);
  SetStatistic("TotalFitCycles", (double)fNumberFitCycles);
  return outputSignals;
}


//______________________________________________________________________________
std::pair<double, double> EXOSignalFitter::FitAndGetChiSquare(EXOChannelSignals& sigs) const
{
  // This function fits signals in EXOChannelSignals and returns the chi-square
  // value of the fit.  This is a translation of the old collection_signal_fit
  // function from EXOReconstruction.  The function uses
  // EXOSignalFitterChiSquare as its fit engine.  
  
  fNumberFitCycles++;
  const EXOSignalModel* model;
  if (!SigModel() or
      (model = SigModel()->GetSignalModelForChannelOrTag(sigs.GetChannel())) == NULL ) {
    LogEXOMsg("No available signal manager", EECritical);
    return std::pair<double,double> (-1.0,-1.0);
  }
  //std::cout << &model << std::endl;


  EXOSignalFitterChiSquare fitter;

  if (sigs.GetWaveform() == 0) {
    LogEXOMsg("Signal has NULL waveform", EEError);
    return std::pair<double,double> (0.0,0.0);
  }
  const EXOWaveform& sigWF = *sigs.GetWaveform();

  std::vector<std::pair<size_t, size_t> > includeInFit; // Stores inclusive ranges to include in fit.

  ////////////////////////////////////////////////////////////////////
  // Define which bins to use in the fit
  size_t len = sigWF.GetLength();
  double midPoint = sigWF.GetSamplingPeriod()/2.;

  // Choose the upper fit boundary
  double upperFit = fUpperFitBoundaryWire;
  if(ChannelIsUWire(TypeOfChannel(sigs.GetChannel()))){
    upperFit = fUpperFitBoundaryWire;
  }
  else if(ChannelIsVWire(TypeOfChannel(sigs.GetChannel()))){
    upperFit = fUpperFitBoundaryVWire;
  }
  else if(ChannelIsAPD(TypeOfChannel(sigs.GetChannel()))){
    upperFit = fUpperFitBoundaryAPD;
  }

  // Choose the lower fit boundary
  double lowerFit = fLowerFitBoundaryWire;
  if(ChannelIsUWire(TypeOfChannel(sigs.GetChannel()))){
    lowerFit = fLowerFitBoundaryWire;
  }
  else if(ChannelIsVWire(TypeOfChannel(sigs.GetChannel()))){
    lowerFit = fLowerFitBoundaryVWire;
  }
  else if(ChannelIsAPD(TypeOfChannel(sigs.GetChannel()))){
    lowerFit = fLowerFitBoundaryAPD;
  }

  //std::cout << "Fit ch is " << sigs.GetChannel() << " bounds " << lowerFit <<  "  " << upperFit << std::endl;

  // Flag the bins which should be included in the fit (bins "close" to a signal), and count them.
  sigs.ResetIterator();
  for (const EXOSignal* signal = sigs.Next(); signal != NULL; signal = sigs.Next()) {
    double LoTime = signal->fTime - lowerFit - midPoint;
    double HiTime = signal->fTime + upperFit - midPoint;

    // Get the index windows "triggered" by the signal.
    // Behavior for t outside the waveform is funny, avoid that.
    size_t LoBin = 0;
    size_t HiBin = sigWF.GetLength() - 1;
    if(LoTime >= sigWF.GetMaxTime()) continue;
    else if(LoTime > sigWF.GetMinTime()) LoBin = sigWF.GetIndexAtTime(LoTime) + 1;
    if(HiTime <= sigWF.GetMinTime()) continue;
    else if(HiTime < sigWF.GetMaxTime()) HiBin = sigWF.GetIndexAtTime(HiTime);
    assert(HiBin < sigWF.GetLength());

    if(LoBin > HiBin) continue;
    includeInFit.push_back(std::make_pair(LoBin, HiBin));
  } // end loop over the data

  size_t ninclude = ConsolidateIncludeInFitVector(includeInFit);

  ////////////////////////////////////////////////////////////////////

  std::vector<std::pair<size_t, size_t> > includeInChi2; // Stores inclusive ranges to include in chi2.

  // Choose the upper fit boundary
  double upperFitChi2 = fUpperFitBoundaryWireRestr;
  double lowerFitChi2 = fLowerFitBoundaryWireRestr;

  // Flag the bins which should be included in the fit (bins "close" to a signal), and count them.
  sigs.ResetIterator();
  for (const EXOSignal* signal = sigs.Next(); signal != NULL; signal = sigs.Next()) {
    double LoTime = signal->fTime - lowerFitChi2 - midPoint;
    double HiTime = signal->fTime + upperFitChi2 - midPoint;

    // Get the index windows "triggered" by the signal.
    // Behavior for t outside the waveform is funny, avoid that.
    size_t LoBin = 0;
    size_t HiBin = sigWF.GetLength() - 1;
    if(LoTime >= sigWF.GetMaxTime()) continue;
    else if(LoTime > sigWF.GetMinTime()) LoBin = sigWF.GetIndexAtTime(LoTime) + 1;
    if(HiTime <= sigWF.GetMinTime()) continue;
    else if(HiTime < sigWF.GetMaxTime()) HiBin = sigWF.GetIndexAtTime(HiTime);
    assert(HiBin < sigWF.GetLength());

    if(LoBin > HiBin) continue;
    includeInChi2.push_back(std::make_pair(LoBin, HiBin));
  } // end loop over the data

  size_t nincludeChi2 = ConsolidateIncludeInFitVector(includeInChi2);

  ////////////////////////////////////////////////////////////////////
  // Set up the time range
  Double_t t_min = sigWF.GetMinTime();
  Double_t t_max = sigWF.GetMaxTime();

  if(not includeInFit.empty()) {
    t_min = sigWF.GetTimeAtIndex(includeInFit.front().first) - 1;
    t_max = sigWF.GetTimeAtIndex(includeInFit.back().second) + 1;
  }
  ////////////////////////////////////////////////////////////////////

  AddSignalsToFitter(sigs, fitter, t_min, t_max, includeInFit);

  HandleVerbosity(fitter);
  fitter.Minimize();
  HandleVerbosity(fitter);

  fNumberTotalMinuitCalls += fitter.GetMinimizer().NCalls();

  // Copy the data into sigs
  sigs.Clear();
  sigs.Add(fitter.GetFitChannelSignalsAt(0));

  // return the chi-square
  double chi_square = fitter.GetMinimizer().MinValue();
  if ( ninclude > 2*sigs.GetNumSignals()) {
    // Gives normalized chi^2
    chi_square /= (ninclude - (2*sigs.GetNumSignals())); 
  }

  // now calculate the chi-square over the restricted window
  fitter.ClearSignals();
  AddSignalsToFitter(sigs, fitter, t_min, t_max, includeInChi2);
  double chi_square_restr = fitter.CalculateChiSquare(fitter.GetMinimizer().X());
  if ( nincludeChi2 > 2*sigs.GetNumSignals()) {
    // Gives normalized chi^2
    chi_square_restr /= (nincludeChi2 - (2*sigs.GetNumSignals())); 
  }

  std::pair<double,double> chi_square_pair(chi_square, chi_square_restr);
  return chi_square_pair;
}


//______________________________________________________________________________
void EXOSignalFitter::ResetCaches() const
{
  // Reset the caches which hold information about channel calculation.  This
  // is usually done at the beginning of Extract().
  fChannelInfoCache.clear();
  fAPDFoundSignals.Clear();
  fAPDSumSignalsHaveBeenCollected = false;
  fNumberFitCycles = 0;
  fNumberTotalMinuitCalls = 0;
}

//______________________________________________________________________________
double EXOSignalFitter::GetOrCalculateNoise(const EXOWaveform& wf) const
{
  // Get noise or calculate the noise AND the baseline.  If the waveform exists in
  // our cache, return the value.  If not, calculate these values. 
  
  // First check if it exists
  ChannelInfoMap::const_iterator iter = fChannelInfoCache.find(wf.fChannel);
  if (iter != fChannelInfoCache.end()){
    ParameterMap::const_iterator inner_iter = iter->second.find("Noisecounts");
    if(inner_iter != iter->second.end()){
      return inner_iter->second;
    }
  }

  // Insert and return the results.
  ParameterMap noiseAndBaseline = fBaselineCalculator.ExtractAll(wf);
  fChannelInfoCache[wf.fChannel].insert(noiseAndBaseline.begin(),noiseAndBaseline.end());
  return noiseAndBaseline["Noisecounts"];
}

//______________________________________________________________________________
double EXOSignalFitter::GetOrCalculateBaseline(const EXOWaveform& wf) const
{
  // Get baseline or calculate the noise AND the baseline.  If the waveform exists in
  // our cache, return the value.  If not, calculate these values. 
  
  // First check if it exists
  ChannelInfoMap::const_iterator iter = fChannelInfoCache.find(wf.fChannel);
  if (iter != fChannelInfoCache.end()){
    ParameterMap::const_iterator inner_iter = iter->second.find("Baseline");
    if(inner_iter != iter->second.end()){
      return inner_iter->second;
    }
  }

  // Insert and return the results.
  ParameterMap noiseAndBaseline = fBaselineCalculator.ExtractAll(wf);
  fChannelInfoCache[wf.fChannel].insert(noiseAndBaseline.begin(),noiseAndBaseline.end());
  return noiseAndBaseline["Baseline"];
}


//______________________________________________________________________________
const EXOChannelSignals& EXOSignalFitter::GetSignalsFromAPDSumFits(
  const EXOSignalCollection& collect) const
{
  // Get Signals from the APD Sum Fits.  This collects signals together that
  // were found on both APD planes, ignoring signals that within a certain time
  // of one another.  This time is defined by
  // EXOTransferFunction::GetEffectiveIntegTime(), that is the effective
  // integration time of the APD signal on the North Plane. 

  if (fAPDSumSignalsHaveBeenCollected) return fAPDFoundSignals;
  fAPDSumSignalsHaveBeenCollected = true;

  // Look for North APD signals
  EXOChannelSignals northSignals;
  const EXOChannelSignals* channelSigs = 
    collect.GetSignalsForChannel(EXOReconUtil::kAPDSumNorthOrBoth);
  if (channelSigs != NULL) northSignals.Add(*channelSigs); 

  // Look for South APD signals
  EXOChannelSignals southSignals;
  channelSigs = collect.GetSignalsForChannel(EXOReconUtil::kAPDSumSouth);
  if (channelSigs != NULL) southSignals.Add(*channelSigs); 

  if(northSignals.GetNumSignals() == 0 and southSignals.GetNumSignals() == 0) {
    // No summed-APD signals found -- so we can return with an empty signal list.
    return fAPDFoundSignals;
  }

  const EXOSignalModel* mod;
  if (!SigModel() || 
      (mod = SigModel()->GetSignalModelForChannelOrTag(EXOReconUtil::kAPDSumNorthOrBoth)) == NULL) {
    // How can we have signals, but not have a signal model?
    LogEXOMsg("Signal model not found for APD sum signal", EEAlert);
    return fAPDFoundSignals;
  }

  // Combine signals that fall within matchTime of one another.
  double matchTime = mod->GetTransferFunction().GetEffectiveIntegTime();
  northSignals.ResetIterator();
  southSignals.ResetIterator();
  const EXOSignal* next_south_signal = southSignals.Next();
  const EXOSignal* next_north_signal = northSignals.Next();

  // Loop over the signals in time
  while (next_south_signal != NULL or
         next_north_signal != NULL) {

    
    if (next_south_signal == NULL || next_north_signal == NULL) {
      // If one of these is NULL, just add the signal that isn't
      fAPDFoundSignals.AddSignal( *( (next_south_signal) ? next_south_signal : next_north_signal));
      next_north_signal = northSignals.Next();
      next_south_signal = southSignals.Next();

    } else {
      // The both are ok.  Now we simply need to check if they should be combined.
      double timeDiff = next_north_signal->fTime - next_south_signal->fTime;
      if ( fabs(timeDiff) <= matchTime ) {
        // They need to be combined 
        EXOSignal sig = *next_south_signal;
        sig.fMagnitude += next_north_signal->fMagnitude; 
        sig.fTime = ( next_north_signal->fTime*next_north_signal->fMagnitude +
                      next_south_signal->fTime*next_south_signal->fMagnitude)/
                    ( next_north_signal->fMagnitude + next_south_signal->fMagnitude );
        fAPDFoundSignals.AddSignal(sig);

        next_north_signal = northSignals.Next();
        next_south_signal = southSignals.Next();
      } else {
        if (timeDiff < 0) { // North came first
          fAPDFoundSignals.AddSignal(*next_north_signal);
          next_north_signal = northSignals.Next();
          
        } else { // south came first
          fAPDFoundSignals.AddSignal(*next_south_signal);
          next_south_signal = southSignals.Next();
        }
      }
       
    }
    
    
  }
  return fAPDFoundSignals;
}

//______________________________________________________________________________
void EXOSignalFitter::HandleVerbosity(const EXOSignalFitterChiSquare& fitter) const
{
  // Handle verbosity (e.g. plotting results of fits, etc.)
  static int SignalOutputNumber = 0;
  if (fitter.GetNumberOfFitChannels() == 0) return;
  // Assuming we don't fit across different channel types here:
  if (not fVerbose.ShouldPlotForChannel(fitter.GetFitChannelSignalsAt(0).GetChannel())) return;
 
  // OK, means we should do something.
  vector<EXOSignalsPlot> plots = fitter.GetPlotOfResults();
  if (not plots.size()) return;
  
  // OK, now we have something.
  for (size_t i=0;i<fitter.GetNumberOfFitChannels();i++) {
    const EXOChannelSignals& sig = fitter.GetFitChannelSignalsAt(i);
    Int_t channelOrTag = sig.GetChannel();
    if (fVerbose.ShouldPlotToScreenForChannel(channelOrTag)) {
      sig.Print(); 
      cout << "Chi-square: " << fitter.GetMinimizer().MinValue() << endl;
    }
    assert(fitter.GetNumberOfFitChannels() == plots.size());
    if (fVerbose.ShouldPlotToScreenForChannel(channelOrTag)) {
      EXOMiscUtil::DisplayInProgram(*plots[i].GetHist(), "Plotting fit status");
    }
    if (fVerbose.ShouldPlotToFileForChannel(channelOrTag)) {
      std::stringstream filename;
      filename << "FitResults-" << SignalOutputNumber << ".png";
      EXOMiscUtil::DumpHistToFile(*plots[i].GetHist(), filename.str(), "Writing fit plot to file");
      SignalOutputNumber++;
    }
  }
}
//______________________________________________________________________________
void EXOSignalFitter::SetUpperFitBoundWireMicroseconds(double val)
{
  fUpperFitBoundaryWire = val*CLHEP::microsecond;
}

//______________________________________________________________________________
void EXOSignalFitter::SetLowerFitBoundWireMicroseconds(double val)
{
  fLowerFitBoundaryWire = val*CLHEP::microsecond;
}
//______________________________________________________________________________
void EXOSignalFitter::SetUpperFitBoundVWireMicroseconds(double val)
{
  fUpperFitBoundaryVWire = val*CLHEP::microsecond;
}

//______________________________________________________________________________
void EXOSignalFitter::SetLowerFitBoundVWireMicroseconds(double val)
{
  fLowerFitBoundaryVWire = val*CLHEP::microsecond;
}
//______________________________________________________________________________
void EXOSignalFitter::SetUpperFitBoundWireRestrMicroseconds(double val)
{
  fUpperFitBoundaryWireRestr = val*CLHEP::microsecond;
}

//______________________________________________________________________________
void EXOSignalFitter::SetLowerFitBoundWireRestrMicroseconds(double val)
{
  fLowerFitBoundaryWireRestr = val*CLHEP::microsecond;
}
//______________________________________________________________________________
void EXOSignalFitter::SetUpperFitBoundAPDMicroseconds(double val)
{
  fUpperFitBoundaryAPD = val*CLHEP::microsecond;
}

//______________________________________________________________________________
void EXOSignalFitter::SetLowerFitBoundAPDMicroseconds(double val)
{
  fLowerFitBoundaryAPD = val*CLHEP::microsecond;
}

//______________________________________________________________________________
void EXOSignalFitter::SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo)
{
  talkTo->CreateCommand(prefix + "/UWireChiSquareCut",
                        "Drop all signals from a u-wire which yields a poor fit",
                        this,
                        fChannelFitChiSquareCut,
                        &EXOSignalFitter::SetChannelFitChiSquareCut );

  talkTo->CreateCommand(prefix + "/UpperFitBoundWire",
                        "Set the upper bound for the fit window used for fitting wire signals in microseconds.",
                        this,
                        fUpperFitBoundaryWire/CLHEP::microsecond,
                        &EXOSignalFitter::SetUpperFitBoundWireMicroseconds );

  talkTo->CreateCommand(prefix + "/LowerFitBoundWire",
                        "Set the lower bound for the fit window used for fitting wire signals in microseconds.",
                        this,
                        fLowerFitBoundaryWire/CLHEP::microsecond,
                        &EXOSignalFitter::SetLowerFitBoundWireMicroseconds );

  talkTo->CreateCommand(prefix + "/UpperFitBoundWireRestr",
                        "Set the upper bound for the restricted window used for calculating the chi^2 only (not used for the fit).  Given in microseconds.",
                        this,
                        fUpperFitBoundaryWireRestr/CLHEP::microsecond,
                        &EXOSignalFitter::SetUpperFitBoundWireRestrMicroseconds );

  talkTo->CreateCommand(prefix + "/LowerFitBoundWireRestr",
                        "Set the lower bound for the restricted window used for calculating the chi^2 only (not used for the fit).  Given in microseconds.",
                        this,
                        fLowerFitBoundaryWireRestr/CLHEP::microsecond,
                        &EXOSignalFitter::SetLowerFitBoundWireRestrMicroseconds );

  talkTo->CreateCommand(prefix + "/UpperFitBoundAPD",
                        "Set the upper bound for the fit window used for fitting apd signals in microseconds.",
                        this,
                        fUpperFitBoundaryAPD/CLHEP::microsecond,
                        &EXOSignalFitter::SetUpperFitBoundAPDMicroseconds );

  talkTo->CreateCommand(prefix + "/LowerFitBoundAPD",
                        "Set the lower bound for the fit window used for fitting apd signals in microseconds.",
                        this,
                        fLowerFitBoundaryAPD/CLHEP::microsecond,
                        &EXOSignalFitter::SetLowerFitBoundAPDMicroseconds );

}

//______________________________________________________________________________
void EXOSignalFitter::AddSignalsToFitter(const EXOChannelSignals& sigs,
                                         EXOSignalFitterChiSquare& fitter,
                                         double t_min,
                                         double t_max,
                                         const std::vector<std::pair<size_t, size_t> >& includeInFit) const
{
  // Call fitter.AddSignalsWithFitModel with the appropriate arguments.
  const EXOSignalModel* model;
  if (!SigModel() or
      (model = SigModel()->GetSignalModelForChannelOrTag(sigs.GetChannel())) == NULL ) {
    LogEXOMsg("No available signal manager", EEAlert);
  }

  StartTimer("CalculateBaselineAndNoise", false);
  double baseline = GetOrCalculateBaseline(*sigs.GetWaveform());
  double noisecounts = GetOrCalculateNoise(*sigs.GetWaveform());
  StopTimer("CalculateBaselineAndNoise");

  // The following handles setting the range behavior for different types of
  // channels..
  switch(sigs.GetBehaviorType()) {
    case EXOReconUtil::kUWire:
      StartTimer("AddUWireSignal", false);
      fitter.AddSignalsWithFitModel(sigs, *model,
        EXOWireFitRanges(t_min, t_max), includeInFit,
        noisecounts, baseline);
      StartTimer("AddUWireSignal");
      break;
    case EXOReconUtil::kUWireInd: 
      fitter.AddSignalsWithFitModel(sigs, *model, 
        EXOWireFitRanges(t_min, t_max), includeInFit, 
        noisecounts, baseline);
      break;
    case EXOReconUtil::kVWire:
      StartTimer("AddVWireSignal", false);
      fitter.AddSignalsWithFitModel(sigs, *model,
        EXOVWireFitRanges(t_min, t_max), includeInFit,
        noisecounts, baseline);
      StopTimer("AddVWireSignal");
      break;
    case EXOReconUtil::kAPD:
      StartTimer("AddAPDSignal", false);
      if(TypeOfChannel(sigs.GetChannel()) == EXOMiscUtil::kAPDGang) {
        fitter.AddSignalsWithFitModel(sigs, *model,
          EXOAPDGangFitRanges(t_min, t_max), includeInFit,
          noisecounts, baseline);
      }
      else /* kAPDSumOfGangs */ {
        fitter.AddSignalsWithFitModel(sigs, *model,
          EXOSumAPDFitRanges(t_min, t_max), includeInFit,
          noisecounts, baseline);
      }
      StopTimer("AddAPDSignal");
      break;
    case EXOReconUtil::kChargeInjection:
      fitter.AddSignalsWithFitModel(sigs, *model,
        EXOChargeInjectionFitRanges(t_min, t_max), includeInFit,
        noisecounts, baseline);
      break;
    default: break;
  }
}

size_t EXOSignalFitter::ConsolidateIncludeInFitVector(std::vector<std::pair<size_t, size_t> >& includeInFit) const
{
  // Merge overlapping fit ranges in includeInFit, and return the number of bins included.
  // The fit ranges in includeInFit are ordered upon return.

  // Sanitize.
  for(size_t i = 0; i < includeInFit.size(); i++) assert(includeInFit[i].first <= includeInFit[i].second);

  // Sort.  Makes merging more efficient.
  std::sort(includeInFit.begin(), includeInFit.end());

  // Merge overlapping intervals.  Ordering is maintained.
  for(size_t i = 0; i < includeInFit.size(); i++) {
    while(i + 1 < includeInFit.size()) {
      if(includeInFit[i].second + 1 < includeInFit[i+1].first) break; // No more overlaps for i.

      // Else, overlap between i and i+1.
      includeInFit[i].second = includeInFit[i+1].second;
      includeInFit.erase(includeInFit.begin() + i + 1);
    }
  }

  // Count how many bins are included in the fit.
  size_t ninclude = 0;
  for(size_t i = 0; i < includeInFit.size(); i++) {
    ninclude += includeInFit[i].second - includeInFit[i].first + 1;
  }
  return ninclude;
}
