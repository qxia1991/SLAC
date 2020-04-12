#include "EXOAnalysisManager/EXOReconstructionModule.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOUWireSignalModelBuilder.hh"
#include "EXOReconstruction/EXOUWireIndSignalModelBuilder.hh"
#include "EXOReconstruction/EXOVWireSignalModelBuilder.hh"
#include "EXOReconstruction/EXOAPDSignalModelBuilder.hh"
#include "EXOReconstruction/EXOChargeInjSignalModelBuilder.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOControlRecordList.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOVWireThresholds.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOCalibUtilities/EXODriftVelocityCalib.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include <iostream>

using EXOMiscUtil::TypeOfChannel;
using EXOMiscUtil::ChannelIsUWire;
using EXOMiscUtil::ChannelIsVWire;
using EXOMiscUtil::ChannelIsAPD;

using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOReconstructionModule, "rec" )

//______________________________________________________________________________
EXOReconstructionModule::EXOReconstructionModule()
: fDriftVelocityCalib(NULL),
  fDriftStatus(kFirstCall),
  fUserDriftVelocity(-1),
  fDriftVelocityTPC1(0), //Will be set each event
  fDriftVelocityTPC2(0), //Will be set each event
  // fCollectionDriftVelocity(2.25*CLHEP::mm/CLHEP::microsecond), // deprecating
  fUserCollectionVelocity(-1),
  fCollectionVelocityTPC1(0), // per event
  fCollectionVelocityTPC2(0), // per event
  fZ_Separation(0.0*CLHEP::mm),
  fRunFlavor(EXOBeginRecord::kUnknownFlavor),
  fSumBothAPDPlanes(false),
  fUWireScalingFactor(ADC_FULL_SCALE_ELECTRONS_WIRE * W_VALUE_LXE_EV_PER_ELECTRON /(CLHEP::keV * ADC_BITS)),
  fVWireScalingFactor(1.),
  fAPDScalingFactor(APD_ADC_FULL_SCALE_ELECTRONS /(ADC_BITS * APD_GAIN)),
  fElectronicsDatabaseFlavor("measured_times"),
  fVWireThresholdDatabaseFlavor("vanilla"),
  fSkipTruncatedData(false),
  fUMatchTime(3.5*SAMPLE_TIME),
  fInductionThresh(500),
  fUWireAdjacentIndSigFindingEnabled(true)
{
  // Add default processors to the list
  fProcLists.push_back(std::make_pair(&fDefineCrossProduct,
    RecProc(false, "define_cross_product")));
  fProcLists.push_back(std::make_pair(&fDefineAPDsums, 
    RecProc(true, "define_apd_sums")));
  //fProcLists.push_back(std::make_pair(&fDefineUWireInd, 
  //  RecProc(true, "define_uwire_ind")));

  fSignalFinders.push_back(std::make_pair(&fMatchedFilterFinder, 
    RecProc(true, "matched_filter_finder"))); 
  fSignalFinders.push_back(std::make_pair(&fMultFinder, 
    RecProc(false, "multiple_sig_finder"))); 
  fSignalFinders.push_back(std::make_pair(&fAPDGangFinder, 
    RecProc(true, "apd_gang_finder")));
  fSignalFinders.push_back(make_pair(&fMatchedFilterFinder,RecProc(false,"uwire_adjacent_ind_sig_finder")));

  fSignalExtractors.push_back(std::make_pair(&fUandAPDExtractor, 
    RecProc(true, "u_and_apd_fitter"))); 
  //fSignalExtractors.push_back(std::make_pair(&fVExtractor, 
   // RecProc(true, "v_wire_extractor"))); 


  fTimingInfo.SetName("ReconStatistics");
  RegisterSharedObject(fTimingInfo.GetName(), fTimingInfo); 
}

EXOReconstructionModule::~EXOReconstructionModule()
{
  RetractObject(fTimingInfo.GetName());
}

