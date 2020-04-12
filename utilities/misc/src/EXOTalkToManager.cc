//______________________________________________________________________________
//
// TalkToManager
// 
// Basic class to allow interaction with a module (or any other class in
// EXOAnalysis) through scripts or at the command line.  CreateCommand handles
// the general method of creating a command from the TalkToManager.  In
// particular, they allow call-back functions to be called for each class to
// set each variable, or more generically call-back functions to be called with
// no parameters (void call-backs) when a command is called.  These functions
// are executed at the time that the manager processes the commands.  
//  
//  
//   For example:
//
//   To call a set function for a string, do the following:
//
//   class MyClass {
//     public:
//       void SetParameter( std::string val ) { fMyParam = val; }
//       void CallBack();
//
//     protected: 
//       std::string fMyParam;
//   };
//  
//
//   And when one sets up the TalkTo manager, one would call:
//
//   talktoManager->CreateCommand("/myclass/setparam", "set param for my class",
//                                this, // pointer to the object,
//                                "default_parameter",   // default parameter
//                                &MyClass::SetParameter); // pointer to class function to
//                                                         // set the parameter
//
//   you may also set up call-back functions:
//
//   talktoManager->CreateCommand("/myclass/callback", "Call back for my class",
//                                this,
//                                &MyClass::CallBack);
//
//______________________________________________________________________________

#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

//______________________________________________________________________________
EXOTalkToManager::EXOTalkToManager(const char* filename) :
  fEcho(false),
  fExit(false),
  fEchoEXO(true),
  finputStream(NULL),
  fCurrentBaseModule(NULL)
{

  // Create the basic commands, show, exit, and echo 
  CreateCommand("show","print all commands and current values", 
                this, &EXOTalkToManager::show_commands);
  CreateCommand("exit","exit program",this, 
                &EXOTalkToManager::SetExit);
  CreateCommand("echo","echo commands",this, fEcho, 
                &EXOTalkToManager::SetEchoCommands);
  CreateCommand("echoexo","echo exo command prompt",this, fEchoEXO, 
                &EXOTalkToManager::SetEchoEXO);

  if(filename)
    SetFilename(filename);
}

//______________________________________________________________________________
EXOTalkToManager::~EXOTalkToManager()
{
  for ( EXOTalkMap::iterator iter = fMap.begin(); iter != fMap.end(); iter++ ) {
    delete iter->second.fCmd;
  }
}

//______________________________________________________________________________
int EXOTalkToManager::SetFilename(const std::string& aFileName)
{
  // Opens command file for reading.
  string file = EXOMiscUtil::SearchForFile(aFileName);
  finputFileStream.open( file.c_str() );

  if ( !finputFileStream.is_open() ) {
    LogEXOMsg("unable to open file " + aFileName, EEAlert);
    return -1;
  }

  cout << "*******************************************************************" << endl;
  cout << "reading in commands from file " << aFileName << endl;
  cout << "*******************************************************************" << endl;
  SetInputStream(finputFileStream);
  return 0;

}
//______________________________________________________________________________
void EXOTalkToManager::SetInputStream(std::istream& input)
{
  finputStream = &input;
}


//______________________________________________________________________________
void EXOTalkToManager::show_commands()
{
  // Dumps all currently available commans and descriptions to the screen. 
  const char *cols = std::getenv("COLUMNS");
  const unsigned int cmd_width = 29; // minus one for ' '
  const unsigned int val_width = 14;
  const unsigned int info_width = (cols)?(strtol(cols,0,0)-cmd_width-val_width-2):40;
  cout << "**************************************************************" << endl;
  cout << "EXOTalkToManager:: available commands" << endl;
  cout << "**************************************************************" << endl;
  cout << "**************************************************************" << endl;
  cout << setw(cmd_width) << left << "Command name:" << ' '
       << setw(info_width) << left << "Description:" << ' '
       << "Current value:" << endl; 
  for ( EXOTalkMap::iterator iter = fMap.begin(); 
          iter != fMap.end(); iter++ ) {
    TalkToCommand& cmd = iter->second;
    std::string help = cmd.fHelpMsg;
    std::string cmdname = cmd.fCommandName;
    std::string val = cmd.fCmd->GetCurrentValue();
    std::string::size_type i = 0, j = 0, k = 0;
    while (i<help.size() && j!=std::string::npos) {
      if (cmdname.size()>cmd_width) {
        cout << cmdname << "\n";
        cmdname = " ";
        continue;
      }
      // The following checks to see if we have a full string, in which case
      // try to break on a space
      k = i + info_width; // how much can fit
      if (k>help.size()) // have plenty of space
        j = help.size();
      else {
        // look for 'spaces' in around
        j = help.find_last_of(" \t",k);
        if (j<=i) j = help.find_first_of(" \t",k);
      }
      // newline is more important
      k = help.find_first_of("\n",i);
      if (k!=std::string::npos && k<j) j = k;

      cout << setw(cmd_width) << left << cmdname << ' '
           << setw(info_width) << left << help.substr(i,j-i) << ' '
           << val << endl; 
      val = " ";
      cmdname = " ";
      i = j + 1;
    }
  }

  cout << "**************************************************************" << endl;

}

