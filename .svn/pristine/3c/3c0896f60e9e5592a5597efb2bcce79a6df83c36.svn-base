//______________________________________________________________________________
#include "EXOAnalysisManager/EXODataTrimModule.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOEventData.hh"
#include <iostream>
using std::cout;
using std::endl;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXODataTrimModule, "trim" )

EXOAnalysisModule::EventStatus EXODataTrimModule::ProcessEvent(EXOEventData *ED)
{
	      
  // Veto events with no clusters at all

  if ( ED->GetNumScintillationClusters() == 0 && ED->GetNumChargeClusters() == 0 ) {

    ED->GetWaveformData()->Clear("C");
    ED->fEventHeader.fSampleCount = 0;

    cout << "no clusters found, trimming event data entirely" << endl;

    return kOk;

  }

  // Otherwise find the earliest charge cluster

  int trimT0 = 99999999;
  int trimT1 = 99999999;
	
  for ( size_t i = 0; i < ED->GetNumChargeClusters(); i++ ) {
    if ( (int)(ED->GetChargeCluster(i)->fCollectionTime/CLHEP::microsecond) < trimT0 ) {
      trimT0 = (int)(ED->GetChargeCluster(i)->fCollectionTime/CLHEP::microsecond);
    }
  }
  
  for ( size_t i = 0; i < ED->GetNumScintillationClusters(); i++ ) {
    if ( (int)(ED->GetScintillationCluster(i)->fTime/CLHEP::microsecond) < trimT0 ) {
      trimT0 = (int)(ED->GetScintillationCluster(i)->fTime/CLHEP::microsecond);
    }
  }
  
  // Calculate the time window

  int nsample = ED->GetWaveformData()->fNumSamples;
  trimT0 = trimT0 - 100;
  if ( trimT0 < 0 ) trimT0 = 0;
  if ( trimT0 >= nsample ) trimT0 = nsample - 1; 
  trimT1 = trimT0 + 200;
  
  if ( trimT1 >= nsample ) trimT1 = nsample - 1;

  int trimmed_nsample = trimT1 - trimT0 + 1;

  // Initialize list of channels to write out.

  for ( size_t i = 0; i < NUMBER_READOUT_CHANNELS; i++ ) keepit[i] = false;

  // Figure out which channels should be kept

  // Keep all wire channels that have signals

  for ( size_t i = 0; i < ED->GetNumChargeClusters(); i++ ) {
    EXOChargeCluster* cluster = ED->GetChargeCluster(i);
    for(size_t j = 0; j < cluster->GetNumUWireSignals(); j++) {
      keepit[ cluster->GetUWireSignalChannelAt(j) ] = true;
    } 
    for(size_t j = 0; j < cluster->GetNumVWireSignals(); j++) {
      keepit[ cluster->GetVWireSignalChannelAt(j) ] = true;
    } 
  }

  // Keep all neighboring channels also

  for ( size_t i = 0; i < NUMBER_READOUT_CHANNELS; i++ ) neighbor[i] = false;

  for ( int i = 0; i < NCHANNEL_PER_WIREPLANE*NWIREPLANE; i++ ) {
    
    if ( i > 0 && i < NCHANNEL_PER_WIREPLANE*NWIREPLANE - 1 ) {
      if ( keepit[i-1] == true || keepit[i+1] == true ) neighbor[i] = true;
    }

    if ( i == 0 ) {
      if ( keepit[i+1] == true ) neighbor[i] = true;
    }

    if ( i == NCHANNEL_PER_WIREPLANE*NWIREPLANE - 1 ) {
      if ( keepit[i-1] == true ) neighbor[i] = true;
    }

  }

  for ( size_t i = 0; i < NUMBER_READOUT_CHANNELS; i++ ) {
    if ( neighbor[i] == true ) keepit[i] = true;
  }

  // Keep all APD channels

  for ( size_t i = NCHANNEL_PER_WIREPLANE*NWIREPLANE; 
	i < NUMBER_READOUT_CHANNELS; i++ ) keepit[i] = true;
  
  // Loop over all channels, and keep only sampled data between
  // trimT1 and trimT2

  for ( size_t i = 0; i < ED->GetWaveformData()->GetNumWaveforms(); i++ ) {

    EXOWaveform& wf = *ED->GetWaveformData()->GetWaveformToEdit(i);

    // Get rid of the summed APD signals for now
    if ( wf.fChannel >= (int)NUMBER_READOUT_CHANNELS or wf.fChannel < 0 or !keepit[wf.fChannel] ) {
        ED->GetWaveformData()->Remove(&wf);
        continue;
    } 

    // Copy the trimmed signal

    std::vector<Int_t>& the_data = wf.GetVectorData();
    the_data.erase(the_data.begin(), the_data.begin() + trimT0); 
    the_data.erase(the_data.begin()+(trimT1-trimT0), 
                   the_data.end()); 

  }
  // Set GetWaveformData()->GetNumWaveforms(), nsample, and nele

  ED->fEventHeader.fSampleCount = trimmed_nsample - 1;
  ED->GetWaveformData()->fNumSamples = trimmed_nsample;

  return kOk;
}
