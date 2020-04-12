//______________________________________________________________________________
#include "EXOCalibUtilities/EXOEnergyResCalib.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

using namespace std;

vector<double> EXOEnergyResCalib::fUserChargeSS(3,0);  
vector<double> EXOEnergyResCalib::fUserChargeMS(3,0);  
vector<double> EXOEnergyResCalib::fUserScintSS(3,0);   
vector<double> EXOEnergyResCalib::fUserScintMS(3,0);   
vector<double> EXOEnergyResCalib::fUserRotatedSS(3,0); 
vector<double> EXOEnergyResCalib::fUserRotatedMS(3,0); 

bool EXOEnergyResCalib::fUseDatabaseForCharge = true;
bool EXOEnergyResCalib::fUseDatabaseForScint = true;
bool EXOEnergyResCalib::fUseDatabaseForRotated = true;

EXOEnergyResCalib::EXOEnergyResCalib() : 
  EXOCalibBase(),
  fChargeSS(3,0),  
  fChargeMS(3,0),  
  fScintSS(3,0),   
  fScintMS(3,0),   
  fRotatedSS(3,0), 
  fRotatedMS(3,0) 
{

}

void EXOEnergyResCalib::UseMCBasedFitCalibration(EXOEnergyMCBasedFit* mcbasedFit)
{ 
  EXOEnergyResCalib::SetUserValuesForRotated(mcbasedFit->GetParByName("Rotated_Resolution_p0",1),
                                             mcbasedFit->GetParByName("Rotated_Resolution_p1",1),
                                             mcbasedFit->GetParByName("Rotated_Resolution_p2",1),
                                             mcbasedFit->GetParByName("Rotated_Resolution_p0",2),
                                             mcbasedFit->GetParByName("Rotated_Resolution_p1",2),
                                             mcbasedFit->GetParByName("Rotated_Resolution_p2",2));
  EXOEnergyResCalib::SetUserValuesForCharge(mcbasedFit->GetParByName("Ionization_Resolution_p0",1),
                                            mcbasedFit->GetParByName("Ionization_Resolution_p1",1),
                                            mcbasedFit->GetParByName("Ionization_Resolution_p2",1),
                                            mcbasedFit->GetParByName("Ionization_Resolution_p0",2),
                                            mcbasedFit->GetParByName("Ionization_Resolution_p1",2),
                                            mcbasedFit->GetParByName("Ionization_Resolution_p2",2));
  EXOEnergyResCalib::SetUserValuesForScint(mcbasedFit->GetParByName("Scintillation_Resolution_p0",1),
                                           mcbasedFit->GetParByName("Scintillation_Resolution_p1",1),
                                           mcbasedFit->GetParByName("Scintillation_Resolution_p2",1),
                                           mcbasedFit->GetParByName("Scintillation_Resolution_p0",2),
                                           mcbasedFit->GetParByName("Scintillation_Resolution_p1",2),
                                           mcbasedFit->GetParByName("Scintillation_Resolution_p2",2));
}


void EXOEnergyResCalib::SetUserValuesForCharge(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS)
{
  fUseDatabaseForCharge = false;
  fUserChargeSS[0] = p0SS*p0SS;
  fUserChargeSS[1] = p1SS*p1SS;
  fUserChargeSS[2] = p2SS*p2SS;
  fUserChargeMS[0] = p0MS*p0MS;
  fUserChargeMS[1] = p1MS*p1MS;
  fUserChargeMS[2] = p2MS*p2MS;
}

void EXOEnergyResCalib::SetUserValuesForScint(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS)
{
  fUseDatabaseForScint = false;
  fUserScintSS[0] = p0SS*p0SS;
  fUserScintSS[1] = p1SS*p1SS;
  fUserScintSS[2] = p2SS*p2SS;
  fUserScintMS[0] = p0MS*p0MS;
  fUserScintMS[1] = p1MS*p1MS;
  fUserScintMS[2] = p2MS*p2MS;
}

void EXOEnergyResCalib::SetUserValuesForRotated(double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS)
{
  fUseDatabaseForRotated = false;
  fUserRotatedSS[0] = p0SS*p0SS;
  fUserRotatedSS[1] = p1SS*p1SS;
  fUserRotatedSS[2] = p2SS*p2SS;
  fUserRotatedMS[0] = p0MS*p0MS;
  fUserRotatedMS[1] = p1MS*p1MS;
  fUserRotatedMS[2] = p2MS*p2MS;
}

