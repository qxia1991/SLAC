//______________________________________________________________________________
// EXOATeamELifeModule.cc
// Simon Slutsky
// Module to find electron lifetime by 
// correcting data for 450 "guess" lifetimes 
// and identifying which guess has the best
// peak. 
// 12/23/10: Moved zcl cut outside the lifetime loop. 
// 12/23/10: Added break statement to stop cut events from being plotted. 
// 12/23/10: Added EXOAnalysis commands to cut on z
// 12/27/10: Add single cluster histograms
// 12/29/10: Started to add zcl binning
// EDIT: Histograms binned by zcl were completed on 12/29/10
// 1/14/11:  Added flag to use ercl instead of eccl. 
// 1/14/11:  Added feature to only use user-specified channel
// 1/17/11:  Modified to automatically return histograms from 
//           all channels. User-specifiable channel is removed. 
// 1/18/11:  Cleaned up old comments and updated plots names with "ecrec"
//           to the more appropriate "esum".

#include "EXOAnalysisManager/EXOATeamELifeModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"

#include <TGraph.h>
#include <TH1.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <math.h>
#include <iostream>
using namespace std;

#define testnum 10000

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOATeamELifeModule, "aelife" )

EXOATeamELifeModule::EXOATeamELifeModule() :
  fcut(0.0),
  zcut_l(-1.0),
  zcut_u(-1.0),
  pfft_low(0.0),
  pfft_high(0.0),
  ecl_bool(true)
{

  //  cout << "constructing EXOATeamELifeModule with name " << NAME << endl;

}

int EXOATeamELifeModule::Initialize()
{
  // Open root file
  f = new TFile (root_char.c_str(), "RECREATE");

  //  cout << "Initializing " << name << endl;

  // Make sure we've got an error logger and an analysis manager

  // Initialize histograms
  const Int_t maxx = 3000;
  const Int_t maxx_fine = 3000;
  binwidth = 25; // in keV
  const Float_t bins = maxx/binwidth;
  
  for(Int_t i = 0; i< i_life_max; i++){
    hsesum[i] = new TH1F();
    hsecl[i] = new TH1F();
    hsesum[i]->SetBins(int(bins), 0, maxx);
    hsecl[i]->SetBins(int(bins), 0, maxx);
    for (Int_t z = 0; z < 40; z++){
      hsesum_z[z][i] = new TH1F();
      hsesum_z[z][i]->SetBins(int(bins), 0, maxx);
      hsecl_z[z][i] = new TH1F();
      hsecl_z[z][i]->SetBins(int(bins), 0, maxx);
    }
    for (Int_t ch = 0; ch < 114; ch++){
      hsesum_chan[ch][i] = new TH1F();
      hsesum_chan[ch][i]->SetBins(int(bins), 0, maxx);
      hsecl_chan[ch][i] = new TH1F();
      hsecl_chan[ch][i]->SetBins(int(bins), 0, maxx);
    }
  }
  hma = new TH1F(); // needed for ffts //
     
  // Set lifetime guesses (Allow this to be set in EXOAnalysis?)
  char hist_name_esum[100];
  char hist_title_esum[100];
  char hist_name_ecl[100];
  char hist_title_ecl[100];
  char hist_name_esum_z[100];
  char hist_title_esum_z[100];
  char hist_name_ecl_z[100];
  char hist_title_ecl_z[100];
  char hist_name_esum_chan[100];
  char hist_title_esum_chan[100];
  char hist_name_ecl_chan[100];
  char hist_title_ecl_chan[100];
  

  for (Int_t i_life = i_life_min; i_life < i_life_max; i_life++){
    lifetime[i_life] = 1*pow(1.015, i_life+13); //start with 1.21us, go to 986.
    sprintf(hist_name_esum, "h%d_esum", i_life);
    sprintf(hist_title_esum, "h%d_esum: %f microsec. All Channels", 
		       i_life, lifetime[i_life]);
    hsesum[i_life]->SetNameTitle(hist_name_esum, hist_title_esum);
    sprintf(hist_name_ecl, "h%d_ecl", i_life);
    sprintf(hist_title_ecl, "h%d_ecl: %f microsec. All Channels", 
		       i_life, lifetime[i_life]);
    hsecl[i_life]->SetNameTitle(hist_name_ecl, hist_title_ecl);
    for (Int_t z = 0; z < 40; z++){
      sprintf(hist_name_esum_z, "h%d_esum_z_%d", i_life, z);
      sprintf(hist_title_esum_z, "h%d_esum_z_%d: %f microsec. All Channels", 
			 i_life, z, lifetime[i_life]);
      hsesum_z[z][i_life]->SetNameTitle(hist_name_esum_z, hist_title_esum_z);
      sprintf(hist_name_ecl_z, "h%d_ecl_z_%d", i_life, z);
      sprintf(hist_title_ecl_z, "h%d_ecl_z_%d: %f microsec. All Channels", 
			 i_life, z, lifetime[i_life]);
      hsecl_z[z][i_life]->SetNameTitle(hist_name_ecl_z, hist_title_ecl_z);
    }
    for (Int_t ch = 0; ch < 114; ch++){
      sprintf(hist_name_esum_chan, "h%d_esum_chan_%d", i_life, ch);
      sprintf(hist_title_esum_chan, "h%d_esum_chan_%d: %f microsec. Channel %d", 
			 i_life, ch, lifetime[i_life], ch);
      hsesum_chan[ch][i_life]->SetNameTitle(hist_name_esum_chan, hist_title_esum_chan);
      
      sprintf(hist_name_ecl_chan, "h%d_ecl_chan_%d", i_life, ch);
      sprintf(hist_title_ecl_chan, "h%d_ecl_chan_%d: %f microsec. Channel %d", 
			 i_life, ch, lifetime[i_life], ch);
      hsecl_chan[ch][i_life]->SetNameTitle(hist_name_ecl_chan, hist_title_ecl_chan);
    }
  } //
  return 0;
}

