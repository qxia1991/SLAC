
#include "EXOAnalysisManager/EXOVWireGainModule.hh"
#include "EXOCalibUtilities/EXOVWireGains.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOVWireSignal.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <iostream>

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOVWireGainModule, "v-wiregain")

EXOVWireGainModule::EXOVWireGainModule()
: fVWireDatabaseFlavor("vanilla"),
  fUserGainCorrection(1.0)
{

}

EXOAnalysisModule::EventStatus EXOVWireGainModule::ProcessEvent(EXOEventData* ED)
{

  // Get the gain corrections from the database.
  const EXOVWireGains* GainsFromDatabase = GetCalibrationFor(EXOVWireGains, EXOVWireGainsHandler, fVWireDatabaseFlavor, ED->fEventHeader);

  // Apply these corrections to U-wire signals.
  for(size_t i = 0; i < ED->GetNumVWireSignals(); i++) {
    EXOVWireSignal* signal = ED->GetVWireSignal(i);
    
    // Divide gains by 300 because reconstruction currently applies a gain correction of 300 (electrons/ADC).
    signal->fCorrectedMagnitude = signal->fMagnitude * fUserGainCorrection * (GainsFromDatabase->GetGainOnChannel(signal->fChannel)/300.0);
    signal->fCorrectedMagnitudeError = signal->fMagnitudeError * fUserGainCorrection * (GainsFromDatabase->GetGainOnChannel(signal->fChannel)/300.0);
  }

  return kOk;
}

void EXOVWireGainModule::SetVWireDatabaseFlavor(std::string flavor)
{
  fVWireDatabaseFlavor = flavor;
}

void  EXOVWireGainModule::SetUserGainCorrection(double val)
{
    fUserGainCorrection = val;
}

int EXOVWireGainModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/v-wiregain/VWireDBFlavor",
                               "Set the flavor string used to query the database for vwire gains",
                               this,
                               fVWireDatabaseFlavor,
                               &EXOVWireGainModule::SetVWireDatabaseFlavor);
  
  talktoManager->CreateCommand("/v-wiregain/UserGainCorrection",
                               "Apply constant multiplicative gain correction to all V-Wire channels (for testing)",
                               this,
                               fUserGainCorrection,
                               &EXOVWireGainModule::SetUserGainCorrection);
  
  return 0;
}
