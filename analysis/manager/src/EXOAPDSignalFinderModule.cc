//______________________________________________________________________________
#include "EXOAnalysisManager/EXOAPDSignalFinderModule.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOReconstruction/EXOAPDSignalModelBuilder.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <iostream>


using EXOMiscUtil::TypeOfChannel;

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOAPDSignalFinderModule, "apdsignalfinder" )

EXOAPDSignalFinderModule::EXOAPDSignalFinderModule():
fElectronicsDatabaseFlavor("measured_times")
{

  //cout << "constructing EXOAPDSignalFinderModule with name " << endl;

}

int EXOAPDSignalFinderModule::Initialize()
{
  fSignalModelManager.AddRegisteredObject(&fMatchedFilterFinder);
  fSignalModelManager.AddRegisteredObject(&fDefineAPDSums);
  //fMatchedFilterFinder->SetTimingStatisticInfo(&fTimingInfo);    
  //fMatchedFilterFinder->SetPrefixName(alist[i].second.fName);     

  fMatchedFilterFinder.SetAPDSearchWindowBegin(120000);
  fMatchedFilterFinder.SetAPDSearchWindowEnd(5000);

  return 0;
}

EXOAnalysisModule::EventStatus EXOAPDSignalFinderModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  fChannelMap = &GetChanMapForHeader(ED->fEventHeader);
  if(not fChannelMap){
    LogEXOMsg("Could not get channel map.", EEError);
  }

  fDefineAPDSums.SetSumBothPlanes(true); // fSumBothAPDPlanes = true; correct?
  return kOk;
}

