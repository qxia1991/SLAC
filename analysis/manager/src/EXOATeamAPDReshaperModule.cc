//______________________________________________________________________________
// Created 06-20-10
// Ian Counts
// Last Modified 11-24-10
// Phil Barbeau
//******************************
// This module unshapes the APD signals with the known unshaping times, which are stored
// in EXODimensions.  It then reshapes the signals with the filter of choice.  Gaussian, 
// Finite Cusp, Trianguler, or RC-RC-CR-CR.  There is also the option to apply no filter.
// The characteristic shaping times are also commands chosen in the .exo file.  The 
// reshaping is performed on each APD channel.  The resulting peak-baseline of each APD
// channel is recorded in ED->csc and ED->cerrsc, orignally created by R. Nielson to record
// individual APD peak heights.  Currently there is only one APD signal that is recorded.  
// Renormalization is determined analytically, by shaping a step function.  
// ====================================================================================
// Changelog:
// 09-01-10:  Created this changelog
// 11-15-10:  Possibly removed a bug with the use of trigoff.  We'll see. ---psb
// 11-24-10:  shortened the length over which the rehsaping is done.  Don't need the full waveform for the APDs, and it helps mitigate 
//            runaway effects from parallel noise.  In the future, consider using Clayton's fixed unshapers that are used in pattern 
//            recognition 3 in reconstruciton. ---psb
// 11-27-10:  added a feature to measure the RMS. ---psb
// 11-27-11:  Update for new EXOAPDSignals. --- V.Belov
// ====================================================================================== 

#include "EXOAnalysisManager/EXOATeamAPDReshaperModule.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <iostream>
#include "TH1F.h"
#include "TF1.h"
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOATeamAPDReshaperModule, "apreshap" )

EXOATeamAPDReshaperModule::EXOATeamAPDReshaperModule(): 
  tau_param(10),
  T_param(6.0),
  RC1_param(10.0),
  RC2_param(10.0),
  CR1_param(10.0),
  CR2_param(10.0),
  overwriteSignals_param(false),
  makeNoiseCorners_param(false)
{}

int EXOATeamAPDReshaperModule::Initialize() {

  cout << "Initializing " << GetName() << endl;

  //-----------
  // calculate renormalization factors
  
  // construct step function
  int length = 1000;
  double signal[length];
  double step_height = 1.0;

  for (int i = 0; i < length; i++) {
    if (i <= length/2) signal[i] = 0.0;  // 0 baseline
    else signal[i] = step_height;
  }

  // filter appropriately
  
  p_f = 0.0;   // renormalization factor: accounts for filtering
  string filter = filter_param;

  if (filter == "no_filter") {
    // Needs to be handled later
  }
  else if (filter == "finite_cusp") {
    FiniteCusp(signal, length);
    double h_fin = Max(signal, length);
    p_f = h_fin / step_height;
  }
  else if (filter == "triangular") {
    Triangular(signal, length);
    double h_tri = Max(signal, length);
    p_f = h_tri / step_height;
  }
  else if (filter == "gaussian") {
    Gaussian(signal, length);
    double h_gauss = Max(signal, length);
    p_f = h_gauss / step_height;
  }
  else if (filter == "rc_cr") {
    RC_CR(signal, length);
    double h_rc_cr = Max(signal, length);
    p_f = h_rc_cr / step_height;
  }

  return 0;
}

EXOAnalysisModule::EventStatus EXOATeamAPDReshaperModule::BeginOfRun(EXOEventData *ED) {
    cout << "At BeginOfRun for " << GetName() << ", run number = " 
         << ED->fRunNumber << endl;
    bool makeNoiseCorners = makeNoiseCorners_param;
    if (makeNoiseCorners) {
      cout << "-- Making Noise Corner Graphs for APD channels." << endl;
    }

  return kOk;
}


