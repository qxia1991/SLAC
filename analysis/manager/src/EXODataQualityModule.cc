//______________________________________________________________________________
 
#include "EXOAnalysisManager/EXODataQualityModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TProfile.h"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXODataQualityModule, "dataqual" )

EXODataQualityModule::EXODataQualityModule() :
  fWriteOffline(true),
  fWriteOnline(true),
  fFilename("DataQualityHistograms.root")
{

  //  cout << "constructing EXODataQualityModule with name " << NAME << endl;

}

int EXODataQualityModule::Initialize()
{

  //  cout << "Initializing " << name << endl;

  // Create root file and histograms
  
  f = new TFile(fFilename.c_str(),"recreate");

  // Hit channels
    
  h101 = new TH1F("h101_val_iu1cl","u channels with reconstructed clusters",152,0,152);
  h101->GetXaxis()->SetTitle("channel number");

  // Cluster energy

  h201 = new TH1F("h201_val_eccl","corrected cluster energy (keV)", 500, 0.0, 5000.0 );
  h201->GetXaxis()->SetTitle("energy (keV)");

  h202 = new TH1F("h202_val_ercl","raw cluster energy (keV)", 500, 0.0, 5000.0 );
  h202->GetXaxis()->SetTitle("energy (keV)");

  h203 = new TH1F("h203_val_usig_e","u-signal energy (keV)", 500, 0.0, 5000.0 );
  h203->GetXaxis()->SetTitle("energy (keV)");

  // Cluster locations

  h301 = new TH1F("h301","charge cluster x position", 250, -250, 250 ); 
  h301->GetXaxis()->SetTitle("cluster x (mm)");

  h302 = new TH1F("h302","charge cluster y position", 250, -250, 250 );
  h302->GetXaxis()->SetTitle("cluster y (mm)");
 
  h303 = new TH1F("h303_val_zcl","charge cluster z position", 250, -250, 250 ); 
  h303->GetXaxis()->SetTitle("cluster z (mm)");

  h304 = new TH1F("h304_val_ucl","charge cluster u position", 250, -250, 250 );
  h304->GetXaxis()->SetTitle("cluster u (mm)");
 
  h305 = new TH1F("h305_val_vcl","charge cluster v position", 250, -250, 250 ); 
  h305->GetXaxis()->SetTitle("cluster v (mm)");

  h306 = new TH1F("h306_val_tcl","charge cluster time", 2000, 0, 2000 );
  h306->GetXaxis()->SetTitle("cluster time (microseconds)");
 
  h307 = new TH2F("h307","charge cluster x vs y", 100, -250, 250, 100, -250, 250 );
  h307->GetXaxis()->SetTitle("cluster x (mm)");
  h307->GetYaxis()->SetTitle("cluster y (mm)");

  h308 = new TH2F("h308","charge cluster x vs z", 100, -250, 250, 100, -250, 250 );
  h308->GetXaxis()->SetTitle("cluster x (mm)");
  h308->GetYaxis()->SetTitle("cluster z (mm)");

  h309 = new TH2F("h309","charge cluster y vs z", 100, -250, 250, 100, -250, 250 );
  h309->GetXaxis()->SetTitle("cluster y (mm)");
  h309->GetYaxis()->SetTitle("cluster z (mm)");

  h310 = new TH2F("h310","charge cluster r vs z", 100, 0.0, 250, 100, -250, 250 );
  h310->GetXaxis()->SetTitle("cluster r (mm)");
  h310->GetYaxis()->SetTitle("cluster z (mm)");
  
  // APD info

  h401 = new TH1F("h401_val_c1sc","Reconstructed APD plane 1 counts", 200, 400.0, 20000.0 );
  h401->GetXaxis()->SetTitle("APD plane 1 counts");

  h402 = new TH1F("h402_val_c2sc","Reconstructed APD plane 2 counts", 200, 400.0, 20000.0 );
  h402->GetXaxis()->SetTitle("APD plane 2 counts");

  h403 = new TH1F("h403","Total reconstructed APD counts", 200, 0.0, 20000.0 );
  h403->GetXaxis()->SetTitle("Total APD counts");

  h404 = new TH1F("h404_val_tsc","APD cluster time (microseconds)", 2000, 0.0, 2000.0 );
  h404->GetXaxis()->SetTitle("APD cluster time (microseconds)");

  h405 = new TH2F("h405","charge z vs APD plane ratio", 100, -250, 250, 100, 0.0, 10.0 );
  h405->GetXaxis()->SetTitle("charge z (mm)");
  h405->GetYaxis()->SetTitle("APD plane 1 / APD plane 2");

  // Number of clusters and total energy

  h501 = new TH1F("h501_val_ncl","Number of charge clusters", 10, 0, 10 );
  h501->GetXaxis()->SetTitle("number of charge clusters");

  h502 = new TH1F("h502_val_nsc","Number of APD clusters", 10, 0, 10 );
  h502->GetXaxis()->SetTitle("number of APD clusters");

  h503 = new TH1F("h503_val_ecrec","Total event energy", 500, 100.0, 5000.0 );
  h503->GetXaxis()->SetTitle("energy (keV)");

  h504 = new TH1F("h504_val_errec","Total raw event energy", 500, 100.0, 5000.0 );
  h504->GetXaxis()->SetTitle("energy (keV)");

  h505 = new TH1F("h505_val_nusig","Number of u-signals", 10, 0, 10 );
  h505->GetXaxis()->SetTitle("number of u-signals");

  // Channel signal

  h601 = new TProfile("h601","RMS signal vs all wire channels", 152, 0.0, 152.0, "s" );
  h601->GetXaxis()->SetTitle("channel number");
  h601->GetYaxis()->SetTitle("ADC counts");

  h602 = new TH2F("h602","RMS signal vs all wire channels",152,0.0,152.0,200,0.0,200);
  h602->GetXaxis()->SetTitle("channel number");
  h602->GetYaxis()->SetTitle("ADC counts");

  h603 = new TH2F("h603","log10 RMS signal vs all wire channels",152,0.0,152.0,200,-1,3);
  h603->GetXaxis()->SetTitle("channel number");
  h603->GetYaxis()->SetTitle("log ADC counts");

  h604 = new TProfile("h604","RMS signal vs empty wire channels", 152, 0.0, 152.0, "s" );
  h604->GetXaxis()->SetTitle("channel number");
  h604->GetYaxis()->SetTitle("ADC counts");

  h605 = new TH2F("h605","RMS signal vs empty wire channels",152,0.0,152.0,200,0.0,200);
  h605->GetXaxis()->SetTitle("channel number");
  h605->GetYaxis()->SetTitle("ADC counts");

  h606 = new TH2F("h606","log10 RMS signal vs empty wire channels",152,0.0,152.0,200,-1,3);
  h606->GetXaxis()->SetTitle("channel number");
  h606->GetYaxis()->SetTitle("log ADC counts");



  h611 = new TProfile("h611","RMS signal vs APD channel (all samples)", 74, 152.0, 226.0, "s" );
  h611->GetXaxis()->SetTitle("channel number");
  h611->GetYaxis()->SetTitle("ADC counts");

  h612 = new TH2F("h612","RMS signal vs APD channel (all samples)",74,152.0,226.0,200,0.0,200);
  h612->GetXaxis()->SetTitle("channel number");
  h612->GetYaxis()->SetTitle("ADC counts");

  h613 = new TH2F("h613","log10 RMS signal vs APD channel (all samples)",74,152.0,226.0,200,-1,3);
  h613->GetXaxis()->SetTitle("channel number");
  h613->GetYaxis()->SetTitle("log ADC counts");

  h614 = new TProfile("h614","RMS signal vs APD channel (signal regions removed)", 
		      74, 152.0, 226.0, "s" );
  h614->GetXaxis()->SetTitle("channel number");
  h614->GetYaxis()->SetTitle("ADC counts");

  h615 = new TH2F("h615","RMS signal vs APD channel (signal regions removed)",
		  74,152.0,226.0,200,0.0,200);
  h615->GetXaxis()->SetTitle("channel number");
  h615->GetYaxis()->SetTitle("ADC counts");

  h616 = new TH2F("h616","log10 RMS signal vs APD channel (signal regions removed)",
		  74,152.0,226.0,200,-1,3);
  h616->GetXaxis()->SetTitle("channel number");
  h616->GetYaxis()->SetTitle("log ADC counts");

  // Channel mean values

  h701 = new TProfile("h701","Mean ADC counts vs all wire channels", 
		      152, 0.0, 152.0, "s" );
  h701->GetXaxis()->SetTitle("channel number");
  h701->GetYaxis()->SetTitle("ADC counts");

  h702 = new TH2F("h702","Mean ADC counts vs all wire channels",
		  152,0.0,152.0,128,0.0,4096);
  h702->GetXaxis()->SetTitle("channel number");
  h702->GetYaxis()->SetTitle("ADC counts");

  h703 = new TProfile("h703","Mean ADC counts vs empty wire channels", 
		      152, 0.0, 152.0, "s" );
  h703->GetXaxis()->SetTitle("channel number");
  h703->GetYaxis()->SetTitle("ADC counts");

  h704 = new TH2F("h704","Mean ADC counts vs empty wire channels",
		  152,0.0,152.0,128,0.0,4096);
  h704->GetXaxis()->SetTitle("channel number");
  h704->GetYaxis()->SetTitle("ADC counts");

  h711 = new TProfile("h711","Mean ADC counts vs APD channel (all samples)", 
		      74, 152.0, 226.0, "s" );
  h711->GetXaxis()->SetTitle("channel number");
  h711->GetYaxis()->SetTitle("ADC counts");

  h712 = new TH2F("h712","Mean ADC counts vs APD channel (all samples)",
		  74,152.0,226.0,128,0.0,4096);
  h712->GetXaxis()->SetTitle("channel number");
  h712->GetYaxis()->SetTitle("ADC counts");

  h713 = new TProfile("h713","Mean ADC counts vs APD channel (signal regions removed)", 
		      74, 152.0, 226.0, "s" );
  h713->GetXaxis()->SetTitle("channel number");
  h713->GetYaxis()->SetTitle("ADC counts");

  h714 = new TH2F("h714","Mean ADC counts vs APD channel (signal regions removed)",
		  74,152.0,226.0,128,0.0,4096);
  h714->GetXaxis()->SetTitle("channel number");
  h714->GetYaxis()->SetTitle("ADC counts");

  // Additional histograms for offline validation

  h801 = new TH1F("h801_val_elxei","Ionization energy deposited in the LXE, fMonteCarloData.fTotalIonizationEnergyInLiquidXe", 500, 0, 5000); 
  h801->GetXaxis()->SetTitle("fMonteCarloData.fTotalIonizationEnergyInLiquidXe (keV)"); 

  h802 = new TH1F("h802_val_elxet","Total energy deposited in the LXE, fMonteCarloData.fTotalEnergyInLiquidXe", 500, 0, 5000); 
  h802->GetXaxis()->SetTitle("fMonteCarloData.fTotalEnergyInLiquidXe (keV)"); 

  h803 = new TH1F("h803_val_evcl","Energy in the v channels", 300, 100, 3000); 
  h803->GetXaxis()->SetTitle("evcl (keV)");

  h804 = new TH1F("h804_val_epart","Particle kinetic energy", 500, 100, 5000); 
  h804->GetXaxis()->SetTitle("epart (keV)");

  h805 = new TH1F("h805_val_apd_hits","APD hits MC", 250, 0.0, 500.0 );
  h805->GetXaxis()->SetTitle("APD hits");

  h806 = new TH1F("h806_val_totalph","Total number of photons generated", 500, 0.0, 20000.0 );
  h806->GetXaxis()->SetTitle("Total photons generated");

  return 0;

}

