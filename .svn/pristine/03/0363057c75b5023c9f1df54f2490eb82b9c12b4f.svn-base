// ------------------------------------------------------------------------------------------------
// EXOSourceStability
//
// Performs 2D fits on source runs and determines peak positions for checking stability of the runs.
// The peak positions for each run are stored in /nfs/slac/g/exo//SourcePositions.root.

#include <iostream>
#include "EXOAnalysisManager/EXOSourceStability.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOControlRecordList.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOSourceStability, "stab" )

double g2(double *x, double *par) 
{
  double A = par[0];
  double x_0 = par[1];
  double y_0 = par[2];
  double sigma_x = par[3];
  double sigma_y = par[4];
  double a = TMath::Cos(par[5])*TMath::Cos(par[5]) / (2*sigma_x*sigma_x) + TMath::Sin(par[5])*TMath::Sin(par[5]) / (2*sigma_y*sigma_y);
  double b = -1.0 * TMath::Sin(2*par[5]) / (4*sigma_x*sigma_x) + TMath::Sin(2*par[5]) / (4*sigma_y*sigma_y);
  double c = TMath::Sin(par[5])*TMath::Sin(par[5]) / (2*sigma_x*sigma_x) + TMath::Cos(par[5])*TMath::Cos(par[5]) / (2*sigma_y*sigma_y);
  
  double val = A*TMath::Exp(-1.0*(a*(x[0] - x_0)*(x[0] - x_0) + 2*b*(x[0] - x_0)*(x[1] - y_0) + c*(x[1] - y_0)*(x[1] - y_0)));
  
  return val;
}

double fun1(double *x, double *par)
{
  return g2(x,par);
}

double fun2(double *x, double *par)
{
  double *p1 = &par[0];
  double *p2 = &par[6];
  double result = g2(x,p1) + g2(x,p2);
  
  return result;
}

double fitFunction1(double *x, double *par)
{
  double A1_gaus = par[0];
  double E1 = par[1];
  double sigma1 = par[2];
  double A1_erf = par[3]*par[0];
  
  double gauss1 = A1_gaus * TMath::Gaus(x[0],E1,sigma1);
  double erf1 = A1_erf * 0.5 * TMath::Erfc((x[0] - E1) / (TMath::Sqrt(2)*sigma1));
  
  return gauss1 + erf1;
}

double fitFunction2(double *x, double *par)
{
  double A1_gaus = par[0];
  double E1 = par[1];
  double sigma1 = par[2];
  double A1_erf = A1_gaus*par[3];
  double A2_gaus = A1_gaus*par[4];
  double E2 = par[5];
  //double sigma2 = sigma1*TMath::Sqrt(E2/E1);
  double sigma2 = par[6];
  double A2_erf = A2_gaus*par[3];
  
  double gauss1 = A1_gaus * TMath::Gaus(x[0],E1,sigma1);
  double erf1 = A1_erf * 0.5 * TMath::Erfc((x[0] - E1) / (TMath::Sqrt(2)*sigma1));
  double gauss2 = A2_gaus * TMath::Gaus(x[0],E2,sigma2);
  double erf2 = A2_erf * 0.5 * TMath::Erfc((x[0] - E2) / (TMath::Sqrt(2)*sigma2));
  
  return gauss1 + erf1 + gauss2 + erf2;
}


int EXOSourceStability::Initialize() {

  std::cout << "EXOSourceStability-> Initialize..." << std::endl;

  xcl = new double[100];
  ycl = new double[100];
  zcl = new double[100];
  ercl = new double[100];
  epcl = new double[100];
  dtcl = new double[100];

  return 0;
}

