//______________________________________________________________________________
#include "EXOAnalysisManager/EXOTreeOutputModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOProcessingInfo.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOControlRecordList.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
// These are normally set by the build file
#ifndef BUILD_ID
#define BUILD_ID "Unknown";
#endif
#ifndef SVN_REV
#define SVN_REV "Unknown";
#endif


IMPLEMENT_EXO_ANALYSIS_MODULE( EXOTreeOutputModule, "toutput" )

EXOTreeOutputModule::EXOTreeOutputModule() :
  fWriteSignals(false),
  fOnlyUWires(false),
  fTrimWFs(false),
  fOnlyDNNEvents(false),
  fEnergySaveCut(700.0),
  fWriteMCCharge(true),
  fCompressSignals(true),
  fMaxFileSize(-1.0),
  fRootTree(NULL),
  fStatisticsTree(NULL),
  fRootFile(NULL),
  fLastEvent(NULL)
{

  TH1::AddDirectory(kFALSE);
}
  
int EXOTreeOutputModule::Initialize()
{
  fRootFile = new TFile(fOutputFilename.c_str(),"RECREATE");
  if ( fRootFile == NULL or fRootFile->IsZombie() ) {
      LogEXOMsg("Error opening: " + fOutputFilename, EEAlert); // terminates
  }

 
  fRootTree = new TTree(EXOMiscUtil::GetEventTreeName().c_str(),EXOMiscUtil::GetEventTreeDescription().c_str());
  if(fRootTree == NULL) {
    LogEXOMsg("Error creating tree", EEAlert);
  }
  EXOEventData TempEventData; // Branch function needs an object, just for retrieving member variable offsets etc.
  if(not fRootTree->Branch(EXOMiscUtil::GetEventBranchName().c_str(), &TempEventData)) {
    LogEXOMsg("Failed to create EventBranch", EEAlert);
  }
  // Make a reference branch for any TRef objects
  fRootTree->BranchRef();
  fLastEvent = NULL;

  fStatisticsTree = new TTree(EXOMiscUtil::GetStatisticsTreeName().c_str(),
                              EXOMiscUtil::GetStatisticsTreeDescription().c_str());

  TObjArray statInfo = FindSharedObjectsOfType(EXOTimingStatisticInfo::Class());
  TIter iter(&statInfo);
  EXOTimingStatisticInfo* stat;
  while ((stat = (EXOTimingStatisticInfo*)iter.Next())) {
    fStatisticsTree->Branch(stat->GetName(), stat);  
  }

  // Set the maximum file size
  if ( fMaxFileSize > 0 ) { 
    const Long64_t bytes_per_gigabyte = 1073741824;
    Long64_t result = (Long64_t)(fMaxFileSize*bytes_per_gigabyte);
    std::cout << "Setting the maximum ROOT file size to " 
              << result << " bytes." << std::endl;
    fRootTree->SetMaxTreeSize(result);
  }


  fRootFile->SetCompressionLevel(EXOMiscUtil::GetTreeCompressionLevel());

  return 0;
}