double EXOEnergyResCalib::Model(double energy, double p0_2, double p1_2, double p2_2) const
{
  return sqrt(p0_2*energy + p1_2 + p2_2*energy*energy);
}

double EXOEnergyResCalib::ScintillationResolution(double energy, int multiplicity) const
{
  //Return the scintillation energy resolution.
  //The following model is used:
  //    ________________________
  //\  /  2        2     2   2  
  // \/ P0 * E + P1  + P2 * E
  // 
  // where E = energy
  // and P0, P1, P2 are the parameters read from the database

  double val = 0;
  if(fUseDatabaseForScint){
    if(multiplicity > 1){
      val = Model(energy,fScintMS[0],fScintMS[1],fScintMS[2]);
    }
    else{
      val = Model(energy,fScintSS[0],fScintSS[1],fScintSS[2]);
    }
  }
  else{
    if(multiplicity > 1){
      val = Model(energy,fUserScintMS[0],fUserScintMS[1],fUserScintMS[2]);
    }
    else{
      val = Model(energy,fUserScintSS[0],fUserScintSS[1],fUserScintSS[2]);
    }
  }
  return val;
}

double EXOEnergyResCalib::ChargeResolution(double energy, int multiplicity) const
{
  //Return the charge energy resolution.
  //The following model is used:
  //    ________________________
  //\  /  2        2     2   2  
  // \/ P0 * E + P1  + P2 * E
  // 
  // where E = energy
  // and P0, P1, P2 are the parameters read from the database

  double val = 0;
  if(fUseDatabaseForCharge){
    if(multiplicity > 1){
      val = Model(energy,fChargeMS[0],fChargeMS[1],fChargeMS[2]);
    }
    else{
      val = Model(energy,fChargeSS[0],fChargeSS[1],fChargeSS[2]);
    }
  }
  else{
    if(multiplicity > 1){
      val = Model(energy,fUserChargeMS[0],fUserChargeMS[1],fUserChargeMS[2]);
    }
    else{
      val = Model(energy,fUserChargeSS[0],fUserChargeSS[1],fUserChargeSS[2]);
    }
  }
  return val;
}

double EXOEnergyResCalib::RotatedResolution(double energy, int multiplicity) const
{
  //Return the rotated energy resolution.
  //The following model is used:
  //    ________________________
  //\  /  2        2     2   2  
  // \/ P0 * E + P1  + P2 * E
  // 
  // where E = energy
  // and P0, P1, P2 are the parameters read from the database

  double val = 0;
  if(fUseDatabaseForRotated){
    if(multiplicity > 1){
      val = Model(energy,fRotatedMS[0],fRotatedMS[1],fRotatedMS[2]);
    }
    else{
      val = Model(energy,fRotatedSS[0],fRotatedSS[1],fRotatedSS[2]);
    }
  }
  else{
    if(multiplicity > 1){
      val = Model(energy,fUserRotatedMS[0],fUserRotatedMS[1],fUserRotatedMS[2]);
    }
    else{
      val = Model(energy,fUserRotatedSS[0],fUserRotatedSS[1],fUserRotatedSS[2]);
    }
  }
  return val;
}

std::string EXOEnergyResCalib::ScintillationResolutionString(std::string EnergyLabel, int multiplicity) const
{
  // Return a string version of the formula used for ScintillationResolution.
  // EnergyLabel will stand in for the energy.
  // This allows us to create cut strings on datasets.
  double p0 = 0;
  double p1 = 0;
  double p2 = 0;
  if(multiplicity > 1){
    p0 = (fUseDatabaseForScint ? fScintMS[0] : fUserScintMS[0]);
    p1 = (fUseDatabaseForScint ? fScintMS[1] : fUserScintMS[1]);
    p2 = (fUseDatabaseForScint ? fScintMS[2] : fUserScintMS[2]);
  }
  else{
    p0 = (fUseDatabaseForScint ? fScintSS[0] : fUserScintSS[0]);
    p1 = (fUseDatabaseForScint ? fScintSS[1] : fUserScintSS[1]);
    p2 = (fUseDatabaseForScint ? fScintSS[2] : fUserScintSS[2]);
  }

  EnergyLabel = "(" + EnergyLabel + ")";
  std::ostringstream ret;
  ret << "sqrt(" << p0 << "*" << EnergyLabel << " + " << p1;
  ret << " + " << p2 << "*" << EnergyLabel << "*" << EnergyLabel << ")";
  return ret.str();
}

