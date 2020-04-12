//______________________________________________________________________________
#include "EXOAnalysisManager/EXOAnalysisManager.hh"
#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOEventData.hh"
#include <string>
#include <iostream>
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "TProcessID.h"
#include "TSystem.h"
#include "TObjString.h"
using namespace std;


//______________________________________________________________________________
EXOAnalysisManager::EXOAnalysisManager( EXOTalkToManager *TALKTOMANAGER) :
  talktoManager(TALKTOMANAGER),
  eventsProcessed(0),
  exoinputmodulename("input"),
  verbose(false),
  very_first_event(true)
 
{

  if ( TALKTOMANAGER == 0 ) {
    LogEXOMsg("talk to manager doesn't exist", EEAlert); // quits
  }
  
  //Create commands for analysis manager
  talktoManager->CreateCommand("begin",
                               "begin processing", 
                               this,
                               &EXOAnalysisManager::RunAnalysis);

  talktoManager->CreateCommand("list",
                               "List all available modules", 
                               this,
                               &EXOAnalysisManager::ShowRegisteredModules);


  talktoManager->CreateCommand("use",
                               "select which modules to run", 
                               this,
                               "",
                               &EXOAnalysisManager::UseListOfModules);
  talktoManager->CreateCommand("usemoduleas",
                               "usemoduleas [modulename] [alias]", 
                               this,
                               "",
                               &EXOAnalysisManager::UseModuleWithAlias);

  talktoManager->CreateCommand("load", 
                               "Load plugin modules using full path or name",
                               this,
                               "",
                               &EXOAnalysisManager::RegisterPlugins);

  talktoManager->CreateCommand("printmodulo",
                         "print event number every ? events", 
                         this,
                         10,
                         &EXOAnalysisManager::SetPrintModulo);
  
  talktoManager->CreateCommand("printmemory",
           "print memory usage",
           this,
           false,
           &EXOAnalysisManager::SetPrintMemory);


  talktoManager->CreateCommand("maxevents",
           "process no more than n events",
           this,
           -1,
           &EXOAnalysisManager::SetMaxEvents);

  talktoManager->CreateCommand("verbose",
           "enable verbose output for analysis manager",
           this,
           false,
           &EXOAnalysisManager::SetVerbose);

  std::string temp;
  for (int intlevel =  (int)EEOk; 
           intlevel <= (int)EEPanic; intlevel++ ) {
    EXOErrorLevel level = (EXOErrorLevel)intlevel;
    if (temp.size()) temp += ", ";
    temp += EXOErrorLogger::ErrorLevel(level);
  }
  std::transform( temp.begin(), temp.end(), 
                  temp.begin(), ::tolower );
  EXOErrorLogger& err = EXOErrorLogger::GetLogger();
  std::string currentStatus = EXOErrorLogger::ErrorLevel(err.GetOutputThreshold());
  std::transform( currentStatus.begin(), currentStatus.end(), 
                  currentStatus.begin(), ::tolower );
  talktoManager->CreateCommand("set_output_threshold",
           "Below this value, output will be suppressed.  Possible values: " + temp,
           &err,
           currentStatus,
           &EXOErrorLogger::SetOutputThreshold);

  talktoManager->CreateCommand("output_function_name",
           "Print the pretty function name in EXO message output.",
           &err,
           false,
           &EXOErrorLogger::SetPrintFunction);



  talktoManager->CreateCommand("/input/file",
                               "name of input file", 
                               this, "", 
                               &EXOAnalysisManager::SetInputFilename);

  // Create commands for EXOCalibManager
  EXOCalibManager::GetCalibManager().TalkTo( talktoManager );

}

//______________________________________________________________________________
EXOAnalysisManager::~EXOAnalysisManager()
{
  for( size_t i=0; i<orderedModuleList.size(); i++ ) {
    /* Avoid null entries of the module when user requests 'input' */
    if ( !orderedModuleList[i].module ) continue;
    if ( !orderedModuleList[i].isOwned ) continue;
    talktoManager->RemoveObjectCommands( orderedModuleList[i].module );
    EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule( orderedModuleList[i].module ); 
  }
}

