//______________________________________________________________________________
//
// EXOMatchedFilterFinder finds waveforms using the matched filter.
//
// This signal finder only finds signals on signals which are defined as
// behaving like APD or U-Wire signals.  See FindSignals for more information.
//______________________________________________________________________________
#include "EXOReconstruction/EXOMatchedFilterFinder.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignalsPlot.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#ifdef USE_THREADS
#include "boost/thread/thread.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#endif

using namespace std;

//______________________________________________________________________________



EXOMatchedFilterFinder::EXOMatchedFilterFinder() 
: fUserWireThreshold(-1.0),  //Negative value means no user threshold is set
  fUserVWireThreshold(-1.0),  //Negative value means no user threshold is set
  fUserAPDThreshold(-1.0),  //Negative value means no user threshold is set
  fWireThresholdFactor(5.0),
  fVWireThresholdFactor(5.0),
  fAPDSumThresholdFactor(4.0),
  fWireSmoothWindow(0),
  fAPDSmoothWindow(0),
  fAPDSearchWindowBegin(0),
  fAPDSearchWindowEnd(0),
  fChargeClusterTime(0),
  fWireSavGolFilter(1,0,2),
  fAPDSavGolFilter(1,0,2),
  fDivideNoise(true),
  fNumThreads(24),
  fUseAPDRealNoise(false),   // Default not using the apd real noise power spectrum
  fUseWireRealNoise(false) // Default not using the wire real noise power spectrum
  
{
  
}



//______________________________________________________________________________
EXOSignalCollection EXOMatchedFilterFinder::FindSignals(
    const EXOReconProcessList &processList, 
    const EXOSignalCollection& inputSignals) const
{
  // Find signals in the process list using a matched filter, this signal
  // finder only finds signals that behave like APD, U-Wire, and charge injection signals.  It
  // ignores, however, individual APD gang channels.  The finder uses an
  // EXOMatchedFilter transformer based upon the EXOSignalModel defined for a
  // particular channel.  This transformer transforms the input waveform for a
  // given channel/tag and this function then searches the transformed waveform
  // for local maxima, saving the time position and estimated amplitude in the
  // the returned EXOSignalCollection.  Only found signals that are above a
  // threshold are kept.  The threshold is calculated either dynamically
  // using GetThresholdFactor() (gives a channel-type dependent factor)
  // multiplied by CalculateNoiseCounts() or can be set to a fixed value
  // by the user via SetFixedUserThreshold(). 
  // 
  // inputSignals is a signal collection already found by another signal finder

  // Collection that we will return
  EXOSignalCollection returnCollection;

  if (SigModel() == NULL) {
    LogEXOMsg("No signal manager available", EEError);
    return returnCollection;
  }

  std::map<Int_t, ChannelHelper> chanHelpers;

  const EXOReconProcessList::WaveformWithType *wfWithType;
  
  // Prepare the matched filters.
  StartTimer("PrepareMatchFilter");
  processList.ResetIterator();
  while((wfWithType = processList.GetNextWaveformAndType()) != NULL){
    if(SkipChannel(wfWithType)) continue;

    // Verify that no channels are duplicated.
    // Simultaneously, default-initialize channel helpers, to fill later.
    //cout << "Type " << wfWithType->fType << endl; 
    const EXOWaveform* wf = wfWithType->fWf;
    Int_t channel = wf->fChannel;
    if(chanHelpers.find(channel) != chanHelpers.end()) LogEXOMsg("Duplicate waveforms!", EEAlert);
    chanHelpers[channel];

    PrepareMatchFilter(*wf);
  }
  StopTimer("PrepareMatchFilter");

  // Convert waveforms to double and transform with the matched filter.
  // If USE_THREADS is defined, do this in parallel.
  StartTimer("ApplyMatchFilter");
  processList.ResetIterator();
#ifdef USE_THREADS
  // If we're using threads, set up a thread pool here.
  // Note: later versions of boost::thread may add a thread pool; currently I have 1.54.
  EXOMatchedFilter::fUseNewArrayInterface = true; // Only the new-array interface lets us use threads safely.
  boost::asio::io_service ioService;
  boost::thread_group thread_group;
  boost::asio::io_service::work* work = new boost::asio::io_service::work(ioService);
  for(size_t i = 0; i < fNumThreads; i++) {
    thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
  }
  while((wfWithType = processList.GetNextWaveformAndType()) != NULL){
    if(SkipChannel(wfWithType)) continue;
    Int_t channel = wfWithType->fWf->fChannel;
    ioService.post(boost::bind(&EXOMatchedFilterFinder::ApplyMatchFilter,
                               this,
                               boost::ref(chanHelpers.find(channel)->second),
                               boost::ref(*wfWithType)));
  }
  // Let ioService know that no more work is coming.
  delete work;
  // Join all threads.
  thread_group.join_all();
  EXOMatchedFilter::fUseNewArrayInterface = false;
#else
  while((wfWithType = processList.GetNextWaveformAndType()) != NULL){
    if(SkipChannel(wfWithType)) continue;
    Int_t channel = wfWithType->fWf->fChannel;
    ApplyMatchFilter(chanHelpers.find(channel)->second, *wfWithType);
  }
#endif
  StopTimer("ApplyMatchFilter");

  // Finish up.
  processList.ResetIterator();
  while((wfWithType = processList.GetNextWaveformAndType()) != NULL){
    if(SkipChannel(wfWithType)) continue;
    Int_t channel = wfWithType->fWf->fChannel;
    ChannelHelper& chanHelper = chanHelpers.find(channel)->second;

    ProcessVerbosity(chanHelper.fChannelSignals, chanHelper.fFilteredWF, chanHelper.fThreshold);

    // Only add to the return collection is we have found signals.
    if (chanHelper.fChannelSignals.GetNumSignals() != 0) {
      returnCollection.AddChannelSignal(chanHelper.fChannelSignals);
    }

  }

  return returnCollection;

}