std::string EXOEnergyResCalib::ChargeResolutionString(std::string EnergyLabel, int multiplicity) const
{
  // Return a string version of the formula used for ChargeResolution.
  // EnergyLabel will stand in for the energy.
  // This allows us to create cut strings on datasets.
  double p0 = 0;
  double p1 = 0;
  double p2 = 0;
  if(multiplicity > 1){
    p0 = (fUseDatabaseForCharge ? fChargeMS[0] : fUserChargeMS[0]);
    p1 = (fUseDatabaseForCharge ? fChargeMS[1] : fUserChargeMS[1]);
    p2 = (fUseDatabaseForCharge ? fChargeMS[2] : fUserChargeMS[2]);
  }
  else{
    p0 = (fUseDatabaseForCharge ? fChargeSS[0] : fUserChargeSS[0]);
    p1 = (fUseDatabaseForCharge ? fChargeSS[1] : fUserChargeSS[1]);
    p2 = (fUseDatabaseForCharge ? fChargeSS[2] : fUserChargeSS[2]);
  }

  EnergyLabel = "(" + EnergyLabel + ")";
  std::ostringstream ret;
  ret << "sqrt(" << p0 << "*" << EnergyLabel << " + " << p1;
  ret << " + " << p2 << "*" << EnergyLabel << "*" << EnergyLabel << ")";
  return ret.str();
}

std::string EXOEnergyResCalib::RotatedResolutionString(std::string EnergyLabel, int multiplicity) const
{
  // Return a string version of the formula used for RotatedResolution.
  // EnergyLabel will stand in for the energy.
  // This allows us to create cut strings on datasets.
  double p0 = 0;
  double p1 = 0;
  double p2 = 0;
  if(multiplicity > 1){
    p0 = (fUseDatabaseForRotated ? fRotatedMS[0] : fUserRotatedMS[0]);
    p1 = (fUseDatabaseForRotated ? fRotatedMS[1] : fUserRotatedMS[1]);
    p2 = (fUseDatabaseForRotated ? fRotatedMS[2] : fUserRotatedMS[2]);
  }
  else{
    p0 = (fUseDatabaseForRotated ? fRotatedSS[0] : fUserRotatedSS[0]);
    p1 = (fUseDatabaseForRotated ? fRotatedSS[1] : fUserRotatedSS[1]);
    p2 = (fUseDatabaseForRotated ? fRotatedSS[2] : fUserRotatedSS[2]);
  }

  EnergyLabel = "(" + EnergyLabel + ")";
  std::ostringstream ret;
  ret << "sqrt(" << p0 << "*" << EnergyLabel << " + " << p1;
  ret << " + " << p2 << "*" << EnergyLabel << "*" << EnergyLabel << ")";
  return ret.str();
}

string EXOEnergyResCalib::__str__() const
{
  stringstream sstream;
  sstream << "-----------------------EXOEnergyResCalib------------------------" << endl;
  sstream << "The following model is used:" << endl;
  sstream << "    ________________________" << endl;
  sstream << "\\  /  2        2     2   2  " << endl;
  sstream << " \\/ P0 * E + P1  + P2 * E   " << endl << endl;
  sstream << "Single Site:" << endl;
  sstream << "             P0         P1        P2" << endl;
  sstream << "Charge:  " << sqrt(fChargeSS[0]) << " " << sqrt(fChargeSS[1]) << " " << sqrt(fChargeSS[2]) << endl;
  sstream << "Scint:   " << sqrt(fScintSS[0]) << " " << sqrt(fScintSS[1]) << " " << sqrt(fScintSS[2]) << endl;
  sstream << "Rotated: " << sqrt(fRotatedSS[0]) << " " << sqrt(fRotatedSS[1]) << " " << sqrt(fRotatedSS[2]) << endl << endl;
  sstream << "Multi Site:" << endl;
  sstream << "             P0         P1        P2" << endl;
  sstream << "Charge:  " << sqrt(fChargeMS[0]) << " " << sqrt(fChargeMS[1]) << " " << sqrt(fChargeMS[2]) << endl;
  sstream << "Scint:   " << sqrt(fScintMS[0]) << " " << sqrt(fScintMS[1]) << " " << sqrt(fScintMS[2]) << endl;
  sstream << "Rotated: " << sqrt(fRotatedMS[0]) << " " << sqrt(fRotatedMS[1]) << " " << sqrt(fRotatedMS[2]) << endl << endl;
  sstream << "EXOEnergyResCalib is set to use " << (fUseDatabaseForCharge ? "DATABASE " : "USER ") << "values for charge," << endl;
  sstream << "                                " << (fUseDatabaseForScint ? "DATABASE " : "USER ") << "values for scint," << endl;
  sstream << "                            and " << (fUseDatabaseForRotated ? "DATABASE " : "USER ") << "values for rotated." << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnergyResCalibHandler)

EXOCalibBase* EXOEnergyResCalibHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOEnergyResCalib* newData = new EXOEnergyResCalib();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOEnergyResCalibHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOEnergyResCalib* newData = new EXOEnergyResCalib();

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
  selectColumns.push_back("Charge_p2");
  selectColumns.push_back("Scint_p0");
  selectColumns.push_back("Scint_p1");
  selectColumns.push_back("Scint_p2");
  selectColumns.push_back("Rotated_p0");
  selectColumns.push_back("Rotated_p1");
  selectColumns.push_back("Rotated_p2");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() != 2){
    LogEXOMsg("Received different number of rows than expected", EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  for(size_t i=0; i<2; i++){
    res.getRow(fields, i);
    string multiplicity = fields[0];
    double c_p0, c_p1, c_p2;
    double s_p0, s_p1, s_p2;
    double r_p0, r_p1, r_p2;
    try {
      c_p0 = EXOMiscUtil::stringToDouble(fields[1]);
      c_p1 = EXOMiscUtil::stringToDouble(fields[2]);
      c_p2 = EXOMiscUtil::stringToDouble(fields[3]);
      s_p0 = EXOMiscUtil::stringToDouble(fields[4]);
      s_p1 = EXOMiscUtil::stringToDouble(fields[5]);
      s_p2 = EXOMiscUtil::stringToDouble(fields[6]);
      r_p0 = EXOMiscUtil::stringToDouble(fields[7]);
      r_p1 = EXOMiscUtil::stringToDouble(fields[8]);
      r_p2 = EXOMiscUtil::stringToDouble(fields[9]);
    }
    catch (EXOExceptWrongType ex) {
      LogEXOMsg("Bad row in calibration data ''", EEError);
      delete newData;
      return 0;
    }
    if(multiplicity == "SS"){
      newData->fChargeSS[0] = c_p0*c_p0;
      newData->fChargeSS[1] = c_p1*c_p1;
      newData->fChargeSS[2] = c_p2*c_p2;
      newData->fScintSS[0] = s_p0*s_p0;
      newData->fScintSS[1] = s_p1*s_p1;
      newData->fScintSS[2] = s_p2*s_p2;
      newData->fRotatedSS[0] = r_p0*r_p0;
      newData->fRotatedSS[1] = r_p1*r_p1;
      newData->fRotatedSS[2] = r_p2*r_p2;
    }
    else if(multiplicity == "MS"){
      newData->fChargeMS[0] = c_p0*c_p0;
      newData->fChargeMS[1] = c_p1*c_p1;
      newData->fChargeMS[2] = c_p2*c_p2;
      newData->fScintMS[0] = s_p0*s_p0;
      newData->fScintMS[1] = s_p1*s_p1;
      newData->fScintMS[2] = s_p2*s_p2;
      newData->fRotatedMS[0] = r_p0*r_p0;
      newData->fRotatedMS[1] = r_p1*r_p1;
      newData->fRotatedMS[2] = r_p2*r_p2;
    }
    else{
      stringstream stream;
      stream << "Unknown multiplicity value: " << multiplicity;
      LogEXOMsg(stream.str(),EEError);
      delete newData;
      return 0;
    }
  }

  return newData;
}
