//______________________________________________________________________________
//
// EXOTreeInputModule
//   Handles the input of a file with an EXO Tree inside.
//
// Can handle mutliple files by calling AddFilesToProcess which also handles
// wildcards.  To do this in an exo script do the following:
// 
//   /input/file my_first_root_file.root
//   /tinput/addfile my_second_root_file.root
//
// If you have numbered files, perhaps the easiest is to use wildcards:
//
//   /input/file my_root_file_0000.root
//   /tinput/addfile my_root_file_000[1-2].root
//
// which will add my_root_file_0000.root, my_root_file_0001.root, and
// my_root_file_0002.root as expected.  
//
// The tree input module also provides access to the control records in the
// binary files.  Each record (deriving from EXOControlRecord) is stored in a
// list EXOControlRecordList.  This object is then shared using the shared
// object mechnanism for analysis modules, allowing analysis modules to have
// access to this object.  An example usage is below:
//
//   EXOControlRecordList* recordList = 
//     dynamic_cast<EXOControlRecordList*>(FindSharedObject("ControlRecords"));
//   if (!recordList) {
//     // handle the case if it's NULL, which means it couldn't be found.
//   } else {
//     // access the list. 
//   }
//
// For more information on how to access the record list, see the
// EXOControlRecordList documentation.
//
//
// See also EXOInputModule for more information.
//______________________________________________________________________________
#include "EXOAnalysisManager/EXOTreeInputModule.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOProcessingInfo.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <iostream>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

#if defined(STATIC) && defined(LINKXROOTD)
#include "TXNetFile.h"
#endif

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOTreeInputModule, "tinput" )

EXOTreeInputModule::EXOTreeInputModule() : 
  fRootFile(NULL),
  fRootTree(NULL),
  fCurrentEventID(-1)
{
  TH1::AddDirectory(kFALSE);
  fEventData = new EXOEventData();
  RegisterSharedObject("ControlRecords", fControlRecords);
}

//______________________________________________________________________________
void EXOTreeInputModule::CloseCurrentFile()
{
  // No-op if the file is already closed.
  if (!fRootFile) return;
  cout << "Closing root file with name " << fRootFile->GetName() << "." << endl;
  
  RetractObject("PrevProcessingInfo");
  delete fRootTree; 
  delete fRootFile; 
  fRootFile = NULL;
  fRootTree = NULL;
}

//______________________________________________________________________________
void EXOTreeInputModule::OpenFile(const std::string& filename)
{

  // Save the information that's about to get overwritten -- if we produce an error, that will let us backtrack.
  // Be sure to delete it if this function is successful!
  TFile* oldFile = fRootFile;
  TTree* oldTree = fRootTree;
  EXOEventData* oldEvent = fEventData;

  // Also save the current directory -- we'll want to make sure it's unchanged regardless.
  TDirectory* OldDir = gDirectory;

  try {
    // Try to open root file
    #if defined(STATIC) && defined(LINKXROOTD)
    fRootFile = new TXNetFile(filename.c_str()); 
    if (!fRootFile or fRootFile->IsZombie()) {
      delete fRootFile;
      fRootFile =  TFile::Open(filename.c_str());
    }
    #else
    fRootFile =  TFile::Open(filename.c_str());
    #endif
    if ( fRootFile == NULL or fRootFile->IsZombie() or not fRootFile->IsOpen()) {
      throw EXOMiscUtil::EXOBadCommand("Failed to open file " + filename);
    }

    // Get the tree
    fRootTree = dynamic_cast<TTree*>(fRootFile->Get(EXOMiscUtil::GetEventTreeName().c_str()));
    if ( fRootTree == NULL ) throw EXOMiscUtil::EXOBadCommand("Unable to find tree in file " + filename);

    // Setup branches
    fEventData = NULL;
    if(Int_t retValue = fRootTree->SetBranchAddress(EXOMiscUtil::GetEventBranchName().c_str(), &fEventData) != 0) {
      std::ostringstream stream;
      stream << "Could not get branch " << EXOMiscUtil::GetEventBranchName() << "; ";
      stream << "SetBranchAddress gave error code " << retValue << ".";
      throw EXOMiscUtil::EXOBadCommand(stream.str());
    }
    if(fRootTree->BuildIndex(EXOMiscUtil::GetEventBranchName().append(".fEventNumber").c_str()) < 0) {
      throw EXOMiscUtil::EXOBadCommand("Failed to build index.");
    }

    Long64_t result = fRootTree->GetEntriesFast();
    if ( result < 0 ) throw EXOMiscUtil::EXOBadCommand("Failed to get the number of entries in the tree.");
    if ( result == 0 ) LogEXOMsg("No entries found in file " + filename, EEWarning);
    cout << "Number of events in tree = " << result << endl;
  }
  catch ( EXOMiscUtil::EXOBadCommand& badCommand ) {
    if(fRootFile) delete fRootFile; // deletes the tree too, if it had been opened.
    // Restore the old file before re-throwing.
    fRootFile = oldFile;
    fRootTree = oldTree;
    fEventData = oldEvent;
    if(OldDir and gDirectory != OldDir) OldDir->cd();
    throw;
  }

  // OK, no error was thrown -- so we should delete the old stuff.
  delete oldFile; // Deletes the old tree and event, too.  The pointers themselves get cleaned up when the function exits.

  // out of order data access intialization
  fCurrentEventID = -1; // no data available yet

  // Also get the glitch tree
  RetractObject(EXOMiscUtil::GetGlitchTreeName());
  TTree *aTree = dynamic_cast<TTree*>(fRootFile->Get(EXOMiscUtil::GetGlitchTreeName().c_str()));
  if(aTree) RegisterSharedObject(EXOMiscUtil::GetGlitchTreeName(), *aTree);
	
  // and get the veto tree
  RetractObject(EXOMiscUtil::GetVetoTreeName());
  aTree = dynamic_cast<TTree*>(fRootFile->Get(EXOMiscUtil::GetVetoTreeName().c_str()));
  if(aTree) RegisterSharedObject(EXOMiscUtil::GetVetoTreeName(), *aTree);

  // and get the muon tree
  RetractObject(EXOMiscUtil::GetMuonTreeName());
  aTree = dynamic_cast<TTree*>(fRootFile->Get(EXOMiscUtil::GetMuonTreeName().c_str()));
  if(aTree) RegisterSharedObject(EXOMiscUtil::GetMuonTreeName(), *aTree);

  // Check for resident processing info and register so other modules can
  // access it 
  TList* userList = fRootTree->GetUserInfo();
  if ( dynamic_cast<EXOProcessingInfo*>(userList->First()) ) {
    RegisterObject("PrevProcessingInfo", *userList->First());
  }
  // Check for control records
  EXOControlRecordList* recList;
  if ( userList->GetEntries() >= 2 && 
       (recList = dynamic_cast<EXOControlRecordList*>(userList->At(1)))) {
    // We have them, add to the current control records
    fControlRecords.Add(*recList);
  }

  // Make sure the current directory is unchanged.
  if(OldDir and gDirectory != OldDir) OldDir->cd();

  cout << "Successfully opened root file with name " << filename << endl;
}

