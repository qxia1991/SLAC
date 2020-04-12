//______________________________________________________
// Check low-level features of the data to make sure everything looks OK.
// We define low-level to be anything we can check before running reconstruction.

#include "EXOAnalysisManager/EXOVerifyDAQ.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <sstream>
#include <numeric>
#include <vector>

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOVerifyDAQ, "verify-daq")

EXOVerifyDAQ::EXOVerifyDAQ()
{}

EXOAnalysisModule::EventStatus EXOVerifyDAQ::BeginOfRun(EXOEventData*)
{
  // Reset variables, in case multiple runs are being processed.
  fChannelNumSamples.clear();
  fChannelSum.clear();
  fChannelSumSquare.clear();
  return kOk;
}

EXOAnalysisModule::EventStatus EXOVerifyDAQ::ProcessEvent(EXOEventData* ED)
{
  // A list of event-by-event checks.
  // Also collect data for the whole-run checks later.

  // Are all of the waveforms we expect actually present?
  const EXOChannelMap& chanMap = GetChanMapForHeader(ED->fEventHeader);
  for(int i = 0; i < NUM_DAQCARDS; i++) {
    for(int j = 0; j < CHANNELS_PER_DAQCARD; j++) {
      int channel = chanMap.get_physical_channel(i,j);
      if(channel < 0 or chanMap.channel_suppressed_by_daq(channel)) continue; // We don't expect this waveform.
      if(ED->GetWaveformData()->GetWaveformWithChannel(channel) == NULL) {
        std::ostringstream stream;
        stream << "Missing waveforms associated with software channel " << channel <<
                  " in run " << ED->fRunNumber << " event " << ED->fEventNumber;
        LogEXOMsg(stream.str(), EEAlert);
      }
    }
  }

  // Are there any unexpected waveforms?
  for(size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++) {
    int software_channel = ED->GetWaveformData()->GetWaveform(i)->fChannel;
    int card, channel;
    chanMap.get_card_and_channel(software_channel, card, channel);
    if(card < 0 or channel < 0) {
      std::ostringstream stream;
      stream << "We have an unexpected waveform with software channel " << ED->GetWaveformData()->GetWaveform(i)->fChannel <<
                " in run " << ED->fRunNumber << " event " << ED->fEventNumber;
      LogEXOMsg(stream.str(), EEAlert);
    }
    if(chanMap.channel_suppressed_by_daq(software_channel)) {
      std::ostringstream stream;
      stream << "A waveform for software channel " << software_channel <<
                " was received; this channel was expected to have been suppressed by the DAQ.";
      LogEXOMsg(stream.str(), EEAlert);
    }
  }

  // Track the cumulative noise on waveforms, to check in EndOfRun.
  for(size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++) {
    Int_t channel = ED->GetWaveformData()->GetWaveform(i)->fChannel;
    const std::vector<Int_t>& wfVector = ED->GetWaveformData()->GetWaveform(i)->GetVectorData();
    fChannelNumSamples[channel] += wfVector.size();
    fChannelSum[channel] += std::accumulate(wfVector.begin(), wfVector.end(), Int_t(0));
    fChannelSumSquare[channel] += std::inner_product(wfVector.begin(), wfVector.end(),
                                                     wfVector.begin(), Long64_t(0));
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOVerifyDAQ::EndOfRun(EXOEventData* ED)
{
  // Some statistics got collected throughout the run; check them here.
  // Logged warnings will still be captured and saved into the output root file.
  // (Of course, logged warnings are printed to screen until the very end, when things are getting destructed.)

  // Apply limits to the cumulative noise on waveforms.
  std::map<Int_t, size_t>::iterator ChannelNumSamples_it = fChannelNumSamples.begin();
  std::map<Int_t, Long64_t>::iterator ChannelSum_it = fChannelSum.begin();
  std::map<Int_t, Long64_t>::iterator ChannelSumSquare_it = fChannelSumSquare.begin();
  while(ChannelNumSamples_it != fChannelNumSamples.end()) {
    // Careful to keep numbers as manageable as possible.  (The integers can get quite large.)
    double StdDev = double(ChannelSum_it->second) / ChannelNumSamples_it->second;
    StdDev *= ChannelSum_it->second;
    StdDev = ChannelSumSquare_it->second - StdDev;
    StdDev /= ChannelNumSamples_it->second;
    StdDev = std::sqrt(StdDev);
    // StdDev is the standard deviation of all samples from this channel.
    /* It is for the data quality group, not me, to select acceptable ranges.
    if(StdDev < -- minimum acceptable standard deviation --) {
      std::ostringstream stream;
      stream << "Too quiet:  StdDev on channel " << ChannelNumSamples_it->first << " is only " << StdDev
             << " for run " << ED->fRunNumber << ", which is anomalously low.";
      LogEXOMsg(stream.str(), EEAlert);
    }
    if(StdDev > -- maximum acceptable standard deviation --) {
      std::ostringstream stream;
      stream << "Too noisy:  StdDev on channel " << ChannelNumSamples_it->first << " is " << StdDev
             << " for run " << ED->fRunNumber << ", which is too high.";
      LogEXOMsg(stream.str(), EEAlert);
    }
    */
    ChannelNumSamples_it++;
    ChannelSum_it++;
    ChannelSumSquare_it++;
  } // end loop over channels.

  return kOk;
}
