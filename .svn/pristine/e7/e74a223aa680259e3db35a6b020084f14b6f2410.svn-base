//______________________________________________________________________________
//
// EXODigitizeSignalModule
//
// Provides a module to directly deposit points of charge 
// (x,y,z,t, energy) and apd signal hits (gang number, time, charge) 
//
// Created M. Marino, March 2011
//______________________________________________________________________________

#include "EXOAnalysisManager/EXODigitizeSignalModule.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <sstream>
using namespace std;
using CLHEP::keV;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXODigitizeSignalModule, "exodig" )

EXODigitizeSignalModule::EXODigitizeSignalModule() : 
  fEventNumber(0)
{
}

//______________________________________________________________________________
EXOEventData* EXODigitizeSignalModule::GetNextEvent()
{
  // GetNextEvent returns an EXOEventData object.  It loops over its internal
  // vectors, generating waveforms at points defined by AddChargePoint,
  // AddAPDHit, etc.

  if (fChargeDeps.empty() and fAPDHits.empty()) return NULL;

  fEventNumber++;
  fEventData.Clear("C");
  EXOMonteCarloData& mc_data = fEventData.fMonteCarloData;
  if (not fChargeDeps.empty()) {
    std::vector<EXOMCPixelatedChargeDeposit>& tempVec = fChargeDeps.front();
    for(size_t i = 0; i < tempVec.size(); i++) {
      EXOMCPixelatedChargeDeposit* pc = mc_data.FindOrCreatePixelatedChargeDeposit(tempVec[i].GetPixelCenter());
      pc->fTotalEnergy += tempVec[i].fTotalEnergy;
      pc->fTotalIonizationEnergy += tempVec[i].fTotalIonizationEnergy;
    }
    fChargeDeps.pop();
  }
  if (not fAPDHits.empty()) {
    std::vector<EXOMCAPDHitInfo>& tempVec = fAPDHits.front();
    for (size_t i=0;i<tempVec.size();i++) {
      mc_data.AddAPDHitInfo(tempVec[i]);
    }
    fAPDHits.pop();
  }

  fEventData.fEventNumber = fEventNumber;
  fEventData.fEventHeader.fIsMonteCarloEvent = true;

  return &fEventData;
}

//______________________________________________________________________________
int EXODigitizeSignalModule::TalkTo( EXOTalkToManager* talkTo )
{
  talkTo->CreateCommand("/exodig/addchargepoint", 
                        "Add charge pixel (position in mm, time in ns, energy in keV): x[double] y[double] z[double] time[double] energy[double]",
                        this,
                        "",
                        &EXODigitizeSignalModule::AddChargePointWithString);

  talkTo->CreateCommand("/exodig/addapdhit", 
                        "Add APD hit: gangNumber[int] time[double] charge[double]",
                        this,
                        "",
                        &EXODigitizeSignalModule::AddAPDHitWithString);

  talkTo->CreateCommand("/exodig/clear", 
                        "Clear all points",
                        this,
                        &EXODigitizeSignalModule::Clear);

  talkTo->CreateCommand("/exodig/nextevent", 
                        "When loading mutiple events, continue to next event",
                        this,
                        &EXODigitizeSignalModule::NextEvent);

  return 0;
}

//______________________________________________________________________________
void EXODigitizeSignalModule::AddChargePointWithString( std::string input )
{
  // Set function for TalkTo functionality.  Users calling this function
  // directly will generally use AddCharePoint instead.

  std::istringstream os(input);
  double xp, yp, zp, time, energy;
  os >> xp >> yp >> zp >> time >> energy; 
  if ( !os.fail() ) AddChargePoint(xp, yp, zp, time, energy);
}

//______________________________________________________________________________
void EXODigitizeSignalModule::AddChargePoint(double xpos, 
                                             double ypos, 
                                             double zpos, 
                                             double time, double energy)
{
  // Add a charge point at a given x, y, z position (in mm), at a certain time (in ns)
  // and with the given energy (in keV).
  const EXOCoordinates Coord(EXOMiscUtil::kXYCoordinates, xpos, ypos, zpos, time);
  EXOMCPixelatedChargeDeposit pc(Coord);
  pc.fTotalEnergy = energy*keV;
  pc.fTotalIonizationEnergy = energy*keV;
  AddChargePoint(pc);
}

//______________________________________________________________________________
void EXODigitizeSignalModule::AddAPDHitWithString(std::string hitString)
{
  // Set function for TalkTo functionality.  Users calling this class directly
  // (in code or scripts) will generally want to use AddAPDHit directly.
  std::istringstream os(hitString);
  int gangNo; 
  double time, charge;
  os >> gangNo >> time >> charge;
  if ( !os.fail() ) AddAPDHit(gangNo, time, charge);
}

//______________________________________________________________________________
void EXODigitizeSignalModule::AddAPDHit(int gangNo, double time, double charge)
{
  // Add an APD at a given gangNo, at a certain time and with a given charge.
  EXOMCAPDHitInfo apd;
  apd.fGangNo = gangNo;
  apd.fTime   = time;
  apd.fCharge = charge;
  AddAPDHit(apd);
}
//____________________________________________________________________________
void EXODigitizeSignalModule::Clear()
{
  // Clear all queued charge and APD hits
  while(not fChargeDeps.empty()) fChargeDeps.pop();
  while(not fAPDHits.empty()) fAPDHits.pop();
  fEventNumber = 0;
}
