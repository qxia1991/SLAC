#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXONewYMatchExtractor.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <cassert>
#include <iostream>

using namespace std;
using EXOMiscUtil::TypeOfChannel;

EXONewYMatchExtractor::EXONewYMatchExtractor()
{

}

//_______________________________________________________________________________________________
EXOSignalCollection EXONewYMatchExtractor::Extract(const EXOReconProcessList& processList, const EXOSignalCollection& inputSignals) const
{
  //Returns a signal collection of found v-wire signals.
  //We just copy the v-wire portion of inputSignals and return that.

  return inputSignals.GetCollectionForType(EXOMiscUtil::kVWire);
}

//______________________________________________________________________________
void EXONewYMatchExtractor::SetupTalkTo(const std::string& prefix, 
  EXOTalkToManager* talkTo)
{
  // Set up TalkTo commands for the Y Match Extractor
}