//______________________________________________________________________________
bool EXOAnalysisManager::CheckAvailabilityOfModule( const std::string& className,
                                                    const std::string& alias )
{
  /*! Returns true if we can instantiate a class 'className' with an alias 'alias'*/
    for ( size_t i=0; i < orderedModuleList.size(); i++ ) {
        EXOModuleContainer& container = orderedModuleList[i];
        if ( container.module                            && 
             container.name                 == className && 
             ( container.module->GetAlias() == alias     || 
               !container.module->CanHaveMultipleClassInstances() ) ) {
          /* This means that the module exists, and either the alias
             is the same, or it cannot have multiple class instances. */
          return false;
        }
    }
    return true;
}
//______________________________________________________________________________
void EXOAnalysisManager::UseModuleWithAlias(std::string modWithAlias )
{
  /*! The following function takes a string with both the module name and the 
      alias within separated by a white-space, e.g.: "modulename alias". */
  std::istringstream astream( modWithAlias );
  std::string tempName, tempAlias;
  if ( astream >> tempName >> tempAlias ) UseModule( tempName, tempAlias );
  else if (modWithAlias != "") throw EXOMiscUtil::EXOBadCommand("Unable to interpret command as a module name and alias.");
}

// Add, but don't own 'used' modules.
//______________________________________________________________________________
bool EXOAnalysisManager::UseModule( EXOAnalysisModule* mod,
                                    const std::string& asName )
{ 
  /*! Use a defined module with the name asName.  The analysis module
      will *not* delete this memory. If asName is not specified (default)
      no filter command will be made. */
  return UseAndOwnModule( mod, asName, false ); 
}

//______________________________________________________________________________
bool EXOAnalysisManager::UseAndOwnModule( EXOAnalysisModule* mod,
                                          const std::string& asName,
                                          bool iOwnThis )
{

  EXOModuleContainer moduleContainer;
  if(asName == exoinputmodulename) {
    if(not orderedModuleList.empty()) throw EXOMiscUtil::EXOBadCommand("Input module can only be the first module.");
    else if(mod != NULL) throw EXOMiscUtil::EXOBadCommand("Program error -- \"input\" module points to non-NULL address.");
    else moduleContainer.name = exoinputmodulename;
  }
  else /* It's a specific module */ {
    if(mod == NULL) throw EXOMiscUtil::EXOBadCommand("Received unspecified module other than \"input\"; that's not allowed.");
    else if(orderedModuleList.empty() and dynamic_cast<EXOInputModule*>(mod) == NULL) {
      throw EXOMiscUtil::EXOBadCommand("The first module should always be an input module.");
    }
    else moduleContainer.name = mod->GetName();
  }

  moduleContainer.nickname   = asName;
  moduleContainer.module     = mod;
  moduleContainer.isOwned    = iOwnThis;
  orderedModuleList.push_back( moduleContainer );

  talktoManager->SetCurrentBaseModule(mod);
  // Should we permit the talk-to command to throw?  That would need different handling.
  if(mod and mod->TalkTo(talktoManager) < 0) {
    talktoManager->RemoveObjectCommands(mod);
    talktoManager->SetCurrentBaseModule(NULL);
    orderedModuleList.pop_back();
    throw EXOMiscUtil::EXOBadCommand("Module " + mod->GetName() + " talk-to gave an error; it must be destroyed.");
  }
  // If the analysis manager has access to a name for the module, and it's not an input module, then make a filter command.
  if ( asName != "" and asName != exoinputmodulename and mod and dynamic_cast<EXOInputModule*>(mod) == NULL) { 
    std::string commandname;
    commandname =  "/" + asName + "/filter";
    talktoManager->CreateCommand(commandname,
                                 "turn filtering on or off for " + asName, 
                                 mod,
                                 false,
                                 &EXOAnalysisModule::SetFiltered);    
  }
  talktoManager->SetCurrentBaseModule(NULL);
  return true;
}

