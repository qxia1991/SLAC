//______________________________________________________________________________
// This module searches for noise events and tags them. Right now it only
// searches for a few types that I noticed. Feel free to add more or ask me
// to add more if you notice other events that are clearly noise that this
// module misses.
//
// Steve Herrin
// 2011-04-13
//
// -o_o-o_o- Changelog -o_o-o_o-
// 2011-04-13  1.0  Initial version
// 2012-02-21  1.1  Added new type of noise
// 2012-10-02  1.2  Bigfix. EXOWaveform::Sum() didn't do what I thought it did.
// -o_o-o_o- -o_o-o_o-

#include <iostream>
#include "EXOAnalysisManager/EXOAlphaNoiseTagger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOAlphaNoiseTagger, "alphanoisetag" )

EXOWaveform* EXOAlphaNoiseTagger::GetAPDSumWaveform(EXOEventData *eventdata) {
  // returns a waveform that's a sum of all APD waveforms
  EXOWaveform *sum = new EXOWaveform();
  sum->SetLength(eventdata->GetWaveformData()->fNumSamples);

  const EXOChannelMap& channelMap = GetChanMapForHeader(eventdata->fEventHeader); 
  for (int i = 152; i < 226; i++) {
    if (channelMap.good_channel(i)) {
      const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(i);
      if (wf == NULL) {
	LogEXOMsg("Couldn't get the waveform for a channel.", EEDebug);
      } else {
	*sum += *wf;
      }
    }
  }
  return sum;
}

bool EXOAlphaNoiseTagger::isRealUSig(EXOEventData *eventdata){
    
    const EXOChannelMap& channelMap = GetChanMapForHeader(eventdata->fEventHeader);
    bool is_real = false;
    double real_threshold = 45;
    double max_min_ratio  = 1.3;

    for (int i = 0; i < 76; i++) {
        int num = (i <= 37) ? (i) : (i + 38);
        if (channelMap.good_channel(num)) {
            const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(num);
            if (wf == NULL) {
                LogEXOMsg("Couldn't get the waveform for a channel.", EEDebug);
            }
            if (int(wf->GetLength()) != eventdata->GetWaveformData()->fNumSamples) {
                LogEXOMsg(Form("Channel %i has length %lu.  (%i expected.)",
                            i, (unsigned long)(wf->GetLength()), eventdata->GetWaveformData()->fNumSamples), EEAlert);
                continue; // But EEAlert will shut down.
            }
            double avg = wf->Sum(0, 256) / 256;

            if ((wf->GetMaxValue() - avg) > real_threshold){
                //Channel has a large pulse on it so might be real
                //Real U-Signals have a larger Positive pulse than negative though so check that negative 
                //signal isn't too large

                if (abs((wf->GetMaxValue() - avg)/(wf->GetMinValue() - avg)) > max_min_ratio){
                    is_real = true;
                    break;
                }

            }
        }
    }

    return is_real;
}

EXOWaveform* EXOAlphaNoiseTagger::GetWireSumWaveform(EXOEventData *eventdata) {
  // returns a waveform that's a sum of all wire waveforms
  EXOWaveform *sum = new EXOWaveform();
  sum->SetLength(eventdata->GetWaveformData()->fNumSamples);

  const EXOChannelMap& channelMap = GetChanMapForHeader(eventdata->fEventHeader); 
  for (int i = 0; i < 76; i++) {
    int num = (i <= 37) ? (i) : (i + 38);
    if (channelMap.good_channel(num)) {
      const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(num);
      if (wf == NULL) {
	LogEXOMsg("Couldn't get the waveform for a channel.", EEDebug);
      } else {
	*sum += *wf;
      }
    }
  }
  return sum;
}

