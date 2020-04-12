//______________________________________________________________________________
#include "EXOCalibUtilities/EXOMCEnergyCalib.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

using namespace std;

bool EXOMCEnergyCalib::fUseDatabaseForCharge = true;
vector<double> EXOMCEnergyCalib::fUserChargeParamsSS(2,0);
vector<double> EXOMCEnergyCalib::fUserChargeParamsMS(2,0);

EXOMCEnergyCalib::EXOMCEnergyCalib() : 
  EXOCalibBase(),
  fChargeParamsSS(2,0),
  fChargeParamsMS(2,0)
{

}

void EXOMCEnergyCalib::SetUserChargeValues(double P0SS,
                                           double P1SS,
                                           double P0MS,
                                           double P1MS)
{
  fUseDatabaseForCharge = false;
  fUserChargeParamsSS[0] = P0SS;
  fUserChargeParamsSS[1] = P1SS;
  fUserChargeParamsMS[0] = P0MS;
  fUserChargeParamsMS[1] = P1MS;
}

double EXOMCEnergyCalib::CalibratedChargeEnergy(double charge,
                                                int multiplicity) const
{
  return GetChargeCalibrationParameter(0,multiplicity) + charge * GetChargeCalibrationParameter(1,multiplicity);
}

double EXOMCEnergyCalib::GetChargeCalibrationParameter(size_t param,
                                                     int multiplicity) const
{
  //Return the energy calibration parameter for charge.
  //The desired parameter is specified in param
  //param == 0 is constant coefficient 
  //param == 1 is linear coefficient
  
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

string EXOMCEnergyCalib::__str__() const
{
  stringstream sstream;
  sstream << "------------------------EXOMCEnergyCalib------------------------" << endl;
  sstream << "We have the following calibration parameters:" << endl << endl;
  sstream << "Single site:" << endl;
  sstream << "           P0     P1" << endl;
  sstream << "Charge:         " << GetChargeCalibrationParameter(0,1) << " " << GetChargeCalibrationParameter(1,1) << endl << endl;
  sstream << "Multi site:" << endl;
  sstream << "           P0     P1" << endl;
  sstream << "Charge:         " << GetChargeCalibrationParameter(0,2) << " " << GetChargeCalibrationParameter(1,2) << endl << endl;
  sstream << "EXOMCEnergyCalib is set to use " << (fUseDatabaseForCharge ? "database " : "user provided ") << "values for charge." << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOMCEnergyCalibHandler)

EXOCalibBase* EXOMCEnergyCalibHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOMCEnergyCalib* newData = new EXOMCEnergyCalib();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOMCEnergyCalibHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOMCEnergyCalib* newData = new EXOMCEnergyCalib();

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
  selectColumns.push_back("Charge_p0");
  selectColumns.push_back("Charge_p1");

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
        newData->fChargeParamsSS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fChargeParamsSS[1] = EXOMiscUtil::stringToDouble(fields[2]);
      }
      else if(multiplicity == "MS"){
        newData->fChargeParamsMS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fChargeParamsMS[1] = EXOMiscUtil::stringToDouble(fields[2]);
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
