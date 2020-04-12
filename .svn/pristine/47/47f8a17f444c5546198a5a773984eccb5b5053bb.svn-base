//______________________________________________________________________________
#include "EXOCalibUtilities/EXOEnergyRatio.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

using namespace std;

bool EXOEnergyRatio::fUseDatabaseForCharge = true;
bool EXOEnergyRatio::fUseDatabaseForScint = true;
bool EXOEnergyRatio::fUseDatabaseForRotated = true;
vector<double> EXOEnergyRatio::fUserChargeParamsSS(3,0);
vector<double> EXOEnergyRatio::fUserScintParamsSS(3,0);
vector<double> EXOEnergyRatio::fUserRotatedParamsSS(3,0);
vector<double> EXOEnergyRatio::fUserChargeParamsMS(3,0);
vector<double> EXOEnergyRatio::fUserScintParamsMS(3,0);
vector<double> EXOEnergyRatio::fUserRotatedParamsMS(3,0);

EXOEnergyRatio::EXOEnergyRatio() : 
  EXOCalibBase(),
  fChargeParamsSS(3,0),
  fScintParamsSS(3,0),
  fRotatedParamsSS(3,0),
  fChargeParamsMS(3,0),
  fScintParamsMS(3,0),
  fRotatedParamsMS(3,0)
{

}

void EXOEnergyRatio::SetUserChargeValues(double P0SS,
                                         double P1SS,
                                         double P2SS,
                                         double P0MS,
                                         double P1MS,
                                         double P2MS)
{
  fUseDatabaseForCharge = false;
  fUserChargeParamsSS[0] = P0SS;
  fUserChargeParamsSS[1] = P1SS;
  fUserChargeParamsSS[2] = P2SS;
  fUserChargeParamsMS[0] = P0MS;
  fUserChargeParamsMS[1] = P1MS;
  fUserChargeParamsMS[2] = P2MS;
}

void EXOEnergyRatio::SetUserScintValues(double P0SS,
                                        double P1SS,
                                        double P2SS,
                                        double P0MS,
                                        double P1MS,
                                        double P2MS)
{
  fUseDatabaseForScint = false;
  fUserScintParamsSS[0] = P0SS;
  fUserScintParamsSS[1] = P1SS;
  fUserScintParamsSS[2] = P2SS;
  fUserScintParamsMS[0] = P0MS;
  fUserScintParamsMS[1] = P1MS;
  fUserScintParamsMS[2] = P2MS;
}

void EXOEnergyRatio::SetUserRotatedValues(double P0SS,
                                          double P1SS,
                                          double P2SS,
                                          double P0MS,
                                          double P1MS,
                                          double P2MS)
{
  fUseDatabaseForRotated = false;
  fUserRotatedParamsSS[0] = P0SS;
  fUserRotatedParamsSS[1] = P1SS;
  fUserRotatedParamsSS[2] = P2SS;
  fUserRotatedParamsMS[0] = P0MS;
  fUserRotatedParamsMS[1] = P1MS;
  fUserRotatedParamsMS[2] = P2MS;
}

