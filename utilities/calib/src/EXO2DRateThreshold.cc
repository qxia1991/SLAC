//______________________________________________________________________________
#include "EXOCalibUtilities/EXO2DRateThreshold.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TMath.h"
#include <iostream>
#include <sstream>

using namespace std;

bool EXO2DRateThreshold::fUseDatabase = true;
double EXO2DRateThreshold::fUserDataP0 = 0;
double EXO2DRateThreshold::fUserDataP1 = 0;
double EXO2DRateThreshold::fUserDataP2 = 0;
double EXO2DRateThreshold::fUserMCP0 = 0;
double EXO2DRateThreshold::fUserMCP1 = 0;
double EXO2DRateThreshold::fUserMCP2 = 0;

EXO2DRateThreshold::EXO2DRateThreshold() : 
  EXOCalibBase(),
  fDataP0(0),
  fDataP1(0),
  fDataP2(0),
  fMCP0(0),
  fMCP1(0),
  fMCP2(0)
{ 

}

void EXO2DRateThreshold::SetUserValues(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2)
{
  fUseDatabase = false;
  fUserDataP0 = data_p0;
  fUserDataP1 = data_p1;
  fUserDataP2 = data_p2;
  fUserMCP0 = mc_p0;
  fUserMCP1 = mc_p1;
  fUserMCP2 = mc_p2;
}

void EXO2DRateThreshold::UseDatabase()
{
  fUseDatabase = true;
}

double EXO2DRateThreshold::GetDataP0() const
{
  return (fUseDatabase ? fDataP0 : fUserDataP0);
}

double EXO2DRateThreshold::GetDataP1() const
{
  return (fUseDatabase ? fDataP1 : fUserDataP1);
}

double EXO2DRateThreshold::GetDataP2() const
{
  return (fUseDatabase ? fDataP2 : fUserDataP2);
}

double EXO2DRateThreshold::GetMCP0() const
{
  return (fUseDatabase ? fMCP0 : fUserMCP0);
}
double EXO2DRateThreshold::GetMCP1() const
{
  return (fUseDatabase ? fMCP1 : fUserMCP1);
}
double EXO2DRateThreshold::GetMCP2() const
{
  return (fUseDatabase ? fMCP2 : fUserMCP2);
}

double EXO2DRateThreshold::Model(double x, double p0, double p1, double p2) const
{
  const double sqrt2 = sqrt(2);
  return p0 * 0.5 * (1 + TMath::Erf((x - p1) / (sqrt2 * p2)));
}

double EXO2DRateThreshold::GetData2DRate(double calibratedEnergy) const
{
  //Return the 2D reconstruction rate for data at given calibratedEnergy
  return Model(calibratedEnergy,GetDataP0(),GetDataP1(),GetDataP2());
}

double EXO2DRateThreshold::GetMC2DRate(double calibratedEnergy) const
{
  //Return the 2D reconstruction rate for MC at given calibratedEnergy
  return Model(calibratedEnergy,GetMCP0(),GetMCP1(),GetMCP2());
}

string EXO2DRateThreshold::__str__() const
{
  stringstream sstream;
  sstream << "-----------------------EXO2DRateThreshold-----------------------" << endl;
  sstream << "The following model is used to calculate the 2D reconstruction rate:" << endl;
  sstream << "P0/2 [1 + Erf((E - P1) / (sqrt2 * P2))]" << endl;
  sstream << "The parameters are:" << endl;
  sstream << "         P0      P1      P2" << endl;
  sstream << "Data: " << GetDataP0() << " " << GetDataP1() << " " << GetDataP2() << endl;
  sstream << "MC:   " << GetMCP0() << " " << GetMCP1() << " " << GetMCP2() << endl;
  sstream << "EXO2DRateThreshold is set to use " << (fUseDatabase ? "database " : "user provided ") << "values" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXO2DRateThresholdHandler)

EXOCalibBase* EXO2DRateThresholdHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  //Return default values if the database can't be read

  // Once newData gets returned, EXOCalibManager owns it.
  EXO2DRateThreshold* newData = new EXO2DRateThreshold();
  return newData;
}

EXOCalibBase* EXO2DRateThresholdHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXO2DRateThreshold* newData = new EXO2DRateThreshold();

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
  selectColumns.push_back("Type");
  selectColumns.push_back("Lim");
  selectColumns.push_back("Thresh");
  selectColumns.push_back("Rise");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() != 2){
    LogEXOMsg("Numbers of row differs from expected", EEError);
    delete newData;
    return 0;
  }

  for(size_t i=0; i<2; i++){
    std::vector<std::string> fields;
    res.getRow(fields, i);
    std::string type = fields[0];
    try{
      if(type == "Data"){
        newData->fDataP0 = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fDataP1 = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fDataP2 = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "MC"){
        newData->fMCP0 = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fMCP1 = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fMCP2 = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else{
        stringstream message;
        message << "Could not recognize type: " << type;
        LogEXOMsg(message.str(), EEError);
        delete newData;
        return 0;
      }
    }
    catch (EXOExceptWrongType ex) {
      stringstream message;
      message << "Bad row in calibration data: " << ex.what();
      LogEXOMsg(message.str(), EEError);
      delete newData;
      return 0;
    }
  }
  return newData;
}
