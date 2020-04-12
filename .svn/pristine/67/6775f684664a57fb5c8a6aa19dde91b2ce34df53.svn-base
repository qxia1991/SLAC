#ifndef EXOEnergyCalib_hh
#define EXOEnergyCalib_hh

#include "EXOCalibUtilities/EXOEnergyMCBasedFit.hh"

#include "EXOUtilities/EXOTimestamp.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include <set>

#include "TRandom3.h" // because GSL engines do not work for me
//#include "Math/GSLRndmEngines.h"
//#include "Math/Random.h"

class EXOEnergyCalib
{
public:
  ~EXOEnergyCalib();
  static EXOEnergyCalib* GetInstanceForFlavor(std::string ratioFlavor, std::string peakFlavor, std::string biasFlavor);
  static EXOEnergyCalib* GetInstanceForFlavor(std::string weeklyFlavor, std::string averageFlavor);

  const char* GetCalibrationType() const;

  // for 1D fits
  
  double CalibratedChargeEnergy(double charge, int multiplicity, const EXOTimestamp& time) const;
  double CalibratedChargeEnergy(double charge, int multiplicity, const EXOEventHeader& header) const;
  double CalibratedChargeEnergy(double charge, int multiplicity, long int seconds, int nano) const;
  double CalibratedScintEnergy(double scint, int multiplicity, const EXOTimestamp& time) const;
  double CalibratedScintEnergy(double scint, int multiplicity, const EXOEventHeader& header) const;
  double CalibratedScintEnergy(double scint, int multiplicity, long int seconds, int nano) const;
  double CalibratedRotatedEnergy(double charge, double scint, int multiplicity, const EXOTimestamp& time) const;
  double CalibratedRotatedEnergy(double charge, double scint, int multiplicity, const EXOEventHeader& header) const;
  double CalibratedRotatedEnergy(double charge, double scint, int multiplicity, long int seconds, int nano) const;
  double RawRotatedEnergy(double charge, double scint, int multiplicity, const EXOTimestamp& time) const;
  double RawRotatedEnergy(double charge, double scint, int multiplicity, const EXOEventHeader& header) const;
  double RawRotatedEnergy(double charge, double scint, int multiplicity, long int seconds, int nano) const;

  // for MC-based 2D fits
  double Angle2D(double energy, int multiplicity, const EXOTimestamp& time) const;
  double Angle2D(double energy, int multiplicity, const EXOEventHeader& header) const;
  double Angle2D(double energy, int multiplicity, long int seconds, int nano) const;  
  
  double Rotated2D(double charge, double scint, int multiplicity, const EXOTimestamp& time) const;
  double Rotated2D(double charge, double scint, int multiplicity, const EXOEventHeader& header) const;
  double Rotated2D(double charge, double scint, int multiplicity, long int seconds, int nano) const;  

  double OrthogonalSigma2D(double energy, int multiplicity, const EXOTimestamp& time) const;
  double OrthogonalSigma2D(double energy, int multiplicity, const EXOEventHeader& header) const;
  double OrthogonalSigma2D(double energy, int multiplicity, long int seconds, int nano) const;
  
  double Distance2D(double charge, double scint, int multiplicity, const EXOTimestamp& time) const;
  double Distance2D(double charge, double scint, int multiplicity, const EXOEventHeader& header) const;
  double Distance2D(double charge, double scint, int multiplicity, long int seconds, int nano) const;  

  bool Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, const EXOTimestamp& time) const;
  bool Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, const EXOEventHeader& header) const;
  bool Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, long int seconds, int nano) const;
  

private:
  EXOEnergyCalib() {;}
  EXOEnergyCalib(std::string ratioFlavor, std::string peakFlavor, std::string biasFlavor);
  EXOEnergyCalib(std::string weeklyFlavor, std::string averageFlavor);
  
  std::string fCalibrationType;
  
  double Calibration(double rawEnergy, double ThRaw, double bias, double p0, double p1, double p2) const;
  double MCBasedFitCalibration(const std::string& channel, double energy, EXOEnergyMCBasedFit* weekly, EXOEnergyMCBasedFit* average, int multiplicity) const;
  
  static std::set<EXOEnergyCalib*> fInstances;
  
  std::string fRatioFlavor;
  std::string fPeakFlavor;
  std::string fBiasFlavor;
  double fThTrueEnergy;

  std::string fWeeklyFlavor;
  std::string fAverageFlavor;
};

#endif