//______________________________________________________________________________
bool EXOAnalysisManager::UseModule( const std::string& nameOrNickname,
                                    const std::string& asName )
{
  /*! Try to use a module, trying to build a module from the class factory.
      This is called following the 'use' or 'usemoduleas' commands and can also 
      be called by users to build compiled in modules using the factory.  The nameOrNickname
      should be either the name of the class, or the nickname of the class.
      If the usemoduleas command is used, asName takes a string that will serve as the
      alias for the module.  */
  std::string theUsedName = nameOrNickname;
  if ( asName != "" ) theUsedName = asName;

  EXOAnalysisModule* mod = NULL;

  try {
    /* We have to handle the input specially. */
    if ( nameOrNickname == exoinputmodulename ) {
      return UseAndOwnModule( NULL, exoinputmodulename, true ); 
    }

    mod = EXOAnalysisModuleFactory::GetInstance().BuildAnalysisModule( nameOrNickname );
    if ( mod == NULL ) {
      throw EXOMiscUtil::EXOBadCommand("Module " + nameOrNickname + " doesn't exist.");
    }
    if ( !CheckAvailabilityOfModule( mod->GetName(), theUsedName ) ) {
      throw EXOMiscUtil::EXOBadCommand("Module \"" + mod->GetName() + "\" can't be used with alias \"" + theUsedName + "\".");
    }
    mod->SetAlias( theUsedName );
    return UseAndOwnModule( mod, theUsedName, true );
  }
  catch ( EXOMiscUtil::EXOBadCommand& badCommand ) {
    // clean-up and re-throw.  If we got to this point, mod was created in this function and needs to be deleted.
    if(mod) EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule( mod );
    throw;
  }
}

//______________________________________________________________________________
void EXOAnalysisManager::UseListOfModules( std::string list_of_modules )
{
  /* Takes a white-space separated list of module and  tries to load. */
  std::istringstream use_list( list_of_modules );
  std::string mod_to_load;
  size_t InitialSize = orderedModuleList.size();
  try {
    while( use_list >> mod_to_load ) UseModule( mod_to_load, "" );
  }
  catch(EXOMiscUtil::EXOBadCommand& aBadCommand) {
    // We need to clean up all of the modules that were created, then re-throw the exception.
    while(orderedModuleList.size() > InitialSize) {
      if(orderedModuleList.back().module and orderedModuleList.back().isOwned) {
        talktoManager->RemoveObjectCommands( orderedModuleList.back().module );
        EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule(orderedModuleList.back().module);
      }
      orderedModuleList.pop_back();
    }
    aBadCommand.append("All modules created by this command have been destroyed.");
    throw;
  }
}

//______________________________________________________________________________
void EXOAnalysisManager::RegisterPlugins( std::string library_name_list)
{
  /* Takes a white-space separated list of library_names_and tries to load. */
  std::istringstream lib_list(library_name_list);
  std::string lib_to_load;
  while( lib_list >> lib_to_load ) {
    /* The TalkTo Command handles the expansion of wildcards and env vars in the string */
    std::vector<std::string> vectorOfLibs = EXOMiscUtil::SearchForMultipleFiles( lib_to_load );
    for (size_t i = 0; i < vectorOfLibs.size(); i++ ) EXOAnalysisModuleFactory::GetInstance().RegisterOnePlugin( vectorOfLibs[i] );
  }
} 

//______________________________________________________________________________
void EXOAnalysisManager::ShowRegisteredModules()
{
  EXOAnalysisModuleFactory::GetInstance().ShowRegisteredModules();
}

