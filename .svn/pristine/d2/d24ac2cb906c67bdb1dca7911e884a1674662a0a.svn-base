//______________________________________________________________________________
#include "EXOCalibUtilities/EXOEnergyMCBasedFit.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

bool EXOEnergyMCBasedFit::fUseDB = true;
std::vector<std::string> EXOEnergyMCBasedFit::fParNamesInDB = EXOEnergyMCBasedFit::GetParNamesInDB();
std::map<std::string, double> EXOEnergyMCBasedFit::fUserPars = EXOEnergyMCBasedFit::GetTemplatePars();

std::vector<std::string> EXOEnergyMCBasedFit::GetParNamesInDB()
{
  std::vector<std::string> names;
  names.push_back("Multiplicity");
  names.push_back("Angle");
  names.push_back("WeekIndex");
  
  std::vector<std::string> channels;
  channels.push_back("Rotated");
  channels.push_back("Ionization");
  channels.push_back("Scintillation");

  std::vector<std::string> parTypes;
  parTypes.push_back("Calibration");
  parTypes.push_back("Resolution");
  parTypes.push_back("TriggerEfficiency");  

  for(int i = 0; i <= 2; i++)
  {
    names.push_back(Form("Correlation_p%d",i));
    if(i == 0)
      names.push_back(Form("TriggerEfficiency_Correlation_p%d",i));
    for(std::vector<std::string>::iterator channel = channels.begin(); channel != channels.end(); channel++)
    {
      std::string ch = *channel;
      for(std::vector<std::string>::iterator parType = parTypes.begin(); parType != parTypes.end(); parType++)
      {
        std::string par = *parType;
        if(par == "TriggerEfficiency" && i == 2)
          continue;
        names.push_back(Form("%s_%s_p%d",ch.c_str(),par.c_str(),i));
      }
    }
  }

  std::sort(names.begin(),names.end());
  return names;  
}

std::map<std::string, double> EXOEnergyMCBasedFit::GetTemplatePars()
{
  std::map<std::string, double> pars;

  std::vector<std::string> multStrs;
  multStrs.push_back("SS");
  multStrs.push_back("MS");
  
  for(std::vector<std::string>::iterator multStr = multStrs.begin(); multStr != multStrs.end(); multStr++)
  {
    std::string mult = *multStr;
    for(std::vector<std::string>::iterator parName = fParNamesInDB.begin(); parName != fParNamesInDB.end(); parName++)
    {
      std::string name = *parName;
      pars.insert(std::pair<std::string, double>(Form("%s_%s",name.c_str(),mult.c_str()),0.));
    }
  }

  return pars;
}

EXOEnergyMCBasedFit::EXOEnergyMCBasedFit() : EXOCalibBase()
{
  fDBPars.clear();
  fDBPars = EXOEnergyMCBasedFit::GetTemplatePars();
}

double EXOEnergyMCBasedFit::GetParByName(const char* name, int multiplicity) const
{
  std::string multStr = (multiplicity == 1) ? "SS" : "MS";
  std::string varName = Form("%s_%s",name,multStr.c_str());

  if(!fUseDB)
  {
    if(fUserPars.count(varName) == 1)
      return fUserPars.at(varName);
    LogEXOMsg(Form("In EXOEnergyMCBasedFit, variable with name %s was not found!!!",varName.c_str()),EEError);
    return std::numeric_limits<double>::quiet_NaN();
  }

  if(fDBPars.count(varName) == 1)
    return fDBPars.at(varName);

  LogEXOMsg(Form("In EXOEnergyMCBasedFit, variable with name %s was not found!!!",varName.c_str()),EEError);
  return std::numeric_limits<double>::quiet_NaN();
}

void EXOEnergyMCBasedFit::SetUserValues(const std::map<std::string, double>& userValues, int multiplicity)
{
  std::string multStr = (multiplicity == 1) ? "SS" : "MS"; 
  for(std::map<std::string, double>::const_iterator userValue = userValues.begin(); userValue != userValues.end(); userValue++)
  {
    if(std::find(fParNamesInDB.begin(),fParNamesInDB.end(),userValue->first) == fParNamesInDB.end())
      LogEXOMsg(Form("Trying to set user value in EXOEnergyMCBasedFit for a column that does not exist in DB: %s!!!",userValue->first.c_str()),EEError);
    fUserPars[Form("%s_%s",userValue->first.c_str(),multStr.c_str())] = userValue->second;
  }
}


void EXOEnergyMCBasedFit::UseDatabase(bool useDB)
{
  fUseDB = useDB;
}

std::string EXOEnergyMCBasedFit::__str__() const
{
  std::stringstream sstream;
  sstream << "-------------------------EXOEnergyMCBasedFit-------------------------" << std::endl;
  sstream << "Energy MC-Based Fit calibration has the following values:" << std::endl;
  sstream << "EXOEnergyMCBasedFit is set to use " << (fUseDB ? "database " : "user provided ") << "values" << std::endl;
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

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnergyMCBasedFitHandler)

EXOCalibBase* EXOEnergyMCBasedFitHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOEnergyMCBasedFit* newData = new EXOEnergyMCBasedFit();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOEnergyMCBasedFitHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOEnergyMCBasedFit* newData = new EXOEnergyMCBasedFit();

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
  selectColumns = EXOEnergyMCBasedFit::GetParNamesInDB();

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() != 2){
    LogEXOMsg("Number of rows for this entry differs from 2",EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string>::iterator multIter = std::find(selectColumns.begin(),selectColumns.end(),"Multiplicity");
  size_t multPos = multIter - selectColumns.begin();
  if(multIter == selectColumns.end())
  {
    LogEXOMsg("Selected columns does not contain entry Multiplicity",EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  for(size_t i=0; i<2; i++) {
    res.getRow(fields, i);
    //for(size_t j = 0; j < fields.size(); j++)
    //  std::cout << j << " " << selectColumns[j] << " = " << fields[j] << std::endl;

    try
    {
      std::string multStr = fields[multPos];
      int multInt = (multStr == "SS") ? 1 : 2;

      for(size_t j = 0; j < fields.size(); j++)
      {
        std::string parName = Form("%s_%s",selectColumns[j].c_str(),multStr.c_str());
        if(j == multPos)
          newData->fDBPars[parName] = static_cast<double>(multInt);
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
