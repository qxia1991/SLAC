//______________________________________________________________________________
#include "EXOCalibUtilities/EXOThoriumPeak.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

using namespace std;

bool EXOThoriumPeak::fUseDatabase = true;
double EXOThoriumPeak::fUserRotationAngleSS = 0;
double EXOThoriumPeak::fUserChargePeakSS = 0;
double EXOThoriumPeak::fUserScintPeakSS = 0;
double EXOThoriumPeak::fUserRotatedPeakSS = 0;
double EXOThoriumPeak::fUserRotationAngleMS = 0;
double EXOThoriumPeak::fUserChargePeakMS = 0;
double EXOThoriumPeak::fUserScintPeakMS = 0;
double EXOThoriumPeak::fUserRotatedPeakMS = 0;

EXOThoriumPeak::EXOThoriumPeak() : 
  EXOCalibBase(),
  fRotationAngleSS(0),
  fChargePeakSS(0),
  fScintPeakSS(0),
  fRotatedPeakSS(0),
  fRotationAngleMS(0),
  fChargePeakMS(0),
  fScintPeakMS(0),
  fRotatedPeakMS(0)
{

}

void EXOThoriumPeak::SetUserValues(double angleSS, double chargeSS, double scintSS, double rotSS, double angleMS, double chargeMS, double scintMS, double rotMS)
{
  fUseDatabase = false;
  fUserRotationAngleSS = angleSS;
  fUserChargePeakSS = chargeSS;
  fUserScintPeakSS = scintSS;
  fUserRotatedPeakSS = rotSS;
  fUserRotationAngleMS = angleMS;
  fUserChargePeakMS = chargeMS;
  fUserScintPeakMS = scintMS;
  fUserRotatedPeakMS = rotMS;
}

void EXOThoriumPeak::UseDatabase()
{
  fUseDatabase = true;
}

double EXOThoriumPeak::GetRotationAngle(int multiplicity) const
{
  double val = 0;
  if(multiplicity > 1){
    val = (fUseDatabase ? fRotationAngleMS : fUserRotationAngleMS);
  }
  else{
    val = (fUseDatabase ? fRotationAngleSS : fUserRotationAngleSS);
  }
  return val;
}

double EXOThoriumPeak::GetChargePeak(int multiplicity) const
{
  double val = 0;
  if(multiplicity > 1){
    val = (fUseDatabase ? fChargePeakMS : fUserChargePeakMS);
  }
  else{
    val = (fUseDatabase ? fChargePeakSS : fUserChargePeakSS);
  }
  return val;
}

double EXOThoriumPeak::GetScintillationPeak(int multiplicity) const
{
  double val = 0;
  if(multiplicity > 1){
    val = (fUseDatabase ? fScintPeakMS : fUserScintPeakMS);
  }
  else{
    val = (fUseDatabase ? fScintPeakSS : fUserScintPeakSS);
  }
  return val;
}

double EXOThoriumPeak::GetRotatedPeak(int multiplicity) const
{
  double val = 0;
  if(multiplicity > 1){
    val = (fUseDatabase ? fRotatedPeakMS : fUserRotatedPeakMS);
  }
  else{
    val = (fUseDatabase ? fRotatedPeakSS : fUserRotatedPeakSS);
  }
  return val;
}

string EXOThoriumPeak::__str__() const
{
  stringstream sstream;
  sstream << "-------------------------EXOThoriumPeak-------------------------" << endl;
  sstream << "Thorium peak calibration has the following values:" << endl;
  sstream << "     Angle   ChargePeak  ScintPeak  RotatedPeak" << endl;
  sstream << "SS: " << GetRotationAngle(1) << " " << GetChargePeak(1) << " " << GetScintillationPeak(1) << " " << GetRotatedPeak(1) << endl;
  sstream << "MS: " << GetRotationAngle(2) << " " << GetChargePeak(2) << " " << GetScintillationPeak(2) << " " << GetRotatedPeak(2) << endl;
  sstream << "EXOThoriumPeak is set to use " << (fUseDatabase ? "database " : "user provided ") << "values" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOThoriumPeakHandler)

EXOCalibBase* EXOThoriumPeakHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOThoriumPeak* newData = new EXOThoriumPeak();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOThoriumPeakHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOThoriumPeak* newData = new EXOThoriumPeak();

  // parse dataIdent.  It should have form
  // TABLE_NAME:COLUMN_NAME:VALUE
  unsigned delim1  = dataIdent.find(':');
  unsigned delim2  = dataIdent.rfind(':');

  // if (delim1 == delim2) { we've got a problem }
  std::string tbl = std::string(dataIdent, 0, delim1);
  std::string col = std::string(dataIdent, delim1 + 1, delim2 - delim1 - 1);
  std::string val = std::string(dataIdent, delim2 + 1);

  // Select which columns to read from TABLE_NAME where COLUMN_NAME=VALUE
  std::vector<std::string> selectColumns, orderColumns;
  selectColumns.push_back("Multiplicity");
  selectColumns.push_back("Angle");
  selectColumns.push_back("Charge");
  selectColumns.push_back("Scint");
  selectColumns.push_back("Rotated");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() != 2){
    LogEXOMsg("Number of rows for this entry differs from 2",EEError);
    delete newData;
    return 0;
  }
  std::vector<std::string> fields;
  for(size_t i=0; i<2; i++) {
    res.getRow(fields, i);
    string multiplicity = fields[0];
    try{
      if(multiplicity == "SS"){
        newData->fRotationAngleSS = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fChargePeakSS = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fScintPeakSS = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fRotatedPeakSS = EXOMiscUtil::stringToDouble(fields[4]);
      }
      else if(multiplicity == "MS"){
        newData->fRotationAngleMS = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fChargePeakMS = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fScintPeakMS = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fRotatedPeakMS = EXOMiscUtil::stringToDouble(fields[4]);
      }
      else{
        stringstream stream;
        stream << "Unknown multiplicity value: " << multiplicity;
        LogEXOMsg(stream.str(),EEError);
        delete newData;
        return 0;
      }
    }
    catch (EXOExceptWrongType ex) {
      LogEXOMsg("Bad row in calibration data ''", EEError);
      delete newData;
      return 0;
    }
  }

  return newData;
}
