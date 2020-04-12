#include "EXOAnalysisManager/EXOAnalysisManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TSystem.h"
#include <iostream>
#ifdef STATIC
#include "TTreePlayer.h"
#include "TStreamerInfo.h"
#endif

using namespace std;
#ifndef BUILD_ID
#define BUILD_ID "Unknown"
#endif

#ifdef STATIC
class TBuffer;
extern void std_string_streamer(TBuffer &b, void *objadd);
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main( int argc, char *argv[] ) {

    cout << "EXOAnalysis: " << BUILD_ID << endl;
    cout << "Build Flags:";
#ifndef HAVE_MYSQL
    cout << " NO MYSQL";
#else
    cout << " MYSQL";
#endif
#ifdef STATIC
    cout << " Static build";
    // Forcing the load of these component classes, we have to ensure the
    // globals in these compilation units get initialized.
    TStreamerInfo::Class();
    TVirtualStreamerInfo::SetFactory(new TStreamerInfo());
    TTreePlayer::Class();
    TVirtualTreePlayer::SetPlayer("TTreePlayer");
    // Force the load of the string streamer
    if (TClass::Class() == NULL) std_string_streamer(*((TBuffer*)0), NULL); 
#endif
    cout << endl;

  // If we ever upgrade to a more recent version of root (5.34+, I think), should replace these with gSystem->ResetSignals().
  gSystem->ResetSignal(kSigBus);
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigQuit);
  gSystem->ResetSignal(kSigInterrupt);
  gSystem->ResetSignal(kSigWindowChanged);
  gSystem->ResetSignal(kSigChild);
  gSystem->ResetSignal(kSigUrgent);
  gSystem->ResetSignal(kSigFloatingException);
  gSystem->ResetSignal(kSigTermination);
  gSystem->ResetSignal(kSigUser1);
  gSystem->ResetSignal(kSigUser2);

  // Create Talk-To Manager
  EXOTalkToManager talktoManager;

  if ( argc >= 2 ) {
    int result = talktoManager.SetFilename( argv[1] );
    if ( result < 0 ) return 0 ;
  }
  else {
    talktoManager.SetInputStream( cin );
  }

  // Make sure there is an instance of TApplication.
  // This is necessary for the graphics libraries to be loaded
  // (Requirement for plotting)
  EXOMiscUtil::GetApplication();
  
  // Create analysis manager
  EXOAnalysisManager analysisManager( &talktoManager );

  // Show registered modules
  analysisManager.ShowRegisteredModules();

    // Run analysis
  talktoManager.InterpretStream(); 

  // Print running statistics
  analysisManager.print_statistics();

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
