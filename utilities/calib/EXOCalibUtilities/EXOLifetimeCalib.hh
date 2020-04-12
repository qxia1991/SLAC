#ifndef EXOLifetimeCalib_hh
#define EXOLifetimeCalib_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include <vector>
#include <string>

#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

class EXOLifetimeCalibHandler : public EXOCalibHandlerBase {

public:
  EXOLifetimeCalibHandler(): EXOCalibHandlerBase() { }
  //~EXOLifetimeCalibHandler() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("a-el-life")
};

class TTimeStamp;
class TF1;

class EXOLifetimeCalib : public EXOCalibBase {
  friend class EXOLifetimeCalibHandler;

public:

  EXOLifetimeCalib() : EXOCalibBase(),fn(0) { }
  EXOLifetimeCalib(const EXOLifetimeCalib& other);
  //~EXOLifetimeCalib() { }

  std::string getFuncType() {return fn_type;}

  double lifetime(double t, double flow=-999) const;
  double lifetime(const TTimeStamp& t, double flow=-999) const;
  double twoPhaseModel(double t, double flow) const; // lifetime based on two-phase purity model
  
private:

  double fn_start; // origin of time in function
  std::string fn_type; // type of function
  std::vector <double> fn_params; // function parameters
  TF1 *fn;         // pointer to ROOT function

};

#endif
