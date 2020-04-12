/////////////////////////////////////////////////
// Resolution method to find electron lifetime

// Makes multiple histograms of charge spectrum, 
// each corrected with a different electron lifetime.
// (These are the ha, haP, and haN.)
// Each histo is fitted to its full energy peak, and a
// resolution is obtained. The resolutions are fitted
// to a parabola and a minimum is found - the lifetime
// corresponding to the minimum is taken as the 
// measured lifetime. 

// Only single-cluster events with scintillation signals
// are included in the spectra, and fiducial cuts are made

// 7/22/11: 
// Implemented ability to scan each TPC half separately
// 08/01/11: Added Uncorrected Charge Cluster Energy Histo
// 08/02/11: Added hstats to track the cuts made
//           Changed parabola fit range to [0.0, 0.0025] to 
//           accomodate higher purity runs. Should change this
//           to two fits, one for the fit range, and a second
//           one to get the parameters.
// 08/07/11: Changed DoFit() to help it automatically find the 
//           best fit region. Seems to work for lifetimes from 170
//           to 1100 us.
// 08/18/11: Changed fit parameters to initialize to early best 
//           guess of minimum. Should improve fit reliability.
// 09/01/11: Added raw and purity-corrected energies (Latter only
//           works if file already has fPurityCorrectedEnergy filled, 
//           meaning lifetime is already found.
//           Improved DoAnalysis() to work on a general charge spectrum.
// 09/06/11: Added ability to make a purity correction and make 
//           purity-corrected raw and grid-corrected spectra.
// 10/03/11: Updated talktomanager create command funtion call and error
//           logger function calls to compile with current build of
//           offline code (build 432). LJK
// 10/05/11: Switched from PLUGIN to MODULE implementation.
//           Removed condition that exits if the min lifetime is at a 
//           boundary, since we might as well still write out the 
//           resgraph in this case. 
// 10/06/11: Extended range of lifetime guesses to negative values to 
//           improve sensitivity to large lifetimes
// 10/11/11: Tightened minimum finding conditions on resolution fit
//           (now the minimum must be > 50% of the average value of the curve)
//           (THIS MAY BE A BAD IDEA)
//           Also, the condition that the minimum had to be within 10% of 
//           the adjacent points was implemented wrong - added an absolute value
//           to account for negative differences. SS 
////////////////////////////////////////////////////////////////////////

#include "EXOAnalysisManager/EXOPurityResModule.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOPurityResModule, "prm")

EXOPurityResModule::EXOPurityResModule()
{

  //  cout << "constructing EXOPurityResModule" << endl;

}


int EXOPurityResModule::Initialize()
{
  
  std::cout << "Initializing resolution purity module" << std::endl;

  gStyle->SetPalette(1); 

  hrcl = new TH1F("hrcl","Raw Charge Cluster Energy (no purity correction)",100 ,0,4000);
  hgcl = new TH1F("hgcl","Grid-Corrected Charge Cluster Energy (no purity correction)",100 ,0,4000);
  hrpcl = new TH1F("hrpcl","Raw Charge Cluster Energy (purity correction)",100 ,0,4000);
  hgpcl = new TH1F("hgpcl","Grid-Corrected Charge Cluster Energy (purity correction)",100 ,0,4000);
  h1 = new TH2F("h1","An Arbitrary Lifetime Correction vs. Drift Time",100,0,120,100,0,4000);
  hapdsum = new TH1F("hapdsum","Sum Scintillation Energy",100,0,6000);
  hapdcl = new TH1F("hapdcl","Single Cluster Scintillation Energy",100,0,5500);
  hstat = new TH1D("hstat","Statistics for Cuts",5 , 0, 5);

  for (Int_t i=0;i<N_GUESSES;i++) {
    //if (sourceInt == 1){
    ha[i] = new TH1F("ha","ha",100,0,4000);
    haP[i] = new TH1F("haP","haP",100,0,4000);
    haN[i]= new TH1F("haN","haN",100,0,4000);
    ha[i]->SetDirectory(0);
    haN[i]->SetDirectory(0);
    haP[i]->SetDirectory(0);
    //invLifetimes[i] = 0.0001 + i*0.0001;
    //invLifetimes[i] = 0.0001 + i*0.00002;
    invLifetimes[i] = -0.001 + i*0.00002;    
    lifetimes[i] = 1.0/invLifetimes[i];
  }
  runNumb = -1; // initialize
  if (lifetime_corr > 0.0){
    std::cout << "Using corrected lifetime " << lifetime_corr << endl;
  }
  else std::cout << "Not using a lifetime to make corrections" << endl;
  return 0;
}