size_t EXOAlphaNoiseTagger::FindTimeWithIndivWireAboveThresh(EXOEventData *eventdata, double threshold) {
  // Find the first sample when an individual u-wire goes above threshold.
  // If never, return one sample past the last one (eg. for 2048-sample waveforms, return 2048).

  // Default if we never go above threshold.
  size_t TimeAboveThresh = eventdata->GetWaveformData()->fNumSamples;

  // Loop through u-wire waveforms.
  for (int i = 0; i < 76; i++) {
    int num = (i <= 37) ? (i) : (i + 38);
    const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(num);
    if (!wf) {
      LogEXOMsg(Form("Channel: %i not found but expected!", i), EEWarning);
      continue;
    }
    if (int(wf->GetLength()) != eventdata->GetWaveformData()->fNumSamples) {
      LogEXOMsg(Form("Channel %i has length %lu.  (%i expected.)",
                     i, (unsigned long)(wf->GetLength()), eventdata->GetWaveformData()->fNumSamples), EEAlert);
      continue; // But EEAlert will shut down.
    }
    double avg = wf->Sum(0, 256) / 256;

    const EXOWaveform& wfRef = *wf;
    for(size_t t = 0; t < TimeAboveThresh; t++) {
      if(wfRef[t] - avg > threshold) {
        TimeAboveThresh = t;
        break;
      }
    }
  }

  return TimeAboveThresh;
}

bool EXOAlphaNoiseTagger::ChannelMaxesOut(EXOEventData *eventdata, int num) {
  // Does the channel max out the ADC at any point in time for this event?
  const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(num);
  if (wf == NULL) {
    LogEXOMsg("Couldn't get the waveform for a channel.", EEDebug);
  } else {
    int NumSamples = eventdata->GetWaveformData()->fNumSamples;
    // If the signal is equal to ADC_BITS-1, the channel is maxing out
    for (int i = 0; i < NumSamples; i++) {
      if (wf->At(i) >= ADC_BITS-1) {
	return true;
      }
    }
  }
  return false;
}

bool EXOAlphaNoiseTagger::isRingingNoise(EXOEventData *eventdata) {
// Decide if the event looks like (ringing) noise

  const int max_n_above = 3;
  const double threshold = 1000;

  // if the event was a wire trigger, then pass it
  if (eventdata->fEventHeader.fTriggerSource != 1) {
    return false;
  }

  // If it's ringing, will pass above threshold a bunch of times
  int n_above = 0;
  bool over = false;

  EXOWaveform *APDSum = GetAPDSumWaveform(eventdata);

  double avg = APDSum->Sum(0, 256) / 256;

  for (size_t t = 0; t < APDSum->GetLength(); t++) {
    if (APDSum->At(t) - avg > threshold) {
      if (!over) {
	n_above += 1;
      }
      over = true;
    } else {
      over = false;
    }
    if (n_above > max_n_above) {
      break;
    }
  }

  delete APDSum;

  return n_above > max_n_above;
}



bool EXOAlphaNoiseTagger::isSumNegBeforePosNoise(EXOEventData *eventdata) {
// If the sum of the wires goes negative before there's a large signal
// on an individual wire, tag the event as noise
// This works best, in my experience.
  const double indiv_threshold = 30;
  const double sum_threshold = 270;

  bool is_noise = false;
  EXOWaveform *WireSum = GetWireSumWaveform(eventdata);

  double avg = WireSum->Sum(0, 256) / 256;

  size_t FirstIndivWireTimeAboveThresh = FindTimeWithIndivWireAboveThresh(eventdata, indiv_threshold);

  for (size_t t = 0; t < FirstIndivWireTimeAboveThresh; t++) {
    if (WireSum->At(t) - avg < -sum_threshold) {
      is_noise = true;
      break;
    }
  }
  
  if(is_noise and isRealUSig(eventdata)){
      num_noise_overlap++;
      eventdata->fEventHeader.fOverlapNoise = true;
      std::cout <<"Overlap (Run: " << eventdata->fRunNumber << " Event: " << eventdata->fEventNumber << ")" <<std::endl;
      is_noise = false;
  }

  delete WireSum;

  return is_noise;
}



int EXOAlphaNoiseTagger::NumMaxoutChannelGlitch(EXOEventData *eventdata) {
// We've been seeing glitches where all channels max out at once
// This checks for them.

  int n_maxed_channels = 0;
  const EXOChannelMap& channelMap = GetChanMapForHeader(eventdata->fEventHeader); 
  for (int num = 0; num < 226; num++) {
    if (channelMap.good_channel(num)) {
      if (ChannelMaxesOut(eventdata, num)) {
	n_maxed_channels++;
      }
    }
  }

  return n_maxed_channels;
}

