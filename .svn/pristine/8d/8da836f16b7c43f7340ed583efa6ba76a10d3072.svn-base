#ifndef EXODiagonalCut_hh
#define EXODiagonalCut_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include <string>

class EXODiagonalCutHandler : public EXOCalibHandlerBase {

public:
  EXODiagonalCutHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("diagonal-cut")
};




class EXOChargeCluster;

class EXODiagonalCut : public EXOCalibBase {
  friend class EXODiagonalCutHandler;

  public:
    EXODiagonalCut();
    static void SetUserValues(double p0_ss, double p1_ss, double p0_ms, double p1_ms);
    static void UseDatabase();
    double GetCriticalScintillationValueSS(double chargeEnergy) const;
    double GetCriticalScintillationValueMS(double chargeEnergy) const;
    bool SurvivesSingleSiteCut(double scintEnergy, double chargeEnergy) const;
    bool SurvivesMultiSiteCut(double scintEnergy, double chargeEnergy) const;
    double GetP0_SS() const;
    double GetP1_SS() const;
    double GetP0_MS() const;
    double GetP1_MS() const;
    std::string  __str__() const;

  private:
    static bool fUseDatabase;

    double fP0_SS; //constant term single site from database
    double fP1_SS; //linear coefficient single site from database
    double fP0_MS; //constant term multi site from database
    double fP1_MS; //linear coefficient multi site from database
    static double fUserP0_SS; //constant term single site set by user
    static double fUserP1_SS; //linear coefficient single site set by user
    static double fUserP0_MS; //constant term multi site set by user
    static double fUserP1_MS; //linear coefficient multi site set by user
};
#endif