//______________________________________________________________________________
void EXOAnalysisManager::CheckModuleList()
{
  // Check for any problems with orderedModuleList before using it.

  if ( orderedModuleList.size() == 0 ) LogEXOMsg("No modules requested to run", EEAlert);

  for(size_t i = 0; i < orderedModuleList.size(); i++) {
    if(i == 0) {
      EXOInputModule* inputModule = dynamic_cast<EXOInputModule*>(orderedModuleList[i].module);
      if(inputModule == NULL) LogEXOMsg("First module is not an input module (or was never correctly initialized)", EEAlert);
      if(inputModule->reads_from_file() and not inputModule->FileIsOpen()) {
        LogEXOMsg("Input module lacks an input file", EEAlert);
      }
    }
    else {
      if(orderedModuleList[i].module == NULL) LogEXOMsg("A module was never created successfully.", EEAlert);
      if(dynamic_cast<EXOInputModule*>(orderedModuleList[i].module)) {
        LogEXOMsg("Only the first module should be an input module", EEAlert);
      }
    }
  }
}

//______________________________________________________________________________
void EXOAnalysisManager::InitAnalysis()
{
  // Run the various commands which only get called once.

  // On the very first event, we should order the modules and initialize them, 
  // print memory info, clear the event data, ect.
 
  if ( very_first_event == true ) {

    // Check that there are no problems with the module list.
    CheckModuleList();
    
    if ( Initialize() < 0 ) throw FailedProcess("Initialize failed");

    // Print memory if requested
    if ( printMemory ) PrintMemory();
 
    eventsProcessed = 0;
    eventTimer.Start();
    very_first_event = false;
  } // end of very first event measures

  // Register some string objects for analysis modules
  const StrVec &mods = GetListOfUsedModules();
  ostringstream os;
  std::ostream_iterator<std::string> it(os, "\n"); 
 
  // Concatenate the vector of strings using the \n character as the delimiter.
  std::copy(mods.begin(), mods.end(), it);
  EXOAnalysisModule::RegisterObject("ModulesUsed", TObjString(os.str().c_str()));

  const StrVec &cmds = talktoManager->GetProcessedCommands();
  os.clear();
  std::copy(cmds.begin(), cmds.end(), it);
  EXOAnalysisModule::RegisterObject("CommandsCalled", TObjString(os.str().c_str()));
  // Done registering.
}

//______________________________________________________________________________
void EXOAnalysisManager::RunAnalysis()
{
  /* Run the Analysis. */ 

  try {
    InitAnalysis();
  }
  catch(FailedProcess& exc) {
    std::cerr << exc.what() << std::endl;
    return; // Mimic old behavior.
  }

  try {
    EXOEventData* eventData = NULL;
    while(maxevents < 0 or eventsProcessed < maxevents) {
      EXOEventData* eventData = StepAnalysis();
      if(eventData == NULL) break;
    }
    if(EndOfRunSegment(eventData) < 0) throw FailedProcess("Failed EndOfRunSegment");
    if(EndOfRun(eventData) < 0) throw FailedProcess("Failed EndOfRun");
    if(printMemory) PrintMemory();
  }
  catch(FailedProcess& exc) {
    // According to old code, in this situation we can only call shutdown.
    std::cerr << exc.what() << std::endl;
    ShutDown();
    return;
  }

  ShutDown();
}

