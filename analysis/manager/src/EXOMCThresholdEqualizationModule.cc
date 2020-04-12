//______________________________________________________________________________
#include "EXOAnalysisManager/EXOMCThresholdEqualizationModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOAPDSignal.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOVWireSignal.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOSignalThreshold.hh"
#include <iostream>
#include <sstream>
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOMCThresholdEqualizationModule, "mcthreshold" )

EXOMCThresholdEqualizationModule::EXOMCThresholdEqualizationModule()
: fRandGen(0),
  fEqualizeAPD(true),
  fEqualizeU(true),
  fEqualizeV(true),
  fAPDMatchTime(RC_TAU_APD_1+RC_TAU_APD_2)
{
  
}

bool EXOMCThresholdEqualizationModule::ShouldSignalSurvive(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude, const EXOEventHeader& header) const
{
  EXOSignalThreshold* thresh = GetCalibrationFor(EXOSignalThreshold,
                                                 EXOSignalThresholdHandler,
                                                 "vanilla",
                                                 header);
  double dataRate = thresh->GetDataSignalRate(sigType,side,magnitude);
  double mcRate = thresh->GetMCSignalRate(sigType,side,magnitude);

  double dice = mcRate * fRandGen.Uniform();
  if(dice > dataRate) {
    return false;
  }
  return true;
}

EXOAnalysisModule::EventStatus EXOMCThresholdEqualizationModule::BeginOfRun(EXOEventData *ED)
{
  if(not ED->fEventHeader.fIsMonteCarloEvent){
    LogEXOMsg("Threshold equalization module should only run on Monte Carlo events",EEError);
    return kError;
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOMCThresholdEqualizationModule::ProcessEvent(EXOEventData *ED)
{
  if(not ED->fEventHeader.fIsMonteCarloEvent){
    LogEXOMsg("Threshold equalization module should only run on Monte Carlo events",EEError);
    return kError;
  }

  if(fEqualizeAPD){
    size_t na = ED->GetNumAPDSignals();
    std::vector<EXOAPDSignal*> apdsToRemove;;
    for(size_t i=0; i<na; i++){
      EXOAPDSignal* sig = ED->GetAPDSignal(i);
      if(sig->fChannel > 2){
        continue;
      }
      EXOMiscUtil::EChannelType sigType = EXOMiscUtil::kAPDSumOfGangs;
      EXOMiscUtil::ETPCSide side = (sig->fChannel == 1 ? EXOMiscUtil::kNorth : EXOMiscUtil::kSouth);
      if(not ShouldSignalSurvive(sigType,side,sig->fRawCounts,ED->fEventHeader)){
        apdsToRemove.push_back(sig);
      }
    }
    RemoveAPDSignals(ED,apdsToRemove);
  }

  if(fEqualizeU){
    std::vector<EXOUWireSignal*> usToRemove;;
    size_t nu = ED->GetNumUWireSignals();
    for(size_t i=0; i<nu; i++){
      EXOUWireSignal* sig = ED->GetUWireSignal(i);
      //TPC side should not matter here
      if(not ShouldSignalSurvive(EXOMiscUtil::kUWire,EXOMiscUtil::kNorth,sig->fRawEnergy,ED->fEventHeader)){
        usToRemove.push_back(sig); 
      }
    }
    for(size_t i=0;i<usToRemove.size();i++){
      stringstream output;
      output << "Removing U signal with fRawEnergy = " << usToRemove[i]->fRawEnergy;
      LogEXOMsg(output.str(),EEDebug);
      ED->Remove(usToRemove[i]);
    }
  }

  if(fEqualizeV){
    std::vector<EXOVWireSignal*> vsToRemove;;
    size_t nv = ED->GetNumVWireSignals();
    for(size_t i=0; i<nv; i++){
      EXOVWireSignal* sig = ED->GetVWireSignal(i);
      //TPC side should not matter here
      if(not ShouldSignalSurvive(EXOMiscUtil::kVWire,EXOMiscUtil::kNorth,sig->fMagnitude,ED->fEventHeader)){
        vsToRemove.push_back(sig);
      }
    }
    for(size_t i=0;i<vsToRemove.size();i++){
      stringstream output;
      output << "Removing V signal with fMagnitude = " << vsToRemove[i]->fMagnitude;
      LogEXOMsg(output.str(),EEDebug);
      ED->Remove(vsToRemove[i]);
    }
  }

  return kOk;
} 

void EXOMCThresholdEqualizationModule::RemoveAPDSignals(EXOEventData* ED, vector<EXOAPDSignal*>& sumSignals) const
{
  vector<EXOAPDSignal*> gangsToRemove;
  for(size_t i=0; i<sumSignals.size(); i++){
    EXOMiscUtil::ETPCSide sumSide = (sumSignals[i]->fChannel == 1 ? EXOMiscUtil::kNorth : EXOMiscUtil::kSouth);
    for(size_t j=0; j<ED->GetNumAPDSignals(); j++){
      EXOAPDSignal* sig = ED->GetAPDSignal(j);
      if(sig->fType != EXOAPDSignal::kGangFit){
        continue;
      }
      /*
      if(EXOMiscUtil::GetTPCSide(sig->fChannel) != sumSide){
        continue;
      }
      */
      if(fabs(sig->fTime - sumSignals[i]->fTime) <= fAPDMatchTime){
        gangsToRemove.push_back(sig);
      }
    }
    stringstream output;
    output << "Removing APD sum signal with fRawCounts = " << sumSignals[i]->fRawCounts;
    LogEXOMsg(output.str(),EEDebug);
    ED->Remove(sumSignals[i]);
  }
  for(size_t i=0; i<gangsToRemove.size(); i++){
    ED->Remove(gangsToRemove[i]);
  }
}

int EXOMCThresholdEqualizationModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/mcthreshold/seed",
                               "Set seed for the random number generator.",
                               this,
                               (unsigned int)0,
                               &EXOMCThresholdEqualizationModule::SetSeed );

  talktoManager->CreateCommand("/mcthreshold/equalizeAPD",
                               "Set whether APD sum signals should be equalized",
                               this,
                               fEqualizeAPD,
                               &EXOMCThresholdEqualizationModule::SetEqualizeAPD );

  talktoManager->CreateCommand("/mcthreshold/equalizeU",
                               "Set whether U wire signals should be equalized",
                               this,
                               fEqualizeU,
                               &EXOMCThresholdEqualizationModule::SetEqualizeU );

  talktoManager->CreateCommand("/mcthreshold/equalizeV",
                               "Set whether V wire signals should be equalized",
                               this,
                               fEqualizeV,
                               &EXOMCThresholdEqualizationModule::SetEqualizeV );
  return 0;
}

