//______________________________________________________
// Fakes wire gains so that MC can fill the same variables as "wiregain" for data.
// Note:  As of this writing (4/4/2012), EXOReconstruction applies a factor of 300 instead of this module.
// If we ever change the wiregain module to apply that factor of 300 instead, we should modify this module to do this.

#include "EXOAnalysisManager/EXOWireGainDummyModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOWireGainDummyModule, "wiregaindummy")

EXOWireGainDummyModule::EXOWireGainDummyModule() {}

EXOAnalysisModule::EventStatus EXOWireGainDummyModule::ProcessEvent(EXOEventData* ED)
{

  if(not ED->fEventHeader.fIsMonteCarloEvent) {
    // The software group consensus is that this module should not change its behavior based on the origin of the event.
    // So, we will fake gain-corrections on data too, but warn the user since this is almost certainly inappropriate.
    LogEXOMsg("You are using dummy correction constants on data -- consider using the wiregain module instead.", EEWarning);
  }

  // Apply these (dummy) corrections to U-wire signals.
  for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
    EXOUWireSignal* signal = ED->GetUWireSignal(i);
    // No gains applied -- this is the dummy module.
    signal->fCorrectedEnergy = signal->fRawEnergy;
    signal->fCorrectedEnergyError = signal->fRawEnergyError;
  }
  return kOk;
}
