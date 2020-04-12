//______________________________________________________________________________
//
// EXOGeant4Module
//
// Provides a plugin interface for the EXOSim Geant4 system.
//
//______________________________________________________________________________

#include "EXOGeant4Module.hh"
#include "EXOSim/EXOEventAction.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#ifdef G4_HAS_VIS 
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#endif
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#include "G4VModularPhysicsList.hh"
#include "G4Version.hh"

#include "Randomize.hh"

#include "EXOSim/EXODetectorConstruction.hh"
#include "EXOSim/EXOFullDetectorConstruction.hh"
#include "EXOSim/EXOPhysicsList.hh"
#include "EXOSim/EXOPrimaryGeneratorAction.hh"
#include "EXOSim/EXORunAction.hh"
#include "EXOSim/EXOEventAction.hh"
#include "EXOSim/EXOSteppingAction.hh"
#include "EXOSim/EXOFullSteppingAction.hh"
#include "EXOSim/EXOSteppingVerbose.hh"
#include "EXOSim/EXOStackingAction.hh"
#include "EXOSim/EXOTrackingAction.hh"
#include "EXOSim/EXOTrackingKiller.hh"
#include "EXOSim/EXOIsotopeTable.hh"
#include "G4IonTable.hh"
#include "G4VIsotopeTable.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOSim/EXOParallelDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
using namespace std;

//______________________________________________________________________________
EXOGeant4Module::EXOGeant4Module() : 
  fRunNumber(1),
  fEventNumber(0),
  fG4EventNumber(-1),
  fG4SubEventNumber(-1),
  fRndInitialSeed(2),
  fRndReadFile(),
  fRndSaveFile(),
  fSkipInitialize(false),
  fPhysicsListName("EXO"),
  fSelectedGeometry(kEXO200),
  fXenonComposition(0),
  fXenonPressure(0.),
  fXenonDensity(0.),
  fSkipEmptyEvents(false),
  fSkipEmptyThreshold(1.0),
  fSourcePosition("0 0 0 0"),
  fVerboseDetConstruction(false),
  fCyclesBeforeWarning(kDefaultLoopWarning),
  fLifetimeLimit(0),
  fG4PrintModulo(0),
  fUseImportanceSampling(false),
  fISVolumeFactor(2.0),
  g4runManager(NULL),
  UI(NULL),
  fVisManager(NULL),
  exosimEventAction(NULL),
  fTrackingAction(0),
  fMacro(NULL),
  fMacroIstreamIsOwned(false),
  fRunToEvents(0)
{
}

EXOGeant4Module::~EXOGeant4Module()
{
  if(fMacroIstreamIsOwned) delete fMacro;
  delete g4runManager;
#ifdef G4VIS_USE
  delete fVisManager;
#endif
}

//______________________________________________________________________________
void EXOGeant4Module::ReadAndApplyMacro()
{
  // Function reads the macro stream and applies found commands to the Geant4 UI
  // until either "/run/beamOn" or the end of macro stream is found.
  // Also see CheckMacroFile() below.

  if (!fMacro) return;

  // Enable echo of G4 commands
  // Usually we are here after Initialize() or after processing all 
  // '/run/beamOn ?' events looking for more commands.
  UI->ApplyCommand("/control/verbose 1");

  string line;
  while(getline(*fMacro,line)) {
    if (line[0]=='#') continue; // skip comments
    if (line.length()<2) continue;
    if (line.compare(0,5,"exit")==0) {
      G4cout << line << endl; // print even this
      fMacro->setstate(std::ios_base::eofbit);
      break;
    } else if (line.compare(0,12,"/run/beamOn ")==0) {
      G4cout << line << endl; // print even this
      fRunToEvents += strtol(line.c_str()+12,0,0);
      // Disable print tons of the following '/run/beamOn 1' commands
      UI->ApplyCommand("/control/verbose 0");
      break;
    } else {
      G4int ret = UI->ApplyCommand(line.c_str());
      // ret = xyy, x00 from G4CommandStatus.hh, yy is first problematic parameter
      // from G4UIterminal::ExecuteCommand()
      G4int c_index = ret % 100;
      G4int c_status = ret - c_index;
      //G4UIcommand *cmd = UIterminal->G4VBasicShell::FindCommand(line);
      string cmd = line.substr(0,line.find(' ')); // cmd->Command();

      switch(c_status) {
        case fCommandSucceeded:
          break;
        case fCommandNotFound:
          LogEXOMsg(Form("Command <%s> not found", cmd.c_str()), EEError);
          break;
        case fIllegalApplicationState:
          LogEXOMsg("Illegal application state -- command refused", EEError);
          break;
        case fParameterOutOfRange:
          LogEXOMsg(Form("Parameter #%d is out of range",c_index), EEError);
          //G4cerr << "Allowed range : " << cmd->GetRange() << G4endl;
          break;
        case fParameterOutOfCandidates:
          LogEXOMsg(Form("Parameter #%d is out of candidate list",c_index), EEError);
          //G4cerr << "Candidates : " << cmd->GetParameter(paramIndex)->GetParameterCandidates() << G4endl;
          break;
        case fParameterUnreadable:
          LogEXOMsg(Form("Parameter #%d is wrong type and/or is not omittable",c_index), EEError);
          break;
        case fAliasNotFound:
        default:
        LogEXOMsg(Form("Command refused, status=%d",status), EEError);
      }
    }
  }

}

