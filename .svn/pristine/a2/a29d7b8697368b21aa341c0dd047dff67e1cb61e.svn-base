#ifndef EXODigitizeModule_hh
#define EXODigitizeModule_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOUtilities/EXODigitizeAPDs.hh"
//#include "EXOUtilities/EXODigitizeWires.hh"
#include "EXOUtilities/EXOSmearMCIonizationEnergy.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOTrimWaveforms.hh"
#include <string>
#include "EXOUtilities/EXO3DDigitizeWires.hh"

class EXODigitizeModule : public EXOAnalysisModule 
{

protected:
  EXOSmearMCIonizationEnergy fSmearMCIonizationEnergy;
  EXODigitizeAPDs            fDigAPDs;
  EXO3DDigitizeWires           fDigWires;
  //EXODigitizeWires           fDigWires;
  EXOTrimWaveforms           fTrimWaveforms;
  bool                       fForceUseInternalSamples;
  int                        fNumSamples;
  double                     fWireNoiseMagnitude;
  double                     fAPDNoiseMagnitude;
  unsigned int               fUnixTimeOfEvent;
  std::string                fElectronicsDatabaseFlavor;
  bool                       fDoIDigitizeAPDs;
  bool                       fDoIDigitizeWires;
  std::string                fMCScalingDatabaseFlavor;
  std::string                fUWireDatabaseFlavor;
  std::string                fVWireDatabaseFlavor;
  EXOTimingStatisticInfo     fTimingInfo; // Timing information for digitization
  double                     fTriggerTime; // Mirrors times held in wire/APD digitizers
  double                     fACSmearSigma; //sigma used to determine the fraction of charge lost to light on per event basis for AC model
public :

  EXODigitizeModule();
  ~EXODigitizeModule();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);

  void SetDigitizeAPDs(bool apds = true);
  void SetDigitizeWires(bool wires = true);
  void SetDigitizationTime(double time);
  void SetWireNoise(double noise);
  void SetAPDNoise(double noise);
  void SetLXeEnergyResolution(double res);
  void SetElectronLifetime(double lifetime);
  void SetDriftVelocity(double drift_vel);
  void SetCollectionDriftVelocity(double drift_vel);
  void SetDigitizeInduction(bool dig_induction = true);
  void SetTriggerTime(double trig_time);
  void SetUnixTimeOfEvent(unsigned int atime)
    { fUnixTimeOfEvent = atime; }
  void SetElectronicDatabaseFlavor(std::string aval) 
      { fElectronicsDatabaseFlavor = aval; }
  void SetMCScalingDatabaseFlavor(std::string aval)
      { fMCScalingDatabaseFlavor = aval; }
  void SetUWireDatabaseFlavor(std::string aval)
      {fUWireDatabaseFlavor = aval;}
  void SetVWireDatabaseFlavor(std::string aval)
      {fVWireDatabaseFlavor = aval;}
  void SetACSmearSigma(double aval)
      {fACSmearSigma = aval; }

  void SetWeightPotentialFiles(std::string);
  void SetElectricFieldFile(std::string);

  DEFINE_EXO_ANALYSIS_MODULE( EXODigitizeModule )

};
#endif

  