//______________________________________________________________________________
EXOEventData* EXOAnalysisManager::StepAnalysis()
{
  // Run processing over at most one event.
  // If there are no more events to process, return NULL.
  // If we encounter an error, raise FailedProcess.
  moduleTimer.ResetRealTime();
  moduleTimer.Start();
  EXOInputModule* inputModule = dynamic_cast<EXOInputModule*>(orderedModuleList[0].module);
  EXOEventData* eventData = inputModule->GetNextEvent();
  moduleTimer.Stop();
  orderedModuleList[0].moduleExecutionTime += moduleTimer.RealTime();

  if(eventData == NULL) return NULL;

  // This is for TRefs, should this go elsewhere?  M. Marino
  static UInt_t savedObjectCount = TProcessID::GetObjectCount();
  TProcessID::SetObjectCount(savedObjectCount);

  if(verbose) cout<<"Just retrieved run "<<eventData->fRunNumber<<" event "<<eventData->fEventNumber<<" for processing."<<endl;

  // Get the run number from the input module, and notice if it has changed.
  static int stale_run_number = -1;
  int runNumber = inputModule->get_run_number();

  if(runNumber < 0) {
    LogEXOMsg("input module returns error for run number", EECritical);
    return NULL; // Rather than raising an exception, because we don't need to shortcircuit EndOfRun?
  }

  bool begin_run = (stale_run_number != runNumber);
  bool end_run = (begin_run and stale_run_number != -1);
  bool begin_segment = (begin_run or inputModule->is_new_run_segment());
  bool end_segment = (begin_segment and stale_run_number != -1);

  if(end_segment) {
    if(EndOfRunSegment(eventData) < 0) throw FailedProcess("EndOfRunSegment failed");
  }

  if(end_run) {
    if(EndOfRun(eventData) < 0) throw FailedProcess("EndOfRun failed");
  }

  if(begin_run) {
    if(BeginOfRun(eventData) < 0) throw FailedProcess("BeginOfRun failed");
  }

  if(begin_segment) {
    if(BeginOfRunSegment(eventData) < 0) throw FailedProcess("BeginOfRunSegment failed");
  }

  // Get the event number from the input module
  if(inputModule->get_event_number() < 0) {
    LogEXOMsg("input module get_event_number return error", EECritical);
    throw FailedProcess("Input module could not get event number");
  }

  // Call the ProcessEvent methods
  if(ProcessEvent(eventData) < 0) throw FailedProcess("ProcessEvent failed.");

  // See if we should run over another event.
  // Also keep track of the running time for the continue_analysis function
  CollectStatistics();

  if(verbose) cout<<"Just finished processing run "<<eventData->fRunNumber<<" event "<<eventData->fEventNumber<<"."<<endl;
  stale_run_number = runNumber;
  return eventData;
}

//______________________________________________________________________________
void EXOAnalysisManager::PrintMemory()
{
  char pscommand[100];
  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
  MemInfo_t memInfo;
  Int_t retMemInfo = gSystem->GetMemInfo(&memInfo);
  ProcInfo_t procInfo;
  Int_t retProcInfo = gSystem->GetProcInfo(&procInfo);
  if(retMemInfo or retProcInfo) {
    std::cout<<"Error getting memory usage information."<<std::endl;
    std::cout<<"gSystem->GetMemInfo returned "<<retMemInfo<<std::endl;
    std::cout<<"gSystem->GetProcInfo returned "<<retProcInfo<<std::endl;
    return;
  }
  std::cout<<"Percent memory used:\t"<<0.1*double(procInfo.fMemResident)/memInfo.fMemTotal<<std::endl;
  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

}

//______________________________________________________________________________
void EXOAnalysisManager::print_statistics()
{
  // Print out module execution statistics

  cout << endl;
  cout << "****************************************************************" << endl;
  cout << "Module execution statistics:\n";
  cout << "----------------------------------------------------------------" << endl;
  cout << "Module               # calls\t# passing    avg. time (sec)\n";
  cout << "----------------------------------------------------------------" << endl;
  
    //    cout << "module index " << i << " and module call count = " << moduleCallCount[i] << endl;
  for ( size_t i=0; i < orderedModuleList.size(); i++ ) {
    EXOModuleContainer& module_container = orderedModuleList[i]; 

    if ( module_container.moduleCallCount == 0 ) continue;

    // We don't include the first call to ProcessEvent action in the
    // module execution time average.

    double avgtime = 0.0;
    if ( module_container.moduleCallCount == 1 ) {
      avgtime = 0.0;
    }
    else {
      avgtime = module_container.moduleExecutionTime/(module_container.moduleCallCount-1);
    }
      
    cout << Form("%-20.20s %-11d%-11d%11.6f\n", module_container.name.c_str(), 
       module_container.moduleCallCount, module_container.modulePassCount, avgtime );
       
  }
  cout << "****************************************************************" << endl;
  cout << endl;

}

