//______________________________________________________________________________
// Created: 01-13-10
// Last Modified: 01-22-10
// Nicole Ackerman
//****************************** 
// This module simulates the trigger
// and fills the trigger parameters in
// the EXO event data.
//==================================================
// Changelog:
// 1-22-10: Created - much of code is based on 
//          EXOExampleModule and the trigger
//          code that was in geant.
//==================================================

#include "EXOAnalysisManager/EXOTriggerModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "TH1D.h"
#include "TF1.h"
#include "TList.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
using std::cout;
using std::endl;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOTriggerModule, "trig" )

EXOTriggerModule::EXOTriggerModule() :
  fAPDTrigger(&(fConfigReader.fAPDConfig)),
  fVGDTrigger(&(fConfigReader.fVGDConfig)), 
  fHVDTrigger(&(fConfigReader.fHVDConfig)),
  fOverwrite(true),
  fPrevtrig(false),
  fVerbose(0),
  fConfigFile("physics-trigger.xml"),
  fOffsetsFile("baseline-offsets.xml")
{

}

int EXOTriggerModule::Initialize()
{
  std::cout << "EXOTriggerModule: getting physics trigger configuration from " << fConfigFile << std::endl;
  std::cout << "EXOTriggerModule: getting baseline offsets from " << fOffsetsFile << std::endl;
  if(fConfigReader.OpenAndParse(fConfigFile)) return -1;
  if(fConfigReader.FillConfiguration()) return -1;
  if(fConfigReader.OpenAndParse(fOffsetsFile)) return -1;
  if(fConfigReader.FillConfiguration()) return -1;

  fConfigReader.fAPDConfig.PrintConfig();
  fConfigReader.fVGDConfig.PrintConfig();
  fConfigReader.fHVDConfig.PrintConfig();

  //Now need to set up Triggers
  fAPDTrigger.Initialize();
  fVGDTrigger.Initialize();
  fHVDTrigger.Initialize();

  return 0;
}

EXOAnalysisModule::EventStatus EXOTriggerModule::BeginOfRun(EXOEventData *ED)
{
  if(fConfigReader.CheckConsistency()){
    return kOk;
  }
  return kDrop;
}

