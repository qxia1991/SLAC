//______________________________________________________________________________
//
// EXOInputModule
// Base class for all input modules.  Each derived class must define
// GetNextEvent and file_type_recognized.  Additional interface functions: 
//
// GetNext()
// GetPrevious()
// GetEvent(int event) // get nth event in data set
// SearchEvent(int eventNumber) // get eventNumber in data set
//
// are also provided for derived classes to overload.  The default return
// values of this functions are NULL.
//
// An example usage from an external program (or ROOT script):
//
//    // the following is only needed in a script. 
//    gSystem->Load("libEXOROOT");
//    // An example of how to do I/O directly instead of using "steering" of
//    // EXOAnalysisManager
//   
//    // Can also work for old files or binary files
//    std::string fileName = "run00001333-recon-3000.root";
//    EXOInputModule* input = 
//      EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile( fileName );
//    if (!input) {
//      cout << "No input module found for: " << fileName << endl;
//      return;
//    }
//   
//    // You can also directly instantiate whichever module you want:
//    // EXOTreeInputModule input;
//   
//    input->SetFilename(fileName);
//    // Gets 0th event in file
//    cout << "Getting first event in file: " << endl;
//    EXOEventData* ed = input->GetEvent(0); 
//    if (ed) cout << ed->fEventNumber << endl;
//   
//    // Can navigate relative to current event, not at the moment only the tree
//    // (old and new) input files handle this.  Binary input can't yet do it and
//    // so will return NULL.
//    cout << "Getting next event: " << endl;
//    ed = input->GetNext();
//    if (ed) cout << ed->fEventNumber << endl;
//    cout << "Getting previous event: " << endl;
//    ed = input->GetPrevious();
//    if (ed) cout << ed->fEventNumber << endl;
//   
//    // Can also get specific events with particular event numbers: 
//    cout << "Searching for event: 101" << endl;
//    ed = input->SearchEvent(101);
//    if (ed) cout << ed->fEventNumber << endl;
//   
//    // Can move on to another file name:
//    // input->SetFilename (next file)
//
//______________________________________________________________________________

#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOGlitchRecord.hh"
#include "EXOUtilities/EXOVetoEventHeader.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "TTree.h"
#include <string>
#include <iostream>
#include <csignal>
using namespace std;

//IMPLEMENT_EXO_ANALYSIS_MODULE( EXOInputModule, "input" )
void break_handler(int sig)
{
  cout << endl << " Break Requested... " << 
       "If input modules support it, then we will exit soon." << endl;
  EXOInputModule::fBreakRequested = true; 
}

bool EXOInputModule::fBreakRequested = false;

EXOInputModule::EXOInputModule() :
  fGlitchTree(0),
  fVetoTree(0)
{
  signal(SIGUSR1, break_handler);
}


EXOInputModule::~EXOInputModule()
{
  RetractObject(EXOMiscUtil::GetGlitchTreeName());
  RetractObject(EXOMiscUtil::GetVetoTreeName());
}
//______________________________________________________________________________
int  EXOInputModule::get_run_number()
{
  // Returns the event number 
  cout << "in EXOInputModule get run number" << endl;
  return 0;
}

//______________________________________________________________________________
int  EXOInputModule::get_event_number()
{
  cout << "in EXOInputModule: get event number" << endl;
  return 0;
}

//______________________________________________________________________________
bool EXOInputModule::is_new_run_segment()
{
  return false;
}

//______________________________________________________________________________
EXOEventData* EXOInputModule::GetEvent(unsigned int event_serialID)
{
  // Returns the n-th event in the data set.  Note, this does *NOT* return an
  // event with a particular event number, and rather the n-th event in the
  // dataset.  To get an event with a particular event number, use SearchEvent.
  // Returns NULL when event not available, or if this function is not
  // implemented by the deriving class.
  return NULL;
}

//______________________________________________________________________________
EXOEventData* EXOInputModule::GetNext()
{
  // Returns the next event to any event previously loaded.  It will return
  // Returns NULL when event not available, or if this function is not
  // implemented by the deriving class.
  return NULL;
}

