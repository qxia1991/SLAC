//______________________________________________________________________________
#include "EXOReconstruction/EXOMultipleSignalFinder.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOReconstruction/EXOSignalsPlot.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTrapezoidalFilter.hh"
#include "EXOUtilities/EXOMatchedFilter.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TH1D.h"
#include <algorithm>
#include <set>
#include <iostream>

using namespace std;

EXOMultipleSignalFinder::EXOMultipleSignalFinder()
: fFilterTimeConstant(2.0*CLHEP::microsecond),
  fFilterFlatTime(0.0*CLHEP::microsecond),
  fSigmaThreshold(5.),
  fTriggerSample((size_t)TRIGGER_SAMPLE),
  fUseMatchedTriangleFilter(false)
{

}

//______________________________________________________________________________
void EXOMultipleSignalFinder::FindSignals(EXOChannelSignals& returnChannelSignals,
                                    const EXOChannelSignals& inputChannelSignals,
                                    const EXODoubleWaveform& Waveform,
                                    size_t Begin,
                                    size_t End,
                                    double aBaseline) const
{
  // Given a (baseline-subtracted) waveform and a [Begin, End) range, reshape and find more signals; add them to Signals.

  // Test a couple of things first, though.  If the tests fail, there is a usage error.
  if(Begin > End or End > Waveform.GetLength()) LogEXOMsg("Begin and End indices are bad", EEAlert);

  // Filter the snippet of the waveform
  EXODoubleWaveform filtered_signal;
  filtered_signal.MakeSimilarTo(Waveform);
  filtered_signal.SetData(&Waveform[Begin], End - Begin);
  filtered_signal.SetTOffset(Waveform.GetTimeAtIndex(Begin));
  Filter(filtered_signal, inputChannelSignals.GetChannel());

  // Now look for signals in the filtered waveform.
  double threshold = fSigmaThreshold * CalculateNoiseCounts(filtered_signal);
  for(size_t i = 1; i < filtered_signal.GetLength() - 2; i++) {
    while(i+2 < filtered_signal.GetLength() and filtered_signal[i+1] > filtered_signal[i]) i++;

    // now filtered_signal[i] is a local maximum.
    if(filtered_signal[i] > threshold) {
      // Keep it OR show that this signal is already found.
      // Note that it could be already-found in the returnChannelSignals too, since that's not necessarily empty.
      if(inputChannelSignals.GetNumSignals() > 0 and
        inputChannelSignals.GetTimeToSignal(filtered_signal.GetTimeAtIndex(i)) <=
          0.5*filtered_signal.GetSamplingPeriod()) {
        // A signal within 0.5us had already been found, so don't add this one.
      }
      else if(returnChannelSignals.GetNumSignals() > 0 and
        returnChannelSignals.GetTimeToSignal(filtered_signal.GetTimeAtIndex(i)) <=
          0.5*filtered_signal.GetSamplingPeriod()) {
        // The multiple-signal finder has already added this one (or close enough).
      }
      else {
        // This is a signal we didn't know about; add it.
        EXOSignal signal;
        signal.fTime = filtered_signal.GetTimeAtIndex(i);
        signal.fTimeError = 0.0;
        signal.fMagnitudeError = 0.0;
        // Signals tend to peak a little after the found time; we're leaning on the fitter to make this precise.
        signal.fMagnitude = Waveform.InterpolateAtPoint(signal.fTime + 5.0*CLHEP::microsecond) + aBaseline;
        returnChannelSignals.AddSignal(signal);
      }
    }

    // Now skip ahead to a local minimum
    while(i+1 < filtered_signal.GetLength() and filtered_signal[i] > filtered_signal[i+1]) i++;
  } // End looking for peaks in the filtered waveform.
  if(fVerbose.ShouldPrintTextForChannel(returnChannelSignals.GetChannel())){
    cout << "*********************************************" << endl;
    cout << "Multiple signal finder found " << returnChannelSignals.GetNumSignals() << " additional signals on channel " << returnChannelSignals.GetChannel() << ":" << endl;
    returnChannelSignals.ResetIterator();
    const EXOSignal* sig = returnChannelSignals.Next();
    while(sig){
      cout << "Signal at time " << sig->fTime << " with magnitude " << sig->fMagnitude << endl;
      sig = returnChannelSignals.Next();
    }
    cout << "*********************************************" << endl << endl;
  }
  if(fVerbose.ShouldPlotForChannel(returnChannelSignals.GetChannel())){
    stringstream title;
    title << "Filtered Waveform channel " << returnChannelSignals.GetChannel();
    EXOSignalsPlot plot(filtered_signal,title.str());
    plot.AddThreshold(threshold);
    plot.AddSignals(inputChannelSignals,kRed);
    plot.AddSignals(returnChannelSignals);
    if(fVerbose.ShouldPlotToScreenForChannel(returnChannelSignals.GetChannel())){
      EXOMiscUtil::DisplayInProgram(*plot.GetHist(),title.str());
    }
  }
}

