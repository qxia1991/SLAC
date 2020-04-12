//______________________________________________________________________________
// The purpose of this module is to correct the charge clusters, which are
// identified and initially measured by the reconstruction module, by the
// appropriate amount due to charge losses from poor xenon purity.  This can be
// done either with a command from the macro files or by reading in the purity
// correction from the database.
//______________________________________________________________________________
// Changelog (Changes before 1/18/11 not noted)
// 1-18-11:   Added functionality to make lifetime correction. Also added
// ability to set the lifetime in the macro file. These were imported from
// EXOATeamPurityCorrModule
//______________________________________________________________________________
// 08-04-11: Modified to read in a parameterized function from the DB, instead
// of a float. (Only polynomials are supported currently.)
// _____________________________________________________________________________
// 02-06-12: Now corrects each TPC separately if "separateTPCs" is specified for
// the flavor. Note that the flavors in the DB are "TPC1" and "TPC2". 
// _____________________________________________________________________________
// 05-12-12: Now can use manual values for each TPC separately.
// _____________________________________________________________________________
// 11-17-18: Add option to switch using two-phase purity model
// _____________________________________________________________________________


#include "EXOAnalysisManager/EXOLifetimeCalibModule.hh" 
#include "EXOCalibUtilities/EXOLifetimeCalib.hh" 
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOFlowRate.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <cmath>
#include <iostream>
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOLifetimeCalibModule, "lifecalib" )

EXOLifetimeCalibModule::EXOLifetimeCalibModule() :
  lifetime_man(0.0), 
  lifetime_manTPC1(0.0),
  lifetime_manTPC2(0.0),
  energy_scale(1.0),
  fIsInverse(false)
{
  SetFlavor("vanilla");
}

EXOAnalysisModule::EventStatus EXOLifetimeCalibModule::BeginOfRun(EXOEventData *ED)
{
  // Use database if no lifetime specified. 
  if( lifetime_man < 0.0001 && lifetime_manTPC1 < 0.0001 && lifetime_manTPC2 < 0.0001){
    LogEXOMsg("No manual lifetime supplied; will use the fit function in the database.", EENotice);
  }

  // Override separate TPC values if all TPC value is used
  if ( lifetime_man >= 0.0001 && (lifetime_manTPC1 >= 0.0001 || lifetime_manTPC2 >= 0.0001) ){
    LogEXOMsg("Manual lifetime was supplied for both entire TPC and for individual TPC. Lifetime for entire TPC will be used, and individual TPC corrections will NOT be made.", EENotice);
    // In this case, we will ignore these in the code below, but we explicitly set them to 0 here for clarity.
    lifetime_manTPC1 = 0.;
    lifetime_manTPC2 = 0.;
  }
  
  // If only 1 TPC is specified, do not apply a correction
  if ( lifetime_man < 0.0001){
    if ( (lifetime_manTPC1 >= 0.0001 && lifetime_manTPC2 < 0.001) ||
	 (lifetime_manTPC1 < 0.0001 && lifetime_manTPC2 >= 0.001) ){
      LogEXOMsg("Lifetime was only specified for one TPC. The value will be ignored and the lifetime from the DB will be applied.", EENotice);
      // In this case, set to 0 so they will be ignored later. 
      lifetime_manTPC1 = 0.;
      lifetime_manTPC2 = 0.;
    }
  }
  
  return kOk;
}