//______________________________________________________________________________
EXOEventData* EXOInputModule::GetPrevious()
{
  // Returns the previous event to any event previously loaded.  Returns NULL
  // when event not available, or if this function is not implemented by the
  // deriving class.
  return NULL;
}

//______________________________________________________________________________
EXOEventData* EXOInputModule::SearchEvent(int event_number)
{
  // Returns the event corresponding to the event number.  Returns NULL when
  // event not available, or if this function is not implemented by the
  // deriving class.
  return NULL;
}

//______________________________________________________________________________
void EXOInputModule::CloseCurrentFile()
{
  // For derived input modules that do not read from a file, no-op.
}

//______________________________________________________________________________
void EXOInputModule::OpenFile(const std::string& /*afile*/)
{
  // For derived input modules that do not read from a file, no-op.
}

//______________________________________________________________________________
void EXOInputModule::SetFilename(std::string aval)
{
  if(not reads_from_file()) throw EXOMiscUtil::EXOBadCommand("Can't set filename for an input module that won't read them.");
  OpenFile(aval);
}

//_____________________________________________________________________________
void EXOInputModule::AddGlitchRecord(EXOGlitchRecord* aGlitch)
{
  // Add a glitch record to the fGlitchTree object; initialize that tree if necessary.
  // This also takes care of filling the tree with each glitch record.

  // If it's the first call, create branch GlitchBranch.
  static bool first_call = true;
  if(first_call) {

    // If the tree doesn't exist yet, create it.
    if(not fGlitchTree) {
      fGlitchTree = new TTree(EXOMiscUtil::GetGlitchTreeName().c_str(), EXOMiscUtil::GetGlitchTreeDescription().c_str());
    }

    // If the branch doesn't exist yet, create it.
    if(not fGlitchTree->FindBranch(EXOMiscUtil::GetGlitchBranchName().c_str())) {
      fGlitchTree->Branch(EXOMiscUtil::GetGlitchBranchName().c_str(), aGlitch);
      fGlitchTree->BranchRef();
    }

    first_call = false;
    RegisterSharedObject(EXOMiscUtil::GetGlitchTreeName(), *fGlitchTree);
  }

  // Update the branch address, and fill with aGlitch.
  fGlitchTree->SetBranchAddress(EXOMiscUtil::GetGlitchBranchName().c_str(), &aGlitch);
  if(fGlitchTree->Fill() < 0) {
    LogEXOMsg("Filling the glitch record tree failed", EEError);
  }

  // We expect aGlitch to change from call to call; go ahead and clear it.
  fGlitchTree->ResetBranchAddresses();
}
//_____________________________________________________________________________
void EXOInputModule::AddVetoRecord(EXOVetoEventHeader* aVeto)
{
  // Add a veto record to the fVetoTree object; initialize that tree if necessary.
  // This also takes care of filling the tree with each veto record.
  
  // If it's the first call, create branch VetoBranch.
  static bool first_call = true;
  if(first_call) {
    
    // If the tree doesn't exist yet, create it.
    if(not fVetoTree) {
      fVetoTree = new TTree(EXOMiscUtil::GetVetoTreeName().c_str(), EXOMiscUtil::GetVetoTreeDescription().c_str());
    }
    
    // If the branch doesn't exist yet, create it.
    if(not fVetoTree->FindBranch(EXOMiscUtil::GetVetoBranchName().c_str())) {
      fVetoTree->Branch(EXOMiscUtil::GetVetoBranchName().c_str(), aVeto);
      fVetoTree->BranchRef();
    }
    
    RegisterSharedObject(EXOMiscUtil::GetVetoTreeName(), *fVetoTree);
    first_call = false;
  }
  
  // Update the branch address, and fill with aVeto.
  fVetoTree->SetBranchAddress(EXOMiscUtil::GetVetoBranchName().c_str(), &aVeto);
  if(fVetoTree->Fill() < 0) {
    LogEXOMsg("Filling the veto record tree failed", EEError);
  }
  
  // We expect aVeto to change from call to call; go ahead and clear it.
  fVetoTree->ResetBranchAddresses();
}
