#ifndef EXOEnergyResCalib_hh
#define EXOEnergyResCalib_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOCalibUtilities/EXOEnergyMCBasedFit.hh"
#include <string>

class EXOEnergyResCalibHandler : public EXOCalibHandlerBase {

public:
  EXOEnergyResCalibHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-resolution")
};

class EXOEnergyResCalib : public EXOCalibBase {
  friend class EXOEnergyResCalibHandler;

public:

  EXOEnergyResCalib();

  static void SetUserValuesForCharge(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS);
  static void SetUserValuesForScint(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS);
  static void SetUserValuesForRotated(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS);

  static void UseDatabaseForCharge() {fUseDatabaseForCharge = true;}
  static void UseDatabaseForScint() {fUseDatabaseForScint = true;}
  static void UseDatabaseForRotated() {fUseDatabaseForRotated = true;}

  static void UseMCBasedFitCalibration(EXOEnergyMCBasedFit* mcbasedFit);
  
  double ScintillationResolution(double energy, int multiplicity) const;
  std::string ScintillationResolutionString(std::string EnergyLabel, int multiplicity) const;
  double ChargeResolution(double energy, int multiplicity) const;
  std::string ChargeResolutionString(std::string EnergyLabel, int multiplicity) const;
  double RotatedResolution(double energy, int multiplicity) const;
  std::string RotatedResolutionString(std::string EnergyLabel, int multiplicity) const;
  std::string __str__() const;
  
private:
  double Model(double energy, double p0_2, double p1_2, double p2_2) const;
  
  std::vector<double> fChargeSS;   //Single site charge resolution coefficients
  std::vector<double> fChargeMS;   //Multi site charge resolution coefficients
  std::vector<double> fScintSS;    //Single site scintillation resolution coefficients
  std::vector<double> fScintMS;    //Multi site scintillation resolution coefficients
  std::vector<double> fRotatedSS;  //Single site rotated resolution coefficients
  std::vector<double> fRotatedMS;  //Multi site rotated resolution coefficients

  //User set values
  static std::vector<double> fUserChargeSS;   //User set Single site charge resolution coefficients
  static std::vector<double> fUserChargeMS;   //User set Multi site charge resolution coefficients
  static std::vector<double> fUserScintSS;    //User set Single site scintillation resolution coefficients
  static std::vector<double> fUserScintMS;    //User set Multi site scintillation resolution coefficients
  static std::vector<double> fUserRotatedSS;  //User set Single site rotated resolution coefficients
  static std::vector<double> fUserRotatedMS;  //User set Multi site rotated resolution coefficients

  static bool fUseDatabaseForCharge;
  static bool fUseDatabaseForScint;
  static bool fUseDatabaseForRotated;

};

#endif