EXOAnalysisModule::EventStatus EXOATeamAPDReshaperModule::ProcessEvent(EXOEventData *ED) {  

  string filter = filter_param;

  int length = ED->GetWaveformData()->fNumSamples;  // length of waveform

  double t0 = FindTrigger(length, ED);

  bool makeNoiseCorners = makeNoiseCorners_param;

  //---------------Begin Standard event by event reshaping------------//
  //loop over all APD channels
  const EXOElectronicsShapers* shapers = GetCalibrationFor(EXOElectronicsShapers, EXOElectronicsShapersHandler, "vanilla", ED->fEventHeader);

  for (size_t channelNum = NWIREPLANE*NCHANNEL_PER_WIREPLANE; channelNum < NUMBER_READOUT_CHANNELS; channelNum++) {

    if(filter_param == "no_filter") p = 1.0;
    else {
      double gain = shapers->GetTransferFunctionForChannel(channelNum).GetGain();
      p = p_f / gain;
    }

    EXOWaveform* wf_ptr = ED->GetWaveformData()->GetWaveformWithChannelToEdit(channelNum);
    if (!wf_ptr) {
      cout << "Channel " << channelNum << " not able to be found." << endl;  
      continue;
    }
    // References are easier to deal with
    EXOWaveform& wf = *wf_ptr;

    //fill wfm with waveform data
    int wfm[length];
    double raw[length];
    double reshaped[length];
    Int_t increment_forward;

    for (int i = 0; i < length; i++) {
      wfm[i] = wf[i]; 
      raw[i] = wfm[i];
    }

    //determine baseline using the original long waveform
    double baseline = Baseline(raw, length, t0);

    int shift = 0;
    length -=shift;

    Double_t* unshaped = EXOMiscUtil::unshape_with_parameters_impl( wfm+shift, length, increment_forward, shapers->GetTransferFunctionForChannel(wf.fChannel), baseline);

    t0 -= increment_forward;

    //filter shortwfm.  Using the shorter one cleans things up a bit.
    if (filter == "no_filter") {

    }
    else if (filter == "finite_cusp") {
      FiniteCusp(unshaped, length);
    }
    else if (filter == "triangular") {
      Triangular(unshaped, length);
    }
    else if (filter == "gaussian") {
      Gaussian(unshaped, length);
    }
    else if (filter == "rc_cr") {
      RC_CR(unshaped, length);
    }
    else {
      LogEXOMsg("invalid filtering method", EEError);
    }

    //renormalize waveforms...note that despite the name, they are no longer unshaped
    for (int i = 0; i < length; i++) {
      unshaped[i] /= p;
    }
        
    //calculate amplitude
    double amplitude = Amplitude(unshaped, length, t0 );

    double rms = MeasureRMS(unshaped, length, t0 );

    //export to csc array.  For the time being, this only works for single scintilation cluster events.
    // M. Marino:
    // To remove info, you now have to do the following:
    // Perhaps migrate this to another data member?
    while (ED->GetNumScintillationClusters() > 1) {
       ED->Remove(ED->GetScintillationCluster(ED->GetNumScintillationClusters()-1));
    }
    if (ED->GetNumScintillationClusters() == 0) {
      // In the case that none exist, fill one. 
      ED->GetNewScintillationCluster();
    } 
    EXOScintillationCluster* scint_clust = ED->GetScintillationCluster(0);
          EXOAPDSignal *sig = ED->GetNewAPDSignal();
          sig->fType = EXOAPDSignal::kGangFit;
          sig->fChannel = channelNum;
          sig->fDescr = "gang_fit";
          sig->fTime = 0.;//ChannelFit[i].Time;
          sig->fRawCounts = amplitude;
          sig->fCountsError = rms;
          sig->fChiSquared = 0.;//ChannelFitChiSquare;
          sig->SetScintCluster(scint_clust);
          scint_clust->InsertAPDSignal(sig);

    bool overwriteSignals = overwriteSignals_param;
    if (overwriteSignals) {

      //add back baseline
      for (int i = 0; i < length; i++) {
	unshaped[i] += baseline;
      } 

      //write waveforms to data
      for (int i =0; i < length; i++) {
	wf[i] = (int) wfm[i];  // data in ints
       
      }
    }
	delete [] unshaped;
  }
  return kOk;
}

int EXOATeamAPDReshaperModule::TalkTo(EXOTalkToManager *talktoManager) {

  talktoManager->CreateCommand("/apreshap/filter_method", 
                               "choose filtering method", 
                               this,
                               "finite_cusp",
                               &EXOATeamAPDReshaperModule::SetFilterParam);

  talktoManager->CreateCommand("/apreshap/tau", "used in multiple filters", this, tau_param, &EXOATeamAPDReshaperModule::Settau_param );


  talktoManager->CreateCommand("/apreshap/T", 
                               "used in finite_cusp filtering", 
                               this, T_param, &EXOATeamAPDReshaperModule::SetT_param );

  talktoManager->CreateCommand("/apreshap/RC1",
                               "used in RC_CR filtering",
                               this, RC1_param, &EXOATeamAPDReshaperModule::SetRC1_param );

  talktoManager->CreateCommand("/apreshap/RC2",
                               "used in RC_CR filtering",
                               this, RC2_param, &EXOATeamAPDReshaperModule::SetRC2_param );

  talktoManager->CreateCommand("/apreshap/CR1",
                               "used in RC_CR filtering",
                               this, CR1_param, &EXOATeamAPDReshaperModule::SetCR1_param );

  talktoManager->CreateCommand("/apreshap/CR2",
                               "used in RC_CR filtering",
                               this, CR2_param, &EXOATeamAPDReshaperModule::SetCR2_param );

  talktoManager->CreateCommand("/apreshap/overwriteSignals", "overwrites data array", this, overwriteSignals_param, &EXOATeamAPDReshaperModule::SetoverwriteSignals_param );

  talktoManager->CreateCommand("/apreshap/makeNoiseCorners", "produces noise corners and finds minimum noise", this, makeNoiseCorners_param, &EXOATeamAPDReshaperModule::SetmakeNoiseCorners_param );

  return 0;
}