//______________________________________________________________________________
bool EXOGeant4Module::CheckMacroFile()
{
  // CheckMacroFile does main job in intelligent processing of macro file.
  // It returns true if there are still events to process, false otherwise.
  // In case there is no "/run/beamOn" command in macro it applies all lines
  // and allow generation of events forever.
  // Otherwise after "/run/beamOn <number>" line execution of macro is 
  // paused until <number> of Geant4 events is processed. fRunToEvents 
  // is used to control this. Then it continues with macro. After all
  // "/run/beamOn" events and end of macro file processing will stop.

  if (!fMacro) return false;
  if (fRunToEvents==0 || fG4EventNumber!=fRunToEvents) return true;
  ReadAndApplyMacro();
  if (!fMacro->good() && fG4EventNumber==fRunToEvents) {
    cout << "done running macro file" << endl;
    return false; // nothing to do
  }
  return true;
}

//______________________________________________________________________________
void EXOGeant4Module::SetMacroFile(std::string aval) 
{
  // Function takes a string and sets the current macro stream to be the
  // ifstream to this file. 

  if (aval.empty()) return;
  G4String filename = EXOMiscUtil::SearchForFile(aval);
  ifstream *fin = new ifstream(filename);
  if (fin->good())
    SetMacroStream(fin, true);
  else
      LogEXOMsg(Form("can't open macro file '%s'",aval.c_str()), EECritical);    
}