//______________________________________________________________________________
int EXOAnalysisManager::Initialize() 
{
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {
    int result = orderedModuleList[i].module->Initialize();
    if ( result < 0 ) {
      LogEXOMsg(Form("module %s returns error",
                orderedModuleList[i].name.c_str()), EECritical);
      return -1;
    }
  }
  return 0;
}


//______________________________________________________________________________
int EXOAnalysisManager::BeginOfRun( EXOEventData* eventData )
{
  // Call BeginOfRun for all registered modules. 
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {
    int result = orderedModuleList[i].module->BeginOfRun(eventData);
    if ( result < 0 ) {
      LogEXOMsg(Form("module %s returns error",
                orderedModuleList[i].name.c_str()), EECritical);
      return -1;
    }
  }
  return 0;
}
 
//______________________________________________________________________________
int EXOAnalysisManager::BeginOfRunSegment( EXOEventData* eventData )
{
  // Call BeginOfRunSegment for all registered modules.  
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {
    int result = orderedModuleList[i].module->BeginOfRunSegment(eventData);
    if ( result < 0 ) {
       LogEXOMsg(Form("module %s returns error",
                 orderedModuleList[i].name.c_str()), EECritical);
      ShutDown();
      return -1;
    }
  }
  return 0;
}

//______________________________________________________________________________
int EXOAnalysisManager::ProcessEvent( EXOEventData* eventData )
{
  // Call ProcessEvent for all registered modules.  Keep track of statistics.
  int passed_modules = 0;
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {

    // Keep track of module running statistics

    passed_modules++;
    EXOModuleContainer& module_container = orderedModuleList[i];

    module_container.moduleCallCount++;
    moduleTimer.ResetRealTime();
    moduleTimer.Start();

    // Call the begin of event methods

    int result = orderedModuleList[i].module->ProcessEvent(eventData);

    // More statistics accounting. Don't add the execution time
    // for the first call to ProcessEvent, because sometimes 
    // modules do initialization on the first call.

    moduleTimer.Stop();
    if ( module_container.moduleFirstCall == true ) {
        module_container.moduleFirstCall = false;
    }
    else {
        module_container.moduleExecutionTime += moduleTimer.RealTime();
    }

    if ( result < 0 ) {
      LogEXOMsg(Form("module %s returns error",
                module_container.name.c_str()), EECritical);
      return -1;
    }
    if ( result == 0 ) module_container.modulePassCount++;
    if ( module_container.module->IsFiltered() && result != 0 ) break;
  }
  return passed_modules;
}
//______________________________________________________________________________
int EXOAnalysisManager::EndOfRunSegment( EXOEventData* eventData )
{
  // Call EndOfRunSegment for all registered modules.
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {

    int result = orderedModuleList[i].module->EndOfRunSegment(eventData);
    if ( result < 0 ) {
      LogEXOMsg(Form("module %s returns error",
                orderedModuleList[i].name.c_str()), EECritical);
      return -1;
    }

  }
  return 0;
}
//______________________________________________________________________________
int EXOAnalysisManager::EndOfRun( EXOEventData* eventData ) 
{
  // Call EndOfRun for all registered modules.
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {
    int result = orderedModuleList[i].module->EndOfRun(eventData);
    if ( result < 0 ) {
      LogEXOMsg("module " + orderedModuleList[i].name + " returns error", EECritical);
      return -1;
    }
  }
  return 0;
}

//______________________________________________________________________________
void EXOAnalysisManager::ShutDown()
{
  // Call ShutDown for all registered modules.
  for ( size_t i = 0; i < orderedModuleList.size(); i++ ) {
    int result = orderedModuleList[i].module->ShutDown();
    if ( result < 0 ) {
      LogEXOMsg("module " + orderedModuleList[i].name + " returns error", EECritical);
    }
  }
  very_first_event = true;
}

