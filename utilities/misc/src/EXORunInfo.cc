//______________________________________________________________________________
//
// EXORunInfo corresponds to 3 classes: 
//
//   EXORunInfo : encapsulates all info about a run
//   EXORunFile : encapsulates all info about a file
//   EXOMetadata : encapsulates metadata 
//
// EXORunInfo objects contain different types of runs.
//
// These objects can be obtained by calling
// EXORunInfoManager::GetRunInfo(runNo, "Data/Processed/masked") with runNo a
// particular run number.
//
// For example, to get all the run file from masked runs, you would do the
// following:
//
//   EXORunInfo ri; 
//   ...
//   EXORunInfo::RunList alist = ri.GetRunFiles();
//   // this list can be looped over
//
// To get processed (Raw ROOT) files, one would use the string
// "Data/Processed/recon" ("Data/Raw/root") instead.
//
// Metadata is saved per file, so when one can query for different data.  This
// data can be simply printed out, for example: 
//
// EXORunInfoManager::GetRunInfo(4000, "Data/Processed/masked").Print()
//
// yields: 
//
//  Run Number: 4000
//  Directory: Data/Processed/masked
//   Physical Loc: /nfs/slac/g/exo_data3/exo_data/data/WIPP/masked/4000/masked00004000-000.root
//    Key: nEventMax, Value: 13200
//    Key: nEventMin, Value: 1
//    Key: nGlitchEvents, Value: 0
//    Key: nRun, Value: 4000
//    Key: nVetoEvents, Value: 0
//    Key: rootTree, Value: tree
//    Key: rootVersion, Value: 52800
//    Key: runType, Value: Charge injection calibration-Internal
//  
//______________________________________________________________________________

#include "EXOUtilities/EXORunInfo.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <sstream>
#include <iostream>
#include "TString.h" 
using namespace std;

ClassImp(EXOMetadata)
ClassImp(EXOMetadataSet)
ClassImp(EXORunFile)
ClassImp(EXORunInfo)

//______________________________________________________________________________
template<typename T>
T EXOMetadata::GetVal() const
{
  T retVal;
  istringstream iss(fValue);
  iss >> retVal;  
  return retVal;
}

//______________________________________________________________________________
int EXOMetadata::AsInt() const
{
  // Return value as integer
  return GetVal<int>();
}

//______________________________________________________________________________
double EXOMetadata::AsDouble() const
{
  // Return value as double
  return GetVal<double>();
}

//______________________________________________________________________________
void EXOMetadata::Print(Option_t*) const
{
  cout << "  Key: " << fKey << ", Value: " << fValue << endl;
}

//______________________________________________________________________________
const EXOMetadata* EXOMetadataSet::FindMetaData(const std::string& dataName) const
{
  // Find a metadata object, will return NULL if it can't be found.
  MDMap::const_iterator iter = fAllMetaData.find(dataName);
  if (iter == fAllMetaData.end()) return NULL;
  return &iter->second;
}

//______________________________________________________________________________
void EXOMetadataSet::Print(Option_t*) const
{
  // Print the metadata set object
  MDMap::const_iterator iter = fAllMetaData.begin();
  for (;iter != fAllMetaData.end(); iter++) {
    iter->second.Print();
  }
}

//______________________________________________________________________________
void EXODataRunInfo::Print(Option_t*) const
{
  cout << "Data Run Information for run: " << fRunNumber << endl;
  fAllMD.Print();
}

//______________________________________________________________________________
void EXORunFile::Print(Option_t*) const
{
  // Print the run file object
  cout << " Physical Loc: " << fPhysicalFileLocation << endl;
  fAllMD.Print();
}

//______________________________________________________________________________
void EXORunInfo::Print(Option_t*) const
{
  // Print the run info object
  cout << "Run Number: " << fRunNumber << endl;
  
  cout << "Directory: " << fDirName << endl;
  RunList::const_iterator li = fRunMapFiles.begin();
  for(;li!=fRunMapFiles.end(); li++) li->Print();
  
}

//______________________________________________________________________________
bool EXORunInfo::RLComp::operator()(const EXORunFile& left, const EXORunFile& right) const
{
  // Compare the run infos by using the following keys.

  static std::vector<std::string> compareKeys;
  if (compareKeys.size() == 0) {
    compareKeys.push_back("nSegment");
    compareKeys.push_back("nEventMin");
  }

  const EXOMetadata *left_md, *right_md; 
  for (size_t i=0;i<compareKeys.size();i++) {
    if ((left_md = left.FindMetaData(compareKeys[i])) and
        (right_md = right.FindMetaData(compareKeys[i]))) { 
      return left_md->AsInt() < right_md->AsInt();
    }
  }
  // Last resort
  return left.GetFileLocation() < right.GetFileLocation();
}


