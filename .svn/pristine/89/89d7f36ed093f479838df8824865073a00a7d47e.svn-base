//______________________________________________________________________________
#include "EXOCalibUtilities/EXOFlowRate.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <cmath>
#include <cstring>
#include <iostream>
#include <TROOT.h>
#include <TClass.h>
#include <TF1.h>
#include "TTimeStamp.h"

EXOFlowRate::EXOFlowRate(const EXOFlowRate& other) 
  : EXOCalibBase(other) {
  fn_start = other.fn_start;
  fn_type = other.fn_type;
  fn_params = other.fn_params;
  fn = other.fn;
}

double EXOFlowRate::flow(const TTimeStamp& t) const
{
  return flow(t.AsDouble());
}

double EXOFlowRate::flow(double event_time) const
{

  double flow = 0.;
  double x = (event_time - fn_start)/86400.; // event time in days, as parameters are in days.
  if (fn_type == "pol10") { // let's cheat
    double xi = 1.;
    for (unsigned int i = 0; i < fn_params.size(); i++,xi*=x) flow += xi * fn_params[i];
    return flow;
  }
  if (!fn && fn_type!="") { // let's try a ROOT function
    TObject* obj = gROOT->GetFunction(fn_type.c_str()); //this (vs FindObject) can also find default functions (eg expo, gaus ), others eg exponential with an offset need to be declared as new named functions to be found... every new fn_type must be added to the DB table's type enum to be preapproved for readback...
    EXOFlowRate *t = const_cast<EXOFlowRate*>(this);
    if (obj && obj->IsA() && !strcmp(obj->IsA()->GetName(),"TF1")) t->fn = (TF1*)obj;
    if (t->fn) for (unsigned int i = 0; i < fn_params.size(); i++) t->fn->SetParameter(i,fn_params[i]);
  }
  if (fn) {
    flow = (*fn)(x);
  } else {
    LogEXOMsg("Flow rate function type '"+fn_type+"' not recognized.", EEAlert);
    flow = INFINITY; // or 0 ?
  }

  return flow;
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOFlowRateHandler)

EXOCalibBase* EXOFlowRateHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOFlowRate * newData = new EXOFlowRate();
  newData->fn_type = "pol0"; // fixed value
  newData->fn_params.push_back(100);
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOFlowRateHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOFlowRate* newData = new EXOFlowRate();

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
  selectColumns.push_back("Origin");
  selectColumns.push_back("p0");
  selectColumns.push_back("p1");
  selectColumns.push_back("p2");
  selectColumns.push_back("p3");
  selectColumns.push_back("p4");
  selectColumns.push_back("p5");
  selectColumns.push_back("p6");
  selectColumns.push_back("p7");
  selectColumns.push_back("p8");
  selectColumns.push_back("p9");
  selectColumns.push_back("Type");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  unsigned nrows = res.getNRows();
  std::vector<std::string> fields;
  for (unsigned ix = 0; ix < nrows; ix++) { // this loop is just loading ix = nrows -1 but checking all rows. 
    res.getRow(fields, ix);
    std::string functype_read; 
    double origin_read;
    std::vector <double> params_read;
    try {
      origin_read = EXOMiscUtil::stringToDouble(fields[0]);
      functype_read = fields[11];
      for (unsigned ip = 1; ip < 11; ip++){
        params_read.push_back( EXOMiscUtil::stringToDouble(fields[ip]) );
      }
      fields.clear();
    }
    catch (EXOExceptWrongType ex) {
      LogEXOMsg("Bad row in calibration data ''", EEError);
      delete newData;
      return 0;
    }
    if (functype_read == "pol10") { // to help the later cheating
      while (!params_read.empty() && params_read.back() == 0){
	params_read.pop_back(); // saves some operations per event
      }
    }
    newData->fn_start = origin_read;
    newData->fn_type = functype_read;
    newData->fn_params = params_read;
  }
  return newData;
  
}
