//______________________________________________________________________________
#include "EXOCalibUtilities/EXOSignalThreshold.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TMath.h"
#include <iostream>
#include <sstream>

using namespace std;
using EXOMiscUtil::EChannelType;

bool EXOSignalThreshold::fUseDatabaseForAPDNorth = true;
bool EXOSignalThreshold::fUseDatabaseForAPDSouth = true;
bool EXOSignalThreshold::fUseDatabaseForU = true;
bool EXOSignalThreshold::fUseDatabaseForV = true;
vector<double> EXOSignalThreshold::fUserDataAPDNorth(3);
vector<double> EXOSignalThreshold::fUserDataAPDSouth(3);
vector<double> EXOSignalThreshold::fUserDataU(3);
vector<double> EXOSignalThreshold::fUserDataV(3);
vector<double> EXOSignalThreshold::fUserMCAPDNorth(3);
vector<double> EXOSignalThreshold::fUserMCAPDSouth(3);
vector<double> EXOSignalThreshold::fUserMCU(3);
vector<double> EXOSignalThreshold::fUserMCV(3);

EXOSignalThreshold::EXOSignalThreshold() : 
  EXOCalibBase(),
  fDataAPDNorth(3),
  fDataAPDSouth(3),
  fDataU(3),
  fDataV(3),
  fMCAPDNorth(3),
  fMCAPDSouth(3),
  fMCU(3),
  fMCV(3)
{ 

}

void EXOSignalThreshold::SetUserValuesForAPDNorth(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2)
{
  fUseDatabaseForAPDNorth = false;
  fUserDataAPDNorth.at(0) = data_p0;
  fUserDataAPDNorth.at(1) = data_p1;
  fUserDataAPDNorth.at(2) = data_p2;
  fUserMCAPDSouth.at(0) = mc_p0;
  fUserMCAPDSouth.at(1) = mc_p1;
  fUserMCAPDSouth.at(2) = mc_p2;
}

void EXOSignalThreshold::SetUserValuesForAPDSouth(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2)
{
  fUseDatabaseForAPDSouth = false;
  fUserDataAPDSouth.at(0) = data_p0;
  fUserDataAPDSouth.at(1) = data_p1;
  fUserDataAPDSouth.at(2) = data_p2;
  fUserMCAPDSouth.at(0) = mc_p0;
  fUserMCAPDSouth.at(1) = mc_p1;
  fUserMCAPDSouth.at(2) = mc_p2;
}

void EXOSignalThreshold::SetUserValuesForU(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2)
{
  fUseDatabaseForU = false;
  fUserDataU.at(0) = data_p0;
  fUserDataU.at(1) = data_p1;
  fUserDataU.at(2) = data_p2;
  fUserMCU.at(0) = mc_p0;
  fUserMCU.at(1) = mc_p1;
  fUserMCU.at(2) = mc_p2;
}

void EXOSignalThreshold::SetUserValuesForV(double data_p0, double data_p1, double data_p2, double mc_p0, double mc_p1, double mc_p2)
{
  fUseDatabaseForV = false;
  fUserDataV.at(0) = data_p0;
  fUserDataV.at(1) = data_p1;
  fUserDataV.at(2) = data_p2;
  fUserMCV.at(0) = mc_p0;
  fUserMCV.at(1) = mc_p1;
  fUserMCV.at(2) = mc_p2;
}

void EXOSignalThreshold::UseDatabaseForAPDNorth()
{
  fUseDatabaseForAPDNorth = true;
}

void EXOSignalThreshold::UseDatabaseForAPDSouth()
{
  fUseDatabaseForAPDSouth = true;
}

void EXOSignalThreshold::UseDatabaseForU()
{
  fUseDatabaseForU = true;
}

void EXOSignalThreshold::UseDatabaseForV()
{
  fUseDatabaseForV = true;
}

