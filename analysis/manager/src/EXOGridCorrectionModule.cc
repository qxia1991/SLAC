//______________________________________________________________________________
// Module to implement a z-dependent correction for the shielding grid inefficiency.
//
// Originally created 3/29/12 by Simon Slutsky (SS)
//
// Initially, the grid correction took the form 1/(1 + A/(|z| - B)), where A and B are found elsewhere by
// fitting the full-energy peak position of z-slices of simulated thorium-228 source data. 
// A is related to shielding grid inefficiency, and B is the anode position
// So E_0 = E_meas / (1 + A/(|z| - B)), where E_0 is the unaffected peak energy at infinity.
// Effect depends on the number of U-Wires collecting charge. Use the 4 U-Wire correction
// for any signal with 4 or more collection wires. (Right now in fact the parameters are 
// set so that any signal with 2 or more collection wires is the same, but this is explicit
// in the DB. SS 03/30/12)
//
// There are now two additional grid correction functional forms available:
//     1. E_meas = E_0*(1.0 / (1 + p_0*exp((|z| - 192.5)/p_1))) (added by Mike Marino, 3/25/13)
//     2. E_meas = E_0*( (1.0 + p_0*|z|) / (1 + p_2*exp((|z| - 192.5)/p_3))) (added by Josiah Walton, 7/9/13)
//

#include "EXOAnalysisManager/EXOGridCorrectionModule.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOGridCorrectionCalib.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOUWireSignal.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TMath.h"
#include <iostream> 

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOGridCorrectionModule, "gridcorr")

EXOGridCorrectionModule::EXOGridCorrectionModule()
  : fGridCorrDatabaseFlavor("vanilla")
{}

EXOAnalysisModule::EventStatus EXOGridCorrectionModule::ProcessEvent(EXOEventData* ED)
{
  
  EXOGridCorrectionCalib* aCalib = 
    GetCalibrationFor(EXOGridCorrectionCalib, EXOGridCorrectionCalibHandler,
		      fGridCorrDatabaseFlavor, ED->fEventHeader);
  
  for (unsigned int i = 0; i < ED->GetNumChargeClusters(); i++) {
    EXOChargeCluster * ecc = ED->GetChargeCluster(i);
    double correction = GetGridCorrection(aCalib, ecc);
    ecc->fCorrectedEnergy          *= correction;
    ecc->fPurityCorrectedEnergy    *= correction;
    
    if (ecc->fCorrectedEnergy>0.1 and ecc->fCorrectedEnergyMix<0.1){
        //If Mixed Energy is empty than there was no mixed signal.  Fill in so this is the same as Old Result
        ecc->fCorrectedEnergyMix       = ecc->fCorrectedEnergy;
        ecc->fPurityCorrectedEnergyMix = ecc->fPurityCorrectedEnergy;
    }
    else{
        //If Mixed Energy is filled than apply the corrections to the mixed Signals
        ecc->fCorrectedEnergyMix       *= correction;
        ecc->fPurityCorrectedEnergyMix *= correction;
    }
    
    // Note: we also update fPurityCorrectedEnergy - this way even if the purity
    // module runs before the grid correction module, the PurityCorrectedEnergy
    // will still get the grid correction. S. Slutsky and C. Davis 03/29/12
    // 1.) fRawEnergy --grid--> fRaw*gridcorr --purity-->fRaw*gridcorr*purcorr
    // 2.) fRawEnergy --purity--> fRaw*purcorr --grid-->fRaw*purcorr*gridcorr
  }
    
  return kOk;
}

double EXOGridCorrectionModule::GetGridCorrection(EXOGridCorrectionCalib * aCalib,
						  EXOChargeCluster * ecc) 
{
  // Correction of form 1 + A/(|z| - z_anode), where A and z_anode depend on # U-Wires 
  double A = 0;
  double z_anode = 0;
  double p_1 = 0;
  double p_2 = 0;
  double p_3 = 0;
  std::set<Int_t> wire_channels;
  for(size_t i=0;i<ecc->GetNumUWireSignals();i++) {
    wire_channels.insert(ecc->GetUWireSignalAt(i)->fChannel);
  }
  size_t nuwire = wire_channels.size(); 

  const std::string& type = aCalib->GetEquationType();
  const std::vector<double>& params_read = aCalib->GetGridCorrParameters();

  if (params_read.size() < 8){ // 8 is enough to hold scale and anode pos for nuwires = 1 through 4
    LogEXOMsg("Bad parameters returned from DB", EEWarning);
    return 1;
  }

  if ((type == "" || type == "1/x") || type == "exp") {
    switch (nuwire) {
      case 0: return 1;
      case 1: 
        A = params_read[0];
        z_anode = params_read[0+4];
        break;
      case 2: 
        A = params_read[1];
        z_anode = params_read[1+4];
        break;
      case 3: 
        A = params_read[2];
        z_anode = params_read[2+4];
        break;
      case 4: 
      default:
        // Note that nuwire == 4 is the last case explicitly included in the DB
        A = params_read[3];
        z_anode = params_read[3+4];
        break;
    };
  } else if (type == "linear_exp") {
    switch (nuwire) {
      case 0: return 1;
      case 1:
        p_1 = params_read[0];
        p_2 = params_read[0+2];
        p_3 = params_read[0+4];
        break;
      case 2:
        p_1 = params_read[1];
        p_2 = params_read[1+2];
        p_3 = params_read[1+4];
        break;
      default:
        p_1 = params_read[0];
        p_2 = params_read[0+2];
        p_3 = params_read[0+4];
        break;
    };
  } else {
    return 1;
  }

  // Note that if A and z_anode are not set, they default to 0 and 
  // a unity correction is returned (i.e., no correction)

  double correction = 1;
  double z_pos = fabs(ecc->fZ);
  // Ensure that we are within the TPC
  if (z_pos > CATHODE_APDFACE_DISTANCE) return correction;  

  if (type == "" || type == "1/x") {
    correction = 1./(1 + A/(z_pos - z_anode));
  } else if (type == "exp") {
    correction = 1 + A*TMath::Exp((z_pos - 192.5)/z_anode);
  } else if (type == "linear_exp") {
    correction = ((1.0 + p_2*TMath::Exp((z_pos - 192.5)/p_3))/(1.0 + p_1*z_pos));
  } else {
    LogEXOMsg("Unknown Grid correction equation type", EEError);
  }
  return correction;
  
}

void EXOGridCorrectionModule::SetGridCorrDatabaseFlavor(std::string flavor)
{
  fGridCorrDatabaseFlavor = flavor;
}

int EXOGridCorrectionModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/gridcorr/gridcorrDBFlavor",
                               "Set the flavor string used to query the database for grid correction",
                               this,
                               fGridCorrDatabaseFlavor,
                               &EXOGridCorrectionModule::SetGridCorrDatabaseFlavor);
  return 0;
}