//______________________________________________________________________________
void EXOMatchedFilterFinder::ApplyMatchFilter(
  EXOMatchedFilterFinder::ChannelHelper& chanHelper,
  const EXOReconProcessList::WaveformWithType& wfWithType) const
{
  // Apply matched filter (and smoothing where appropriate),
  // compute the matched filter threshold,
  // and locate peaks in the filtered waveform.
  // Return results via chanHelper.
  // This function is thread-safe provided channels are not duplicated.
  // We should have already checked for this condition.
  const EXOWaveform& wf = *wfWithType.fWf;
  chanHelper.fFilteredWF = wf;
  fFilters.find(wf.fChannel)->second.Transform(&chanHelper.fFilteredWF);

  if(wfWithType.fType == EXOReconUtil::kAPD and fAPDSmoothWindow > 0){
    EXODoubleWaveform temp;
    temp.MakeSimilarTo(chanHelper.fFilteredWF);
    fAPDSavGolFilter.Transform(&chanHelper.fFilteredWF,&temp);
    chanHelper.fFilteredWF = temp;
  }
  else if((wfWithType.fType == EXOReconUtil::kUWire or 
	   wfWithType.fType == EXOReconUtil::kVWire or 
	   wfWithType.fType == EXOReconUtil::kUWireInd) and fWireSmoothWindow > 0){
    EXODoubleWaveform temp;
    temp.MakeSimilarTo(chanHelper.fFilteredWF);
    fWireSavGolFilter.Transform(&chanHelper.fFilteredWF,&temp);
    chanHelper.fFilteredWF = temp;
  }

  switch(wfWithType.fType){
    case EXOReconUtil::kAPD: chanHelper.fThreshold = fUserAPDThreshold; break;
    case EXOReconUtil::kUWire: chanHelper.fThreshold = fUserWireThreshold; break;
    case EXOReconUtil::kUWireInd: chanHelper.fThreshold = fUserWireThreshold; break;
    case EXOReconUtil::kVWire: chanHelper.fThreshold = fUserVWireThreshold; break;
    default: chanHelper.fThreshold = -1;
  }
  if(chanHelper.fThreshold < 0.0){
    // Threshold on APDs was last evaluated independently by Russell and
    // Clayton around 8/1/2011.
    chanHelper.fThreshold = GetThresholdFactor(wfWithType.fType)*
                            CalculateNoiseCountsIteratively(chanHelper.fFilteredWF);
  }

  chanHelper.fChannelSignals.SetChannel(wf.fChannel);
  chanHelper.fChannelSignals.SetBehaviorType(wfWithType.fType);
  chanHelper.fChannelSignals.SetWaveform(&wf);

  size_t nsample = wf.GetLength();
  size_t i0 = 0;
  size_t i1 = wf.GetLength() - 1;

  std::vector<std::pair<EXOSignal,double> > FoundSignals;

  for(size_t i=i0+1; i<i1-1; i++) {
    if(wfWithType.fType == EXOReconUtil::kChargeInjection) {
      // We need to be able to find negative signals too.
      while(std::fabs(chanHelper.fFilteredWF[i+1]) > std::fabs(chanHelper.fFilteredWF[i]) and i+1<i1) i++;
    }
    else {
      while(chanHelper.fFilteredWF[i+1] > chanHelper.fFilteredWF[i] and i+1<i1) i++;
    }

    // now matched_filter_magnitude[i] is a local maximum.
    if(fAPDSearchWindowBegin <= 0){  // if APD search window is disabled...
      if((wfWithType.fType == EXOReconUtil::kChargeInjection ?
          std::fabs(chanHelper.fFilteredWF[i]) :
          chanHelper.fFilteredWF[i]) > chanHelper.fThreshold) {
        EXOSignal signal;
        signal.fTime = chanHelper.fFilteredWF.GetTimeAtIndex(i);
        signal.fTimeError = 0.0;
        signal.fMagnitudeError = 0.0;
  
        // Set the magnitude to the peak value of the raw waveform
        if(wfWithType.fType == EXOReconUtil::kVWire){
          //V-wire amplitude seems to be the peak to peak value
          size_t pos1 = (i-2>0) ? i-2 : 0;
          size_t pos2 = (i+7<nsample) ? i+7 : nsample-1;
          size_t posZeroTransition = (i+1<nsample) ? i+1 : nsample-1;
          signal.fMagnitude = wf[pos1] - wf[pos2] + wf[posZeroTransition];
        }
        else{
          // U-wire and APD Signal peaks roughly 5 microseconds after charge deposit;
          // this is a rough estimate.  I'm leaning on the fitter to correct the energy.
          // FIXME is this still correct?
          size_t en_est = (i+5<nsample) ? i+5 : nsample - 1;
  
          // This avoids issues of normalization from ecs =
          // matched_filter_magnitude.
          signal.fMagnitude = wf[en_est];
        }
        FoundSignals.push_back(std::make_pair(signal, chanHelper.fFilteredWF[i]));
      }

    }else{ // if APD search window is enabled...
      //cout << "New!\n";
      if(wfWithType.fType == EXOReconUtil::kAPD){
        EXOSignal signal;
        signal.fTime = chanHelper.fFilteredWF.GetTimeAtIndex(i);
        signal.fTimeError = 0.0;
        signal.fMagnitudeError = 0.0;
        size_t en_est = (i+5<nsample) ? i+5 : nsample - 1;
        signal.fMagnitude = wf[en_est];
        if((signal.fTime <= fChargeClusterTime + fAPDSearchWindowEnd) and (signal.fTime >= fChargeClusterTime - fAPDSearchWindowBegin)){
          // if the signal is within the window.
          signal.fFilteredWFPeakMagnitude = chanHelper.fFilteredWF[i]; // "max-threshold"
          signal.fFilteredWFPeakTime = chanHelper.fFilteredWF.GetTimeAtIndex(i); // "z-candidate" 
          chanHelper.fChannelSignals.SetCacheInformationFor("Threshold",chanHelper.fThreshold);
          FoundSignals.push_back(std::make_pair(signal, chanHelper.fFilteredWF[i]));
        }
      }
    }

    if(wfWithType.fType == EXOReconUtil::kChargeInjection) {
      while(i<i1-1 and std::fabs(chanHelper.fFilteredWF[i]) > std::fabs(chanHelper.fFilteredWF[i+1])) i++;
    }
    else {
      while(i<i1-1 && chanHelper.fFilteredWF[i] > chanHelper.fFilteredWF[i+1]) i++;
    }
  }

  // If charge injection, we take only signals matching the sign of the largest;
  // otherwise, take all (since all are positive).
  if(wfWithType.fType == EXOReconUtil::kChargeInjection) {
    double MaxSignal = 0.0;
    for(size_t i = 0; i < FoundSignals.size(); i++) {
      if(std::fabs(FoundSignals[i].second) > std::fabs(MaxSignal)) MaxSignal = FoundSignals[i].second;
    }
    for(size_t i = 0; i < FoundSignals.size(); i++) {
      // Keep signals where MaxSignal and FoundSignals[i].second have the same sign.
      if(MaxSignal*FoundSignals[i].second >= 0.0) {
        chanHelper.fChannelSignals.AddSignal(FoundSignals[i].first);
      }
    }
  }
  else {
    for(size_t i = 0; i < FoundSignals.size(); i++) {
      chanHelper.fChannelSignals.AddSignal(FoundSignals[i].first);
    }
  }
}