EXOAnalysisModule::EventStatus EXODataQualityModule::ProcessEvent(EXOEventData *ED)
{

  // Loop over all reconstructed clusters and enter the info
  // into the histogram

  for ( unsigned int i = 0; i < ED->GetNumChargeClusters(); i++ ) {

    // First the hit channels

    EXOChargeCluster* cluster = ED->GetChargeCluster(i);
    for(size_t j=0; j<cluster->GetNumUWireSignals(); j++) {
      h101->Fill( cluster->GetUWireSignalChannelAt(j) );
    }
    double totalVWireEnergy = 0.0;
    for(size_t j=0; j<cluster->GetNumVWireSignals(); j++) {
      h101->Fill( cluster->GetVWireSignalChannelAt(j) );
      totalVWireEnergy += cluster->GetVWireSignalAt(j)->fMagnitude;
    }
    h803->Fill( totalVWireEnergy );

    // Cluster energy

    h201->Fill( cluster->fCorrectedEnergy );
    h202->Fill( cluster->fRawEnergy );

    // Position

    h301->Fill( cluster->fX );
    h302->Fill( cluster->fY );
    h303->Fill( cluster->fZ );
    h304->Fill( cluster->fU );
    h305->Fill( cluster->fV );
    h306->Fill( cluster->fCollectionTime/CLHEP::microsecond );

    h307->Fill( cluster->fX , cluster->fY );
    h308->Fill( cluster->fX , cluster->fZ );
    h309->Fill( cluster->fY , cluster->fZ );

    double r = sqrt( cluster->fX*cluster->fX + cluster->fY*cluster->fY );

    h310->Fill( r , cluster->fZ );

  }

  // U-signals

  for ( unsigned int i = 0; i < ED->GetNumUWireSignals(); i++ ) {

    EXOUWireSignal* usignal = ED->GetUWireSignal(i);

    h203->Fill( usignal->fRawEnergy );

  }
    
  // APD reconstruction results

  // First find the z location of the largest charge cluster

  double zlarge = -1000.0;
  double elarge = -1.0;
  for ( unsigned int i = 0; i < ED->GetNumChargeClusters(); i++ ) {
    EXOChargeCluster* cluster = ED->GetChargeCluster(i);
    if ( cluster->fCorrectedEnergy > elarge ) zlarge = cluster->fZ;
  }
				  
  for ( unsigned int i = 0; i < ED->GetNumScintillationClusters(); i++ ) {

    EXOScintillationCluster* scint_cluster = ED->GetScintillationCluster(i);
    double south = scint_cluster->GetCountsOnAPDPlane(EXOMiscUtil::kSouth);
    double north = scint_cluster->GetCountsOnAPDPlane(EXOMiscUtil::kNorth);
    h401->Fill( north );
    h402->Fill( south );
    h403->Fill( north + south );
    h404->Fill( scint_cluster->fTime/CLHEP::microsecond );
    
    double ratio1 = -1000;
    if ( south != 0.0 ) ratio1 = north/south;
    h405->Fill( zlarge, ratio1 );

  }

  // Reconstructed event variables

  h501->Fill( ED->GetNumChargeClusters() );
  h502->Fill( ED->GetNumScintillationClusters() );
  h505->Fill( ED->GetNumUWireSignals() );

  // RMS signal values and mean values on wire channels

  // Loop over wire channels

  for ( size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++ ) {

    const EXOWaveform& wf = *ED->GetWaveformData()->GetWaveform(i);
    if ( wf.fChannel >= 152 ) continue;

    if ( wf.GetLength() <= 1 ) continue;
    int length = wf.GetLength();

    // Initialize RMS and sums

    double RMS = -1.0;
    double mean = 0.0;
    double signal_sum = 0.0;
    double signal_sqr_sum = 0.0;

    // Loop over the digitized values in the signal

    for ( size_t j = 0; j < wf.GetLength(); j++ ) {
      signal_sum += wf[j];
      signal_sqr_sum += wf[j]*wf[j];
    }

    // Calculate RMS and mean
    
    RMS = signal_sqr_sum/(int)(length) - 
      (signal_sum/(int)(length))*(signal_sum/(int)(length));
    RMS = sqrt(RMS);
    
    mean = signal_sum/(int)(length);

    // Do not make an entry if the RMS is invalid (but why would it be???)
    
    if ( RMS < 0.0 ) continue;

    // Fill plots for all wire channels

    Int_t chan = wf.fChannel;
    h601->Fill(chan,RMS);
    h602->Fill(chan,RMS);
    h603->Fill(chan,log10(RMS));

    h701->Fill(chan,mean);
    h702->Fill(chan,mean);

    // Determine if this channel had a signal on it
    
    bool channel_was_hit = false;
    
    for ( unsigned int k = 0; k < ED->GetNumChargeClusters(); k++ ) {

      EXOChargeCluster* cluster = ED->GetChargeCluster(k);
      for(size_t index = 0; index < cluster->GetNumUWireSignals(); index++) {
        if( cluster->GetUWireSignalChannelAt(index) == chan ){
          channel_was_hit = true;
          break;
        }
      }
      if(channel_was_hit){
        break;
      }
      // For the v signals, also veto the neighboring channels to be safe
      for(size_t index = 0; index < cluster->GetNumVWireSignals(); index++) {
        int vchan = cluster->GetVWireSignalChannelAt(index);
        if( vchan == chan || vchan+1 == chan || vchan-1 == chan ){
          channel_was_hit = true;
          break;
        }
      }
      if(channel_was_hit){
        break;
      }
    }

    if ( channel_was_hit == false ) {

      h604->Fill(chan,RMS);
      h605->Fill(chan,RMS);
      h606->Fill(chan,log10(RMS));

      h703->Fill(chan,mean);
      h704->Fill(chan,mean);

    }
      
  } // end loop over wire channels

  // RMS signal values on APD channels

  // Loop over APD channels

  for ( size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++ ) {

    const EXOWaveform& wf = *ED->GetWaveformData()->GetWaveform(i);
    Int_t chan = wf.fChannel;
    if ( chan < 152 || chan >= 226 ) continue;

    if ( wf.GetLength() <= 1 ) continue;

    // Initialize RMS and sums

    double RMS = -1.0;
    double mean = 0.0;
    double signal_sum = 0.0;
    double signal_sqr_sum = 0.0;

    // Loop over the digitized values in the signal

    for ( size_t j = 0; j < wf.GetLength(); j++ ) {
      signal_sum += wf[j]; 
      signal_sqr_sum += wf[j]*wf[j];
    }

    // Calculate RMS and mean
    size_t length = wf.GetLength();
    
    RMS = signal_sqr_sum/(int)(length) - 
      (signal_sum/(int)(length))*(signal_sum/(int)(length));
    RMS = sqrt(RMS);

    mean = signal_sum/(int)(length);
    
    // Do not make an entry if the RMS is invalid (but why would it be???)
    
    if ( RMS < 0.0 ) continue;

    // Fill plots for all wire channels

    h611->Fill(chan,RMS);
    h612->Fill(chan,RMS);
    h613->Fill(chan,log10(RMS));
    
    h711->Fill(chan,mean);
    h712->Fill(chan,mean);

    // Re-do the calculation, cutting out time samples near reconstructed APD clusters

    RMS = -1.0;
    mean = 0.0;
    signal_sum = 0.0;
    signal_sqr_sum = 0.0;
    int sample_count = 0;

    // Loop over the digitized values in the signal

    bool ignore_this_sample = false;

    for ( size_t j = 0; j < length; j++ ) {

      // See if this sample is near a reconstructed APD cluster

      for ( unsigned int k = 0; k < ED->GetNumScintillationClusters(); k++ ) {
	if ( fabs(SAMPLE_TIME*j - ED->GetScintillationCluster(k)->fTime) < 20*SAMPLE_TIME ) ignore_this_sample = true;
      }

      if ( ignore_this_sample == false ) {
	signal_sum += wf[j];
	signal_sqr_sum += wf[j]*wf[j];
	sample_count++;
      }

    }

    if ( sample_count == 0 ) continue;

    // Calculate RMS
    
    RMS = signal_sqr_sum/(int)(sample_count) - 
      (signal_sum/(int)(sample_count))*(signal_sum/(int)(sample_count));
    RMS = sqrt(RMS);

    mean = signal_sum/(int)(sample_count);

    // Do not make an entry if the RMS is invalid (but why would it be???)
    
    if ( RMS < 0.0 ) continue;
    
    h614->Fill(chan,RMS);
    h615->Fill(chan,RMS);
    h616->Fill(chan,log10(RMS));
      
    h713->Fill(chan,mean);
    h714->Fill(chan,mean);

  } // end loop over APD channels

    // Additional code validation histograms


    EXOMonteCarloData& mc_dat = ED->fMonteCarloData;
    h801->Fill( mc_dat.fTotalIonizationEnergyInLiquidXe );
    h802->Fill( mc_dat.fTotalEnergyInLiquidXe );
    h806->Fill( mc_dat.fTotalPhotons );


 for ( size_t i = 0; i < mc_dat.GetNumParticleInformation(); i++ ) {

    h804->Fill( mc_dat.GetParticleInformation(i)->fKineticEnergykeV );

  }

  for ( size_t i = 0; i < mc_dat.GetNumAPDHits(); i++ ) {

    h805->Fill( mc_dat.GetAPDHit(i)->fNumHits + mc_dat.GetAPDHit(i)->fCharge );

  }

  return kOk;
}