//______________________________________________________________________________
EXOSignalCollection EXOMultipleSignalFinder::FindSignals(
    const EXOReconProcessList &processList,
    const EXOSignalCollection& inputSignals) const
{
  // Break the waveform into 256-sample slices that cover the already-found signals (with a 10-sample margin).
  // Then unshape and reshape these slices with a sharper differentiation time constant.
  // This lets us resolve close-together signals better.
  // We only look at u-wire channels

  // Collection that we will return.  (Note that it will get appended to the inputSignals.)
  EXOSignalCollection returnCollection;

  // Number of samples to reshape
  const size_t pulseLength = 256;

  inputSignals.ResetIterator();
  //We look for additional u-wire signals based on previously found u-wire signals
  const EXOChannelSignals* channelSignals = NULL;
  while ( (channelSignals = inputSignals.NextOfType(EXOReconUtil::kUWire)) != NULL) {

    //Really only search on true u-wire channels. Skip everything else.
    if( not EXOMiscUtil::ChannelIsUWire(EXOMiscUtil::TypeOfChannel(channelSignals->GetChannel())) ){
      continue;
    }

    // Pull the waveform and convert to an EXODoubleWaveform.
    EXODoubleWaveform dwf = *channelSignals->GetWaveform();

    if(dwf.GetLength() < pulseLength) {
      LogEXOMsg("Could not apply MultipleSignalFinder to a signal; waveform length was too short", EEWarning);
      continue;
    }

    // Subtract baseline from waveform
    double baseline = fBaselineCalculator.Extract(*channelSignals->GetWaveform());
    dwf -= baseline;

    // Let's make a collection of the indices we need to cover.
    std::set<size_t> IndicesOfSignals;
    const EXOSignal* signal = NULL;
    channelSignals->ResetIterator();
    while( (signal = channelSignals->Next()) != NULL) {
      size_t ThisIndex = dwf.GetIndexAtTime(signal->fTime);
      if(ThisIndex != dwf.GetLength()) IndicesOfSignals.insert(ThisIndex);
      else LogEXOMsg("A found signal lies outside of the trace; skipping", EEWarning);
    }

    // Construct a new EXOChannelSignals to hold added signals.
    EXOChannelSignals returnChannelSignals;
    returnChannelSignals.MakeSimilarTo(*channelSignals);

    // Rules:  Every signal must be covered with a 10-sample margin.
    // Figure of Merit:
    //   We always prefer fewer reshapings to more.
    //   We try to center the signals with equal margin on either side.
    // (This could probably be more intelligent, by doing a regrouping to achieve better total margins.  Oh well.)
    const size_t MinMargin = 10;

    std::set<size_t>::iterator SignalIndex = IndicesOfSignals.begin();
    while(SignalIndex != IndicesOfSignals.end()) {
      size_t Begin = std::max(*SignalIndex, MinMargin);
      Begin -= MinMargin; // Note we can't do this all in one step -- size_t is unsigned.
      size_t End = Begin + pulseLength;

      std::set<size_t>::iterator LastSignalIndex; // will point to the position of the last covered signal.
      if(End >= dwf.GetLength()) {
        // Note that this won't push Begin into negative values; we've checked that the waveform is long enough.
        Begin = dwf.GetLength() - pulseLength;
        End = dwf.GetLength();
        LastSignalIndex = IndicesOfSignals.end(); // We need to keep covering up through the end, regardless of margin.
        LastSignalIndex--;
      }
      else {
        // So we're not at the end -- ensure we keep covering signals through to the margin boundary.
        LastSignalIndex = IndicesOfSignals.upper_bound(End - MinMargin);
        LastSignalIndex--;
      }

      // OK, so we are able to cover signals SignalIndex through (and including) LastSignalIndex.
      // Re-select Begin and end to center them.
      size_t AvgIndex = (*SignalIndex + *LastSignalIndex)/2;
      Begin = std::max(AvgIndex, pulseLength/2);
      Begin -= pulseLength/2; // Note we can't do this all in one step -- size_t is unsigned.
      End = Begin + pulseLength;
      if(End > dwf.GetLength()) {
        Begin = dwf.GetLength() - pulseLength;
        End = dwf.GetLength();
      }

      // Check that we still cover SignalIndex and LastSignalIndex with an appropriate margin.
      // If not, there's an error in my logic.
      if(*SignalIndex < Begin or *LastSignalIndex >= End) {
        LogEXOMsg("Somehow we ended up with a signal that's not within the reshaping region", EEAlert);
      }

      // OK, do it.
      FindSignals(returnChannelSignals, *channelSignals, dwf, Begin, End, baseline);

      // Now skip the other signals that got included in this one reshaping, and continue.
      SignalIndex = LastSignalIndex;
      SignalIndex++;
    } // End loop over signals in this EXOChannelSignals.

    // Only add signals from this channel if there were some.
    if(returnChannelSignals.GetNumSignals() != 0) returnCollection.AddChannelSignal(returnChannelSignals);
  } // End loop over channelSignals in inputSignals

  return returnCollection;
}