//______________________________________________________________________________
double EXOMatchedFilterFinder::GetThresholdFactor(EXOReconUtil::ESignalBehaviorType type) const
{
  // Return Threshold factor for type of channel, U-wire, or APD signal
  switch (type) {
    case EXOReconUtil::kUWire: return fWireThresholdFactor;
    case EXOReconUtil::kUWireInd: return fWireThresholdFactor;
    case EXOReconUtil::kVWire: return fVWireThresholdFactor;
    case EXOReconUtil::kAPD: return fAPDSumThresholdFactor; 
    default: return 3.75;
  }
}

//______________________________________________________________________________
double EXOMatchedFilterFinder::CalculateNoiseCounts(const EXODoubleWaveform& wf) const 
{
  // Calculate the noise counts for a filtered waveform.  These noise counts
  // are used to dynamically calculate a threshold for each waveform.

  // We apply a threshold to the real part of the matched filter.
  Double_t noise_counts = 0.0;
  Double_t noise_counts_temp = 0.0;
  Int_t ignore = 0;
  
  size_t nsample = wf.GetLength();
  for(size_t i=100; i<nsample; i++) { // For wires, I don't get the first few
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
  noise_counts = 1.25*noise_counts/(nsample - 100.0);
  
  // Do another pass cutting out any points greater than 3 sigma to further correct for any signals.
  noise_counts_temp = 0.0;
  ignore = 0;
  double threshold = 3.0*noise_counts;
  for(size_t i=100; i<nsample; i++) {
    if (fabs(wf[i]) < threshold) noise_counts_temp += fabs(wf[i]);
    else ignore++;
  }
  noise_counts = 1.25*noise_counts_temp/(nsample - ignore - 100.0);
  return noise_counts;
}

//______________________________________________________________________________
double EXOMatchedFilterFinder::CalculateNoiseCountsIteratively(const EXODoubleWaveform& wf) const 
{
  // Calculate the noise counts by iterating until the width of the distribution 
  // stabilizes, to attempt to be robust to signals in the traces
  // If this procedure fails to converge, return the calculation after a single iteration (see
  // CalculateNoiseCounts() above)

  // We apply a threshold to the real part of the matched filter.
  Double_t noise_counts = 0.0;
  Double_t noise_counts_temp = 0.0;
  Double_t curr_noise_counts = 0.0;
  Int_t ignore = 0;
  
  Bool_t keepgoing = true;
  size_t nsample = wf.GetLength();
  
  // get initial value of MAD, with all pulses still included
  for(size_t i=100; i<nsample; i++) {
    noise_counts += fabs(wf[i]);
  }
  noise_counts = 1.25*noise_counts/(nsample - 100.0);

  while( keepgoing ) {

    ignore = 0;
    noise_counts_temp = 0;

    double threshold = 3.0*noise_counts;
    for(size_t i=100; i<nsample; i++) {
      if (fabs(wf[i]) < threshold) noise_counts_temp += fabs(wf[i]);
      else ignore++;
    }

    curr_noise_counts = 1.25*noise_counts_temp/(nsample - ignore - 100.0);
    
    // Check to see if the noise has stabilized, 10% seems to be low enough
    // to ensure the effect of large pulses is removed from the MAD
    if( fabs(curr_noise_counts - noise_counts) < 0.1*noise_counts ) {
      return curr_noise_counts;
    }

    // Check for too few samples remaining
    if( nsample - ignore - 100 < 100) {
      return CalculateNoiseCounts( wf );
    }
    noise_counts = curr_noise_counts;
  }
    

  return noise_counts;
}

//______________________________________________________________________________
void EXOMatchedFilterFinder::PrepareMatchFilter(const EXOWaveform& wf) const
{
  // Apply matched filter given a particular channel. The matched filter is
  // correctly calculated if it needs to be.  The matched filter is reset in
  // NotifySignalModelHasChanged when the signal model manager detects a change
  // has been made in a signal model. 

  EXOMatchedFilter& filter = fFilters[wf.fChannel];

  if (not filter.WaveformMatchesFilterSettings(wf)) {
    const EXOSignalModel* sigmod;
    if (SigModel() == NULL or 
       (sigmod = SigModel()->GetSignalModelForChannelOrTag(wf.fChannel)) == NULL) {
     LogEXOMsg(Form("Unable to find signal model for channel: %d", wf.fChannel), EEError);
     return;
    }

    // Apply the u-wire noise power spectrum corresponding to the channel of the waveform
    if (fDivideNoise and fUseWireRealNoise and
        wf.GetLength() == 2048 and 
        (sigmod->GetBehaviorType() == EXOReconUtil::kUWire || sigmod->GetBehaviorType() == EXOReconUtil::kUWireInd) ) {

      int channel = (wf.fChannel <= EXOReconUtil::kUWireIndOffset) ? wf.fChannel * (-1) + EXOReconUtil::kUWireIndOffset : wf.fChannel;

      const EXOWaveform *noise_wf = fWireNoiseWfd->GetWaveformWithChannel(channel);
      EXODoubleWaveform *double_noise = new EXODoubleWaveform;

      double_noise->SetLength(1025);
      double_noise->ConvertFrom(*noise_wf);

      // The noise PSD in EXOWaveform is stored as integer, scaled by 10^2 to keep the significant digits
      // When we apply the noise PSD, we have to scale it by 1/10^2 to get the correct double waveform
      *double_noise /= 100.;
      *double_noise += 1.;

      filter.SetNoisePowerSqrMag(*double_noise);

      delete double_noise;

      //cout << "Set real u-wire noise PSD for channel " << channel <<  endl;
      
      // Apply the apd noise power spectrum corresponding to the channel of the waveform
    } else if (fDivideNoise and fUseAPDRealNoise and
            wf.GetLength() == 2048 and 
            sigmod->GetBehaviorType() == EXOReconUtil::kAPD) {

      int channel = wf.fChannel;
      //cout << "Channel: " << channel << std::endl;
      const EXOWaveform *noise_wf = fAPDNoiseWfd->GetWaveformWithChannel(channel);
      EXODoubleWaveform *double_noise = new EXODoubleWaveform;
      double_noise->SetLength(1025);
      double_noise->ConvertFrom(*noise_wf);

      // The noise PSD in EXOWaveform is stored as integer, scaled by 10^2 to keep the significant digits
      // When we apply the noise PSD, we have to scale it by 1/10^2 to get the correct double waveform
      *double_noise /= 100.;
      *double_noise += 1.;

      filter.SetNoisePowerSqrMag(*double_noise);
      
      delete double_noise;

      //cout << "Set real APD noise PSD for channel " << channel <<  endl;


      // Apply the original noise power spectrum if the user chooses so
    } else if (fDivideNoise and !fUseAPDRealNoise and
	       wf.GetLength() == 2048 and 
	       sigmod->GetBehaviorType() == EXOReconUtil::kAPD and 
	       EXOMiscUtil::CanWeUse_noise_sq_mag_wire(sigmod->GetTransferFunction())) {

      filter.SetNoisePowerSqrMag(EXOMiscUtil::noise_sq_mag_apd());


      // Apply v-wire noise power spectrum corresponding to the channel of the waveform
    } else if (fDivideNoise and fUseWireRealNoise and
        wf.GetLength() == 2048 and 
	       (sigmod->GetBehaviorType() == EXOReconUtil::kVWire)) {

      int channel = wf.fChannel;
      const EXOWaveform *noise_wf = fWireNoiseWfd->GetWaveformWithChannel(channel);
      EXODoubleWaveform *double_noise = new EXODoubleWaveform;
      double_noise->SetLength(1025);
      double_noise->ConvertFrom(*noise_wf);

      // The noise PSD in EXOWaveform is stored as integer, scaled by 10^2 to keep the significant digits
      // When we apply the noise PSD, we have to scale it by 1/10^2 to get the correct double waveform
      *double_noise /= 100.;
      *double_noise += 1.;

      filter.SetNoisePowerSqrMag(*double_noise);

      delete double_noise;
      
      //cout << "Set real v-wire noise PSD for channel " << channel <<  endl;

    } else {
      // Reset Noise power spectrum
      filter.SetNoisePowerSqrMag();
      
    }

    filter.SetTemplateToMatch(
                           sigmod->GetModelWaveform(), wf.GetLength(), 
                           (sigmod->GetBehaviorType() == EXOReconUtil::kAPD) ? 0 : 100); 
  }

}

//______________________________________________________________________________
void EXOMatchedFilterFinder::ProcessVerbosity(const EXOChannelSignals& sigs,
                                              const EXODoubleWaveform& filtered_signal,
                                              double threshold) const
{
  // Process verbosity to show plots, etc., during processing.

  // Process vebosity, can we make this cleaner?
  static int HistDumper = 0;
  if (sigs.GetNumSignals() == 0) return;
  if(fVerbose.ShouldPrintTextForChannel(sigs.GetChannel())){
    cout << "*********************************************" << endl;
    cout << "Matched filter finder found " << sigs.GetNumSignals() << " signals on channel " << sigs.GetChannel() << ":" << endl;
    sigs.ResetIterator();
    const EXOSignal* sig = sigs.Next();
    while(sig){
      cout << "Signal at time " << sig->fTime << " with magnitude " << sig->fMagnitude << endl;
      sig = sigs.Next();
    }
    cout << "*********************************************" << endl << endl;
  }
  if(!fVerbose.ShouldPlotForChannel(sigs.GetChannel())) return;
  
  stringstream title;
  title << " (shaped) signal channel " << sigs.GetChannel();
  EXOSignalsPlot rawPlot(*sigs.GetWaveform(),title.str());

  if(fVerbose.ShouldPlotToScreenForChannel(sigs.GetChannel())){
    EXOMiscUtil::DisplayInProgram(*rawPlot.GetHist(), "drawing raw (shaped) data");
  }
  if(fVerbose.ShouldPlotToFileForChannel(sigs.GetChannel())){
    std::stringstream filename;
    filename << "RawWaveform-" << HistDumper << ".png";
    EXOMiscUtil::DumpHistToFile(*rawPlot.GetHist(), filename.str(), "drawing raw (shaped) data to file");
  }

  title.clear();
  title.str("");
  title << "filtered data (real part) channel " << sigs.GetChannel();
  EXOSignalsPlot filterPlot(filtered_signal,title.str());
  filterPlot.AddThreshold(threshold);
  filterPlot.AddSignals(sigs);

  if(fVerbose.ShouldPlotToScreenForChannel(sigs.GetChannel())){
    EXOMiscUtil::DisplayInProgram(*filterPlot.GetHist(), "drawing real part of matched filter");
  }
  if(fVerbose.ShouldPlotToFileForChannel(sigs.GetChannel())){
    std::stringstream filename;
    filename << "MatchedFilter-" << HistDumper << ".png";
    EXOMiscUtil::DumpHistToFile(*filterPlot.GetHist(), filename.str(), "drawing real part of matched filter to file");
    HistDumper++;
  } 
  
}

//______________________________________________________________________________
void EXOMatchedFilterFinder::NotifySignalModelHasChanged(int chanOrTag, 
  const EXOSignalModel& /*mod*/)
{
  // We are notified when a signal model has changed so that we can reset the
  // matched filter for this channel. 
  
  // Be lazy, just reset the matched filter.  It will be calculated when it
  // needs to be. 
  fFilters[chanOrTag].Reset();
}

//______________________________________________________________________________
void EXOMatchedFilterFinder::SetupTalkTo(const std::string& prefix,
  EXOTalkToManager* talkTo)
{
  // Set up talk to commands for the matched filter finder
  talkTo->CreateCommand(prefix + "/UserWireThreshold",
                        "Set a fixed threshold for the wire matched filter finder. Negative value means threhsold is calculated automatically",
                        this,
                        fUserWireThreshold,
                        &EXOMatchedFilterFinder::SetFixedWireThreshold );

  talkTo->CreateCommand(prefix + "/UserVWireThreshold",
                        "Set a fixed threshold for the v-wire matched filter finder. Negative value means threhsold is calculated automatically",
                        this,
                        fUserVWireThreshold,
                        &EXOMatchedFilterFinder::SetFixedVWireThreshold );

  talkTo->CreateCommand(prefix + "/UserAPDSumThreshold",
                        "Set a fixed threshold for the apd sum matched filter finder. Negative value means threhsold is calculated automatically",
                        this,
                        fUserAPDThreshold,
                        &EXOMatchedFilterFinder::SetFixedAPDSumThreshold );

  talkTo->CreateCommand(prefix + "/WireThresholdFactor",
                        "Set number of sigmas of noise to define a threshold for U-wire waveforms",
                        this,
                        fWireThresholdFactor,
                        &EXOMatchedFilterFinder::SetWireThresholdFactor );

  talkTo->CreateCommand(prefix + "/VWireThresholdFactor",
                        "Set number of sigmas of noise to define a threshold for V-wire waveforms",
                        this,
                        fVWireThresholdFactor,
                        &EXOMatchedFilterFinder::SetVWireThresholdFactor );

  talkTo->CreateCommand(prefix + "/APDSumThresholdFactor",
                        "Set number of sigmas of noise to define a threshold for apd sum waveforms",
                        this,
                        fAPDSumThresholdFactor,
                        &EXOMatchedFilterFinder::SetAPDSumThresholdFactor );

  talkTo->CreateCommand(prefix + "/WireSmoothWindow",
                        "Set window size for smoothing the matched-filter output. Zero or negative value disables smoothing.",
                        this,
                        fWireSmoothWindow,
                        &EXOMatchedFilterFinder::SetWireSmoothWindow );

  talkTo->CreateCommand(prefix + "/APDSmoothWindow",
                        "Set window size for smoothing the matched-filter output. Zero or negative value disables smoothing.",
                        this,
                        fAPDSmoothWindow,
                        &EXOMatchedFilterFinder::SetAPDSmoothWindow );

  talkTo->CreateCommand(prefix + "/DivideNoise",
                        "Set whether noise spectrum should be divided out if possible.",
                        this,
                        fDivideNoise,
                        &EXOMatchedFilterFinder::SetDivideNoise);

  talkTo->CreateCommand(prefix + "/NumThreads",
                        "When compiled with threads, specify how many to use.",
                        this,
                        fNumThreads,
                        &EXOMatchedFilterFinder::SetNumThreads);

  talkTo->CreateCommand(prefix + "/UseWireNoisePowerSpectrum",
                        "Set whether to use wire noise power spectrum.",
                        this,
                        fUseWireRealNoise,
                        &EXOMatchedFilterFinder::SetWireNoisePowerSpectrum);

  talkTo->CreateCommand(prefix + "/UseAPDNoisePowerSpectrum",
			"Set whether to use APD noise power spectrum.",
                        this,
                        fUseAPDRealNoise,
                        &EXOMatchedFilterFinder::SetAPDNoisePowerSpectrum);

  talkTo->CreateCommand(prefix + "/WireNoiseFile",
			"Set the name of the file which contains the wire noise.",
                        this,
                        fWireNoiseFilenameParam,
                        &EXOMatchedFilterFinder::SetWireNoiseFilename);

  talkTo->CreateCommand(prefix + "/APDNoiseFile",
			"Set the name of the file which contains the APD noise.",
                        this,
                        fAPDNoiseFilenameParam,
                        &EXOMatchedFilterFinder::SetAPDNoiseFilename);

  talkTo->CreateCommand(prefix + "/APDSearchWindowBegin",
                        "Set the beginning time of the APD signal search window. Zero or negative value disables the window.",
                        this,
                        fAPDSearchWindowBegin,
                        &EXOMatchedFilterFinder::SetAPDSearchWindowBegin );

  talkTo->CreateCommand(prefix + "/APDSearchWindowEnd",
                        "Set the ending time of the APD signal search window.",
                        this,
                        fAPDSearchWindowEnd,
                        &EXOMatchedFilterFinder::SetAPDSearchWindowEnd );

}

