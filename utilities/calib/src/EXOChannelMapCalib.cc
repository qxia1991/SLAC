// 4/04/11 S.S.
// EXOChannelMapCalib.cc
// Reads Channel Map data from the Database. 

#include "EXOCalibUtilities/EXOChannelMapCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

IMPLEMENT_EXO_CALIB_HANDLER(EXOChannelMapHandler)

//______________________________________________________________________________
EXOCalibBase* EXOChannelMapHandler::readDB(const std::string& dataIdent,
                                           const std::string& formatVersion)
{
  using namespace EXOCalib;

  // When newData gets returned, EXOCalibManager will own it.
  EXOChannelMapCalib* newData = new EXOChannelMapCalib;
  
  // Get names of the columns to be read from the database
  std::vector <std::string> getColumns;
  getColumns.push_back("software_chan"); 
  getColumns.push_back("card_address");
  getColumns.push_back("card_channel");
  getColumns.push_back("good_channel");
  getColumns.push_back("Suppressed_by_daq");

  // Parse dataIdent.  It should have form TABLE_NAME:COLUMN_NAME:VALUE.
  // See also EXOLifetimeCalibHandler.cc for explicit parsing   
  std::string where("where chan_map_version = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where chan_map_version = '#'";
  // Fetch the values from the database
  RowResults res = GetDBRowsWith("TPC_channel_map", getColumns, getColumns, where);
  
  // Read the values that have just been fetched.
  // Each row of res is an array containing
  // [0] software_chan,
  // [1] card_address, 
  // [2] card_channel, 
  // [3] good_channel,
  // [4] Suppressed_by_daq

  cout << res.getNRows() << " rows in Channel Map DB." << endl;

  // Initialize to -1; some card channels are not connected to
  // software channels and will retain software channel = -1.
  int* temp_map = new int[NUM_DAQCARDS*CHANNELS_PER_DAQCARD];
  for (int k = 0; k < NUM_DAQCARDS*CHANNELS_PER_DAQCARD; k++) temp_map[k] = -1;
  bool* temp_good_channel_array = new bool[226];

  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline, i);
    if (readline.size() != getColumns.size()) {
      LogEXOMsg("Unexpected row numbers.", EEError);
      return NULL;
    }
    for (size_t k = 0; k < getColumns.size(); k++){
      if ( readline[k].compare("") == 0 || readline[k].compare(" ") == 0 ){
        std::ostringstream output_stream;
        output_stream << "Empty field in DB at line " << i << ". Returning Default Channel Map.";
        LogEXOMsg(output_stream.str(), EEError);
        return NULL;
      }
    }
       
    try {
      int software_channel = EXOMiscUtil::stringToInt(readline[0]);
      temp_map[newData->get_card_number_array(EXOMiscUtil::stringToInt(readline[1]))*CHANNELS_PER_DAQCARD
               + EXOMiscUtil::stringToInt(readline[2])] = software_channel;
      // FixME -- this assumes the channels are returned from the database ordered by software channel, with none missing.
      // We shouldn't depend on i to mean anything.
      // Not changing for now, so that this commit is exactly equivalent to before.
      temp_good_channel_array[i] = EXOMiscUtil::stringToInt(readline[3]);
      if(readline[4] == "1") newData->fSuppressedByDaq.insert(software_channel);
    }
    catch (EXOExceptWrongType ex) {
      std::string temp;
      for (size_t k=0;k<readline.size();k++) temp += readline[k] + " "; 
      LogEXOMsg("Untranslatable to integers: " + temp, EECritical);
      delete newData;
      return NULL;
    }
   
  }

  newData->set_map( temp_map );
  newData->set_good_channel_array ( temp_good_channel_array );
  newData->set_version( EXOChannelMap::kDBChannelMap );

  return newData;
  
}


