#ifndef EXOEnergyRatio_hh
#define EXOEnergyRatio_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

#include <string>
#include <vector>

class EXOEnergyRatioHandler : public EXOCalibHandlerBase {

public:
  EXOEnergyRatioHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-ratio")
};


class EXOEnergyRatio : public EXOCalibBase {
  friend class EXOEnergyRatioHandler;

public:

  EXOEnergyRatio();

  enum SignalType {NoSignal,
		   RotatedSignal,
		   ChargeSignal,
		   ScintSignal};
  static void SetUserChargeValues(double P0SS,
                                  double P1SS,
                                  double P2SS,
                                  double P0MS,
                                  double P1MS,
                                  double P2MS);

  static void SetUserScintValues(double P0SS,
                                 double P1SS,
                                 double P2SS,
                                 double P0MS,
                                 double P1MS,
                                 double P2MS);

  static void SetUserRotatedValues(double P0SS,
                                   double P1SS,
                                   double P2SS,
                                   double P0MS,
                                   double P1MS,
                                   double P2MS);

  static void UseDatabaseForCharge(){fUseDatabaseForCharge = true;}
  static void UseDatabaseForScint(){fUseDatabaseForScint = true;}
  static void UseDatabaseForRotated(){fUseDatabaseForRotated = true;}

  double GetChargeCalibrationParameter(size_t param,
                                       int multiplicity) const;

  double GetScintCalibrationParameter(size_t param,
                                      int multiplicity) const;

  double GetRotatedCalibrationParameter(size_t param,
                                        int multiplicity) const;

  std::string __str__() const;
  
private:

  // Single site calibration constants
  std::vector<double> fChargeParamsSS;     //Charge calibration parameters
  std::vector<double> fScintParamsSS;      //Scintillation calibration parameters
  std::vector<double> fRotatedParamsSS;    //Rotated calibration parameters

  //Multi site calibration constants
  std::vector<double> fChargeParamsMS;     //Charge calibration parameters
  std::vector<double> fScintParamsMS;      //Scintillation calibration parameters
  std::vector<double> fRotatedParamsMS;    //Rotated calibration parameters

  //User-set parameters
  static bool fUseDatabaseForCharge;
  static bool fUseDatabaseForScint;
  static bool fUseDatabaseForRotated;

  static std::vector<double> fUserChargeParamsSS;     //User provided Charge calibration parameters
  static std::vector<double> fUserScintParamsSS;      //User provided Scintillation calibration parameters
  static std::vector<double> fUserRotatedParamsSS;    //User provided Rotated calibration parameters

  static std::vector<double> fUserChargeParamsMS;     //User provided Charge calibration parameters
  static std::vector<double> fUserScintParamsMS;      //User provided Scintillation calibration parameters
  static std::vector<double> fUserRotatedParamsMS;    //User provided Rotated calibration parameters

};

#endif