EXOAnalysisModule::EventStatus EXOTriggerModule::ProcessEvent(EXOEventData *ED)
{

  const EXOChannelMap& chanMap = GetChanMapForHeader(ED->fEventHeader);
  fAPDTrigger.SetChannelMap(chanMap);
  fVGDTrigger.SetChannelMap(chanMap);
  fHVDTrigger.SetChannelMap(chanMap);
  fAPDTrigger.Reset();
  fVGDTrigger.Reset();
  fHVDTrigger.Reset();

  //want to loop over the channels for THIS SYSTEM
  //0-37: u-wires for the +z half of the detector
  //38-75: v-wires for the +z half
  //76-113: u-wires for the -z half
  //114-151: v-wires for the -z half
  //152-188: APDs in the +z half
  //189-225: APDs in the -z half
  //226: summed APD signal for the +z half
  //227: summed APD signal for the -z half

  //Loop overall possible channels
  fAPD_IndividualToTotalChannel.clear();
  fVGD_IndividualToTotalChannel.clear();
  fHVD_IndividualToTotalChannel.clear();

  size_t apdindex = 0, vgdindex = 0, hvdindex = 0;
  size_t nwaveforms = ED->GetWaveformData()->GetNumWaveforms();
  for(size_t totchannelindex = 0; totchannelindex < nwaveforms; totchannelindex++){
    int channel = ED->GetWaveformData()->GetWaveform(totchannelindex)->fChannel;
    if(channel<-2){
      std::cout << "EXOTriggerModule: ERROR: Channel " << channel << " not recognized" << std::endl;
      return kDrop;
    }
    else if(channel<0){
      //if it is channel -1 or -2 it is the
      // SUMMED APD CHANNEL
      //actually ignoring this right now
    }
    else if(channel<38){ //U WIRES(1)
      fVGD_IndividualToTotalChannel.insert(std::pair<int,int>(vgdindex,totchannelindex));
      vgdindex++;
    }
    else if(channel<76){ //V WIRES (2)
      fHVD_IndividualToTotalChannel.insert(std::pair<int,int>(hvdindex,totchannelindex));
      hvdindex++;
    }
    else if(channel<114){ //U WIRES (1)
      fVGD_IndividualToTotalChannel.insert(std::pair<int,int>(vgdindex,totchannelindex));
      vgdindex++;
    }
    else if(channel<152){ //V WIRES (2)
      fHVD_IndividualToTotalChannel.insert(std::pair<int,int>(hvdindex,totchannelindex));
      hvdindex++;
    }
    else if(channel<226){ //APD (0)
      fAPD_IndividualToTotalChannel.insert(std::pair<int,int>(apdindex,totchannelindex));
      apdindex++;
    }
    else{
      std::cout << "EXOTriggerModule: ERROR: Channel " << channel << " not recognized" << std::endl;
      return kDrop;
    }

  }//ends loop to ED->GetWaveformData()->GetNumWaveforms()

  //Need to see if the event has evidence that a trigger has been run before
  // the bool val is fPrevtrig
  if((ED->fEventHeader.fTriggerSource!=0)&&fPrevtrig==0){
    cout << "******************************" << endl;
    cout << "TRIGGER HAS BEEN RUN BEFORE" << endl;
    if(fOverwrite){
      cout << "TRIGGER WILL BE RE-RUN" << endl;
    }
    else{
      cout << "trigger WILL NOT run" << endl;
    }
    cout << "******************************" << endl;
    fPrevtrig=true;
  }

  GetTriggerInfo(ED);

  return kOk;
}