int EXOATeamAPDReshaperModule::ShutDown() {

  cout << "At ShutDown for " << GetName() << endl;

  bool makeNoiseCorners = makeNoiseCorners_param;

  if (makeNoiseCorners) {
 
    cout << "-- Done with Noise Corner Graphs for APD channels." << endl;
  }    

  return 0;
}




/* private functions */

////////////////////////////////////////////////////////////////
// requires: tau, T
////////////////////////////////////////////////////////////////

void EXOATeamAPDReshaperModule::FiniteCusp(double signal[], int length) {

  int tau = tau_param;
  double T = T_param;

  double temp[length];
  for (int i = 0; i < length; i++) {
    temp[i] = 0.0;
  }

  double t;

  for (int i = 1; i < length; i++) {
    for (int j = i + 1 + int(T/2); j < min(i + int(T*tau/2.0) + 1 + int(T/2), length); j++) {

      t = j - i - T/2.0;
      temp[j] += (signal[i] - signal[i-1]) * sinh(t/tau);
    }
    for (int j = min(i + int(T*tau/2.0) + 1 + int(T/2), length); 
	 j < min(i + int(T*tau) + 1+ int(T/2), length); j++) {

      t = j - i - T/2.0;
      temp[j] += (signal[i] - signal[i-1]) * sinh((T*tau-t)/tau);
    }
  }

  //copy temp to signal
  for (int i = 0; i < length; i++) {
    signal[i] = temp[i];
  }

}

////////////////////////////////////////////////////////////////
// requires: tau
////////////////////////////////////////////////////////////////

void EXOATeamAPDReshaperModule::Triangular(double signal[], int length) {

  int tau = tau_param;

  double temp[length];
  for (int i = 0; i < length; i++) {
    temp[i] = 0.0;
  }

  double t;

  for (int i = 1; i < length; i++) {
    for (int j = i+1; j < min(i + int(2.0*tau) + 1, length); j++) {
      t = fabs(j - i - 1.0*tau);
      temp[j] += (signal[i] - signal[i-1])*(1.0-1.0*t/tau);
    }
  }

  //copy temp to signal
  for (int i = 0; i < length; i++) {
    signal[i] = temp[i];
  }
  
}

////////////////////////////////////////////////////////////////
// requires: tau
////////////////////////////////////////////////////////////////

void EXOATeamAPDReshaperModule::Gaussian(double signal[], int length) {
 
  int tau = tau_param;
  
  double sample_time = 1.0;
  double CR_tratio = sample_time / tau;
  double RC_tratio = sample_time / tau;


  CR_shaper(signal, length, CR_tratio, 0.0);

  RC_shaper(signal, length, RC_tratio);
  RC_shaper(signal, length, RC_tratio);
  RC_shaper(signal, length, RC_tratio);  
  RC_shaper(signal, length, RC_tratio);

}


////////////////////////////////////////////////////////////////
// requires: RC1, RC2, CR1, CR2
////////////////////////////////////////////////////////////////
void EXOATeamAPDReshaperModule::RC_CR(double signal[], int length) {
  
  double RC1 = RC1_param;
  double RC2 = RC2_param;
  double CR1 = CR1_param;
  double CR2 = CR2_param;

  double sample_time = 1.0;
  double RC1_tratio = sample_time / RC1;
  double RC2_tratio = sample_time / RC2;
  double CR1_tratio = sample_time / CR1;
  double CR2_tratio = sample_time / CR2;

  RC_shaper(signal, length, RC1_tratio);
  RC_shaper(signal, length, RC2_tratio);
  CR_shaper(signal, length, CR1_tratio, 0.0);
  CR_shaper(signal, length, CR2_tratio, 0.0);

}