EXOAnalysisModule::EventStatus EXOPurityResModule::ProcessEvent(EXOEventData *ed)
{
  if (ed->fRunNumber != runNumb && runNumb != -1){
    LogEXOMsg("Run number has changed. Stopping processing to avoid mixing runs.", EECritical);
    return kOk;
  }
  if (runNumb == -1){
    runNumb = ed->fRunNumber;   // fill the run number
    runStart = ed->fEventHeader.fTriggerSeconds; // only do this on the first event
  }
  runStop = ed->fEventHeader.fTriggerSeconds; // let this fill each event until the last
  
  ncl = ed->GetNumChargeClusters(); //fNumberOfSites isn't filled
  nsc = ed->GetNumScintillationClusters();
   
  for (int j=0;j<nsc;j++) {
    if (ed->GetScintillationCluster(j)->fTime>1200000) {
      c1sc = ed->GetScintillationCluster(j)->GetCountsOnAPDPlane(EXOMiscUtil::kNorth);
      c2sc = ed->GetScintillationCluster(j)->GetCountsOnAPDPlane(EXOMiscUtil::kSouth);
      if (c1sc>0&&c2sc>0)
	hapdcl->Fill(c1sc+c2sc);
    }
  }
  
  for (int j=0; j<ncl; j++) {
    int multi = 0;
    int scint = 0;
    int scintcl = 0;
    
    for (int k=0; k<ncl; k++) {
      if (fabs(ed->GetChargeCluster(j)->fCollectionTime - ed->GetChargeCluster(k)->fCollectionTime) < 150000 && j!=k)
	{
	  multi++;
	}
    }
    for (int k=0;k<nsc;k++) {
      if (fabs(ed->GetChargeCluster(j)->fCollectionTime - ed->GetScintillationCluster(k)->fTime) < 150000) {
	scint++;
	c1sumsc = ed->GetScintillationCluster(k)->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth);
	c2sumsc = ed->GetScintillationCluster(k)->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth);
      }
    }

