//______________________________________________________________________________
#include "EXOAnalysisManager/EXOEventSelectionModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include <iostream>
#ifdef HAVE_ROOT
#include <TROOT.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOEventSelectionModule, "evsel" )

EXOEventSelectionModule::EXOEventSelectionModule() :
  nell(-1), 
  neul(-1),
  nrll(-1),
  nrul(-1),
  nscll(-1),
  nscul(-1),
  nclll(-1),
  nclul(-1),
  ecrecll(-1), 
  ecrecul(-1),
  trigsecll(-1),
  trigsecul(-1),
  trigsubll(-1),
  trigsubul(-1),
  trigdriftll(-1),
  trigdriftul(-1),
  snll(-1),
  snul(-1),
  srll(-1),
  srul(-1),   
  trigsumll(-1), 
  trigsumul(-1), 
  inll(-1),
  inul(-1),
  irll(-1),
  irul(-1),
  indsumll(-1), 
  indsumul(-1),
  trigoffll(-1),
  trigofful(-1),
  srcll(-1),
  srcul(-1),
  muon_inc(-1)
{}

int EXOEventSelectionModule::Initialize()
{

  return 0;

  
}

EXOAnalysisModule::EventStatus EXOEventSelectionModule::BeginOfRun(EXOEventData *ED)
{
  if (fEventListFilename!="") LoadEventList(fEventListFilename);

  printf("************************************************************************\n");
  printf("EXOEventSelectionModule: Selecting events with the following parameters:\n");
  printf("************************************************************************\n");
  if ( nell >= 0 ) printf("fEventNumber >= %d\n", nell);
  if ( neul >= 0 ) printf("fEventNumber <= %d\n", neul);
  if ( nrll >= 0 ) printf("fRunNumber >= %d\n", nrll);
  if ( nrul >= 0 ) printf("fRunNumber <= %d\n", nrul);
  if ( nscll >= 0 ) printf("num scintillation clusters >= %d\n", nscll);
  if ( nscul >= 0  ) printf("num scintillation clusters <= %d\n", nscul);
  if ( nclll >= 0  ) printf("num charge clusters >= %d\n", nclll);
  if ( nclul >= 0  ) printf("num charge clusters <= %d\n", nclul);
  if ( ecrecll >= 0 ) printf("fTotalGridEfficiencyCorrectedEnergy >= %f\n", ecrecll);
  if ( ecrecul >= 0  ) printf("fTotalGridEfficiencyCorrectedEnergy <= %f\n", ecrecul);
  if ( trigsecll >= 0  ) printf("fEventHeader.fTriggerSeconds >= %d\n", trigsecll);
  if ( trigsecul >= 0  ) printf("fEventHeader.fTriggerSeconds <= %d\n", trigsecul);
  if ( trigsubll >= 0  ) printf("fEventHeader.fTriggerMicroSeconds >= %d\n", trigsubll);
  if ( trigsubul >= 0  ) printf("fEventHeader.fTriggerMicroSeconds <= %d\n", trigsubul);
  if ( trigdriftll >= 0 ) printf("fEventHeader.fTriggerDrift >= %d\n", trigdriftll);
  if ( trigdriftul >= 0  ) printf("fEventHeader.fTriggerDrift <= %d\n", trigdriftul);
  if ( snll >= 0 ) printf("fEventHeader.fSumTriggerThreshold >= %d\n", snll);
  if ( snul >= 0  ) printf("fEventHeader.fSumTriggerThreshold <= %d\n", snul);
  if ( srll >= 0  ) printf("fEventHeader.fSumTriggerRequest >= %d\n", srll);
  if ( srul >= 0  ) printf("fEventHeader.fSumTriggerRequest <= %d\n", srul);
  if ( trigsumll >= 0  ) printf("fEventHeader.fSumTriggerValue >= %d\n", trigsumll);
  if ( trigsumul >= 0  ) printf("fEventHeader.fSumTriggerValue <= %d\n", trigsumul);
  if ( inll >= 0 ) printf("fEventHeader.fIndividualTriggerThreshold >= %d\n", inll);
  if ( inul >= 0  ) printf("fEventHeader.fIndividualTriggerThreshold <= %d\n", inul);
  if ( irll >= 0 ) printf("fEventHeader.fIndividualTriggerRequest >= %d\n", irll);
  if ( irul >= 0  ) printf("fEventHeader.fIndividualTriggerRequest <= %d\n", irul);
  if ( indsumll >= 0 ) printf("fEventHeader.fMaxValueChannel >= %d\n", indsumll);
  if ( indsumul >= 0  ) printf("fEventHeader.fMaxValueChannel <= %d\n", indsumul);
  if ( trigoffll >= 0 ) printf("fEventHeader.fTriggerOffset >= %d\n", trigoffll);
  if ( trigofful >= 0  ) printf("fEventHeader.fTriggerOffset <= %d\n", trigofful);
  if ( srcll >= 0 ) printf("fEventHeader.fTriggerSource >= %d\n", srcll);
  if ( srcul >= 0 ) printf("fEventHeader.fTriggerSource <= %d\n", srcul);
  if ( muon_inc == 0) printf("Exclude muon events\n");
  if ( muon_inc == 1) printf("Include only muon events\n");

  printf("************************************************************************\n");


  return kOk;
}