EXOAnalysisModule::EventStatus EXOSourceStability::BeginOfRun(EXOEventData *ED) 
{
  // get control record list
  EXOControlRecordList* reclist= dynamic_cast<EXOControlRecordList*>(FindSharedObject("ControlRecords"));
  if (!reclist) {
    LogEXOMsg("Can't find record list?", EEError);
    return kOk;
  }
  
  SourceRun = true;
  const EXOBeginSourceCalibrationRunRecord* rec = reclist->GetNextRecord<EXOBeginSourceCalibrationRunRecord>();
  if (!rec) {
    // if it's NULL, it means it didn't find this type of record, i.e. this is not a source run.
    SourceRun = false;
    std::cout << "EXOSourceStability-> This is not a source run" << std::endl;
    return kOk;
  }
  
  std::string SourceTypeString = rec->GetSourceTypeString();
  if (!SourceTypeString.compare("Weak Co")) {SourceType = kCo60; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  if (!SourceTypeString.compare("Strong Co")) {SourceType = kCo60; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  if (!SourceTypeString.compare("Weak Cs")) {SourceType = kCs137; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  if (!SourceTypeString.compare("Strong Cs")) {SourceType = kCs137; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  if (!SourceTypeString.compare("Weak Th")) {SourceType = kTh228; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  if (!SourceTypeString.compare("Strong Th")) {SourceType = kTh228; std::cout << "EXOSourceStability-> Source type: " << SourceTypeString << std::endl;}
  
  char hTitle[50];
  sprintf(hTitle,"Single site events (run %i)",ED->fRunNumber);
  
  if (SourceType == kCs137) {hSingleSite = new TH2F("hSingleSite",hTitle,200,0,1500,200,0,5000);}
  if (SourceType == kCo60) {hSingleSite = new TH2F("hSingleSite",hTitle,200,0,2500,200,0,10000);}
  if (SourceType == kTh228) {hSingleSite = new TH2F("hSingleSite",hTitle,200,0,3500,200,0,11000);}
  
  hAnticorr = new TH1F("hAnticorr",hTitle,200,0,3500);

  return kOk;
}

EXOAnalysisModule::EventStatus EXOSourceStability::ProcessEvent(EXOEventData *ED)
{
  if (!SourceRun) {return kOk;}
  
  RunID = ED->fRunNumber;
  TrigSec = ED->fEventHeader.fTriggerSeconds;
  
  double purFitP0;
  double purFitP1;
  double purFitP2;
  double purFitP3;
  double purFitP4;
  
  double purTime = double(ED->fEventHeader.fTriggerSeconds - 1304146800.0) / 3600.0 / 24.0;
  
  if (purTime < 58) {
    purFitP0 = -284.596;
    purFitP1 = 53.6978;
    purFitP2 = -1.88664;
    purFitP3 = 0.0269101;
    purFitP4 = -0.000133772;
  }
  if (purTime >= 58 && purTime < 81.6) {
    purFitP0 = 14068.5;
    purFitP1 = -908.011;
    purFitP2 = 21.8864;
    purFitP3 = -0.230994;
    purFitP4 = 0.00090631;
  }
  if (purTime >= 81.6 && purTime < 94.0) {
    purFitP0 = -9011.55;
    purFitP1 = 115.417;
    purFitP2 = 0.0;
    purFitP3 = 0.0;
    purFitP4 = 0.0;
  }
  if (purTime >= 94.0 && purTime < 102.5) {
    purFitP0 = 2000.0;
    purFitP1 = 0.0;
    purFitP2 = 0.0;
    purFitP3 = 0.0;
    purFitP4 = 0.0;
  }
  if (purTime >= 102.5 && purTime < 113.0) {
    purFitP0 = -1208000.0;
    purFitP1 = 34380.0;
    purFitP2 = -325.9;
    purFitP3 = 1.03;
    purFitP4 = 0.0;
  }
  if (purTime >= 113.0 && purTime < 129.6) {
    purFitP0 = -48740.0;
    purFitP1 = 805.0;
    purFitP2 = -3.259;
    purFitP3 = 0.0;
    purFitP4 = 0.0;
  }
  if (purTime >= 129.6 && purTime < 142.0) {
    purFitP0 = -29510.0;
    purFitP1 = 230.1;
    purFitP2 = 0.0;
    purFitP3 = 0.0;
    purFitP4 = 0.0;
  }
  if (purTime >= 142.0) {
    purFitP0 = 4000.0;
    purFitP1 = 0.0;
    purFitP2 = 0.0;
    purFitP3 = 0.0;
    purFitP4 = 0.0;
  }
  
  double elife = purFitP4*purTime*purTime*purTime*purTime + purFitP3*purTime*purTime*purTime + purFitP2*purTime*purTime + purFitP1*purTime + purFitP0;
  
  int nsc = ED->GetNumScintillationClusters();
  for (int scID = 0; scID < nsc; scID++) {
     EXOScintillationCluster *scint_cluster = ED->GetScintillationCluster(scID);

     double tsc1 = scint_cluster->fTime;

     bool GoodScintCluster = true;
     // make sure no other scintillation cluster is within 110us
     for (int i = 0; i < nsc; i++) {
        if (i == scID) {continue;}

        double tsc2 = ED->GetScintillationCluster(i)->fTime;
        if (TMath::Abs(tsc2 - tsc1) < 110000) {GoodScintCluster = false; break;}
     }

     if (!GoodScintCluster) {continue;}
     if (scint_cluster->fTime > 1928000) {continue;}
     int ncl = scint_cluster->GetNumChargeClusters();

     csc = scint_cluster->fRawEnergy;

     // safe all charge clusters in array
     for (int clID = 0; clID < ncl; clID++) {
        EXOChargeCluster *charge_cluster = scint_cluster->GetChargeClusterAt(clID);

        xcl[clID] = charge_cluster->fX;
        ycl[clID] = charge_cluster->fY;
        zcl[clID] = charge_cluster->fZ;
        dtcl[clID] = charge_cluster->fDriftTime / 1000.0;
        ercl[clID] = charge_cluster->fCorrectedEnergy;
        if (charge_cluster->fPurityCorrectedEnergy == 0.0) {epcl[clID] = charge_cluster->fCorrectedEnergy * TMath::Exp(dtcl[clID]/elife);}
        else {epcl[clID] = charge_cluster->fPurityCorrectedEnergy;}
     }

     // apply fiducial cut
     bool fiducial = true;
     ecraw = 0.0;
     ecrec = 0.0;
     for (int i = 0; i < ncl; i++) {
        ecraw += ercl[i];
        ecrec += epcl[i];
        if (TMath::Sqrt(xcl[i]*xcl[i] + ycl[i]*ycl[i]) > 163) {fiducial = false; continue;}
        if (zcl[i] > 152 || zcl[i] < -152) {fiducial = false; continue;}
        if (zcl[i] > -20 && zcl[i] < 20) {fiducial = false; continue;}
     }
     if (ncl == 0) {fiducial = false;}

     if (!fiducial) {continue;}
    if (ncl == 1) {hSingleSite->Fill(ecrec,csc); hAnticorr->Fill((csc*TMath::Sin(0.1956) + ecrec*TMath::Cos(0.1956))*0.6138 - 3.328);}
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOSourceStability::EndOfRun(EXOEventData *ED) 
{
  if (!SourceRun) {return kOk;}
  
  std::cout << "EXOSourceStability-> Fitting..." << std::endl;
  
  if (SourceType == kCs137 ) {
    TF2 *fit = new TF2("fit",fun1,500,700,2000,3000,6);
    fit->SetParameters(40,600,2500,70,200,0.0);
    fit->SetParNames("A","E_I","E_S","#sigma_I","#sigma_S","#Theta");
    
    fit->SetParLimits(0,0,1000);
    fit->SetParLimits(1,540,640);
    fit->SetParLimits(2,2200,2700);
    fit->SetParLimits(3,10,100);
    fit->SetParLimits(4,20,600);
    
    hSingleSite->Fit("fit","rq");
    
    double par[6];
    double *parErr;
    fit->GetParameters(par);
    parErr = fit->GetParErrors();
    
    TF1 *fitMin = new TF1("fitMin",fitFunction1,500,800,4);
    
    fitMin->SetParNames("A1","E1","#sigma","A2");
    
    fitMin->SetParameters(2000,662,100,0.2);
    
    fitMin->SetParLimits(1,600,720);
    fitMin->SetParLimits(2,10,200);
    fitMin->SetParLimits(3,0.05,2.0);
    
    hAnticorr->Fit("fitMin","r");
    
    double E1 = fitMin->GetParameter(1);
    double sigma1 = fitMin->GetParameter(2);
    double E1_err = fitMin->GetParError(1);
    double sigma1_err = fitMin->GetParError(2);
    double Res = sigma1/E1;
    double Res_err = TMath::Sqrt(1/(E1*E1)*sigma1_err*sigma1_err + (sigma1/(E1*E1))*(sigma1/(E1*E1))*E1_err*E1_err);
    
    char FormatedOutput[1000];
    sprintf(FormatedOutput,"%i\t%i\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.4f\n",RunID,TrigSec - 1304146800,par[1],par[2],par[3],par[4],par[5],parErr[1],parErr[2],parErr[3],parErr[4],parErr[5],E1,sigma1,E1_err,sigma1_err,Res,Res_err);
    
    FILE *ResultFile;
    ResultFile = fopen("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Cs137.dat","a");
    fputs(FormatedOutput,ResultFile);
    fclose(ResultFile);
    
    // save 2D plot in file
    TFile *f2DPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Cs1372DPlots.root","UPDATE");
    hSingleSite->Write();
    f2DPlot->Close();
    
    // save anticorrelation plot in file
    TFile *fAnticorrPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Cs137AnticorrPlots.root","UPDATE");
    hAnticorr->Write();
    f2DPlot->Close();
  }

  if (SourceType == kCo60) {
    TF2 *fit = new TF2("fit",fun2,1000,1500,3500,6000,12);
    double params[12] = {40,1100,4000,70,200,0.0,40,1300,4600,70,200,0.0};
    fit->SetParameters(params);
    
    fit->SetParLimits(0,0,1000);
    fit->SetParLimits(1,1000,1200);
    fit->SetParLimits(2,3600,4400);
    fit->SetParLimits(3,10,100);
    fit->SetParLimits(4,20,600);
    fit->SetParLimits(6,0,1000);
    fit->SetParLimits(7,1250,1400);
    fit->SetParLimits(8,4400,5000);
    fit->SetParLimits(9,10,100);
    fit->SetParLimits(10,20,600);
    
    hSingleSite->Fit("fit","rq");
    
    double par[12];
    double *parErr;
    fit->GetParameters(par);
    parErr = fit->GetParErrors();
    
    TF1 *fitMin = new TF1("fitMin",fitFunction2,900,1450,7);
    fitMin->SetParNames("A1","E1","#sigma1","R1","R2","E2","#sigma2");
    
    fitMin->SetParameters(3000,1173,75,0.6,0.8,1332,75);
    fitMin->SetParLimits(1,1070,1270);
    fitMin->SetParLimits(2,10,120);
    fitMin->SetParLimits(3,0.05,1.0);
    fitMin->SetParLimits(4,0.5,1.5);
    fitMin->SetParLimits(5,1220,1440);
    fitMin->SetParLimits(6,10,130);
    
    hAnticorr->Fit("fitMin","r");
    
    double E1 = fitMin->GetParameter(1);
    double sigma1 = fitMin->GetParameter(2);
    double E2 = fitMin->GetParameter(5);
    double sigma2 = fitMin->GetParameter(6);
    double E1_err = fitMin->GetParError(1);
    double sigma1_err = fitMin->GetParError(2);
    double E2_err = fitMin->GetParError(5);
    double sigma2_err = fitMin->GetParError(6);
    double Res1 = sigma1/E1;
    double Res1_err = TMath::Sqrt(1/(E1*E1)*sigma1_err*sigma1_err + (sigma1/(E1*E1))*(sigma1/(E1*E1))*E1_err*E1_err);
    double Res2 = sigma2/E2;
    double Res2_err = TMath::Sqrt(1/(E2*E2)*sigma2_err*sigma2_err + (sigma2/(E2*E2))*(sigma2/(E2*E2))*E2_err*E2_err);
    
    char FormatedOutput[1000];
    sprintf(FormatedOutput,"%i\t%i\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.4f\t%.4f\t%.4f\n",RunID,TrigSec - 1304146800,par[1],par[2],par[3],par[4],par[5],par[7],par[8],par[9],par[10],par[11],parErr[1],parErr[2],parErr[3],parErr[4],parErr[5],parErr[7],parErr[8],parErr[9],parErr[10],parErr[11],E1,E2,sigma1,sigma2,E1_err,E2_err,sigma1_err,sigma2_err,Res1,Res2,Res1_err,Res2_err);
    
    FILE *ResultFile;
    ResultFile = fopen("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Co60.dat","a");
    fputs(FormatedOutput,ResultFile);
    fclose(ResultFile);
    
    // save 2D plot in file
    TFile *f2DPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Co602DPlots.root","UPDATE");
    hSingleSite->Write();
    f2DPlot->Close();
    
    // save anticorrelation plot in file
    TFile *fAnticorrPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Co60AnticorrPlots.root","UPDATE");
    hAnticorr->Write();
    f2DPlot->Close();
  }
  
  if (SourceType == kTh228) {
    if (hSingleSite->GetEntries() < 18000) {return kOk;}
    
    TF2 *fit = new TF2("fit",fun1,2450,2850,7500,9800,6);
    fit->SetParameters(40,2640,8500,70,200,-0.13);
    fit->SetParNames("A","E_I","E_S","#sigma_I","#sigma_S","#Theta");
    
    fit->SetParLimits(0,0,1000);
    fit->SetParLimits(1,2560,2700);
    fit->SetParLimits(2,8000,9000);
    fit->SetParLimits(3,10,200);
    fit->SetParLimits(4,20,600);
    fit->SetParLimits(5,-0.2,-0.05);

    hSingleSite->Fit("fit","rq");
    
    double par[6];
    double *parErr;
    fit->GetParameters(par);
    parErr = fit->GetParErrors();
    
    TF1 *fitMin = new TF1("fitMin",fitFunction1,2300,2750,4);
    
    fitMin->SetParNames("A1","E1","#sigma","A2");
    
    fitMin->SetParameters(10000,2615,100,0.2);
    
    fitMin->SetParLimits(1,2400,2800);
    fitMin->SetParLimits(2,50,200);
    fitMin->SetParLimits(3,0.05,2.0);
    
    hAnticorr->Fit("fitMin","r");
    
    double E1 = fitMin->GetParameter(1);
    double sigma1 = fitMin->GetParameter(2);
    double E1_err = fitMin->GetParError(1);
    double sigma1_err = fitMin->GetParError(2);
    double Res = sigma1/E1;
    double Res_err = TMath::Sqrt(1/(E1*E1)*sigma1_err*sigma1_err + (sigma1/(E1*E1))*(sigma1/(E1*E1))*E1_err*E1_err);

    char FormatedOutput[1000];
    sprintf(FormatedOutput,"%i\t%i\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.4f\n",RunID,TrigSec - 1304146800,par[1],par[2],par[3],par[4],par[5],parErr[1],parErr[2],parErr[3],parErr[4],parErr[5],E1,sigma1,E1_err,sigma1_err,Res,Res_err);

    FILE *ResultFile;
    ResultFile = fopen("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Th228.dat","a");
    fputs(FormatedOutput,ResultFile);
    fclose(ResultFile);
    
    // save 2D plot in file
    TFile *f2DPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Th2282DPlots.root","UPDATE");
    hSingleSite->Write();
    f2DPlot->Close();
    
    // save anticorrelation plot in file
    TFile *fAnticorrPlot = new TFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Th228AnticorrPlots.root","UPDATE");
    hAnticorr->Write();
    f2DPlot->Close();
  }
  
  hSingleSite->Reset();
  
  return kOk;
}

int EXOSourceStability::TalkTo(EXOTalkToManager *talktoManager)
{
  return 0;
}

int EXOSourceStability::ShutDown()
{
  if (!SourceRun) {return 0;}
  
  SourceStabilityPlot();
  
  return 0;
}

void EXOSourceStability::SourceStabilityPlot()
{
  gROOT->SetStyle("Plain");
  
  TTree* tCs137 = new TTree;
  TTree* tCo60 = new TTree;
  TTree* tTh228 = new TTree;
  
  tCs137->ReadFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Cs137.dat");
  tCo60->ReadFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Co60.dat");
  tTh228->ReadFile("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Th228.dat");
  
  int runID_Cs137;
  int time_Cs137;
  double E_I_Cs137;
  double E_S_Cs137;
  double sigma_I_Cs137;
  double sigma_S_Cs137;
  double E_I_err_Cs137;
  double E_S_err_Cs137;
  double sigma_I_err_Cs137;
  double sigma_S_err_Cs137;
  double A_Cs137;
  double R_Cs137;
  double A_err_Cs137;
  double R_err_Cs137;
  
  int runID_Co60;
  int time_Co60;
  double E_I1_Co60;
  double E_S1_Co60;
  double E_I2_Co60;
  double E_S2_Co60;
  double sigma_I1_Co60;
  double sigma_S1_Co60;
  double sigma_I2_Co60;
  double sigma_S2_Co60;
  double E_I1_err_Co60;
  double E_S1_err_Co60;
  double E_I2_err_Co60;
  double E_S2_err_Co60;
  double sigma_I1_err_Co60;
  double sigma_S1_err_Co60;
  double sigma_I2_err_Co60;
  double sigma_S2_err_Co60;
  double A1_Co60;
  double R1_Co60;
  double A2_Co60;
  double R2_Co60;
  double A1_err_Co60;
  double R1_err_Co60;
  double A2_err_Co60;
  double R2_err_Co60;
  
  int runID_Th228;
  int time_Th228;
  double E_I_Th228;
  double E_S_Th228;
  double sigma_I_Th228;
  double sigma_S_Th228;
  double E_I_err_Th228;
  double E_S_err_Th228;
  double sigma_I_err_Th228;
  double sigma_S_err_Th228;
  double A_Th228;
  double R_Th228;
  double A_err_Th228;
  double R_err_Th228;
  
  int maxRunID = 0;
  
  tCs137->SetBranchAddress("RunNumber",&runID_Cs137);
  tCs137->SetBranchAddress("Time",&time_Cs137);
  tCs137->SetBranchAddress("E_I",&E_I_Cs137);
  tCs137->SetBranchAddress("E_S",&E_S_Cs137);
  tCs137->SetBranchAddress("sigma_I",&sigma_I_Cs137);
  tCs137->SetBranchAddress("sigma_S",&sigma_S_Cs137);
  tCs137->SetBranchAddress("E_I_err",&E_I_err_Cs137);
  tCs137->SetBranchAddress("E_S_err",&E_S_err_Cs137);
  tCs137->SetBranchAddress("sigma_I_err",&sigma_I_err_Cs137);
  tCs137->SetBranchAddress("sigma_S_err",&sigma_S_err_Cs137);
  tCs137->SetBranchAddress("A",&A_Cs137);
  tCs137->SetBranchAddress("R",&R_Cs137);
  tCs137->SetBranchAddress("A_err",&A_err_Cs137);
  tCs137->SetBranchAddress("R_err",&R_err_Cs137);
  
  tCo60->SetBranchAddress("RunNumber",&runID_Co60);
  tCo60->SetBranchAddress("Time",&time_Co60);
  tCo60->SetBranchAddress("E_I1",&E_I1_Co60);
  tCo60->SetBranchAddress("E_S1",&E_S1_Co60);
  tCo60->SetBranchAddress("E_I2",&E_I2_Co60);
  tCo60->SetBranchAddress("E_S2",&E_S2_Co60);
  tCo60->SetBranchAddress("sigma_I1",&sigma_I1_Co60);
  tCo60->SetBranchAddress("sigma_S1",&sigma_S1_Co60);
  tCo60->SetBranchAddress("sigma_I2",&sigma_I2_Co60);
  tCo60->SetBranchAddress("sigma_S2",&sigma_S2_Co60);
  tCo60->SetBranchAddress("E_I1_err",&E_I1_err_Co60);
  tCo60->SetBranchAddress("E_S1_err",&E_S1_err_Co60);
  tCo60->SetBranchAddress("E_I2_err",&E_I2_err_Co60);
  tCo60->SetBranchAddress("E_S2_err",&E_S2_err_Co60);
  tCo60->SetBranchAddress("sigma_I1_err",&sigma_I1_err_Co60);
  tCo60->SetBranchAddress("sigma_S1_err",&sigma_S1_err_Co60);
  tCo60->SetBranchAddress("sigma_I2_err",&sigma_I2_err_Co60);
  tCo60->SetBranchAddress("sigma_S2_err",&sigma_S2_err_Co60);
  tCo60->SetBranchAddress("A1",&A1_Co60);
  tCo60->SetBranchAddress("R1",&R1_Co60);
  tCo60->SetBranchAddress("A2",&A2_Co60);
  tCo60->SetBranchAddress("R2",&R2_Co60);
  tCo60->SetBranchAddress("A1_err",&A1_err_Co60);
  tCo60->SetBranchAddress("R1_err",&R1_err_Co60);
  tCo60->SetBranchAddress("A2_err",&A2_err_Co60);
  tCo60->SetBranchAddress("R2_err",&R2_err_Co60);
  
  tTh228->SetBranchAddress("RunNumber",&runID_Th228);
  tTh228->SetBranchAddress("Time",&time_Th228);
  tTh228->SetBranchAddress("E_I",&E_I_Th228);
  tTh228->SetBranchAddress("E_S",&E_S_Th228);
  tTh228->SetBranchAddress("sigma_I",&sigma_I_Th228);
  tTh228->SetBranchAddress("sigma_S",&sigma_S_Th228);
  tTh228->SetBranchAddress("E_I_err",&E_I_err_Th228);
  tTh228->SetBranchAddress("E_S_err",&E_S_err_Th228);
  tTh228->SetBranchAddress("sigma_I_err",&sigma_I_err_Th228);
  tTh228->SetBranchAddress("sigma_S_err",&sigma_S_err_Th228);
  tTh228->SetBranchAddress("A",&A_Th228);
  tTh228->SetBranchAddress("R",&R_Th228);
  tTh228->SetBranchAddress("A_err",&A_err_Th228);
  tTh228->SetBranchAddress("R_err",&R_err_Th228);
  
  int nCs137 = tCs137->GetEntries();
  int nCo60 = tCo60->GetEntries();
  int nTh228 = tTh228->GetEntries();
  
  double *xCs137 = new double[nCs137];
  double *ICs137 = new double[nCs137];
  double *SCs137 = new double[nCs137];
  double *ISCs137 = new double[nCs137];
  double *SSCs137 = new double[nCs137];
  double *IECs137 = new double[nCs137];
  double *SECs137 = new double[nCs137];
  double *ISECs137 = new double[nCs137];
  double *SSECs137 = new double[nCs137];
  double *ACs137 = new double[nCs137];
  double *AECs137 = new double[nCs137];
  double *RCs137 = new double[nCs137];
  double *RECs137 = new double[nCs137];
  double *xCo60_1 = new double[nCo60];
  double *ICo60_1 = new double[nCo60];
  double *SCo60_1 = new double[nCo60];
  double *ISCo60_1 = new double[nCo60];
  double *SSCo60_1 = new double[nCo60];
  double *IECo60_1 = new double[nCo60];
  double *SECo60_1 = new double[nCo60];
  double *ISECo60_1 = new double[nCo60];
  double *SSECo60_1 = new double[nCo60];
  double *xCo60_2 = new double[nCo60];
  double *ICo60_2 = new double[nCo60];
  double *SCo60_2 = new double[nCo60];
  double *ISCo60_2 = new double[nCo60];
  double *SSCo60_2 = new double[nCo60];
  double *IECo60_2 = new double[nCo60];
  double *SECo60_2 = new double[nCo60];
  double *ISECo60_2 = new double[nCo60];
  double *SSECo60_2 = new double[nCo60];
  double *ACo60_1 = new double[nCo60];
  double *AECo60_1 = new double[nCo60];
  double *RCo60_1 = new double[nCo60];
  double *RECo60_1 = new double[nCo60];
  double *ACo60_2 = new double[nCo60];
  double *AECo60_2 = new double[nCo60];
  double *RCo60_2 = new double[nCo60];
  double *RECo60_2 = new double[nCo60];
  double *xTh228 = new double[nTh228];
  double *ITh228 = new double[nTh228];
  double *STh228 = new double[nTh228];
  double *ISTh228 = new double[nTh228];
  double *SSTh228 = new double[nTh228];
  double *IETh228 = new double[nTh228];
  double *SETh228 = new double[nTh228];
  double *ISETh228 = new double[nTh228];
  double *SSETh228 = new double[nTh228];
  double *ATh228 = new double[nTh228];
  double *AETh228 = new double[nTh228];
  double *RTh228 = new double[nTh228];
  double *RETh228 = new double[nTh228];
  
  for (int i = 0; i < nCs137; i++) {
    tCs137->GetEntry(i);
    
    xCs137[i] = runID_Cs137;
    ICs137[i] = E_I_Cs137;
    SCs137[i] = E_S_Cs137;
    ISCs137[i] = sigma_I_Cs137;
    SSCs137[i] = sigma_S_Cs137;
    IECs137[i] = E_I_err_Cs137;
    SECs137[i] = E_S_err_Cs137;
    ISECs137[i] = sigma_I_err_Cs137;
    SSECs137[i] = sigma_S_err_Cs137;
    ACs137[i] = A_Cs137;
    AECs137[i] = A_err_Cs137;
    RCs137[i] = R_Cs137;
    RECs137[i] = R_err_Cs137;
    
    if (runID_Cs137 > maxRunID) {maxRunID = runID_Cs137;}
  }
  
  for (int i = 0; i < nCo60; i++) {
    tCo60->GetEntry(i);
    
    xCo60_1[i] = runID_Co60;
    ICo60_1[i] = E_I1_Co60;
    SCo60_1[i] = E_S1_Co60;
    ISCo60_1[i] = sigma_I1_Co60;
    SSCo60_1[i] = sigma_S1_Co60;
    IECo60_1[i] = E_I1_err_Co60;
    SECo60_1[i] = E_S1_err_Co60;
    ISECo60_1[i] = sigma_I1_err_Co60;
    SSECo60_1[i] = sigma_S1_err_Co60;
    ICo60_2[i] = E_I2_Co60;
    SCo60_2[i] = E_S2_Co60;
    ISCo60_2[i] = sigma_I2_Co60;
    SSCo60_2[i] = sigma_S2_Co60;
    IECo60_2[i] = E_I2_err_Co60;
    SECo60_2[i] = E_S2_err_Co60;
    ISECo60_2[i] = sigma_I2_err_Co60;
    SSECo60_2[i] = sigma_S2_err_Co60;
    ACo60_1[i] = A1_Co60;
    AECo60_1[i] = A1_err_Co60;
    RCo60_1[i] = R1_Co60;
    RECo60_1[i] = R1_err_Co60;
    ACo60_2[i] = A2_Co60;
    AECo60_2[i] = A2_err_Co60;
    RCo60_2[i] = R2_Co60;
    RECo60_2[i] = R2_err_Co60;
    
    if (runID_Co60 > maxRunID) {maxRunID = runID_Co60;}
  }
  
  for (int i = 0; i < nTh228; i++) {
    tTh228->GetEntry(i);
    
    xTh228[i] = runID_Th228;
    ITh228[i] = E_I_Th228;
    STh228[i] = E_S_Th228;
    ISTh228[i] = sigma_I_Th228;
    SSTh228[i] = sigma_S_Th228;
    IETh228[i] = E_I_err_Th228;
    SETh228[i] = E_S_err_Th228;
    ISETh228[i] = sigma_I_err_Th228;
    SSETh228[i] = sigma_S_err_Th228;
    ATh228[i] = A_Th228;
    AETh228[i] = A_err_Th228;
    RTh228[i] = R_Th228;
    RETh228[i] = R_err_Th228;
    
    if (runID_Th228 > maxRunID) {maxRunID = runID_Th228;}
  }
  
  TGraphErrors *grCs137_I = new TGraphErrors(nCs137,xCs137,ICs137,0,IECs137);
  TGraphErrors *grCs137_S = new TGraphErrors(nCs137,xCs137,SCs137,0,SECs137);
  TGraphErrors *grCs137_IS = new TGraphErrors(nCs137,xCs137,ISCs137,0,ISECs137);
  TGraphErrors *grCs137_SS = new TGraphErrors(nCs137,xCs137,SSCs137,0,SSECs137);
  TGraphErrors *grCs137_A = new TGraphErrors(nCs137,xCs137,ACs137,0,AECs137);
  TGraphErrors *grCs137_R = new TGraphErrors(nCs137,xCs137,RCs137,0,RECs137);
  TGraphErrors *grCo60_1_I = new TGraphErrors(nCo60,xCo60_1,ICo60_1,0,IECo60_1);
  TGraphErrors *grCo60_1_S = new TGraphErrors(nCo60,xCo60_1,SCo60_1,0,SECo60_1);
  TGraphErrors *grCo60_1_IS = new TGraphErrors(nCo60,xCo60_1,ISCo60_1,0,ISECo60_1);
  TGraphErrors *grCo60_1_SS = new TGraphErrors(nCo60,xCo60_1,SSCo60_1,0,SSECo60_1);
  TGraphErrors *grCo60_2_I = new TGraphErrors(nCo60,xCo60_1,ICo60_2,0,IECo60_2);
  TGraphErrors *grCo60_2_S = new TGraphErrors(nCo60,xCo60_1,SCo60_2,0,SECo60_2);
  TGraphErrors *grCo60_2_IS = new TGraphErrors(nCo60,xCo60_1,ISCo60_2,0,ISECo60_2);
  TGraphErrors *grCo60_2_SS = new TGraphErrors(nCo60,xCo60_1,SSCo60_2,0,SSECo60_2);
  TGraphErrors *grCo60_1_A = new TGraphErrors(nCo60,xCo60_1,ACo60_1,0,AECo60_1);
  TGraphErrors *grCo60_1_R = new TGraphErrors(nCo60,xCo60_1,RCo60_1,0,RECo60_1);
  TGraphErrors *grCo60_2_A = new TGraphErrors(nCo60,xCo60_1,ACo60_2,0,AECo60_2);
  TGraphErrors *grCo60_2_R = new TGraphErrors(nCo60,xCo60_1,RCo60_2,0,RECo60_2);
  TGraphErrors *grTh228_I = new TGraphErrors(nTh228,xTh228,ITh228,0,IETh228);
  TGraphErrors *grTh228_S = new TGraphErrors(nTh228,xTh228,STh228,0,SETh228);
  TGraphErrors *grTh228_IS = new TGraphErrors(nTh228,xTh228,ISTh228,0,ISETh228);
  TGraphErrors *grTh228_SS = new TGraphErrors(nTh228,xTh228,SSTh228,0,SSETh228);
  TGraphErrors *grTh228_A = new TGraphErrors(nTh228,xTh228,ATh228,0,AETh228);
  TGraphErrors *grTh228_R = new TGraphErrors(nTh228,xTh228,RTh228,0,RETh228);
  
  grCs137_I->SetTitle("Ionization");
  grCs137_S->SetTitle("Scintillation");
  grCs137_IS->SetTitle("sigma ionization");
  grCs137_SS->SetTitle("sigma scintillation");
  
  grCs137_I->GetXaxis()->SetTitle("run number");
  grCs137_S->GetXaxis()->SetTitle("run number");
  grCs137_IS->GetXaxis()->SetTitle("run number");
  grCs137_SS->GetXaxis()->SetTitle("run number");
  grCs137_A->GetXaxis()->SetTitle("run number");
  grCs137_R->GetXaxis()->SetTitle("run number");
  
  grCs137_I->GetYaxis()->SetTitle("peak position");
  grCs137_S->GetYaxis()->SetTitle("peak position");
  grCs137_IS->GetYaxis()->SetTitle("peak sigma");
  grCs137_SS->GetYaxis()->SetTitle("peak sigma");
  grCs137_A->GetYaxis()->SetTitle("peak position");
  grCs137_R->GetYaxis()->SetTitle("resolution");
  
  grCs137_I->GetYaxis()->SetTitleOffset(1.2);
  grCs137_S->GetYaxis()->SetTitleOffset(1.2);
  grCs137_IS->GetYaxis()->SetTitleOffset(1.2);
  grCs137_SS->GetYaxis()->SetTitleOffset(1.2);
  grCs137_A->GetYaxis()->SetTitleOffset(1.2);
  grCs137_R->GetYaxis()->SetTitleOffset(1.2);
  
  grCo60_1_I->GetXaxis()->SetTitle("run number");
  grCo60_1_S->GetXaxis()->SetTitle("run number");
  grCo60_1_IS->GetXaxis()->SetTitle("run number");
  grCo60_1_SS->GetXaxis()->SetTitle("run number");
  grCo60_1_A->GetXaxis()->SetTitle("run number");
  grCo60_1_R->GetXaxis()->SetTitle("run number");
  
  grCo60_1_I->GetYaxis()->SetTitle("peak position");
  grCo60_1_S->GetYaxis()->SetTitle("peak position");
  grCo60_1_IS->GetYaxis()->SetTitle("peak sigma");
  grCo60_1_SS->GetYaxis()->SetTitle("peak sigma");
  grCo60_1_A->GetYaxis()->SetTitle("peak position");
  grCo60_1_R->GetYaxis()->SetTitle("resolution");
  
  grCo60_1_I->GetYaxis()->SetTitleOffset(1.2);
  grCo60_1_S->GetYaxis()->SetTitleOffset(1.2);
  grCo60_1_IS->GetYaxis()->SetTitleOffset(1.2);
  grCo60_1_SS->GetYaxis()->SetTitleOffset(1.2);
  grCo60_1_A->GetYaxis()->SetTitleOffset(1.2);
  grCo60_1_R->GetYaxis()->SetTitleOffset(1.2);
  
  grCo60_2_I->GetXaxis()->SetTitle("run number");
  grCo60_2_S->GetXaxis()->SetTitle("run number");
  grCo60_2_IS->GetXaxis()->SetTitle("run number");
  grCo60_2_SS->GetXaxis()->SetTitle("run number");
  grCo60_2_A->GetXaxis()->SetTitle("run number");
  grCo60_2_R->GetXaxis()->SetTitle("run number");
  
  grCo60_2_I->GetYaxis()->SetTitle("peak position");
  grCo60_2_S->GetYaxis()->SetTitle("peak position");
  grCo60_2_IS->GetYaxis()->SetTitle("peak sigma");
  grCo60_2_SS->GetYaxis()->SetTitle("peak sigma");
  grCo60_2_A->GetYaxis()->SetTitle("peak position");
  grCo60_2_R->GetYaxis()->SetTitle("resolution");
  
  grCo60_2_I->GetYaxis()->SetTitleOffset(1.2);
  grCo60_2_S->GetYaxis()->SetTitleOffset(1.2);
  grCo60_2_IS->GetYaxis()->SetTitleOffset(1.2);
  grCo60_2_SS->GetYaxis()->SetTitleOffset(1.2);
  grCo60_2_A->GetYaxis()->SetTitleOffset(1.2);
  grCo60_2_R->GetYaxis()->SetTitleOffset(1.2);
  
  grTh228_I->GetXaxis()->SetTitle("run number");
  grTh228_S->GetXaxis()->SetTitle("run number");
  grTh228_IS->GetXaxis()->SetTitle("run number");
  grTh228_SS->GetXaxis()->SetTitle("run number");
  grTh228_A->GetXaxis()->SetTitle("run number");
  grTh228_R->GetXaxis()->SetTitle("run number");
  
  grTh228_I->GetYaxis()->SetTitle("peak position");
  grTh228_S->GetYaxis()->SetTitle("peak position");
  grTh228_IS->GetYaxis()->SetTitle("peak sigma");
  grTh228_SS->GetYaxis()->SetTitle("peak sigma");
  grTh228_A->GetYaxis()->SetTitle("peak position");
  grTh228_R->GetYaxis()->SetTitle("resolution");
  
  grTh228_I->GetYaxis()->SetTitleOffset(1.2);
  grTh228_S->GetYaxis()->SetTitleOffset(1.2);
  grTh228_IS->GetYaxis()->SetTitleOffset(1.2);
  grTh228_SS->GetYaxis()->SetTitleOffset(1.2);
  grTh228_A->GetYaxis()->SetTitleOffset(1.2);
  grTh228_A->GetYaxis()->SetTitleOffset(1.2);
  
  grCs137_I->GetYaxis()->SetRangeUser(500,3000);
  grCs137_S->GetYaxis()->SetRangeUser(1000,10000);
  grCs137_IS->GetYaxis()->SetRangeUser(40,150);
  grCs137_SS->GetYaxis()->SetRangeUser(300,700);
  
  grCs137_I->GetXaxis()->SetLimits(2400,maxRunID+100);
  grCs137_S->GetXaxis()->SetLimits(2400,maxRunID+100);
  grCs137_IS->GetXaxis()->SetLimits(2400,maxRunID+100);
  grCs137_SS->GetXaxis()->SetLimits(2400,maxRunID+100);
  
  grCs137_I->SetMarkerStyle(20);
  grCs137_S->SetMarkerStyle(20);
  grCs137_IS->SetMarkerStyle(20);
  grCs137_SS->SetMarkerStyle(20);
  grCs137_A->SetMarkerStyle(20);
  grCs137_R->SetMarkerStyle(20);
  grCo60_1_I->SetMarkerStyle(22);
  grCo60_1_S->SetMarkerStyle(22);
  grCo60_1_IS->SetMarkerStyle(22);
  grCo60_1_SS->SetMarkerStyle(22);
  grCo60_1_A->SetMarkerStyle(22);
  grCo60_1_R->SetMarkerStyle(22);
  grCo60_2_I->SetMarkerStyle(23);
  grCo60_2_S->SetMarkerStyle(23);
  grCo60_2_IS->SetMarkerStyle(23);
  grCo60_2_SS->SetMarkerStyle(23);
  grCo60_2_A->SetMarkerStyle(23);
  grCo60_2_R->SetMarkerStyle(23);
  grTh228_I->SetMarkerStyle(21);
  grTh228_S->SetMarkerStyle(21);
  grTh228_IS->SetMarkerStyle(21);
  grTh228_SS->SetMarkerStyle(21);
  grTh228_A->SetMarkerStyle(21);
  grTh228_R->SetMarkerStyle(21);
  
  grCs137_I->SetMarkerSize(0.8);
  grCs137_S->SetMarkerSize(0.8);
  grCs137_IS->SetMarkerSize(0.8);
  grCs137_SS->SetMarkerSize(0.8);
  grCs137_A->SetMarkerSize(0.8);
  grCs137_R->SetMarkerSize(0.8);
  grCo60_1_I->SetMarkerSize(0.8);
  grCo60_1_S->SetMarkerSize(0.8);
  grCo60_1_IS->SetMarkerSize(0.8);
  grCo60_1_SS->SetMarkerSize(0.8);
  grCo60_1_A->SetMarkerSize(0.8);
  grCo60_1_R->SetMarkerSize(0.8);
  grCo60_2_I->SetMarkerSize(0.8);
  grCo60_2_S->SetMarkerSize(0.8);
  grCo60_2_IS->SetMarkerSize(0.8);
  grCo60_2_SS->SetMarkerSize(0.8);
  grCo60_2_A->SetMarkerSize(0.8);
  grCo60_2_R->SetMarkerSize(0.8);
  grTh228_I->SetMarkerSize(0.8);
  grTh228_S->SetMarkerSize(0.8);
  grTh228_IS->SetMarkerSize(0.8);
  grTh228_SS->SetMarkerSize(0.8);
  grTh228_A->SetMarkerSize(0.8);
  grTh228_R->SetMarkerSize(0.8);
  
  char Cs137_I_label[50];
  char Cs137_S_label[50];
  char Co60_I1_label[50];
  char Co60_S1_label[50];
  char Co60_I2_label[50];
  char Co60_S2_label[50];
  char Th228_I_label[50];
  char Th228_S_label[50];
  sprintf(Cs137_I_label,"Cs137 (%.2f +- %.2f)",TMath::Mean(nCs137,ICs137),TMath::RMS(nCs137,ICs137));
  sprintf(Cs137_S_label,"Cs137 (%.2f +- %.2f)",TMath::Mean(nCs137,SCs137),TMath::RMS(nCs137,SCs137));
  sprintf(Co60_I1_label,"Co60 (1) (%.2f +- %.2f)",TMath::Mean(nCo60,ICo60_1),TMath::RMS(nCo60,ICo60_1));
  sprintf(Co60_S1_label,"Co60 (1) (%.2f +- %.2f)",TMath::Mean(nCo60,SCo60_1),TMath::RMS(nCo60,SCo60_1));
  sprintf(Co60_I2_label,"Co60 (2) (%.2f +- %.2f)",TMath::Mean(nCo60,ICo60_2),TMath::RMS(nCo60,ICo60_2));
  sprintf(Co60_S2_label,"Co60 (2) (%.2f +- %.2f)",TMath::Mean(nCo60,SCo60_2),TMath::RMS(nCo60,SCo60_2));
  sprintf(Th228_I_label,"Th228 (%.2f +- %.2f)",TMath::Mean(nTh228,ITh228),TMath::RMS(nTh228,ITh228));
  sprintf(Th228_S_label,"Th228 (%.2f +- %.2f)",TMath::Mean(nTh228,STh228),TMath::RMS(nTh228,STh228));
  
  TLegend *l1 = new TLegend(0.65,0.15,0.88,0.4);
  l1->AddEntry(grCs137_I,Cs137_I_label,"lp");
  l1->AddEntry(grCo60_1_I,Co60_I1_label,"lp");
  l1->AddEntry(grCo60_2_I,Co60_I2_label,"lp");
  l1->AddEntry(grTh228_I,Th228_I_label,"lp");
  
  TLegend *l2 = new TLegend(0.65,0.15,0.88,0.4);
  l2->AddEntry(grCs137_S,Cs137_S_label,"lp");
  l2->AddEntry(grCo60_1_S,Co60_S1_label,"lp");
  l2->AddEntry(grCo60_2_S,Co60_S2_label,"lp");
  l2->AddEntry(grTh228_S,Th228_S_label,"lp");
  
  l1->SetFillColor(0);
  l2->SetFillColor(0);
  
  TCanvas *c1 = new TCanvas();
  grCs137_I->Draw("AZP");
  grCo60_1_I->Draw("ZPsame");
  grCo60_2_I->Draw("ZPsame");
  grTh228_I->Draw("ZPsame");
  l1->Draw("same");
  c1->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Ionization.png","RECREATE");
  
  TCanvas *c2 = new TCanvas();
  grCs137_S->Draw("AZP");
  grCo60_1_S->Draw("ZPsame");
  grCo60_2_S->Draw("ZPsame");
  grTh228_S->Draw("ZPsame");
  l2->Draw("same");
  c2->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/Scintillation.png","RECREATE");
  
  TCanvas *c3 = new TCanvas();
  grCs137_IS->Draw("AZP");
  grCo60_1_IS->Draw("ZPsame");
  grCo60_2_IS->Draw("ZPsame");
  grTh228_IS->Draw("ZPsame");
  c3->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/SigmaIonization.png","RECREATE");
  
  TCanvas *c4 = new TCanvas();
  grCs137_SS->Draw("AZP");
  grCo60_1_SS->Draw("ZPsame");
  grCo60_2_SS->Draw("ZPsame");
  grTh228_SS->Draw("ZPsame");
  c4->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/SigmaScintillation.png","RECREATE");
  
  // individual plots
  grCs137_I->GetYaxis()->SetRangeUser(TMath::MinElement(nCs137,ICs137) - 20, TMath::MaxElement(nCs137,ICs137) + 20);
  grCs137_S->GetYaxis()->SetRangeUser(TMath::MinElement(nCs137,SCs137) - 50, TMath::MaxElement(nCs137,SCs137) + 50);
  grCs137_I->GetXaxis()->SetRangeUser(TMath::MinElement(nCs137,xCs137) - 50, TMath::MaxElement(nCs137,xCs137) + 50);
  grCs137_S->GetXaxis()->SetRangeUser(TMath::MinElement(nCs137,xCs137) - 50, TMath::MaxElement(nCs137,xCs137) + 50);
  
  grCs137_I->SetTitle("Cs137");
  grCs137_S->SetTitle("Cs137");
  grCs137_A->SetTitle("Cs137");
  grCs137_R->SetTitle("Cs137");
  grCo60_1_I->SetTitle("Co60 (1)");
  grCo60_1_S->SetTitle("Co60 (1)");
  grCo60_1_A->SetTitle("Co60 (1)");
  grCo60_1_R->SetTitle("Co60 (1)");
  grCo60_2_I->SetTitle("Co60 (2)");
  grCo60_2_S->SetTitle("Co60 (2)");
  grCo60_2_A->SetTitle("Co60 (2)");
  grCo60_2_R->SetTitle("Co60 (2)");
  grTh228_I->SetTitle("Th228");
  grTh228_S->SetTitle("Th228");
  grTh228_A->SetTitle("Th228");
  grTh228_R->SetTitle("Th228");
  
  TCanvas *c5 = new TCanvas();
  c5->Divide(2,2);
  c5->cd(1);
  grCs137_I->Draw("AZP");
  c5->cd(2);
  grCo60_1_I->Draw("AZP");
  c5->cd(3);
  grCo60_2_I->Draw("AZP");
  c5->cd(4);
  grTh228_I->Draw("AZP");
  c5->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/IonizationIndividual.png");
  
  TCanvas *c6 = new TCanvas();
  c6->Divide(2,2);
  c6->cd(1);
  grCs137_S->Draw("AZP");
  c6->cd(2);
  grCo60_1_S->Draw("AZP");
  c6->cd(3);
  grCo60_2_S->Draw("AZP");
  c6->cd(4);
  grTh228_S->Draw("AZP");
  c6->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/ScintillationIndividual.png");
  
  TCanvas *c7 = new TCanvas();
  c7->Divide(2,2);
  c7->cd(1);
  grCs137_A->Draw("AZP");
  c7->cd(2);
  grCo60_1_A->Draw("AZP");
  c7->cd(3);
  grCo60_2_A->Draw("AZP");
  c7->cd(4);
  grTh228_A->Draw("AZP");
  c7->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/AnticorrelationIndividual.png");
  
  TCanvas *c8 = new TCanvas();
  c8->Divide(2,2);
  c8->cd(1);
  grCs137_R->Draw("AZP");
  c8->cd(2);
  grCo60_1_R->Draw("AZP");
  c8->cd(3);
  grCo60_2_R->Draw("AZP");
  c8->cd(4);
  grTh228_R->Draw("AZP");
  c8->SaveAs("/nfs/slac/g/exo/ATeamWIPPData/StabilityPlots/ResolutionIndividual.png");
  
  return;
}

/*double EXOSourceStability::g2(double *x, double *par) 
{
  double A = par[0];
  double x_0 = par[1];
  double y_0 = par[2];
  double sigma_x = par[3];
  double sigma_y = par[4];
  double a = TMath::Cos(par[5])*TMath::Cos(par[5]) / (2*sigma_x*sigma_x) + TMath::Sin(par[5])*TMath::Sin(par[5]) / (2*sigma_y*sigma_y);
  double b = -1.0 * TMath::Sin(2*par[5]) / (4*sigma_x*sigma_x) + TMath::Sin(2*par[5]) / (4*sigma_y*sigma_y);
  double c = TMath::Sin(par[5])*TMath::Sin(par[5]) / (2*sigma_x*sigma_x) + TMath::Cos(par[5])*TMath::Cos(par[5]) / (2*sigma_y*sigma_y);
  
  double val = A*TMath::Exp(-1.0*(a*(x[0] - x_0)*(x[0] - x_0) + 2*b*(x[0] - x_0)*(x[1] - y_0) + c*(x[1] - y_0)*(x[1] - y_0)));
  
  return val;
}

double EXOSourceStability::fun1(double *x, double *par)
{
  return g2(x,par);
}

double EXOSourceStability::fun2(double *x, double *par)
{
  double *p1 = &par[0];
  double *p2 = &par[6];
  double result = g2(x,p1) + g2(x,p2);
  
  return result;
}*/