///////////////////////////////////////////////////////////////////////////////
//////// FOR DATA /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    if(!driftBool){
      
      dtcl = ed->GetChargeCluster(j)->fDriftTime;
      //if (multi == 0 && scint==1 && pow(ed->GetChargeCluster(j)->fX, 2) + pow(ed->GetChargeCluster(j)->fY,2) < 163*163 && c1sumsc+c2sumsc>200 && fabs(ed->GetChargeCluster(j)->fZ) > 20 && fabs(ed->GetChargeCluster(j)->fZ) < 172 ) {
      ///////////////////// CUTS ///////////////////////////////////////////////
      hstat->Fill(0);
      if (multi == 0){
	hstat->Fill(1);
	if (scint == 1){
	  hstat->Fill(2);
	  if (pow(ed->GetChargeCluster(j)->fX, 2) + pow(ed->GetChargeCluster(j)->fY,2) < 163*163){
	    hstat->Fill(3);
	    if (c1sumsc+c2sumsc>200){
	      hstat->Fill(4);
	      if (fabs(ed->GetChargeCluster(j)->fZ) > 20 && fabs(ed->GetChargeCluster(j)->fZ) < 172){
		hstat->Fill(5);
		////////////////////////////////////////////////////////////////
		ercl = ed->GetChargeCluster(j)->fRawEnergy;
		egcl = ed->GetChargeCluster(j)->fCorrectedEnergy;
		hrcl->Fill(ercl);
		hgcl->Fill(egcl);
		if (lifetime_corr > 0.0){
		  erpcl = ed->GetChargeCluster(j)->fRawEnergy * 
		    exp( (ed->GetChargeCluster(j)->fDriftTime) / ((lifetime_corr)*CLHEP::microsecond) );
		  egpcl = ed->GetChargeCluster(j)->fCorrectedEnergy *
		    exp( (ed->GetChargeCluster(j)->fDriftTime) / ((lifetime_corr)*CLHEP::microsecond) );
		  hrpcl->Fill(erpcl);
		  hgpcl->Fill(egpcl);
		}
		h1->Fill(dtcl/1000,egcl*exp(dtcl/245000));
		hapdsum->Fill(c1sumsc+c2sumsc);
		for (int l=0; l<N_GUESSES; l++) {
		  ha[l]->Fill(egcl*exp(dtcl/(lifetimes[l]*1000)));
		  if (ed->GetChargeCluster(j)->fZ > 0.0){
		    haP[l]->Fill(egcl*exp(dtcl/(lifetimes[l]*1000)));
		  }
		  if (ed->GetChargeCluster(j)->fZ < 0.0){
		    haN[l]->Fill(egcl*exp(dtcl/(lifetimes[l]*1000)));
		  }
		}
	      }
	    }
	  }
	}
      }
      
    }
///////////////////////////////////////////////////////////////////////////////
//////// FOR SIMS /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
   //    not updated with all the latest plots

    if(driftBool){ // remove z-dependent cuts for sims and use fCollection time to get drift time
      dtcl = ed->GetChargeCluster(j)->fCollectionTime - 1024000;
      if (multi == 0 && pow(ed->GetChargeCluster(j)->fX, 2) + pow(ed->GetChargeCluster(j)->fY,2) < 163*163)      // && dtcl < 101000 && dtcl > 12000)
	{
	  egcl = ed->GetChargeCluster(j)->fCorrectedEnergy;
	  hgcl->Fill(egcl);
	  h1->Fill(dtcl/1000,egcl*exp(dtcl/245000));
	  hapdsum->Fill(c1sumsc+c2sumsc);
	  for (int l=0; l<N_GUESSES; l++) {
	    ha[l]->Fill(egcl*exp(dtcl/(lifetimes[l]*1000)));
	  }
	}
    }
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
 
  }
  return kOk;
}

