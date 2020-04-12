#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXOYMatchExtractor.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOCalibUtilities/EXOVWireThresholds.hh"
#include "TH1D.h"
#include "Rtypes.h"
#include <sstream>
#include <list>

using namespace std;
using EXOMiscUtil::TypeOfChannel;

EXOYMatchExtractor::EXOYMatchExtractor()
: fWireMatchTime(3.5*SAMPLE_TIME),
  fThresholdFactor(4.0),
  fThreshold(20.0),
  fThresholds(NULL)
{}

//_______________________________________________________________________________________________
vector< pair<int,double> > EXOYMatchExtractor::FakeCluster(const EXOSignalCollection& inputSignals) const
{
  std::list<SignalWithChannel> energySorted;
  inputSignals.ResetIterator();
  const EXOChannelSignals* cs = 0;
  while((cs = inputSignals.NextOfType(EXOReconUtil::kUWire)) != 0){
    cs->ResetIterator();
    const EXOSignal* sig = 0;
    while((sig = cs->Next()) != 0){
      SignalWithChannel signal;
      signal.fChannel = cs->GetChannel();
      signal.fSignal = sig;
      energySorted.push_back(signal);
    }
  }
  EnergySorter esort;
  energySorted.sort(esort);
  std::list<SignalWithChannel> channelSorted(energySorted);
  ChannelSorter chsort;
  channelSorted.sort(chsort);

  vector< pair<int,double> > returnSignals;
  while(energySorted.size() > 0){
    SignalWithChannel comparisonSignal = energySorted.front();

    double meanTime = comparisonSignal.fSignal->fMagnitude * comparisonSignal.fSignal->fTime;
    double energySum = comparisonSignal.fSignal->fMagnitude;

    std::list<SignalWithChannel>::iterator comparisonIterator = channelSorted.begin();
    while(*comparisonIterator != comparisonSignal){
      comparisonIterator++;
    }

    int previousChannel = comparisonSignal.fChannel;
    list<SignalWithChannel>::reverse_iterator riter(comparisonIterator);
    while(riter != channelSorted.rend()){
      if(riter->fChannel - previousChannel < -1){
        break;
      }
      if( fabs(comparisonSignal.fSignal->fTime - riter->fSignal->fTime) > fWireMatchTime){
        riter++;
        continue;
      }

      previousChannel = riter->fChannel;
      meanTime += riter->fSignal->fMagnitude * riter->fSignal->fTime;
      energySum += riter->fSignal->fMagnitude;

      energySorted.remove(*riter);
      channelSorted.remove(*riter);
    }
    previousChannel = comparisonSignal.fChannel;

    list<SignalWithChannel>::iterator iter(comparisonIterator);
    iter++;
    while(iter != channelSorted.end()){
      if(iter->fChannel - previousChannel > 1){
        break;
      }
      if( fabs(comparisonSignal.fSignal->fTime - iter->fSignal->fTime) > fWireMatchTime){
        iter++;
        continue;
      }

      previousChannel = iter->fChannel;
      meanTime += iter->fSignal->fMagnitude * iter->fSignal->fTime;
      energySum += iter->fSignal->fMagnitude;

      energySorted.remove(*iter);
      iter = channelSorted.erase(iter);
    }
    channelSorted.remove(comparisonSignal);
    energySorted.remove(comparisonSignal);
    
    if(energySum){
      meanTime /= energySum;
    }
    returnSignals.push_back(pair<int,double>(comparisonSignal.fChannel,meanTime));
  }

  return returnSignals;
}

//_______________________________________________________________________________________________
EXOSignalCollection EXOYMatchExtractor::Extract(const EXOReconProcessList& processList, const EXOSignalCollection& inputSignals) const
{
  //Returns a signal collection of found v-wire signals.
  //processList should contain waveforms on v-wire channels.
  //inputSignals is a signal collection already found by another signal finder and should contain found signals on U-wires.

  //The signal collection that we will return. Only add v-wire signals
  EXOSignalCollection returnCollection;

  vector< pair<int,double> > clusters = FakeCluster(inputSignals);

  for(unsigned int i=0; i<clusters.size(); i++){
    FindAndAddVSignal(clusters[i].first,clusters[i].second,processList,returnCollection);
  }
  return returnCollection;
}

