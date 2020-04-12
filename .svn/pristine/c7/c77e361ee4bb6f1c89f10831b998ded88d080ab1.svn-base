//______________________________________________________________________________
#include "EXOAnalysisManager/EXOExampleModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <iostream>
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOExampleModule, "example" )

EXOExampleModule::EXOExampleModule()
{

  //  cout << "constructing EXOExampleModule with name " << NAME << endl;

}

int EXOExampleModule::Initialize()
{

  return 0;
}

EXOAnalysisModule::EventStatus EXOExampleModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  return kOk;
}

EXOAnalysisModule::EventStatus EXOExampleModule::ProcessEvent(EXOEventData *ED)
{

  //  cout << "At ProcessEvent for " << name << " run rumber = " << ED->fRunNumber << 
  //    ", event number = " << ED->fEventNumber << endl;

  // Get the number of samples in the event

  //  cout << "example: qbeta = " << ED->fMonteCarloData.fBetaDecayQValue << endl;


  return kOk;
}

EXOAnalysisModule::EventStatus EXOExampleModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}

int EXOExampleModule::TalkTo(EXOTalkToManager *talktoManager)
{

  // Create some example commands

  // example double command

  talktoManager->CreateCommand("ANewDoubleCommand","read in a double", 
           this, 0.0, &EXOExampleModule::SetDoubleParam);

  // example int command

  talktoManager->CreateCommand("ANewIntCommand","read in an int", 
           this, 0, &EXOExampleModule::SetIntParam );
  
  // example bool command

  talktoManager->CreateCommand("ANewBoolCommand","read in a bool", 
           this, false, &EXOExampleModule::SetBoolParam );
  
  // example string command

  talktoManager->CreateCommand("ANewStringCommand","read in a string", 
           this, "", &EXOExampleModule::SetStringParam );

  // example generic call-back 

  talktoManager->CreateCommand("ANewCallBackCommand","generic call-back", 
           this, &EXOExampleModule::CallBack );

  return 0;
}

int EXOExampleModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}

void EXOExampleModule::CallBack()
{
  std::cout << "EXOExampleModule::CallBack() called. " << std::endl;
}