int EXOPurityResModule::ShutDown()
{
  file = new TFile(outfile,"recreate");
  TCanvas *ca = new TCanvas("ca","ca",400,600);

  // Draw common histos
  //TCanvas *c0 = new TCanvas("c0","c0",400,600);
  //hgcl->Draw();
  hgcl->GetXaxis()->SetTitle("Energy (keV)");
  //hgcl->Write();

  //TCanvas *c1 = new TCanvas("c1","c1",400,600);
  //h1->Draw("colz");
  h1->GetXaxis()->SetTitle("Drift Time");
  h1->GetYaxis()->SetTitle("Energy (keV)");
  h1->Write();

  hapdsum->GetXaxis()->SetTitle("Photons");
  hapdsum->Write();
  
  //TCanvas *c2 = new TCanvas("c2","c2",400,600);
  //hapdcl->Draw();
  hapdcl->Write();
  
  //TCanvas *c3 = new TCanvas("c2","c3",400,600);
  //hstat->Draw();
  hstat->Write();
  
  hrcl->GetXaxis()->SetTitle("Energy (keV)");
  //hrcl->Write();

  hrpcl->GetXaxis()->SetTitle("Energy (keV)");
  hgpcl->GetXaxis()->SetTitle("Energy (keV)");
  //hpcl->Write();

  if(sourceInt == 1){
    DoAnalysis_Th(hrcl, -1);
    DoAnalysis_Th(hgcl, -1);
    DoAnalysis_Th(hrpcl, -1);    
    DoAnalysis_Th(hgpcl, -1);

    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Th(ha[i], i);
    }
    DoFit(0);   
    cout << "___________________________________________________" << endl;
    cout << "Both TPC's" << endl;
    cout << "___________________________________________________" << endl;
    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Th(haP[i], i);
    }
    DoFit(1); 
    cout << "___________________________________________________" << endl;
    cout << "Pos TPC" << endl;
    cout << "___________________________________________________" << endl;
    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Th(haN[i], i);    
    }
    DoFit(2);
    cout << "___________________________________________________" << endl;
    cout << "Neg TPC" << endl;
    cout << "___________________________________________________" << endl;
  }
  if(sourceInt == 2){
    DoAnalysis_Co(hrcl, -1);
    DoAnalysis_Co(hgcl, -1);
    DoAnalysis_Co(hrpcl, -1);    
    DoAnalysis_Co(hgpcl, -1);    

    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Co(ha[i],i);
    }
    DoFit(0);
    cout << "___________________________________________________" << endl;
    cout << "Both TPC's" << endl;
    cout << "___________________________________________________" << endl;
    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Co(haP[i], i);
    }
    DoFit(1);
    cout << "___________________________________________________" << endl;
    cout << "Pos TPC" << endl;
    cout << "___________________________________________________" << endl;
    for (Int_t i = 0; i < N_GUESSES; i++){
      DoAnalysis_Co(haN[i], i);
    } 
    DoFit(2);
    cout << "___________________________________________________" << endl;
    cout << "Neg TPC" << endl;
    cout << "___________________________________________________" << endl;
  }
  
  /* Not implemented
  if(sourceInt == 3){
    DoAnalysis_Cs();
    DoFit(0);
    cout << "___________________________________________________" << endl;
    cout << "Both TPC's" << endl;
    cout << "___________________________________________________" << endl;
    DoAnalysis_Cs(ha);
    DoFit(1);
    cout << "___________________________________________________" << endl;
    cout << "Pos TPC" << endl;
    cout << "___________________________________________________" << endl;
    DoAnalysis_Cs(haP);
    DoFit(2);
    cout << "___________________________________________________" << endl;
    cout << "Neg TPC" << endl;
    cout << "___________________________________________________" << endl;
  }
  */

  // Don't write until after the fitting is over.
  hrcl->Write();
  hgcl->Write();
  hrpcl->Write();
  hgpcl->Write();

  file->Close();

  datafile = "/nfs/slac/g/exo/ATeamWIPPData/Purity/Resolution_elife.dat";
  // datafile = "/nfs/slac/g/exo/lkaufman/sep2011_build432/Purity/Resolution_elife.dat";
  //datafile = "./Resolution_elife_dump.dat";
  cout << "Writing to data file "  << datafile << endl;
  // append the data to the file of electron lifetimes
  ofstream lifefile;
  lifefile.open(datafile, ofstream::app);
  lifefile << runNumb << "\t" << runStart << "\t" << runStop << "\t";
  lifefile.precision(4);
  lifefile << bestLife[0] << "\t" << bestLifeErrP[0] << "\t" << bestLifeErrN[0]
	   << "\t" << bestResMin[0] << "\t" << bestResMinErr[0] << "\t" 
	   << sourceInt << "\t" << sourcePos << "\t" <<  bestLife[1] << "\t" 
	   << bestLifeErrP[1] << "\t" << bestLifeErrN[1] << "\t" << bestLife[2] 
	   << "\t" << bestLifeErrP[2] << "\t" << bestLifeErrN[2] << endl; 

  //cout << "/////////////////////////////////////////////////////////////" << endl;
  //cout << "/////////////////////////////////////////////////////////////" << endl;
  //cout << endl;  
  //cout << "Don't forget to add the source position to " << endl 
  //     << datafile << endl;
  //cout << endl;  
  //cout << "/////////////////////////////////////////////////////////////" << endl;
  //cout << "/////////////////////////////////////////////////////////////" << endl;

  return 0; 
}