//______________________________________________________________________________
void EXOYMatchExtractor::FindAndAddVSignal(int uChannel, double uTime, const EXOReconProcessList& processList, EXOSignalCollection& returnCollection) const
{
  if(!fThresholds){
    LogEXOMsg("No thresholds from the database registered",EEAlert);
  }
  EXOSignal foundSignal;
  // Try to guess what range of time we can use for a baseline, cutting out the signal.
  double LastCleanTime = uTime - 2.0*UPLANE_VPLANE_DISTANCE/DRIFT_VELOCITY;
  //Loop over all v-wire waveforms
  const EXOReconProcessList::WaveformWithType *wfWithType = NULL;
  processList.ResetIterator();
  while((wfWithType = processList.GetNextWaveformAndType()) != NULL){
    //Only look for v wire signals
    if(wfWithType->fType != EXOReconUtil::kVWire){
      continue;
    }
    const EXOWaveform* wf = wfWithType->fWf;
    //u- and v-wire signal should lie on same half of the detector
    if(!EXOMiscUtil::OnSameDetectorHalf(uChannel,wf->fChannel)){
      continue;
    }
    // GetIndexAtTime will return wf->GetLength if LastCleanTime is bad -- which, actually, is about the best we can do.
    fBaselineCalculator.SetTriggerSample(wf->GetIndexAtTime(LastCleanTime));
    double baseline = fBaselineCalculator.Extract(*wf);
    double YMatchFOM = ApplyYMatch(*wf, uTime, baseline);
    double threshold = fThreshold;
    if(threshold < 0){
      threshold = fThresholds->GetThresholdOnChannel(wf->fChannel) * fThresholdFactor;
    }
    if(YMatchFOM >= threshold){
      EXOSignal newSignal;
      newSignal.fTime = uTime;
      newSignal.fTimeError = 0.0;
      newSignal.fMagnitude = YMatchFOM; 
      newSignal.fMagnitudeError = 0.0;
      returnCollection.AddSignalOnWaveform(newSignal,wfWithType);
      returnCollection.SetCacheInformationForChannel("Baseline",baseline,wf->fChannel);
    }
  }
}