EXOAnalysisModule::EventStatus EXOTreeOutputModule::ProcessEvent(EXOEventData *ED)
{

  ED->fEventHeader.fSVNRevision = SVN_REV; 
  ED->fEventHeader.fBuildID = BUILD_ID; 

  if (fLastEvent != ED) {
    // If the event has changed (an input object returns another event)
    // we need to update addresses.
    fLastEvent = ED;
    fRootTree->SetBranchAddress(EXOMiscUtil::GetEventBranchName().c_str(), &fLastEvent);
    fRootTree->SetBranchStatus("*",1);
    if (! fWriteSignals ) {
      fRootTree->SetBranchStatus(EXOMiscUtil::GetWaveformBranchName().append("*").c_str(), 0);
    }
    if (! fWriteMCCharge ) {
       fRootTree->SetBranchStatus("fMonteCarloData.fPixelatedChargeDeposits*", 0);
    }
  }
  if (fWriteSignals && fCompressSignals) {
      
      //if(fOnlyDNNEvents and (ED->GetTotalPurityCorrectedEnergy()<900 or ED->GetNumScintillationClusters()<0.5)){
    if(fOnlyDNNEvents and (ED->GetTotalPurityCorrectedEnergy()< fEnergySaveCut)){  
        //fRootTree->SetBranchStatus(EXOMiscUtil::GetWaveformBranchName().append("*").c_str(), 0);
          EXOWaveformData* wfData = ED->GetWaveformData();
          for (size_t ch=0; ch<NUMBER_READOUT_CHANNELS; ch++){
              EXOWaveform *wf = wfData->GetWaveformWithChannelToEdit(ch);
              wfData->Remove(wf);
          }
      }
      else{
        // For the DNN we only need the U-Wires
        // Remove all Non-UWires from the WF-Data
        if (fOnlyUWires){
            EXOWaveformData* wfData = ED->GetWaveformData();
            for (size_t ch=0; ch<NUMBER_READOUT_CHANNELS; ch++){
                if (not EXOMiscUtil::ChannelIsUWire(EXOMiscUtil::TypeOfChannel(ch))){
                    EXOWaveform *wf = wfData->GetWaveformWithChannelToEdit(ch);
                    wfData->Remove(wf);
                }
                else if(fTrimWFs){
                    EXOWaveform *wf     = wfData->GetWaveformWithChannelToEdit(ch);
                
                    // If you trim than you need to subtract the mean here since you lose the ability to get the baseline once
                    // the trim is complete.
                    // The Compression Algorithm only works for positive Numbers though so need to add a fake baseline back in
                
                    //get baseline from WF histogram We use range [0->800] because we are assuming this is MC
                    double baseline     = (((wf->SubWaveform(0,800)).GimmeHist())->Integral())/800.0;
                
                    EXOTemplWaveform<int> wf_sub  = wf->SubWaveform(1000,1350);
                    wf_sub -= baseline;
                    wf_sub += 1780; //can't go negative if you want to compress 
                    wfData->Remove(wf);
                
                    EXOWaveform& new_wf = *wfData->GetNewWaveform();
                    new_wf.fChannel = ch;
                    new_wf.SetSamplingFreq(wf_sub.GetSamplingFreq());
                    new_wf.SetLength(wf_sub.GetLength());
                
                    for (size_t wfi=0; wfi<wf_sub.GetLength(); wfi++){
                        new_wf[wfi] = wf_sub.At(wfi);
                    }
                    wfData->SortByChannel();
                } 
            }
        }
        //ED->GetWaveformData()->Compress();
      }
      ED->GetWaveformData()->Compress();
  }
  if (fRootTree->Fill() < 0) {
    LogEXOMsg("A write error occurred", EEAlert); // terminates
  }
  if (fStatisticsTree->Fill() < 0) {
    LogEXOMsg("A write error occurred in the stat tree", EEAlert); // terminates
  }
  return kOk;
}

int EXOTreeOutputModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/toutput/file","name of output file",this, 
                               "output.root", &EXOTreeOutputModule::SetOutputFilename);

  talktoManager->CreateCommand("/toutput/writeSignals",
        "write all digitized signals to root file",
        this, fWriteSignals, &EXOTreeOutputModule::SetWriteSignals );
  
  talktoManager->CreateCommand("/toutput/onlyUWires",
        "write only the digitized u-wire signals to root file",
        this, fOnlyUWires, &EXOTreeOutputModule::SetOnlyUWires );

  talktoManager->CreateCommand("/toutput/trimWFs",
        "trim the U-wire wfs from sample 1000 to 1350 for DNN",
        this, fTrimWFs, &EXOTreeOutputModule::SetTrimWFs );

  talktoManager->CreateCommand("/toutput/onlyDNNEvents",
                               "write only WFs for the events with Charge-E > energyCut [keV]",
                                this, fOnlyDNNEvents, &EXOTreeOutputModule::SetOnlyDNNEvents );

  talktoManager->CreateCommand("/toutput/energyCut",
                                "energy cut for saving WFs(default=700keV)",
                                this, fEnergySaveCut, &EXOTreeOutputModule::SetEnergySaveCut );

  talktoManager->CreateCommand("/toutput/writeMCCharge",
                        "write MC charge deposits to root file",
                        this, fWriteMCCharge, &EXOTreeOutputModule::SetWriteMCCharge );

  talktoManager->CreateCommand("/toutput/compressSignals",
                          "compress digitized signals", this, fCompressSignals,
                          &EXOTreeOutputModule::SetCompressSignals );

  // negative default value signals that the root default value should be used.
  talktoManager->CreateCommand("/toutput/maxFileSize",
                               "max file size in gigabytes",this, fMaxFileSize,
                               &EXOTreeOutputModule::SetMaxFileSize );
  return 0;
}