// Check if a large number of wires go negative before going positive.
// bool EXOAlphaNoiseTagger::isNegBeforePosNoise(EXOEventData *eventdata) {
//   const int max_n_noise_wires = 19;
//   const double threshold = 80;

//   // if the event was an APD trigger, then pass it
//   if (eventdata->fEventHeader.fTriggerSource == 1) {
//       return false;
//   }

//   int n_noise_wires = 0;

//   for (int i=0; i < 2*NCHANNEL_PER_WIREPLANE; i++) {
      
//     int num = i < NCHANNEL_PER_WIREPLANE ? i : i + 2 * NCHANNEL_PER_WIREPLANE;
//     EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(i);
//     if (wf == NULL) {
//       LogEXOMsg("Couldn't get the waveform for a channel.", 1);
//       return true;
//     }

//     double avg = wf->Sum(0,255)/256;

//     for (size_t t = 0; t < wf->GetLength(); t++) {
//       if ((wf->At(t) - avg) > threshold) {
// 	break;
//     } else if ((wf->At(t) - avg)  < -threshold) {
//       n_noise_wires += 1;
//       break;
//       }
//     }
//   }
//   if (verbose && n_noise_wires > max_n_noise_wires) {
//     std::cout << "Event " << eventdata->fEventNumber << " failed u-wire positive before negative noise check." << std::endl;
//   }
//   return n_noise_wires > max_n_noise_wires;
// }



bool EXOAlphaNoiseTagger::isAPDBounceNoise(EXOEventData *eventdata) {
// Decide if the event looks like a particular type of noise in which
// one APD channel maxes, then goes to near zero, then goes to baseline,
// then dips low or goes high again. E.g. run 3117, event 101

  // when it goes to zero, it's not quite zero. 20 is a reasonable threshold
  // and 1024 is a good threshold for the subsequent dip down
  // or 2048 is a good threshold for the subsequent bounce up
  const int zero_threshold = 20;
  const int dip_threshold = 1024;
  const int bump_threshold = 2048;

// loop through all APD channels (even though this seems limited to plane 1)
  for (int i = 152; i < 226; i++) {

    const EXOWaveform *wf = eventdata->GetWaveformData()->GetWaveformWithChannel(i);
    if (wf == NULL) {
      LogEXOMsg("Couldn't get the waveform for a channel.", EEDebug);
    } else {
      int NumSamples = eventdata->GetWaveformData()->fNumSamples;

      bool maxes_out = false;
      bool zeroes_out = false;
      bool returns_to_baseline = false;

      // If the signal is equal to ADC_BITS-1, the channel is maxing out
      for (int t = 0; t < NumSamples; t++) {
	if (wf->At(t) >= ADC_BITS-1) {
	  maxes_out = true;
	}
	if (maxes_out && wf->At(t) < zero_threshold) {
	  zeroes_out = true;
	}
	if (maxes_out && zeroes_out && wf->At(t) >= dip_threshold) {
	  returns_to_baseline = true;
	}
	if (maxes_out && zeroes_out && returns_to_baseline && (wf->At(t) < dip_threshold || wf->At(t) >= bump_threshold)) {
	  // that means we've seen all we need to decide it's noise
	  return true;
	}
      }
    }
  }

  // if nothing caused a return true, then it's not noise
  return false;
}



int EXOAlphaNoiseTagger::Initialize() {

  std::cout << "Initializing EXO Alpha Team Noise Tagger." << std::endl;
  std::cout << "  Events that look like noise will be tagged." << std::endl;

  return 0;
}



EXOAnalysisModule::EventStatus EXOAlphaNoiseTagger::BeginOfRun(EXOEventData *eventdata) {
  num_noise_events  = 0;
  num_noise_overlap = 0;
  run_number = -1;
  run_start_time = 0;
  run_end_time = 0;

  return kOk;
}