//______________________________________________________________________________
void EXOGeant4Module::SetMacroStream(std::istream* astream, bool g4ModNowOwnsThis)
{
  // Sets the macro to the new stream astream.  EXOGeant4Module takes ownership
  // of this object (and will delete it) if g4ModNowOwnsThis is set to true.

  if (!astream || !astream->good()) return;
  if (fMacroIstreamIsOwned) delete fMacro;
  fMacro = astream;
  fMacroIstreamIsOwned = g4ModNowOwnsThis;
}
//______________________________________________________________________________
void EXOGeant4Module::ResetRandomNumberGenerator()
{
  // Resets the seed of the random number generator. 

  // Makes sure the random number generator is set.
  SetRandomNumberGenerator();
  if (!fRndReadFile.empty()) {
    CLHEP::HepRandom::restoreEngineStatus(fRndReadFile.c_str());
    CLHEP::HepRandom::showEngineStatus();
  } else {
//  if (CLHEP::HepRandom::getTheSeed() != seed ) {
    CLHEP::HepRandom::setTheSeed(fRndInitialSeed);
  }
 
}
//______________________________________________________________________________
void EXOGeant4Module::SetRandomNumberGenerator()
{
  // Sets the random number generator to use the Mersenne Twister.  Called by
  // ResetSeed().
  static CLHEP::HepRandomEngine* theEngine = 0;
  if ( !theEngine ) {
    theEngine = new CLHEP::MTwistEngine();
    CLHEP::HepRandom::setTheEngine( theEngine );
  } else if ( theEngine != CLHEP::HepRandom::getTheEngine() ) {
    cout << "Another engine has been set by CLHEP instead of "
            "the MTwistEngine originally set." << endl;
    CLHEP::HepRandom::setTheEngine( theEngine );
  }

}
//______________________________________________________________________________
int EXOGeant4Module::Initialize()
{

  // Make sure we've got an error logger, analysis manager, and event data object
  static bool firstInitCall = true;

  // Here we begin the stuff that used to appear in the main() function
  // of the EXOsim executable. Only do this on the first call.

  if ( firstInitCall == true ) {

    firstInitCall = false;

    SetRandomNumberGenerator();

    //my Verbose output class
    G4VSteppingVerbose::SetInstance(new EXOSteppingVerbose);
    
    // Construct the default run manager
    g4runManager = new G4RunManager;
    

    // set mandatory initialization classes
    G4VUserDetectorConstruction* detector = NULL;
    
    // more mandatory initialization classes
    EXOPrimaryGeneratorAction * genaction = NULL; 
    EXOStackingAction*        stackaction = NULL;
    EXORunAction*             runaction   = NULL;
    EXOSteppingAction*         stepaction = NULL;
    exosimEventAction                     = NULL;
    fTrackingAction                       = NULL;

    switch (fSelectedGeometry) {
      case kEXO200: {
        EXODetectorConstruction *det = new EXODetectorConstruction();
        det->SetConstructionVerbose(fVerboseDetConstruction);
        det->SetSourcePosition(fSourcePosition);
        det->SetXenonComposition((EXODetectorConstruction::xenonComposition_t)fXenonComposition);
        if (fXenonPressure>0.) det->SetXenonPressure(fXenonPressure*bar);
        if (fXenonDensity>0.) det->SetXenonDensity(fXenonDensity*g/cm3);
        det->DefineMaterials();
        detector = det;
        break; }
      case kFullEXO:
        detector    = new EXOFullDetectorConstruction;
        break;
      default: break;
    }
    g4runManager->SetUserInitialization(detector);

    EXOPhysicsList* exoPhysicsList = new EXOPhysicsList;
    // also see UI commands below

    if (fPhysicsListName=="EXO") {
      // our own physics list
      // I hope one day it'll make register with factory and I could omit this
      g4runManager->SetUserInitialization(exoPhysicsList);
    } else {
      G4VModularPhysicsList* physicsList = exoPhysicsList->GetReferencePhysicsList(fPhysicsListName);
      // Unfortunately factory creates some default physics list if can't find name provided
      // this annihilate the following check
      // so be carefull, look for warnings and use '/physics/list' command
      if (physicsList==0)
        LogEXOMsg(Form("can't find physics list '%s'",fPhysicsListName.c_str()), EECritical);
      g4runManager->SetUserInitialization(physicsList);
    }

    stackaction = new EXOStackingAction();
    runaction   = new EXORunAction(stackaction);
    fTrackingAction = new EXOTrackingAction();
    if (fSelectedGeometry == kEXO200) {
      stepaction  = new EXOSteppingAction(static_cast<EXODetectorConstruction*>(detector));
      EXOTrackingKiller *killer = new EXOTrackingKiller(static_cast<EXODetectorConstruction*>(detector));
      stackaction->SetTrackingKiller(killer);
      stepaction->SetTrackingKiller(killer);
    }

    // Define parallel world used for importance sampling and register it
    G4String parallelName("ParallelWorld");
    EXOParallelDetectorConstruction* pdet = 
        new EXOParallelDetectorConstruction(parallelName);

    if (fUseImportanceSampling) {
      detector->RegisterParallelWorld(pdet);
      exoPhysicsList->AddParallelWorldName(parallelName);
    }


    genaction   = new EXOPrimaryGeneratorAction();


    exosimEventAction = new EXOEventAction(runaction,stackaction,stepaction);

    // set user action classes
    
    g4runManager->SetUserAction(genaction);
    g4runManager->SetUserAction(runaction);
    g4runManager->SetUserAction(exosimEventAction);
    g4runManager->SetUserAction(stackaction);  
    g4runManager->SetUserAction(stepaction);
    g4runManager->SetUserAction(fTrackingAction);
   
    //
    // Override the default G4 Isotope table by poking our version into the IonTable first
    //
    EXOIsotopeTable* exoIsotopeTable = new EXOIsotopeTable();
    if (fLifetimeLimit>0.) exoIsotopeTable->SetLifetimeLimit(fLifetimeLimit*second);
    G4IonTable *theIonTable = (G4IonTable *)(G4ParticleTable::GetParticleTable()->GetIonTable());
    G4VIsotopeTable *aVirtualTable = exoIsotopeTable;
    theIonTable->RegisterIsotopeTable(aVirtualTable);

    if (!fSkipInitialize) {
      cout << "Initializing G4 kernel..." << endl;
      g4runManager->Initialize();
      cout << "g4runManager initialized" << endl;
    }

    // Set up importance sampling

    if (fUseImportanceSampling) {
      G4cout << "Using Importance Sampling..." << G4endl;
      pdet->SetupBiasing(fISVolumeFactor);      
    }

    // get the pointer to the User Interface manager 
    UI = G4UImanager::GetUIpointer();  
    if ( UI == 0 ) {
      LogEXOMsg("unable to get G4UIManager", EECritical);
      return -1;
    }

    // Set initial seed 
    ResetRandomNumberGenerator(); 
    
    // Echo back the .mac commands to the terminal

    G4String verbosecommand = "/control/verbose 1";
    UI->ApplyCommand(verbosecommand);

    // disable Cerenkov process
    // This is workaround during switch to a new physics
    UI->ApplyCommand("/process/inactivate Cerenkov");
    
    // Read in the geant4 macro file
    G4String executecommand = "/control/execute ";
    fRunToEvents = 0;
    ReadAndApplyMacro();
    if (!fMacro || (!fMacro->good() && fRunToEvents==0)) cout << "done running macro" << endl;


    // We don't want to echo back the "/run/beamOn 1" commands, so turn 
    // off verbose echo now.

    G4String noverbosecommand = "/control/verbose 0";
    UI->ApplyCommand(noverbosecommand);


  } // end of first call stuff

  // Make sure we have a UIManager

  if ( UI == NULL ) {
    LogEXOMsg("unable to get G4UIManager", EECritical);
    return -1;
  }


  // Reset the geant event data array
  return 0;
}

