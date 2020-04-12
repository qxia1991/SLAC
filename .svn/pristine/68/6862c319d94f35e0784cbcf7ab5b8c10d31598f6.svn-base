#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <sstream>
#include <string>

IMPLEMENT_EXO_CALIB_HANDLER(EXOMCChannelScalingHandler)

double EXOMCChannelScaling::GetScalingOnChannel(const size_t channel) const
{
  std::map<size_t, double>::const_iterator Iter = m_scalings.find(channel);
  if(Iter == m_scalings.end()) {
    std::ostringstream stream;
    stream<<"The scaling on channel "<<channel<<" was not found.";
    LogEXOMsg(stream.str(), EEAlert);
    return 0.0;
  }
  return Iter->second;
}

bool EXOMCChannelScaling::HasChannel(const size_t channel) const
{
  std::map<size_t, double>::const_iterator Iter = m_scalings.find(channel);
  return (Iter != m_scalings.end());
}

EXOCalibBase* EXOMCChannelScalingHandler::readDefault(const std::string& dataIdent, const std::string& formatVersion)
{
  EXOMCChannelScaling* newCalib = new EXOMCChannelScaling;
  for(size_t channel = 0; channel < NUMBER_READOUT_CHANNELS; channel++) {
    double aCalib = 1.0;
    newCalib->m_scalings[channel] = aCalib;
  }
  return newCalib;
}

EXOCalibBase* EXOMCChannelScalingHandler::readDB(const std::string& dataIdent, const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOMCChannelScaling* newData = new EXOMCChannelScaling;

  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns;
  getColumns.push_back("software_channel");
  getColumns.push_back("scale_factor");

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("mc_scaling", getColumns, getColumns, where);

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

    // Read scaling.
    stream.clear();
    stream.str(readline[1]);
    double scaling;
    result = (bool)(stream >> scaling);
    if(not result) {
       LogEXOMsg("Converting \"" + readline[1] + "\" to a scaling failed.", EEAlert);
       delete newData;
       return NULL;
    }

    newData->m_scalings[channel] = scaling;
  }

  // Check that all channels got filled.
  for(size_t channel = 0; channel < NUMBER_READOUT_CHANNELS; channel++) {
    if(not newData->HasChannel(channel)) {
      std::ostringstream output;
      output << "Failed to find mc scaling for channel "<<channel;
      LogEXOMsg(output.str(), EEAlert);
      delete newData;
      return NULL;
    }
  }

  return newData;
}