//_________________________________________________________________________
void EXOMultipleSignalFinder::GetShapedModel(EXODoubleWaveform &fShapedModel, double fFilterTimeConstant) const
{

  const int wireoffset = 50;
  const int nsample = 256;
  const size_t init_length = 100000;
  EXODoubleWaveform unshaped_model_initializer;

  double initializer[init_length];

  unshaped_model_initializer.SetLength(init_length);
  unshaped_model_initializer.SetSamplingPeriod(0.1);

  EXOTrapezoidalFilter fTrapFilter;
  fTrapFilter.SetRampTime(fFilterTimeConstant);
  fTrapFilter.SetFlatTime(fFilterFlatTime);

  fShapedModel.SetSamplingPeriod(1.);

  double totalshapingtimes = fFilterTimeConstant;
  double shaped_model_tmax = TMath::Min(double(nsample-wireoffset-2), 4*totalshapingtimes);
  fShapedModel.SetTOffset(-double(wireoffset));
  fShapedModel.SetLength((size_t) ((shaped_model_tmax-fShapedModel.GetMinTime())/fShapedModel.GetSamplingPeriod()+2));
  for(size_t i=0; i<669; i++) {
    unshaped_model_initializer[i] = 0.;
  }
  for(size_t i=669; i<init_length; i++) {
    unshaped_model_initializer[i]=1.;
  }

  fTrapFilter.Transform(&unshaped_model_initializer);

  size_t shaped_model_length = fShapedModel.GetLength();
  double tmin = fShapedModel.GetMinTime();
  double per = fShapedModel.GetSamplingPeriod();
  for(size_t i = 0; i<shaped_model_length; i++) {
    fShapedModel[i] = unshaped_model_initializer.InterpolateAtPoint(tmin + i*per + 66.9);
  }
}

double EXOMultipleSignalFinder::CalculateNoiseCounts(const EXODoubleWaveform& wf) const
{
  // Calculate the noise counts for a filtered waveform.  These noise counts
  // are used to dynamically calculate a threshold for each waveform.

  // We apply a threshold to the real part of the matched filter.
  double noise_counts = 0.0;
  double noise_counts_temp = 0.0;
  int ignore = 0;

  size_t nsample = wf.GetLength();
  for(size_t i=0; i<nsample; i++) { // For wires, I don't get the first few
                                // elements of the filter (due to the
                               	// finite rise time of wires); so, the
                                // first elements are sometimes
                               	// identically zero.
    // use Mean Absolute Deviation (MAD) instead of RMS to get the noise since
    // it is less sensitive to outliers (signal in this case). We will multiply
    // by 1.25 = sqrt(pi/2) since that is ratio of RMS/MAD for a normal
    // distribution.
    noise_counts += fabs(wf[i]);
  }
  noise_counts = 1.25*noise_counts/(nsample - 0.);

  // Do another pass cutting out any points greater than 3 sigma to further correct for any signals.
  noise_counts_temp = 0.0;
  ignore = 0;
  for(size_t i=100; i<nsample; i++) {
    if ((fabs(wf[i])/noise_counts) < 3.0) noise_counts_temp += fabs(wf[i]);
    else ignore++;
  }
  noise_counts = 1.25*noise_counts_temp/(nsample - ignore - 0.);
  return noise_counts;
}

