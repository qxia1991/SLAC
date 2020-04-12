//______________________________________________________________________________
#include "EXOCalibUtilities/EXOEnergyBias.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

using namespace std;

bool EXOEnergyBias::fUseDatabaseForCharge = true;
bool EXOEnergyBias::fUseDatabaseForRotated = true;
vector<double> EXOEnergyBias::fUserBiasesChargeSS(4,0);
vector<double> EXOEnergyBias::fUserBiasesChargeMS(4,0);
vector<double> EXOEnergyBias::fUserBiasesRotatedSS(4,0);
vector<double> EXOEnergyBias::fUserBiasesRotatedMS(4,0);

EXOEnergyBias::EXOEnergyBias() : 
  EXOCalibBase(),
  fBiasesChargeSS(4,0),
  fBiasesChargeMS(4,0),
  fBiasesRotatedSS(4,0),
  fBiasesRotatedMS(4,0)
{

}

void EXOEnergyBias::SetUserValuesForCharge(double Co1173SS, double Co1333SS, double Cs662SS, double Th2615SS, double Co1173MS, double Co1333MS, double Cs662MS, double Th2615MS)
{
  fUseDatabaseForCharge = false;
  fUserBiasesChargeSS[0] = Co1173SS;
  fUserBiasesChargeSS[1] = Co1333SS;
  fUserBiasesChargeSS[2] = Cs662SS;
  fUserBiasesChargeSS[3] = Th2615SS;
  fUserBiasesChargeMS[0] = Co1173MS;
  fUserBiasesChargeMS[1] = Co1333MS;
  fUserBiasesChargeMS[2] = Cs662MS;
  fUserBiasesChargeMS[3] = Th2615MS;
}

void EXOEnergyBias::SetUserValuesForRotated(double Co1173SS, double Co1333SS, double Cs662SS, double Th2615SS, double Co1173MS, double Co1333MS, double Cs662MS, double Th2615MS)
{
  fUseDatabaseForRotated = false;
  fUserBiasesRotatedSS[0] = Co1173SS;
  fUserBiasesRotatedSS[1] = Co1333SS;
  fUserBiasesRotatedSS[2] = Cs662SS;
  fUserBiasesRotatedSS[3] = Th2615SS;
  fUserBiasesRotatedMS[0] = Co1173MS;
  fUserBiasesRotatedMS[1] = Co1333MS;
  fUserBiasesRotatedMS[2] = Cs662MS;
  fUserBiasesRotatedMS[3] = Th2615MS;
}

void EXOEnergyBias::UseDatabaseForCharge()
{
  fUseDatabaseForCharge = true;
}

void EXOEnergyBias::UseDatabaseForRotated()
{
  fUseDatabaseForRotated = true;
}

double EXOEnergyBias::GetUserBiasCharge(size_t param, int multiplicity) const
{
  return (multiplicity > 1 ? fUserBiasesChargeMS[param] : fUserBiasesChargeSS[param]);
}

double EXOEnergyBias::GetDBBiasCharge(size_t param, int multiplicity) const
{
  return (multiplicity > 1 ? fBiasesChargeMS[param] : fBiasesChargeSS[param]);
}

double EXOEnergyBias::GetUserBiasRotated(size_t param, int multiplicity) const
{
  return (multiplicity > 1 ? fUserBiasesRotatedMS[param] : fUserBiasesRotatedSS[param]);
}

double EXOEnergyBias::GetDBBiasRotated(size_t param, int multiplicity) const
{
  return (multiplicity > 1 ? fBiasesRotatedMS[param] : fBiasesRotatedSS[param]);
}

double EXOEnergyBias::GetCo1173BiasCharge(int multiplicity) const
{
  return (fUseDatabaseForCharge ? GetDBBiasCharge(0,multiplicity) : GetUserBiasCharge(0,multiplicity));
}

double EXOEnergyBias::GetCo1333BiasCharge(int multiplicity) const
{
  return (fUseDatabaseForCharge ? GetDBBiasCharge(1,multiplicity) : GetUserBiasCharge(1,multiplicity));
}

double EXOEnergyBias::GetCs662BiasCharge(int multiplicity) const
{
  return (fUseDatabaseForCharge ? GetDBBiasCharge(2,multiplicity) : GetUserBiasCharge(2,multiplicity));
}

double EXOEnergyBias::GetTh2615BiasCharge(int multiplicity) const
{
  return (fUseDatabaseForCharge ? GetDBBiasCharge(3,multiplicity) : GetUserBiasCharge(3,multiplicity));
}

