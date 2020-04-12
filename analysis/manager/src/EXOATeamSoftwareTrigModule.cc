//______________________________________________________________________________
#include "EXOAnalysisManager/EXOATeamSoftwareTrigModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOATeamSoftwareTrigModule, "asofttrig" )

EXOATeamSoftwareTrigModule::EXOATeamSoftwareTrigModule() :
  threshold(60.0)
{ }

EXOATeamSoftwareTrigModule::EventStatus EXOATeamSoftwareTrigModule::ProcessEvent(EXOEventData *ED)
{

  int above_threshold = 0;

  int trigger = ED->fEventHeader.fTriggerOffset;
  int nch = ED->GetWaveformData()->GetNumWaveforms(); 

  //loop over all u-wire channels
  for ( int j = 0; j < nch; j++ ) {

    const EXOWaveform& wf = *ED->GetWaveformData()->GetWaveform(j);
    Int_t ch = wf.fChannel;
    if ( ch < 0 ) continue;
    if ( ch >= NWIREPLANE*NWIRE_PER_PLANE ) continue;
    if ( (ch/NWIRE_PER_PLANE)%2 != 0 ) continue;

    //fill wfm with waveform data
    double wfm[wf.GetLength()];

    for (size_t i = 0; i < wf.GetLength(); i++) {
      wfm[i] = wf[i]; 
    }

    //calculate amplitude
    double amplitude = Amplitude(wfm, wf.GetLength(), trigger );

    if (amplitude > threshold) above_threshold = 1;
  }

  if (above_threshold == 0) return kDrop;
  return kOk;
}

int EXOATeamSoftwareTrigModule::TalkTo(EXOTalkToManager *talktoManager)
{



  talktoManager->CreateCommand("/asofttrig/threshold","trigger threshold for software trigger", 
                               this, threshold, &EXOATeamSoftwareTrigModule::Setthreshold );

  return 0;
}

// From pretrace time
double EXOATeamSoftwareTrigModule::Baseline(double signal[], int length, double trigger) {
  
  if (trigger > length) {
    LogEXOMsg( "trigger index out of bounds", EECritical);
    return -1.0;
  }
  
  double sum = 0;
  int PreTraceBuffer = 25;
  int BaseLineLength = 500;
  
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

double EXOATeamSoftwareTrigModule::Max(double signal[], int length) {

  double max = signal[0];
  for (int i = 1; i < length; i++) {
    if (signal[i] > max) max = signal[i];
  }

  return max;
}


double EXOATeamSoftwareTrigModule::Min(double signal[], int length) {
   
   double min = signal[0];
   for (int i = 1; i < length; i++) {
     if (signal[i] < min) min = signal[i];
   }

   return min;
}


double EXOATeamSoftwareTrigModule::Amplitude(double signal[], int length, double trigger) {
 
  return fabs(Max(signal, length) - Baseline(signal, length, trigger));
}
