#ifndef EXOSignalThreshold_hh
#define EXOSignalThreshold_hh

#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include <string>
#include <vector>

class EXOSignalThresholdHandler : public EXOCalibHandlerBase {

public:
  EXOSignalThresholdHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("signal-threshold")
};




class EXOChargeCluster;

class EXOSignalThreshold : public EXOCalibBase {
  friend class EXOSignalThresholdHandler;

  public:
    EXOSignalThreshold();
    static void SetUserValuesForAPDNorth(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2);
    static void SetUserValuesForAPDSouth(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2);
    static void SetUserValuesForU(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2);
    static void SetUserValuesForV(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2);
    static void UseDatabaseForAPDNorth();
    static void UseDatabaseForAPDSouth();
    static void UseDatabaseForU();
    static void UseDatabaseForV();
    double GetDataAPDNorthParam(size_t i) const;
    double GetMCAPDNorthParam(size_t i) const;
    double GetDataAPDSouthParam(size_t i) const;
    double GetMCAPDSouthParam(size_t i) const;
    double GetDataUParam(size_t i) const;
    double GetMCUParam(size_t i) const;
    double GetDataVParam(size_t i) const;
    double GetMCVParam(size_t i) const;
    double GetDataSignalRate(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude) const;
    double GetMCSignalRate(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude) const;
    std::string  __str__() const;

  private:
    double Model(double x, double p0, double p1, double p2) const;

    static bool fUseDatabaseForAPDNorth;
    static bool fUseDatabaseForAPDSouth;
    static bool fUseDatabaseForU;
    static bool fUseDatabaseForV;

    std::vector<double> fDataAPDNorth;
    std::vector<double> fDataAPDSouth;
    std::vector<double> fDataU;
    std::vector<double> fDataV;
    std::vector<double> fMCAPDNorth;
    std::vector<double> fMCAPDSouth;
    std::vector<double> fMCU;
    std::vector<double> fMCV;
    static std::vector<double> fUserDataAPDNorth;
    static std::vector<double> fUserDataAPDSouth;
    static std::vector<double> fUserDataU;
    static std::vector<double> fUserDataV;
    static std::vector<double> fUserMCAPDNorth;
    static std::vector<double> fUserMCAPDSouth;
    static std::vector<double> fUserMCU;
    static std::vector<double> fUserMCV;
};
#endif
