//______________________________________________________________________________
#include "EXOCalibUtilities/EXOLifetimeCalib.hh"
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

EXOLifetimeCalib::EXOLifetimeCalib(const EXOLifetimeCalib& other) 
  : EXOCalibBase(other) {
  fn_start = other.fn_start;
  fn_type = other.fn_type;
  fn_params = other.fn_params;
  fn = other.fn;
}

double EXOLifetimeCalib::lifetime(const TTimeStamp& t, double flow) const
{
  return lifetime(t.AsDouble(), flow);
}

double EXOLifetimeCalib::lifetime(double event_time, double flow) const
{
  // calculate the electron lifetime using provided function name and parameters
  // Argument of function assume time since fn_start in days
  // Result of function is lifetime in CLHEP standard units (nanoseconds)

  double lifetime = 0.;
  double x = (event_time - fn_start)/86400.; // event time in days, as parameters are in days.
  if (fn_type == "pol10") { // let's cheat
    double xi = 1.;
    for (unsigned int i = 0; i < fn_params.size(); i++,xi*=x) lifetime += xi * fn_params[i];
    return lifetime*CLHEP::microsecond;
  }
  if (fn_type == "two-phase") { // let's cheat
    // get flow rate
    if (flow<0) {
        LogEXOMsg(Form("Unphysical flow rate input for twophase model: %.2f SLPM", flow), EEError);
        // FIXME
        // do something to exit?
    }
    lifetime = twoPhaseModel(x, flow);
    // two-phase model output lifetime in ms
    return lifetime*CLHEP::microsecond;
  }
  if (!fn && fn_type!="") { // let's try a ROOT function
    TObject* obj = gROOT->GetFunction(fn_type.c_str()); //this (vs FindObject) can also find default functions (eg expo, gaus ), others eg exponential with an offset need to be declared as new named functions to be found... every new fn_type must be added to the DB table's type enum to be preapproved for readback...
    EXOLifetimeCalib *t = const_cast<EXOLifetimeCalib*>(this);
    if (obj && obj->IsA() && !strcmp(obj->IsA()->GetName(),"TF1")) t->fn = (TF1*)obj;
    if (t->fn) for (unsigned int i = 0; i < fn_params.size(); i++) t->fn->SetParameter(i,fn_params[i]);
  }
  if (fn) {
    lifetime = (*fn)(x);
  } else {
    LogEXOMsg("Lifetime function type '"+fn_type+"' not recognized.", EEAlert);
    lifetime = INFINITY; // or 0 ?
  }

  return lifetime*CLHEP::microsecond;
}

double EXOLifetimeCalib::twoPhaseModel(double t, double flow) const {
    // t in days since last interupption
    // flow rate in SLPM
    //
    // two-phase model expression from Cindy
    // z: flow, x: time 
    // n = 1/(k0 TauInitial z (V + z \[Alpha]) (k0 V - 1. z + 
    //    k0 z \[Alpha])) E^(-((x (z + k0 (V + z \[Alpha])))/
    //  V)) (E^((x (z + k0 (V + z \[Alpha])))/V)
    //     kp TauInitial V (z (-1. z - 1. V \[Epsilon]) + 
    //      k0 (V + z \[Alpha]) (z + V \[Epsilon])) + 
    //   E^((x z)/V)
    //     z (k0 (kp V + z) (-1. V - 1. z \[Alpha]) + 
    //      k0^2 (1. V^2 + 2. V z \[Alpha] + 1. z^2 \[Alpha]^2) + 
    //      kp TauInitial V (1. z + 1. V \[Epsilon])) + 
    //   E^((k0 x (V + z \[Alpha]))/V)
    //     k0 kp V ((1. - 1. TauInitial) z^2 \[Alpha] - 
    //      1. TauInitial V^2 \[Epsilon] + 
    //      V z (1. + TauInitial (-1. - 1. \[Alpha] \[Epsilon]))))

    double tau0     = fn_params[0]; // initial lifetime [ms] at each period
    double C        = fn_params[1]; // constants to relate impurity concentration to tau
    double epsilon  = fn_params[2];
    double k0       = fn_params[3];
    double alpha    = fn_params[4];
    double kp       = fn_params[5];

    const double V  = 58; // detector volumn in liters

    double n = 1./k0/tau0/flow/(V+flow*alpha)/
                (k0*V-flow+k0*flow*alpha)*
                std::exp(-t*(flow+(k0*(V+flow*alpha)))/V)*(
                    std::exp(t*(flow+k0*(V+flow*alpha))/V)*kp*tau0*V*
                    (flow*(-flow-V*epsilon)+k0*(V+flow*alpha)*(flow+V*epsilon)) +
                    std::exp(t*flow/V)*flow*(
                        k0*(kp*V+flow)*(-V-flow*alpha)+
                        k0*k0*(V*V+2.*V*flow*alpha+flow*flow*alpha*alpha) + 
                        kp*tau0*V*(flow+V*epsilon) ) + 
                    std::exp(k0*t*(V+flow*alpha)/V)*k0*kp*V*( 
                        (1.-tau0)*flow*flow*alpha-tau0*V*V*epsilon +
                        V*flow*(1.+tau0*(-1.-alpha*epsilon)) )
                );
    double tau = C/n; // in ms

    // convert to microsecond to be consistent with other functypes in lifetime() 
    tau = tau*1000.;

    return tau;

}

IMPLEMENT_EXO_CALIB_HANDLER(EXOLifetimeCalibHandler)

EXOCalibBase* EXOLifetimeCalibHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOLifetimeCalib * newData = new EXOLifetimeCalib();
  newData->fn_type = "pol0"; // fixed value
  newData->fn_params.push_back(100);
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOLifetimeCalibHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOLifetimeCalib* newData = new EXOLifetimeCalib();

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
    if (functype_read == "pol10" ||
        functype_read == "two-phase") { // to help the later cheating
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
