//______________________________________________________________________________
// EXOTreeSaverModule
//
//   A class that will aid in saving trees from modules.  If a class wishes to
//   have a tree saved, then the following should be done:
//
//   1.  Derive from this class
//   2.  Overload GetTreeDescription() and GetTreeName() to return the strings
//   of the name and description
//   3.  Overload SetupBranches() to setup the branches for your tree.
//   4.  During ProcessEvent call FillTree
//
//   This class will automatically fill event number and run number in your
//   tree, so you don't need to do this.  See EXOATeamComptonModule for an
//   example implementation.
//
//______________________________________________________________________________

#include "EXOAnalysisManager/EXOTreeSaverModule.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "TROOT.h"
#include "TTree.h"

EXOTreeSaverModule::EXOTreeSaverModule() :
  fEventNumber(0),
  fRunNumber(0),
  fTree(0),
  fShouldFillTree(true)
{
  // Default Constructor
}

EXOTreeSaverModule::~EXOTreeSaverModule()
{
  // We unfortunately can't delete because of how ROOT deals with memory esp
  // with trees.
  if (fRegisteredTreeName != "") RetractObject(fRegisteredTreeName);
}

//______________________________________________________________________________
void EXOTreeSaverModule::FillTree(EXOEventData* ED)
{
  // Fill the tree for an event.  This will automatically call SetupBranches
  // the first time it is called to ensure that the user-defined branches are
  // setup.

  if (!fShouldFillTree) return;
  if (!fTree) {
    if (GetTreeName() == "") {
      LogEXOMsg("Tree name is not set, cannot save tree in " + GetName(), EEError);
      fShouldFillTree = false;
      return;
    }  
    if (!gDirectory->IsWritable()) {
      LogEXOMsg("The current directory is not writable.  You *MUST* be running with the touput "
                "module to output a tree, not building tree in " + GetName(), EEError);
      fShouldFillTree = false;
      return;
    }
    fTree = new TTree(GetTreeName().c_str(), GetTreeDescription().c_str());
    fTree->Branch("fRunNumber", &fRunNumber, "fRunNumber/I");
    fTree->Branch("fEventNumber", &fEventNumber, "fEventNumber/I");
    SetupBranches(fTree);
    fRegisteredTreeName = GetTreeName();
    RegisterSharedObject(fRegisteredTreeName, *fTree);
  }
  fRunNumber = ED->fRunNumber;
  fEventNumber = ED->fEventNumber;
  fTree->Fill();
}
