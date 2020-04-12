#include "EXOCalibUtilities/EXOVWireGains.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <sstream>
#include <string>

IMPLEMENT_EXO_CALIB_HANDLER(EXOVWireGainsHandler)

double EXOVWireGains::GetGainOnChannel(size_t channel) const
{
  std::map<size_t, DoubleWithError>::const_iterator Iter = m_gains.find(channel);
  if(Iter == m_gains.end()) {
    std::ostringstream stream;
    stream<<"The gain on channel "<<channel<<" was not found.";
    LogEXOMsg(stream.str(), EEAlert);
    return 0.0;
  }
  return Iter->second.value;
}

double EXOVWireGains::GetGainErrorOnChannel(size_t channel) const
{
  std::map<size_t, DoubleWithError>::const_iterator Iter = m_gains.find(channel);
  if(Iter == m_gains.end()) {
    std::ostringstream stream;
    stream<<"The gain error on channel "<<channel<<" was not found.";
    LogEXOMsg(stream.str(), EEAlert);
    return 0.0;
  }
  return Iter->second.error;
}

bool EXOVWireGains::HasChannel(size_t channel) const
{
  std::map<size_t, DoubleWithError>::const_iterator Iter = m_gains.find(channel);
  return (Iter != m_gains.end());
}

EXOCalibBase* EXOVWireGainsHandler::readDefault(const std::string& dataIdent, const std::string& formatVersion)
{
  EXOVWireGains* newCalib = new EXOVWireGains;
  for(size_t channel = 0; channel < NCHANNEL_PER_WIREPLANE; channel++) {
    EXOVWireGains::DoubleWithError aCalib;
    aCalib.value = 300;
    aCalib.error = 0;
    newCalib->m_gains[channel] = aCalib;
    newCalib->m_gains_only[channel] = aCalib.value;
  }
  for(size_t channel = 2*NCHANNEL_PER_WIREPLANE; channel < 3*NCHANNEL_PER_WIREPLANE; channel++) {
    EXOVWireGains::DoubleWithError aCalib;
    aCalib.value = 300;
    aCalib.error = 0;
    newCalib->m_gains[channel] = aCalib;
    newCalib->m_gains_only[channel] = aCalib.value;
  }
  return newCalib;
}

EXOCalibBase* EXOVWireGainsHandler::readDB(const std::string& dataIdent, const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOVWireGains* newData = new EXOVWireGains;

  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns;
  getColumns.push_back("SoftwareChannel");
  getColumns.push_back("Gain");
  getColumns.push_back("Error");

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("vwire", getColumns, getColumns, where);

  // read values that have just been fetched.
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);
    std::istringstream stream;

    // Read channel number.
    stream.clear();
    stream.str(readline[0]);
    size_t channel;
    bool result = (bool)(stream >> channel);
    if(not result) {
       LogEXOMsg("Converting \"" + readline[0] + "\" to a channel number failed.", EEAlert);
       delete newData;
       return NULL;
    }
    if(newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "There are duplicate entries for channel " << channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }

    // Read gain.
    stream.clear();
    stream.str(readline[1]);
    double gain;
    result = (bool)(stream >> gain);
    if(not result) {
       LogEXOMsg("Converting \"" + readline[1] + "\" to a gain failed.", EEAlert);
       delete newData;
       return NULL;
    }

    // Read error in gain.
    stream.clear();
    stream.str(readline[2]);
    double gain_error;
    result = (bool)(stream >> gain_error);
    if(not result) {
       LogEXOMsg("Converting \"" + readline[2] + "\" to a gain error failed.", EEAlert);
       delete newData;
       return NULL;
    }

    EXOVWireGains::DoubleWithError aCalib;
    aCalib.value = gain;
    aCalib.error = gain_error;
    newData->m_gains[channel] = aCalib;
    newData->m_gains_only[channel] = aCalib.value;
  }

  // Check that all U-wires got filled.
  for(size_t channel = NCHANNEL_PER_WIREPLANE; channel < 2*NCHANNEL_PER_WIREPLANE; channel++) {
    if(not newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "Failed to find v-wire correction for channel "<<channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }
  }
  for(size_t channel = 3*NCHANNEL_PER_WIREPLANE; channel < 4*NCHANNEL_PER_WIREPLANE; channel++) {
    if(not newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "Failed to find v-wire correction for channel "<<channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }
  }

  return newData;
}