int EXOPurityResModule::TalkTo(EXOTalkToManager *tm)
{
  tm->CreateCommand("/prm/source", "1: Th, 2: Co, 3: Cs", this, "0", 
		    &EXOPurityResModule::SetSourceType);
  //if (sourceInt == NULL){
  //  LogEXOMsg("Please set the source type.", 1);
  // }
  
  tm->CreateCommand("/prm/position", "Source Pos. (eg. 2 for S2)", this, "-1",
		    &EXOPurityResModule::SetSourcePos);
  
  tm->CreateCommand("/prm/file", "Output File Name", this, "prm_out.root",
		    &EXOPurityResModule::SetOutFile);
  if (outfile == NULL){
    LogEXOMsg("Please enter an output file name.", EEDebug);
  }
  tm->CreateCommand("/prm/drift","0: fDriftTime 1: fCollectionTime",this,"0",
		    &EXOPurityResModule::SetDriftTime);
  //  Make a command to accept manual purity value for making corrections
  double default_life = 0.0;
  //  lifetime_corr = tm->CreateCommand<double>("/prm/lifetime", "Enter e- lifetime in microsec.", default_life);
  tm->CreateCommand("/prm/lifetime", "Enter e- lifetime in microsec.", this, "default_life", &EXOPurityResModule::SetLifetime);
  if ( lifetime_corr < 0 ) {
    LogEXOMsg("No Lifetime value found", EEDebug);
    return -1;
  }
  
  return 0;

}

void EXOPurityResModule::DoAnalysis_Th(TH1F * ha, Int_t i)
{
  
  TF1 *f1 = new TF1("f1","gaus",1500,3500);
  // Fitting to a combination of gaussian and erfc (for the compton shelf) with the same sigma
  TF1 *f2 = new TF1("fit","[0]*(exp(-0.5*pow( (x-[1])/[2], 2 ))) + [3]*([0]*( (TMath::Erfc((x-[1])/(sqrt(2)*[2])))))",2000,3500);
  f2->SetParameter(0,100);
  f2->SetParameter(1,2600);
  f2->SetParameter(2,70);
  f2->SetParLimits(2,50,2000);
  f2->SetParameter(3,0.2);
  f2->SetParLimits(3,0.1,0.5);

  //  ha->Draw();
  f1->SetRange(2000,3500);
  ha->Fit("f1","RQ");
  f1->SetRange(f1->GetParameter(1)*0.8,f1->GetParameter(1)*1.35);
  ha->Fit("f1","RQ");
  f2->SetRange(f1->GetParameter(1)*0.75,f1->GetParameter(1)*1.35);
  f2->SetParameter(0,100);
  f2->SetParameter(1,2600);
  f2->SetParameter(2,200);
  f2->SetParameter(3,0.3);
  ha->Fit("fit","RQ");
  if (i > -1) {
    res[i] = f2->GetParameter(2)/f2->GetParameter(1);
    res_err[i] = sqrt( pow(f2->GetParError(2)/f2->GetParameter(2), 2) + pow(f2->GetParError(1)/f2->GetParameter(1), 2) ) * res[i];
    means[i] = f2->GetParameter(1);
    ha->Write();
    // cout << i << ": Resolution = " << res[i] << "+-" << res_err[i] << endl;
  }
}
 
