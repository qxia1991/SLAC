//______________________________________________________________________________
// EXOCalibHandlerBase
//
// Class that handles filling an EXOCalibBase object with data from a DB or
// default location. 
// Reading the database involves creating two types of classes:
//   - 'Data' class deriving from EXOCalibBase
//   - 'Handler' class deriving from EXOCalibHandlerBase.  This class handles
//   reading from the source material (e.g. database, xml files, etc.) and
//   filling information in the above 'Data' Class.
//
// An example would be:
//
//    class EXOMyDBInfoHandler : public EXOCalibHandlerBase {
//      ...
//      // Notice they are protected.
//      protected:
//      // The following is the function that you must write for this class if you would like to read from the DB
//      EXOCalibBase* readDB(const std::string& dataIdent,
//                         const std::string& formatVersion);
//    
//      // The following is a function you must write for this class to handle default reads (e.g. without DB)
//      EXOCalibBase* readDefault(const std::string& dataIdent,
//                         const std::string& formatVersion);
//    
//      ...
//      // The following is also essential
//      DEFINE_EXO_CALIB_HANDLER("mydbhandlertype")
//    }; 
//
//  There are a few examples of this code, including EXOElectronicsShapers,
//  EXOChannelMapCalib.  In the implementation file (.cc file), you must add
//  the following line:
//  
//    IMPLEMENT_EXO_CALIB_HANDLER(EXOMyDBInfoHandler)
//  
//______________________________________________________________________________
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#ifdef HAVE_MYSQL
#include "EXODBUtilities/EXOMysqlReadConnection.hh"
#include "EXODBUtilities/EXOMysqlResults.hh"
#include "EXODBUtilities/EXORdbException.hh"
#endif
// for plain text driver
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <EXOUtilities/EXOMiscUtil.hh>
#include "TString.h"
//
#include <iostream> //FIXME
using std::cout;
using std::endl;

//______________________________________________________________________________
EXOCalibBase* EXOCalibHandlerBase::read(const std::string& type, 
                                        const std::string& dataIdent,
                                        const std::string& formatVersion,
                                        EXOCalib::METADATASOURCE msrc)
{
  // This private function is called by EXOCalibManager.  It routes the calls
  // to the appropriate virtual derived class function.  This wrapping allows
  // derived classes to not have to perform this checking multiple times.

  if ( type != GetHandlerType() ) {
    LogEXOMsg("requested type (" + type + ") != handler name (" + GetHandlerType() + ")", EEAlert);
    return NULL;
  }
  EXOCalibBase* ret;
  switch (msrc) {
    case EXOCalib::METADATASOURCEmysql: 
#ifndef HAVE_MYSQL
      LogEXOMsg("Database access is not enabled in this build", EEError);
      return NULL;
#endif
      ret = readDB(dataIdent, formatVersion);
      if (ret) ret->m_SourceOfData = msrc;
      return ret;
    case EXOCalib::METADATASOURCEtext: 
      ret = readDB(dataIdent, formatVersion);
      if (ret) ret->m_SourceOfData = msrc;
      return ret;
    case EXOCalib::METADATASOURCEdefault: 
      ret = readDefault(dataIdent, formatVersion);
      if (ret) ret->m_SourceOfData = msrc;
      return ret;
    default:
      LogEXOMsg("Data source not recognized", EEError);
      return NULL;
  }  
}

//______________________________________________________________________________
EXOCalibBase* EXOCalibHandlerBase::readDB(const std::string& /*dataIdent*/,
                                          const std::string& /*formatVersion*/)
{
  // Derived classes should overload this function to enable the functionality
  // to read a DB.  If this function is directly called, it is normally due to
  // a user incorrectly configuring EXOCalibManager.
  LogEXOMsg(GetHandlerType() + ": read database not implemented", EEError);
  return NULL;
}