void EXOLifetimeCalibModule::SetFlavor(std::string flavor) 
{
  // Set the flavor
  if (flavor == "separateTPCs"){
    m_flavor[0] = "TPC1"; 
    m_flavor[1] = "TPC2"; 
    m_flavor[2] = "TPC1";
    m_flavor[3] = "TPC2";

  }
  else if (flavor == "separateTPCs_2013_0nu"){
    m_flavor[0] = "TPC1_2013_0nu"; 
    m_flavor[1] = "TPC2_2013_0nu"; 
    m_flavor[2] = "TPC1_2013_0nu";
    m_flavor[3] = "TPC2_2013_0nu";
  }
  else if (flavor == "separateTPCs_MC_v2"){
    m_flavor[0] = "MC-v2-TPC1"; 
    m_flavor[1] = "MC-v2-TPC2"; 
    m_flavor[2] = "MC-v2-TPC1";
    m_flavor[3] = "MC-v2-TPC2";

  }
  else if (flavor == "separateTPCs_MC_2D_v1") {
    m_flavor[0] = "MC-2D-v1-TPC1";  
    m_flavor[1] = "MC-2D-v1-TPC2";  
    m_flavor[2] = "MC-2D-v1-TPC1";
    m_flavor[3] = "MC-2D-v1-TPC2";

  }
  else if (flavor =="separateTPCs_andWires_MC_v1"){
       m_flavor[0] = "MC-v1-TPC1-1wire";
       m_flavor[1] = "MC-v1-TPC2-1wire";
       m_flavor[2] = "MC-v1-TPC1-2wire";
       m_flavor[3] = "MC-v1-TPC2-2wire";
  }
  else if (flavor == "separateTPCs_andWires_MC_v2"){
       m_flavor[0] = "MC-v2-TPC1-1wire";
       m_flavor[1] = "MC-v2-TPC2-1wire";
       m_flavor[2] = "MC-v2-TPC1-2wire";
       m_flavor[3] = "MC-v2-TPC2-2wire";
  }
  else if (flavor == "separateTPCs_andWires_MC_v4"){
       m_flavor[0] = "MC-v4-TPC1-1wire";
       m_flavor[1] = "MC-v4-TPC2-1wire";
       m_flavor[2] = "MC-v4-TPC1-2wire";
       m_flavor[3] = "MC-v4-TPC2-2wire";
  }
  else if (flavor == "inverse_separateTPCs_andWires_MC_v1"){
       m_flavor[0] = "MC-inverse-TPC1-1wire";
       m_flavor[1] = "MC-inverse-TPC2-1wire";
       m_flavor[2] = "MC-inverse-TPC1-2wire";
       m_flavor[3] = "MC-inverse-TPC2-2wire";
       fIsInverse = true;
       std::cout << "Using Inverse Lifetime" << std::endl;
  }
  else if (flavor == "TPC1" || flavor == "TPC2"){ 
    LogEXOMsg("Selecting flavor TPC1 or TPC2 is not implemented. To correct each TPC half separately, choose flavor 'separateTPCs'.", EEAlert);
  }
  else if (flavor.find("separateTPCs") != std::string::npos) {
    // This is a separateTPC general command, it should be of the form:
    //    separateTPCs TPC1:[flavor_TPC1] TPC2:[flavor_TPC2]
    std::vector<std::string> types(4);
    types[0] = "TPC1:"; 
    types[1] = "TPC2:"; 
    types[2] = "TPC1:";
    types[3] = "TPC2:";
    try {
      for (size_t i=0;i<types.size();i++) {
        size_t apos = flavor.find(types[i]); 
        if (apos == std::string::npos) throw 1; 
        std::string this_type = flavor.substr(apos + types[i].size());
        size_t anend = this_type.find_first_of(" \t\n");
        this_type = this_type.substr(0, anend); 
        if (this_type.size() == 0) throw 2;
        m_flavor[i] = this_type;
      }
    } catch (int e) {
      LogEXOMsg("flavor could not be parsed, separateTPCs needs to be in the form: separateTPCs TPC1:[flavor_TPC1] TPC2:[flavor_TPC2]", EEAlert);
    }
  }
  else { 
    // Assume the same flavor for both TPCs
    m_flavor[0] = flavor; 
    m_flavor[1] = flavor; 
    m_flavor[2] = flavor;
    m_flavor[3] = flavor;
  }

}

int EXOLifetimeCalibModule::GetNumWires(EXOChargeCluster *cc){
  std::set<Int_t> wire_channels;
  for(size_t i=0;i<cc->GetNumUWireSignals();i++) {
    wire_channels.insert(cc->GetUWireSignalAt(i)->fChannel);
  }
  size_t nuwire = wire_channels.size();
  return nuwire;
}



