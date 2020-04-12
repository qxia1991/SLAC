#include "EXOUtilities/EXOTriggerConfig.hh"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

EXOTriggerConfig::EXOTriggerConfig()
{

}

void EXOTriggerConfig::PrintConfig()
{
  string trigsys;
  switch(fSystem){
    case APD: trigsys = "APD";
              break;
    case VGD: trigsys = "VGD";
              break;
    case HVD: trigsys = "HVD";
              break;
  }
  cout << "-------------------------------------------------------" << endl;
  cout << "*** " << trigsys << " Trigger System ***" << endl << endl;
  cout << "** Individual Trigger System **" << endl;
  cout << "* Hysteresis(" << fIndividualHysteresis << ")" << endl;
  for(TriggerSet::iterator it = fIndividualTriggerSet.begin(); it != fIndividualTriggerSet.end(); it++){
    cout << "Trigger " << it->fIndex << ": Threshold(" << it->fThreshold << "), Prescale(" << it->fPrescale << ")" << endl << endl;
  }
  cout << "** Sum Trigger System **" << endl;
  cout << "* Hysteresis(" << fSumHysteresis << "), BackgroundCount(" << fBackgroundCount << ")" << endl;
  for(TriggerSet::iterator it = fSumTriggerSet.begin(); it != fSumTriggerSet.end(); it++){
    cout << "Trigger " << it->fIndex << ": Threshold(" << it->fThreshold << "), Prescale(" << it->fPrescale << ")" << endl << endl;
  }
  cout << "The following channels are disabled:" << endl;
  cout << "(module|channel)" << endl;
  int counter = 1;
  for(PairIntSet::iterator setiter = fDisabledChannels.begin(); setiter != fDisabledChannels.end(); setiter++){
    stringstream stream;
    stream << "(" << setiter->first << "|" << setiter->second << ")";
    cout << setw(8) << stream.str();
    if(!(counter%5)){ 
      cout << endl;
    }
    counter++;
  }
  cout << endl;
  cout << "** Baseline Corrections **" << endl;
  cout << "* BaselineReference(" << fBaselineReference << "), BaselineComputedFromRun(" << fBaselineComputedFromRun << ")" << endl;
  cout << "(module|channel): offset" << endl;
  counter = 1;
  for(PairIntMap::iterator mapiter = fChannelOffsets.begin(); mapiter != fChannelOffsets.end(); mapiter++){
    stringstream stream;
    stream << "(" << mapiter->first.first << "|" << mapiter->first.second << "): " << mapiter->second;
    cout << setw(14) << stream.str();
    if(!(counter%5)){
      cout << endl;
    }
    counter++;
  }
  cout << endl;
}
