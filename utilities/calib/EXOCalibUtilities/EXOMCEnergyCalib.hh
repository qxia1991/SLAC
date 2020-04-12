#ifndef EXOMCEnergyCalib_hh
#define EXOMCEnergyCalib_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

#include <string>
#include <vector>

class EXOMCEnergyCalibHandler : public EXOCalibHandlerBase {

public:
  EXOMCEnergyCalibHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-calib-mc")
};


class EXOMCEnergyCalib : public EXOCalibBase {
  friend class EXOMCEnergyCalibHandler;

public:

  EXOMCEnergyCalib();

  enum SignalType {NoSignal,
		   RotatedSignal,
		   ChargeSignal,
		   ScintSignal};
  static void SetUserChargeValues(double P0SS,
                                  double P1SS,
                                  double P0MS,
                                  double P1MS);

  double GetChargeCalibrationParameter(size_t param,
                                       int multiplicity) const;

  double CalibratedChargeEnergy(double charge,
                                int multiplicity) const;

  static void UseDatabaseForCharge(){fUseDatabaseForCharge = true;}

  std::string __str__() const;
  
private:

  // Single site calibration constants
  std::vector<double> fChargeParamsSS;     //Charge calibration parameters

  //Multi site calibration constants
  std::vector<double> fChargeParamsMS;     //Charge calibration parameters

  //User-set parameters
  static bool fUseDatabaseForCharge;

  static std::vector<double> fUserChargeParamsSS;     //User provided Charge calibration parameters

  static std::vector<double> fUserChargeParamsMS;     //User provided Charge calibration parameters

};

#endif
