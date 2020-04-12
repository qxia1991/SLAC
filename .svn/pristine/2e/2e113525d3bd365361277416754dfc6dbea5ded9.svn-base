//______________________________________________________________________________
// This Module can be used to filter events based on environmental criteria.
// As of 10/20/11, the only criterion is the clean room siren.

#include "EXOAnalysisManager/EXOEnvironmentModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOEnvironmentCalib.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOEnvironmentModule, "environment" )
  
EXOEnvironmentModule::EXOEnvironmentModule()
{
  
  //cout << "constructing EXOEnvironmentModule with name " << NAME << endl;
  
}

EXOAnalysisModule::EventStatus EXOEnvironmentModule::ProcessEvent(EXOEventData *ED)
{
  //Get environment info from the DB
  EXOEnvironmentCalib * eCalib = GetCalibrationFor(EXOEnvironmentCalib,
						  EXOEnvironmentHandler,
						  m_flavor,
						  ED->fEventHeader);

  //EXOTimestamp tmstmp = eCalib->validSince();
  //LogEXOMsg(tmstmp.getString(),EEWarning);

  if (eCalib == NULL) {
    LogEXOMsg("Environment data not found.", EEAlert);
    return kOk;
  }
  ED->fEventHeader.fSirenActiveInCR = eCalib->GetSirenState();
  if (ED->fEventHeader.fSirenActiveInCR == true){
    return kDrop;
  }
  
  return kOk;
}

int EXOEnvironmentModule::TalkTo(EXOTalkToManager *talktoManager)
{

  //  Make a command to get calibration flavor
  talktoManager->CreateCommand("/environ/flavor",
                               "calibration flavor to be used",
                               this,
                               "vanilla",
                               &EXOEnvironmentModule::SetFlavor);
  
  return 0;
}