EXOAnalysisModule::EventStatus EXOATeamELifeModule::BeginOfRun(EXOEventData *ED)
{
  cout << "At BeginOfRun for " << GetName() << ", run number = " 
       << ED->fRunNumber << endl;

  if (ecl_bool) { 
    cout << "Using corrected cluster energies." << endl;
  }
  if (!ecl_bool){ 
    cout << "Using raw cluster energies." << endl;
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOATeamELifeModule::ProcessEvent(EXOEventData *ED)
{
  if (ED->GetNumChargeClusters() < 1 || ED->GetChargeCluster(0)->fDriftTime < 0.001){
    if (ED->fEventNumber == 1){
      cout << "******************************" << endl;
      cout << "dtcl is probably not defined."  << endl; 
      cout << "******************************" << endl;
    } 
  }
  // Detector edge cut. Set flag to mark cut events. 
  Bool_t bzcut = 0; 
  if (zcut_u > 0) {
    for (size_t icl = 0; icl < ED->GetNumChargeClusters(); icl++){ 
      if (fabs(ED->GetChargeCluster(icl)->fZ) > zcut_u){
	bzcut = 1;
	break;
      }
    }
  }
  if (bzcut == 0) {
    if (zcut_l > 0) {
      for (size_t icl = 0; icl < ED->GetNumChargeClusters(); icl++){ 
	if (fabs(ED->GetChargeCluster(icl)->fZ) < zcut_l){
	  bzcut = 1;
	  break;
	}
      }
    }
  }
  Int_t chan = 0; // holder for channel of signal
  if (bzcut == 0){
    // Loop over lifetime guesses
    for (Int_t i_life = i_life_min; i_life < i_life_max; i_life++) {
      ecevent = 0;  //reinitialize
      //Apply the guessed electron lifetime by summing over all clusters
      for (size_t icl = 0; icl < ED->GetNumChargeClusters(); icl++) {
        EXOChargeCluster* charge_cluster = ED->GetChargeCluster(icl);
	if (charge_cluster->fDriftTime < 0.001){
	  //Some old files don't have the dtcl variable. If so, 
	  //define start time of event to be time of first scint. cluster
	  t0 = 0;
          if (ED->GetNumScintillationClusters() > 0) t0 = ED->GetScintillationCluster(0)->fTime;
	  exp_arg = ( charge_cluster->fCollectionTime - t0 ) / (lifetime[i_life] * 1000);
	  // (tcl and t0 defined in nanosec)
	}      
	else{ 
	  exp_arg = ( charge_cluster->fDriftTime ) / (lifetime[i_life] * 1000);
	  //lifetime defined in microsec, so *1000 to convert to ns.
	  //cout << "exp_arg = " << exp_arg << endl;
	}
	// skip the event if the drift time for a cluster is too small
	// Print out a message the first time this happens
	if (exp_arg > 700){
	  //cout << "Skipping event #" << ED->fEventNumber << "." << endl;
	  //cout << "Drift time too small for cluster " << icl << "." << endl;
	  break;
	}
	
	// Choose raw or corrected cluster energy, based on user input
	Double_t e_clust = 0;
	if (ecl_bool) { 
	  e_clust = charge_cluster->fCorrectedEnergy; 
	}
	if (!ecl_bool){ 
	  e_clust = charge_cluster->fRawEnergy;
	}
	
	// add term for this cluster to total event energy
	ecevent = ecevent + e_clust*exp(exp_arg);  
	
        chan = -1;
        if (charge_cluster->GetNumUWireSignals() > 0) {
	  chan = charge_cluster->GetUWireSignalChannelAt(0);
        }
	// While we're here, fill the single cluster histograms. 
	hsecl[i_life]->Fill(e_clust*exp(exp_arg));
	// Fill the channel separated histograms
	if(chan >= 0 && chan <=113){
	  hsecl_chan[chan][i_life]->Fill(e_clust*exp(exp_arg));
	}
	for (Int_t z = 0; z < 40; z++){
	  if(-200 + z*10 < ED->GetChargeCluster(0)->fZ && -190 + z*10 > ED->GetChargeCluster(0)->fZ){
	    hsecl_z[z][i_life]->Fill(e_clust*exp(exp_arg));
	    if(chan >= 0 && chan <=113){
	    }
	  }
	}
	
      }
      hsesum[i_life]->Fill(ecevent);
      hsesum[i_life]->SetBinContent(1, 0); // Empty the O keV bin
      if (ED->GetNumChargeClusters() == 1 && 
          ED->GetChargeCluster(0)->GetNumUWireSignals() > 0){
	//Write out special histos for channel-by-channel or z slices
	//only if single site.
        EXOChargeCluster* charge_cluster = ED->GetChargeCluster(0);
	hsesum_chan[charge_cluster->GetUWireSignalChannelAt(0)][i_life]->Fill(ecevent);
	for (Int_t z = 0; z < 40; z++){
	  if(-200 + z*10 < charge_cluster->fZ && -190 + z*10 > charge_cluster->fZ){
	    hsesum_z[z][i_life]->Fill(ecevent);
	  }
	}
      }
    }
  }

  return kOk;
}

EXOAnalysisModule::EventStatus EXOATeamELifeModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}

int EXOATeamELifeModule::TalkTo(EXOTalkToManager *talktoManager)
{
  //  command to read in iso_char
    
  talktoManager->CreateCommand("/aelife/source",
                               "specify source (Co60, Cs137, Th228)", 
                               this, "Co60", &EXOATeamELifeModule::SetSource );

  //  specify output path
    
  talktoManager->CreateCommand("/aelife/output","name and path of output file", 
                               this, "./aelifeoutput.root", &EXOATeamELifeModule::SetRootFile );
    
  //cut command  
  
  talktoManager->CreateCommand("/aelife/cut","Cut below energy (kev)",
                               this, fcut, &EXOATeamELifeModule::Setfcut );
 
  //Cut on z-position (upper)  
  
  talktoManager->CreateCommand("/aelife/zcut_u","Cut if abs(zcl) greater than (mm)",
                               this, zcut_u, &EXOATeamELifeModule::Setzcut_u );
  //Cut on z-position (lower)
  
  talktoManager->CreateCommand("/aelife/zcut_l","Cut if abs(zcl) less than (mm)",
                               this, zcut_l, &EXOATeamELifeModule::Setzcut_l );

  // Set default fft bounds for isotope 
  default_low = 0.0;
  default_high = 0.0;
  if (iso_char == "Co60"){                   // Co-60
    default_low = 0.0035; default_high = 0.008;
  }
  else if (iso_char == "Cs137"){                   // Cs-137
    default_low = 0.008; default_high = 0.02;
  }
  else if (iso_char == "Th228"){                   // Th-228
    default_low = 0.001; default_high = 0.005;
  }
  else {
    LogEXOMsg("Unknown isotope specified; cannot proceed.", EEWarning);
  } //

  //User defined FFT range low limit  
  
  talktoManager->CreateCommand("/aelife/rangelow","FFT range low limit",
                               this, default_low, &EXOATeamELifeModule::Setpfft_low );
 
  //User defined FFT range high limit  

  talktoManager->CreateCommand("/aelife/rangehigh","FFT range low limit",
                               this, default_high, &EXOATeamELifeModule::Setpfft_high );
  // Command to toggle between ercl and eccl (Default to eccl)

  talktoManager->CreateCommand("/aelife/corr","0: use ercl (raw), 1: use eccl (corr)",
                               this, ecl_bool, &EXOATeamELifeModule::Setecl_bool );
 
  return 0;
}


int EXOATeamELifeModule::ShutDown()
{
  f->cd(); //Mysteriously allows output to root file to work

  cout << "At ShutDown for " << GetName() << endl;

  // Dereference fft limit pointers.
  fft_low = pfft_low;
  fft_high = pfft_high;

  cout << "FFT range Low = " << fft_low << endl;
  cout << "FFT range High = " << fft_high << endl;
 
  //Write out uncut energy histos
  for (Int_t ican = 0; ican < i_life_max; ican++){
    hsesum[ican]->Write();
    hsecl[ican]->Write();
    for (Int_t zcan = 0; zcan < 40; zcan++){
      hsesum_z[zcan][ican]->Write();
      hsecl_z[zcan][ican]->Write();
    }
    for (Int_t chcan = 0; chcan < 114; chcan++){
      hsesum_chan[chcan][ican]->Write();
      hsecl_chan[chcan][ican]->Write();
    }
  }
  // Should we write out the cut histos as well?
  // Prob suffic to write out cut energy
  
  // Cut off histograms  below user-specified energy (fcut)
  Int_t nbins_cut = hsesum[0]->GetNbinsX();
  bin_cut = int( nbins_cut * ( (fcut) /
			       (hsesum[0]->GetBinCenter(nbins_cut) +
				0.5*hsesum[0]->GetBinWidth(nbins_cut) ) ) );
  cout << "bin_cut = " << bin_cut << endl;
  for (Int_t i = i_life_min; i < i_life_max; i++){
    for (Int_t j = 0; j < bin_cut; j++){
      hsesum[i]->SetBinContent( j, hsesum[i]->GetBinContent(bin_cut) );
    }
  }
  
  //Find FFT's and figure of merit.
  // First make just 0th plot to get parameters
  // convert integration region to bin numbers
  hfft[0] = new TH1F();
  hfft[0]->SetNameTitle("h0_fft", "h0: 1.2 us");
  fft(hsesum[0]);
  nbins = hma->GetNbinsX();
  h_low = float(hma->GetBinLowEdge(0));
  h_high = float(hma->GetBinLowEdge(nbins) + hma->GetBinWidth(1));
  hfft[0]->SetBins(nbins, h_low, h_high);
  for (Int_t p = 0; p < nbins; p++){
    hfft[0]->SetBinContent(p, hma->GetBinContent(p));
  }
  bin_low = int( nbins * (fft_low - h_low) / (h_high - h_low) );
  bin_high = int( nbins * (fft_high - h_low) / (h_high - h_low) );
  hf_power[0] = hfft[0]->Integral(bin_low, bin_high);
  hfft[0]->Write();
  char hist_name[100];
  char hist_title[100];
  for(Int_t i = 1; i < i_life_max; i++){
    sprintf(hist_name, "h%d_fft", i);
    sprintf(hist_title, "h%d: %f microsec. All Channels",
		        i, lifetime[i]);
    //sprintf(hist_title, "h%d: %f microsec. Channel %d",
    //	   i, lifetime[i], *chan);
    hfft[i] = new TH1F();
    hfft[i]->SetNameTitle(hist_name, hist_title);
    hfft[i]->SetBins(nbins, h_low, h_high);
    
    fft(hsesum[i]);
    for (Int_t p = 0; p < nbins; p++){
      hfft[i]->SetBinContent(p, hma->GetBinContent(p));
    }
    hf_power[i] = hfft[i]->Integral(bin_low, bin_high);
    hfft[i]->Write();
  }
  
  TGraph * fft_graph = new TGraph(i_life_max, lifetime, hf_power);
  fft_graph->SetName("fft_graph");
  Char_t fftgt[100];
  sprintf(fftgt, "FFT Power between %f and %f", fft_low, fft_high); 
  fft_graph->SetTitle(fftgt);
  fft_graph->GetXaxis()->SetTitle("electron lifetime guess (microseconds)");
  fft_graph->Draw("A*");
  fft_graph->Write();
  
  // const int ffth_max = 10*int(lifetime[i_life_max]/10);
  TH1F * fft_hist = new TH1F("fft_hist", "High Frequency Power",
			     1000000, 0, 1000);
  for (Int_t i = 0; i < i_life_max; i++){
    int lifebin = int( lifetime[i]*1000 ); 
    fft_hist->SetBinContent(lifebin, hf_power[i]);
  }
  fft_hist->GetXaxis()->SetTitle("electron lifetime guess (microseconds)");
  fft_hist->Write();
  f->Close();

  return 0;
}



//FFT Accessory functions
void EXOATeamELifeModule::fft( TH1 * h1 ){

  if ( h1 == NULL ) {
    cout << "histogram is empty? returning dummy histo." << endl;
    return;
  }

  Int_t nbinsraw = h1->GetNbinsX();
  // pad the data with zeros, to make the number of data points a
  // simple power of 2, for the benefit of realft().

    Int_t nbins = 1;
    while ( nbins < nbinsraw ) nbins *= 2;
    //cout << "nbins after padding = " << nbins << endl;

    float *fn = new float[nbins];
    for ( Int_t i = 0; i < nbins; i++ ) fn[i] = 0.0;

    for ( Int_t i = 0; i < nbinsraw; i++ ) {
      fn[i] = h1->GetBinContent(i+1);
    }

    EXOMiscUtil::realft( fn-1, nbins, 1 );

    TH1F *htest;

    htest = (TH1F*)gROOT->FindObject("hre");
    if ( htest != NULL ) htest->Delete();
    htest = (TH1F*)gROOT->FindObject("him");
    if ( htest != NULL ) htest->Delete();

    htest = (TH1F*)gROOT->FindObject("hma");
    if ( htest != NULL ) htest->Delete();
    
    Double_t delta = h1->GetBinWidth(0); // bin width in keV
    //cout << "Base histo bin width = " << delta << endl;
    TH1F *hre = new TH1F("hre","",nbins+1,-1.0*(0.5+1.0/nbins)/delta,
			 (0.5+1.0/nbins)/delta);
    TH1F *him = new TH1F("him","",nbins+1,-1.0*(0.5+1.0/nbins)/delta,
			 (0.5+1.0/nbins)/delta);
    hma->SetNameTitle("hma","");
    hma->SetBins(nbins+1,-1.0*(0.5+1.0/nbins)/delta,(0.5+1.0/nbins)/delta);

    for ( Int_t i = 0; i < nbins/2; i++ ) {
      if ( i == 0 ) {
	
	// zero frequency	
	hre->SetBinContent(1+nbins/2,fn[0]);
	him->SetBinContent(1+nbins/2,0.0);
	hma->SetBinContent(1+nbins/2,fabs(fn[0]));

	// highest positive frequency 
	hre->SetBinContent(1+nbins,fn[1]);
	him->SetBinContent(1+nbins,0.0);
	hma->SetBinContent(1+nbins,fabs(fn[1]));

	// lowest negative frequency is the same
	hre->SetBinContent(1,fn[1]);
	him->SetBinContent(1,0.0);
	hma->SetBinContent(1,fabs(fn[1]));
	
      }
      else {

	// positive frequency
	hre->SetBinContent(1+nbins/2+i,fn[2*i]);
	him->SetBinContent(1+nbins/2+i,fn[2*i+1]);
	hma->SetBinContent(1+nbins/2+i,sqrt(fn[2*i]*fn[2*i] + 
					    fn[2*i+1]*fn[2*i+1]));

	// negative frequency is the complex conjugate
	hre->SetBinContent(1+nbins/2-i,fn[2*i]);
	him->SetBinContent(1+nbins/2-i,-1.0*fn[2*i+1]);
	hma->SetBinContent(1+nbins/2-i,sqrt(fn[2*i]*fn[2*i] + 
					    fn[2*i+1]*fn[2*i+1]));
      }
    }
    //cout << "fn[0] = " << fn[0] << endl;
    //cout << "fn[1] = " << fn[1] << endl;
    
    EXOMiscUtil::realft( fn-1, nbins, -1 );
    
    htest = (TH1F*)gROOT->FindObject("hret");
    if ( htest != NULL ) htest->Delete();

    TH1F *hret = new TH1F("hret","",nbins,0.0,nbins);
    for ( Int_t i = 0; i < nbins; i++ ) hret->SetBinContent(i+1,fn[i]);
    
    delete [] fn;
    return;
}

