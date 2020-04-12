#ifndef EXOElectronicsShapers_hh
#define EXOElectronicsShapers_hh

#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOElectronics.hh"
#include <map>

class EXOElectronicsShapers : public EXOCalibBase, public EXOElectronics {
  friend class EXOElectronicsShapersHandler;
};

class EXOElectronicsShapersHandler : public EXOCalibHandlerBase {

public:

  EXOElectronicsShapersHandler() : EXOCalibHandlerBase() {}

  EXOCalibBase* FillWithDefault() const;

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("shaping-times")
};

#endif