int EXOTreeOutputModule::ShutDown()
{
  if ( fRootTree == NULL ) {
    LogEXOMsg("fRootTree is NULL????", EEAlert); // terminates
  }
  if ( fRootFile == NULL ) {
    LogEXOMsg("fRootFile is NULL????", EEAlert); // terminates
  }

  // It will be useful to index the event tree by run/event numbers.  Do this.
  // (Doesn't prevent others from building a different index later.)
  if(fRootTree->GetEntries() > 0) {
    Int_t ret = fRootTree->BuildIndex("fRunNumber", "fEventNumber");
    if(ret < 0) LogEXOMsg("An index for the event tree could not be build -- why?", EEAlert);
  }

  // Save the processing information.
  EXOProcessingInfo* procInfo = new EXOProcessingInfo;
  if(procInfo == NULL) {
    LogEXOMsg("Failed to create EXOProcessingInfo object", EEAlert);
  }

  const TObject* allMods = FindObject("ModulesUsed");
  if (allMods) {
    procInfo->SetAllModules( allMods->GetName() );
  }
  const TObject* allCmds = FindObject("CommandsCalled");
  if (allCmds) {
    procInfo->SetAllCommands( allCmds->GetName() );
  }
  procInfo->SetBuildID( BUILD_ID );
  procInfo->SetSVNRev( SVN_REV );
  procInfo->SetStartTimeOfProcess();
  procInfo->SetCalibrationsFromDatabase(EXOCalibManager::GetCalibManager().AllCalibrationsAreFromDatabase());
  procInfo->SetErrorLoggerMsg(EXOErrorLogger::GetLogger().GetSummary(false));
  const EXOProcessingInfo* prevProcInfo;
  if ( (prevProcInfo = dynamic_cast<const EXOProcessingInfo*>(FindObject("PrevProcessingInfo"))) ) {
    procInfo->SetPrevProcInfo( *prevProcInfo );
  }
  // Now save it to the tree.
  // The TTree now owns the object meaning it will delete it when necessary
  fRootTree->GetUserInfo()->Add(procInfo);

  // Now look for the binary control records.
  const EXOControlRecordList* controlRecords = 
    dynamic_cast<const EXOControlRecordList*>(FindSharedObject("ControlRecords"));
  
  if (controlRecords) {
    // Clone them so we can add them to the tree.  The tree deletes the objects
    // that are added in this way.
    EXOControlRecordList* newList = new EXOControlRecordList(*controlRecords);
    fRootTree->GetUserInfo()->Add(newList);
  }

  // Do the following to get the pointer to the current file.
  // (The current file might change if root has decided to write out 
  // multiple files, which happens when tree size exceeds MaxTreeSize)
  std::cout << "Getting the pointer to the current file from the root tree..." << std::endl;
  fRootFile = fRootTree->GetCurrentFile();

  // Frustratingly, ROOT still has many global objects.  CD-ing to this file
  // ensures that we Clone all the trees in the following into the current
  // file.  That is, all trees must be associated with a file, this ensures
  // they are associated with this file.
  fRootFile->cd();

  TObjArray allSharedTrees = FindSharedObjectsOfType( TTree::Class() );
  TIter next(&allSharedTrees);
  TTree* sharedTree;

  typedef std::set<std::string> StrSet;
  StrSet foundTrees;
  while ((sharedTree = (TTree*) next())) {
    foundTrees.insert(sharedTree->GetName()); 
    if(sharedTree->GetDirectory() == fRootFile) {
      // Cloning a tree from and to the same directory can cause problems,
      // particularly when the "to" directory hasn't really been written yet.
      std::cout << "Found " << sharedTree->GetName() <<
                   "; no need to clone, it already belongs in the correct directory." << std::endl;
      continue;
    }
    std::cout << "Cloning " << sharedTree->GetName() << " for rewriting" << std::endl; 
    // The pointer will get discarded when we close the files.  Do *not* call
    // this with "fast".  Calling with "fast" means the tree is cloned with
    // TTreeCloner which generates a lot of unnecessary TProcessID objects.
    sharedTree->CloneTree(-1)->SetDirectory(fRootFile);
  }
  
  #define ENSURE_TREE_EXISTS(tree) \
    if ( foundTrees.find(EXOMiscUtil::Get  ## tree ## TreeName()) == foundTrees.end() ) { \
      TTree* newTree = new TTree(EXOMiscUtil::Get ## tree ## TreeName().c_str(),          \
                                 EXOMiscUtil::Get ## tree ## TreeDescription().c_str());  \
      newTree->SetDirectory(fRootFile);                                                   \
    } 

  // Handle the glitch tree, which lives in EXOInputModule.
  ENSURE_TREE_EXISTS(Glitch); 
  // Handle the veto tree, which lives in EXOInputModule.
  ENSURE_TREE_EXISTS(Veto);
  
  std::cout << "Writing root file " << fRootFile->GetName() << "...." << std::endl;
  fRootFile->Write("", TObject::kOverwrite);
  std::cout << "Closing root file " << fRootFile->GetName() << "...." << std::endl;
  fRootFile->Close();
  std::cout << "Done writing file." << std::endl;

  // Closing the ROOT file deletes the TTree
  fRootTree = NULL;
  fStatisticsTree = NULL;
  delete fRootFile;
  fRootFile = NULL;
  return 0;
}
