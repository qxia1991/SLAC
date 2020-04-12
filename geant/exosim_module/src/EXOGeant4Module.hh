#ifndef EXOGeant4Module_hh
#define EXOGeant4Module_hh
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOGeant4Module                                                      //
//                                                                      //
// Geant 4 module for interface to exosim.                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "EXOAnalysisManager/EXOInputModule.hh"
#include <cstddef> //for size_t


class G4RunManager;
class G4UImanager;
class EXOEventAction;
class EXOTrackingAction;
class G4VisExecutive;
class EXOGeant4Module: public EXOInputModule 
{

public:
  enum EAvailableGeometries { 
    kEXO200 = 0,
    kFullEXO,
    kNumberOfGeometries
  };

  enum EGeant4Consts {
    kDefaultLoopWarning = 10000
  };


protected:

  int fRunNumber;     // current run number
  int fEventNumber;   // current event number
  int fG4EventNumber;     // sequential event number in Geant4
  int fG4SubEventNumber;  // sub number in case of multiple EXO events in G4 event

  long   fRndInitialSeed;      // Set initial state of random number generator from this seed
  std::string fRndReadFile;    // Read initial state of random number generator from this file
  std::string fRndSaveFile;    // write random generator state to this file
  bool   fSkipInitialize;      // skip /run/initialize in Initialize() so user will have more freedom
  std::string fPhysicsListName; // name of a G4 physics list package, 'EXO' is for our private list
  EAvailableGeometries fSelectedGeometry;    // selected geometry
  int    fXenonComposition;    // xenon isotopic composition, actually xenonComposition_t
  double fXenonPressure;       // liquid xenon material pressure
  double fXenonDensity;        // liquid xenon material density
  bool   fSkipEmptyEvents;     // Do not return events with no energy deposited in the tpc.
  double fSkipEmptyThreshold; 
  std::string fSourcePosition;  // Put source capsule in geometry at specified position
  bool fVerboseDetConstruction;  // Turn on detector construction verbouse output
  size_t fCyclesBeforeWarning; // consecutive cycles without any event that will generate a warning. 
  double fLifetimeLimit;       // limit isotope lifetime to this value
  int    fG4PrintModulo;       // print line after every this number of G4 events

  bool   fUseImportanceSampling; // Use Importance Sampling 
  double fISVolumeFactor;        // Volume factor for Importance sampling 

  G4RunManager *g4runManager;
  G4UImanager *UI;  
  G4VisExecutive *fVisManager;


  EXOEventAction *exosimEventAction;
  EXOTrackingAction* fTrackingAction;
  
  std::istream*        fMacro;               // Pointer to macro istream
  bool                 fMacroIstreamIsOwned; // if istream is owned by thi module
  int                  fRunToEvents;         // next stopping point

  void SetRandomNumberGenerator();
  void ReadAndApplyMacro();

public :

  EXOGeant4Module();
  ~EXOGeant4Module();

  EXOEventData* GetNextEvent();
  int  get_run_number();
  int  get_event_number();
  bool reads_from_file() const {return false;}
  bool CheckMacroFile();
  int Initialize();
  int TalkTo(EXOTalkToManager *tm);

  void ResetRandomNumberGenerator();

  void SetRunNumber( int aval ) { fRunNumber = aval; }
  void SetRndInitialSeed( long aval ) { fRndInitialSeed = aval; }
  void SetRndReadFile( std::string aval ) { fRndReadFile = aval; }
  void SetRndSaveFile( std::string aval ) { fRndSaveFile = aval; }
  void SetSkipInitialize( bool aval ) { fSkipInitialize = aval; }
  void SetPhysicsListName( std::string aval );
  void SetGeometryType(std::string stringOfType);
  void SetXenonComposition( std::string aval );
  void SetXenonPressure( double aval ) { fXenonPressure = aval; }
  void SetXenonDensity( double aval ) { fXenonDensity = aval; }
  void SkipEmptyEvents(bool aVal) { fSkipEmptyEvents = aVal; }
  void SkipEmptyThreshold(double aVal) { fSkipEmptyThreshold = aVal; }

  void SetUseImportanceSampling(bool aVal) { fUseImportanceSampling = aVal; }
  void SetImportanceSamplingGeometryFactor(double aVal) { fISVolumeFactor = aVal; }
  
  void SetSourcePosition( std::string aval ){ fSourcePosition = aval; }
  void SetConstructionVerbose( bool value){ fVerboseDetConstruction = value; }
  
  void SetLoopCycleWarningCount( size_t aVal ) { fCyclesBeforeWarning = aVal; }
  void SetLifetimeLimit( double aVal ) { fLifetimeLimit = aVal; }
  void SetG4PrintModulo( int aVal ) { fG4PrintModulo = aVal; }

  std::string GetStringNameForGeometryType(EAvailableGeometries type) const;
  std::string GetGeometryTypeString() const { return GetStringNameForGeometryType(fSelectedGeometry); }
  EAvailableGeometries GetGeometryType() const { return fSelectedGeometry; }
  std::string GetXenonComposition() const;

  void SetMacroFile(std::string aval); 
  void SetMacroStream(std::istream* inputStream, bool g4ModNowOwnsThis = false);

  void StartGeant4TermSession();
  void EnableVisualization();

  DEFINE_EXO_ANALYSIS_MODULE( EXOGeant4Module )

};

#include "EXOUtilities/EXOPluginUtilities.hh"
EXO_DEFINE_PLUGIN_MODULE(EXOGeant4Module, "exosim")
#endif

//---- inlines -----------------------------------------------------------------

inline int EXOGeant4Module::get_run_number()
{
  // The run number for monte carlo events is a parameter set by the user. 
  // Default value is 1.
  return fRunNumber;
}
inline int EXOGeant4Module::get_event_number()
{
  // Get current event number. 
  return fEventNumber;
}