void EXOATeamAPDReshaperModule::CR_shaper(double signal[], int n, double tratio, double baseline) {

  // CR_shapes a signal
  // n = number of samples
  // tratio = sample_time/CR_time

  double temp[n];

  double w = exp( -1.0*tratio );

  temp[0] = signal[0];
  for (int i = 1; i < n; i++ ) {
    temp[i] = baseline + signal[i] - signal[i-1] +
      (temp[i-1] - baseline)*w;
  }

  //copy temp to signal
  for (int i = 0; i < n; i++) {
    signal[i] = temp[i];
  }

}


void EXOATeamAPDReshaperModule::RC_shaper(double signal[], int n, double tratio) {

  // RC_shapes a signal
  // n = number of samples
  // tratio = sample_time/RC_time

  double temp[n];

  double w = exp( -1.0*tratio );
  double d1, d0;

  temp[0] = signal[0];
  d0 = 0.0;
  for (int i = 1; i < n; i++ ) {
    d1 = signal[i] - signal[i-1] + d0*w;
    temp[i] = signal[i]*(1.0-w) + signal[i-1]*w - 
      d0*w*w;
    d0 = d1;
  }

  //copy temp to signal
  for (int i = 0; i < n; i++) {
    signal[i] = temp[i];
  }

}


void EXOATeamAPDReshaperModule::CR_unshaper(double signal[], int n, double tratio, double baseline) {

  // CR_unshapes a signal
  // n = number of samples
  // tratio = sample_time/CR_time

  double temp[n];

  double w = exp( -1.0*tratio );

  temp[0] = signal[0];

  for ( Int_t i = 1; i < n; i++ ) {
    temp[i] = signal[i] + temp[i-1] - signal[i-1]*w - 
      baseline*(1.0-w);

  }

  //copy temp to signal
  for (int i = 0; i < n; i++) {
    signal[i] = temp[i];
  }

}


void EXOATeamAPDReshaperModule::RC_unshaper(double signal[], int n, double tratio) {

  // RC_unshapes a signal
  // n = number of samples
  // tratio = sample_time/RC_time

  double temp[n];

  for (int i = 0; i < n; i++ ) temp[i] = 0.0;

  double w = exp( -1.0*tratio );
  if ( w >= 1.0 ) {
    cout << "RC_unshaper: tratio shouldn't be zero" << endl;
    return;
  }
  double d1, d0;

  temp[0] = signal[0];
  d0 = 0.0;
  for (int i = 1; i < n-1; i++ ) {
    temp[i] = (signal[i+1] - temp[i-1]*w + d0*w*w)/(1.0-w);
    d1 = temp[i] - temp[i-1] + d0*w;
    d0 = d1;
  }

  // shift it forward by one bin
  for (int i = n-1; i > 0; i-- ) {
    temp[i] = temp[i-1];
  }

  //copy temp to signal
  for (int i = 0; i < n; i++) {
    signal[i] = temp[i];
  }

}

// From pretrace time
double EXOATeamAPDReshaperModule::Baseline(double signal[], int length, double trigger) {
  
  if (trigger > length) {
    LogEXOMsg("trigger index out of bounds", EEError);
    return -1.0;
  }
  
  double sum = 0;
  int PreTraceBuffer = int( 0.3*trigger );
  int BaseLineLength = int( 0.9*trigger - PreTraceBuffer );
  
  int beginBaseLine = 0;
  if (trigger - PreTraceBuffer - BaseLineLength > beginBaseLine) {
    beginBaseLine = int(trigger -  PreTraceBuffer - BaseLineLength);
  }

  int endBaseLine = 1;
  if (trigger - PreTraceBuffer > endBaseLine) {
    endBaseLine = int(trigger - PreTraceBuffer); 
  }

  for (int i = beginBaseLine; i < endBaseLine; i++) {
    sum += signal[i];
  }

  BaseLineLength = endBaseLine - beginBaseLine;

  return sum / BaseLineLength;
}

double EXOATeamAPDReshaperModule::FindTrigger(int length, EXOEventData *ED) {

  // find index of trigger: smallest element in tsc
  double t0 = 0;
  size_t nsc = ED->GetNumScintillationClusters();
  if (nsc > 1) t0 = ED->GetScintillationCluster(0)->fTime;
  for (size_t i=1;i<nsc;i++) {
    EXOScintillationCluster* scint = ED->GetScintillationCluster(i);
    if (t0 < scint->fTime) t0 = scint->fTime;
  }
  t0 /= 1000.0;     // tsc value in nanoseconds; convert to microseconds

  // if reconstruction (which fills tsc) has not been run...
  if (t0 == 0) {
    //t0 = ED->fEventHeader.fTriggerOffset / 1000.0;  //Don't know if this makes sense -psb
    t0 = ED->fEventHeader.fTriggerOffset / 1.0;
  }

  // if trigoff has not been set for some reason...
  if (t0 == 0) {
    t0 = TRIGGER_TIME / 1000.0;
  }
 
  if (t0 > length) {
    LogEXOMsg( "trigger index out of bounds", EECritical);
  }  

  return t0;
}

