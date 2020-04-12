//______________________________________________________________________________
#include "EXOCalibUtilities/EXOGridCorrectionCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <cmath>
#include <cstring>
#include <iostream>
#include <TROOT.h>
#include <TClass.h>
//#include <TF1.h>
#include "TTimeStamp.h"

//______________________________________________________________________________
const std::vector <double>& EXOGridCorrectionCalib::GetGridCorrParameters() const
{
  return fparams;
}

//______________________________________________________________________________
double EXOGridCorrectionCalib::GetGridCorrParameter(int i) const
{
  if (i < int( fparams.size() )){
    return fparams[i];
  }
  // If i >= number of entries in fparams:
  LogEXOMsg("Index out-of-range in fparams.", EEWarning);
  return 0;
}


IMPLEMENT_EXO_CALIB_HANDLER(EXOGridCorrectionCalibHandler)

//______________________________________________________________________________
EXOCalibBase* EXOGridCorrectionCalibHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOGridCorrectionCalib * newData = new EXOGridCorrectionCalib();
  newData->fparams.push_back(100);
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

//______________________________________________________________________________
EXOCalibBase* EXOGridCorrectionCalibHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOGridCorrectionCalib* newData = new EXOGridCorrectionCalib();

  // parse dataIdent.  It should have form
  // TABLE_NAME:COLUMN_NAME:VALUE
  size_t delim1  = dataIdent.find(':');
  size_t delim2  = dataIdent.rfind(':');

  // if (delim1 == delim2) { we've got a problem }
  std::string tbl = std::string(dataIdent, 0, delim1);
  std::string col = std::string(dataIdent, delim1 + 1, delim2 - delim1 - 1);
  std::string val = std::string(dataIdent, delim2 + 1);

  // Select which columns to read from TABLE_NAME where COLUMN_NAME=VALUE
  std::vector<std::string> selectColumns, orderColumns;
  selectColumns.push_back("Scale_1U");
  selectColumns.push_back("Scale_2U");
  selectColumns.push_back("Scale_3U");
  selectColumns.push_back("Scale_4U");
  selectColumns.push_back("Anode_pos_1U");
  selectColumns.push_back("Anode_pos_2U");
  selectColumns.push_back("Anode_pos_3U");
  selectColumns.push_back("Anode_pos_4U");
  selectColumns.push_back("eqn_type");

  std::string where(" where ");
  where += col + std::string(" = '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  size_t nrows = res.getNRows();
  std::vector<std::string> fields;
  for (size_t ix = 0; ix < nrows; ix++) {
    res.getRow(fields, ix);
    std::vector <double> params_read;
    try {
      for (size_t ip = 0; ip < fields.size()-1; ip++){
        params_read.push_back( EXOMiscUtil::stringToDouble(fields[ip]) );
      }
      newData->fEqnType = fields[fields.size()-1];
      fields.clear();
    }
    catch (EXOExceptWrongType ex) {
      LogEXOMsg("Bad row in calibration data ''", EEError);
      delete newData;
      return 0;
    }
    newData->fparams = params_read;
  }
  return newData;
  
}