//______________________________________________________________________________
EXOCalibBase* EXOCalibHandlerBase::readDefault(const std::string& /*dataIdent*/,
                                               const std::string& /*formatVersion*/)
{
  // Derived classes should overload this function to enable the functionality
  // to read a Default.  If this function is directly called, it is normally
  // due to a user incorrectly configuring EXOCalibManager.
  LogEXOMsg(GetHandlerType() + ": read default not implemented", EEError);
  return NULL;
}

//______________________________________________________________________________
EXOCalibHandlerBase::RowResults EXOCalibHandlerBase::GetDBRowsWith(
                              const std::string& table,
                              const StrVec& getCols, 
                              const StrVec& orderCols, 
                              const std::string& where)
{
  // protected convenience function to wrap all the database calling for
  // EXOCalibHandlerBase classes.  This abstracts the mysql interface out,
  // allowing (in principle) future extensions to different types of databases.
  EXOCalib::METADATASOURCE msrc = EXOCalibManager::GetCalibManager().GetMetadataAccessType();
  if (msrc==EXOCalib::METADATASOURCEtext) {
    std::string where_loc = where;
    std::transform(where_loc.begin(), where_loc.end(), where_loc.begin(), ::tolower);
    std::string::size_type i,j;
    std::string id_key, id_val;
    std::string whitespace(" \t\r\n");
    i = j = where_loc.find("where");
    if (i!=std::string::npos) {
      // Find the first word 
      i = where_loc.find_first_not_of(whitespace,i+5);
      // Find the end of the first word
      j = where_loc.find_first_of(whitespace + "=",i);

      // Now we have the id key
      id_key = where_loc.substr(i,j-i);

      // continue trying to find the following value, we need to search for an equals sign 
      while (j < where_loc.size() and where_loc[j] != '=') j++; 
      if (j < where_loc.size()) {
        // j should now point to the equals sign
	i = where_loc.find_first_not_of(whitespace,j+1);
        if (where_loc[i]=='\'' || where_loc[i]=='"') {
          // Handle the case where the id val is surrounded by a string
          // Look for the next delimiter
          j = where_loc.find(where_loc[i],i+1);
          i++;
        } else {
          j = where_loc.find_first_of(whitespace);
          if (j == std::string::npos) j = where_loc.size(); 
        }
	id_val = where_loc.substr(i,j-i);
      }
      if (id_key=="" || id_val=="") {
        LogEXOMsg("can't parse WHERE option: '"+where_loc+"'", EEAlert);
        id_key = "";
      }
    }

    std::string file = "";
    std::string textDBdir = EXOCalibManager::GetCalibManager().GetTextDBdir();
    if(textDBdir!="") {
      file = EXOMiscUtil::SearchForFile(textDBdir+ "/" + table + ".txt");
    }
    else{
      file = EXOMiscUtil::SearchForFile("data/db/"+table+".txt");
    }
    if(file == "") {
      // Try searching for it without folders.
      file = EXOMiscUtil::SearchForFile(table + ".txt");
    }
    if(file == "") {
      LogEXOMsg("Failed to find \"" + table + ".txt\" in default locations.", EEAlert);
      return RowResults();
    }
    std::ifstream fin(file.c_str());
    if (!fin.good()) {
      LogEXOMsg("can't open database file '"+file+"'", EEAlert);
      return RowResults();
    }
    std::string line;
    StrVec header, row, res;
    StrVec getColsLoc = getCols;
    for(size_t i=0;i<getColsLoc.size();i++) { 
      std::string& tem = getColsLoc[i];
      std::transform(tem.begin(), tem.end(), tem.begin(), ::tolower);
    }
    std::map<std::string,unsigned int> key;

    if (getline(fin,line)) { // header
      EXOMiscUtil::stringTokenize(line, "\t", header);
      for(size_t i=0;i<header.size();i++) {
        std::string tem = header[i];
        std::transform(tem.begin(), tem.end(), tem.begin(), ::tolower);
        key[tem] = i;
      }
    }
    
    // Make sure we got all columns
    for (size_t i=0;i<getColsLoc.size();i++) {
      if (key.find(getColsLoc[i]) == key.end()) {
          LogEXOMsg(Form("Column ('%s') in table ('%s') not found.", 
            getColsLoc[i].c_str(), table.c_str()), EEError);
          return RowResults();
      }
    }

    while (getline(fin,line)) {
      EXOMiscUtil::stringTokenize(line, "\t", row);
      if (id_key=="" || row.at(key[id_key])==id_val) {
        line.clear(); 
        for(size_t i=0;i<getColsLoc.size();i++) { 
          if (i) line+="\t"; 
          line+=row.at(key[getColsLoc[i]]); 
        }
        res.push_back(line);
      }
    }
    fin.close();
    // FIXME sort res
    return RowResults(res);
  }
#ifdef HAVE_MYSQL
  else if (msrc==EXOCalib::METADATASOURCEmysql) {
    EXOMysqlReadConnection* conn = EXOCalibManager::GetCalibManager().borrowConnection();
    if (conn == NULL) {
      // Shouldn't happen.  This connection was just used to get
      // metadata
      LogEXOMsg("db connection not available", EEAlert);
      return RowResults();
    }
    try {
      return RowResults(conn->select(table, getCols, orderCols, where));
    } catch (EXORdbException rexcept) {
      LogEXOMsg("MYSQL exception: " + rexcept.getMsg(), EEError);
      return RowResults();
    }
  }
#endif
  else {
    return RowResults();
  }
}
//______________________________________________________________________________
unsigned int EXOCalibHandlerBase::RowResults::getNRows() const
{
  // Get the number of rows found.

#ifdef HAVE_MYSQL
  if (fRes.get()) return fRes->getNRows();
#endif
  return fTable.size();
}

