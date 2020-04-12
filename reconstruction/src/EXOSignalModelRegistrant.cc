//______________________________________________________________________________
//
// EXOSignalModelRegistrant
//
// Lightweight base-class for class which register an EXOSignalModelManager
// object.  These classes will be notified of changes to a signal model with a
// particular channel/tag number using NotifySignalModelHasChanged.
//
// There is also the possibility of derived classes being able to save generic
// statistics.  To do so, they must simply call SetStatistic. 
//
//______________________________________________________________________________

#include "EXOReconstruction/EXOSignalModelRegistrant.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"

//______________________________________________________________________________
EXOSignalModelRegistrant::~EXOSignalModelRegistrant()
{
  // Remove ourselves from being registered.
  if (fSigModelMgr) fSigModelMgr->RemoveRegisteredObject(this);
}

//______________________________________________________________________________
void EXOSignalModelRegistrant::TalkTo(const std::string& prefix, 
  EXOTalkToManager* talkTo) 
{
  SetupTalkTo(prefix, talkTo);
  fVerbose.TalkTo(prefix, talkTo);
}

//______________________________________________________________________________
void EXOSignalModelRegistrant::SetStatistic(const std::string& tag,
  double aVal) const
{
  // Sets a statistic for a given tag.
  if (fStatistics) fStatistics->SetStatisticForTag( fPrefix + "." + tag, aVal);
}

//______________________________________________________________________________
void EXOSignalModelRegistrant::StartTimer(const std::string& tag, bool reset /* = true */) const
{
  // Start a timer for a given tag.
  if (fStatistics) fStatistics->StartTimerForTag( fPrefix + "." + tag, reset);
}

//______________________________________________________________________________
void EXOSignalModelRegistrant::StopTimer(const std::string& tag) const
{
  // Stop a timer for a given tag.
  if (fStatistics) fStatistics->StopTimerForTag( fPrefix + "." + tag);
}

//______________________________________________________________________________
void EXOSignalModelRegistrant::ResetTimer(const std::string& tag) const
{
  // Reset a timer for a given tag.
  if (fStatistics) fStatistics->ResetTimerForTag( fPrefix + "." + tag);
}