double EXOATeamAPDReshaperModule::Max(double signal[], int length) {

  int buffer = 30;
  double max = signal[0];
  for (int i = 1 + buffer; i < length-buffer; i++) {
    if (signal[i] > max) max = signal[i];
  }

  return max;
}


double EXOATeamAPDReshaperModule::Min(double signal[], int length) {
   
  int buffer = 30;
   double min = signal[0];
   for (int i = 1+buffer; i < length-buffer; i++) {
     if (signal[i] < min) min = signal[i];
   }

   return min;
}


double EXOATeamAPDReshaperModule::Amplitude(double signal[], int length, double trigger) {
 
  return fabs(Max(signal, length) - Baseline(signal, length, trigger));
}

// This function will only measure the rms of the noise around the baseline from 0.3*t0 to 0.9*t0.
// Care must be taken to avoid any event at the trigger, or edge effects early due to reshaping.
// As such, one will only get good results when the characteristic shaping time is ~<0.3*t0,
// which is clearly fine for trigger times of ~ 1024 us. I pass in the trigger time so that I do not 
// spend all of my time trying to determine it.
double EXOATeamAPDReshaperModule::MeasureRMS(double signal[], int length, double trigger) {
  
  int firstSample = int( 0.3 * trigger );
  int lastSample = int( 0.9 * trigger );
  double sum = 0.0;
  double baseline = 0.0;

  for (int i = firstSample; i < lastSample; i++) {
    sum += signal[i];
  } 
  
  if (  lastSample - firstSample > 0) {
    baseline = sum / ( firstSample - lastSample );
  }
  else {
    baseline = signal[firstSample];
  }

  double sumSquare = 0.0;
  double rms = 0.0;
  
  for (int i = firstSample; i < lastSample; i++) {
    sumSquare += ( signal[i] - baseline ) * ( signal[i] - baseline );  
  }

  if (  lastSample - firstSample > 0) {
    rms = sqrt( sumSquare / ( lastSample - firstSample ) );
  }
  else {
    rms = sqrt( sumSquare );
  }
  return rms;
}

//==============================

EXOATeamAPDReshaperModule::NoiseCorner::NoiseCorner()
{
  cornername = "";
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = 25;
  Init();
}

EXOATeamAPDReshaperModule::NoiseCorner::NoiseCorner(const string name)
{
  cornername = name;
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = 25;
  Init( );
}

EXOATeamAPDReshaperModule::NoiseCorner::NoiseCorner(const string name, int length)
{
  cornername = name;
  if (length > 25) {
    length = 25;
  }
  maxlength = length;
  Init();
}

EXOATeamAPDReshaperModule::NoiseCorner::~NoiseCorner()
{
  delete [] shapingTimes;
  delete [] rmsValues;
  delete [] rmsErrors;

  for (int i = 0; i < maxlength; i++){
    delete hPeakHeights[i];
  }
}

void EXOATeamAPDReshaperModule::NoiseCorner::Init(){

  numTimes = 0;
  NotCalculated = true;

  shapingTimes = new int[maxlength];
  rmsValues = new double[maxlength];
  rmsErrors = new double[maxlength];

  for (int i = 0; i < maxlength; i++) {
    shapingTimes[i] = 0;
    rmsValues[i] = 0.0;
    rmsErrors[i] = 0.0;
    stringstream int_to_string;
    int_to_string << i;
    string htitle = cornername + "hist_" + int_to_string.str();
    cout << htitle << endl;
    hPeakHeights[i] = new TH1F(htitle.c_str(), htitle.c_str(), 4096, 0, 4096 );

  }
}

int* EXOATeamAPDReshaperModule::NoiseCorner::GetShapingTimes(){

  int *temp = new int[numTimes];
  
  for (int i = 0; i < numTimes; i++) {
    temp[i] = shapingTimes[i];
  }
  return temp;
}

int EXOATeamAPDReshaperModule::NoiseCorner::GetShapingTime(int index){
  return shapingTimes[index];
}

void EXOATeamAPDReshaperModule::NoiseCorner::Calculate(){
  NotCalculated = false;
  
  TF1 *gaussfit = new TF1("gaussfit", "gaus");

  for (int i = 0; i < numTimes; i++){
    hPeakHeights[i]->Fit("gaussfit", "Q");
    rmsValues[i] = gaussfit->GetParameter( 2 );
    rmsErrors[i] = gaussfit->GetParError( 2 );
  }

  delete gaussfit;
}