EXOAnalysisModule::EventStatus EXOLifetimeCalibModule::ProcessEvent(EXOEventData *ED)
{
  double lifetime = 0; // storage
  double event_time = ED->fEventHeader.fTriggerSeconds;
  int ncl = ED->GetNumChargeClusters();
  double origin;
  double zcl;
  int nws;
  std::string m_flavorTmp;
  EXOChargeCluster * charge_clus;
  
  for( int i = 0; i < ncl; i++){
    charge_clus = ED->GetChargeCluster(i);
    charge_clus->fPurityCorrectedEnergy    = 0.0;
    charge_clus->fPurityCorrectedEnergyMix = 0.0;
    zcl = charge_clus->fZ;
    nws = GetNumWires(charge_clus);

    // Get lifetime from database if manually entered value is <= 0. 
    if ( lifetime_man < 0.0001 && lifetime_manTPC1 < 0.0001 && lifetime_manTPC2 < 0.0001){
      // Here m_flavor could be vanilla, TPC1, TPC2, or separateTPCs.
      // If "separateTPCs", check z-position and grab calib for that side of TPC
      // (In database, these will be "TPC1" or "TPC2")
      if (zcl > 0.0 and nws < 1.5){ m_flavorTmp = m_flavor[0]; }
      if (zcl > 0.0 and nws > 1.5){ m_flavorTmp = m_flavor[2]; }
      if (zcl < 0.0 and nws < 1.5){ m_flavorTmp = m_flavor[1]; }
      if (zcl < 0.0 and nws > 1.5){ m_flavorTmp = m_flavor[3]; }
      EXOLifetimeCalib* aCalib = GetCalibrationFor(EXOLifetimeCalib,
						   EXOLifetimeCalibHandler, 
						   m_flavorTmp, 
						   ED->fEventHeader);
      
      if (aCalib == NULL) {
	LogEXOMsg("Lifetime data not found. fChargeClusters.fPurityCorrectedEnergy will be filled with zero", EECritical); 
	return kOk;
      }

      double flow = -999;
      if (aCalib->getFuncType()=="twophase"){
        // use twophase purity model
        // need to get the flow rate
        EXOFlowRate* flowRate = GetCalibrationFor(EXOFlowRate,
						  EXOFlowRateHandler, 
						  m_flowFlavor, 
						  ED->fEventHeader);
        if (flowRate == NULL) {
          LogEXOMsg("Flow rate data not found. Check flow rate flavor are set correctly or switch to use polinominal fit for purity.", EECritical); 
          // FIXME
          // do something to exit here???
          return kOk;
        }
        flow = flowRate->flow(event_time);
      
      }


      // Get the lifetime from the calibration
      lifetime = aCalib->lifetime(event_time, flow);
      //std::cout << "Lifetime " << lifetime << std::endl;
      if(fIsInverse) {
          lifetime = 1.e6/lifetime; 
          //std::cout << "Lifetime Inverse " << lifetime << std::endl;
      }
      if (lifetime < 0.0001){
	LogEXOMsg("Negative or zero lifetime obtained. fChargeClusters.fPurityCorrectedEnergy will be filled with zeroes", EECritical);
	return kOk;
      }
    }
  
    // Use manually entered lifetime if it exists
    else if (lifetime_man > 0.0001){ // ignore individual TPC manual values if this value is set
      lifetime = (lifetime_man)*CLHEP::microsecond;
    } 
    else if (lifetime_man < 0.0001){
      if (lifetime_manTPC1 >= 0.0001 && lifetime_manTPC2 >= 0.0001){
	if (zcl > 0.0){ lifetime = (lifetime_manTPC1)*CLHEP::microsecond; }
	if (zcl < 0.0){ lifetime = (lifetime_manTPC2)*CLHEP::microsecond; }
      }
    }

    else {
      LogEXOMsg("Unrecognized combination of manual lifetimes entered. Making no correction.", EEWarning);
      lifetime = (1000000000.)*CLHEP::microsecond;
    }
    
    // Apply the correction
    charge_clus = ED->GetChargeCluster(i);
    charge_clus->fPurityCorrectedEnergy = 
      charge_clus->fCorrectedEnergy * (energy_scale) * exp( (charge_clus->fDriftTime) / lifetime );
    charge_clus->fPurityCorrectedEnergyMix =
              charge_clus->fCorrectedEnergyMix * (energy_scale) * exp( (charge_clus->fDriftTime) / lifetime );
    if (zcl > -1000.){
      /*cout << charge_clus->fDriftTime;
      cout << " " << lifetime << " ";
      cout << charge_clus->fCorrectedEnergy << " ";
      cout << charge_clus->fPurityCorrectedEnergy << endl;
      */
    }
  }
  
  return kOk;
}

int EXOLifetimeCalibModule::TalkTo(EXOTalkToManager *talktoManager)
{

  //  Make a command to get calibration flavor
  talktoManager->CreateCommand("/lifecalib/flavor", 
                               "Calibration flavor. Use 'separateTPCs' to correct separately, "
                               "in the form 'separateTPCs TPC1:[flavorTPC1] TPC2:[flavorTPC2]'",
                               this,
                               "vanilla",
                               &EXOLifetimeCalibModule::SetFlavor);
  talktoManager->CreateCommand("/lifecalib/flowFlavor", 
                               "Flow rate flavor in DB to be used, needed if twophase model is used",
                               this,
                               "vanilla",
                               &EXOLifetimeCalibModule::SetFlowFlavor);

  //  Make a command to accept manual purity value
  talktoManager->CreateCommand("/lifecalib/manual", 
                               "Enter e- lifetime in microsec for whole TPC. Defaults to DB.",
                               this, lifetime_man, &EXOLifetimeCalibModule::Setlifetime_man );

  //  Make a command to accept manual purity values for TPC 1
  talktoManager->CreateCommand("/lifecalib/manualTPC1",
			       "Enter e- lifetime in microsec for TPC 1. Do not use with 'manual'.", 
			       this, lifetime_manTPC1, &EXOLifetimeCalibModule::Setlifetime_manTPC1 );

  //  Make a command to accept manual purity values for TPC 2
  talktoManager->CreateCommand("/lifecalib/manualTPC2",
			       "Enter e- lifetime in microsec for TPC 2. Do not use with 'manual'.", 
			       this, lifetime_manTPC2, &EXOLifetimeCalibModule::Setlifetime_manTPC2 );
  
  //  ...maybe also level specification.  Possible form would be 
  //  character string with delimiters, e.g. "PROD DEV" would indicate
  // first try for PROD, then DEV

  //  Make a command to set correction to the energy scale
  talktoManager->CreateCommand("/lifecalib/energy_scale","Enter energy scale correction factor. Defaults to 1.0.", this, energy_scale, &EXOLifetimeCalibModule::Setenergy_scale );

  return 0;
}