//______________________________________________________________________________
EXOEventData* EXOGeant4Module::GetNextEvent()
{
  // Generate a new set of geant events if necessary Like all EXOInputModules,
  // returns NULL when there are no more events.

  size_t loopedTimes = 0;
  while(1) {
    if (BreakWasRequested()) return NULL;
    loopedTimes++;
    if ( loopedTimes >= fCyclesBeforeWarning ) {
      ostringstream os;
      os << "Have looped " << loopedTimes << " times without dumping energy in LXe." << endl
         << "    To suppress this message, you can use SetLoopCycleWarningCount "    
         << "or use the TalkTo command /exosim/loop_cycle_warning_count.";
      LogEXOMsg(os.str().c_str(), EECritical);
      loopedTimes = 0;
    }
    
    EXOEventData* tempED;

    // This call to EXOEventAction::GetNextEvent() return the next available
    // event. It will return 0 if there are no more events available, meaning
    // we have to ask Geant4 to process more events using the ApplyCommand
    // method call..
    while ((tempED = exosimEventAction->GetNextEvent()) == 0) {
     
      fG4EventNumber++; fG4SubEventNumber = 0;
      if (fG4PrintModulo && fG4EventNumber%fG4PrintModulo==0 && fG4EventNumber) {
        G4cout << __func__ << ": " << fG4EventNumber << " G4 events processed" << G4endl;
        if (!fRndSaveFile.empty())
          CLHEP::HepRandom::saveEngineStatus(fRndSaveFile.c_str());
      }
      if ( !CheckMacroFile() ) return NULL; // end of macro file events
      fTrackingAction->SetEventID(fG4EventNumber);
      fTrackingAction->Clear();
      G4String command = "/run/beamOn 1";   // .. if events aren't specified in macro
      UI->ApplyCommand(command);
  
    }
      
    
    EXOEventData& eventData = *tempED; 
    EXOMonteCarloData& mcData = eventData.fMonteCarloData;
    
    // If filtering is turned on, see if we have any energy 
    // in the LXe.
    if ( fSkipEmptyEvents ) {
      //if ( mcData.fTotalEnergyInLiquidXe < 1.0*keV ) {
      
      if ( mcData.fTotalEnergyInLiquidXe*keV < fSkipEmptyThreshold*keV ) {
        fEventNumber++;
        continue;
      }
    }
    
    // Set the correct run and event number in the geant event data
    // The run number for monte carlo events is a parameter set by the user.
    
    eventData.fRunNumber = fRunNumber;
    eventData.fEventNumber = fEventNumber;
    
    // Set the compression id to indicate monte carlo data
    
    eventData.fEventHeader.fCompressionID = 0x4000;
    eventData.fEventHeader.fGeant4EventNumber = fG4EventNumber;
    //eventData.fEventHeader.fGeant4SubEventNumber = fG4SubEventNumber; // works on its own
    eventData.fEventHeader.fIsMonteCarloEvent = true;
   
    // Increment event number and geant event index
    
    fEventNumber++;
    //This isn't quite what we want with SubEventNumber BMong 9/6/2012
    fG4SubEventNumber++;
    
    // Set flag indicating the following event comes from monte carlo.
    if( G4RunManager::GetRunManager() != NULL ) {
    
      const EXORunAction* runAction = 
        dynamic_cast< const EXORunAction* >(
        G4RunManager::GetRunManager()->GetUserRunAction());
      if( (runAction && 
           runAction->returnAlphaAnticorr() && 
           runAction->returnIncrement()%2!=0) ){
         continue; 
      }
    } 
    return &eventData ;
  }
  // We will never reach here.
  return NULL;

}