double* EXOATeamAPDReshaperModule::NoiseCorner::GetrmsValues(){
  // calculates and then returns the rms Values of the histograms.
  // can include methods here in the future.

  if (NotCalculated){
    Calculate();
  }

  return rmsValues;
}

double EXOATeamAPDReshaperModule::NoiseCorner::GetrmsValue(int index){

  if (NotCalculated) {
    Calculate();
  }
  return rmsValues[index];
}

double* EXOATeamAPDReshaperModule::NoiseCorner::GetrmsErrors(){
  
  if (NotCalculated) {
    Calculate();
  }
  
  return rmsErrors;
}

double EXOATeamAPDReshaperModule::NoiseCorner::GetrmsError(int index){

  if (NotCalculated) {
    Calculate();
  }
  return rmsErrors[index];
}

int EXOATeamAPDReshaperModule::NoiseCorner::GetArrayLength(){
  return numTimes;
}

int EXOATeamAPDReshaperModule::NoiseCorner::GetBestTau(){
  int val = 0;
  double minimum = -1.0;  

  for (int i = 0; i < numTimes; i++){
    if (minimum == -1.0){
      minimum = GetrmsValue(i);
      val = GetShapingTime(i);
    }
    else if (minimum > GetrmsValue(i) ) {
      minimum = GetrmsValue(i);
      val = GetShapingTime(i);
    }
  }
  return val;
}

void EXOATeamAPDReshaperModule::NoiseCorner::Add(int tau, double peakHeight){
  int index = ArrayIndex( tau );
  NotCalculated = true;

  if (index == -1){
    // this shaping time does not yet have an index
    shapingTimes[ numTimes ] = tau;
    hPeakHeights[ numTimes ] ->Fill( peakHeight );
    numTimes++;
  }
  else {
    //this shaping time has already been indexed.  We don't need to fill tau, but we need to fill the histogram.
    hPeakHeights[ index ]->Fill( peakHeight );
  }
 
}

int EXOATeamAPDReshaperModule::NoiseCorner::ArrayIndex(int tau){
  // returns -1 if not in array, returns index if it is.
  int val = -1;
  for (int i = 0; i < numTimes; i++){
    if (tau == shapingTimes[i]){
      val = i;
      i = numTimes;
    }
  }
  return val;
}
//======================//

EXOATeamAPDReshaperModule::Filters::Filters(){
  filtername = "";
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = 25;
  numTTimes = int (maxlength/2.0);
  Init();
}

EXOATeamAPDReshaperModule::Filters::Filters(string name){
  filtername = name;
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = 25;
  numTTimes = int (maxlength/2.0);
  Init();
}

EXOATeamAPDReshaperModule::Filters::Filters(string name, int length){
  filtername = name;
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = length;
  numTTimes = int (maxlength / 2.0);
  Init();
}

// here name is the given name of the object, length is the number of shaping times that will be used
// and Tlength is the number of TInts that will be used for the finit cusp filter.
EXOATeamAPDReshaperModule::Filters::Filters(string name, int length, int Tlength){
  filtername = name;
  // here maxlength is the maximum number of shaping times for which information will be stored.
  maxlength = length;
  numTTimes = Tlength;
  Init();
}

EXOATeamAPDReshaperModule::Filters::~Filters(){
  
  delete triangular;
  delete gaussian;

  for (int i = 0; i < numTTimes; i++){
    delete finiteCusp[i];
  }
  delete tempval;
  delete tempval2;
}

void EXOATeamAPDReshaperModule::Filters::Init(){

  BestNotCalculated = true;

  triangular = new NoiseCorner("triangular_" + filtername, maxlength );
  gaussian = new NoiseCorner("gaussian_" + filtername, maxlength );
  
  for (int i = 0; i < numTTimes; i++){
    stringstream int_to_string;
    int_to_string << i;
    finiteCusp[i] = new NoiseCorner("finiteCusp_" + int_to_string.str() + "_" + filtername, maxlength);
  }
  
  tempval = new int[1];
  tempval2 = new double[1];

}

void EXOATeamAPDReshaperModule::Filters::Add(string filterType, int tau, double peakHeight){

  BestNotCalculated = true;

  if (filterType == "triangular"){
    triangular->Add(tau, peakHeight);
  }
  else if (filterType == "gaussian"){
    gaussian->Add(tau, peakHeight);
  }
    
}

