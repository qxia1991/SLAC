#ifndef EXOUWireGains_hh
#define EXOUWireGains_hh

#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include <map>
#include <cstddef> //for size_t

class EXOUWireGainsHandler : public EXOCalibHandlerBase
{
public:
  EXOUWireGainsHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("uwire-gains")
};

class EXOUWireGains : public EXOCalibBase
{
public:
  struct DoubleWithError {
    double value;
    double error;
  };

  double GetGainOnChannel(const size_t channel) const;
  double GetGainErrorOnChannel(const size_t channel) const;
  bool HasChannel(const size_t channel) const;

  std::map <size_t, double> GetGainsChannelMap() const {
      return m_gains_only;
  }


  EXOUWireGains() : EXOCalibBase() {}

private:
  std::map <size_t, DoubleWithError> m_gains;
  std::map <size_t, double> m_gains_only;

  friend class EXOUWireGainsHandler;
};
#endif