double EXOEnergyBias::GetCo1173BiasRotated(int multiplicity) const
{
  return (fUseDatabaseForRotated ? GetDBBiasRotated(0,multiplicity) : GetUserBiasRotated(0,multiplicity));
}

double EXOEnergyBias::GetCo1333BiasRotated(int multiplicity) const
{
  return (fUseDatabaseForRotated ? GetDBBiasRotated(1,multiplicity) : GetUserBiasRotated(1,multiplicity));
}

double EXOEnergyBias::GetCs662BiasRotated(int multiplicity) const
{
  return (fUseDatabaseForRotated ? GetDBBiasRotated(2,multiplicity) : GetUserBiasRotated(2,multiplicity));
}

double EXOEnergyBias::GetTh2615BiasRotated(int multiplicity) const
{
  return (fUseDatabaseForRotated ? GetDBBiasRotated(3,multiplicity) : GetUserBiasRotated(3,multiplicity));
}

string EXOEnergyBias::__str__() const
{
  stringstream sstream;
  sstream << "--------------------------EXOEnergyBias-------------------------" << endl;
  sstream << "Energy bias calibration has the following values:" << endl;
  sstream << "Charge:" << endl;
  sstream << "     Co1173   Co1333   Cs662   Th2615" << endl;
  sstream << "SS: " << GetCo1173BiasCharge(1) << " " << GetCo1333BiasCharge(1) << " " << GetCs662BiasCharge(1) << " " << GetTh2615BiasCharge(1) << endl;
  sstream << "MS: " << GetCo1173BiasCharge(2) << " " << GetCo1333BiasCharge(2) << " " << GetCs662BiasCharge(2) << " " << GetTh2615BiasCharge(2) << endl << endl;
  sstream << "Rotated:" << endl;
  sstream << "     Co1173   Co1333   Cs662   Th2615" << endl;
  sstream << "SS: " << GetCo1173BiasRotated(1) << " " << GetCo1333BiasRotated(1) << " " << GetCs662BiasRotated(1) << " " << GetTh2615BiasRotated(1) << endl;
  sstream << "MS: " << GetCo1173BiasRotated(2) << " " << GetCo1333BiasRotated(2) << " " << GetCs662BiasRotated(2) << " " << GetTh2615BiasRotated(2) << endl << endl;
  sstream << "EXOEnergyBias is set to use " << (fUseDatabaseForCharge ? "database " : "user provided ") << "values for charge" << endl;
  sstream << "                        and " << (fUseDatabaseForRotated ? "database " : "user provided ") << "values for rotated" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnergyBiasHandler)

EXOCalibBase* EXOEnergyBiasHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOEnergyBias* newData = new EXOEnergyBias();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOEnergyBiasHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOEnergyBias* newData = new EXOEnergyBias();

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
  selectColumns.push_back("Charge_Co1");
  selectColumns.push_back("Charge_Co2");
  selectColumns.push_back("Charge_Cs");
  selectColumns.push_back("Charge_Th");
  selectColumns.push_back("Rotated_Co1");
  selectColumns.push_back("Rotated_Co2");
  selectColumns.push_back("Rotated_Cs");
  selectColumns.push_back("Rotated_Th");

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
        newData->fBiasesChargeSS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fBiasesChargeSS[1] = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fBiasesChargeSS[2] = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fBiasesChargeSS[3] = EXOMiscUtil::stringToDouble(fields[4]);
        newData->fBiasesRotatedSS[0] = EXOMiscUtil::stringToDouble(fields[5]);
        newData->fBiasesRotatedSS[1] = EXOMiscUtil::stringToDouble(fields[6]);
        newData->fBiasesRotatedSS[2] = EXOMiscUtil::stringToDouble(fields[7]);
        newData->fBiasesRotatedSS[3] = EXOMiscUtil::stringToDouble(fields[8]);
      }
      else if(multiplicity == "MS"){
        newData->fBiasesChargeMS[0] = EXOMiscUtil::stringToDouble(fields[1]);
        newData->fBiasesChargeMS[1] = EXOMiscUtil::stringToDouble(fields[2]);
        newData->fBiasesChargeMS[2] = EXOMiscUtil::stringToDouble(fields[3]);
        newData->fBiasesChargeMS[3] = EXOMiscUtil::stringToDouble(fields[4]);
        newData->fBiasesRotatedMS[0] = EXOMiscUtil::stringToDouble(fields[5]);
        newData->fBiasesRotatedMS[1] = EXOMiscUtil::stringToDouble(fields[6]);
        newData->fBiasesRotatedMS[2] = EXOMiscUtil::stringToDouble(fields[7]);
        newData->fBiasesRotatedMS[3] = EXOMiscUtil::stringToDouble(fields[8]);
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