void EXOATeamAPDReshaperModule::Filters::Add(string filterType, int tau, double T, double peakHeight){

  int Tindex = ArrayIndex( T );

  BestNotCalculated = true;

  if (filterType == "finiteCusp"){
    if (Tindex == -1){
      TTimes[numTTimes] = T;
      finiteCusp[numTTimes]->Add(tau, peakHeight);
      numTTimes++;
    }
    else{
      finiteCusp[numTTimes]->Add(tau, peakHeight);
    }
  }

}

int EXOATeamAPDReshaperModule::Filters::ArrayIndex(double T){
  int val = -1;
  
  for (int i = 0; i < numTTimes; i++) {
    if (TTimes[i] == T){
      val = i;
      i = numTTimes;
    }
  }

  return val;
}

int EXOATeamAPDReshaperModule::Filters::GetTArrayLength(){
  return numTTimes;
}

int EXOATeamAPDReshaperModule::Filters::GetArrayLength(string filterType){
  
  int val = 0;

  if (filterType == "triangular"){
    val = triangular->GetArrayLength();
  }
  else if (filterType == "gaussian"){
    val = gaussian->GetArrayLength();
  }
  else if (filterType == "finiteCusp"){
    val = finiteCusp[0]->GetArrayLength();
  }
  return val;
}

int EXOATeamAPDReshaperModule::Filters::GetArrayLength(string filterType, double T){
  int Tindex = ArrayIndex( T );
  int val = 0;
  

  if (filterType == "finiteCusp"){
    val = finiteCusp[Tindex]->GetArrayLength();
  }

  return val;
}

double EXOATeamAPDReshaperModule::Filters::GetBestT(){
  double minimum = -1.0;

  double val = 0.0;

  int bestindex = 0;

  if (numTTimes > 0){
    for (int i = 0; i < numTTimes; i++){
      if (minimum == -1.0){
	bestindex = i;
	minimum = finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() );
      }
      else {
	if (minimum > finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() )){
	  minimum = finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() );
	  bestindex = i;		      
	}
      }
    }
    val = TTimes[bestindex];
  }
  
    
  return val;
}

int EXOATeamAPDReshaperModule::Filters::GetBestTindex(){
  double minimum = -1.0;

  int bestindex = 0;

  for (int i = 0; i < numTTimes; i++){
    if (minimum == -1.0){
      bestindex = i;
      minimum = finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() );
    }
    else {
      if (minimum > finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() )){
	minimum = finiteCusp[i]->GetrmsValue( finiteCusp[i]->GetBestTau() );
	bestindex = i;		      
      }
    }
  }
    
  return bestindex;
}

string EXOATeamAPDReshaperModule::Filters::GetBestShaper(){
 
  if (BestNotCalculated){
    CalculateBest();
  }

  return bestfilter;
}

int EXOATeamAPDReshaperModule::Filters::GetBestShaperTau(){
  if (BestNotCalculated){
    CalculateBest();
  }

  return bestTau;
}

double EXOATeamAPDReshaperModule::Filters::GetBestShaperT(){
  double val = 0.0;
  if (BestNotCalculated){
    CalculateBest();
  }

  if (GetBestShaper() == "finiteCusp"){
    val = GetBestT();
  }
  
  return val;
}

void EXOATeamAPDReshaperModule::Filters::CalculateBest(){
  bestTau = 0;
  bestfilter = "";
  bestT = 0.0;

  BestNotCalculated = false;

  double minimum = 0.0;
  if (triangular->GetArrayLength() > 0) {
    bestTau = triangular->GetBestTau();
    minimum = triangular->GetrmsValue( bestTau );
    bestfilter = "triangular";
  }
    
  if (minimum > gaussian->GetrmsValue( gaussian->GetBestTau()) && gaussian->GetArrayLength() > 0 ){
    bestfilter = "gaussian";
    bestTau = gaussian->GetBestTau();
  }
  
  int BestTindex = GetBestTindex();
  if (minimum > finiteCusp[ BestTindex ]->GetrmsValue( finiteCusp[ BestTindex ]->GetBestTau() ) && numTTimes > 0 ){
    bestfilter = "finiteCusp";
    bestT = GetBestT();
    bestTau = finiteCusp[ BestTindex ]->GetBestTau();
  }
}

int* EXOATeamAPDReshaperModule::Filters::GetFilterShapingTimes(string filterType){



  if (filterType == "triangular"){
    return triangular->GetShapingTimes();
  }
  else if (filterType == "gaussian"){
    return gaussian->GetShapingTimes();
  }
  else if (filterType == "finiteCusp"){
    return finiteCusp[0]->GetShapingTimes();
  }

  return tempval;
}