void EXOPurityResModule::DoAnalysis_Co(TH1F * ha, Int_t i)
{
  TF1 *f1 = new TF1("f1","gaus",900,1500);
  // Fit to two gaussians plus two erfc's (for the compton shelf) with the same sigma
  TF1 *f2 = new TF1("fit","[0]*(exp(-0.5*pow( (x-[1])/[2] , 2 )) + [4]*exp(-0.5*pow( (x-1.136*[1])/[2], 2 ))) + [3]*([0]*((TMath::Erfc((x-[1])/(sqrt(2)*[2]))+[4]*TMath::Erfc((x-1.136*[1])/(sqrt(2)*[2])))))",900,1600);

  f2->SetParameter(0,100);
  f2->SetParameter(1,1170);
  f2->SetParameter(2,70);
  f2->SetParLimits(2,50,2000);
  f2->SetParameter(3,0.2);
  f2->SetParLimits(3,0.1,0.5);
  f2->FixParameter(4,0.75);

  ha->Draw();
  f1->SetRange(900,1700);
  ha->Fit("f1","RQ");
  f1->SetRange(f1->GetParameter(1)*0.9,f1->GetParameter(1)*1.5);
  ha->Fit("f1","RQ");
  f2->SetRange(f1->GetParameter(1)*0.7,f1->GetParameter(1)*1.5);
  f2->SetParameter(0,100);
  f2->SetParameter(1,1170);
  f2->SetParameter(2,200);
  f2->SetParameter(3,0.3);
  ha->Fit("fit","RQ");
  if (i > -1) {
    res[i] = f2->GetParameter(2)/f2->GetParameter(1);
    res_err[i] = sqrt( pow(f2->GetParError(2)/f2->GetParameter(2),2) + 
		       pow(f2->GetParError(1)/f2->GetParameter(1),2) ) * res[i];
    means[i] = f2->GetParameter(1);
    ha->Write();
    //cout << i << ": Resolution = " << res[i] << " " << res_err[i] << endl;
  }
  
}
/*
void EXOPurityResModule::DoAnalysis_Th(TH1F * ha[N_GUESSES])
{
  
  TF1 *f1 = new TF1("f1","gaus",1500,3500);
  // Fitting to a combination of gaussian and erfc (for the compton shelf) with the same sigma
  TF1 *f2 = new TF1("fit","[0]*(exp(-0.5*pow( (x-[1])/[2], 2 ))) + [3]*([0]*( (TMath::Erfc((x-[1])/(sqrt(2)*[2])))))",2000,3500);
  f2->SetParameter(0,100);
  f2->SetParameter(1,2600);
  f2->SetParameter(2,70);
  f2->SetParLimits(2,50,2000);
  f2->SetParameter(3,0.2);
  f2->SetParLimits(3,0.1,0.5);
  
  for (int i=0; i<N_GUESSES; i++) {
    ha[i]->Draw();
    f1->SetRange(2000,3500);
    ha[i]->Fit("f1","RQ");
    f1->SetRange(f1->GetParameter(1)*0.8,f1->GetParameter(1)*1.35);
    ha[i]->Fit("f1","RQ");
    f2->SetRange(f1->GetParameter(1)*0.75,f1->GetParameter(1)*1.35);
    f2->SetParameter(0,100);
    f2->SetParameter(1,2600);
    f2->SetParameter(2,200);
    f2->SetParameter(3,0.3);
    ha[i]->Fit("fit","RQ");
    res[i] = f2->GetParameter(2)/f2->GetParameter(1);
    res_err[i] = sqrt( pow(f2->GetParError(2)/f2->GetParameter(2), 2) + pow(f2->GetParError(1)/f2->GetParameter(1), 2) ) * res[i];
    means[i] = f2->GetParameter(1);
    ha[i]->Write();
    // cout << i << ": Resolution = " << res[i] << "+-" << res_err[i] << endl;
  }

}
 
void EXOPurityResModule::DoAnalysis_Co(TH1F * ha[N_GUESSES])
{
  TF1 *f1 = new TF1("f1","gaus",900,1500);
  // Fit to two gaussians plus two erfc's (for the compton shelf) with the same sigma
  TF1 *f2 = new TF1("fit","[0]*(exp(-0.5*pow( (x-[1])/[2] , 2 )) + [4]*exp(-0.5*pow( (x-1.136*[1])/[2], 2 ))) + [3]*([0]*((TMath::Erfc((x-[1])/(sqrt(2)*[2]))+[4]*TMath::Erfc((x-1.136*[1])/(sqrt(2)*[2])))))",900,1600);

  f2->SetParameter(0,100);
  f2->SetParameter(1,1170);
  f2->SetParameter(2,70);
  f2->SetParLimits(2,50,2000);
  f2->SetParameter(3,0.2);
  f2->SetParLimits(3,0.1,0.5);
  f2->FixParameter(4,0.75);

  for (int i=0; i<N_GUESSES; i++) {
    ha[i]->Draw();
    f1->SetRange(900,1700);
    ha[i]->Fit("f1","RQ");
    f1->SetRange(f1->GetParameter(1)*0.9,f1->GetParameter(1)*1.5);
    ha[i]->Fit("f1","RQ");
    f2->SetRange(f1->GetParameter(1)*0.7,f1->GetParameter(1)*1.5);
    f2->SetParameter(0,100);
    f2->SetParameter(1,1170);
    f2->SetParameter(2,200);
    f2->SetParameter(3,0.3);
    ha[i]->Fit("fit","RQ");
    res[i] = f2->GetParameter(2)/f2->GetParameter(1);
    res_err[i] = sqrt( pow(f2->GetParError(2)/f2->GetParameter(2),2) + 
		       pow(f2->GetParError(1)/f2->GetParameter(1),2) ) * res[i];
    means[i] = f2->GetParameter(1);
    ha[i]->Write();
    //cout << i << ": Resolution = " << res[i] << " " << res_err[i] << endl;
  }

}
*/
int EXOPurityResModule::DoFit(int TPCpart)
{
  // TPCpart: 0 = all, 1 = Pos Z, 2 = Neg Z

  TCanvas *c3 = new TCanvas("c3","c3",400,600);
  TGraphErrors *g1 = new TGraphErrors(N_GUESSES,invLifetimes,res,0,res_err);
  char graphname[200];
  char graphtitle[200];
  sprintf(graphtitle, "Fit to Best Resolution, TPC %d", TPCpart);
  sprintf(graphname, "resgraph%d", TPCpart);
  g1->Draw("A*");
  g1->SetTitle("Fit to Best Resolution");
  g1->GetXaxis()->SetTitle("Inverse Electron Lifetime (1/#mus)");
  g1->GetYaxis()->SetTitle("Fitted Resolution");
  g1->SetName(graphname);
 
  // 07/11 New way of getting error bars
  // Make the minimum of the parabola an explicit parameter of the fit, and let root's fitter 
  // find it and its error bars. 
  // Based on the form of a parabola
  // (y-b) = a(x-c)^2, where c is the position of the minimum 
  // --> y =     (ac^2 + b)         -       (2ac)*x        +      ax^2 

  double invMin1 = 0.0;
  double invMin2 = 0.0;
  double bestInvMin = -1.0;
  double bestInvMinErr = -1.0;
  double resMin = 1000000.;
  double resAvg = 0.0;
  double resSum = 0.0;

  bestLife[TPCpart] = -1.0;
  bestLifeErrP[TPCpart] = -1.0;
  bestLifeErrN[TPCpart] = -1.0;
  bestResMin[TPCpart] = 0.0;
  bestResMinErr[TPCpart] = 0.0;
  
  // find mean value so we can reject values far below the mean
  for (int i = 0; i < N_GUESSES; i++){
    resSum += res[i];
  }
  resAvg = resSum/N_GUESSES;
  for (int i = 0; i < g1->GetMaxSize()-1; i++){
    if (res[i] < resMin){ // Check for minimum
      if (res[i] > 0.0001 && res[i] > 0.5*resAvg){ // only accept if non-negative and is not well below the average 
	if (res_err[i]/res[i] < 0.10 && abs( (res[i] - res[i+1])/res[i] ) < 0.10){ // Less than 10% errors and difference from neighbor
	  invMin1 = invLifetimes[i];
	  resMin = res[i];
	}
      }
    }
  }
  cout << "invMin1: " << invMin1 << endl;
  TF1 * rfit = new TF1("rfit", "[0]*[2]*[2] + [1] - 2*[0]*[2]*x + [0]*x*x" , 0.0, 1.0);
  rfit->SetParameter(0, 6000);
  //  rfit->SetParameter(1, 0.04);
  //rfit->SetParameter(2, 0.001);  
  rfit->SetParameter(1, resMin);
  rfit->SetParameter(2, invMin1);  
  gStyle->SetOptFit(1);
  g1->Fit("rfit","R","", invMin1 - 0.002, invMin1 + 0.002);

  char canname[200];
  int cani = sprintf(canname, "tempfit%d", TPCpart);
  TCanvas * ctempfit = new TCanvas(canname,"");
  g1->Draw("A*");
  ctempfit->Write();
  ctempfit->Close();
  invMin2 = rfit->GetParameter(2);
  cout << "invMin2: " << invMin2 << endl;
  //  g1->Fit("rfit","R","", invMin2 - 0.0015, invMin2 + 0.0015);  
  g1->Fit("rfit","R","", invMin2 - 0.00075, invMin2 + 0.00075);  // 08/19/11
  bestInvMin = rfit->GetParameter(2);
  bestInvMinErr = rfit->GetParError(2);
  bestLife[TPCpart] = 1.0/bestInvMin; 
  bestLifeErrP[TPCpart] = -( bestLife[TPCpart] - 1.0/(bestInvMin - bestInvMinErr) );
  bestLifeErrN[TPCpart] =    bestLife[TPCpart] - 1.0/(bestInvMin + bestInvMinErr) ;
  bestResMin[TPCpart] = rfit->GetParameter(1);
  bestResMinErr[TPCpart] = rfit->GetParError(1);
  
  cout << "-----------------------------------------------------------------" << endl;  
  cout << "Output File Name: " << outfile << endl;
  cout << "Fitted Minimum: " << bestInvMin << "+-" << bestInvMinErr << endl;
  cout << "Best Lifetime: " << bestLife[TPCpart] << " +" << bestLifeErrP[TPCpart]
       << "-" << bestLifeErrN[TPCpart]  << endl;
  cout << "Best Resolution: "<< bestResMin[TPCpart] << "+- " << bestResMinErr[TPCpart] << endl;
  
  g1->Write();

  return 0;

  /*
    char results[200];
    char resresults[200];
    sprintf(results, "Electron Lifetime: %f +%f -%f",
    bestlife, higherr, lowerr);
    sprintf(resresults, "Best Resolution: %f +- %f", 
    res_min, res_minerr);
    TPaveText * tp = new TPaveText(0.20,0.2,0.7,0.4);
    tp->AddText(results);
    tp->AddText(resresults);
    tp->SetName("LifeRes");
    tp->SetFillColor(0); // text is black on white
    tp->SetTextSize(0.04);
    tp->SetTextAlign(12);
    tp->Draw();
    tp->Write();
  */
  
}



  // Original way to get minimum resolution and errors. 
  /*
    TF1 *f3 = new TF1("f3","pol2",0.0025,0.0055);
    g1->Fit("f3","R");
    
    // find minimum of parabola
    
    double min = -1.0*f3->GetParameter(1)/2.0/f3->GetParameter(2);
    double min_value = f3->Eval(min);
    
    cout << "min " << min << " lifetime " << 1/min << " res " << min_value << endl;
    
    //find range
    
    double ll = 10000.0;
    double ul = 0.0;
    double llmean = 0;
    double ulmean = 0;
    
    for (int i=0; i<N_GUESSES; i++) {
    if (res[i] - res_err[i] < min_value && invLifetimes[i] > 0.002 && invLifetimes[i] <0.006) {
    if (lifetimes[i] < ll) {
    ll = lifetimes[i];
    llmean = means[i];
    }
    if (lifetimes[i] > ul) {
    ul = lifetimes[i];
    ulmean = means[i];
    }
    }
    }
    
    cout << "ll " << ll << " ul " << ul << endl; 
    cout << (ll+ul)/2 << "+-" << (ul-ll)/2 << endl;
    cout << "llmean " << llmean << " ulmean " << ulmean << endl; 
    cout << (llmean+ulmean)/2 << "+-" << (ulmean-llmean)/2 << endl;
    
  */

