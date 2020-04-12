//______________________________________________________________________________
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOEnergyZCorrection.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

bool EXOEnergyZCorrection::fUseDB = true;
std::vector<std::string> EXOEnergyZCorrection::fParNamesInDB = EXOEnergyZCorrection::GetParNamesInDB();
std::map<std::string, double> EXOEnergyZCorrection::fUserPars = EXOEnergyZCorrection::GetTemplatePars();
std::map<std::string, int> EXOEnergyZCorrection::fEqnType = EXOEnergyZCorrection::GetEqnTypeMap();

std::map<std::string, int> EXOEnergyZCorrection::GetEqnTypeMap()
{
  std::map<std::string, int> eqn;
  eqn.insert(std::pair<std::string, double>("linear",1));
  eqn.insert(std::pair<std::string, double>("power",2));

  return eqn;
}

std::vector<std::string> EXOEnergyZCorrection::GetParNamesInDB()
{
  std::vector<std::string> names;
  names.push_back("Eqn_type");
  names.push_back("Week_index");
  for(int i = 0; i < 6; i++)
  {
    names.push_back(Form("P%d",i));
  }
  
  std::sort(names.begin(),names.end());
  return names;  
}

std::map<std::string, double> EXOEnergyZCorrection::GetTemplatePars()
{
  std::map<std::string, double> pars;
  
  for(std::vector<std::string>::iterator parName = fParNamesInDB.begin(); parName != fParNamesInDB.end(); parName++)
  {
    std::string name = *parName;
    pars.insert(std::pair<std::string, double>(name,0.));
  }

  return pars;
}

EXOEnergyZCorrection::EXOEnergyZCorrection() : EXOCalibBase()
{
  fDBPars.clear();
  fDBPars = EXOEnergyZCorrection::GetTemplatePars();
}

double EXOEnergyZCorrection::GetZcorrection(double z, std::string flavor, const EXOEventHeader& header)
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return GetZcorrection(z,flavor,seconds, 1000*microseconds);
}

double EXOEnergyZCorrection::GetZcorrection(double z, std::string flavor, long int seconds, int nano)
{
  return GetZcorrection(z,flavor, EXOTimestamp(seconds,nano));
}

double EXOEnergyZCorrection::GetZcorrection(double z, std::string flavor, const EXOTimestamp& time)
{
  EXOEnergyZCorrection* zCorr = GetCalibrationFor(EXOEnergyZCorrection, EXOEnergyZCorrectionHandler, flavor, time);
  
  int eqnType = zCorr->GetParByName("Eqn_type");
  if(eqnType == 1)
    return EvalLinear(z,zCorr);
  if(eqnType == 2)
    return EvalPower(z,zCorr);

  LogEXOMsg("Equation type not set, will not evaluate z correction.",EEError);
  
  return 0.;
}

double EXOEnergyZCorrection::EvalLinear(double z, EXOEnergyZCorrection* zCorr)
{
  if(z < 0)
    return zCorr->GetParByName("P0") + zCorr->GetParByName("P1")*(-z);
  
  return zCorr->GetParByName("P3") + zCorr->GetParByName("P4")*(z);
}

double EXOEnergyZCorrection::EvalPower(double z, EXOEnergyZCorrection* zCorr)
{
  if(z < 0)
    return zCorr->GetParByName("P0") + zCorr->GetParByName("P1")*pow(-z,zCorr->GetParByName("P2"));
  
  return zCorr->GetParByName("P3") + zCorr->GetParByName("P4")*pow(z,zCorr->GetParByName("P5"));
}

int EXOEnergyZCorrection::GetEquationTypeAsInt(const char* name) const
{
  std::string type(name);
  if(fEqnType.count(type) < 1)
    return 0;
  return fEqnType.at(type);
}

double EXOEnergyZCorrection::GetParByName(const char* name) const
{
  std::string varName(name);

  if(!fUseDB)
  {
    if(fUserPars.count(varName) == 1)
      return fUserPars.at(varName);
    LogEXOMsg(Form("In EXOEnergyZCorrection, variable with name %s was not found!!!",varName.c_str()),EEError);
    return std::numeric_limits<double>::quiet_NaN();
  }

  if(fDBPars.count(varName) == 1)
    return fDBPars.at(varName);

  LogEXOMsg(Form("In EXOEnergyZCorrection, variable with name %s was not found!!!",varName.c_str()),EEError);
  return std::numeric_limits<double>::quiet_NaN();
}

void EXOEnergyZCorrection::SetUserValues(const std::map<std::string, double>& userValues)
{
  for(std::map<std::string, double>::const_iterator userValue = userValues.begin(); userValue != userValues.end(); userValue++)
  {
    if(std::find(fParNamesInDB.begin(),fParNamesInDB.end(),userValue->first) == fParNamesInDB.end())
      LogEXOMsg(Form("Trying to set user value in EXOEnergyZCorrection for a column that does not exist in DB: %s!!!",userValue->first.c_str()),EEError);
    fUserPars[userValue->first] = userValue->second;
  }
}

void EXOEnergyZCorrection::UseDatabase(bool useDB)
{
  fUseDB = useDB;
}

std::string EXOEnergyZCorrection::__str__() const
{
  std::stringstream sstream;
  sstream << "-------------------------EXOEnergyZCorrection-------------------------" << std::endl;
  sstream << "Energy Z Correction has the following values:" << std::endl;
  sstream << "EXOEnergyZCorrection is set to use " << (fUseDB ? "database " : "user provided ") << "values" << std::endl;
  if(fUseDB)
  {
    for(std::map<std::string, double>::const_iterator par = fDBPars.begin(); par != fDBPars.end(); par++)
    {
      sstream << par->first << " = " << par->second << std::endl;
    }
  }
  else
  {
    for(std::map<std::string, double>::const_iterator par = fUserPars.begin(); par != fUserPars.end(); par++)
    {
      sstream << par->first << " = " << par->second << std::endl;
    }
  }
  
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnergyZCorrectionHandler)

EXOCalibBase* EXOEnergyZCorrectionHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOEnergyZCorrection* newData = new EXOEnergyZCorrection();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOEnergyZCorrectionHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOEnergyZCorrection* newData = new EXOEnergyZCorrection();

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
  selectColumns = EXOEnergyZCorrection::GetParNamesInDB();

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() > 1){
    LogEXOMsg("Number of rows for this entry > 1",EEError);
    delete newData;
    return 0;
  }
  
  std::vector<std::string>::iterator eqnIter = std::find(selectColumns.begin(),selectColumns.end(),"Eqn_type");
  size_t eqnPos = eqnIter - selectColumns.begin();
  if(eqnIter == selectColumns.end())
  {
    LogEXOMsg("Selected columns does not contain Equation type",EEError);
    delete newData;
    return 0;
  }
  
  //for(size_t i=0; i<res.getNRows(); i++) {
  std::vector<std::string> fields;
  res.getRow(fields, 0);
  for(size_t j = 0; j < fields.size(); j++)
  {
    //std::cout << j << " " << selectColumns[j] << " = " << fields[j] << std::endl;

    try
    {
      for(size_t j = 0; j < fields.size(); j++)
      {
        std::string parName(selectColumns[j].c_str());
        if(j == eqnPos)
          newData->fDBPars[parName] = static_cast<double>(newData->GetEquationTypeAsInt(fields[j].c_str()));
        else
          newData->fDBPars[parName] = EXOMiscUtil::stringToDouble(fields[j]);
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