//______________________________________________________________________________
void EXOAnalysisManager::CollectStatistics()
{
  // Collect statistics, incremementing how many events have been processed and
  // outputing if greater than the print modulo number.
    eventsProcessed++;
    if ( (eventsProcessed % printModulo) == 0 ) {
      eventTimer.Stop();
      cout << eventsProcessed << " events processed (" 
       << printModulo/eventTimer.RealTime() << "Hz)" << endl;
      if(printMemory) PrintMemory();        
      eventTimer.ResetRealTime();
      eventTimer.Start();
    }
}

//______________________________________________________________________________
const EXOAnalysisManager::StrVec& EXOAnalysisManager::GetListOfUsedModules()
{
  // Returns an ordered  list of the used modules in this processing.
  static std::vector<std::string> vectorOfModuleNames;
  vectorOfModuleNames.clear();
  for(size_t i=0;i<orderedModuleList.size();i++) {
    vectorOfModuleNames.push_back( orderedModuleList[i].nickname );
  }
  return vectorOfModuleNames;
}

//______________________________________________________________________________
void EXOAnalysisManager::SetInputFilename(std::string val)
{
  // If you are using xrootd, be sure to include the full prefix "root://".
  // No-op if you use a blank filename -- regrettably, no error is currently thrown.

  if(val == "") return; // This default value gets called once -- to fix, but not critical (and no worse than before).

  if(orderedModuleList.empty()) throw EXOMiscUtil::EXOBadCommand("You have not added any modules to the list yet.");

  if((orderedModuleList[0].name == exoinputmodulename) xor (orderedModuleList[0].module == NULL)) {
    throw EXOMiscUtil::EXOBadCommand("First module is set up incorrectly for the \"input\" module.");
  }

  // Find the file.
  if(val.find("root://") != 0) { // Can't check through xrootd (yet?) -- it needs to be a full path.
    std::string tempFilename = EXOMiscUtil::SearchForFile(val);
    if(tempFilename == "") {
      throw EXOMiscUtil::EXOBadCommand("Failed to locate file " + val); // nothing to clean-up, ok to be outside try block.
    }
    val = tempFilename;
  }

  try {

    if(orderedModuleList[0].name == exoinputmodulename) {
      // Find the correct module, based on the filename
      orderedModuleList[0].module = EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile(val);
      if ( orderedModuleList[0].module == NULL ) {
        throw EXOMiscUtil::EXOBadCommand("file type not recognized by input modules");
      }
      talktoManager->SetCurrentBaseModule(orderedModuleList[0].module);
      if(orderedModuleList[0].module->TalkTo(talktoManager) < 0) {
        throw EXOMiscUtil::EXOBadCommand("Error returned from talk-tos for module that could read this file; the file was not opened.");
      }
      talktoManager->SetCurrentBaseModule(NULL);
    }
    else {
      // OK, instead we were given a specific module.
      EXOInputModule* TempInputMod = dynamic_cast<EXOInputModule*>(orderedModuleList[0].module); // run-time check
      if(TempInputMod == NULL or not TempInputMod->reads_from_file()) {
        throw EXOMiscUtil::EXOBadCommand("Your first module is not a module that expects input");
      }

      // Open the file.
      TempInputMod->SetFilename(val);
    }
  } // End try

  catch ( EXOMiscUtil::EXOBadCommand& badCommand ) {
    // If the "input" module was used, we have to clean it up before leaving.
    // If it was a specific input module, then it should have cleaned itself up -- nothing to do here.
    if(orderedModuleList[0].name == exoinputmodulename) {
      talktoManager->SetCurrentBaseModule(NULL);
      if(orderedModuleList[0].module) {
        talktoManager->RemoveObjectCommands(orderedModuleList[0].module);
        EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule(orderedModuleList[0].module);
        orderedModuleList[0].module = NULL;
        badCommand.append("Destroying the input module that was created.");
      }
    }
    throw;
  }
}
