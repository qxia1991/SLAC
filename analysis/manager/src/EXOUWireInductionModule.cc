//______________________________________________________________________________
// Module to identify U wire signals with "induction-like" characteristics.  This
// module calculates discriminators based on the pulse timing, integral, and
// fit chi^2, and sets a flag if a signal appears to be induction based on these
// discriminators.
//
// 2012-11-20 DCM:  Initial version
// 2013-04-17 DCM:  Fix finding of nearest neighbor energy for pulses split across multiple signals
//
#include "EXOAnalysisManager/EXOUWireInductionModule.hh"
#include "EXOReconstruction/EXOUWireIndSignalModelBuilder.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "TH1D.h"
#include <iostream>
#include "TMath.h"
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOUWireInductionModule, "uind" )

EXOUWireInductionModule::EXOUWireInductionModule()
:  fShapingPrePulseLength(32),
   fShapingPostPulseLength(128),
   fElectronicsDatabaseFlavor("measured_times"),
   fChargeMatchTime(40.0*SAMPLE_TIME),  // Want larger time than for standard clustering since 
			  	       // start may be misestimated for induction signals
   fIntegLowerBound(10),                  
   fIntegUpperBound(40),
   fMaxMinLowerBound(100),
   fMaxMinUpperBound(100),
   fMaxMinLowerFrac(0.05),
   fMaxMinUpperFrac(0.9),
   fMaxMinInductionBound(30.0),
   fIntegInductionBound(40.0),
   fChi2RatInductionBound(-0.5),
   fNeighEngInductionBound(1000.0)
{}

int EXOUWireInductionModule::Initialize()
{
 
  return 0;
}

EXOAnalysisModule::EventStatus EXOUWireInductionModule::BeginOfRun(EXOEventData *ED)
{

  return kOk;
}