double EXOSignalThreshold::GetDataAPDNorthParam(size_t i) const
{
  return (fUseDatabaseForAPDNorth ? fDataAPDNorth.at(i) : fUserDataAPDNorth.at(i));
}

double EXOSignalThreshold::GetDataAPDSouthParam(size_t i) const
{
  return (fUseDatabaseForAPDSouth ? fDataAPDSouth.at(i) : fUserDataAPDSouth.at(i));
}

double EXOSignalThreshold::GetMCAPDNorthParam(size_t i) const
{
  return (fUseDatabaseForAPDNorth ? fMCAPDNorth.at(i) : fUserMCAPDNorth.at(i));
}

double EXOSignalThreshold::GetMCAPDSouthParam(size_t i) const
{
  return (fUseDatabaseForAPDSouth ? fMCAPDSouth.at(i) : fUserMCAPDSouth.at(i));
}

double EXOSignalThreshold::GetDataUParam(size_t i) const
{
  return (fUseDatabaseForU ? fDataU.at(i) : fUserDataU.at(i));
}

double EXOSignalThreshold::GetMCUParam(size_t i) const
{
  return (fUseDatabaseForU ? fMCU.at(i) : fUserMCU.at(i));
}

double EXOSignalThreshold::GetDataVParam(size_t i) const
{
  return (fUseDatabaseForV ? fDataV.at(i) : fUserDataV.at(i));
}

double EXOSignalThreshold::GetMCVParam(size_t i) const
{
  return (fUseDatabaseForV ? fMCV.at(i) : fUserMCV.at(i));
}

double EXOSignalThreshold::Model(double x, double p0, double p1, double p2) const
{
  const double sqrt2 = sqrt(2);
  return p0 * 0.5 * (1 + TMath::Erf((x - p1) / (sqrt2 * p2)));
}

double EXOSignalThreshold::GetDataSignalRate(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude) const
{
  //Return the signal reconstruction rate for data at given signal magnitude 
  switch(sigType){
    case EXOMiscUtil::kAPDSumOfGangs:
      if(side == EXOMiscUtil::kNorth){
        return Model(magnitude,GetDataAPDNorthParam(0),GetDataAPDNorthParam(1),GetDataAPDNorthParam(2));
      }
      else if(side == EXOMiscUtil::kSouth){
        return Model(magnitude,GetDataAPDSouthParam(0),GetDataAPDSouthParam(1),GetDataAPDSouthParam(2));
      }
      break;
    case EXOMiscUtil::kUWire:
      return Model(magnitude,GetDataUParam(0),GetDataUParam(1),GetDataUParam(2));
      break;
    case EXOMiscUtil::kVWire:
      return Model(magnitude,GetDataVParam(0),GetDataVParam(1),GetDataVParam(2));
      break;
    default:
      LogEXOMsg("Unknown signal type. Can't calculate signal reconstruction rate.",EEError);
  }
  return 0.;
}

double EXOSignalThreshold::GetMCSignalRate(EXOMiscUtil::EChannelType sigType, EXOMiscUtil::ETPCSide side, double magnitude) const
{
  //Return the signal reconstruction rate for MC at given signal magnitude
  switch(sigType){
    case EXOMiscUtil::kAPDSumOfGangs:
      if(side == EXOMiscUtil::kNorth){
        return Model(magnitude,GetMCAPDNorthParam(0),GetMCAPDNorthParam(1),GetMCAPDNorthParam(2));
      }
      else if(side == EXOMiscUtil::kSouth){
        return Model(magnitude,GetMCAPDSouthParam(0),GetMCAPDSouthParam(1),GetMCAPDSouthParam(2));
      }
      break;
    case EXOMiscUtil::kUWire:
      return Model(magnitude,GetMCUParam(0),GetMCUParam(1),GetMCUParam(2));
      break;
    case EXOMiscUtil::kVWire:
      return Model(magnitude,GetMCVParam(0),GetMCVParam(1),GetMCVParam(2));
      break;
    default:
      LogEXOMsg("Unknown signal type. Can't calculate signal reconstruction rate.",EEError);
  }
  return 0.;
}

