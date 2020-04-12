#ifndef EXOCopyDenoisedCalib_hh
#define EXOCopyDenoisedCalib_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOUtilities/EXOTimestamp.hh"
#include "EXOUtilities/EXOEventHeader.hh"

#include <iostream>
#include <string>
#include <vector>


class EXOCopyDenoisedCalibHandler : public EXOCalibHandlerBase {

public:
  EXOCopyDenoisedCalibHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("denoised")
};


class EXOCopyDenoisedCalib : public EXOCalibBase {
  friend class EXOCopyDenoisedCalibHandler;

public:

  EXOCopyDenoisedCalib();

  static const char* GetFileName(std::string flavor, const EXOEventHeader& header);
  static const char* GetFileName(std::string flavor, long int seconds, int nano);
  static const char* GetFileName(std::string flavor, const EXOTimestamp& time);
  
  static void UseDatabase(bool useDB);
  static void SetUserValue(std::string userValue);
  std::string __str__() const;
  
private:
  
  static bool fUseDB;
  static std::string fFileName;

};

#endif
