#ifndef EXOGridCorretionCalib_hh
#define EXOGridCorretionCalib_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include <vector>
#include <string>

#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

class EXOGridCorrectionCalibHandler : public EXOCalibHandlerBase {

public:
  EXOGridCorrectionCalibHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("shielding-grid-corrections")
};

class EXOGridCorrectionCalib : public EXOCalibBase {
  friend class EXOGridCorrectionCalibHandler;

public:

  const std::vector <double>& GetGridCorrParameters() const; 
  double GetGridCorrParameter(int i) const; 
  const std::string& GetEquationType() const
    { return fEqnType; }
 
private:

  std::vector <double> fparams;  // stored parameters
  std::string          fEqnType; // Equation type 

};

#endif
