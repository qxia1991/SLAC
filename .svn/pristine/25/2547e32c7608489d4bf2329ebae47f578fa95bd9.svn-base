#ifndef EXOAnalysisManager_hh
#define EXOAnalysisManager_hh

#include "EXOUtilities/EXOPluginUtilities.hh"
#include "TStopwatch.h"
#include <vector>
#include <stdexcept>

class EXOAnalysisModule;
class EXOCalibManager;
class EXOEventData;
class EXOTalkToManager;

class EXOAnalysisManager  {

private :
    
  EXOTalkToManager*      talktoManager;

  class EXOModuleContainer {
    public:
      EXOModuleContainer() : module(NULL), 
                             moduleCallCount(0),
                             modulePassCount(0),
                             moduleFirstCall(true),
                             moduleExecutionTime(0.0),
                             isOwned(true)
                             {}
      EXOAnalysisModule* module;
      int                moduleCallCount;
      int                modulePassCount;
      bool               moduleFirstCall;
      double             moduleExecutionTime;
      bool               isOwned;
      std::string        name;
      std::string        nickname;
  };

  std::vector< EXOModuleContainer > orderedModuleList;

  int                 eventsProcessed;
  TStopwatch          eventTimer;
  TStopwatch          moduleTimer;

  int                 maxevents;
  int                 printModulo;
  bool                printMemory;

  const std::string   exoinputmodulename;

  bool                verbose;
  bool                very_first_event;
  
  bool                UseAndOwnModule( EXOAnalysisModule* module,
                                       const std::string& asName, 
                                       bool managerOwnsModule );

public :

  EXOAnalysisManager( EXOTalkToManager *TALKTOMANAGER );
  
  ~EXOAnalysisManager();

  // Exception class to communicate processing errors.
  class FailedProcess : public std::runtime_error {
    public:
      FailedProcess(const std::string& what) : std::runtime_error(what) {}
  };

  /*! Run the Analysis. */ 
  void  RunAnalysis();
  void InitAnalysis();
  EXOEventData* StepAnalysis();

  /*! Returns true if we can instantiate a class 'className' with an alias 'alias'*/
  bool  CheckAvailabilityOfModule( const std::string& className,
                                   const std::string& alias );

  // Use a module that a user has instantiated
  bool  UseModule( EXOAnalysisModule* module,
                   const std::string& asName = "" );

  // Use a module from the factory.
  bool  UseModule( const std::string& nameOrNickname,
                   const std::string& asName = "");

  void  UseModuleWithAlias( std::string modWithAlias );
  void  UseListOfModules( std::string list_of_modules );
  void  RegisterPlugins( std::string library_name_list); 
  void  CheckModuleList();

  void                   ShowRegisteredModules();

  int Initialize();
  int BeginOfRun( EXOEventData* eventData );
  int BeginOfRunSegment( EXOEventData* eventData );
  int ProcessEvent( EXOEventData* eventData );
  int EndOfRunSegment( EXOEventData* eventData );
  int EndOfRun( EXOEventData* eventData );

  void ShutDown();
  void PrintMemory();
  void CollectStatistics();
  void print_statistics();

  void SetMaxEvents(int aval) { maxevents = aval; }
  void SetPrintModulo(int aval) { printModulo = (aval > 0 ? aval : 1); }
  void SetPrintMemory(bool aval) { printMemory = aval; }
  void SetVerbose(bool VALUE = true) { verbose = VALUE; }
  void SetInputFilename(std::string val);

  typedef std::vector<std::string> StrVec;
  const StrVec& GetListOfUsedModules(); 

  EXOAnalysisModule* GetModuleAt(size_t i) { return orderedModuleList[i].module; } 
  size_t GetNumberOfModules() const { return orderedModuleList.size(); }
};
#endif