//______________________________________________________________________________
double EXOYMatchExtractor::ApplyYMatch(const EXOWaveform& wf, 
  double uTime, double baseline) const
{
  // Return a figure of merit for matching the v-wire signal template to the waveform on this channel.
  // If something bad happens (eg. no template v-wire function found), log an EEError and return 0.0.

  // Get the signal model.  If it can't be found, log an EEError and return a FOM of 0.0.
  const EXOSignalModel* sigmod;
  if (SigModel() == NULL or (sigmod = SigModel()->GetSignalModelForChannelOrTag(wf.fChannel)) == NULL) {
    std::ostringstream stream;
    stream << "Unable to find signal model for channel: " << wf.fChannel;
    LogEXOMsg(stream.str(), EEError);
    return 0.0;
  }
  const EXODoubleWaveform& modelWF = sigmod->GetModelWaveform();

  if(fYMatchNormalization.find(sigmod->GetChannelOrTag()) == fYMatchNormalization.end()) {
    // Need to initialize (or recompute) this value.
    double renormValue = 0.0;
    for(size_t i = 0; i < modelWF.GetLength(); i++) renormValue += modelWF[i]*modelWF[i];
    if(renormValue == 0.0) LogEXOMsg("V-signal template function is zero -- this is nonsense", EEAlert);
    fYMatchNormalization[sigmod->GetChannelOrTag()] = (modelWF.GetMaxValue()-modelWF.GetMinValue()) / renormValue;
  }

  double FOM = 0.0; // Figure of Merit
  double wfStart = wf.GetTimeAtIndex(0);
  double wfEnd = wf.GetTimeAtIndex(wf.GetLength());
  for (size_t ii = 0; ii < modelWF.GetLength(); ii++) {
    double interpolationTime = uTime + modelWF.GetTimeAtIndex(ii);
    if(interpolationTime < wfStart or interpolationTime >= wfEnd) continue;
    FOM += (wf.InterpolateAtPoint(interpolationTime) - baseline)*modelWF[ii];
  }
  FOM *= fYMatchNormalization[sigmod->GetChannelOrTag()];

  // Verbose output (if requested).
  if(fVerbose.ShouldPrintVWireText()) {
    std::cout<<"FOM for v-wire channel "<<sigmod->GetChannelOrTag()<<
               " (paired with u-wire at deposit time "<<uTime/CLHEP::microsecond<<" us) is "<<
               FOM<<"."<<std::endl;
  }
  if(fVerbose.ShouldPlotVWireToScreen()) {
    std::ostringstream stream;
    stream << "Channel " << sigmod->GetChannelOrTag() << " (t=0 corresponds to the u-deposit time)";
    TH1D wfHist("wfHist", stream.str().c_str(), modelWF.GetLength(),
                modelWF.GetTimeAtIndex(0)/CLHEP::microsecond,
                modelWF.GetTimeAtIndex(modelWF.GetLength())/CLHEP::microsecond);
    for(size_t i = 0; i < modelWF.GetLength(); i++) {
      double interpolationTime = uTime + modelWF.GetTimeAtIndex(i);
      if(interpolationTime < wfStart or interpolationTime >= wfEnd) continue;
      wfHist.SetBinContent(i+1, wf.InterpolateAtPoint(interpolationTime));
    }
    TH1D templateHist;
    modelWF.LoadIntoHist(templateHist);
    templateHist.Scale(FOM/(modelWF.GetMaxValue()-modelWF.GetMinValue()));
    for(size_t i = 0; i < modelWF.GetLength(); i++) templateHist.SetBinContent(i+1, templateHist.GetBinContent(i+1)+baseline);
    templateHist.SetLineColor(kRed);
    std::vector<TObject*> ObjectsToDraw;
    ObjectsToDraw.push_back(&wfHist);
    ObjectsToDraw.push_back(&templateHist);
    EXOMiscUtil::DisplayInProgram(ObjectsToDraw, "Template wf scaled to match v-wire signal (should match up if done correctly).");
  }
  // End verbose output

  return FOM;
}


//______________________________________________________________________________
void EXOYMatchExtractor::SetupTalkTo(const std::string& prefix, 
  EXOTalkToManager* talkTo)
{
  // Set up TalkTo commands for the Y Match Extractor
  talkTo->CreateCommand(prefix + "/WireMatchTime",
                        "Set the clustering time for u-wires which is used when finding v-wire signals.",
                        this,
                        fWireMatchTime,
                        &EXOYMatchExtractor::SetWireMatchTime );

  talkTo->CreateCommand(prefix + "/VWireFixedThreshold",
                        "Set a fixed threshold for v-signal matched filter. If value is negative, database values * VWireThresholdFactor is used instead.",
                        this,
                        fThreshold,
                        &EXOYMatchExtractor::SetThreshold );

  talkTo->CreateCommand(prefix + "/VWireThresholdFactor",
                        "Set the threshold factor (number of sigmas) for v-signal matched filter FOM.",
                        this,
                        fThresholdFactor,
                        &EXOYMatchExtractor::SetThresholdFactor );

}

//_____________________________________________________________________________
void EXOYMatchExtractor::NotifySignalModelHasChanged(int signalOrTag, const EXOSignalModel& /*model*/)
{
  // All we need to compute is the renormalization factor -- just clear it, it will be recomputed on next use.
  fYMatchNormalization.erase(signalOrTag);
}
