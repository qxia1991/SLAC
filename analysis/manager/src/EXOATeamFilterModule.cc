//______________________________________________________________________________
// Created 02-10-11
// Russell Neilson
// Last Modified 02-10-11
// Russell neilson
//******************************
// This module applies a filter to the APD channels pre-reconstruction, by
// setting real and imaginary parts of the FFT to zero for known noisy
// frequencies.
// ====================================================================================
// Changelog:
// 02-10-11:  Created this changelog
// ====================================================================================== 


#include "EXOAnalysisManager/EXOATeamFilterModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOATeamFilterModule, "afilter" )

EXOATeamFilterModule::EXOATeamFilterModule()
{

  //  cout << "constructing EXOExampleModule with name " << NAME << endl;

}

int EXOATeamFilterModule::Initialize()
{

  //  cout << "Initializing " << name << endl;

  // Make sure we've got an error logger and an analysis manager

  return 0;
}

EXOAnalysisModule::EventStatus EXOATeamFilterModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  return kOk;
}

EXOAnalysisModule::EventStatus EXOATeamFilterModule::ProcessEvent(EXOEventData *ED)
{


  // MGM fix to put waveform variable on the heap, avoiding 
  // gcc errors regarding negative indices.
  Float_t *waveform = new Float_t[2048];
  Int_t APDchannel0 = NWIREPLANE*NCHANNEL_PER_WIREPLANE;
  Int_t nsample = ED->GetWaveformData()->fNumSamples;
  if (nsample == 2048) {
    
    for (size_t k=0; k < ED->GetWaveformData()->GetNumWaveforms(); k++) {
      EXOWaveform& wf = *ED->GetWaveformData()->GetWaveformToEdit(k); 
      if (wf.fChannel >= APDchannel0) {
	
	for (int i=0; i<nsample; i++) {
	  waveform[i] = wf[i];
	}
	
	// take fft
	EXOMiscUtil::realft(waveform-1,2048,1);
	
	// remove known noise frequencies
	
	for (int i = 400; i<410; i++) {
	  waveform[i] = 0;
	}
	for (int i = 802; i<820; i++) {
	  waveform[i] = 0;
	}
	for (int i = 648; i<654; i++) {
	  waveform[i] = 0;
	}
	for (int i = 774; i<780; i++) {
	  waveform[i] = 0;
	}
	for (int i = 706; i<714; i++) {
	  waveform[i] = 0;
	}
	for (int i = 1412; i<1428; i++) {
	  waveform[i] = 0;
	}
	
	// do the inverse transform
	EXOMiscUtil::realft(waveform-1,2048,-1);
	
	// Digitize and load in to event data 
	for (int i = 0; i < nsample; i++) {
	  wf[i] = (Int_t)(2.0/2048*waveform[i]+0.5);
	}
      }
    }
  }
  delete [] waveform;
  return kOk;
}

EXOAnalysisModule::EventStatus EXOATeamFilterModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}

int EXOATeamFilterModule::TalkTo(EXOTalkToManager *talktoManager)
{


  return 0;
}

int EXOATeamFilterModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}




