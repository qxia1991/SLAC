#include "EXOCalibUtilities/EXOVWireThresholds.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <sstream>
#include <string>

IMPLEMENT_EXO_CALIB_HANDLER(EXOVWireThresholdsHandler)

EXOVWireThresholdsHandler::EXOVWireThresholdsHandler()
: EXOCalibHandlerBase(),
  fDefaultThreshold(3.78) //This is just the average value for the time of writing (5-Apr-2012)
{

}

double EXOVWireThresholds::GetThresholdOnChannel(int channel) const
{
  ChannelMap::const_iterator Iter = m_thresholds.find(channel);
  if(Iter == m_thresholds.end()) {
    std::ostringstream stream;
    stream<<"The threshold on channel "<<channel<<" was not found.";
    LogEXOMsg(stream.str(), EEAlert);
    return 0.0;
  }
  return Iter->second;
}

bool EXOVWireThresholds::HasChannel(int channel) const
{
  ChannelMap::const_iterator Iter = m_thresholds.find(channel);
  return (Iter != m_thresholds.end());
}

EXOCalibBase* EXOVWireThresholdsHandler::readDefault(const std::string& dataIdent, const std::string& formatVersion)
{
  EXOVWireThresholds* newCalib = new EXOVWireThresholds;
  for(size_t channel = NCHANNEL_PER_WIREPLANE; channel < 2*NCHANNEL_PER_WIREPLANE; channel++) {
    newCalib->m_thresholds[channel] = fDefaultThreshold;
  }
  for(size_t channel = 3*NCHANNEL_PER_WIREPLANE; channel < 4*NCHANNEL_PER_WIREPLANE; channel++) {
    newCalib->m_thresholds[channel] = fDefaultThreshold;
  }
  return newCalib;
}

EXOCalibBase* EXOVWireThresholdsHandler::readDB(const std::string& dataIdent, const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOVWireThresholds* newData = new EXOVWireThresholds;

  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns;
  getColumns.push_back("SoftwareChannel");
  getColumns.push_back("Threshold");

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("vwire_thresholds", getColumns, getColumns, where);

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

    // Read threshold.
    stream.clear();
    stream.str(readline[1]);
    double threshold;
    result = (bool)(stream >> threshold);
    if(not result) {
       LogEXOMsg("Converting \"" + readline[1] + "\" to a gain failed.", EEAlert);
       delete newData;
       return NULL;
    }

    newData->m_thresholds[channel] = threshold;
  }

  // Check that all V-wires got filled.
  for(size_t channel = NCHANNEL_PER_WIREPLANE; channel < 2*NCHANNEL_PER_WIREPLANE; channel++) {
    if(not newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "Failed to find v-wire threshold for channel "<<channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }
  }
  for(size_t channel = 3*NCHANNEL_PER_WIREPLANE; channel < 4*NCHANNEL_PER_WIREPLANE; channel++) {
    if(not newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "Failed to find v-wire threshold for channel "<<channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }
  }

  return newData;
}
