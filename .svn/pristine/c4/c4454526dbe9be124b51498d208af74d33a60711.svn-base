#ifndef EXOFlowRate_hh
#define EXOFlowRate_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include <vector>
#include <string>

#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

class EXOFlowRateHandler : public EXOCalibHandlerBase {

public:
  EXOFlowRateHandler(): EXOCalibHandlerBase() { }
  //~EXOFlowRateHandler() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("flow")
};

class TTimeStamp;
class TF1;

class EXOFlowRate : public EXOCalibBase {
  friend class EXOFlowRateHandler;

public:

  EXOFlowRate() : EXOCalibBase(),fn(0) { }
  EXOFlowRate(const EXOFlowRate& other);
  //~EXOFlowRate() { }

  double flow(double t) const;
  double flow(const TTimeStamp& t) const;
  
private:

  double fn_start; // origin of time in function
  std::string fn_type; // type of function
  std::vector <double> fn_params; // function parameters
  TF1 *fn;         // pointer to ROOT function

};

#endif