//______________________________________________________________________________
bool EXOCalibHandlerBase::RowResults::getRow(StrVec& fields, unsigned int i) const
{
  // Return the row number i in the vector of strings, fields.
#ifdef HAVE_MYSQL
  if (fRes.get()) return fRes->getRow(fields,i);
#endif
  if (i<fTable.size()) {
    EXOMiscUtil::stringTokenize(fTable[i], "\t", fields);
    return true;
  }
  return false;
}

//______________________________________________________________________________
EXOCalibHandlerBase::RowResults::RowResults() 
{}

//______________________________________________________________________________
EXOCalibHandlerBase::RowResults::RowResults(std::auto_ptr<EXOMysqlResults> res) : 
  fRes(res)
{} 
//______________________________________________________________________________
EXOCalibHandlerBase::RowResults::RowResults(const StrVec& res) : 
  fTable(res) 
{} 

//______________________________________________________________________________
EXOCalibHandlerBase::RowResults&
   EXOCalibHandlerBase::RowResults::operator=(const EXOCalibHandlerBase::RowResults& res) 
{
  // Copy operator, transfer ownership of the ptr
  fRes = const_cast<RowResults&>(res).fRes;
  fTable = res.fTable;
  return *this;
}
//______________________________________________________________________________

EXOCalibHandlerBase::RowResults::RowResults(const EXOCalibHandlerBase::RowResults& res) :
  fRes(0), fTable(res.fTable)
{
  // Copy constructor, transferring ownership
  fRes = const_cast<RowResults&>(res).fRes;
}
//______________________________________________________________________________

EXOCalibHandlerBase::RowResults::~RowResults() 
{
  // explicitly release 
  fRes.reset();
}

//______________________________________________________________________________
EXOVCalibHandlerBuilder::EXOVCalibHandlerBuilder( const std::string& name ) 
{ 
  EXOCalibManager::GetCalibManager().registerHandler(this, name); 
} 
//______________________________________________________________________________
EXOVCalibHandlerBuilder::~EXOVCalibHandlerBuilder() 
{ 
  EXOCalibManager::GetCalibManager().unRegisterHandler(this); 
} 
//______________________________________________________________________________
void EXOVCalibHandlerBuilder::DeleteCalibHandler( EXOCalibHandlerBase* toDelete )
{ 
  delete toDelete;  
}


