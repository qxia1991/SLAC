#ifndef EXOThoriumPeak_hh
#define EXOThoriumPeak_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

#include <string>
#include <vector>

class EXOThoriumPeakHandler : public EXOCalibHandlerBase {

public:
  EXOThoriumPeakHandler(): EXOCalibHandlerBase() { }

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("thorium-peak")
};


class EXOThoriumPeak : public EXOCalibBase {
  friend class EXOThoriumPeakHandler;

public:

  EXOThoriumPeak();

  double GetRotationAngle(int multiplicity) const;
  double GetChargePeak(int multiplicity) const;
  double GetScintillationPeak(int multiplicity) const;
  double GetRotatedPeak(int multiplicity) const;

  static void SetUserValues(double angleSS, double chargeSS, double scintSS, double rotSS, double angleMS, double chargeMS, double scintMS, double rotMS);
  static void UseDatabase();
  std::string __str__() const;
  
private:

  // Single site parameters
  double fRotationAngleSS;  //Anticorrelation rotation angle
  double fChargePeakSS;     //Charge peak position (in raw energy)
  double fScintPeakSS;      //Scintillation peak position (in raw energy)
  double fRotatedPeakSS;    //Rotated peak position (in raw energy)

  //Multi site parameters
  double fRotationAngleMS;  //Anticorrelation rotation angle
  double fChargePeakMS;     //Charge peak position (in raw energy)
  double fScintPeakMS;      //Scintillation peak position (in raw energy)
  double fRotatedPeakMS;    //Rotated peak position (in raw energy)

  //User-set parameters
  static bool fUseDatabase;

  // Single site user parameters
  static double fUserRotationAngleSS;  //Anticorrelation rotation angle
  static double fUserChargePeakSS;     //Charge peak position (in raw energy)
  static double fUserScintPeakSS;      //Scintillation peak position (in raw energy)
  static double fUserRotatedPeakSS;    //Rotated peak position (in raw energy)

  //Multi site user parameters
  static double fUserRotationAngleMS;  //Anticorrelation rotation angle
  static double fUserChargePeakMS;     //Charge peak position (in raw energy)
  static double fUserScintPeakMS;      //Scintillation peak position (in raw energy)
  static double fUserRotatedPeakMS;    //Rotated peak position (in raw energy)

};

#endif