double EXOEnergyRatio::GetChargeCalibrationParameter(size_t param,
                                                     int multiplicity) const
{
  //Return the energy calibration parameter for charge.
  //The desired parameter is specified in param
  //param == 0 is constant coefficient 
  //param == 1 is linear coefficient
  //param == 2 is quadratic coefficient
  
  double val = 0;
  if(multiplicity > 1){
    if(fUseDatabaseForCharge){
      if(param >= fChargeParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fChargeParamsMS[param];
    }
    else{
      if(param >= fUserChargeParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserChargeParamsMS[param];
    }
  }
  else{
    if(fUseDatabaseForCharge){
      if(param >= fChargeParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fChargeParamsSS[param];
    }
    else{
      if(param >= fUserChargeParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserChargeParamsSS[param];
    }
  }

  return val;
}

double EXOEnergyRatio::GetScintCalibrationParameter(size_t param,
                                                    int multiplicity) const
{
  //Return the energy calibration parameter for scintillation.
  //The desired parameter is specified in param
  //param == 0 is constant coefficient 
  //param == 1 is linear coefficient
  //param == 2 is quadratic coefficient
  
  double val = 0;
  if(multiplicity > 1){
    if(fUseDatabaseForScint){
      if(param >= fScintParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fScintParamsMS[param];
    }
    else{
      if(param >= fUserScintParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserScintParamsMS[param];
    }
  }
  else{
    if(fUseDatabaseForScint){
      if(param >= fScintParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fScintParamsSS[param];
    }
    else{
      if(param >= fUserScintParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserScintParamsSS[param];
    }
  }

  return val;
}

double EXOEnergyRatio::GetRotatedCalibrationParameter(size_t param,
                                                    int multiplicity) const
{
  //Return the energy calibration parameter for rotated energy.
  //The desired parameter is specified in param
  //param == 0 is constant coefficient 
  //param == 1 is linear coefficient
  //param == 2 is quadratic coefficient
  
  double val = 0;
  if(multiplicity > 1){
    if(fUseDatabaseForRotated){
      if(param >= fRotatedParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fRotatedParamsMS[param];
    }
    else{
      if(param >= fUserRotatedParamsMS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserRotatedParamsMS[param];
    }
  }
  else{
    if(fUseDatabaseForRotated){
      if(param >= fRotatedParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fRotatedParamsSS[param];
    }
    else{
      if(param >= fUserRotatedParamsSS.size()){
        LogEXOMsg("The requested parameter does not exist",EEError);
        return 0;
      }
      val = fUserRotatedParamsSS[param];
    }
  }

  return val;
}

string EXOEnergyRatio::__str__() const
{
  stringstream sstream;
  sstream << "-------------------------EXOEnergyRatio-------------------------" << endl;
  sstream << "We have the following calibration parameters:" << endl << endl;
  sstream << "Single site:" << endl;
  sstream << "           P0     P1     P2" << endl;
  sstream << "Charge:         " << GetChargeCalibrationParameter(0,1) << " " << GetChargeCalibrationParameter(1,1) << " " << GetChargeCalibrationParameter(2,1) << endl;
  sstream << "Scintillation:  " << GetScintCalibrationParameter(0,1) << " " << GetScintCalibrationParameter(1,1) << " " << GetScintCalibrationParameter(2,1) << endl;
  sstream << "Rotated:        " << GetRotatedCalibrationParameter(0,1) << " " << GetRotatedCalibrationParameter(1,1) << " " << GetRotatedCalibrationParameter(2,1) << endl << endl;
  sstream << "Multi site:" << endl;
  sstream << "           P0     P1     P2" << endl;
  sstream << "Charge:         " << GetChargeCalibrationParameter(0,2) << " " << GetChargeCalibrationParameter(1,2) << " " << GetChargeCalibrationParameter(2,2) << endl;
  sstream << "Scintillation:  " << GetScintCalibrationParameter(0,2) << " " << GetScintCalibrationParameter(1,2) << " " << GetScintCalibrationParameter(2,2) << endl;
  sstream << "Rotated:        " << GetRotatedCalibrationParameter(0,2) << " " << GetRotatedCalibrationParameter(1,2) << " " << GetRotatedCalibrationParameter(2,2) << endl;
  sstream << "EXOEnergyRatio is set to use " << (fUseDatabaseForCharge ? "database " : "user provided ") << "values for charge," << endl;
  sstream << "                             " << (fUseDatabaseForScint ? "database " : "user provided ") << "values for scint," << endl;
  sstream << "                         and " << (fUseDatabaseForRotated ? "database " : "user provided ") << "values for rotated" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnergyRatioHandler)

EXOCalibBase* EXOEnergyRatioHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOEnergyRatio* newData = new EXOEnergyRatio();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOEnergyRatioHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOEnergyRatio* newData = new EXOEnergyRatio();

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
  selectColumns.push_back("Rotated_p0");
  selectColumns.push_back("Rotated_p1");
  selectColumns.push_back("Rotated_p2");
  selectColumns.push_back("Charge_p0");
  selectColumns.push_back("Charge_p1");
  selectColumns.push_back("Charge_p2");
  selectColumns.push_back("Scint_p0");
  selectColumns.push_back("Scint_p1");
  selectColumns.push_back("Scint_p2");

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
        newData->fRotatedParamsSS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fRotatedParamsSS[1] = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fRotatedParamsSS[2] = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fChargeParamsSS[0] = EXOMiscUtil::stringToDouble(fields[4]);
        newData->fChargeParamsSS[1] = EXOMiscUtil::stringToDouble(fields[5]);
        newData->fChargeParamsSS[2] = EXOMiscUtil::stringToDouble(fields[6]);
        newData->fScintParamsSS[0] = EXOMiscUtil::stringToDouble(fields[7]);
        newData->fScintParamsSS[1] = EXOMiscUtil::stringToDouble(fields[8]);
        newData->fScintParamsSS[2] = EXOMiscUtil::stringToDouble(fields[9]);
      }
      else if(multiplicity == "MS"){
        newData->fRotatedParamsMS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fRotatedParamsMS[1] = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fRotatedParamsMS[2] = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fChargeParamsMS[0] = EXOMiscUtil::stringToDouble(fields[4]);
        newData->fChargeParamsMS[1] = EXOMiscUtil::stringToDouble(fields[5]);
        newData->fChargeParamsMS[2] = EXOMiscUtil::stringToDouble(fields[6]);
        newData->fScintParamsMS[0] = EXOMiscUtil::stringToDouble(fields[7]);
        newData->fScintParamsMS[1] = EXOMiscUtil::stringToDouble(fields[8]);
        newData->fScintParamsMS[2] = EXOMiscUtil::stringToDouble(fields[9]);
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
