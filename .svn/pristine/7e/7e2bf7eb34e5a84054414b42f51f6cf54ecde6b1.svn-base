//______________________________________________________________________________
//
// EXOProcessingInfo
//
// Class that holds information about the processing of a run.  This can be
// stored in a TTree, or in a TTree::GetUserInfo().  In particular, it stores
// the following information: 
//
//   1. Processing Time (Date)
//   2. Modules used in processing 
//   3. Commands used in processing 
//   4. Build ID of EXOAnalysis
//   5. SVN revision of EXOAnalysis
//   6. Output from the error logger (up to the time the file was written).
//   7. Whether calibration data used in this processing came from the database
// 
// This is stored by EXOTreeOutputModule in the TTree::GetUserInfo() list.
// This means that it accessible by doing: 
// 
//    EXOProcessingInfo* proc_info = (EXOProcessingInfo*)tree->GetUserInfo()->First();
// 
// Processing information will be chained so that previous processing
// information will also be saved, see GetPrevProcInfo(); 
//
// M. Marino Apr 2011
#include "EXOUtilities/EXOProcessingInfo.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
ClassImp( EXOProcessingInfo )
//______________________________________________________________________________
void EXOProcessingInfo::Print(Option_t * opt) const
{
  // Dump out information to the screen.  This will also print out information
  // from previous processing.
  if (fPrevProc) fPrevProc->Print(opt);
  cout << "********************************************************" << endl;
  cout << "*****************EXOProcessingInfo Dump*****************" << endl;
  cout << "Build ID: " << GetBuildID() << endl; 
  cout << "SVN Revision: " << GetSVNRev() << endl; 
  cout << "Start Date of Processing: " << GetStartTimeOfProcess().AsString(opt) << endl; 
  if(fCalibrationsFromDatabase) cout << "All calibration data came from the database." << endl;
  else cout << "At least one calibration handler used default values." << endl;
  cout << endl;
  cout << "Modules used:" << endl;
  for (size_t i=0;i<GetModulesUsed().size();i++) {
    cout << "  " << GetModulesUsed()[i] << endl;
  }
  cout << endl;
  cout << "Commands called:" << endl;
  for (size_t i=0;i<GetCommandsCalled().size();i++) {
    cout << "  " << GetCommandsCalled()[i] << endl;
  }
  cout << fErrorLoggerMsg; // endl already included
  cout << "********************************************************" << endl;
}

//______________________________________________________________________________
void EXOProcessingInfo::SetPrevProcInfo( const EXOProcessingInfo& prev ) 
{
  // Set processing information from a previous run.  This copies the data.
  delete fPrevProc;
  fPrevProc = new EXOProcessingInfo(prev);
}

//______________________________________________________________________________
EXOProcessingInfo::EXOProcessingInfo() : TObject(), 
  fBuildID(BUILD_ID),
  fSVNRevision(SVN_REV),
  fCalibrationsFromDatabase(true), 
  fPrevProc(NULL) 
{
  // Default constructor
} 

//______________________________________________________________________________
EXOProcessingInfo::EXOProcessingInfo(const EXOProcessingInfo& other) : 
  TObject(other), fPrevProc(0)
{
  // Copy constructor
  fBuildID = other.fBuildID;
  fSVNRevision = other.fSVNRevision;
  fErrorLoggerMsg = other.fErrorLoggerMsg;
  fVectorOfModules = other.fVectorOfModules;
  fCommandsCalled = other.fCommandsCalled;
  fStartTimeOfProcess = other.fStartTimeOfProcess;
  fCalibrationsFromDatabase = other.fCalibrationsFromDatabase;
  if (other.fPrevProc) {
    fPrevProc = new EXOProcessingInfo(*other.fPrevProc);
  } 
}
//______________________________________________________________________________
EXOProcessingInfo& EXOProcessingInfo::operator=(const EXOProcessingInfo& other)
{
  // Copy operator 
  fBuildID = other.fBuildID;
  fSVNRevision = other.fSVNRevision;
  fErrorLoggerMsg = other.fErrorLoggerMsg;
  fVectorOfModules = other.fVectorOfModules;
  fCommandsCalled = other.fCommandsCalled;
  fStartTimeOfProcess = other.fStartTimeOfProcess;
  fCalibrationsFromDatabase = other.fCalibrationsFromDatabase;
  delete fPrevProc;
  if (other.fPrevProc) {
    fPrevProc = new EXOProcessingInfo(*other.fPrevProc);
  } else fPrevProc = NULL; 
  return *this;
}
//______________________________________________________________________________
void EXOProcessingInfo::SetStartTimeOfProcess()
{
  fStartTimeOfProcess = EXOMiscUtil::GetStartTimeOfProcess();
} 

//______________________________________________________________________________
void EXOProcessingInfo::SetAllModules( const std::string& mods )
{
  // Overload to handle a white-space separated list of modules.
  std::istringstream os(mods);
  std::string temp;
  fVectorOfModules.clear();
  while (os >> temp) fVectorOfModules.push_back(temp);
}
//______________________________________________________________________________
void EXOProcessingInfo::SetAllCommands( const std::string& cmds )
{
  // Overload to handle a list separated by endlines.
  std::istringstream os(cmds);
  std::string temp;
  fCommandsCalled.clear();
  while (std::getline(os, temp)) fCommandsCalled.push_back(temp);
}
