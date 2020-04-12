//______________________________________________________________________________
//
// This module loops over single-site 2-wire charge clusters, sums both wire
// waveforms and fits the summed waveform to extract the deposited energy.
//
// !!! The fRawEnergy and fCorrectedEnergy variables of the charge cluster are
// overwritten!!!
//
// The module requires fCorrectedEnergy to be filled, so it should run after
// the gain correction module.
// Also it should run before the purity correction module.
//
//______________________________________________________________________________
#include "EXOAnalysisManager/EXOSummedWaveformFitter.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOReconstruction/EXOUWireSignalModelBuilder.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignalCollection.hh"
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXOSignal.hh"
#include <iostream>

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOSummedWaveformFitter, "sumfitter" )

EXOSummedWaveformFitter::EXOSummedWaveformFitter()
: fFilename("SummedFitterOutput.root"),
  fElectronicsDatabaseFlavor("measured_times"),
  fUWireDatabaseFlavor("source_calibration"),
  fClusteringTime(3.5*SAMPLE_TIME),
  fUWireScalingFactor(ADC_FULL_SCALE_ELECTRONS_WIRE * W_VALUE_LXE_EV_PER_ELECTRON /(CLHEP::keV * ADC_BITS))
{ }

int EXOSummedWaveformFitter::Initialize()
{
  fSignalModelManager.AddRegisteredObject(&fSignalFitter);

  return 0;
}

EXOAnalysisModule::EventStatus EXOSummedWaveformFitter::ProcessEvent(EXOEventData *ED)
{
  EXOWaveformData* wfd = ED->GetWaveformData();
  wfd->Decompress();

  //Shaping times database
  const EXOElectronicsShapers* electronicsShapers = GetCalibrationFor(
      EXOElectronicsShapers,
      EXOElectronicsShapersHandler,
      fElectronicsDatabaseFlavor,
      ED->fEventHeader);

  //U-wire gains database
  const EXOUWireGains* uwireGains = GetCalibrationFor(
      EXOUWireGains,
      EXOUWireGainsHandler,
      fUWireDatabaseFlavor,
      ED->fEventHeader);

  //Loop over relevant clusters
  int tag = 0;
  size_t nsc = ED->GetNumScintillationClusters();
  for(size_t i=0; i<nsc; i++){
    EXOScintillationCluster* sc = ED->GetScintillationCluster(i);
    // Only "single site" charge clusters
    if(sc->GetNumChargeClusters() != 1){
      continue;
    }
    EXOChargeCluster* cc = sc->GetChargeClusterAt(0);
    // Only 2-wire charge clusters
    if(cc->GetNumUWireSignals() != 2){
      continue;
    }

    tag--;
    
    //Get the two channels and corresponding energies
    int channel1 = cc->GetUWireSignalAt(0)->fChannel;
    int channel2 = cc->GetUWireSignalAt(1)->fChannel;
    double energy1 = cc->GetUWireSignalAt(0)->fCorrectedEnergy;
    double energy2 = cc->GetUWireSignalAt(1)->fCorrectedEnergy;
    double energysum = energy1 + energy2;
    // we divide by energysum
    if(energysum <= 0){
      continue;
    }

    //Get shaping times and gains for these channels
    const EXOTransferFunction& tf1 = electronicsShapers->GetTransferFunctionForChannel(channel1);
    const EXOTransferFunction& tf2 = electronicsShapers->GetTransferFunctionForChannel(channel2);
    double gain1 = uwireGains->GetGainOnChannel(channel1) / 300.;
    double gain2 = uwireGains->GetGainOnChannel(channel2) / 300.;
    

    //Construct "average" shaping times. We assume that both channels have the same number of
    //differentiators and integrators.
    EXOTransferFunction tf;
    for(size_t j=0; j<tf1.GetNumDiffStages(); j++){
      double difftime = (energy1 * tf1.GetDiffTime(j) + energy2 * tf2.GetDiffTime(j)) / energysum;
      tf.AddDiffStageWithTime(difftime);
    }
    for(size_t j=0; j<tf1.GetNumIntegStages(); j++){
      double integtime = (energy1 * tf1.GetIntegTime(j) + energy2 * tf2.GetIntegTime(j)) / energysum;
      tf.AddIntegStageWithTime(integtime);
    }

    //Sum the two waveforms
    EXOWaveform wf;
    wf.MakeSimilarTo(*wfd->GetWaveformWithChannel(channel1));
    wf.Zero();
    wf += *(wfd->GetWaveformWithChannel(channel1));
    wf += *(wfd->GetWaveformWithChannel(channel2));
    wf.fChannel = tag;

    //Build signal model and process list
    fSignalModelManager.BuildSignalModelForChannelOrTag(tag, EXOUWireSignalModelBuilder(tf));
    EXOReconProcessList processList;
    processList.Add(wf,EXOReconUtil::kUWire);

    //Create singals based on the charge cluster data.
    EXOSignal Signal;
    Signal.fMagnitude = cc->fRawEnergy;
    Signal.fTime = cc->fCollectionTime;
    EXOChannelSignals ChannelSignals;
    ChannelSignals.SetChannel(tag);
    ChannelSignals.SetWaveform(&wf);
    ChannelSignals.AddSignal(Signal);
    EXOSignalCollection foundSignals;
    foundSignals.AddChannelSignal(ChannelSignals);

    //Fit. Should return a SignalCollection with only one EXOChannelSignal
    EXOSignalCollection collection = fSignalFitter.Extract(processList,foundSignals);

    //Get the energy from the fit and overwrite the existing charge cluster energy
    const EXOChannelSignals* cs = collection.GetSignalsForChannel(tag);
    if(cs){
      cs->ResetIterator();
      const EXOSignal* sig = cs->Next();
      if(!sig){
        continue;
      }
      double energy = sig->fMagnitude * fUWireScalingFactor;
      double gainEnergy = energy * (energy1 * gain1 + energy2 * gain2) / energysum; 
      cc->fRawEnergy = energy;
      cc->fCorrectedEnergy = gainEnergy;
    }
    else{
      cout << "no signal found!" << endl;
    }
  }// end loop over scintillation clusters

  return kOk;
}

int EXOSummedWaveformFitter::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/sumfitter/file","Name of the output file", 
           this, fFilename, &EXOSummedWaveformFitter::SetFilename);

  talktoManager->CreateCommand("/sumfitter/WireMatchTime","Set the clustering time which was used for u-wires",
           this, fClusteringTime, &EXOSummedWaveformFitter::SetClusteringTime);

  talktoManager->CreateCommand("/sumfitter/ElectronicsDBFlavor","Set the flavor string used to query the database for electronics shaping times",
           this, fElectronicsDatabaseFlavor, &EXOSummedWaveformFitter::SetShapingDatabaseFlavor);

  talktoManager->CreateCommand("/sumfitter/UWireDBFlavor","Set the flavor string used to query the database for u-wire gains",
           this, fUWireDatabaseFlavor, &EXOSummedWaveformFitter::SetGainDatabaseFlavor);

  talktoManager->CreateCommand("/sumfitter/UWireScaling","scaling * ADC counts = fRawEnergy",
           this, fUWireScalingFactor, &EXOSummedWaveformFitter::SetUWireScaling);

  return 0;
}