//______________________________________________________________________________
void EXOGeant4Module::StartGeant4TermSession()
{
  // Start a Geant4 terminal session to interact directly with the Geant4 UI.
  // This will then wait for commands from the user and will continue with
  // processing EXOAnalysis commands once 'exit' is given.

  // this must be on the heap, G4UIterminal owns it!
#ifdef G4_HAS_VIS 
  G4UIterminal session(new G4UItcsh);
  session.SessionStart();
#else
  LogEXOMsg("UI shell disabled in this build.", EEError);
#endif
}

//______________________________________________________________________________
void EXOGeant4Module::EnableVisualization()
{
  // Enable visualization, this should be called to set up all the vis
  // commands, etc. to view the detector geometry. 
  if ( fVisManager ) {
    LogEXOMsg("Visualization has already been enabled, did you mean to call this twice?", EEWarning);
    return;
  }
#ifdef G4VIS_USE
  fVisManager = new G4VisExecutive(); 
  if (!fSkipInitialize) fVisManager->Initialize();
#endif
} 

//______________________________________________________________________________
static const char* GoodPhysicsLists[] = {
  "EXO", // our own physics list
#if 0+G4VERSION_NUMBER < 960
  "LHEP",
#endif
#if 0+G4VERSION_NUMBER >= 930
  "FTFP_BERT",
#endif
#if 0+G4VERSION_NUMBER >= 950
  "FTFP_BERT_PEN",
#endif
#if 0+G4VERSION_NUMBER >= 960
  "FTFP_BERT_HP",
#endif
  "QGSP_BERT",
  "QGSP_BERT_HP",
#if 0+G4VERSION_NUMBER >= 950
  "Shielding",
#endif
};
static const int kNumberGoodPhysicsLists = sizeof(GoodPhysicsLists)/sizeof(GoodPhysicsLists[0]);
//______________________________________________________________________________
void EXOGeant4Module::SetPhysicsListName( std::string name )
{
  bool force = false;
  if (!name.compare(0,6,"force ")) {
    force = true;
    name.erase(0, name.find_first_not_of(' ', 6));
  }

  for (int i=0; i<kNumberGoodPhysicsLists;i++)
    if (name==GoodPhysicsLists[i]) {
      fPhysicsListName = name;
      return;
    }
  LogEXOMsg(Form("Physics list '%s' is not recommended", name.c_str()), EEError);
  if (force) fPhysicsListName = name;
}

