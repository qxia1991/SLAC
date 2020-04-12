//______________________________________________________________________________
// Module to set denoised energy from previous denoising
//
// Originally created Nov 16 2016 (Caio)
//
// This module assign denoised energy from previous denoising during processing
// It avoids re-running the denoising algorithm that is expensive (CPU and time)
//
// A library of denoised energy should be made for each denoising
// Each library (a ROOT file carrying a TTree) is saved at SLAC
// but a DB table carries the flavor and file location
// The user can specify which flavor to copy the denoised info utilize
//

#include <iostream> 

#include "EXOAnalysisManager/EXOCopyDenoisedModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOCalibUtilities/EXOCopyDenoisedCalib.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOCopyDenoisedModule, "copy-denoised")

EXOCopyDenoisedModule::EXOCopyDenoisedModule()
  : fDenoisedEnergyFlavor("svnr10114")
{
  fDenoisedInfo.clear();  
}

EXOAnalysisModule::EventStatus EXOCopyDenoisedModule::BeginOfRun(EXOEventData *ED)
{
  // Read denoised energy library (file/tree) for this run

  int runNumber = ED->fRunNumber;
  fDenoisedInfo.clear();

  std::string fileName = EXOCopyDenoisedCalib::GetFileName(fDenoisedEnergyFlavor.c_str(),ED->fEventHeader);

  LogEXOMsg(Form("Using file %s to copy denoised energy",fileName.c_str()),EENotice);

  TFile *denoisedFile = TFile::Open(fileName.c_str());
  if(!denoisedFile or denoisedFile->IsZombie())
  {
    LogEXOMsg("Cannot find the denoised library file. Will not copy denoised energy.", EEAlert);
    return kError;
  }
  TTree *denoisedTree = dynamic_cast<TTree*>(denoisedFile->Get("DenoisedEnergyTree"));
  if(!denoisedTree)
  {
    LogEXOMsg("Cannot find the denoised library tree. Will not copy denoised energy.", EEAlert);
    denoisedFile->Close();
    return kError;
  }
  denoisedTree->SetEstimate(denoisedTree->GetEntries()+1);
  denoisedTree->Draw("event:nsc:denoised:error:code:x:y:z:time",Form("run == %d",runNumber),"para goff");
  for(int i = 0; i < denoisedTree->GetSelectedRows(); i++)
  {
    int event = (int) denoisedTree->GetVal(0)[i];
    int nsc = (int) denoisedTree->GetVal(1)[i];
    double denoised = denoisedTree->GetVal(2)[i];
    double error = denoisedTree->GetVal(3)[i];
    int code = (int) denoisedTree->GetVal(4)[i];
    double x = denoisedTree->GetVal(5)[i];
    double y = denoisedTree->GetVal(6)[i];
    double z = denoisedTree->GetVal(7)[i];
    double time = denoisedTree->GetVal(8)[i];
    fDenoisedInfo.push_back(EXODenoisedInfo(runNumber,event,nsc,denoised,error,code,x,y,z,time));
  }

  denoisedFile->Close();  
  
  return kOk;
}


EXOAnalysisModule::EventStatus EXOCopyDenoisedModule::ProcessEvent(EXOEventData* ED)
{
  // Main function to copy denoised info into event
  int eventNumber = ED->fEventNumber;
  
  for (unsigned int i = 0; i < ED->GetNumScintillationClusters(); i++)
  {
     EXOScintillationCluster *sc = ED->GetScintillationCluster(i);
     size_t idx = GetDenoisedInfoIdx(eventNumber,sc->fTime);
     if(!idx)
     {
       sc->fDenoisedEnergy = -999;
       sc->fDenoisedEnergyError = -999;
       sc->fDenoisingInternalCode = -999;
     }
     else
     {
       idx--; // bring back the original index
       sc->fDenoisedEnergy = fDenoisedInfo.at(idx).fDenoisedEnergy;
       sc->fDenoisedEnergyError = fDenoisedInfo.at(idx).fDenoisedError;
       sc->fDenoisingInternalCode = fDenoisedInfo.at(idx).fDenoisingInternalCode;
       //fDenoisedInfo.erase(fDenoisedInfo.begin()+idx);
     }
  }     
    
  return kOk;
}

size_t EXOCopyDenoisedModule::GetDenoisedInfoIdx(int event, double time)
{
  // Get the denoised energy info from vector of copies
  // Determine scintillation cluster based on time difference < 1 us
  
  for(size_t i = 0; i < fDenoisedInfo.size(); i++)
  {
    if(fDenoisedInfo[i].fEventNumber == event)
    {
      if(std::fabs(fDenoisedInfo[i].fTime - time) < 1000) // check if WF time difference btw cluster is < 1us
      {
        return i+1;
      }
    }
  }

  return 0;
}

void EXOCopyDenoisedModule::SetDenoisedEnergyFlavor(std::string flavor)
{
  // Set DB flavor of denoised energy
  EXOCopyDenoisedCalib::UseDatabase(true);
  
  fDenoisedEnergyFlavor = flavor;
}

void EXOCopyDenoisedModule::SetDenoisedEnergyFile(std::string name)
{
  // Set a file name to copy denoised energy

  EXOCopyDenoisedCalib::UseDatabase(false);
  EXOCopyDenoisedCalib::SetUserValue(name);  
}


int EXOCopyDenoisedModule::TalkTo(EXOTalkToManager *talktoManager)
{
  // TalkTo commands definition for this module
  
  talktoManager->CreateCommand("/copy-denoised/denoisingFlavor",
                               "Set the DB flavor to query denoising energy version.",
                               this,
                               fDenoisedEnergyFlavor,
                               &EXOCopyDenoisedModule::SetDenoisedEnergyFlavor);

  talktoManager->CreateCommand("/copy-denoised/denoisingFile",
                               "Do not use DB to query denoising energy version, but force use of a given file.",
                               this,
                               fDenoisedEnergyFlavor,
                               &EXOCopyDenoisedModule::SetDenoisedEnergyFile);
  return 0;
}


