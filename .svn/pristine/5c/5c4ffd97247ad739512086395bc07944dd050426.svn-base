#ifndef EXOProcessingInfo_hh
#define EXOProcessingInfo_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TTimeStamp
#include "TTimeStamp.h"
#endif
#include <string> 

class EXOProcessingInfo : public TObject {

  public:
    typedef std::vector<std::string> StrVec;
    EXOProcessingInfo();
    EXOProcessingInfo(const EXOProcessingInfo& other);
    EXOProcessingInfo& operator=(const EXOProcessingInfo& other);
    virtual ~EXOProcessingInfo() { delete fPrevProc; }

    const StrVec& GetModulesUsed()        const { return fVectorOfModules; }
    const StrVec& GetCommandsCalled()     const { return fCommandsCalled; }
    const std::string& GetBuildID()       const { return fBuildID; }
    const std::string& GetSVNRev()        const { return fSVNRevision; }
    const std::string& GetErrorLoggerMsg() const{ return fErrorLoggerMsg; }
    const TTimeStamp&  GetStartTimeOfProcess() const { return fStartTimeOfProcess; }
    const bool& GetCalibrationsFromDatabase() const { return fCalibrationsFromDatabase; }

    void AddModule( const std::string& amod ) { fVectorOfModules.push_back( amod ); } 
    void ClearModules() { fVectorOfModules.clear(); } 
    void AddCommand( const std::string& cmd ) { fCommandsCalled.push_back( cmd ); }
    void ClearCommands() { fCommandsCalled.clear(); } 
    
    void SetBuildID( const std::string& id ) { fBuildID = id; }
    void SetSVNRev( const std::string& rev ) { fSVNRevision = rev; }
    void SetAllModules( const StrVec& mods ) { fVectorOfModules = mods; }
    void SetAllCommands( const StrVec& cmds ) { fCommandsCalled = cmds; }
    void SetAllModules( const std::string& mods );
    void SetAllCommands( const std::string& cmds );
    void SetStartTimeOfProcess();
    void SetCalibrationsFromDatabase(const bool& input) { fCalibrationsFromDatabase = input; }
    void SetErrorLoggerMsg(const std::string& ErrorLoggerOutput) { fErrorLoggerMsg = ErrorLoggerOutput; }

    virtual void Print(Option_t * = "") const;
    void SetPrevProcInfo( const EXOProcessingInfo& prev );
    const EXOProcessingInfo* GetPrevProcInfo() const; 

  protected:
    std::string fBuildID;         // Identification string of the build
    std::string fSVNRevision;     // SVN revision of the build 
    std::string fErrorLoggerMsg;  // All output from the error logger (up to when the file was written)
    StrVec fVectorOfModules;      // List of modules used in order of usage
    StrVec fCommandsCalled;       // List of commands used
    TTimeStamp fStartTimeOfProcess;    // Start time of processing
    bool fCalibrationsFromDatabase;    // false if any calibrations used default values rather than database values
    EXOProcessingInfo* fPrevProc; // Previous processing information

  ClassDef( EXOProcessingInfo, 5 )
};

//---- inlines -----------------------------------------------------------------

inline const EXOProcessingInfo* EXOProcessingInfo::GetPrevProcInfo() const
{
  // Returns processing information from previous runs over the data.  This can
  // be chained so that the following will work to loop through the previous
  // processing information:
  //  
  //  const EXOProcessingInfo* proc_info = currentInfo;
  //  const EXOProcessingInfo* prev_info;
  //  while ( (prev_info = proc_info->GetPrevProcInfo() ) ) {
  //      // Do something with prev_info 
  //      proc_info = prev_info; 
  //  } 
  //
  // returns NULL when no previous info exists.
  return fPrevProc;
}

#endif /* EXOProcessingInfo_hh */
