#ifndef EXOChannelMapCalib_hh
#define EXOChannelMapCalib_hh

//#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOUtilities/EXOChannelMap.hh"

#include <string>

class EXOChannelMapCalib : public EXOCalibBase, public EXOChannelMap {
friend class EXOChannelMapHandler;
};

// Define handler here instead of in a separate file for
// convenience

class EXOChannelMapHandler : public EXOCalibHandlerBase {

public:

  EXOChannelMapHandler() {}

protected:
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);
  DEFINE_EXO_CALIB_HANDLER("channelmap")
};

#endif