int EXODataQualityModule::TalkTo(EXOTalkToManager *talktoManager)
{

  talktoManager->CreateCommand("/data_quality/offline",
                                "write code validation data to root file",
                                this,
                                fWriteOffline,
                                &EXODataQualityModule::SetWriteOffline );
  talktoManager->CreateCommand("/data_quality/online",
                               "write WIPP online data to root file",
                               this,
                               fWriteOnline,
                               &EXODataQualityModule::SetWriteOnline );

  talktoManager->CreateCommand("/data_quality/filename",
                               "set output root filename",
                               this,
                               fFilename,
                               &EXODataQualityModule::SetOutputFilename );


  return 0;
}

int EXODataQualityModule::ShutDown()
{

  f->cd();
  
  if (fWriteOffline == true && fWriteOnline == false)
    {
      h101->Write();
      
      h201->Write();
      h202->Write();
      h203->Write();
      
      h303->Write();
      h304->Write();
      h305->Write();
      h306->Write();
      
      h401->Write();
      h402->Write();
      h404->Write();
      
      h501->Write();
      h502->Write();
      h504->Write();
      h505->Write();
      
      h801->Write();
      h802->Write();
      h803->Write();
      h804->Write();
      h805->Write();
      h806->Write();
      
      f->Close();
    }
  else if (fWriteOffline == false && fWriteOnline == true)
    {
      h101->Write();
      
      h201->Write();
      
      h301->Write();
      h302->Write();
      h303->Write();
      h304->Write();
      h305->Write();
      h306->Write();
      h307->Write();
      h308->Write();
      h309->Write();
      h310->Write();
      
      h401->Write();
      h402->Write();
      h403->Write();
      h404->Write();
      h405->Write();
      
      h501->Write();
      h502->Write();
      h503->Write();
      
      h601->Write();
      h602->Write();
      h603->Write();
      h604->Write();
      h605->Write();
      h606->Write();
      
      h611->Write();
      h612->Write();
      h613->Write();
      h614->Write();
      h615->Write();
      h616->Write();
      
      h701->Write();
      h702->Write();
      h703->Write();
      h704->Write();
      
      h711->Write();
      h712->Write();
      h713->Write();
      h714->Write();
      
      f->Close();
    }
  else if (fWriteOffline == true && fWriteOnline == true)
    {
      h101->Write();
      
      h201->Write();
      h202->Write();
      h203->Write();
      
      h301->Write();
      h302->Write();
      h303->Write();
      h304->Write();
      h305->Write();
      h306->Write();
      h307->Write();
      h308->Write();
      h309->Write();
      h310->Write();
      
      h401->Write();
      h402->Write();
      h403->Write();
      h404->Write();
      h405->Write();
      
      h501->Write();
      h502->Write();
      h503->Write();
      h504->Write();
      h505->Write();
      
      h601->Write();
      h602->Write();
      h603->Write();
      h604->Write();
      h605->Write();
      h606->Write();
      
      h611->Write();
      h612->Write();
      h613->Write();
      h614->Write();
      h615->Write();
      h616->Write();
      
      h701->Write();
      h702->Write();
      h703->Write();
      h704->Write();
      
      h711->Write();
      h712->Write();
      h713->Write();
      h714->Write();
      
      h801->Write();
      h802->Write();
      h803->Write();
      h804->Write();
      h805->Write();
      h806->Write();
      
      f->Close();
    }
  else if (fWriteOffline == false && fWriteOnline == false) {
  
    f->Close();

  }
  
  return 0;
}


