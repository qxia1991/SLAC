#ifndef EXOMCChannelScaling_hh
#define EXOMCChannelScaling_hh

#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include <map>
#include <cstddef> //for size_t

class EXOMCChannelScalingHandler : public EXOCalibHandlerBase
{
public:
  EXOMCChannelScalingHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("mc_scaling")
};

class EXOMCChannelScaling : public EXOCalibBase
{
public:

  double GetScalingOnChannel(const size_t channel) const;
  bool HasChannel(const size_t channel) const;

  std::map <size_t, double> GetScalingChannelMap() const {
    return m_scalings;
  }

  EXOMCChannelScaling() : EXOCalibBase() {}

private:
  std::map <size_t, double> m_scalings;

  friend class EXOMCChannelScalingHandler;
};
#endif