EXOAnalysisModule::EventStatus EXOUWireInductionModule::ProcessEvent(EXOEventData *ED)
{


  EventStatus returnStatus;

  // First, calculate quantities used to discriminate between induction and deposition pulses
  // and fill these values in the U wire signal object
  returnStatus = CalculatePulseTiming(ED);   // Max-to-min time from raw trace
  if(returnStatus != kOk) return returnStatus;

  returnStatus = CalculatePulseIntegral(ED);   // Integral of unshaped pulse
  if(returnStatus != kOk) return returnStatus;

  returnStatus = CalculateFitChi2Ratio(ED);  // Log of ratio of deposition to induction chi^2
  if(returnStatus != kOk) return returnStatus;

  returnStatus = CalculateMaxNeighEng(ED);  // Max raw energy on nearest neighbor wire
  if(returnStatus != kOk) return returnStatus;

  // Now, for each U wire signal, determine if it is "induction-like" and save a flag in the
  // signal object

  for(unsigned int i=0; i<ED->GetNumUWireSignals(); i++){
    EXOUWireSignal *currSig = ED->GetUWireSignal(i);

    Bool_t ind_like = false;

    if( (currSig->fMaxMinTime < fMaxMinInductionBound) &&
	(currSig->fUnshapedInteg < fIntegInductionBound) &&
	(currSig->fChiSqRatio > fChi2RatInductionBound) &&
	(currSig->fMaxNeighEng > fNeighEngInductionBound) ) ind_like = true;

    currSig->fIsInduction = ind_like;

  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOUWireInductionModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}


EXOAnalysisModule::EventStatus EXOUWireInductionModule::CalculatePulseTiming(EXOEventData* ED)  {
  // Calculates the time between the maximum and minimum of the pulse


  //First loop over all U wire signals from reconstruction
  EXOWaveformData *wf_data = ED->GetWaveformData();

  for(unsigned int i=0; i<ED->GetNumUWireSignals(); i++){
    EXOUWireSignal *currSig = ED->GetUWireSignal(i);

    // Pull the waveform and convert to an EXODoubleWaveform.
    if(wf_data->GetWaveformWithChannel(currSig->fChannel) == NULL) continue;
    EXODoubleWaveform dwf  = wf_data->GetWaveformWithChannel(currSig->fChannel)->Convert<Double_t>();
    
    //Subtract baseline from waveform
    double baseline = fBaselineCalculator.Extract(dwf);
    dwf -= baseline;

    // Set beginning and ending index for reshaping, ensuring that the desired
    // window around the peak lies within the trace
    size_t peak_idx = dwf.GetIndexAtTime(currSig->fTime);
    size_t begin_idx = (peak_idx + 1 > fMaxMinLowerBound) ? peak_idx + 1 - fMaxMinLowerBound : 0;
    size_t end_idx = (peak_idx + fMaxMinUpperBound < dwf.GetLength()) ? peak_idx + fMaxMinUpperBound : dwf.GetLength();

    // Initialize new waveform with the points in the window
    EXODoubleWaveform windowed_signal;
    windowed_signal.MakeSimilarTo(dwf);
    windowed_signal.SetData(&dwf[begin_idx], end_idx-begin_idx);
    windowed_signal.SetTOffset(dwf.GetTimeAtIndex(begin_idx));

    // For now, we use a simple algorithm which finds the point following the
    // peak for which the pulse last falls below 90% of the max height
    // and the first point after which it falls below 10% above the min height.
    // In the future, we may want to add some additional filtering of noise 
    // before doing the search.  This algorithm will also not perform well
    // if there is pile up in the 100 us window around the pulse max

    // First find location of pulse max/min in search window:
    Double_t pulse_max = -99999.9;
    Double_t pulse_min = 99999.9;
    size_t max_loc = 0;
    for(size_t j=0; j<windowed_signal.GetLength(); j++) {
      // Check for max
      if(windowed_signal[j] > pulse_max) {
	pulse_max = windowed_signal[j];
	max_loc = j;
      }
    }
    
    //Now search from the location of the maximum to the end of the window for a minimum
    for(size_t j=max_loc; j<windowed_signal.GetLength(); j++) {      
      // Check for min
      if(windowed_signal[j] < pulse_min) {
	pulse_min = windowed_signal[j];
      }
    }

    // Make sure we found something reasonable
    if( (pulse_max == -99999.9) || (pulse_min == 99999.9) ) {
      LogEXOMsg(Form("Max-to-min finding failed, channel: %d",currSig->fChannel), EEWarning); 
      return kOk;
    }

    // Now walk away from maximum and find the last crossing time of the upper threshold
    Double_t tot_height = pulse_max - pulse_min;
    Double_t upper_thresh = pulse_min + tot_height*fMaxMinUpperFrac;
    Double_t lower_thresh = pulse_min + tot_height*fMaxMinLowerFrac;

    size_t max_cross = 0;
    size_t min_cross = 0;

    // First find crossing for upper threshold
    for(size_t j=max_loc; j<windowed_signal.GetLength()-1; j++) {
      if( (windowed_signal[j] > upper_thresh) && (windowed_signal[j+1] <= upper_thresh)) max_cross = j;
    }

    // Now search for first crossing of lower threshold from the max
    for(size_t j=max_loc; j<windowed_signal.GetLength()-1; j++) {
      if( (windowed_signal[j] > lower_thresh) && (windowed_signal[j+1] <= lower_thresh)) {
	min_cross = j;
        break; // stop searching after finding first crossing
      }
    }

    // For a bit more accuracy, interpolate to the exact threshold
    Double_t max_cross_interp = max_cross + 
                (upper_thresh-windowed_signal[max_cross+1])/(windowed_signal[max_cross]-windowed_signal[max_cross+1]);
    Double_t min_cross_interp = min_cross + 
                (lower_thresh-windowed_signal[min_cross+1])/(windowed_signal[min_cross]-windowed_signal[min_cross+1]);

    currSig->fMaxMinTime =  min_cross_interp - max_cross_interp;

  }

  return kOk;
}


EXOAnalysisModule::EventStatus EXOUWireInductionModule::CalculatePulseIntegral(EXOEventData* ED)  {
  // Calculate the integral of the unshaped pulse.  Also, calculate the max pulse
  // height for normalization

  const EXOElectronicsShapers *electronicsShapers = GetCalibrationFor(
          EXOElectronicsShapers, 
          EXOElectronicsShapersHandler, 
          fElectronicsDatabaseFlavor, 
          ED->fEventHeader);

  // Number of samples to reshape 
  const size_t pulse_length = fShapingPostPulseLength + fShapingPrePulseLength + 1;

  //First loop over all U wire signals from reconstruction
  EXOWaveformData *wf_data = ED->GetWaveformData();

  for(unsigned int i=0; i<ED->GetNumUWireSignals(); i++){
    EXOUWireSignal *currSig = ED->GetUWireSignal(i);
    
    // Pull the waveform and convert to an EXODoubleWaveform.
    if(wf_data->GetWaveformWithChannel(currSig->fChannel) == NULL) continue;
    EXODoubleWaveform dwf  = wf_data->GetWaveformWithChannel(currSig->fChannel)->Convert<Double_t>();
    
    //Subtract baseline from waveform
    double baseline = fBaselineCalculator.Extract(dwf);
    dwf -= baseline;

    // Set beginning and ending index for reshaping, ensuring that the desired
    // window around the peak lies within the trace
    size_t peak_idx = dwf.GetIndexAtTime(currSig->fTime);
    size_t begin_idx = (peak_idx + 1 > fShapingPrePulseLength) ? peak_idx + 1 - fShapingPrePulseLength : 0;
    size_t end_idx = peak_idx + fShapingPostPulseLength;
    size_t trace_length = dwf.GetLength();
    
    // If the window is clipped, push it back into the trace
    if( end_idx >= trace_length ) {
      begin_idx = (trace_length + 1 > pulse_length) ? trace_length + 1 - pulse_length : 0;
      end_idx = trace_length;
    } else if(end_idx < pulse_length) {
      //in this case, start the window at 0
      begin_idx = 0; 
      end_idx = (pulse_length < trace_length) ? pulse_length - 1 : trace_length; 
    }

    size_t offset = peak_idx - begin_idx;  //index of the pulse start in the subtrace

    //Get the transfer function for the channel
    const EXOTransferFunction& tf = 
      electronicsShapers->GetTransferFunctionForChannel(currSig->fChannel);


    EXODoubleWaveform unshaped_signal;
    unshaped_signal.MakeSimilarTo(dwf);
    unshaped_signal.SetData(&dwf[begin_idx], end_idx-begin_idx);
    unshaped_signal.SetTOffset(dwf.GetTimeAtIndex(begin_idx));

    // Max peak height of shaped pulse in shaping window, used to normalize integral.
    // Don't use energy from fit for now since deposition template misestimates height
    // of induction pulses.
    Double_t shaped_max = unshaped_signal.GetMaxValue();

    UnshapeSignal(unshaped_signal, tf);

    // Only do the integral on the central 50 samples (-10,+40) to reduce problems resulting from 
    // poor unshaping (e.g. run away baselines).  May want to correct baseline slopes to avoid this problem
    size_t low_offset = fIntegLowerBound;
    size_t high_offset = fIntegUpperBound;
    if( offset < low_offset || (pulse_length-offset)< high_offset ) {
      // Do nothing, leaving error value for this event
      // LogEXOMsg(Form("Unshaping failed, insufficient buffer from trace edge, channel: %d",currSig->fChannel), EEWarning); 
    } else {
      currSig->fUnshapedInteg = unshaped_signal.Sum(offset-low_offset, offset+high_offset)/shaped_max;
    }

  }


  return kOk;

}


void EXOUWireInductionModule::UnshapeSignal(EXODoubleWaveform& filtered_signal, const EXOTransferFunction& tf) const {

  EXODoubleWaveform filtered_signal_temp;
  filtered_signal_temp.MakeSimilarTo(filtered_signal);

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

}


EXOAnalysisModule::EventStatus EXOUWireInductionModule::CalculateFitChi2Ratio(EXOEventData* ED)  {
  // Calculate the chi2 ratio between induction and deposition fits

  for(unsigned int i=0; i<ED->GetNumUWireSignals(); i++) {
    EXOUWireSignal *currSig = ED->GetUWireSignal(i);

    Double_t dep_chi_sq = currSig->fChiSquareRestr;
    Double_t ind_chi_sq = currSig->fChiSquareIndRestr;
    
    // If no "induction" signal was found on the same channel for which a 
    // deposition signal was found then unlikely to be deposition.  In this
    // case, assign a high induction chi^2 value
    if( ind_chi_sq == -1 ) ind_chi_sq = 1e3;

    // Save the log of the ratio of the chi2 values
    currSig->fChiSqRatio = log10(dep_chi_sq/ind_chi_sq);

  }

  return kOk;
}


EXOAnalysisModule::EventStatus EXOUWireInductionModule::CalculateMaxNeighEng(EXOEventData* ED)  {
  // Calculate the chi2 ratio between induction and deposition fits

  for(size_t i=0; i<ED->GetNumUWireSignals(); i++) {
   
    EXOUWireSignal *currSig = ED->GetUWireSignal(i);

    Int_t channel = currSig->fChannel;    
    Double_t curr_time = currSig->fTime;

    // Check for neighboring signal
    Double_t totalEnergyLower = 0.0;
    Double_t totalEnergyUpper = 0.0;
    for(size_t j=0; j<ED->GetNumUWireSignals(); j++) {

      // Skip current iteration
      if( j == i) continue;

      EXOUWireSignal *compSig = ED->GetUWireSignal(j);

      // Check lower channel
      if( ((channel-1) == compSig->fChannel) && 
	  (std::abs(curr_time-compSig->fTime) <= fChargeMatchTime) &&
	  (EXOMiscUtil::TypeOfChannel(compSig->fChannel)==EXOMiscUtil::kUWire)  ) {

	// We have a signal on a neighboring U-wire channel in the time window
	totalEnergyLower += compSig->fRawEnergy;
      }

      // Check upper channel
      if( ((channel+1) == compSig->fChannel) && 
	  (std::abs(curr_time-compSig->fTime) <= fChargeMatchTime) &&
	  (EXOMiscUtil::TypeOfChannel(compSig->fChannel)==EXOMiscUtil::kUWire)  ) {

	// We have a signal on a neighboring U-wire channel in the time window
	totalEnergyUpper += compSig->fRawEnergy;
      }

    } // end loop over comparison signals

    currSig->fMaxNeighEng = std::max(totalEnergyLower,totalEnergyUpper);

  } // end loop over all signals

  return kOk;
}



int EXOUWireInductionModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/uind/UnshapingPrePulseSamples",
			       "Number of samples before the pulse used to unshape the pulse", 
			       this, 
			       fShapingPrePulseLength,
			       &EXOUWireInductionModule::SetShapingPrePulse);

  talktoManager->CreateCommand("/uind/UnshapingPostPulseSamples",
			       "Number of samples after the pulse used to unshape the pulse", 
			       this, 
			       fShapingPostPulseLength,
			       &EXOUWireInductionModule::SetShapingPostPulse);

  talktoManager->CreateCommand("/uind/IntegralPrePulseSamples",
			       "Number of samples before the pulse used to calculate the integral", 
			       this, 
			       fIntegLowerBound,
			       &EXOUWireInductionModule::SetIntegralPrePulse);

  talktoManager->CreateCommand("/uind/IntegralPostPulseSamples",
			       "Number of samples after the pulse used to calculate the integral", 
			       this, 
			       fIntegUpperBound,
			       &EXOUWireInductionModule::SetIntegralPostPulse);

  talktoManager->CreateCommand("/uind/MaxMinPrePulseSamples",
			       "Number of samples before the pulse used to calculate the max-to-min time", 
			       this, 
			       fMaxMinLowerBound,
			       &EXOUWireInductionModule::SetMaxMinPrePulse);

  talktoManager->CreateCommand("/uind/MaxMinPostPulseSamples",
			       "Number of samples after tche pulse used to calculate the max-to-min time", 
			       this, 
			       fMaxMinUpperBound,
			       &EXOUWireInductionModule::SetMaxMinPostPulse);

  talktoManager->CreateCommand("/uind/ElectronicsDBFlavor",
                               "Set the flavor string used to query the database for electronics shaping times (used for unshaping)",
                               this,
                               fElectronicsDatabaseFlavor,
                               &EXOUWireInductionModule::SetElectronicDatabaseFlavor );

  talktoManager->CreateCommand("/uind/MaxMinTimeInductionCut",
			       "Cut on min-to-max time used to identify induction signals", 
			       this, 
			       fMaxMinInductionBound,
			       &EXOUWireInductionModule::SetMaxMinInductionCut);

  talktoManager->CreateCommand("/uind/IntegralInductionCut",
			       "Cut on unshaped pulse integral used to identify induction signals", 
			       this, 
			       fIntegInductionBound,
			       &EXOUWireInductionModule::SetIntegInductionCut);

  talktoManager->CreateCommand("/uind/ChiSqRatioInductionCut",
			       "Cut on chi^2 ratio for deposition and induction templates used to identify induction signals", 
			       this, 
			       fChi2RatInductionBound,
			       &EXOUWireInductionModule::SetChi2RatInductionCut);

  talktoManager->CreateCommand("/uind/UNeighborEnergyInductionCut",
			       "Cut on nearest neighbor U-wire energy used to identify induction signals", 
			       this, 
			       fNeighEngInductionBound,
			       &EXOUWireInductionModule::SetNeighEngInductionCut);

  return 0;
}

int EXOUWireInductionModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}

void EXOUWireInductionModule::CallBack()
{
  std::cout << "EXOUWireInductionModule::CallBack() called. " << std::endl;
}