int EXOTriggerModule::GetTriggerInfo(EXOEventData *ED)
{

  bool trigflag = false; //if currently in trigger
  // Want to make sure no old trigger information remains
  ED->fEventHeader.fSumTriggerRequest=0;
  ED->fEventHeader.fIndividualTriggerRequest=0;
  ED->fEventHeader.fSumTriggerThreshold = 0;
  ED->fEventHeader.fSumTriggerValue = 0;
  ED->fEventHeader.fIndividualTriggerThreshold = 0;
  ED->fEventHeader.fMaxValueChannel = 0;
  ED->fEventHeader.fTriggerOffset = 0;
  ED->fEventHeader.fTriggerSource = 0;

  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  //Look at frame
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  int nsample = ED->GetWaveformData()->fNumSamples;
  EXOTrigger* pTrig = NULL;
  int triggered = 0; 

  const EXOWaveformData& wf_data = *(ED->GetWaveformData());

  //Loop through samples
  for ( int samp=0; samp<nsample; samp++)
  {
    if(triggered)
    {
      //A trigger has already occured
      //Saves on time and only records the first trigger
      break;	 
    }
    //Work forwards since trigger only done if not previously asserted
    for (int sys=0; sys<3; sys++)
    {  
      System trigsys;
      if(sys==kAPD){
        trigsys = kAPD;
        pTrig=&fAPDTrigger;
      }
      else if(sys==kVGD){
        trigsys = kVGD;
        pTrig=&fVGDTrigger;
      }
      else if(sys==kHVD){
        trigsys = kHVD;
        pTrig=&fHVDTrigger;
      }
      else return -1;

      size_t nchannels = GetNChannels(trigsys);
      //Create the array of samples to pass to the Trigger
      std::vector<int> sysframe;
      std::vector<int> channels;
      for(size_t channel=0; channel < nchannels; channel++) 
      {
        const EXOWaveform& wf = *wf_data.GetWaveform(GetTotalChannelIndex(channel,trigsys));
        sysframe.push_back(wf[samp]);
        channels.push_back(wf.fChannel);
      } // end loop over digitized data 	  

      //Trigger returns whether or not a trigger is active
      trigflag = pTrig->Trigger(sysframe,channels,trigflag); 

      if(trigflag)
      {
        ED->fEventHeader.fTriggerOffset = samp;
        ED->fEventHeader.fTriggerSource = trigsys+1;
        triggered = true;
        if(pTrig->SumTriggered() && !(pTrig->IndividualTriggered())){
          ED->fEventHeader.fSumTriggerThreshold = pTrig->GetWhichSumTriggerPassed();
          ED->fEventHeader.fSumTriggerValue = pTrig->GetChannelSum();
          ED->fEventHeader.fSumTriggerRequest = 1;
          if(fVerbose){
            int thresh = pTrig->GetPassedSumThreshold();
            cout << "Sum trigger triggered. Threshold of " << thresh << " exceeded by sum: " << pTrig->GetChannelSum() << ", background: " << pTrig->GetBackgroundAverage() << endl;
          }
        }
        else if(pTrig->IndividualTriggered() && !(pTrig->SumTriggered())){
          ED->fEventHeader.fIndividualTriggerThreshold = pTrig->GetWhichIndivTriggerPassed();
          ED->fEventHeader.fMaxValueChannel = pTrig->GetMaxChannel();
          ED->fEventHeader.fIndividualTriggerRequest = 1;
          if(fVerbose){
            int thresh = pTrig->GetPassedIndivThreshold() + pTrig->GetOffset(pTrig->GetMaxChannel());
            cout << "Individual trigger triggered. Threshold of " << thresh << " exceeded by channel " << pTrig->GetMaxChannel() << ", value: " << pTrig->GetMaxChannelValue() << endl;
            if(fVerbose > 1 && fVerbose != 3){
              PrintIndividualWaveform(ED,thresh,pTrig->GetMaxChannel());
            }
          }
        }
      }
    } //ends loop over the trigger systems
  } // end loop over digitized data (over the samples)   

  if(!triggered){
    if(fVerbose > 2){
      cout << "Trigger did NOT trigger!" << endl;
      for(int sys=0; sys<3; sys++){
        if(sys==kAPD){
          pTrig=&fAPDTrigger;
        }
        else if(sys==kVGD){
          pTrig=&fVGDTrigger;
        }
        else if(sys==kHVD){
          pTrig=&fHVDTrigger;
        }
        if(pTrig->GetNSumThresholds() > 0){
          int thresh = pTrig->GetLowestSumThreshold();
	  if(fVerbose){
	    cout << "Lowest sum trigger was " << thresh << endl;
	  }
        }
        if(pTrig->GetNIndividualThresholds() > 0){
          int thresh = pTrig->GetLowestIndivThreshold() + pTrig->GetOffset(pTrig->GetMaxChannel());
	  if(fVerbose) {
	    cout << "Lowest individual trigger was " << thresh << endl;
	    PrintIndividualWaveform(ED,thresh,pTrig->GetMaxChannel());
	  }
        }
      }
    }
  }

  //******************************
  //Now that the triggering is done
  //Reset the trigger systems
  //******************************
  fAPDTrigger.Reset();
  fVGDTrigger.Reset();
  fHVDTrigger.Reset();

  return !(ED->fEventHeader.fIndividualTriggerRequest+ED->fEventHeader.fSumTriggerRequest);
}

int EXOTriggerModule::TalkTo(EXOTalkToManager *talktoManager)
{

  // Will want:
  //  * Overwrite existing trigger data?
  //  * Where to get the trigger file from?
  //  * Print trigger information?

  talktoManager->CreateCommand("/trig/overwrite","record new trigger over al trigger info", this,fOverwrite,&EXOTriggerModule::SetOverwrite);

  talktoManager->CreateCommand("/trig/config","trigger configuration file",this,fConfigFile,&EXOTriggerModule::SetConfigFile);

  talktoManager->CreateCommand("/trig/offset","baseline offset file",this,fOffsetsFile,&EXOTriggerModule::SetOffsetsFile);

  talktoManager->CreateCommand("/trig/verbose","verbosity level",this,fVerbose,&EXOTriggerModule::SetVerbosity);
  
  return 0;
}

