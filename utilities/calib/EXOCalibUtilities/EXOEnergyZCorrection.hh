#ifndef EXOEnergyZCorrection_hh
#define EXOEnergyZCorrection_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOUtilities/EXOTimestamp.hh"
#include "EXOUtilities/EXOEventHeader.hh"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

#include "TString.h"

class EXOEnergyZCorrectionHandler : public EXOCalibHandlerBase {

public:
  EXOEnergyZCorrectionHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-z-correction")
};


class EXOEnergyZCorrection : public EXOCalibBase {
  friend class EXOEnergyZCorrectionHandler;

public:

  EXOEnergyZCorrection();
  static std::map<std::string, double> GetTemplatePars();
  static std::vector<std::string> GetParNamesInDB();
  static std::map<std::string, int> GetEqnTypeMap();

  static double GetZcorrection(double z, std::string flavor, const EXOEventHeader& header);
  static double GetZcorrection(double z, std::string flavor, long int seconds, int nano);
  static double GetZcorrection(double z, std::string flavor, const EXOTimestamp& time);
  
  static void UseDatabase(bool useDB);
  double GetParByName(const char* name) const;
  int GetEquationTypeAsInt(const char* name) const;
  static void SetUserValues(const std::map<std::string, double>& userValues);
  std::string __str__() const;
  
private:
  
  static bool fUseDB;
  static std::vector<std::string> fParNamesInDB;

  static std::map<std::string, int> fEqnType;
  std::map<std::string, double> fDBPars;
  static std::map<std::string, double> fUserPars;

  static double EvalLinear(double z, EXOEnergyZCorrection* zCorr);
  static double EvalPower(double z, EXOEnergyZCorrection* zCorr);

};

#endif
