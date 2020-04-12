//______________________________________________________________________________
// Module to set dnn variables from previous dnn application
//
// Originally created April 20 2019 (Mike)
//
// This module assign DNN variables from previous DNN application during processing
// It avoids re-running the DNN algorithm that is expensive (CPU and time).  Very similar to 
// copy denoise module from Caio.
//
//

#include <iostream> 

#include "EXOAnalysisManager/EXOCopyDNNModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOControlRecordList.hh"
#include "TChain.h"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOCopyDNNModule, "copy-dnn")

EXOCopyDNNModule::EXOCopyDNNModule()
: fRunFlavor(EXOBeginRecord::kUnknownFlavor)
{
  fDNNInfo.clear();  
}


std::string EXOCopyDNNModule::GetDNNFileName(int runNumber)
{
    //Hardcodin paths.  Not ideal but trying to be quick.  
    std::string dnn_path;
    if (runNumber < 6400){
        //This is Phase-1
        if (fRunFlavor == EXOBeginRecord::kDatPhysics){
            dnn_path = "/nfs/slac/g/exo_data8/exo_data/data/WIPP/processedDNN_phase1/";
        }
        else if (fRunFlavor == EXOBeginRecord::kDatSrcClb){
            dnn_path = "/nfs/slac/g/exo_data8/exo_data/data/WIPP/processedDNN_phase1_source/";
        }
        else{
            dnn_path = "";
        }
    }
    else {
        if (fRunFlavor == EXOBeginRecord::kDatPhysics){
            dnn_path = "/nfs/slac/g/exo_data8/exo_data/data/WIPP/processedDNN_phase2/";
        }
        else if (fRunFlavor == EXOBeginRecord::kDatSrcClb){
            dnn_path = "/nfs/slac/g/exo_data8/exo_data/data/WIPP/processedDNN/";
        }
        else{
            dnn_path = "";
        }
    }
    
    std::string fileName = "";
    //std::cout << (dnn_path != "") << "  " << not(dnn_path == "") << std::endl;
    if (not(dnn_path == "")){
        fileName = dnn_path + Form("/*%i*root",runNumber);
    }
    //std::cout << "Load dnn file: "<< fileName << std::endl;
    return fileName;
}

EXOAnalysisModule::EventStatus EXOCopyDNNModule::BeginOfRun(EXOEventData *ED)
{

  //================================================================
  // Start by finding out type of run 
  // DNN only applied to LB and Source
  EXOControlRecordList* recordList = dynamic_cast<EXOControlRecordList*>(FindSharedObject("ControlRecords"));
  const EXOBeginRecord* beginRun = NULL;
  if(recordList) {
    beginRun = recordList->GetPreviousRecord<EXOBeginRecord>(*ED);
    const EXOEndRecord* endRun = recordList->GetPreviousRecord<EXOEndRecord>(*ED);
    if(beginRun and endRun) {
      // We need to confirm that the run started with beginRun hasn't already been ended by endRun.
      EXOControlRecordList::ControlRecordComp compareRecords;
      if(compareRecords(beginRun, endRun)) beginRun = NULL; // the run has already ended.
    }
  }
  if(beginRun) fRunFlavor = beginRun->GetRunFlavor();
  else {
    // Monte carlo, for instance, lacks control records.
    fRunFlavor = EXOBeginRecord::kUnknownFlavor;
  }
  //==============================================================


  /// Read dnn variable (file/tree) for this run
  int runNumber = ED->fRunNumber;
  fDNNInfo.clear();

  std::string fileName = GetDNNFileName(runNumber);
  LogEXOMsg(Form("Using file %s to copy dnn ",fileName.c_str()),EENotice);

  if (fileName==""){

      LogEXOMsg("Cannot find the DNN file.  Either this is a CI/Laser run or we missed it.  Will make all -999???",
                EENotice);
      return kOk;
  }

  
  TChain dnnTree("tree");
  dnnTree.Add(fileName.c_str());
  
  if(dnnTree.GetEntries()<0.5)
  { 
    if (runNumber<9681){
        LogEXOMsg("This isn't a CI/Laser run but no DNN file was loaded ... not ideal", EEAlert);
    }
    else{
        return kOk;
    }
  }

  //Got the file and tree now draw into arrays and save in struct
  dnnTree.SetEstimate(dnnTree.GetEntries()+1);
  dnnTree.Draw("fEventNumber:fDNNVarRaw:fDNNVarRecon:fDNNChargeEnergy", "@fScintClusters.size()==1", "para goff");
  for(int i = 0; i < dnnTree.GetSelectedRows(); i++)
  {
    int event                = (int)    dnnTree.GetVal(0)[i];
    double fDNNVarRaw        = (double) dnnTree.GetVal(1)[i];
    double fDNNVarRecon      = (double) dnnTree.GetVal(2)[i]; 
    double fDNNChargeEnergy  = (double) dnnTree.GetVal(3)[i];
    fDNNInfo.push_back(EXODNNInfo(runNumber,event,fDNNVarRaw, fDNNVarRecon, fDNNChargeEnergy));
  }
  //dnnFile->Close();  
  return kOk;
}


EXOAnalysisModule::EventStatus EXOCopyDNNModule::ProcessEvent(EXOEventData* ED)
{
  // Main function to copy denoised info into event
  int eventNumber = ED->fEventNumber;
  
  for (unsigned int i = 0; i < ED->GetNumScintillationClusters(); i++)
  {
     EXOScintillationCluster *sc = ED->GetScintillationCluster(i);
     size_t idx = GetDNNInfoIdx(eventNumber);
     
     if(!idx)
     {
       sc->fDNNChargeEnergy = -999;
       sc->fDNNVarRaw       = -999;
       sc->fDNNVarRecon     = -999; 
     }
     else
     {
       idx--; // bring back the original index
       sc->fDNNChargeEnergy = fDNNInfo.at(idx).fDNNChargeEnergy;
       sc->fDNNVarRaw       = fDNNInfo.at(idx).fDNNVarRaw;
       sc->fDNNVarRecon     = fDNNInfo.at(idx).fDNNVarRecon;
     }
  }     
    
  return kOk;
}

size_t EXOCopyDNNModule::GetDNNInfoIdx(int event)
{
  // Get the dnn info from vector of copies
  // Determine scintillation cluster based on time difference < 1 us
  
  for(size_t i = 0; i < fDNNInfo.size(); i++)
  {
    if(fDNNInfo[i].fEventNumber == event)
    {
        //return i+1 so that only 0 when false
        //will pull off the extra +1 after
        return i+1;
    }
  }
  return 0;
}

/*
void EXOCopyDNNModule::SetDNNEnergyFile(std::string name)
{
  // Set a file name to copy dnn energy

  EXOCopyDNNCalib::UseDatabase(false);
  EXOCopyDNNCalib::SetUserValue(name);  
}
*/

int EXOCopyDNNModule::TalkTo(EXOTalkToManager *talktoManager)
{
  // TalkTo commands definition for this module
  
  //talktoManager->CreateCommand("/copy-dnn/dnnFile",
  //                             "Give file for dnn variable copying.",
  //                             this,
  //                             fDNNFlavor,
  //                             &EXOCopyDNNModule::SetDNNEnergyFile);
  
  return 0;
}