void EXOMultipleSignalFinder::Filter (EXODoubleWaveform& filtered_signal, int Channel) const
{

  EXODoubleWaveform fShapedModel;

  GetShapedModel(fShapedModel,fFilterTimeConstant);

  const int pulseLength=256;

  const EXOSignalModel* sigmod;
  if (SigModel() == NULL or
     (sigmod = SigModel()->GetSignalModelForChannelOrTag(Channel)) == NULL) {
   LogEXOMsg(Form("Unable to find signal model for channel: %d",Channel), EEError);
   return;
  }
  const EXOTransferFunction& tf = sigmod->GetTransferFunction();

  EXOTrapezoidalFilter fTrapFilter;
  fTrapFilter.SetRampTime(fFilterTimeConstant);
  fTrapFilter.SetFlatTime(fFilterFlatTime);

  EXODoubleWaveform filtered_signal_temp;
  filtered_signal_temp.MakeSimilarTo(filtered_signal);

  // plot to screen, if in verbose mode.
  if((fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kUWire) or
     (fVerbose.ShouldPlotVWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kVWire) or
     (fVerbose.ShouldPlotAPDToScreen()  and (EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDGang or
                                             EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDSumOfGangs))) {
    TH1D hist;
    filtered_signal.LoadIntoHist(hist);
    std::ostringstream message;
    message << "This is a sub-trace of channel " << Channel << ": just the raw waveform.";
    EXOMiscUtil::DisplayInProgram(hist, message.str());
  }

  for (unsigned ii=0; ii<tf.GetNumIntegStages(); ii++) {
    // Each iteration unshapes one integrator from filtered_signal; filtered_signal_temp is used for scratch space.
    EXOMiscUtil::RC_unshaper(filtered_signal_temp.GetData(), filtered_signal.GetData(),
                             filtered_signal.GetLength(), filtered_signal.GetSamplingPeriod()/tf.GetIntegTime(ii));
    std::swap(filtered_signal.GetVectorData(), filtered_signal_temp.GetVectorData());
  }

  for (unsigned ii=0; ii<tf.GetNumDiffStages(); ii++) {
    // Each iteration unshapes one differentiator from filtered_signal; filtered_signal_temp is used for scratch space.
    EXOMiscUtil::CR_unshaper(filtered_signal_temp.GetData(), filtered_signal.GetData(),
                             filtered_signal.GetLength(), filtered_signal.GetSamplingPeriod()/tf.GetDiffTime(ii), 0.0);
    std::swap(filtered_signal.GetVectorData(), filtered_signal_temp.GetVectorData());
  }

  // plot to screen, if in verbose mode.
  if((fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kUWire) or
     (fVerbose.ShouldPlotVWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kVWire) or
     (fVerbose.ShouldPlotAPDToScreen()  and (EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDGang or
                                             EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDSumOfGangs))) {
    TH1D hist;
    filtered_signal.LoadIntoHist(hist);
    std::ostringstream message;
    message << "This is a sub-trace of channel " << Channel << ": unshaped in MultipleSignalFinder, not yet reshaped.";
    EXOMiscUtil::DisplayInProgram(hist, message.str());
  }

  fTrapFilter.Transform(&filtered_signal);

  // plot to screen, if in verbose mode.
  if((fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kUWire) or
     (fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kVWire) or
     (fVerbose.ShouldPlotAPDToScreen()  and (EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDGang or
                                             EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDSumOfGangs))) {
    TH1D hist;
    filtered_signal.LoadIntoHist(hist);
    std::ostringstream message;
    message << "This is a sub-trace of channel " << Channel << ": reshaped with triangular filter.";
    EXOMiscUtil::DisplayInProgram(hist, message.str());
  }

  if (fUseMatchedTriangleFilter) {
    EXOMatchedFilter Filter;
    Filter.SetTemplateToMatch(fShapedModel,pulseLength,0);
    Filter.SetNoisePowerSqrMag(EXOMiscUtil::noise_sq_mag_wire());
    Filter.Transform(&filtered_signal);

    // plot to screen, if in verbose mode.
    if((fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kUWire) or
       (fVerbose.ShouldPlotUWireToScreen() and EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kVWire) or
       (fVerbose.ShouldPlotAPDToScreen()  and (EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDGang or
                                               EXOMiscUtil::TypeOfChannel(Channel) == EXOMiscUtil::kAPDSumOfGangs))) {
      TH1D hist;
      filtered_signal.LoadIntoHist(hist);
      std::ostringstream message;
      message << "This is a sub-trace of channel " << Channel << ": matched filter output from reshaped waveform.";
      EXOMiscUtil::DisplayInProgram(hist, message.str());
    }
  }
}

void EXOMultipleSignalFinder::SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo)
{
  // Set up talk to commands for the multiple signal finder
  talkTo->CreateCommand(prefix + "/useMatchedTriangleFilter",
                        "Set whether to use triangular matched filter",
                        this,
                        fUseMatchedTriangleFilter,
                        &EXOMultipleSignalFinder::SetUseMatchedTriangleFilter );

  talkTo->CreateCommand(prefix + "/setFilterTimeConstant",
                        "Set triangular filter time constant in ns",
                        this,
                        fFilterTimeConstant,
                        &EXOMultipleSignalFinder::SetFilterTimeConstant );

  talkTo->CreateCommand(prefix + "/setFilterFlatTime",
                        "Set triangular filter flat time in ns",
                        this,
                        fFilterFlatTime,
                        &EXOMultipleSignalFinder::SetFilterFlatTime );

  talkTo->CreateCommand(prefix + "/setSigmaThreshold",
                        "Set triangular threshold in sigma above the noise",
                        this,
                        fSigmaThreshold,
                        &EXOMultipleSignalFinder::SetSigmaThreshold );

}