//______________________________________________________________________________
int EXOReconstructionModule::Initialize()
{
  // Initialize the reconstruction module, this sets up the modules under the
  // recon module.
#define INIT_RECONLIST(alist)                                 \
  for (size_t i=0;i<alist.size();i++) {                       \
    fSignalModelManager.AddRegisteredObject(alist[i].first);  \
    alist[i].first->SetTimingStatisticInfo(&fTimingInfo);     \
    alist[i].first->SetPrefixName(alist[i].second.fName);     \
  }

  // Tell the signal process listers about the signal model manager
  INIT_RECONLIST(fProcLists)

  // Tell the signal finders about the signal model manager
  INIT_RECONLIST(fSignalFinders)

  // Tell the parameter extractor(s) about the signal model manager
  INIT_RECONLIST(fSignalExtractors)

  fTimingInfo.Clear();
  return 0;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOReconstructionModule::BeginOfRun(EXOEventData *ED)
{
  // At the beginning of the run, the module just retrieves the correct channel
  // map.  The assumption is that the channel map is correct for a given run.
  fChannelMap = &GetChanMapForHeader(ED->fEventHeader);
  if(not fChannelMap){
    LogEXOMsg("Could not get channel map.", EEError);
  }

  // Get the begin-run record for this run.
  EXOControlRecordList* recordList = dynamic_cast<EXOControlRecordList*>(FindSharedObject("ControlRecords"));
  const EXOBeginRecord* beginRun = NULL;
  if(recordList) {
    beginRun = recordList->GetPreviousRecord<EXOBeginRecord>(*ED);
    const EXOEndRecord* endRun = recordList->GetPreviousRecord<EXOEndRecord>(*ED);
    if(beginRun and endRun) {
      // We need to confirm that the run started with beginRun hasn't already been ended by endRun.
      EXOControlRecordList::ControlRecordComp compareRecords;
      if(compareRecords(beginRun, endRun)) beginRun = NULL; // the run has already ended.
    }
  }
  if(beginRun) fRunFlavor = beginRun->GetRunFlavor();
  else {
    // Monte carlo, for instance, lacks control records.
    fRunFlavor = EXOBeginRecord::kUnknownFlavor;
  }

  // Tell the matched filter which data taking phase this run is in.
  //   Phase 1: Runs 2464 - 6482
  //   Interphase period: Runs 6483 - 6940
  //   Phase 2: Runs 6947 onwards
  // If we need more fine-grained modeling of wire/apd noise, then this should be
  // converted into a calib DB table  (5/11/2016 Raymond Tsang).
  fMatchedFilterFinder.SetDataTakingPhase(ED->fRunNumber < 6945 ? "1" : "2");

  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOReconstructionModule::EndOfRun(EXOEventData *ED)
{
  // At the end of the run -- right now all we do is clear the old begin-run record.
  fRunFlavor = EXOBeginRecord::kUnknownFlavor;
  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOReconstructionModule::ProcessEvent(EXOEventData *ED)
{
  // Process event.

  ////////////////////////////////////////////////////////////
  //Reset the EXOEventData object
  ED->ResetForReconstruction();
  ////////////////////////////////////////////////////////////

  fTimingInfo.Reset();

  // overwrite the trigger position if the value exists in trigoff.  priority
  // is: trigoff > root header file trigger position > EXODimensions file
  // TRIGGER_TIME
  int trig_offset = ED->fEventHeader.fTriggerOffset;
  if ( trig_offset > ED->GetWaveformData()->fNumSamples ) { 
    LogEXOMsg("trigger time lies outside of trace", EEDebug);
    return kDrop;
  }
  if (trig_offset > 0) {
    fUandAPDExtractor.SetTriggerSample( trig_offset );
  }

  if ( ED->GetWaveformData()->GetNumWaveforms() == 0 ) {
    LogEXOMsg("no digitized data for this event", EEDebug);
    return kDrop;
  }
  // Do not skip events with saturated traces.  Allows reconstruction
  // to find additional interactions in traces containing TPC muons
  if ( ED->fHasSaturatedChannel ) { // Set by the noise tagger.
    LogEXOMsg("ADC saturated by at least one channel", EEDebug);
  }
  if ( ED->IsTaggedAsNoise() ) {
    if(fRunFlavor == EXOBeginRecord::kClbInt or
       fRunFlavor == EXOBeginRecord::kClbExt or
       fRunFlavor == EXOBeginRecord::kClbCombined) {
      // Calibration runs generally do have negative-going wire signals.
      // So we want to ignore that flag in calibration runs.
      if(ED->IsTaggedAsNoise_Excluding(EXOEventData::kSummedWiresWentNegative)) {
        // OK, it's tagged as noise for a legitimate reason.
        LogEXOMsg("Skipping event tagged as noise", EEDebug);
        return kDrop;
      }
    }
    else {
      // Skip all data events with any noise tag.  (If you don't, you can get rogue events that cause problems.)
      LogEXOMsg("Skipping event tagged as noise", EEDebug);
      return kDrop;
    }
  } // End check of noise tags.
  if ( ED->fEventHeader.fSirenActiveInCR ) {
    LogEXOMsg("Skipping event that occurred during clean room alarm", EEDebug);
    return kDrop;
  }
  if ( fSkipTruncatedData ) {
    if ( ED->fEventHeader.fIsMonteCarloEvent ) {
      LogEXOMsg("Don't use \"/rec/SkipTruncatedData\" on MC.  I don't even know how long full length is.", EEAlert);
    }
    else {
      if ( ED->GetWaveformData()->fNumSamples != 2048 ) {
        LogEXOMsg("Skipping truncated event", EEDebug);
        return kDrop;
      }
    }
  } // end if ( fSkipTruncatedData )
 
  ////////////////////////////////////////////////////////////
  // Create processing list and add all good waveforms to it
  EXOReconProcessList processList;

  EXOWaveformData *wfd = ED->GetWaveformData();
  size_t nwaveforms = wfd->GetNumWaveforms();

  for(size_t i = 0; i < nwaveforms; i++) {
    int channel = wfd->GetWaveform(i)->fChannel;
    if(fChannelMap and not fChannelMap->good_channel(channel)) continue;

    if(fRunFlavor == EXOBeginRecord::kClbInt or
       fRunFlavor == EXOBeginRecord::kClbExt or
       fRunFlavor == EXOBeginRecord::kClbCombined) {
      // It's a charge injection run -- do something different.
      processList.Add(*wfd->GetWaveform(i), EXOReconUtil::kChargeInjection);
    }
    else {
      // Data, MC, or hasn't been rootified recently enough to have control records available.  Do default.
      EXOReconUtil::ESignalBehaviorType signalType;
      switch (TypeOfChannel(channel)) {
        case EXOMiscUtil::kUWire: 
          signalType = EXOReconUtil::kUWire; break;
        case EXOMiscUtil::kVWire: 
          signalType = EXOReconUtil::kVWire; break;
        case EXOMiscUtil::kAPDGang:
          signalType = EXOReconUtil::kAPD; break;
        default:
          signalType = EXOReconUtil::kUndefined;
      }
      processList.Add(*wfd->GetWaveform(i),signalType);
    }
  }
  ////////////////////////////////////////////////////////////
 
  ////////////////////////////////////////////////////////////
  // Get the electronics from the database and fill the signal model manager
  // with the corresponding transfer functions for all waveforms
  if(fElectronicsDatabaseFlavor == "vanilla" and not ED->fEventHeader.fIsMonteCarloEvent) {
    LogEXOMsg("The vanilla electronics table is generally only appropriate for monte carlo data.", EEWarning);
  }
  const EXOElectronicsShapers *electronicsShapers = GetCalibrationFor(
       EXOElectronicsShapers, 
       EXOElectronicsShapersHandler, 
       fElectronicsDatabaseFlavor, 
       ED->fEventHeader);

  SetDriftVelocity(ED);
  SetCollectionVelocity(ED);

  processList.ResetIterator();
  const EXOReconProcessList::WaveformWithType* wfWithType = NULL;
  while ( (wfWithType = processList.GetNextWaveformAndType()) != NULL ) {
    const EXOWaveform* wf = wfWithType->fWf;
    int channel = wf->fChannel;
    const EXOTransferFunction& tf = 
      electronicsShapers->GetTransferFunctionForChannel(channel);

    //std::cout << "Transfer function ch " << channel << " : D1 " << tf.GetDiffTime(0) << std::endl;

    switch (wfWithType->fType) {
      case EXOReconUtil::kUWire: 
        fSignalModelManager.BuildSignalModelForChannelOrTag(
          channel, EXOUWireSignalModelBuilder(tf));

	// Also build a signal model for U Wire induction signals, denoted by negative channel
        fSignalModelManager.BuildSignalModelForChannelOrTag(
	  EXOReconUtil::kUWireIndOffset - channel, EXOUWireIndSignalModelBuilder(tf));
        break;
      case EXOReconUtil::kVWire:
        if(EXOMiscUtil::GetTPCSide(channel) == EXOMiscUtil::kNorth) {
          fSignalModelManager.BuildSignalModelForChannelOrTag(
            channel, EXOVWireSignalModelBuilder(tf, fDriftVelocityTPC1, fCollectionVelocityTPC1, fZ_Separation));
        }
        else /* it's in the south */ {
          fSignalModelManager.BuildSignalModelForChannelOrTag(
            channel, EXOVWireSignalModelBuilder(tf, fDriftVelocityTPC2, fCollectionVelocityTPC2, fZ_Separation));
        }
        break;
      case EXOReconUtil::kAPD: 
        fSignalModelManager.BuildSignalModelForChannelOrTag(
          channel, EXOAPDSignalModelBuilder(tf));
        break;
      case EXOReconUtil::kChargeInjection:
        // Charge injection signals look like APD signals (or at least, treat them that way for now).
        fSignalModelManager.BuildSignalModelForChannelOrTag(
          channel, EXOChargeInjSignalModelBuilder(tf));
        break;
      default: break;
    }
  }
  ////////////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////
  // Also create and add APD sum waveforms
  fDefineAPDsums.SetSumBothPlanes(fSumBothAPDPlanes);

#define BEGIN_PROCESS_RECONLIST(alist)                       \
  for (size_t i=0;i<alist.size();i++) {                      \
    fTimingInfo.StartTimerForTag(alist[i].second.fName);     \
    if (alist[i].second.fDoProcess) {                        

#define END_PROCESS_RECONLIST(alist)                         \
    }                                                        \
    fTimingInfo.StopTimerForTag(alist[i].second.fName);      \
  }


  // Loop over the process lists, process if necessary
  BEGIN_PROCESS_RECONLIST(fProcLists)
    processList.Add(fProcLists[i].first->GetProcessList(processList));
  END_PROCESS_RECONLIST(fProcLists)

  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // Find signals
  EXOSignalCollection foundSignals;

  BEGIN_PROCESS_RECONLIST(fSignalFinders)
    foundSignals.Add(fSignalFinders[i].first->FindSignals(processList, foundSignals));
  END_PROCESS_RECONLIST(fSignalFinders)
  ////////////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////
  // Extract (refine) parameters from the found signals
  EXOSignalCollection refinedSignals;
  
  const EXOSignalCollection* sigColl = &foundSignals;
  BEGIN_PROCESS_RECONLIST(fSignalExtractors)
    // Use found signals on the first run through.
    refinedSignals.Add(fSignalExtractors[i].first->Extract(processList, *sigColl));
    //sigColl = &refinedSignals;
  END_PROCESS_RECONLIST(fSignalExtractors)
  ////////////////////////////////////////////////////////////

  // Now that we've found all the U-wire signals we can, go back and look for U-wire
  // induction signals on neighboring channels that are close in time

  //The following also fills EXODefineUWireIndProcessList::fCandidateSignalList
  EXOSignalCollection refinedIndSignals;
  if(fUWireAdjacentIndSigFindingEnabled){//then we should look for u-wire induction signals of some sort
    EXOReconProcessList indProcessList=CompileUWireIndProcessList(processList,refinedSignals);

    //Find signals for the induction channels.
    EXOSignalCollection foundIndSignals;
    foundIndSignals.Add((&fMatchedFilterFinder)->FindSignals(indProcessList,foundIndSignals));

    const EXOSignalCollection *indSignalCollection=&foundIndSignals;
    refinedIndSignals.Add((&fUandAPDExtractor)->Extract(indProcessList,*indSignalCollection));
  }

  ////////////////////////////////////////////////////////////
  //Add all found signals to EXOEventData
  EXOAPDSignal::EXOAPDSignal_type apdSignalType = EXOAPDSignal::kPlaneFit;
  std::string apdTypeString = "plane_fit";
  if(fSumBothAPDPlanes){
    apdSignalType = EXOAPDSignal::kFullFit;
    apdTypeString = "total_fit";
  }
  
  refinedSignals.ResetIterator();
  const EXOChannelSignals *chanSig = 0;
  while((chanSig = refinedSignals.Next()) != NULL){
    chanSig->ResetIterator();
    int channel = chanSig->GetChannel();
    const EXOSignal *sig = 0;
    // The following gets information from the cache that we might want to
    // store.  We do not check if it exists in the cache, the functions return
    // 0 when it doesn't.
    double baseline = chanSig->GetCacheInformationFor("Baseline");
    double baselineError = chanSig->GetCacheInformationFor("BaselineError");
    double chiSquare = chanSig->GetCacheInformationFor("ChiSquare");
    double chiSquareRestr = chanSig->GetCacheInformationFor("ChiSquareRestr");
    while((sig = chanSig->Next()) != NULL){
      switch(chanSig->GetBehaviorType()) {

        /* U-wire signals */
        case EXOReconUtil::kUWire: {

	  // Only save the signals from deposition fits
	  if( channel > EXOReconUtil::kUWireIndOffset) {

	    EXOUWireSignal *uws = ED->GetNewUWireSignal();
	    uws->fChannel = channel;
	    uws->fTime = sig->fTime;
	    uws->fTimeError = sig->fTimeError;
	    uws->fRawEnergy = sig->fMagnitude * fUWireScalingFactor;
	    uws->fRawEnergyError = sig->fMagnitudeError * fUWireScalingFactor;
	    uws->fBaseline = baseline;
	    uws->fBaselineError = baselineError;
	    uws->fChiSquare = chiSquare;
	    uws->fChiSquareRestr = chiSquareRestr;  // chi^2 over restricted window (can be different than 
	                                            // fitting window)

	    
	    // Now cycle through induction signals and see if there is one
	    // corresponding to this channel. 
	    refinedIndSignals.ResetIterator();
	    double chiSquareInd = -1;  // Error value if no signal found on same channel
	    double chiSquareIndRestr = -1;
	    const EXOChannelSignals *chanSigInd = 0;
	    while((chanSigInd = refinedIndSignals.Next()) != NULL){
	      chanSigInd->ResetIterator();
	      int channel_ind = chanSigInd->GetChannel();
	      // If there's a corresponding induction signal on the same channel 
	      // then save its chi^2
	      if( (EXOReconUtil::kUWireIndOffset-channel_ind) == channel) {
		chiSquareInd = chanSigInd->GetCacheInformationFor("ChiSquare");
		chiSquareIndRestr = chanSigInd->GetCacheInformationFor("ChiSquareRestr");
		break;
	      }
	    }
	    // Now save the induction chi^2 (or error value) the the U-wire signal object
	    uws->fChiSquareInd = chiSquareInd;
	    uws->fChiSquareIndRestr = chiSquareIndRestr;
	    
	  }
	  
          break;
        }

        /* V-wire signals */
        case EXOReconUtil::kVWire: {
          EXOVWireSignal *vws = ED->GetNewVWireSignal();
          vws->fChannel = channel;
          vws->fTime = sig->fTime;
          vws->fTimeError = sig->fTimeError;
          vws->fMagnitude = sig->fMagnitude * fVWireScalingFactor;
          vws->fMagnitudeError = sig->fMagnitudeError * fVWireScalingFactor;
          vws->fBaseline = baseline;
          vws->fBaselineError = baselineError;
          vws->fChiSquare = chiSquare;
          vws->fChiSquareRestr = chiSquareRestr;
          break;
        }

        /* APD Gang fits */
        case EXOReconUtil::kAPD: {
          EXOAPDSignal *apdsig = ED->GetNewAPDSignal();
          apdsig->fTime = sig->fTime;
          apdsig->fRawCounts = sig->fMagnitude * fAPDScalingFactor;
          apdsig->fCountsError = sig->fMagnitudeError * fAPDScalingFactor;
          apdsig->fChiSquared = chiSquare;

          if(TypeOfChannel(channel) == EXOMiscUtil::kAPDGang) {
            apdsig->fType = EXOAPDSignal::kGangFit;
            apdsig->fDescr = "gang_fit";
            apdsig->fChannel = channel;
          }
          else /* All others, including APD Sum */ {
            apdsig->fType = apdSignalType;
            apdsig->fDescr = apdTypeString;
            apdsig->fChannel = (fSumBothAPDPlanes) ? 0 : -channel;
          }
          break;
        }

        /* Charge Injection Signals */
        case EXOReconUtil::kChargeInjection: {
          EXOChargeInjectionSignal* cis = ED->GetNewChargeInjectionSignal();
          cis->fChannel = channel;
          cis->fTime = sig->fTime;
          cis->fTimeError = sig->fTimeError;
          cis->fMagnitude = sig->fMagnitude;
          cis->fMagnitudeError = sig->fMagnitudeError;
          cis->fBaseline = baseline;
          cis->fBaselineError = baselineError;
          cis->fChiSquare = chiSquare;
          break;
        }

        default: break;
      }
    }
  }

  //Step through u-wire induction signals and add them to the event.
  refinedIndSignals.ResetIterator();
  while((chanSig = refinedIndSignals.Next()) != NULL){
    chanSig->ResetIterator();
    int channel = chanSig->GetChannel();

    // The following gets information from the cache that we might want to
    // store.  We do not check if it exists in the cache, the functions return
    // 0 when it doesn't.
    double baseline       = chanSig->GetCacheInformationFor("Baseline");
    double baselineError  = chanSig->GetCacheInformationFor("BaselineError");
    double chiSquare      = chanSig->GetCacheInformationFor("ChiSquare");
    double chiSquareRestr = chanSig->GetCacheInformationFor("ChiSquareRestr");
    const EXOSignal *sig = 0;
    while((sig = chanSig->Next()) != NULL){
      if(chanSig->GetBehaviorType() == EXOReconUtil::kUWireInd){
	EXOUWireInductionSignal *uwis = ED->GetNewUWireInductionSignal();
	uwis->fChannel        = channel;
	uwis->fTime           = sig->fTime;
	uwis->fTimeError      = sig->fTimeError;
	uwis->fMagnitude      = sig->fMagnitude;
	uwis->fMagnitudeError = sig->fMagnitudeError;
	uwis->fBaseline       = baseline;
	uwis->fBaselineError  = baselineError;
	uwis->fChiSquare      = chiSquare;
      }
    }
  }

  ////////////////////////////////////////////////////////////
  return kOk;
}

//______________________________________________________________________________
int EXOReconstructionModule::TalkTo(EXOTalkToManager *talktoManager)
{
  // Set up the talkto for the recon module
  talktoManager->CreateCommand("/rec/SumBothAPDPlanes",
                               "Sum both APD planes together rather than reconstructing two summed planes separately.",
                               this,
                               fSumBothAPDPlanes,
                               &EXOReconstructionModule::SetSumBothAPDPlanes );

  talktoManager->CreateCommand("/rec/UWireScaling",
                               "scaling * ADC counts = fRawEnergy",
                               this,
                               fUWireScalingFactor,
                               &EXOReconstructionModule::SetUWireScalingFactor );

  talktoManager->CreateCommand("/rec/VWireScaling",
                               "scaling * ADC counts = fMagnitude",
                               this,
                               fVWireScalingFactor,
                               &EXOReconstructionModule::SetVWireScalingFactor );

  talktoManager->CreateCommand("/rec/APDScaling",
                               "scaling * ADC counts = fRawCounts",
                               this,
                               fAPDScalingFactor,
                               &EXOReconstructionModule::SetAPDScalingFactor );

  talktoManager->CreateCommand("/rec/ElectronicsDBFlavor",
                               "Set the flavor string used to query the database for electronics shaping times",
                               this,
                               fElectronicsDatabaseFlavor,
                               &EXOReconstructionModule::SetElectronicDatabaseFlavor );

  talktoManager->CreateCommand("/rec/VWireThresholdDBFlavor",
                               "Set the flavor string used to query the database for v-wire thresholds",
                               this,
                               fVWireThresholdDatabaseFlavor,
                               &EXOReconstructionModule::SetVWireThresholdDatabaseFlavor );

  ////////////////////////////////////////////////////////////
  // For the U/APD signal fitter.  Should go away eventually.
  talktoManager->CreateCommand("/rec/UWireChiSquareCut",
                               "Drop all signals from a u-wire which yields a poor fit",
                               &fUandAPDExtractor,
                               -1.,
                               &EXOSignalFitter::SetChannelFitChiSquareCut );

  talktoManager->CreateCommand("/rec/UpperFitBoundWire",
                               "Set the upper bound for the fit window used for fitting wire signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetUpperFitBoundWireMicroseconds );

  talktoManager->CreateCommand("/rec/LowerFitBoundWire",
                               "Set the lower bound for the fit window used for fitting wire signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetLowerFitBoundWireMicroseconds );

  talktoManager->CreateCommand("/rec/UpperFitBoundVWire",
                               "Set the upper bound for the fit window used for fitting V-wire signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetUpperFitBoundVWireMicroseconds );

  talktoManager->CreateCommand("/rec/LowerFitBoundVWire",
                               "Set the lower bound for the fit window used for fitting V-wire signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetLowerFitBoundVWireMicroseconds );

  talktoManager->CreateCommand("/rec/UpperFitBoundWireRestr",
                               "Set the upper bound for the restricted window used for calculating the chi^2 only (not used for the fit).  Given in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetUpperFitBoundWireRestrMicroseconds );
  
  talktoManager->CreateCommand("/rec/LowerFitBoundWireRestr",
                               "Set the lower bound for the restricted window used for calculating the chi^2 only (not used for the fit).  Given in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetLowerFitBoundWireRestrMicroseconds );

  talktoManager->CreateCommand("/rec/UpperFitBoundAPD",
                               "Set the upper bound for the fit window used for fitting apd signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetUpperFitBoundAPDMicroseconds );

  talktoManager->CreateCommand("/rec/LowerFitBoundAPD",
                               "Set the lower bound for the fit window used for fitting apd signals in microseconds.",
                               &fUandAPDExtractor,
                               40.,
                               &EXOSignalFitter::SetLowerFitBoundAPDMicroseconds );

  talktoManager->CreateCommand("/rec/SkipTruncatedData", "Skip data with truncated waveforms (no-op on MC)", this,
                               false, &EXOReconstructionModule::SetSkipTruncatedData );

  talktoManager->CreateCommand("/rec/drift_velocity_mm_per_ns",
                               "Set the drift velocity, in mm/ns (affects v-wire signals)",
                               this,
                               fUserDriftVelocity,
                               &EXOReconstructionModule::SetUserDriftVelocity);

  talktoManager->CreateCommand("/rec/collection_drift_velocity_mm_per_ns",
                               "Sets the drift velocity between the u and v wires",
                               this,
                               fUserCollectionVelocity,
                               &EXOReconstructionModule::SetUserCollectionVelocity);

  std::string availModules;
  ////////////////////////////////////////////////////////////
  // Now pass along the TalkTo to the base modules
  for (size_t i=0;i<fProcLists.size();i++) {
    fProcLists[i].first->TalkTo("/rec/" + 
      fProcLists[i].second.fName, talktoManager);
    availModules += fProcLists[i].second.fName + " ";
  }

  for (size_t i=0;i<fSignalFinders.size();i++) {
    fSignalFinders[i].first->TalkTo("/rec/" + 
      fSignalFinders[i].second.fName, talktoManager);
    availModules += fSignalFinders[i].second.fName + " ";
  }
  for (size_t i=0;i<fSignalExtractors.size();i++) { 
    fSignalExtractors[i].first->TalkTo("/rec/" + 
      fSignalExtractors[i].second.fName, talktoManager);
    availModules += fSignalExtractors[i].second.fName + " ";
  }


  // And create commands dealing with the stages
  talktoManager->CreateCommand("/rec/enable_stage",
                               "Enable a reconstruction stage, form: [stage name] [true|false], "  
                               "Available stages: " + availModules,
                               this,
                               "",
                               &EXOReconstructionModule::EnableStage);

  talktoManager->CreateCommand("/rec/show_stage_status",
                               "Show status of reconstruction stages", 
                               this,
                               &EXOReconstructionModule::PrintStageStatus);

  talktoManager->CreateCommand("/rec/Z_Separation",
                               "Imitate spatial distribution of charge cluster with two pixels separated in Z (in mm; used only for v-wires currently)",
                               this,
                               fZ_Separation,
                               &EXOReconstructionModule::SetZ_Separation);

  return 0;
}

//______________________________________________________________________________
void EXOReconstructionModule::EnableStage(std::string stage_plus_bool)
{
  if (stage_plus_bool == "") return;                                                       
  std::istringstream istr(stage_plus_bool);                                                
  std::string boolStr;                                                             
  std::string stageName;                                                            
  istr >> stageName >> boolStr;                                                     
  std::istringstream nstr(boolStr);                                                
                                                                                   
  if ( istr.fail() ) goto fail;                                                    
                                                                                   
  bool cmdBool;                                                                    
  if (! (nstr >> cmdBool) ) {                                                      
    std::transform( boolStr.begin(), boolStr.end(), boolStr.begin(), ::tolower );  
    if (boolStr == "true") cmdBool = true;                                         
    else if (boolStr == "false" ) cmdBool = false;                                 
    else goto fail;                                                                
  }                                                                                
  for (size_t i=0;i<fProcLists.size();i++) {
    RecProc& proc = fProcLists[i].second;
    if (proc.fName == stageName) {
      proc.fDoProcess = cmdBool;
      return;
    }
  }

  for (size_t i=0;i<fSignalFinders.size();i++) {
    RecProc& proc = fSignalFinders[i].second;
    if (proc.fName == stageName) {
      proc.fDoProcess = cmdBool;
      return;
    }
  }
  for (size_t i=0;i<fSignalExtractors.size();i++) { 
    RecProc& proc = fSignalExtractors[i].second;
    if (proc.fName == stageName) {
      proc.fDoProcess = cmdBool;
      return;
    }
  }
                                                                         
fail:                                                                              
  LogEXOMsg("Error parsing string '" + stage_plus_bool + "'", EEError);                                             
}
//______________________________________________________________________________
void EXOReconstructionModule::PrintStageStatus() 
{
  // Print the stage status, i.e. which stages will be run and which not.

  std::cout << "Reconstruction Stage running status: " << std::endl;
  std::cout << "  Processing List Stage: " << std::endl;
   for (size_t i=0;i<fProcLists.size();i++) {
    const RecProc& proc = fProcLists[i].second;
    std::cout << "    " << proc.fName << ", ";
    if (proc.fDoProcess) std::cout << "Running"; 
    else std::cout << "NOT Running"; 
    std::cout << std::endl;
  }

  std::cout << "  Signal Finding Stage: " << std::endl;
  for (size_t i=0;i<fSignalFinders.size();i++) {
    const RecProc& proc = fSignalFinders[i].second;
    std::cout << "    " << proc.fName << ", ";
    if (proc.fDoProcess) std::cout << "Running"; 
    else std::cout << "NOT Running"; 
    std::cout << std::endl;
  }

  std::cout << "  Signal Extraction Stage: " << std::endl;
  for (size_t i=0;i<fSignalExtractors.size();i++) { 
    const RecProc& proc = fSignalExtractors[i].second;
    std::cout << "    " << proc.fName << ", ";
    if (proc.fDoProcess) std::cout << "Running"; 
    else std::cout << "NOT Running"; 
    std::cout << std::endl;
  }
  
}
//______________________________________________________________________________
void EXOReconstructionModule::SetUWireScalingFactor(double val)
{
  //This scaling factor times ADC counts = fRawEnergy
  fUWireScalingFactor = val;
}

//______________________________________________________________________________
void EXOReconstructionModule::SetVWireScalingFactor(double val)
{
  //This scaling factor times ADC counts = fMagnitude
  fVWireScalingFactor = val;
}

//______________________________________________________________________________
void EXOReconstructionModule::SetAPDScalingFactor(double val)
{
  //This scaling factor times ADC counts = fRawCounts
  fAPDScalingFactor = val;
}

//______________________________________________________________________________
void EXOReconstructionModule::SetDriftVelocity( EXOEventData* ED )
{
//Sets the drift velocity that is used for this event.
//The current order of preferences on data we can identify as monte carlo:
//1) User-set value (command /rec/drift_velocity_mm_per_ns)
//2) EXODimensions.hh value
//If we can't identify it as monte carlo:
//1) User-set value (command /rec/drift_velocity_mm_per_ns)
//2) Fetch from database.
//3) EXODimensions.hh value.
// FixME:  This is a duplicate of the same function in EXOClusteringModule.
//         Additionally, there is a different drift velocity interface in EXODigitizeModule.
//         Surely there's a way to unify all of this...
//         Maybe drift velocity should be a shared object, and many modules have the ability to set it?

  if ( fUserDriftVelocity > 0 ) {
    if ( fDriftStatus != kUserSet ) {
      fDriftVelocityTPC1 =  fUserDriftVelocity;
      fDriftVelocityTPC2 =  fUserDriftVelocity;
      if ( fDriftStatus != kFirstCall ) std::cout<<"EXOReconstructionModule: changing drift velocity mid-job."<<std::endl;
      std::cout<<"EXOReconstructionModule: using user-set drift velocity = "<<fUserDriftVelocity<<" mm/ns."<<std::endl;
      fDriftStatus = kUserSet;
    }
    return;
  }

  if ( ED->fEventHeader.fIsMonteCarloEvent ) {
    if ( fDriftStatus != kDefaultMC) {
      fDriftVelocityTPC1 = DRIFT_VELOCITY;
      fDriftVelocityTPC2 = DRIFT_VELOCITY;
      if ( fDriftStatus != kFirstCall ) std::cout<<"EXOReconstructionModule: changing drift velocity mid-job."<<std::endl;
      std::cout<<"EXOReconstructionModule: running on monte carlo data; using drift velocity = "<<DRIFT_VELOCITY<<" mm/ns from EXODimensions.hh file."<<std::endl;
      fDriftStatus = kDefaultMC;
    }
    return;
  }

  // It's not MC at this point, and there's no user setting, so query the database.
  if ( fDriftVelocityCalib != NULL ) {
    // is the old calibration still good?
    if( fDriftVelocityCalib->isValid( 
           EXOTimestamp(ED->fEventHeader.fTriggerSeconds, 
                        static_cast<int>(ED->fEventHeader.fTriggerMicroSeconds*CLHEP::microsecond)) ) ) {
      if( fDriftStatus != kDatabase ) {
        fDriftVelocityTPC1 = fDriftVelocityCalib->get_drift_velocity_TPC1();
        fDriftVelocityTPC2 = fDriftVelocityCalib->get_drift_velocity_TPC2();
        std::cout<<"EXOReconstructionModule: changing drift velocity mid-job."<<std::endl;
        std::cout<<"Reusing old database values that are still valid."<<std::endl;
        fDriftStatus = kDatabase;
      }
      return;
    }
  }

  if ( fDriftStatus != kDefaultQueryFail) {
    std::cout<<"EXOReconstructionModule: querying calibManager for drift velocity."<<std::endl;
    if ( fDriftStatus == kDatabase ) std::cout<<"The old timestamp was no longer valid."<<std::endl;
    fDriftVelocityCalib = GetCalibrationFor(EXODriftVelocityCalib, 
                                            EXODriftVelocityHandler, 
                                            "vanilla", 
                                            ED->fEventHeader);
    if(fDriftVelocityCalib) {
      fDriftVelocityTPC1 = fDriftVelocityCalib->get_drift_velocity_TPC1();
      fDriftVelocityTPC2 = fDriftVelocityCalib->get_drift_velocity_TPC2();
      std::cout<<"EXOReconstructionModule: successfully retrieved drift velocity from database."<<std::endl;
      std::cout<<"drift_velocity_TPC1 = "<<fDriftVelocityCalib->get_drift_velocity_TPC1()<<std::endl;
      std::cout<<"drift_velocity_TPC2 = "<<fDriftVelocityCalib->get_drift_velocity_TPC2()<<std::endl;
      fDriftStatus = kDatabase;
      if (fCollectionStatus == kDatabase){ fCollectionStatus = kDBUpdate;} //push update on the collection as well
      return;
    }
  }

  // the database read failed
  if ( fDriftStatus != kDefaultQueryFail ) {
    fDriftVelocityTPC1 = DRIFT_VELOCITY;
    fDriftVelocityTPC2 = DRIFT_VELOCITY;
    std::cout<<"EXOReconstructionModule: database query was unsuccessful; henceforth using default drift velocity = "<<DRIFT_VELOCITY<<" mm/ns from EXODimensions.hh."<<std::endl;
    fDriftStatus = kDefaultQueryFail;
  }
  return;
}

//______________________________________________________________________________
void EXOReconstructionModule::SetCollectionVelocity( EXOEventData* ED )
{
//Sets the collection velocity that is used for this event.
//The current order of preferences to set the collection velocity : 
//1) User-set value (command /rec/collection_velocity_mm_per_ns)
// if identified as monte carlo:
//2) EXODimensions.hh value
// if we can't identify it as monte carlo:
//2) Fetch from database.
//3) EXODimensions.hh value.
// FixME:  This is a duplicate of the same function in EXOClusteringModule.
//         Additionally, there is a different collection velocity interface in EXODigitizeModule.
//         Surely there's a way to unify all of this...
//         Maybe collection velocity should be a shared object, and many modules have the ability to set it? 
//hrm ah.

  if ( fUserCollectionVelocity > 0 ) {
    if ( fCollectionStatus != kUserSet ) {
      fCollectionVelocityTPC1 =  fUserCollectionVelocity;
      fCollectionVelocityTPC2 =  fUserCollectionVelocity;
      if ( fCollectionStatus != kFirstCall ) std::cout<<"EXOReconstructionModule: changing collection velocity mid-job."<<std::endl;
      std::cout<<"EXOReconstructionModule: using user-set collection velocity = "<<fUserCollectionVelocity<<" mm/ns."<<std::endl;
      fCollectionStatus = kUserSet;
    }
    return;
  }

  if ( ED->fEventHeader.fIsMonteCarloEvent ) {
    if ( fCollectionStatus != kDefaultMC) {
      fCollectionVelocityTPC1 = COLLECTION_VELOCITY;
      fCollectionVelocityTPC2 = COLLECTION_VELOCITY;
      if ( fCollectionStatus != kFirstCall ) std::cout<<"EXOReconstructionModule: changing collection velocity mid-job."<<std::endl;
      std::cout<<"EXOReconstructionModule: running on monte carlo data; using collection velocity = "<<COLLECTION_VELOCITY<<" mm/ns from EXODimensions.hh file."<<std::endl;
      fCollectionStatus = kDefaultMC;
    }
    return;
  }

  // It's not MC at this point, and there's no user setting, so query the database.
  if ( fDriftVelocityCalib != NULL ) {
    if (fCollectionStatus == kDBUpdate or fCollectionStatus == kFirstCall){ // setdriftvelocity updated the database to a new valid one -- allows db sharing while keeping the option of just looking up the collection status and setting the drift velocity manually
        fCollectionVelocityTPC1 = fDriftVelocityCalib->get_collection_velocity_TPC1();
        fCollectionVelocityTPC2 = fDriftVelocityCalib->get_collection_velocity_TPC2();
	fCollectionStatus = kDatabase;
    }
    // if we get here we aren't using db in setdriftvelocity so we have to ask all the questions, first -- is the old calibration still valid?

    // is the old calibration still good?
    if( fDriftVelocityCalib->isValid( 
           EXOTimestamp(ED->fEventHeader.fTriggerSeconds, 
                        static_cast<int>(ED->fEventHeader.fTriggerMicroSeconds*CLHEP::microsecond)) ) ) {
      if( fCollectionStatus != kDatabase ) {
        fCollectionVelocityTPC1 = fDriftVelocityCalib->get_collection_velocity_TPC1();
        fCollectionVelocityTPC2 = fDriftVelocityCalib->get_collection_velocity_TPC2();
        std::cout<<"EXOReconstructionModule: changing collection velocity mid-job."<<std::endl;
        std::cout<<"Reusing old database values that are still valid."<<std::endl;
        fCollectionStatus = kDatabase;
      }
      return;
    }
  }

  if ( fCollectionStatus != kDefaultQueryFail) {
    std::cout<<"EXOReconstructionModule: querying calibManager for collection velocity."<<std::endl;
    if ( fCollectionStatus == kDatabase ) std::cout<<"The old timestamp was no longer valid."<<std::endl;
    fDriftVelocityCalib = GetCalibrationFor(EXODriftVelocityCalib, 
                                            EXODriftVelocityHandler, 
                                            "vanilla", 
                                            ED->fEventHeader);
    if(fDriftVelocityCalib) {
      fCollectionVelocityTPC1 = fDriftVelocityCalib->get_collection_velocity_TPC1();
      fCollectionVelocityTPC2 = fDriftVelocityCalib->get_collection_velocity_TPC2();
      std::cout<<"EXOReconstructionModule: successfully retrieved collection velocity from database."<<std::endl;
      std::cout<<"collection_velocity_TPC1 = "<<fDriftVelocityCalib->get_collection_velocity_TPC1()<<std::endl;
      std::cout<<"collection_velocity_TPC2 = "<<fDriftVelocityCalib->get_collection_velocity_TPC2()<<std::endl;
      fCollectionStatus = kDatabase;
      return;
    }
  }

  // the database read failed
  if ( fCollectionStatus != kDefaultQueryFail ) {
    fCollectionVelocityTPC1 = COLLECTION_VELOCITY;
    fCollectionVelocityTPC2 = COLLECTION_VELOCITY;
    std::cout<<"EXOReconstructionModule: database query was unsuccessful; henceforth using default collection velocity = "<<COLLECTION_VELOCITY<<" mm/ns from EXODimensions.hh."<<std::endl;
    fCollectionStatus = kDefaultQueryFail;
  }
  return;
}

//______________________________________________________________________________
EXOReconProcessList EXOReconstructionModule::CompileUWireIndProcessList(const EXOReconProcessList &processList,const EXOSignalCollection &inputSignals) const{
  EXOReconProcessList ret;

  //First do simple clustering of u-wire signals into bundles, then find
  //the energy weighted average start time of each bundle.
  list<pair<double,double> > signals;//EXOSignal parameters are not Double_t :(
  inputSignals.ResetIterator();
  const EXOChannelSignals *cs=0;
  while((cs=inputSignals.Next()) != NULL){
    cs->ResetIterator();
    const EXOSignal *s=0;
    while((s=cs->Next()) != NULL)
      signals.push_back(make_pair(s->fTime,s->fMagnitude*fUWireScalingFactor));//time and energy

    //Starting with the largest energy, find energy weighted time of all
    //signals within the charge clustering time.
    sortBySecondDescending bySecond;
    signals.sort(bySecond);

    list<pair<double,double> >::iterator signal=signals.begin();//->first = time
                                                                //->second= energy
    while(signal!=signals.end()){//should NOT be end()-1
      double energySum          = signal->second;
      double energyWeightedTime = signal->first*signal->second;

      list<pair<double,double> >::iterator anotherSignal=signal; anotherSignal++;//start with next signal
      while(anotherSignal!=signals.end()){
    	if(TMath::Abs(anotherSignal->first - signal->first) < fUMatchTime){
	  energySum          += anotherSignal->second;
	  energyWeightedTime += anotherSignal->first*anotherSignal->second;
	  anotherSignal=signals.erase(anotherSignal);
    	}
	else anotherSignal++;
      }

      //Replace signal with bundled signal if it is above some threshold,
      if(energySum > fInductionThresh){
	*signal=make_pair(energyWeightedTime/energySum,energySum);
	signal++;
      }//otherwise drop this signal.
      else signal=signals.erase(signal);
    }

    //Add the refined signals with energy weighted signal times to a processing
    //list. Also store these signal times for later use by induction signal
    //finders/fitters.
    signal=signals.begin();
    while(signal!=signals.end()){//should NOT be end()-1
      Int_t channel=cs->GetChannel()-1;
      if(EXOMiscUtil::TypeOfChannel(channel)==EXOMiscUtil::kUWire){
	EXODefineUWireIndProcessList::SetChannelToAdd(channel);
	EXODefineUWireIndProcessList::AddCandidateSignal(channel,signal->first);
	ret.Add(EXODefineUWireIndProcessList::StaticGetProcessList(processList));
      }
      channel+=2;//+1 from charge collecting u-wire
      if(TypeOfChannel(channel)==EXOMiscUtil::kUWire){//use local TypeOfChannel check because the one in EXOMiscUtil is baked
	EXODefineUWireIndProcessList::SetChannelToAdd(channel);
	EXODefineUWireIndProcessList::AddCandidateSignal(channel,signal->first);
	ret.Add(EXODefineUWireIndProcessList::StaticGetProcessList(processList));
      }
      signal++;
    }
  }

  return ret;
}