int EXOTriggerModule::ShutDown()
{

  //Want print out info here.
  cout << endl;
  cout << "*==============================*" << endl;
  cout << "*    INDIVIDUAL TRIGGERS       *" << endl;
  cout << "*==============================*" << endl;
  cout << std::setw(8) << "| THRESH |" << std::setw(6) << "APD" << std::setw(6) <<  "VG" << std::setw(7) << "HV" <<  std::setw(3) << "|" << endl;
  for(int a=0; a<4;a++)
    {
      cout << "|" << std::setw(5) << a << "   |";
//      cout << std::setw(5) << fAPDTrigger.GetHowManyIndivPasses(a);
//      cout << std::setw(5) << fVGDTrigger.GetHowManyIndivPasses(a);
//      cout << std::setw(5) << fHVDTrigger.GetHowManyIndivPasses(a);
      cout << "|" << endl;
    }
  cout << "*==============================*" << endl;
  cout << "*        SUM TRIGGERS          *" << endl;
  cout << "*==============================*" << endl;
  cout << std::setw(8) << "| THRESH |" << std::setw(6) << "APD" << std::setw(6) <<  "VG" << std::setw(7) << "HV" <<  std::setw(3) << "|" << endl;
  for(int a=0; a<4;a++)
    {
      cout << "|" << std::setw(5) << a << "   |";
//      cout << std::setw(5) << fAPDTrigger.GetHowManySumPasses(a);
//      cout << std::setw(5) << fVGDTrigger.GetHowManySumPasses(a);
//      cout << std::setw(5) << fHVDTrigger.GetHowManySumPasses(a);
      cout << "|" << endl;
      cout << "|" << endl;
    }
  cout << "*==============================*" << endl;
  return 0;
}

int EXOTriggerModule::GetTotalChannelIndex(int individualchannelindex, EXOTriggerModule::System sys) const
{
  const Intmap *map = NULL;
  switch(sys){
    case kUnknown: return -1;
                   break;
    case kAPD: map = &fAPD_IndividualToTotalChannel;
               break;
    case kVGD: map = &fVGD_IndividualToTotalChannel;
               break;
    case kHVD: map = &fHVD_IndividualToTotalChannel;
               break;
    default: return -1;
  }
  Intmap::const_iterator it = map->find(individualchannelindex);
  if(it != map->end()){
    return it->second;
  }
  return -1;
}

int EXOTriggerModule::GetNChannels(EXOTriggerModule::System sys) const
{
  const Intmap *map = NULL;
  switch(sys){
    case kUnknown: return 0;
                   break;
    case kAPD: map = &fAPD_IndividualToTotalChannel;
               break;
    case kVGD: map = &fVGD_IndividualToTotalChannel;
               break;
    case kHVD: map = &fHVD_IndividualToTotalChannel;
               break;
    default: return 0;
  }
  return map->size();
}

void EXOTriggerModule::PrintIndividualWaveform(const EXOEventData *ED, int threshold, int channel) const
{
  int nsample = ED->GetWaveformData()->fNumSamples;
  const EXOWaveform & waveform = *(ED->GetWaveformData()->GetWaveformWithChannel(channel));
  std::stringstream label, threshstr;
  label << "waveform and trigger threshold, channel " << channel;
  TH1D* wfhist = waveform.GimmeHist();
  wfhist->SetTitle(label.str().c_str());
  threshstr << threshold;
  TF1 thresh = TF1("threshold",threshstr.str().c_str(),0,nsample);
  thresh.SetLineColor(kRed);
  wfhist->GetListOfFunctions()->Add(&thresh);
  EXOMiscUtil::DisplayInProgram(*wfhist,"drawing waveform and trigger threshold");
  wfhist->GetListOfFunctions()->Delete();
  delete wfhist; wfhist = NULL;
}
