//______________________________________________________________________________
#include "EXOCalibUtilities/EXOEnvironmentCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <string>
#include <iostream>
  
//______________________________________________________________________________

IMPLEMENT_EXO_CALIB_HANDLER(EXOEnvironmentHandler)


bool EXOEnvironmentCalib::GetSirenState() {
  return sirenInCR;
}
  
bool EXOEnvironmentCalib::IsBadTimeInterval() {
  return BadTimeInterval;
}
 
EXOCalibBase* EXOEnvironmentHandler::readDB(const std::string& dataIdent,
                                              const std::string& formatVersion)
{
  
  // Once we return newData, it is owned by EXOCalibManager.
  EXOEnvironmentCalib* newData = new EXOEnvironmentCalib;

  
  // Normally, should parse dataIdent.  It should have form TABLE_NAME:COLUMN_NAME:VALUE.
  // But, it turns out we need extra identifiers anyway (like "type"), so might as well drop the parsing.
  std::vector <std::string> getColumns;
  getColumns.push_back("Evacuation_alarm_string");
  getColumns.push_back("bad_time_interval");

  getColumns.push_back("Id");

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  std::cout << "DataIdent: " << dataIdent.substr(dataIdent.find_last_of(':')+1) << std::endl;
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("environment", getColumns, getColumns, where);
  //EXOTimestamp tmstmp = newData->validSince();
  //std::cout << tmstmp.getString() << std::endl;

   // read values that have just been fetched
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);
    LogEXOMsg(readline[0], EENotice );
    LogEXOMsg(readline[1], EENotice );
    LogEXOMsg(readline[2], EENotice );

    if(readline[0] == "false") newData->sirenInCR = false;
    else if(readline[0] == "true") newData->sirenInCR = true;
    else LogEXOMsg("Environment Database error: Returns unreadable value",EEAlert);

    if(readline[1] == "false") newData->BadTimeInterval = false;
    else if(readline[1] == "true") newData->BadTimeInterval = true;
    else LogEXOMsg("Environment Database error: returns unreadable value", EEAlert);
  }
  
  return newData;
  
}