string EXOSignalThreshold::__str__() const
{
  stringstream sstream;
  sstream << "-----------------------EXOSignalThreshold-----------------------" << endl;
  sstream << "The following model is used to calculate the signal reconstruction rate:" << endl;
  sstream << "P0/2 [1 + Erf((E - P1) / (sqrt2 * P2))]" << endl;
  sstream << "The parameters are:" << endl;
  sstream << "         P0      P1      P2" << endl;
  sstream << "APD North Data: " << GetDataAPDNorthParam(0) << " " << GetDataAPDNorthParam(1) << " " << GetDataAPDNorthParam(2) << endl;
  sstream << "APD North MC:   " << GetMCAPDNorthParam(0) << " " << GetMCAPDNorthParam(1) << " " << GetMCAPDNorthParam(2) << endl;
  sstream << "APD South Data: " << GetDataAPDSouthParam(0) << " " << GetDataAPDSouthParam(1) << " " << GetDataAPDSouthParam(2) << endl;
  sstream << "APD South MC:   " << GetMCAPDSouthParam(0) << " " << GetMCAPDSouthParam(1) << " " << GetMCAPDSouthParam(2) << endl;
  sstream << "U         Data: " << GetDataUParam(0) << " " << GetDataUParam(1) << " " << GetDataUParam(2) << endl;
  sstream << "U         MC:   " << GetMCUParam(0) << " " << GetMCUParam(1) << " " << GetMCUParam(2) << endl;
  sstream << "V         Data: " << GetDataVParam(0) << " " << GetDataVParam(1) << " " << GetDataVParam(2) << endl;
  sstream << "V         MC:   " << GetMCVParam(0) << " " << GetMCVParam(1) << " " << GetMCVParam(2) << endl;
  sstream << "EXOSignalThreshold is set to use " << (fUseDatabaseForAPDNorth ? "database " : "user provided ") << "values for APD north," << endl;
  sstream << "" << (fUseDatabaseForAPDSouth ? "database " : "user provided ") << "values for APD south," << endl;
  sstream << "" << (fUseDatabaseForU ? "database " : "user provided ") << "values for U-wires" << endl;
  sstream << "and " << (fUseDatabaseForV ? "database " : "user provided ") << "values for V-wires" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOSignalThresholdHandler)

EXOCalibBase* EXOSignalThresholdHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  //Return default values if the database can't be read

  // Once newData gets returned, EXOCalibManager owns it.
  EXOSignalThreshold* newData = new EXOSignalThreshold();
  return newData;
}

EXOCalibBase* EXOSignalThresholdHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOSignalThreshold* newData = new EXOSignalThreshold();

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

  if(res.getNRows() != 8){
    LogEXOMsg("Numbers of row differs from expected", EEError);
    delete newData;
    return 0;
  }

  for(size_t i=0; i<8; i++){
    std::vector<std::string> fields;
    res.getRow(fields, i);
    std::string type = fields[0];
    try{
      if(type == "DataAPDNorth"){
        newData->fDataAPDNorth.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fDataAPDNorth.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fDataAPDNorth.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "MCAPDNorth"){
        newData->fMCAPDNorth.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fMCAPDNorth.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fMCAPDNorth.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "DataAPDSouth"){
        newData->fDataAPDSouth.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fDataAPDSouth.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fDataAPDSouth.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "MCAPDSouth"){
        newData->fMCAPDSouth.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fMCAPDSouth.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fMCAPDSouth.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "DataU"){
        newData->fDataU.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fDataU.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fDataU.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "MCU"){
        newData->fMCU.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fMCU.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fMCU.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "DataV"){
        newData->fDataV.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fDataV.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fDataV.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
      }
      else if(type == "MCV"){
        newData->fMCV.at(0) = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fMCV.at(1) = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fMCV.at(2) = EXOMiscUtil::stringToDouble(fields[3]);
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
