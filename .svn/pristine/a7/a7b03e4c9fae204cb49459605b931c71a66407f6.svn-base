#ifndef EXOVWireThresholds_hh
#define EXOVWireThresholds_hh

#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include <map>
#include <cstddef> //for size_t

class EXOVWireThresholdsHandler : public EXOCalibHandlerBase
{
public:
  EXOVWireThresholdsHandler();

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  double fDefaultThreshold;

  DEFINE_EXO_CALIB_HANDLER("vwire-thresholds")
};

class EXOVWireThresholds : public EXOCalibBase
{
  typedef std::map < int, double > ChannelMap;
public:

  virtual double GetThresholdOnChannel(int channel) const;
  virtual bool HasChannel(int channel) const;

  EXOVWireThresholds() : EXOCalibBase() {}

private:
  ChannelMap m_thresholds;

  friend class EXOVWireThresholdsHandler;
};
#endif