//______________________________________________________________________________
std::string EXOGeant4Module::GetStringNameForGeometryType(
  EAvailableGeometries type) const
{
  // Return the string name for the given geometry type
  switch(type) {
    case kEXO200:  return "EXO200";
    case kFullEXO: return "FullEXO";
    default:       return "";
  }
} 

//______________________________________________________________________________
void EXOGeant4Module::SetGeometryType(std::string geom)
{
  // Set the geometry for the given string
  
  // Make sure everything is lower case
  string tempGeom = geom;
  std::transform(tempGeom.begin(), tempGeom.end(), tempGeom.begin(), ::tolower);
  for (int i=0;i<kNumberOfGeometries;i++) {
    std::string test = GetStringNameForGeometryType((EAvailableGeometries)i); 
    std::transform(test.begin(), test.end(), test.begin(), ::tolower);
    if (test == tempGeom) {
      fSelectedGeometry = (EAvailableGeometries) i;
      return;
    } 
  } 
  LogEXOMsg(Form("Unable to find geometry: %s", geom.c_str()), EEError);
}

//______________________________________________________________________________
static const char* XenonCompositionName[] = {
  "geant4",
  "natural",
  "enriched",
  "depleted"
};
static const int kNumberXenonCompositions = sizeof(XenonCompositionName)/sizeof(*XenonCompositionName);
//______________________________________________________________________________
std::string EXOGeant4Module::GetXenonComposition() const
{
  if (0<=fXenonComposition && fXenonComposition<kNumberXenonCompositions)
    return XenonCompositionName[fXenonComposition];
  else 
    return "unknown";
}

//______________________________________________________________________________
void EXOGeant4Module::SetXenonComposition(std::string aval)
{
  for (int i=0;i<kNumberXenonCompositions;i++)
    if (aval==XenonCompositionName[i]) {
      fXenonComposition = i; return; }
  LogEXOMsg(Form("Unable to find composition: %s", aval.c_str()), EEError);
}


