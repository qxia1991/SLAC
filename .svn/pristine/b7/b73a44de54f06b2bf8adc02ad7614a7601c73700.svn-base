#ifndef EXOAPDGains_hh
#define EXOAPDGains_hh

#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include <vector>

class EXOAPDGainsHandler : public EXOCalibHandlerBase
{
public:
  EXOAPDGainsHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("apd-gains")
};

class EXOAPDGains : public EXOCalibBase
{
public:
  EXOAPDGains(); // : EXOCalibBase() {}
  //~EXOAPDGains();

  bool exists(unsigned int ich) const;
  double gain(unsigned int ich) const;
  double gain_error(unsigned int ich) const;

private:
  // gain is defined as signal for one photon
  struct channel_gain {
    unsigned int ichannel;
    double gain;
    double gain_error;
  };

  std::vector<channel_gain> gains;

  friend class EXOAPDGainsHandler;
};
#endif
