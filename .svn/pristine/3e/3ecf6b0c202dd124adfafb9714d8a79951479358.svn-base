//______________________________________________________________________________
#include "EXOCalibUtilities/EXODriftVelocityCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <string>
#include <iostream>

IMPLEMENT_EXO_CALIB_HANDLER(EXODriftVelocityHandler)

//______________________________________________________________________________
EXOCalibBase* EXODriftVelocityHandler::readDB(const std::string& dataIdent,
                                              const std::string& formatVersion)
{

  // Once we return newData, it is owned by EXOCalibManager.
  EXODriftVelocityCalib* newData = new EXODriftVelocityCalib;

  // Normally, should parse dataIdent.  It should have form TABLE_NAME:COLUMN_NAME:VALUE.
  // But, it turns out we need extra identifiers anyway (like "type"), so might as well drop the parsing.
  std::vector <std::string> getColumns;
  getColumns.push_back("type"); // this column contains the labels TPC1, TPC2.
  getColumns.push_back("drift_velocity"); // bulk drift velocity
  getColumns.push_back("collection_velocity"); // this column is the drift velocity between the wireplanes 
  getColumns.push_back("collection_time"); // this should have a constant relation to the collection_velocity
  std::string where("where drift_velocity_calib_key = '");// ...yes this is the actual name of the primary key for the drift_velocity table...
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);// the value of said key found for the condition valid at the investigated time
  where.push_back('\''); // where = "where drift_velocity_calib_key = '#'"
  RowResults res = GetDBRowsWith("drift_velocity", getColumns, getColumns, where);

  // read values that have just been fetched; extract TPC1 and TPC2 values.
  bool found_TPC1 = false;
  bool found_TPC2 = false;
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);
    if(readline[0] == "TPC1") {
      if(found_TPC1) {
        LogEXOMsg("Database error: multiple entries for TPC1", EECritical);
      }
      newData->m_DriftVelocityHolder.drift_velocity_TPC1 = EXOMiscUtil::stringToDouble(readline[1]);
      newData->m_DriftVelocityHolder.collection_velocity_TPC1 = EXOMiscUtil::stringToDouble(readline[2]);
      newData->m_DriftVelocityHolder.collection_time_TPC1 = EXOMiscUtil::stringToDouble(readline[3]);
      found_TPC1 = true;
    }
    else if(readline[0] == "TPC2") {
      if(found_TPC2) {
        LogEXOMsg("Database error: multiple entries for TPC2", EECritical);
      }
      try { // unclear why only one tpc read is wrapped in a try... 
        newData->m_DriftVelocityHolder.drift_velocity_TPC2 = EXOMiscUtil::stringToDouble(readline[1]);
	newData->m_DriftVelocityHolder.collection_velocity_TPC2 = EXOMiscUtil::stringToDouble(readline[2]);
	newData->m_DriftVelocityHolder.collection_time_TPC2 = EXOMiscUtil::stringToDouble(readline[3]);
      }
      catch (EXOExceptWrongType ex) {
        LogEXOMsg("Untranslatable item in calibration data", EECritical);
        delete newData;
        return NULL;
      }
      found_TPC2 = true;
    }
  }
  if(!found_TPC1 or !found_TPC2) {
    delete newData;  // If we allocated any memory, then that should be cleared.
    return NULL;
  }

  return newData;

}
