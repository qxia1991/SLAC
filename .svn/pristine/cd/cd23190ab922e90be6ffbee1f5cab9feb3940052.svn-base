//______________________________________________________________________________
//   EXOAPDGains
// Queries APD gains from database to calculate number of registered photons.
//______________________________________________________________________________
#include "EXOCalibUtilities/EXOAPDGains.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh" // for Form()

IMPLEMENT_EXO_CALIB_HANDLER(EXOAPDGainsHandler)

EXOAPDGains::EXOAPDGains() : EXOCalibBase()
  { gains.reserve(NUMBER_READOUT_CHANNELS); }

#define _exists(A) ((A)<gains.size() && gains[(A)].gain!=0.)
bool EXOAPDGains::exists(unsigned int ich) const
{
  return _exists(ich);
}

double EXOAPDGains::gain(unsigned int ich) const
{
  if (!_exists(ich)) {
    std::ostringstream stream;
    stream<<"The gain on channel "<<ich<<" was not found.";
    LogEXOMsg(stream.str(), EEWarning);
    return 0.0;
  }
  return gains[ich].gain;
}

double EXOAPDGains::gain_error(unsigned int ich) const
{
  if (!_exists(ich)) {
    std::ostringstream stream;
    stream<<"The gain on channel "<<ich<<" was not found.";
    LogEXOMsg(stream.str(), EEWarning);
    return 0.0;
  }
  return gains[ich].gain_error;
}

//const unsigned char NUMBER_READOUT_CHANNELS = NCHANNEL_PER_WIREPLANE*NWIREPLANE+NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;
//const double APD_GAIN = 200.0;
const unsigned int NCHANNELS = NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;
const unsigned int ICHANNEL0 = NCHANNEL_PER_WIREPLANE*NWIREPLANE;

EXOCalibBase* EXOAPDGainsHandler::readDefault(const std::string& dataIdent, const std::string& formatVersion)
{
  EXOAPDGains* newCalib = new EXOAPDGains;
  EXOAPDGains::channel_gain g;
  g.ichannel = unsigned(-1), g.gain = 0., g.gain_error = 0.;
  newCalib->gains.resize(NUMBER_READOUT_CHANNELS,g);
  for(unsigned int ich=ICHANNEL0; ich<NCHANNELS; ich++) {
    g.ichannel = ich, g.gain = APD_GAIN, g.gain_error = 0.;
    newCalib->gains[ich] = g;
  }
  return newCalib;
}

EXOCalibBase* EXOAPDGainsHandler::readDB(const std::string& dataIdent, const std::string& formatVersion)
{
  //std::cout << "dataIdent " << dataIdent << " formatVersion " << formatVersion << std::endl;
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOAPDGains* newCalib = new EXOAPDGains;
  EXOAPDGains::channel_gain g0;
  g0.ichannel = unsigned(-1), g0.gain = 0., g0.gain_error = 0.;
  newCalib->gains.resize(NUMBER_READOUT_CHANNELS,g0);

  std::string::size_type i, j;
  i = dataIdent.find(':');
  j = dataIdent.find(':',i+1);
  std::string where;
  if (i!=std::string::npos && j!=std::string::npos)
    where += "where " + dataIdent.substr(i+1,j-i-1) + " = '" + dataIdent.substr(j+1) + "'";
  else
    LogEXOMsg("Can't parse ident string: '"+dataIdent+"'", EEWarning);
  // where = "where id = '#'"
  //std::cout << __func__ <<" "<< dataIdent.substr(0,i) <<" "<< where << std::endl;

  std::string dataIdentData = dataIdent.substr(0,i);
  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns, order;
  if(dataIdentData == "APDGains")
    getColumns.push_back("Channel");
  else if(dataIdentData == "APD_Gains")
    getColumns.push_back("ChannelID");
  getColumns.push_back("Gain");
  if(dataIdentData == "APD_Gains")
    getColumns.push_back("GainError");


  RowResults res = GetDBRowsWith(dataIdent.substr(0,i), getColumns, order, where);

  std::cout << __func__ <<" "<< res.getNRows() << std::endl;
  // read values that have just been fetched.
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);
    //std::cout << "getrow "; for(int j=0;j<readline.size();j++){std::cout <<" " << getColumns[j] << " = "<< readline[j];} std::cout << std::endl;

    char *p = 0;
    // Read channel number.
    int ich = strtol(readline[0].c_str(),&p,10);
    if (!p && *p!='\0') {
      std::ostringstream stream;
      stream << "Bad value for channel number: " << ich << " from '" << readline[0] << "' line# " << i;
      LogEXOMsg(stream.str(), EEWarning);
    }
    if(newCalib->exists(ich)) {
      std::ostringstream stream;
      stream << "There are duplicate entries for channel " << ich;
      LogEXOMsg(stream.str(), EEWarning);
    }

    // get the rest
    EXOAPDGains::channel_gain g;
    g.ichannel = ich;
    g.gain = strtof(readline[1].c_str(),&p);
    if (!p && *p!='\0') {
      std::ostringstream stream;
      stream << "Bad value for channel gain: " << g.gain << " from '" << readline[1] << "' line# "<<i;
      LogEXOMsg(stream.str(), EEWarning);
    }
    g.gain_error = 0.;//strtof(readline[2].c_str(),&p);
    if(dataIdentData == "APD_Gains")
      g.gain_error = strtof(readline[2].c_str(),&p);
    //std::cout <<"\t"<<g.ichannel<<" "<<g.gain<<" "<<g.gain_error<<" "<<newCalib->gains[ich].ichannel << std::endl;
    // ... and store
    if (ich>0) {
      if ((unsigned int)ich>=newCalib->gains.size()) newCalib->gains.resize(ich,g0);
      newCalib->gains[ich] = g;
    }
  }
  
  // Check that all U-wires got filled.
  for(unsigned int ich=ICHANNEL0; ich<NCHANNELS; ich++) {
    if(!newCalib->exists(ich)) {
      std::ostringstream stream;
      stream << "Failed to find u-wire correction for channel " << ich;
      LogEXOMsg(stream.str(), EEWarning);
    }
  }

  return newCalib;
}
