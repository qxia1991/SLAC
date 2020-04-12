#include "EXOMGMPluginTwo.hh" 
#include <iostream>
#include <string>
#include "EXOUtilities/EXOTalkToManager.hh" 

int EXOMGMPluginTwo::Initialize() { 
  std::cout << "PlugIn Initialize " << GetAlias() << " " << fMyLoggedName << std::endl; return 0; }
EXOAnalysisModule::EventStatus EXOMGMPluginTwo::BeginOfRun(EXOEventData *ED) {
  std::cout << "Plugin BeginOfRun " << GetAlias() << " " << fMyLoggedName << std::endl; return kOk; }

int EXOMGMPluginTwo::TalkTo(EXOTalkToManager *talktoManager)
{
  // Constructing the name of the command takes a bit more effort since we want to allow this class
  // to have more than one copy.  We prepend the alias to the command name.  See the test_plugin.exo
  // file in the test directory for how to use this functionality.  
  std::string command = "/";
  command += GetAlias();
  command += "/myloggedname";
  talktoManager->CreateCommand(command.c_str(), "Logging name", this, "", &EXOMGMPluginTwo::SetLoggedName); 
  return 0;
}
