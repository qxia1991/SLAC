#ifndef EXOEnvironmentCalib_hh
#define EXOEnvironmentCalib_hh

#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

class EXOEnvironmentHandler : public EXOCalibHandlerBase 
{

public: 
  EXOEnvironmentHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);
  
  DEFINE_EXO_CALIB_HANDLER("environment")
};

class EXOEnvironmentCalib : public EXOCalibBase
{
 public:
  bool GetSirenState(); 
  bool IsBadTimeInterval(); 
  //Eg. bool GetCompressorState();

  EXOEnvironmentCalib() : EXOCalibBase() {}

private:
  bool sirenInCR;
  bool BadTimeInterval;
  friend class EXOEnvironmentHandler;
};

#endif
