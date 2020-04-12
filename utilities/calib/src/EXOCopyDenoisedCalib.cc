//______________________________________________________________________________
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCopyDenoisedCalib.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>

bool EXOCopyDenoisedCalib::fUseDB = true;
std::string EXOCopyDenoisedCalib::fFileName = "";

EXOCopyDenoisedCalib::EXOCopyDenoisedCalib() : EXOCalibBase()
{
}

const char* EXOCopyDenoisedCalib::GetFileName(std::string flavor, const EXOEventHeader& header)
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return GetFileName(flavor,seconds, 1000*microseconds);
}

const char* EXOCopyDenoisedCalib::GetFileName(std::string flavor, long int seconds, int nano)
{
  return GetFileName(flavor, EXOTimestamp(seconds,nano));
}

const char* EXOCopyDenoisedCalib::GetFileName(std::string flavor, const EXOTimestamp& time)
{
  std::string fileName = "";
  if(fUseDB)
  {
    EXOCopyDenoisedCalib* cpDen = GetCalibrationFor(EXOCopyDenoisedCalib, EXOCopyDenoisedCalibHandler, flavor, time);
    fileName = cpDen->fFileName;
  }
  else
  {
    fileName = fFileName;
  }
  if(fileName != "")
    return fileName.c_str();
  LogEXOMsg("Could not find file with denoised energy values to be copied. Will not set denoised energy values.",EEError);
  
  return 0;
}

void EXOCopyDenoisedCalib::SetUserValue(std::string userValue)
{
  fFileName = userValue;
}

void EXOCopyDenoisedCalib::UseDatabase(bool useDB)
{
  fUseDB = useDB;
}

std::string EXOCopyDenoisedCalib::__str__() const
{
  std::stringstream sstream;
  sstream << "-------------------------EXOCopyDenoisedCalib-------------------------" << std::endl;
  sstream << "Copy denoised energy has the following values:" << std::endl;
  sstream << "EXOCopyDenoisedCalib is set to use " << (fUseDB ? "database " : "user provided ") << "values" << std::endl;
  sstream << "file location" << " = " << fFileName << std::endl;
  
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOCopyDenoisedCalibHandler)

EXOCalibBase* EXOCopyDenoisedCalibHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOCopyDenoisedCalib* newData = new EXOCopyDenoisedCalib();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOCopyDenoisedCalibHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOCopyDenoisedCalib* newData = new EXOCopyDenoisedCalib();

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
  selectColumns.push_back("File_loc");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() > 1){
    LogEXOMsg("Number of rows for this entry > 1",EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  res.getRow(fields, 0);
  newData->fFileName = fields[0].c_str();
  
  return newData;  
}
