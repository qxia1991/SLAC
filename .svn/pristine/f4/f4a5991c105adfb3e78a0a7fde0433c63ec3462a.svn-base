#ifndef EXO2DRateThreshold_hh
#define EXO2DRateThreshold_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include <string>

class EXO2DRateThresholdHandler : public EXOCalibHandlerBase {

public:
  EXO2DRateThresholdHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("position-threshold")
};




class EXOChargeCluster;

class EXO2DRateThreshold : public EXOCalibBase {
  friend class EXO2DRateThresholdHandler;

  public:
    EXO2DRateThreshold();
    static void SetUserValues(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2);
    static void UseDatabase();
    double GetDataP0() const;
    double GetDataP1() const;
    double GetDataP2() const;
    double GetMCP0() const;
    double GetMCP1() const;
    double GetMCP2() const;
    double GetData2DRate(double calibratedEnergy) const;
    double GetMC2DRate(double calibratedEnergy) const;
    std::string  __str__() const;

  private:
    double Model(double x, double p0, double p1, double p2) const;

    static bool fUseDatabase;

    double fDataP0;
    double fDataP1;
    double fDataP2;
    double fMCP0;
    double fMCP1;
    double fMCP2;
    static double fUserDataP0;
    static double fUserDataP1;
    static double fUserDataP2;
    static double fUserMCP0;
    static double fUserMCP1;
    static double fUserMCP2;
};
#endif
