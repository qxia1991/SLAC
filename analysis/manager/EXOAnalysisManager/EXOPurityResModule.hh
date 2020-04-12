#ifndef EXOPurityResModule_hh_
#define EXOPurityResModule_hh_

//#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOAnalysisModule.hh"
#include "TFile.h"
#include <iostream>
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
//#include "fstream"
#include "TStyle.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TPaveText.h"

using namespace std;

#define N_GUESSES 350

class EXOPurityResModule : public EXOAnalysisModule
{
public:
  //  EXOPurityResModule (EXOAnalysisManager* mgr) : EXOAnalysisModule(mgr) {}
  EXOPurityResModule();
  ~EXOPurityResModule() {};
  int          Initialize();
  EventStatus  ProcessEvent(EXOEventData *ED);
  int          TalkTo(EXOTalkToManager *tm);
  int          ShutDown();
  void         SetSourceType(std::string aval) { sourceInt = atoi( aval.c_str() ); }
  void         SetSourcePos(std::string aval) { sourcePos = atoi( aval.c_str() ); }
  void         SetOutFile(std::string aval) { outfile = aval.c_str(); }
  void         SetDriftTime(std::string aval) {driftBool = atoi(aval.c_str());}
  void         SetLifetime(std::string aval) {lifetime_corr = atof(aval.c_str());}

private:
  int          runNumb;
  int          runStart;
  int          runStop;
  TFile        *file;
  TH1F         *ha[N_GUESSES];
  TH1F         *haP[N_GUESSES];
  TH1F         *haN[N_GUESSES];
  //TH1F         *h0;
  TH2F         *h1; 
  //TH1F         *h2;
  //TH1F         *h3;
  TH1F         *hapdcl;
  TH1F         *hapdsum;
  TH1F         *hrcl;
  TH1F         *hgcl;  
  TH1F         *hrpcl;
  TH1F         *hgpcl;
  TH1D         *hstat;
  double       lifetimes[N_GUESSES];
  double       invLifetimes[N_GUESSES];
  double       res[N_GUESSES];
  double       res_err[N_GUESSES];
  double       means[N_GUESSES];
  double       bestLife[3], bestLifeErrP[3], bestLifeErrN[3];
  double       bestResMin[3], bestResMinErr[3];
  int          sourceInt;
  int          sourcePos;
  double       lifetime_corr;
  Bool_t       driftBool;
  const char   *outfile;
  const char   *datafile;

  
  Int_t        ncl,nsc;
  double       c1sumsc,c2sumsc,c1sc,c2sc,ercl,egcl,erpcl,egpcl,dtcl;

  //double     fitf(double *v, double *par);
  //double     fitf_co(double *v, double * par);
  //double     res_fit(double *v, double * par);

  void         DoAnalysis_Th(TH1F * happ, Int_t i);
  void         DoAnalysis_Co(TH1F * happ, Int_t i);
  //void         DoAnalysis_Cs(TH1F * happ[N_GUESSES]);
  int          DoFit(int TPCpart);

  DEFINE_EXO_ANALYSIS_MODULE( EXOPurityResModule )
    
};

#endif
