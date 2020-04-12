//______________________________________________________________________________
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include <string>
#include <sstream>
#include <cassert>

IMPLEMENT_EXO_CALIB_HANDLER(EXOElectronicsShapersHandler)

//______________________________________________________________________________
EXOCalibBase* EXOElectronicsShapersHandler::readDefault(const std::string& dataIdent,
                                                        const std::string& formatVersion)
{
  return FillWithDefault();
}
//______________________________________________________________________________
EXOCalibBase* EXOElectronicsShapersHandler::readDB(const std::string& dataIdent,
                                                   const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOElectronicsShapers* newData = new EXOElectronicsShapers;

  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns;
  getColumns.push_back("Software_channel");
  getColumns.push_back("Scheme");
  for(int shaper_index = 0; shaper_index < 10; shaper_index++) {
    std::ostringstream stream;
    stream << "T" << shaper_index;
    getColumns.push_back(stream.str());
  }

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("shaping_times", getColumns, getColumns, where);

  // read values that have just been fetched.
  std::map<size_t, EXOTransferFunction>& TransferFunctionMap = newData->m_ElectronicsShapersMap;
  TransferFunctionMap.clear();
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);
    assert(readline.size() >= 2); // We need at least two columns, to have a well-defined read.
    std::istringstream stream;

    // Read channel number.
    stream.clear();
    stream.str(readline[0]);
    size_t channel;
    bool result = (bool)(stream >> channel);
    assert(result); // If result is false, the conversion failed.
    assert(not newData->HasTransferFunctionForChannel(channel)); // If this fails, there was a duplicate entry.
    EXOTransferFunction& aTransfer = TransferFunctionMap[channel];

    // Read shaping times.
    std::string scheme = readline[1];
    for(size_t CharIndex = 0; CharIndex < scheme.size(); CharIndex++) {
      char& ShaperType = scheme[CharIndex];
      assert(ShaperType == 'D' or ShaperType == 'I'); // Any other character is an error.
      assert(readline.size() > 2+CharIndex); // We need to have fetched enough columns to store all of the shaping times.
      stream.clear();
      stream.str(readline[2+CharIndex]);
      double ShapingTimeInMicroSeconds;
      result = (bool)(stream >> ShapingTimeInMicroSeconds);
      assert(result); // If result is false, the conversion failed.
      if(ShaperType == 'D') {
        aTransfer.AddDiffStageWithTime(ShapingTimeInMicroSeconds*CLHEP::microsecond);
      }
      else /*ShaperType == 'I'*/ {
        aTransfer.AddIntegStageWithTime(ShapingTimeInMicroSeconds*CLHEP::microsecond);
      }
    }
  }

  // Check that all channels got filled.
  for(size_t i = 0; i < NUMBER_READOUT_CHANNELS; i++) {
    assert(newData->HasTransferFunctionForChannel(i));
  }
  // Need to check that all APDs have identical shaping times -- otherwise, some pieces of code (particularly reconstruction) will break.
  for(size_t i = NCHANNEL_PER_WIREPLANE*NWIREPLANE; i < NUMBER_READOUT_CHANNELS; i++) {
    assert(newData->GetTransferFunctionForChannel(i) ==
           newData->GetTransferFunctionForChannel(NCHANNEL_PER_WIREPLANE*NWIREPLANE));
  }

  return newData;

}

//______________________________________________________________________________
EXOCalibBase* EXOElectronicsShapersHandler::FillWithDefault() const
{
  // Allocate and return an EXOElectronicsShapers object filled with default values.
  // The object is owned by whoever called the function.
  EXOElectronicsShapers* ThisCalib = new EXOElectronicsShapers;
  assert(ThisCalib); // If ThisCalib is NULL, the cast or allocation failed.

  // Clear the transfer function map, replacing it with locally-stored values (in case we're recycling oldData).
  ThisCalib->m_ElectronicsShapersMap.clear();
  ThisCalib->m_SourceOfData = EXOCalib::METADATASOURCEmysql; // TEMPORARY!!  Right now there is no correct database set of values.
  LogEXOMsg("Letting EXOElectronicsShapers fake a database read, until the database is correctly implemented.", EENotice);

  for(size_t channel = 0; channel < NCHANNEL_PER_WIREPLANE*NWIREPLANE; channel++) {
    EXOTransferFunction& Transfer = ThisCalib->m_ElectronicsShapersMap[channel];
    Transfer.AddDiffStageWithTime(CR_TAU_WIRE_1);
    Transfer.AddDiffStageWithTime(CR_TAU_WIRE_2);
    Transfer.AddDiffStageWithTime(CR_TAU_WIRE_3);
    Transfer.AddIntegStageWithTime(RC_TAU_WIRE_1);
    Transfer.AddIntegStageWithTime(RC_TAU_WIRE_2);
  }
  for(size_t channel = NCHANNEL_PER_WIREPLANE*NWIREPLANE; channel < NUMBER_READOUT_CHANNELS; channel++) {
    EXOTransferFunction& Transfer = ThisCalib->m_ElectronicsShapersMap[channel];
    Transfer.AddDiffStageWithTime(CR_TAU_APD_1);
    Transfer.AddDiffStageWithTime(CR_TAU_APD_2);
    Transfer.AddDiffStageWithTime(CR_TAU_APD_3);
    Transfer.AddIntegStageWithTime(RC_TAU_APD_1);
    Transfer.AddIntegStageWithTime(RC_TAU_APD_2);
  }
  return ThisCalib;
}
