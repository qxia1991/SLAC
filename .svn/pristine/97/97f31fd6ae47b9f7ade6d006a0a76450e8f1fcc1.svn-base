//______________________________________________________
// Fakes v-wire gains so that MC can fill the same variables as "v-wiregain" for data.

#include "EXOAnalysisManager/EXOVWireGainDummyModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOVWireSignal.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOVWireGainDummyModule, "v-wiregaindummy")

EXOVWireGainDummyModule::EXOVWireGainDummyModule() {}

EXOAnalysisModule::EventStatus EXOVWireGainDummyModule::ProcessEvent(EXOEventData* ED)
{

  if(not ED->fEventHeader.fIsMonteCarloEvent) {
    // The software group consensus is that this module should not change its behavior based on the origin of the event.
    // So, we will fake gain-corrections on data too, but warn the user since this is almost certainly inappropriate.
    LogEXOMsg("You are using dummy correction constants on data -- consider using the v-wiregain module instead.", EEWarning);
  }

  // Apply these (dummy) corrections to V-wire signals.
  for(size_t i = 0; i < ED->GetNumVWireSignals(); i++) {
    EXOVWireSignal* signal = ED->GetVWireSignal(i);
    // No gains applied -- this is the dummy module.
    signal->fCorrectedMagnitude = signal->fMagnitude;
    signal->fCorrectedMagnitudeError = signal->fMagnitudeError;
  }
  return kOk;
}
