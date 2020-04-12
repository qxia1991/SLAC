#ifndef EXOVWireGains_hh
#define EXOVWireGains_hh

#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include <map>
#include <cstddef> //for size_t

class EXOVWireGainsHandler : public EXOCalibHandlerBase
{
public:
  EXOVWireGainsHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("vwire-gains")
};

class EXOVWireGains : public EXOCalibBase
{
public:
  struct DoubleWithError {
    double value;
    double error;
  };

  double GetGainOnChannel(size_t channel) const;
  double GetGainErrorOnChannel(size_t channel) const;
  bool HasChannel(size_t channel) const;
  
  std::map <size_t, double> GetGainsChannelMap() const {
      return m_gains_only;
  }

  EXOVWireGains() : EXOCalibBase() {}

private:
  std::map <size_t, DoubleWithError> m_gains;
  std::map <size_t, double> m_gains_only;  

  friend class EXOVWireGainsHandler;
};
#endif
