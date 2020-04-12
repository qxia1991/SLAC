#ifndef EXOSourceStability_hh
#define EXOSourceStability_hh

#include "EXOAnalysisModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXODimensions.hh"

#include <TROOT.h>
#include <TH2.h>
#include <TF2.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TFile.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

enum SourceType_t {kCs137, kCo60, kTh228};

class EXOSourceStability : public EXOAnalysisModule 
{

private :

  int RunID;
  int TrigSec;
  
  bool SourceRun;
  SourceType_t SourceType;

  TH2F *hSingleSite;
  TH1F *hAnticorr;

  double ecraw;
  double ecrec;
  double csc;
  double *xcl;
  double *ycl;
  double *zcl;
  double *dtcl;
  double *ercl;
  double *epcl;

protected:

public :

  ~EXOSourceStability() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  EventStatus EndOfRun(EXOEventData *ED);

  int TalkTo(EXOTalkToManager *talktoManager);

  int ShutDown();
  
  void SourceStabilityPlot();
  
  //double g2(double *x, double *par);
  //double fun1(double *x, double *par);
  //double fun2(double *x, double *par);

  DEFINE_EXO_ANALYSIS_MODULE( EXOSourceStability )

};
#endif