EXOAnalysisModule::EventStatus EXOAPDSignalFinderModule::ProcessEvent(EXOEventData *ED)
{

  //  cout << "At ProcessEvent for " << "APDSignalFinder" << " run rumber = " << ED->fRunNumber << 
  //    ", event number = " << ED->fEventNumber << endl;

  //--------------------------------------------------------------------------------
  // Create ProcessList
  EXOReconProcessList APDGangsProcessList;
  EXOWaveformData *wfd = ED->GetWaveformData();
  size_t nwaveforms = wfd->GetNumWaveforms();
  //cout << "nwf = " << nwaveforms << endl;



  // Create interim process list consisting of APD gangs.
  for(size_t i = 0; i < nwaveforms; i++) {
    int channel = wfd->GetWaveform(i)->fChannel;
    // Skip bad channels
    if(fChannelMap and not fChannelMap->good_channel(channel)) continue;

    // Skip charge injection runs
    //if(fRunFlavor == EXOBeginRecord::kClbInt or fRunFlavor == EXOBeginRecord::kClbExt or fRunFlavor == EXOBeginRecord::kClbCombined) continue;

    // Add to the interim process list only when it is an APD gang 
    if(TypeOfChannel(channel) == EXOMiscUtil::kAPDGang){
      APDGangsProcessList.Add(*wfd->GetWaveform(i),EXOReconUtil::kAPD);
    }
  }
  //cout << "Pre  Size = " << APDGangsProcessList.GetSize() << endl;

  const EXOElectronicsShapers *electronicsShapers = GetCalibrationFor(
       EXOElectronicsShapers, 
       EXOElectronicsShapersHandler, 
       fElectronicsDatabaseFlavor, 
       ED->fEventHeader);

  APDGangsProcessList.ResetIterator();
  const EXOReconProcessList::WaveformWithType* wfWithType = NULL;
  while ( (wfWithType = APDGangsProcessList.GetNextWaveformAndType()) != NULL ) { 
    const EXOWaveform* wf = wfWithType->fWf;
    int channel = wf->fChannel;
    const EXOTransferFunction& tf = electronicsShapers->GetTransferFunctionForChannel(channel);
    fSignalModelManager.BuildSignalModelForChannelOrTag(channel, EXOAPDSignalModelBuilder(tf));
  }

  // Create process list with APD sum waveforms
  EXOReconProcessList processList;
  processList.Add(fDefineAPDSums.GetProcessList(APDGangsProcessList));
  //cout << "Post Size = " << processList.GetSize() << endl;
  //cout << "APDSUM processlist: " << processList.GetSize() << endl;
 
  //cout << "ncc = " << ED->GetNumChargeClusters() << endl;

  for(size_t i = 0; i < ED->GetNumChargeClusters(); i++){
    // Look for charge clusters
    EXOChargeCluster* cc = ED->GetChargeCluster(i);
    Double_t tcl = cc->fCollectionTime;
    fMatchedFilterFinder.SetChargeClusterTime(tcl);

    //cout << "t_coll = " << tcl << endl;
    //--------------------------------------------------------------------------------
    // Apply Matched Filter
    EXOSignalCollection foundSignals;
    foundSignals.Add(fMatchedFilterFinder.FindSignals(processList, foundSignals));
  
    //--------------------------------------------------------------------------------
    // Add signals to EXOEventData
    foundSignals.ResetIterator();
    const EXOChannelSignals *chanSig = 0;
    //cout << "num sig = " << foundSignals.GetNumChannelSignals() << endl;
    while((chanSig = foundSignals.Next()) != NULL){
      //cout << "found chansig!\n";
      chanSig->ResetIterator();
      int channel = chanSig->GetChannel();
      const EXOSignal *sig = 0;
      double threshold = chanSig->GetCacheInformationFor("Threshold");
      // Variables for finding largest of the local maxima found.
      bool init = true;
      double wfMag = -999;
      double wfTime = -999;
  
      while((sig = chanSig->Next()) != NULL){
        //cout << "found sig!\n";
        if(chanSig->GetBehaviorType() == EXOReconUtil::kAPD){
           //cout << "is APD!\n";
          // Save info to EXOEventData  (EXOSignal, EXOUWireSignal)
          //if(ED->GetNumUWireSignals() > 0) std::cout << "Number of UWire Signals: " << ED->GetNumUWireSignals() << endl;
          if(init || sig->fFilteredWFPeakMagnitude > wfMag){
            //cout << "is init or max!\n";
            wfMag = sig->fFilteredWFPeakMagnitude;
            wfTime = sig->fFilteredWFPeakTime;
            init = false;
          }
        }
      }
      if(!init){
        //cout << "save to uwire!\n";
        for(size_t i = 0; i < cc->GetNumUWireSignals(); i++){
          EXOUWireSignal * uwiresig = cc->GetUWireSignalAt(i);
          uwiresig->fAPDFilteredWFPeakMagnitude = wfMag;
          uwiresig->fAPDFilteredWFPeakTime = wfTime;
          uwiresig->fAPDFilteredWFThreshold = threshold;
        }
      }
    }

  }
  //cout << "end looping over ccs\n";

  return kOk;
}

EXOAnalysisModule::EventStatus EXOAPDSignalFinderModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}

int EXOAPDSignalFinderModule::TalkTo(EXOTalkToManager *talktoManager)
{

  // Create some example commands

//  // example double command
//
//  talktoManager->CreateCommand("ANewDoubleCommand","read in a double", 
//           this, 0.0, &EXOAPDSignalFinderModule::SetDoubleParam);
//
//  // example int command
//
//  talktoManager->CreateCommand("ANewIntCommand","read in an int", 
//           this, 0, &EXOAPDSignalFinderModule::SetIntParam );
//  
//  // example bool command
//
//  talktoManager->CreateCommand("ANewBoolCommand","read in a bool", 
//           this, false, &EXOAPDSignalFinderModule::SetBoolParam );
//  
//  // example string command
//
//  talktoManager->CreateCommand("ANewStringCommand","read in a string", 
//           this, "", &EXOAPDSignalFinderModule::SetStringParam );
//
//  // example generic call-back 
//
//  talktoManager->CreateCommand("ANewCallBackCommand","generic call-back", 
//           this, &EXOAPDSignalFinderModule::CallBack );
  
  fMatchedFilterFinder.TalkTo("/apdsignalfinder/matched_filter_finder",talktoManager);

  return 0;
}

int EXOAPDSignalFinderModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}

void EXOAPDSignalFinderModule::CallBack()
{
  std::cout << "EXOAPDSignalFinderModule::CallBack() called. " << std::endl;
}


