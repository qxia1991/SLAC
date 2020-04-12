//______________________________________________________________________________
#include "EXOAnalysisManager/EXOCrossCorrelationModule.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOFastFourierTransformFFTW.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TH1.h"
#include "TMath.h"
#include "TFile.h"
#include <iostream>
using namespace std;
using namespace TMath;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOCrossCorrelationModule, "crosscorr" )

EXOCrossCorrelationModule::EXOCrossCorrelationModule()
: fMaxEventNumber(99),
  fMinEventNumber(0),
  fMinchannel(NCHANNEL_PER_WIREPLANE*NWIREPLANE),
  fMaxchannel(fMinchannel + NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE),
  fNSamples(2048),
  fEventCounter(0),
  fFilename("Correlations.root"),
  fFile(NULL)
{

}

EXOCrossCorrelationModule::~EXOCrossCorrelationModule()
{
  delete fFile;
}

int EXOCrossCorrelationModule::Initialize()
{
  fFile = new TFile(fFilename.c_str(),"RECREATE");
  for(int channel1 = fMinchannel; channel1 < fMaxchannel; channel1++){
    for(int channel2 = channel1+1; channel2 < fMaxchannel; channel2++){
      EXOWaveformFT& corrwf = fCollection.GetCorrelation(channel1,channel2);
      corrwf.SetLength(fNSamples/2+1);
      corrwf.SetSamplingFreq(CLHEP::megahertz);
      corrwf.Zero();
    }
  }
  return 0;
}

EXOAnalysisModule::EventStatus EXOCrossCorrelationModule::BeginOfRun(EXOEventData *ED)
{
  return kOk;
}

EXOAnalysisModule::EventStatus EXOCrossCorrelationModule::ProcessEvent(EXOEventData *ED)
{
  if(ED->fEventNumber < fMinEventNumber || ED->fEventNumber > fMaxEventNumber){
    return kOk;
  }
  //Only look at random trigger data
  if(ED->fEventHeader.fSumTriggerRequest || ED->fEventHeader.fIndividualTriggerRequest){
    return kOk;
  }
  fEventCounter++;
  EXOFastFourierTransformFFTW& fft = EXOFastFourierTransformFFTW::GetFFT(fNSamples);
  int N = fNSamples/2+1;
  for(int channel1 = fMinchannel; channel1 < fMaxchannel; channel1++){
    EXODoubleWaveform wholeWF1 = ED->GetWaveformData()->GetWaveformWithChannel(channel1)->Convert < Double_t >();
    EXODoubleWaveform wf1 = wholeWF1.SubWaveform(0,fNSamples);
    //cout << "Length of waveform1: " << wf1.GetLength() << endl;
    //cout << "Frequency of waveform1: " << wf1.GetSamplingFreq() << endl;
    if(wf1.GetLength() < fNSamples){
      LogEXOMsg("Waveform is shorter than used number of samples. Normalization might be wrong!",EEError);
      continue;
    }
    EXOWaveformFT wf1ft;
    wf1ft.SetLength(N);
    fft.PerformFFT(wf1,wf1ft);

    for(int channel2 = channel1+1; channel2 < fMaxchannel; channel2++){
      EXODoubleWaveform wholeWF2 = ED->GetWaveformData()->GetWaveformWithChannel(channel2)->Convert < Double_t >();
      EXODoubleWaveform wf2 = wholeWF2.SubWaveform(0,fNSamples);
      //cout << "Length of waveform2: " << wf1.GetLength() << endl;
      //cout << "Frequency of waveform2: " << wf2.GetSamplingFreq() << endl;
      if(wf2.GetLength() < fNSamples){
        LogEXOMsg("Waveform is shorter than used number of samples. Normalization might be wrong!",EEError);
        continue;
      }
      EXOWaveformFT wf2ft;
      wf2ft.SetLength(N);
      fft.PerformFFT(wf2,wf2ft);

      EXOWaveformFT wf12ft;
      fCrossProduct.Transform(wf1ft,wf2ft,wf12ft);
      //cout << "collection length: " << fCollection.GetCorrelation(channel1,channel2).GetLength() << endl;
      //cout << "collection frequ: " << fCollection.GetCorrelation(channel1,channel2).GetSamplingFreq() << endl;
      //cout << endl;
      fCollection.GetCorrelation(channel1,channel2) += wf12ft;
    }
  }
  return kOk;
}

EXOAnalysisModule::EventStatus EXOCrossCorrelationModule::EndOfRun(EXOEventData *ED)
{
  return kOk;
}

int EXOCrossCorrelationModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/crosscorr/maxevent","set the maximum event number",
           this, fMaxEventNumber, &EXOCrossCorrelationModule::SetMaxEventNumber);

  talktoManager->CreateCommand("/crosscorr/minevent","set the event number with which to start",
           this, fMinEventNumber, &EXOCrossCorrelationModule::SetMinEventNumber);

  talktoManager->CreateCommand("/crosscorr/nsamples","set the number of samples of a waveform that are used (starting at the beginning)",
           this, fNSamples, &EXOCrossCorrelationModule::SetNumSamples);

  talktoManager->CreateCommand("/crosscorr/file","set the filename",
           this, fFilename, &EXOCrossCorrelationModule::SetFilename);

  return 0;
}

int EXOCrossCorrelationModule::ShutDown()
{
  fCollection.Normalize(fEventCounter);
  /*
  for(int channel1 = fMinchannel; channel1 < fMaxchannel; channel1++){
    for(int channel2 = channel1+1; channel2 < fMaxchannel; channel2++){
      TH1* plot = fCollection.GetCorrelation(channel1,channel2).GimmeHist("","Abs");
      EXOMiscUtil::DisplayInProgram(*plot,"");
      delete plot;
    }
  }
  */
  fFile->cd();
  complex<double> temp(1,1);
  fCollection.Write("fCollection",TObject::kOverwrite);
  fFile->Close();
  delete fFile;
  fFile = NULL;

  return 0;
}