//______________________________________________________________________________
EXOEventData* EXOTreeInputModule::GetNextEvent()
{

  if(BreakWasRequested()){
    return NULL;
  }
  // Load the entry
  EXOEventData* ed;
  if ((ed = GetEvent( fCurrentEventID+1 )) == NULL) {
    if (CheckNextFile()) return GetNextEvent();
    return NULL; 
  }

  return ed;

}

//______________________________________________________________________________
EXOEventData* EXOTreeInputModule::GetEvent(unsigned int event)
{
  // work function for fetching the data ... 
  // fetch the event_serialID-th in the open file
  if (fRootTree == NULL) {
    LogEXOMsg("tree or event data not available", EEAlert); // quits
  }

  fEventData->Clear();
  // fetch the data ... 
  if (fRootTree->LoadTree(event) < 0) {
    // It's only an error if this entry/event is lower than the total number of
    // entries.
    if (event < fRootTree->GetEntriesFast() ) { 
      LogEXOMsg("could not find data", EEAlert); // quits
    }
    
    return NULL;
  }
  if (fRootTree->GetEntry(event) < 0) {
    LogEXOMsg("incomplete data fetching", EEAlert); // quits
  }

  // Decompress waveforms
  fEventData->GetWaveformData()->Decompress();
  fCurrentEventID = event;
  return fEventData;

}

//______________________________________________________________________________
EXOEventData* EXOTreeInputModule::GetNext()
{
  // get next event ... 
  return GetEvent(fCurrentEventID + 1);
}

//______________________________________________________________________________
EXOEventData* EXOTreeInputModule::GetPrevious()
{
  // get previous event ... 
  return GetEvent(fCurrentEventID - 1);
}

//______________________________________________________________________________
EXOEventData* EXOTreeInputModule::SearchEvent(int event_number)
{
  // search for event number "ne" and fetch it
  return GetEvent(
    fRootTree->GetEntryNumberWithIndex(event_number)); 
}

//______________________________________________________________________________
int  EXOTreeInputModule::get_run_number() 
{
  return fEventData->fRunNumber; 
}

//______________________________________________________________________________
int  EXOTreeInputModule::get_event_number() 
{ 
  return fEventData->fEventNumber; 
}

//______________________________________________________________________________
EXOTreeInputModule::~EXOTreeInputModule()
{
  RetractObject("ControlRecords");
  delete fEventData;
  delete fRootTree;
  delete fRootFile;
}
//______________________________________________________________________________
bool EXOTreeInputModule::CheckNextFile()
{
  // Check the next file in the list to open, returning true if a
  // new file was opened, false if no new file was opened.

  if (fFiles.empty()) return false; 
  bool retVal = false;
  while (!fFiles.empty() && !retVal) {
    std::string nextFile = fFiles.front(); 
    fFiles.pop_front();
    SetFilename(nextFile);
    // The following means we were successful, unfortunately this
    // is necessary because SetFilename does not return a value.
    if ( fRootFile && fRootTree ) retVal = true; 
  }
  return retVal;
}

//______________________________________________________________________________
int EXOTreeInputModule::TalkTo(EXOTalkToManager* tm)
{
  // Add commands, including /tinput/addfile"
  tm->CreateCommand("/tinput/addfile",
                    "Add files or files (with wildcards) to process",
                    this, 
                    "", 
                    &EXOTreeInputModule::AddFilesToProcess);
  return 0;

}
//______________________________________________________________________________
void EXOTreeInputModule::AddFilesToProcess(std::string fileOrFiles)
{
  // Add File or files to process.  This handles wildcard names
  // as well.
  std::vector<std::string> allFiles = EXOMiscUtil::SearchForMultipleFiles(fileOrFiles);
  for (size_t i=0;i<allFiles.size();i++) {
    LogEXOMsg("Adding file " + allFiles[i] + " to list of files to process", EENotice);
    fFiles.push_back(allFiles[i]);
  }
}
//______________________________________________________________________________
bool EXOTreeInputModule::FileIsOpen() const
{
  // Return true if this module currently has a file opened, false otherwise.
  return (fRootFile and fRootFile->IsOpen());
}