EXOAnalysisModule::EventStatus EXOEventSelectionModule::ProcessEvent(EXOEventData *ED) { return CheckEvent(ED); }

EXOAnalysisModule::EventStatus EXOEventSelectionModule::CheckEvent(const EXOEventData *ED) const
{

  bool veto = false;

  if (fEventList.size()) {
    unsigned i;
    for(i=0;i<fEventList.size();i++)
      if (fEventList[i].first==ED->fRunNumber && fEventList[i].second==ED->fEventNumber) break;
    if (!veto) veto = !(i<fEventList.size());
  }

#ifdef HAVE_ROOT
  if (fSelection!="") {
    char buf[256];
    snprintf(buf,255,"const EXOEventData *ED=(const EXOEventData*)%p; ",ED);
    string cmd;
    cmd += buf; // standard prefix, define ED
    cmd += fSelection;
    cmd += ";"; // suppress output of processing
    if (!veto) veto = !gROOT->ProcessLine(cmd.c_str()); // processing returns 'we take it'
    //cout << "xxx '" << ED->fEventNumber <<"' "<< veto <<" "<< endl;
  }
#endif

  // Select on run number and event number

  if ( nell >= 0 && ED->fEventNumber < nell ) veto = true;
  if ( neul >= 0 && ED->fEventNumber > neul ) veto = true;

  if ( nrll >= 0 && ED->fRunNumber < nrll ) veto = true;
  if ( nrul >= 0 && ED->fRunNumber > nrul ) veto = true;

  // Select on the number of scintillation clusters and charge clusters

  if ( nscll >= 0 && ED->GetNumScintillationClusters() < (unsigned int) nscll ) veto = true;
 if ( nscul >= 0 && ED->GetNumScintillationClusters() > (unsigned int) nscul ) veto = true;

  if ( nclll >= 0 && ED->GetNumChargeClusters() < (unsigned int) nclll ) veto = true;
  if ( nclul >= 0 && ED->GetNumChargeClusters() > (unsigned int) nclul ) veto = true;

  // Select on the total reconstructed energy

  // Select on trigger info

  const EXOEventHeader& header = ED->fEventHeader;
  if ( trigsecll >= 0 && header.fTriggerSeconds < (unsigned int)trigsecll ) veto = true;
  if ( trigsecul >= 0 && header.fTriggerSeconds > (unsigned int)trigsecul ) veto = true;

  if ( trigsubll >= 0 && header.fTriggerMicroSeconds < (unsigned int)trigsubll ) veto = true;
  if ( trigsubul >= 0 && header.fTriggerMicroSeconds > (unsigned int)trigsubul ) veto = true;

  if ( trigdriftll >= 0 && header.fTriggerDrift < trigdriftll ) veto = true;
  if ( trigdriftul >= 0 && header.fTriggerDrift > trigdriftul ) veto = true;

  if ( snll >= 0 && header.fSumTriggerThreshold < (unsigned int)snll ) veto = true;
  if ( snul >= 0 && header.fSumTriggerThreshold > (unsigned int)snul ) veto = true;

  if ( srll >= 0 && header.fSumTriggerRequest < (unsigned int)srll ) veto = true;
  if ( srul >= 0 && header.fSumTriggerRequest > (unsigned int)srul ) veto = true;

  if ( trigsumll >= 0 && header.fSumTriggerValue < (unsigned int)trigsumll ) veto = true;
  if ( trigsumul >= 0 && header.fSumTriggerValue > (unsigned int)trigsumul ) veto = true;

  if ( inll >= 0 && header.fIndividualTriggerThreshold < (unsigned int)inll ) veto = true;
  if ( inul >= 0 && header.fIndividualTriggerThreshold > (unsigned int)inul ) veto = true;

  if ( irll >= 0 && header.fIndividualTriggerRequest < (unsigned int)irll ) veto = true;
  if ( irul >= 0 && header.fIndividualTriggerRequest > (unsigned int)irul ) veto = true;

  if ( indsumll >= 0 && header.fMaxValueChannel < (unsigned int)indsumll ) veto = true;
  if ( indsumul >= 0 && header.fMaxValueChannel > (unsigned int)indsumul ) veto = true;

  if ( trigoffll >= 0 && header.fTriggerOffset < (unsigned int)trigoffll ) veto = true;
  if ( trigofful >= 0 && header.fTriggerOffset > (unsigned int)trigofful ) veto = true;

  if ( srcll >= 0 && header.fTriggerSource < (unsigned int)srcll ) veto = true;
  if ( srcul >= 0 && header.fTriggerSource > (unsigned int)srcul ) veto = true;

  if (muon_inc == 0 && header.fTaggedAsMuon ) veto = true;
  if (muon_inc == 1 && !header.fTaggedAsMuon ) veto = true;

  if ( veto == true ) return kDrop;

  return kOk;

}

int EXOEventSelectionModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/evsel/event_list",
                               "File containing list of desired events, make it with tree->Scan()", 
                               this, "", 
                               &EXOEventSelectionModule::SetEventListFilename);

#ifdef HAVE_ROOT
  talktoManager->CreateCommand("/evsel/selection",
                               "Flexible selection rule of C++ code returning bool, use EXOEventData *ED", 
                               this, "", 
                               &EXOEventSelectionModule::SetSelection);
#endif

  
  int int0 = -1;
  double double0 = -1.0;

  talktoManager->CreateCommand("/evsel/ne_ll","lower limit for fEventNumber", this, int0, &EXOEventSelectionModule::Setnell );
  talktoManager->CreateCommand("/evsel/ne_ul","upper limit for fEventNumber", this, int0, &EXOEventSelectionModule::Setneul );
  talktoManager->CreateCommand("/evsel/nr_ll","lower limit for fRunNumber", this, int0, &EXOEventSelectionModule::Setnrll );
  talktoManager->CreateCommand("/evsel/nr_ul","upper limit for fRunNumber", this, int0, &EXOEventSelectionModule::Setnrul );
  talktoManager->CreateCommand("/evsel/nsc_ll","lower limit for num scintillation clusters", this, int0, &EXOEventSelectionModule::Setnscll );
  talktoManager->CreateCommand("/evsel/nsc_ul","upper limit for num scintillation clusters", this, int0, &EXOEventSelectionModule::Setnscul );
  talktoManager->CreateCommand("/evsel/ncl_ll","lower limit for num charge clusters", this, int0, &EXOEventSelectionModule::Setnclll );
  talktoManager->CreateCommand("/evsel/ncl_ul","upper limit for num charge clusters", this, int0, &EXOEventSelectionModule::Setnclul );
  talktoManager->CreateCommand("/evsel/ecrec_ll","lower limit for fTotalGridEfficiencyCorrectedEnergy", this, double0, &EXOEventSelectionModule::Setecrecll );
  talktoManager->CreateCommand("/evsel/ecrec_ul","upper limit for fTotalGridEfficiencyCorrectedEnergy", this, double0, &EXOEventSelectionModule::Setecrecul );
  talktoManager->CreateCommand("/evsel/trigsec_ll","lower limit for fEventHeader.fTriggerSeconds", this, int0, &EXOEventSelectionModule::Settrigsecll );
  talktoManager->CreateCommand("/evsel/trigsec_ul","upper limit for fEventHeader.fTriggerSeconds", this, int0, &EXOEventSelectionModule::Settrigsecul );
  talktoManager->CreateCommand("/evsel/trigsub_ll","lower limit for fEventHeader.fTriggerMicroSeconds", this, int0, &EXOEventSelectionModule::Settrigsubll );
  talktoManager->CreateCommand("/evsel/trigsub_ul","upper limit for fEventHeader.fTriggerMicroSeconds", this, int0, &EXOEventSelectionModule::Settrigsubul );
  talktoManager->CreateCommand("/evsel/trigdrift_ll","lower limit for fEventHeader.fTriggerDrift", this, int0, &EXOEventSelectionModule::Settrigdriftll );
  talktoManager->CreateCommand("/evsel/trigdrift_ul","upper limit for fEventHeader.fTriggerDrift", this, int0, &EXOEventSelectionModule::Settrigdriftul );
  talktoManager->CreateCommand("/evsel/sn_ll","lower limit for snll", this, int0, &EXOEventSelectionModule::Setsnll );
  talktoManager->CreateCommand("/evsel/sn_ul","upper limit for snul", this, int0, &EXOEventSelectionModule::Setsnul );
  talktoManager->CreateCommand("/evsel/sr_ll","lower limit for srll", this, int0, &EXOEventSelectionModule::Setsrll );
  talktoManager->CreateCommand("/evsel/sr_ul","upper limit for srul", this, int0, &EXOEventSelectionModule::Setsrul );     
  talktoManager->CreateCommand("/evsel/trigsum_ll","lower limit for fEventHeader.fSumTriggerValue", this, int0, &EXOEventSelectionModule::Settrigsumll );
  talktoManager->CreateCommand("/evsel/trigsum_ul","upper limit for fEventHeader.fSumTriggerValue", this, int0, &EXOEventSelectionModule::Settrigsumul );
  talktoManager->CreateCommand("/evsel/in_ll","lower limit for fEventHeader.fIndividualTriggerThreshold", this, int0, &EXOEventSelectionModule::Setinll );
  talktoManager->CreateCommand("/evsel/in_ul","upper limit for fEventHeader.fIndividualTriggerThreshold", this, int0, &EXOEventSelectionModule::Setinul );
  talktoManager->CreateCommand("/evsel/ir_ll","lower limit for fEventHeader.fIndividualTriggerRequest", this, int0, &EXOEventSelectionModule::Setirll );
  talktoManager->CreateCommand("/evsel/ir_ul","upper limit for fEventHeader.fIndividualTriggerRequest", this, int0, &EXOEventSelectionModule::Setirul );
  talktoManager->CreateCommand("/evsel/indsum_ll","lower limit for fEventHeader.fMaxValueChannel", this, int0, &EXOEventSelectionModule::Setindsumll );
  talktoManager->CreateCommand("/evsel/indsum_ul","upper limit for fEventHeader.fMaxValueChannel", this, int0, &EXOEventSelectionModule::Setindsumul );
  talktoManager->CreateCommand("/evsel/trigoff_ll","lower limit for fEventHeader.fTriggerOffset", this, int0, &EXOEventSelectionModule::Settrigoffll );
  talktoManager->CreateCommand("/evsel/trigoff_ul","upper limit for fEventHeader.fTriggerOffset", this, int0, &EXOEventSelectionModule::Settrigofful );
  talktoManager->CreateCommand("/evsel/src_ll","lower limit for fEventHeader.fTriggerSource", this, int0, &EXOEventSelectionModule::Setsrcll );
  talktoManager->CreateCommand("/evsel/src_ul","upper limit for fEventHeader.fTriggerSource", this, int0, &EXOEventSelectionModule::Setsrcul );
  talktoManager->CreateCommand("/evsel/muon_include","0 selects all but muon events 1 selects muon events", this, int0, &EXOEventSelectionModule::Setmuon_inc );

  // Set the default value of the filter status to true.
  SetFiltered( true );
  return 0;
}

