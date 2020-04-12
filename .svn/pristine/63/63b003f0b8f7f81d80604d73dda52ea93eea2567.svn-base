//______________________________________________________
// Implement wire gains to convert from ADC counts back to electrons deposited from the LXe.
// Note:  As of this writing (9-26-2011), EXOReconstruction implements some of this correction too.
// The plan is to let this module absorb the correction from EXOReconstruction in the near future.
// 2014/06/17 DCM: Remove warning for MC data since real noise MC is now being gain
//                 corrected

#include "EXOAnalysisManager/EXOWireGainModule.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOWireGainModule, "wiregain")

EXOWireGainModule::EXOWireGainModule()
: fUWireDatabaseFlavor("source_calibration")
{}

EXOAnalysisModule::EventStatus EXOWireGainModule::ProcessEvent(EXOEventData* ED)
{

  // Get the gain corrections from the database.
  const EXOUWireGains* GainsFromDatabase = GetCalibrationFor(EXOUWireGains, EXOUWireGainsHandler, fUWireDatabaseFlavor, ED->fEventHeader);

  // Apply these corrections to U-wire signals.
  for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
    EXOUWireSignal* signal = ED->GetUWireSignal(i);
    // Divide gains by 300 because reconstruction currently applies a gain correction of 300 (electrons/ADC).
    signal->fCorrectedEnergy = signal->fRawEnergy * (GainsFromDatabase->GetGainOnChannel(signal->fChannel)/300.0);
    signal->fCorrectedEnergyError = signal->fRawEnergyError * (GainsFromDatabase->GetGainOnChannel(signal->fChannel)/300.0);
  }

  return kOk;
}

void EXOWireGainModule::SetUWireDatabaseFlavor(std::string flavor)
{
  fUWireDatabaseFlavor = flavor;
}

int EXOWireGainModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/wiregain/UWireDBFlavor",
                               "Set the flavor string used to query the database for uwire gains",
                               this,
                               fUWireDatabaseFlavor,
                               &EXOWireGainModule::SetUWireDatabaseFlavor);
  return 0;
}
