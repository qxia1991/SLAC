//______________________________________________________________________________
//______________________________________________________________________________
//------------------------------------------------------------------------------

#include "EXOAnalysisManager/EXOUneighbors.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"


#include <iostream>

#include "TMath.h"

using namespace std;
using EXOMiscUtil::GetMeanUorVPositionFromChannel;
using EXOMiscUtil::GetTPCSide;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOUneighbors, "uneighbors" )

EXOUneighbors::EXOUneighbors()
{
 nsig=4;
}

EXOUneighbors::~EXOUneighbors()
{
}

int EXOUneighbors::Initialize()
{
  return 0;
}

EXOAnalysisModule::EventStatus EXOUneighbors::BeginOfRun(EXOEventData *ED)
{
  return kOk;
}

EXOAnalysisModule::EventStatus EXOUneighbors::ProcessEvent(EXOEventData *ED)
{

  ApplyUneighborCorrection(ED);

  return kOk;
}

EXOAnalysisModule::EventStatus EXOUneighbors::EndOfRun(EXOEventData *ED)
{
  return kOk;
}

int EXOUneighbors::TalkTo(EXOTalkToManager *talktoManager)
{
 talktoManager->CreateCommand("/uneighbors/setNsigma",
			"Mamixum amplitude, in terms of sigma baseline, still considered baseline fluctuation",this,
				nsig,
				&EXOUneighbors::SetNsigma); 
  return 0;
}

int EXOUneighbors::ShutDown()
{
  return 0;
  
}



void EXOUneighbors::ApplyUneighborCorrection(EXOEventData* ED)
{
 int numc = (int)ED->GetNumChargeClusters();
 if(numc==0) {
  return;
 };

 EXOChargeCluster* qi = 0;
 EXOUWireSignal* ws = 0;

 for(int i=0;i<numc;++i) {
  qi = ED->GetChargeCluster(i);
  if(!qi) {
   LogEXOMsg(Form("Uneigbhors: Run %i, Event %i. Could not retrieve charge cluster. Moving on",ED->fRunNumber,ED->fEventNumber), EEWarning);
   continue;
  };
  int numw = (int)qi->GetNumUWireSignals();
  if(numw!=1)
   continue;//only apply correction to 1-wire clusters

  double Uorig = qi->fU;
  double Vorig = qi->fV;
  double Zorig = qi->fZ;
  double Xorig = qi->fX;
  double Yorig = qi->fY;
  double Ucorr = Uorig;
  double Xcorr = Xorig;
  double Ycorr = Yorig;

  if(Uorig==-999. || Vorig==-999 || Zorig==-999) {
   LogEXOMsg(Form("Uneigbhors: Run %i, Event %i. Will not correct unreconstructed coordinate. Moving on",ED->fRunNumber,ED->fEventNumber), EEDebug);
   continue;
  };
 
  ws = qi->GetUWireSignalAt(0);
  if(!ws) {
   LogEXOMsg(Form("Uneigbhors: Run %i, Event %i. Could not retrieve u-wire signal. Moving on",ED->fRunNumber,ED->fEventNumber), EEWarning);
   continue;
  }
  int channel0 = ws->fChannel;
  int chm = channel0-1;
  int chp = channel0+1;
   
  double t0 = qi->GetUWireSignalAt(0)->fTime;
  double Pm(0),Pp(0);
  
  if(EXOMiscUtil::TypeOfChannel(chm) == EXOMiscUtil::kUWire)
   FindPulseParams(ED,t0,chm,Pm);
  if(EXOMiscUtil::TypeOfChannel(chp) == EXOMiscUtil::kUWire)
   FindPulseParams(ED,t0,chp,Pp);

  double corr = GetUneighborsCorrection(Pm, Pp);
  Ucorr = Uorig+corr;
  qi->fU = Ucorr;//correct cluster's U coordinate
 
  EXOMiscUtil::UVToXYCoords(Ucorr,Vorig,Xcorr,Ycorr,Zorig);
  qi->fX = Xcorr;
  qi->fY = Ycorr;
 };//loop over charge clusters
};
    

    
 void EXOUneighbors::FindPulseParams(EXOEventData* ED,double t0,int channel,double& P)
{
 double woff = 10;//offset of the window in which neighbor is analyzed, wrt collection pulse
 int wwid = 30;//window width
 t0 = t0/1000.;
 t0 = t0 - woff; 

 P=-999.;//error condition by default;

 //Simplest algorithm for estimation of the pulse height on channel=channel around t0
 std::vector<int> vCollection;
 int it0 = TMath::FloorNint(t0);
 int nSamples = it0-50;

 if(it0 <=0 || it0>=2000 || nSamples < 10) {
  LogEXOMsg("FindPulseParams: t0 is negative or too close to edge of waveform. Set P to -999",EEWarning);
  return;
 };

 EXOWaveformData* wmf_data;
 EXOWaveform* wmf;
 
 wmf_data = ED->GetWaveformData();
 if(!wmf_data) {
  LogEXOMsg("FindPulseParams: Could not retrieve waveform data. Set P to -999",EEWarning);
  return;
 };

 int numwv = wmf_data->GetNumWaveforms();
 bool found(false);
 for(int i=0;i<numwv;++i) {
  wmf = wmf_data->GetWaveformToEdit(i);
  if(!wmf)
   continue;
  wmf->Decompress();
  if(wmf->fChannel == channel) {
   found = true;
   vCollection=wmf->GetVectorData();
  };
 };

 if(!found) {
  LogEXOMsg("FindPulseParams: Could not find waveform for Uneighbor signal. Set P to -999",EEWarning);
  return;
 };
 
 int csize = (int)vCollection.size();
 if(csize < (it0 + wwid)) {
  LogEXOMsg("FindPulseParams: returned waveform vector is too short. Set P to -999",EEWarning);
  return;
 }

 double bline(0),brms(0);
 for(int i=0;i<nSamples;++i) {
   bline = bline + ((double)vCollection[i])/((double)nSamples);
 };
 for(int i=0;i<nSamples;++i) {
  brms = brms + pow(((double)vCollection[i]-bline),2.);
 };
 brms = brms/(((double)nSamples)-1.);
 brms = sqrt(brms);

 double pos(bline);
 for(int i=it0;i<(it0+wwid);++i) {
  if( vCollection[i] > pos) {
   pos = (double)vCollection[i];
  };
 };
 pos = pos - bline;
 //if positive deviation is less than nsig, assume no signal at all
 if(pos < (((double)nsig)*brms)) 
  P = 0;
 else 
  P = pos;

};
  
//___________________________________________________________________
double EXOUneighbors::GetUneighborsCorrection(double fP1, double fP2)
{
 //Simplest algorithm for adjusting U-wire coordinate to account
 //for possible induction signal on neighboring wires
 
 if(fP1<0 || fP2<0 || (fP1+fP2==0)) {
  //at least one of the induction signals does not make sense, or sum adds to zeor.  return NULL.
  return 0.;
 };



 double corr = -4.5*(fP1-fP2)/(fP1+fP2);

 return corr;
}

//-------------------------------------------------------------------
