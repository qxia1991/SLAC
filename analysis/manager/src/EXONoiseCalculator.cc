/* This module loops over all waveforms in the data and calculates
 * their baseline and noisecount.
 * The baseline and noisecount values are calculated in the same 
 * way as in reconstruction.
 * The module saves the extracted information in a separate ROOT 
 * tree and file.
 * The base filename can be set via TalkTo command.
 */

//______________________________________________________________________________
#include "EXOAnalysisManager/EXONoiseCalculator.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOEventInfo.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TAxis.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXONoiseCalculator, "noisecalc" )
EXONoiseCalculator::EXONoiseCalculator()
: fFilenameBase("NoiseOutput"),
  fFile(NULL),
  fTree(NULL),
  fNevents(0)
{
  fEventInfo = new EXOEventInfo();
}

EXONoiseCalculator::~EXONoiseCalculator()
{
  delete fEventInfo;
  delete fFile;
  delete fTree;
}


EXOAnalysisModule::EventStatus EXONoiseCalculator::BeginOfRun(EXOEventData *ED)
{
  if(!ED){
    return kError;
  }
  if ( ED->GetWaveformData()->GetNumWaveforms() == 0 ) {
    LogEXOMsg("no digitized data for first event. Need it to initialize channels", EEError);
    return kError;
  }
  const EXOChannelMap& channelMap = GetChanMapForHeader(ED->fEventHeader);
  fNevents = 0;
  fChannels.clear();
  for(size_t i=0; i<ED->GetWaveformData()->GetNumWaveforms(); i++){
    int channel = ED->GetWaveformData()->GetWaveform(i)->fChannel;
    if(channelMap.good_channel(channel)){
      fChannels.insert(channel);
    }
  }
  stringstream filename; 
  filename << fFilenameBase << "_Run" << ED->fRunNumber << ".root";
  fFile = new TFile(filename.str().c_str(),"RECREATE");
  if(fFile->IsZombie()){
    cout << "Error opening file " << filename.str() << endl;
    delete fFile;
    fFile = NULL;
    return kError;
  }

  fTree = new TTree("NoiseTree","NoiseTree");

  fTree->Branch("fEventInfo",&fEventInfo);

  return kOk;
}

EXOAnalysisModule::EventStatus EXONoiseCalculator::ProcessEvent(EXOEventData *ED)
{
  if(!ED){
    return kError;
  }
  if ( ED->GetWaveformData()->GetNumWaveforms() == 0 ) {
    LogEXOMsg("no digitized data for this event. Skipping.", EEDebug);
    return kDrop;
  }
  fNevents++;
  fEventInfo->Clear();
  fEventInfo->fRunNumber = ED->fRunNumber;
  fEventInfo->fEventNumber = ED->fEventNumber;
  EXOWaveformData* wfd = ED->GetWaveformData();
  wfd->Decompress();
  for(size_t i=0; i<wfd->GetNumWaveforms(); i++){
    const EXOWaveform *wf = wfd->GetWaveform(i);
    EXOChannelInfo* info = fEventInfo->GetNewChannelInfo();
    info->fChannel = wf->fChannel;
    info->fLength = wf->GetLength();
    EXOMiscUtil::ParameterMap params = fBaselineCalculator.ExtractAll(*wf);
    info->fBaseline = params["Baseline"];
    info->fNoiseCounts = params["Noisecounts"];
  }
  fTree->Fill();
  return kOk;
}

