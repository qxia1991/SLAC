//______________________________________________________________________________
//
// EXODataSet 
//
//   Class to encapsulate information of a list of runs that make up a data
//   set.  See EXORunInfoManager to see how one can populate this list. 
//______________________________________________________________________________
#include "EXOUtilities/EXODataSet.hh"
#include "EXOUtilities/EXORunInfoManager.hh"
#include <iostream>
using namespace std;

//______________________________________________________________________________
const EXORunInfo& EXODataSet::GetRunAt(size_t i) const
{
  // Return the run number at i, returns the value from the EXORunInfoManager
  return EXORunInfoManager::GetRunInfo(fRunList[i], fDirectory); 
}

//______________________________________________________________________________
void EXODataSet::Print(Option_t* /*opt*/) const
{
  // Print out contents of data set

  cout << "EXODataSet" << endl
       << "  Filter: " << fFilterString << endl; 
  if (fRunList.size() == 0) {
    cout << "  Empty" << endl;
    return;
  }
  cout << "  [" << fRunList[0];
  for (size_t i=1;i<fRunList.size();i++) cout << ", " << fRunList[i];
  cout << "]" << endl;
}

//______________________________________________________________________________
EXODataSet::DSiterator EXODataSet::begin()
{
  // beginning iterator
  return DSiterator(this); 
}
//______________________________________________________________________________
EXODataSet::DSiterator EXODataSet::end()
{
  // End iterator
  DSiterator tmp(this); 
  tmp._state = size();
  return tmp;
}