int* EXOATeamAPDReshaperModule::Filters::GetFilterShapingTimes(string filterType, double T){



  if (filterType == "finiteCusp"){
    return finiteCusp[ ArrayIndex(T) ]->GetShapingTimes();
  }

  return tempval;
}

double* EXOATeamAPDReshaperModule::Filters::GetFilterrmsValues(string filterType){



  if (filterType == "triangular"){
    return triangular->GetrmsValues();
  }
  else if (filterType == "gaussian"){
    return gaussian->GetrmsValues();
  }
  else if (filterType == "finiteCusp"){
    return finiteCusp[0]->GetrmsValues();
  }

  return tempval2;
}

double* EXOATeamAPDReshaperModule::Filters::GetFilterrmsValues(string filterType, double T){



  if (filterType == "finiteCusp"){
    return finiteCusp[ ArrayIndex(T) ]->GetrmsValues();
  }

  return tempval2;
}

double* EXOATeamAPDReshaperModule::Filters::GetFilterrmsErrors(string filterType){



  if (filterType == "triangular"){
    return triangular->GetrmsErrors();
  }
  else if (filterType == "gaussian"){
    return gaussian->GetrmsErrors();
  }
  else if (filterType == "finiteCusp"){
    return finiteCusp[0]->GetrmsErrors();
  }

  return tempval2;
}

double* EXOATeamAPDReshaperModule::Filters::GetFilterrmsErrors(string filterType, double T){



  if (filterType == "finiteCusp"){
    return finiteCusp[ ArrayIndex(T) ]->GetrmsErrors();
  }

  return tempval2;
}

EXOATeamAPDReshaperModule::APDNoiseCorners::APDNoiseCorners(string name, int length, int Tlength){
  
  noisename = name;

  for (int i = 0; i < 74; i++){
    channels[i] = new Filters(Form("%s_ch%d", noisename.c_str(), i), length, Tlength); 
  }
}

EXOATeamAPDReshaperModule::APDNoiseCorners::~APDNoiseCorners(){

  for (int i = 0; i < 74; i++){
    delete channels[i];
  }
}

void EXOATeamAPDReshaperModule::APDNoiseCorners::Add(int ch, string filterType, int tau, double peakHeight){
  if (filterType == "triangular" || filterType == "gaussian"){
    channels[ch]->Add(filterType, tau, peakHeight);
  }
}

void EXOATeamAPDReshaperModule::APDNoiseCorners::Add(int ch, string filterType, int tau, double T, double peakHeight){
  if (filterType == "finiteCusp"){
    channels[ch]->Add(filterType, tau, T, peakHeight);
  }
}

int EXOATeamAPDReshaperModule::APDNoiseCorners::GetTArrayLength(int ch){
  return channels[ch]->GetTArrayLength();
}

int EXOATeamAPDReshaperModule::APDNoiseCorners::GetArrayLength(int ch, string filterType){
  
  return channels[ch]->GetArrayLength(filterType);
}

int EXOATeamAPDReshaperModule::APDNoiseCorners::GetArrayLength(int ch, string filterType, double T){
  
  return channels[ch]->GetArrayLength(filterType, T);
}

double EXOATeamAPDReshaperModule::APDNoiseCorners::GetBestT(int ch){

  return channels[ch]->GetBestT();
}

int EXOATeamAPDReshaperModule::APDNoiseCorners::GetBestTindex(int ch){

  return channels[ch]->GetBestTindex();
}

string EXOATeamAPDReshaperModule::APDNoiseCorners::GetBestShaper(int ch){
  
  return channels[ch]->GetBestShaper();
}

int EXOATeamAPDReshaperModule::APDNoiseCorners::GetBestShaperTau(int ch){

  return channels[ch]->GetBestShaperTau();
}

double EXOATeamAPDReshaperModule::APDNoiseCorners::GetBestShaperT(int ch){

  return channels[ch]->GetBestShaperT();
}

int* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterShapingTimes(int ch, string filterType){
  
  return channels[ch]->GetFilterShapingTimes(filterType);
}

int* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterShapingTimes(int ch, string filterType, double T){

  return channels[ch]->GetFilterShapingTimes(filterType, T);
}

double* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterrmsValues(int ch, string filterType){

  return channels[ch]->GetFilterrmsValues(filterType);
}

double* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterrmsValues(int ch, string filterType, double T){

  return channels[ch]->GetFilterrmsValues(filterType);
}

double* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterrmsErrors(int ch, string filterType){

  return channels[ch]->GetFilterrmsErrors(filterType);
}

double* EXOATeamAPDReshaperModule::APDNoiseCorners::GetFilterrmsErrors(int ch, string filterType, double T){

  return channels[ch]->GetFilterrmsErrors(filterType, T);
}