EXOAnalysisModule::EventStatus EXONoiseCalculator::EndOfRun(EXOEventData* ED)
{
  fTree->SetEstimate(fTree->GetEntries());

  map<string,TDirectory*> directories;

  TDirectory* Noisedir = fFile->mkdir("Noise");
  TDirectory* Baselinedir = fFile->mkdir("Baseline");
  directories.insert(make_pair(string("Baseline APD"),Baselinedir->mkdir("APD")));
  directories.insert(make_pair(string("Baseline U"),Baselinedir->mkdir("U-wire")));
  directories.insert(make_pair(string("Baseline V"),Baselinedir->mkdir("V-wire")));
  directories.insert(make_pair(string("Noise APD"),Noisedir->mkdir("APD")));
  directories.insert(make_pair(string("Noise U"),Noisedir->mkdir("U-wire")));
  directories.insert(make_pair(string("Noise V"),Noisedir->mkdir("V-wire")));

  map<string, vector<double> > averages;

  averages.insert(make_pair(string("Baseline U North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Baseline U South"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Baseline V North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Baseline V South"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Baseline APD North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Baseline APD South"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise U North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise U South"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise V North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise V South"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise APD North"),vector<double>(fNevents,0)));
  averages.insert(make_pair(string("Noise APD South"),vector<double>(fNevents,0)));

  map<string, int> sums;

  sums.insert(make_pair(string("Baseline U North"),0));
  sums.insert(make_pair(string("Baseline U South"),0));
  sums.insert(make_pair(string("Baseline V North"),0));
  sums.insert(make_pair(string("Baseline V South"),0));
  sums.insert(make_pair(string("Baseline APD North"),0));
  sums.insert(make_pair(string("Baseline APD South"),0));
  sums.insert(make_pair(string("Noise U North"),0));
  sums.insert(make_pair(string("Noise U South"),0));
  sums.insert(make_pair(string("Noise V North"),0));
  sums.insert(make_pair(string("Noise V South"),0));
  sums.insert(make_pair(string("Noise APD North"),0));
  sums.insert(make_pair(string("Noise APD South"),0));

  for(set<int>::iterator channel = fChannels.begin(); channel != fChannels.end(); channel++){
    EXOMiscUtil::EChannelType type = EXOMiscUtil::TypeOfChannel(*channel);
    EXOMiscUtil::ETPCSide side = EXOMiscUtil::GetTPCSide(*channel);
    string typestring = "";
    string sidestring = "";
    switch(type){
      case EXOMiscUtil::kAPDGang:
        typestring = "APD";
        break;
      case EXOMiscUtil::kUWire:
        typestring = "U";
        break;
      case EXOMiscUtil::kVWire:
        typestring = "V";
        break;
      default:
        LogEXOMsg("Unknown waveform type",EEWarning);
    }
    switch(side){
      case EXOMiscUtil::kNorth:
        sidestring = "North";
        break;
      case EXOMiscUtil::kSouth:
        sidestring = "South";
        break;
      default:
        LogEXOMsg("Channel has undefined detector side",EEError);
    }

    stringstream chan;
    chan << *channel;
    string cutstring = "fChannel == " + chan.str();

    fTree->Draw("fNoiseCounts:fEventNumber",cutstring.c_str(),"goff");
    double* array = fTree->GetV1();
    if(fTree->GetSelectedRows() != fNevents){
      LogEXOMsg("Number of selected rows not as expected",EEError);
      return kDrop;
    }
    directories["Noise "+typestring]->cd();
    TGraph noiseGraph(fTree->GetSelectedRows(),fTree->GetV2(),array);
    noiseGraph.SetName(("Noise_"+chan.str()).c_str());
    noiseGraph.GetXaxis()->SetTitle("Event number");
    noiseGraph.GetYaxis()->SetTitle("Noisecount");
    noiseGraph.SetTitle(("Noise "+sidestring+" "+typestring+", channel "+chan.str()).c_str());
    noiseGraph.Write("",TObject::kOverwrite);
    string key = "Noise "+typestring+" "+sidestring;
    vector<double>& noiseAverage = averages[key];
    for(int j=0; j<fNevents; j++){
      noiseAverage[j] += array[j];
    }
    sums[key]++;

    fTree->Draw("fBaseline:fEventNumber",cutstring.c_str(),"goff");
    array = fTree->GetV1();
    if(fTree->GetSelectedRows() != fNevents){
      LogEXOMsg("Number of selected rows not as expected",EEError);
      return kDrop;
    }
    directories["Baseline "+typestring]->cd();
    TGraph baselineGraph(fTree->GetSelectedRows(),fTree->GetV2(),fTree->GetV1());
    baselineGraph.SetName(("Baseline_"+chan.str()).c_str());
    baselineGraph.GetXaxis()->SetTitle("Event number");
    baselineGraph.GetYaxis()->SetTitle("Baseline");
    baselineGraph.SetTitle(("Baseline "+sidestring+" "+typestring+", channel "+chan.str()).c_str());
    baselineGraph.Write("",TObject::kOverwrite);
    key = "Baseline "+typestring+" "+sidestring;
    vector<double>& baselineAverage = averages[key];
    for(int j=0; j<fNevents; j++){
      baselineAverage[j] += array[j];
    }
    sums[key]++;
  }

  fTree->Draw("fEventNumber","","goff");

  for(map<string, vector<double> >::iterator iter=averages.begin(); iter != averages.end(); iter++){
    for(size_t j=0; j<iter->second.size(); j++){
      (iter->second)[j] /= sums[iter->first];
    }
    TGraph averageGraph(fNevents,fTree->GetV1(),&((iter->second)[0]));
    averageGraph.SetName(iter->first.c_str());
    averageGraph.SetTitle(iter->first.c_str());
    averageGraph.GetXaxis()->SetTitle("Event number");
    if(iter->first.find("Baseline") != string::npos){
      averageGraph.GetYaxis()->SetTitle("Baseline");
      Baselinedir->cd();
      averageGraph.Write("",TObject::kOverwrite);
    }
    else if(iter->first.find("Noise") != string::npos){
      averageGraph.GetYaxis()->SetTitle("Noisecount");
      Noisedir->cd();
      averageGraph.Write("",TObject::kOverwrite);
    }
    else{
      cout << "should not be here!" << endl;
    }
  }

  return kOk;
}

int EXONoiseCalculator::ShutDown()
{
  if(!fTree){
    cout << "Error in EXONoiseCalculator::ShutDown(): No tree!" << endl;
    return -1;
  }
  if(!fFile){
    cout << "Error in EXONoiseCalculator::ShutDown(): No file!" << endl;
    return -1;
  }

  fFile->cd();
  fTree->Write("",TObject::kOverwrite);
  fFile->Close();
  fTree = NULL;  //closing the file deletes the tree.
  delete fFile;
  fFile = NULL;
  return 0;
}

int EXONoiseCalculator::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/noisecalc/file","Base name of the output file", 
           this, fFilenameBase, &EXONoiseCalculator::SetFilenameBase);

  return 0;
}

