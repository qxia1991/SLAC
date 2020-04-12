#include "EXODBUtilities/EXOMysqlResults.hh"
#include "mysql.h"

EXOMysqlResults::EXOMysqlResults(MYSQL_RES* results) : m_myres(results) 
{
}

EXOMysqlResults::~EXOMysqlResults() 
{
  mysql_free_result(m_myres);
}

unsigned int EXOMysqlResults::getNRows() const 
{
  // Return number of rows in results
  return mysql_num_rows(m_myres);
}

bool EXOMysqlResults::getRow(std::vector<std::string>& fields, 
                             unsigned int i, bool clear) const 
{
  // Get array of field values for ith row of result set
  mysql_data_seek(m_myres, i);
  MYSQL_ROW myRow = mysql_fetch_row(m_myres);
  unsigned long const* lengths = mysql_fetch_lengths(m_myres);
  
  unsigned nFields = mysql_num_fields(m_myres);
  
  if (clear) fields.clear();
  
  for (unsigned int iField = 0; iField < nFields; iField++) {
    if (myRow[iField])
      fields.push_back(std::string(myRow[iField],
                                   lengths[iField]));
    else
      fields.push_back("");
  }
  
  return true;
}

bool EXOMysqlResults::getRowPtrs(std::vector<std::string*>& fields, 
                                 unsigned int i, bool clear) const 
{
  //  Get array of field values for ith row of result set.  If a field 
  //  value is NULL, return a zero ptr for that element of the array
  //  --> It is the responsibility of the caller to delete the strings
  //  containing the field values.  See service cleanFieldPtrs.

  mysql_data_seek(m_myres, i);
  MYSQL_ROW myRow = mysql_fetch_row(m_myres);
  unsigned long const* lengths = mysql_fetch_lengths(m_myres);
  
  unsigned nFields = mysql_num_fields(m_myres);
  
  
  if (clear) fields.clear();
  
  for (unsigned int iField = 0; iField < nFields; iField++) {
    if (myRow[iField])
      fields.push_back(new std::string(myRow[iField],
                                       lengths[iField]));
    else
      fields.push_back(0);
  }
  
  return true;
  
}
// May also want to do 
//  MYSQL_FIELD* fields = mysql_fetch_fields(m_myres);
// and make these available to client, especially for select * queries