EXOAnalysisModule::EventStatus EXOAlphaNoiseTagger::ProcessEvent(EXOEventData *eventdata)
{
  // Fills EXOEventData::fNoiseTags and EXOEventHeader::fTaggedAsNoise.
  // Note:  fTaggedAsNoise provides less detail than fNoiseTags; it is being phased out.

  // Reset variables first -- in case we reprocess and this module changes.
  eventdata->fEventHeader.fTaggedAsNoise = false;
  eventdata->ClearNoiseTags();

  // Keep track of run times so we can report noise rate
  if (run_number < 0 ) {
    run_number = eventdata->fRunNumber;
    run_start_time = eventdata->fEventHeader.fTriggerSeconds;
  }
  run_end_time = eventdata->fEventHeader.fTriggerSeconds;


  // Check for various types of noise
  if (check_sumwire) {
    if (isSumNegBeforePosNoise(eventdata)) {
      if (verbose) {
	std::cout << "Run " << run_number << ", Event " << eventdata->fEventNumber << " failed sum u-wire going negative noise check." << std::endl;
      }
      eventdata->fEventHeader.fTaggedAsNoise = true;
      eventdata->SetNoiseTag(EXOEventData::kSummedWiresWentNegative);
      num_noise_events++;
    }
  }

  // Check for saturation.
  int num_channels_maxout = NumMaxoutChannelGlitch(eventdata);
  if(num_channels_maxout > 0) {
    // Not enough to qualify as noise-tagged, but reconstruction still likes to have this information.
    // Do this regardless of the value of check_glitch.
    eventdata->fHasSaturatedChannel = true;
  }
  if (check_glitch) {
    if (num_channels_maxout >= 100) { // 100 is our hard-coded limit for maxout noise.
      if (verbose) {
	std::cout << "Run " << run_number << ", Event " << eventdata->fEventNumber << " failed glitch check." << std::endl;
      }
      eventdata->fEventHeader.fTaggedAsNoise = true;
      eventdata->SetNoiseTag(EXOEventData::kManySaturatedWaveforms);
      num_noise_events++;
    }
  }
  if (check_apd_ringing) {
    if (isRingingNoise(eventdata)) {
      if (verbose) {
	std::cout << "Run " << run_number << ", Event " << eventdata->fEventNumber << " failed APD ringing noise check." << std::endl;
      }
      eventdata->fEventHeader.fTaggedAsNoise = true;
      eventdata->SetNoiseTag(EXOEventData::kAPDRingingNoise);
      num_noise_events++;
    }
  }
  if (check_apd_bounce) {
    if (isAPDBounceNoise(eventdata)) {
      if (verbose) {
	std::cout << "Run " << run_number << ", Event " << eventdata->fEventNumber << " failed APD bounce off rails followed by dip/bump noise check." << std::endl;
      }
      eventdata->fEventHeader.fTaggedAsNoise = true;
      eventdata->SetNoiseTag(EXOEventData::kAPDBouncingNoise);
      num_noise_events++;
    }
  }

  return (eventdata->IsTaggedAsNoise() ? kDrop : kOk);
}

EXOAnalysisModule::EventStatus EXOAlphaNoiseTagger::EndOfRun(EXOEventData *eventdata) {
  //if (verbose) {
    std::cout << "Found " << num_noise_events << " noise events in run " << run_number << std::endl;
    std::cout << "  Noise rate: " << static_cast<double>(num_noise_events)/(run_end_time-run_start_time) << " Hz" << std::endl;
    std::cout << "Num of Noise events overlapping real is " << num_noise_overlap << std::endl;
    //}

  return kOk;
}



int EXOAlphaNoiseTagger::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/alphanoisetag/verbose",
			       "print out information about noise events",
			       this,
			       false,
			       &EXOAlphaNoiseTagger::SetVerbose);

  talktoManager->CreateCommand("/alphanoisetag/apdringing",
			       "check for APD ringing noise",
			       this,
			       false,
			       &EXOAlphaNoiseTagger::SetCheckAPDRinging);

  talktoManager->CreateCommand("/alphanoisetag/sumwire",
			       "check for sum u-wires going negative noise",
			       this,
			       true,
			       &EXOAlphaNoiseTagger::SetCheckSumWire);
  talktoManager->CreateCommand("/alphanoisetag/glitch",
			       "check for all channels maxing out glitch",
			       this,
			       true,
			       &EXOAlphaNoiseTagger::SetCheckGlitch);
  talktoManager->CreateCommand("/alphanoisetag/apdbounce",
			       "check for APD channel bouncing off max, min, then dipping or bumping",
			       this,
			       true,
			       &EXOAlphaNoiseTagger::SetCheckAPDBounce);

  return 0;
}