//______________________________________________________________________________
int EXOGeant4Module::TalkTo(EXOTalkToManager *talktoManager)
{
  std::string temp;

  talktoManager->CreateCommand("/exosim/macro",
                               "geant4 macro file",
                               this,
                               "",
                               &EXOGeant4Module::SetMacroFile );

  talktoManager->CreateCommand("/exosim/initial_seed",
                               "Initialize random number generator with this seed.",
                               this,
                               fRndInitialSeed,
                               &EXOGeant4Module::SetRndInitialSeed );

  talktoManager->CreateCommand("/exosim/RndReadFile",
                               "Initialize random number generator state from this file (overrides seed).",
                               this,
                               fRndReadFile,
                               &EXOGeant4Module::SetRndReadFile );

  talktoManager->CreateCommand("/exosim/RndSaveFile",
                               "Save random number generator state to this file.",
                               this,
                               fRndSaveFile,
                               &EXOGeant4Module::SetRndSaveFile );

  talktoManager->CreateCommand("/exosim/SkipInitialize",
                               "Skip /run/initialize in Initialize() to be more flexible. "
                               "Remember to do this in macro instead.",
                               this,
                               fSkipInitialize,
                               &EXOGeant4Module::SetSkipInitialize );

  talktoManager->CreateCommand("/exosim/run_number",
                               "set the monte carlo run number",
                               this,
                               fRunNumber,
                               &EXOGeant4Module::SetRunNumber );

  temp.clear();
  for (int i=0; i<kNumberGoodPhysicsLists;i++)
    temp += GoodPhysicsLists[i], temp += ", ";
  talktoManager->CreateCommand("/exosim/physics_ListName",
                               "set G4 physics list, available packages: " + temp,
                               this,
                               fPhysicsListName,
                               &EXOGeant4Module::SetPhysicsListName );

  talktoManager->CreateCommand("/exosim/xenon_composition",
                               "Set isotopic composition of xenon, available modes: geant4, natural, enriched, depleted.",
                               this,
                               GetXenonComposition(),
                               &EXOGeant4Module::SetXenonComposition );

  talktoManager->CreateCommand("/exosim/xenon_pressure",
                               "Set pressure of liquid xenon, bar.",
                               this,
                               fXenonPressure,
                               &EXOGeant4Module::SetXenonPressure );

  talktoManager->CreateCommand("/exosim/xenon_density",
                               "Set density of liquid xenon, g/cm3.",
                               this,
                               fXenonDensity,
                               &EXOGeant4Module::SetXenonDensity );

  talktoManager->CreateCommand("/exosim/loop_cycle_warning_count",
                               "Number of cycles without energy deposition before a warning, -1 to suppress.",
                               this,
                               fCyclesBeforeWarning,
                               &EXOGeant4Module::SetLoopCycleWarningCount);

  talktoManager->CreateCommand("/exosim/isotope_lifetime_limit",
                               "Limit lifetime of isotopes decay to this value.",
                               this,
                               fLifetimeLimit,
                               &EXOGeant4Module::SetLifetimeLimit);

  talktoManager->CreateCommand("/exosim/g4printmodulo",
                               "Print a line after each this number of G4 events processed.",
                               this,
                               fG4PrintModulo,
                               &EXOGeant4Module::SetG4PrintModulo);

  talktoManager->CreateCommand("/exosim/g4interactive",
                               "Begins an interactive session with the G4 UI, useful for debugging and visualization.",
                               this,
                               &EXOGeant4Module::StartGeant4TermSession);

  talktoManager->CreateCommand("/exosim/enablevis",
                               "Enables and initializes the visualization environment.",
                               this,
                               &EXOGeant4Module::EnableVisualization);

  temp.clear();
  for (int i=0;i<kNumberOfGeometries;i++) {
    temp += GetStringNameForGeometryType((EAvailableGeometries)i) + ", "; 
  }
  talktoManager->CreateCommand("/exosim/geometry",
                               "Set the geometry, available geometries: " + temp,
                               this,
                               GetGeometryTypeString(),
                               &EXOGeant4Module::SetGeometryType);

  talktoManager->CreateCommand("/exosim/SkipEmptyEvents",
                               "Do not return events which have less than 1 keV deposited in the LXe.",
                               this,
                               fSkipEmptyEvents,
                               &EXOGeant4Module::SkipEmptyEvents);
                               
  talktoManager->CreateCommand("/exosim/SkipEmptyThreshold",
                               "The threshold that SkipEmptyEvents will skip in keV (default 1keV)",
                                this,
                                fSkipEmptyThreshold,
                                &EXOGeant4Module::SkipEmptyThreshold);

  talktoManager->CreateCommand("/exosim/SourcePosition",
                               "Place source capsule at this position in TPC coortinates, otherwise dont place the capsule at all: # X Y Z, where # is source position [2,5,8] and X Y Z is position in cm",
                               this,
                               fSourcePosition,
                               &EXOGeant4Module::SetSourcePosition);
                               
  talktoManager->CreateCommand("/exosim/VerboseConstruction",
                               "Print volumes' detailed information. Obsolete. Use '/detector/dumpGeometry' instead.",
                               this,
                               fVerboseDetConstruction,
                               &EXOGeant4Module::SetConstructionVerbose);

  talktoManager->CreateCommand("/exosim/UseImportanceSampling",
                               "Use importance sampling for gammas.  Important for lead shielding simulations.",
                               this,
                               fUseImportanceSampling,
                               &EXOGeant4Module::SetUseImportanceSampling);

  talktoManager->CreateCommand("/exosim/SetISFactor",
                               "Set IS importance sampling geometry factor.  This is the factor by which "
                               "the importance of the parallel geometries is increased as they near the "
                               "detector TPC.",
                               this,
                               fISVolumeFactor,
                               &EXOGeant4Module::SetImportanceSamplingGeometryFactor);


  return 0;
}

