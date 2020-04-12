#ifndef EXOEnergyBias_hh
#define EXOEnergyBias_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

#include <string>
#include <vector>

class EXOEnergyBiasHandler : public EXOCalibHandlerBase {

public:
  EXOEnergyBiasHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-bias")
};


class EXOEnergyBias : public EXOCalibBase {
  friend class EXOEnergyBiasHandler;

public:

  EXOEnergyBias();

  double GetCo1173BiasCharge(int multiplicity) const;
  double GetCo1333BiasCharge(int multiplicity) const;
  double GetCs662BiasCharge(int multiplicity) const;
  double GetTh2615BiasCharge(int multiplicity) const;
  double GetCo1173BiasRotated(int multiplicity) const;
  double GetCo1333BiasRotated(int multiplicity) const;
  double GetCs662BiasRotated(int multiplicity) const;
  double GetTh2615BiasRotated(int multiplicity) const;

  static void SetUserValuesForCharge(double Co1173SS, double Co1333SS, double Cs662SS, double Th2615SS, double Co1173MS, double Co1333MS, double Cs662MS, double Th2615MS);
  static void SetUserValuesForRotated(double Co1173SS, double Co1333SS, double Cs662SS, double Th2615SS, double Co1173MS, double Co1333MS, double Cs662MS, double Th2615MS);
  static void UseDatabaseForCharge();
  static void UseDatabaseForRotated();
  std::string __str__() const;
  
private:

  double GetUserBiasCharge(size_t param, int multiplicity) const;
  double GetDBBiasCharge(size_t param, int multiplicity) const;
  double GetUserBiasRotated(size_t param, int multiplicity) const;
  double GetDBBiasRotated(size_t param, int multiplicity) const;

  std::vector<double> fBiasesChargeSS;  //Single Site Charge Biases (Co1, Co2, Cs, Th)
  std::vector<double> fBiasesChargeMS;  //Multi Site Charge Biases (Co1, Co2, Cs, Th)
  std::vector<double> fBiasesRotatedSS;  //Single Site Rotated Biases (Co1, Co2, Cs, Th)
  std::vector<double> fBiasesRotatedMS;  //Multi Site Rotated Biases (Co1, Co2, Cs, Th)

  static bool fUseDatabaseForCharge;
  static bool fUseDatabaseForRotated;
  static std::vector<double> fUserBiasesChargeSS;  //User-set Single Site Charge Biases (Co1, Co2, Cs, Th)
  static std::vector<double> fUserBiasesChargeMS;  //User-set Multi Site Charge Biases (Co1, Co2, Cs, Th)
  static std::vector<double> fUserBiasesRotatedSS;  //User-set Single Site Rotated Biases (Co1, Co2, Cs, Th)
  static std::vector<double> fUserBiasesRotatedMS;  //User-set Multi Site Rotated Biases (Co1, Co2, Cs, Th)
};

#endif
