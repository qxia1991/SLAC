#ifndef EXOEnergyMCBasedFit_hh
#define EXOEnergyMCBasedFit_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

#include "TString.h"

class EXOEnergyMCBasedFitHandler : public EXOCalibHandlerBase {

public:
  EXOEnergyMCBasedFitHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("energy-mcbased-fit")
};


class EXOEnergyMCBasedFit : public EXOCalibBase {
  friend class EXOEnergyMCBasedFitHandler;

public:

  EXOEnergyMCBasedFit();
  static std::map<std::string, double> GetTemplatePars();
  static std::vector<std::string> GetParNamesInDB();

  static void UseDatabase(bool useDB);
  double GetParByName(const char* name, int multiplicity) const;
  static void SetUserValues(const std::map<std::string, double>& userValues, int multiplicity);
  std::string __str__() const;
  
private:
  
  static bool fUseDB;
  static std::vector<std::string> fParNamesInDB;
  
  std::map<std::string, double> fDBPars;
  static std::map<std::string, double> fUserPars;

};

#endif
