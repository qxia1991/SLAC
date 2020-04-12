//______________________________________________________________________________
#include "EXOCalibUtilities/EXODiagonalCut.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <iostream>
#include <sstream>

using namespace std;

bool EXODiagonalCut::fUseDatabase = true;
double EXODiagonalCut::fUserP0_SS = 2600.67;
double EXODiagonalCut::fUserP1_SS = 3.405;
double EXODiagonalCut::fUserP0_MS = 2876.03;
double EXODiagonalCut::fUserP1_MS = 3.63036;

EXODiagonalCut::EXODiagonalCut() : 
  EXOCalibBase(),
  fP0_SS(2600.67),
  fP1_SS(3.405),
  fP0_MS(2876.03),
  fP1_MS(3.63036)
{ 

}

void EXODiagonalCut::SetUserValues(double p0_ss, double p1_ss, double p0_ms, double p1_ms)
{
  fUseDatabase = false;
  fUserP0_SS = p0_ss;
  fUserP1_SS = p1_ss;
  fUserP0_MS = p0_ms;
  fUserP1_MS = p1_ms;
}

void EXODiagonalCut::UseDatabase()
{
  fUseDatabase = true;
}

double EXODiagonalCut::GetCriticalScintillationValueSS(double chargeEnergy) const
{
  //Return the maximum scintillation energy allowed to survive the single site diagonal cut
  return GetP1_SS() * chargeEnergy + GetP0_SS();
}

double EXODiagonalCut::GetCriticalScintillationValueMS(double chargeEnergy) const
{
  //Return the maximum scintillation energy allowed to survive the multi site diagonal cut
  return GetP1_MS() * chargeEnergy + GetP0_MS();
}

bool EXODiagonalCut::SurvivesSingleSiteCut(double scintEnergy, double chargeEnergy) const
{
  //Return whether the combination of scintEnergy and chargeEnergy survives the single site diagonal cut
  if(scintEnergy > GetCriticalScintillationValueSS(chargeEnergy)){
    return false;
  }
  return true;
}

bool EXODiagonalCut::SurvivesMultiSiteCut(double scintEnergy, double chargeEnergy) const
{
  //Return whether the combination of scintEnergy and chargeEnergy survives the multi site diagonal cut
  if(scintEnergy > GetCriticalScintillationValueMS(chargeEnergy)){
    return false;
  }
  return true;
}

string EXODiagonalCut::__str__() const
{
  stringstream sstream;
  sstream << "-------------------------EXODiagonalCut-------------------------" << endl;
  sstream << "Diagonal cut allows scintillation to be at most:" << endl;
  sstream << "Scint = " << GetP1_SS() << " * Charge + " << GetP0_SS() << " (SS)" << endl;
  sstream << "Scint = " << GetP1_MS() << " * Charge + " << GetP0_MS() << " (MS)" << endl;
  sstream << "EXODiagonalCut is set to use " << (fUseDatabase ? "database " : "user provided ") << "values" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

double EXODiagonalCut::GetP0_SS() const
{
  if(!fUseDatabase){
    return fUserP0_SS;
  }
  return fP0_SS;
}

double EXODiagonalCut::GetP1_SS() const
{
  if(!fUseDatabase){
    return fUserP1_SS;
  }
  return fP1_SS;
}

double EXODiagonalCut::GetP0_MS() const
{
  if(!fUseDatabase){
    return fUserP0_MS;
  }
  return fP0_MS;
}

double EXODiagonalCut::GetP1_MS() const
{
  if(!fUseDatabase){
    return fUserP1_MS;
  }
  return fP1_MS;
}

IMPLEMENT_EXO_CALIB_HANDLER(EXODiagonalCutHandler)

EXOCalibBase* EXODiagonalCutHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  //Return default values if the database can't be read

  // Once newData gets returned, EXOCalibManager owns it.
  EXODiagonalCut* newData = new EXODiagonalCut();
  return newData;
}

EXOCalibBase* EXODiagonalCutHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXODiagonalCut* newData = new EXODiagonalCut();

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
  selectColumns.push_back("P0_ms");
  selectColumns.push_back("P0_ss");
  selectColumns.push_back("P1_ms");
  selectColumns.push_back("P1_ss");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() > 1){
    LogEXOMsg("Received more rows than expected", EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  res.getRow(fields, 0);
  try {
    newData->fP0_MS = EXOMiscUtil::stringToDouble(fields[0]);
    newData->fP0_SS = EXOMiscUtil::stringToDouble(fields[1]);
    newData->fP1_MS = EXOMiscUtil::stringToDouble(fields[2]);
    newData->fP1_SS = EXOMiscUtil::stringToDouble(fields[3]);
  }
  catch (EXOExceptWrongType ex) {
    stringstream message;
    message << "Bad row in calibration data: " << ex.what();
    LogEXOMsg(message.str(), EEError);
    delete newData;
    return 0;
  }
  return newData;
}
