#ifndef EXOLifetimeCalibModule_hh
#define EXOLifetimeCalibModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
class EXOEventData;
class EXOTalkToManager;
class EXOChargeCluster;

//-----------------------------------------------------------------------
// Changelog: Not updated before 1/18/11
// 1/18/11:   Updated to include functionality to make a simple 
//            z-dependent correction once the lifetime is read in.
//-----------------------------------------------------------------------

/*
    The class EXOLifetimeCalibModule exemplifies two functionalities
    which need not - and for "real" calibrations probably often won't - be 
    handled by the same class:

      - registers a handler for a calibration type at init time
      - fetches current calibration of that type at BeginOfEvent time

    The handler for a particular calibration type should be registered 
    only once, but there can be several clients which read that calibration
    type.  If there are more than one, they can ask for the same flavor
    or different flavors.  
*/
class EXOLifetimeCalibModule : public EXOAnalysisModule 
{

private :

  // flavor is a characteristic of calibration readers, not handlers
  std::string       m_flavor[4];
  std::string       m_flowFlavor; // flowRate flavor

  // cache serial number of fetched calibration.  If it changes for
  // next event, we'll know to recompute any derived quantities

  const std::string    m_ourType;
  double               lifetime_man;
  double               lifetime_manTPC1;
  double               lifetime_manTPC2;
  double               energy_scale;
  bool                 fIsInverse;

protected:

public :

  EXOLifetimeCalibModule();

  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  int TalkTo(EXOTalkToManager *tm);
  void SetFlavor(std::string aval);
  void SetFlowFlavor(std::string aval){ m_flowFlavor=aval; }
  void Setlifetime_man(double aval) { lifetime_man = aval; }
  void Setlifetime_manTPC1(double aval) { lifetime_manTPC1 = aval; }
  void Setlifetime_manTPC2(double aval) { lifetime_manTPC2 = aval; }
  void Setenergy_scale(double aval) { energy_scale = aval; } 
  int GetNumWires(EXOChargeCluster *cc);

  DEFINE_EXO_ANALYSIS_MODULE( EXOLifetimeCalibModule )

};
#endif
