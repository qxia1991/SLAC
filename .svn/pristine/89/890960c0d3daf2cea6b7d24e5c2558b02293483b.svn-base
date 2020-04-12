//______________________________________________________________________________
// Module Documentation: EXORealNoiseModule
// Created by: P. S. Barbeau
// 6/7/2011
// Last Modified 3/27/2014
//
// 2014/3/24 DCM: Read from random starting index in noise file to ensure all 
//                traces are sampled evenly
// 2014/3/27 DCM: Fix to use TFile::Open to allow accessing remote files
// 2015/4/14 DCM: Add option to skip adding noise for APDs, U-wires, or V-wires
//
// Purpose: Allows to generate root files composed of only noise traces 
// (solicited triggeres) and then use this noise file as the source of
// noise, to be added to simulation.  The generation occurs using the filter
// command in combination with the tree output module.
// The incorporation should use a significant number of noise 
// traces ~1000.  All waveforms for all channels are used.
// What this means is that true correlated and environmental noise is
// incorporated in the simulation.  ---PSB
// Note: Solicited triggers induce signals through the TEM. So, even the noise traces
// selected by this module are note truly signal-free. --- Clayton, from email with Phil
//______________________________________________________________________________

#include "EXOAnalysisManager/EXORealNoiseModule.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXORealNoiseModule, "realnoise" )

EXORealNoiseModule::EXORealNoiseModule()
: fMakeNoiseFile(false),
  fUseNoiseFile(false),
  fNumRandomTrigs(0),
  fNumNoiseTraces(0),
  fNoiseFilenameParam("noisefile.root"),
  fOffset(1600),
  fNoiseIndex(0),
  fRequestedLength(2048),
  fSkipAPDs(false),
  fSkipVWires(false),
  fSkipUWires(false),
  fNoiseFile(NULL),
  fNoiseTree(NULL),
  fNoiseEventData(NULL)
{

}

EXORealNoiseModule::~EXORealNoiseModule()
{
  if(fNoiseFile) {
    fNoiseFile->Close();
    delete fNoiseFile;
  }
}

EXOAnalysisModule::EventStatus EXORealNoiseModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  if(fUseNoiseFile and fMakeNoiseFile){
    LogEXOMsg("Cannot produce AND use noise file in the same run",EEAlert);
  }
  if(not (fUseNoiseFile or fMakeNoiseFile)){
    LogEXOMsg("Set whether to use or to produce noise file!",EEAlert);
  }
  if (fUseNoiseFile) {
    fNoiseFile = TFile::Open(fNoiseFilenameParam.c_str());
    fNoiseTree = dynamic_cast<TTree*>(fNoiseFile->Get("tree"));
    if(not fNoiseTree){
      LogEXOMsg("Could not find tree with name \"tree\" in file "+fNoiseFilenameParam,EEAlert);
    }
    fNoiseTree->SetBranchAddress("EventBranch", &fNoiseEventData);
    fNumNoiseTraces = fNoiseTree->GetEntries();
    cout << "Number of triggers in noise file to be used: " << fNumNoiseTraces << endl;

    // initialize noise index to start reading from random index
    // in file, to ensure we fully sample all traces
    TRandom3 *r = new TRandom3(ED->fRunNumber);
    fNoiseIndex = (int)(r->Rndm() * fNumNoiseTraces);
  }
  
  return kOk;
}