int EXOEventSelectionModule::LoadEventList(const string &filename)
{
  ifstream fin;
  fin.open(filename.c_str());
  if (!fin.good()) {
    LogEXOMsg("Failed to open file", EEError);
    //cout << __func__ << ": failed to open '" << filename <<"': "<< endl;
    return -1;
  }
  cout << __func__ << ": opened '" << filename <<"'"<< endl;
  string s;
  string::size_type i, j;
  int k, i1 = -1, i2 = -1;
  int irun = 0, iev = 0; bool in_head = false;
  while(getline(fin,s)) {
    if (s.find_first_not_of("*")==string::npos) continue;
    if (s.find_first_not_of("+-")==string::npos) continue;
    if (!s.compare(0,4,"==> ")) continue;
    i=s.find_first_not_of(" ",0); k=0; j=s.find_first_of("*|",i);
    while(i!=string::npos) {
      if (!s.compare(i,4,"Row ") || !s.compare(i,6,"Event ")) in_head = true;
      if (in_head) {
        if (!s.compare(i,9,"fRunNumber ",9) || !s.compare(i,3,"nr ")) i1 = k;
        if (!s.compare(i,9,"fEventNumber ",9) || !s.compare(i,3,"ne ")) i2 = k;
      } else {
        if (k==i1) irun = strtol(s.substr(i,j-i).c_str(),0,10);
        if (k==i2) iev = strtol(s.substr(i,j-i).c_str(),0,10);
      }
      //cout << k <<" "<< i <<" "<< j <<" '"<< s.substr(i,j-i) <<"'"<< endl;
      if (j==string::npos) break;
      i=s.find_first_not_of(" ",j+1); k++; j=s.find_first_of("*|",i);
    }
    if (in_head) {
      cout << __func__ <<": using fields: run="<< i1 <<", event="<< i2 << endl;
      if (i1<0 || i2<0) cout << __func__ << ": warning: some fields not found, will use 0 for it" << endl;
      in_head = false;
    } else {
      //cout <<"= "<< ev.dat <<" "<< ev.fil <<" "<< ev.event << endl;
      if (k>=i1 && k>=i2) fEventList.push_back(std::pair<int,int>(irun,iev));
    }
    s.clear();
  }
  cout << __func__ << ": loaded " << fEventList.size() << " records" << endl;
  return fEventList.size();
}