//______________________________________________________________________________
void EXOTalkToManager::InterpretCommand( const std::string& command )
{
  // Interpret a command, executing the associated function or storing data in
  // the associated memory location.

  // read the value from the command
  std::istringstream istr(command);
  std::string temp;
  istr >> temp;

  // see if we have this command in the list
  EXOTalkMap::iterator iter = fMap.find(temp);

  if ( iter == fMap.end() ) {
    if(IsInteractive()) {
      LogEXOMsg("command " + temp + " not found", EEError);
    }
    else {
      LogEXOMsg("command " + temp + " not found", EEAlert);
    }
    return;
  }
  TalkToCommand& theCmd = iter->second;
  std::string totalCommand, temp1;
  istr >> totalCommand;
  while(istr >> temp1) totalCommand += " " + temp1;

  try {
    theCmd.fCmd->ProcessCommand(totalCommand);
  }
  catch (EXOMiscUtil::EXOBadCommand aBadCommand) {
    if(IsInteractive()) {
      LogEXOMsg("A bad command was issued: " + command + "\n" + aBadCommand.what() + "\nTry again.", EEError);
    }
    else {
      LogEXOMsg("A bad command was issued: " + command + "\n" + aBadCommand.what(), EEAlert);
    }
    return;
  }

  if (fEcho ) {
    cout << "echo: read in value " << theCmd.fCmd->GetCurrentValue() 
         << " from command " << theCmd.fCommandName << endl;
  }
  // Since we succeeded, save the command.
  fProcessedCommands.push_back( temp + " " + totalCommand );

}

//______________________________________________________________________________
void EXOTalkToManager::InterpretStream()
{
  // Interpret given stream, either a file or an interactive input. 
  if ( ! finputStream ) {
    LogEXOMsg("No input stream.", EEAlert);
    return;
  }
  istream &in = *finputStream; 
  fProcessedCommands.clear();
  while (in) {
    if (fExit) return;
    if (IsInteractive() and fEchoEXO) cout << "EXO> ";

    std::string command;
    if (!getline(in, command)) return;
    if (in.fail()) {
      LogEXOMsg("Reading from stream failed", EEAlert);
      return;
    }

    // Removing white space 
    std::string::size_type startpos = command.find_first_not_of(" \t");
    std::string::size_type startcomment = command.find_first_of("%#");
    if ( startpos >= startcomment ) continue; // If no non-whitespace characters appear, both equal npos.
    if ( startcomment == std::string::npos) startcomment = command.size();
    command = command.substr(startpos, startcomment-startpos);

    // Echo back the command when not in interactive mode
    if (not IsInteractive()) cout << "EXO> " << command << endl;

    InterpretCommand( command );
  }
}

//______________________________________________________________________________
void EXOTalkToManager::AddCommandToMap(const TalkToCommand& cmd)
{
  // Protected, add command to the local map
  EXOTalkMap::iterator iter = fMap.find(cmd.fCommandName);
  if ( iter != fMap.end() ) {
    LogEXOMsg("replacing command " + cmd.fCommandName + " ", EEDebug);
    delete iter->second.fCmd;
  }
  fMap[cmd.fCommandName] = cmd;
}

//______________________________________________________________________________
void EXOTalkToManager::RemoveObjectCommands(void* objAddress)
{
  // Remove any commands associated with an object from TalkTo
  // If objAddress points to the base EXOAnalysisModule whose TalkTo created the command, remove that command too.
  if (!objAddress) return;
  std::vector< std::string > deleteVec; 
  for ( EXOTalkMap::iterator iter = fMap.begin(); iter != fMap.end(); iter++ ) {
    if (iter->second.fCmd->GetObjectPtr() == objAddress or iter->second.fBaseModule == objAddress) {
      deleteVec.push_back(iter->first);
    }
  }
  for(size_t i=0;i<deleteVec.size();i++) {
    delete fMap[deleteVec[i]].fCmd;
    fMap.erase(deleteVec[i]);
  }
}

//______________________________________________________________________________
const EXOTalkToManager::EXOTalkVec& EXOTalkToManager::GetAllCommands() const
{
  // Returns all the commands in a vector
  static EXOTalkVec avec;
  avec.clear();
  for (EXOTalkMap::const_iterator iter = fMap.begin(); iter != fMap.end(); iter++ ) {
    avec.push_back( iter->second );
  }
  return avec;

}

//_____________________________________________________________________________
bool EXOTalkToManager::IsInteractive() const
{
  return (finputStream == &std::cin);
}