EXOAnalysisModule::EventStatus EXORealNoiseModule::ProcessEvent(EXOEventData *ED)
{

  if (fMakeNoiseFile){
    if( not (ED->fEventHeader.fIndividualTriggerRequest == 0 and 
             ED->fEventHeader.fSumTriggerRequest == 0)){
      return kDrop;
    }
    bool veto = false;
    if( ED->GetWaveformData()->fNumSamples != fRequestedLength){ 
      stringstream lstream;
      lstream << "Waveform length does not match requested length (";
      lstream << fRequestedLength;
      lstream << ") Skipping event.";
      LogEXOMsg(lstream.str(),EEWarning);
    }
    fNumRandomTrigs++;

    EXOWaveformData* wf_data = ED->GetWaveformData();
    double length = wf_data->fNumSamples;
    for(size_t i=0; i < wf_data->GetNumWaveforms(); i++){
      EXOWaveform& wf = *wf_data->GetWaveformToEdit(i);
      int baseline = int(double(wf.Sum())/double(length));
      //we need to subtract an offset from the baseline (and add it
      //again when adding the noise waveform to the simulated one)
      //because the waveforms (when compressed) are unsigned integers
      wf -= (baseline - fOffset);
    }

    return kOk;
  }

  if (fUseNoiseFile){
    //Get the right event from the noise file
    fNoiseTree->GetEntry(fNoiseIndex);
    //Increment fNoiseIndex, modulo the total number of triggers in the file.
    //Thus we loop around back to the beginning.
    fNoiseIndex++;
    if (fNoiseIndex >= fNumNoiseTraces){
      fNoiseIndex = 0;
    }
    
    //Get the waveform data object
    EXOWaveformData* noise_wf_data = fNoiseEventData->GetWaveformData();
    noise_wf_data->Decompress();
    EXOWaveformData* wf_data = ED->GetWaveformData();
    int length = wf_data->fNumSamples;
    
    if ((wf_data->GetNumWaveforms() != noise_wf_data->GetNumWaveforms() ) or (length != noise_wf_data->fNumSamples) ){
      LogEXOMsg("noise file and simulation incompatible for this event",EEWarning);
      return kDrop;
    }
    for (unsigned i=0; i < wf_data->GetNumWaveforms(); i++){
      EXOWaveform& wf = *wf_data->GetWaveformToEdit(i);

      // Allow user to skip APDs or wire channels individually (in case noise
      // has already been added in the digitizer for a given channel type)
      if( fSkipAPDs && EXOMiscUtil::TypeOfChannel(wf.fChannel)==EXOMiscUtil::kAPDGang ) continue;
      if( fSkipVWires && EXOMiscUtil::TypeOfChannel(wf.fChannel)==EXOMiscUtil::kVWire ) continue;
      if( fSkipUWires && EXOMiscUtil::TypeOfChannel(wf.fChannel)==EXOMiscUtil::kUWire ) continue;

      // must index by channel -- cannot assume indices the same between noise file and data
      const EXOWaveform& noise_wf = *noise_wf_data->GetWaveformWithChannel(wf.fChannel);
      wf += noise_wf;
      wf -= fOffset;
        
      //Starting around run 8850/8851 we lost channel VCh-119.
      //Time stamp for first event in 8850 is 1516115373.  If we ever get a Noise WF after this 
      //just kill the Signal on this channel to ensure we don't add any isues
      if (fNoiseEventData->fEventHeader.fTriggerSeconds > 1516115373 and wf.fChannel==119){
        wf *= 0.0;
        wf += noise_wf;
      }

    }
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXORealNoiseModule::EndOfRun(EXOEventData *ED)
{
  cout << "At EndOfRun for " << GetName() << endl;
  if (fMakeNoiseFile == true) {
    cout << "Number of Random triggers in file " << fNumRandomTrigs << endl;
  }

  return kOk;
}

int EXORealNoiseModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/realnoise/makeNoiseFile","read in a bool", 
           this, fMakeNoiseFile, &EXORealNoiseModule::SetMakeNoiseFile );

  talktoManager->CreateCommand("/realnoise/useNoiseFile","read in a bool", 
           this, fUseNoiseFile, &EXORealNoiseModule::SetUseNoiseFile );

  talktoManager->CreateCommand("/realnoise/NoiseFile", "choose noise input file",
           this, fNoiseFilenameParam, &EXORealNoiseModule::SetNoiseFilename);

  talktoManager->CreateCommand("/realnoise/WaveformLength", "read in int",
           this, fRequestedLength, &EXORealNoiseModule::SetRequestedLength);

  talktoManager->CreateCommand("/realnoise/skipAPDs","Skip APD channels when adding noise", 
           this, fSkipAPDs, &EXORealNoiseModule::SetSkipAPDs );

  talktoManager->CreateCommand("/realnoise/skipVWires","Skip V-wire channels when adding noise", 
           this, fSkipVWires, &EXORealNoiseModule::SetSkipVWires );

  talktoManager->CreateCommand("/realnoise/skipUWires","Skip U-wire channels when adding noise", 
           this, fSkipUWires, &EXORealNoiseModule::SetSkipUWires );

  return 0;
}

void EXORealNoiseModule::SetMakeNoiseFile(bool aval)
{
  fMakeNoiseFile = aval;
  SetFiltered(aval);
}
