//______________________________________________________________________________
//
//
// -o_o-o_o- -o_o-o_o-

#include <iostream>
#include "EXOAnalysisManager/EXOReconNoiseTagger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOUWireSignal.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOReconNoiseTagger, "reconnoisetag" )



EXOReconNoiseTagger::EXOReconNoiseTagger()
:fChiSquareCut(2.0),
 fEnergyCut(100.0),
 fMultCut(0.0),
 fTimeCut(40.e3)
{
}


int EXOReconNoiseTagger::Initialize() {

  std::cout << "  Initializing EXO Recon Team Noise Tagger." << std::endl;
  std::cout << "  Events that look like noise post Reconstruction will be tagged." << std::endl;
  return 0;
}



EXOAnalysisModule::EventStatus EXOReconNoiseTagger::BeginOfRun(EXOEventData *eventdata) {
  run_number = -1;
  
  num_removed_events=0;
  num_adjust_events =0;
  run_start_time    =0;
  run_end_time      =0;
  num_bad_chi2      =0;

  return kOk;
}

std::vector<EXOUWireSignal*> EXOReconNoiseTagger::GetListOfSuspicous(EXOEventData *ED)
{
    // Loop over U-Wire Signals and create a list of all that fit the Energy/Chi-Square Cut
    // Defined by fEnergyCut and fChiSquareCut
    // these are the signals we claim are noise

    std::vector<EXOUWireSignal*> ulist;
    for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
        EXOUWireSignal* signal = ED->GetUWireSignal(i);

        //std::cout << "Initial USig " << signal->fChannel << " " << signal->fTime <<
        //          " "  <<  signal->fChiSquare << " "<< signal->fCorrectedEnergy << std::endl;

        //Does the signal look like a bad event (small energy but large chi-square)
        if (signal->fCorrectedEnergy < fEnergyCut and signal->fChiSquare > fChiSquareCut){
            ulist.push_back(signal);
        }
   }

   return ulist;
}


std::vector<EXOUWireSignal*> EXOReconNoiseTagger::FindSigsToRemove(EXOEventData *ED, EXOUWireSignal* badSignal)
{
   //Now loop over signals and compare to the suspicous signal
   //if there are no "real" signals (E>Ecut) with tcut than find all signals withing tcut and add to list
   
   std::vector<EXOUWireSignal*> toBeRemoved;
   for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
        EXOUWireSignal* signal = ED->GetUWireSignal(i);
            
        if (signal->fCorrectedEnergy > fEnergyCut and abs(signal->fTime-badSignal->fTime) < fTimeCut){
            // There appears to be a real Signal within 40us so can't do anything
            // allow this event.
            toBeRemoved.clear();
            break;    
        }
        else if(signal->fCorrectedEnergy < fEnergyCut and abs(signal->fTime-badSignal->fTime) < fTimeCut) {
            toBeRemoved.push_back(signal);
        } 
   }

   return toBeRemoved;
}

EXOAnalysisModule::EventStatus EXOReconNoiseTagger::ProcessEvent(EXOEventData *ED)
{
   ED->fEventHeader.fNoiseRemoved = false;
   //Keep track of run times so we can report noise rate
   if (run_number < 0 ) {
      run_number     = ED->fRunNumber;
      run_start_time = ED->fEventHeader.fTriggerSeconds;
   }
   run_end_time = ED->fEventHeader.fTriggerSeconds;
   
   std::vector<EXOUWireSignal*> badChiList = GetListOfSuspicous(ED); 
   std::vector<EXOUWireSignal*> toBeRemoved;

   if(badChiList.size() > 0.5){
        num_bad_chi2++;
   }
   else{
       num_good_events++;
   }

   //Loop over suspicous channels and create a list of the signals which should be removed
   for (size_t i=0; i < badChiList.size(); i++){
        
        EXOUWireSignal* badSignal = badChiList[i];  
        std::vector<EXOUWireSignal*> toBeRemoved_temp = FindSigsToRemove(ED, badSignal);
        
        //See if the signal is already in the list of to be removed.  If it is than don't add again
        for(size_t j=0; j<toBeRemoved_temp.size(); j++){
            if (std::find(toBeRemoved.begin(),toBeRemoved.end(),toBeRemoved_temp[j]) == toBeRemoved.end()){
                toBeRemoved.push_back(toBeRemoved_temp[j]);
            }
        }
   }

   if (toBeRemoved.size() > (fMultCut+0.5)){
        for (size_t i=0; i < toBeRemoved.size(); i++){
            EXOUWireSignal* signal = toBeRemoved[i];
            
            //std::cout << "Remove USig " << " " << signal->fChannel << " " << signal->fTime 
            //          << " " << signal->fChiSquare << " " << signal->fCorrectedEnergy << std::endl;
            
            ED->Remove(signal);
        }
        if (ED->GetNumUWireSignals() < 0.5) {
            //This event is now completely empty
            num_removed_events += 1;
        }
        else{
            //We did something but there is still stuff leftover
            num_adjust_events+=1;
        }
        ED->fEventHeader.fNoiseRemoved = true;
        //std::cout << "End Event (Removed: " << toBeRemoved.size() << " Left: " << ED->GetNumUWireSignals() << ")"  << std::endl;
        //std::cout <<  std::endl;
   }
   else if(badChiList.size() > 0.5){
       num_ignored++;
   }

   return kOk;

   //First Loop over U-Wire Signals and see if any fit the model "Noise Signal"
   /*
   for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
        EXOUWireSignal* signal = ED->GetUWireSignal(i);
        
        //Does the signal look like a bad event (small energy but large chi-square)
        if (signal->fCorrectedEnergy < fEnergyCut and signal->fChiSquare > fChiSquareCut){
            badChi=true;
            if (signal->fChiSquare > worstChi){
                worstChi       = signal->fChiSquare;
                worstChiTime   = signal->fTime;
            }
        }    
   }
   */

   //If we found any suspicous signals check if there are any other small U-Signals within 40us
   /*if (badChi) {
       num_bad_chi2+=1;
       bool doRemoval = true;
       
       std::vector<EXOUWireSignal*> toBeRemoved;

       for(size_t i = 0; i < ED->GetNumUWireSignals(); i++) {
            EXOUWireSignal* signal = ED->GetUWireSignal(i);
            
            //std::cout << "Signal (ch, time, energy)" << signal->fChannel << " " << signal->fCorrectedEnergy << "  " << signal->fTime
            //          << "  " << worstChiTime << std::endl;

            if (signal->fCorrectedEnergy > fEnergyCut and abs(signal->fTime-worstChiTime) < fTimeCut){
                // There appears to be a real Signal within 40us so can't do anything
                // allow this event.
                num_ignored +=1;        
                doRemoval = false;
                break;    
            }
            else if(signal->fCorrectedEnergy < fEnergyCut and abs(signal->fTime-worstChiTime) < fTimeCut) {
                toBeRemoved.push_back(signal);
            }
       }

       // At this point there is a list of signals to remove and there wasn't a real event within 40us.
       // Now do the removal.
       if (doRemoval){
            for (int i=0; i < toBeRemoved.size(); i++){
                
                //EXOUWireSignal* signal = ED->GetUWireSignal(toBeRemoved[i]);
                EXOUWireSignal* signal = toBeRemoved[i];
                //std::cout << " Removed (ch, time, energy)" << signal->fChannel << " " 
                //          << signal->fCorrectedEnergy << "  "  << signal->fTime  << "  " << worstChiTime << std::endl;
                
                //std::cout << "Remove Size " << toBeRemoved[i] << std::endl;

                ED->Remove(signal);
                //std::cout << "Removed Good" << std::endl;
            }
       

            if (toBeRemoved.size()> (0.5) and ED->GetNumUWireSignals() < 0.5){
                //This is now an empty event 
                num_removed_events += 1;
            }
            else if (toBeRemoved.size()> 0.5){
                //There is still a real signal here but we did remove some weird signals
                num_adjust_events+=1;
                //std::cout << "*******************MJPARITAL" << std::endl;
            
            }
       }    
   
   }
   else{
      //This Event had no suspcious chi-square events so don't do anything
      num_good_events+=1;
   }
  return kOk;
  */
}

EXOAnalysisModule::EventStatus EXOReconNoiseTagger::EndOfRun(EXOEventData *eventdata) {

  std::cout << "===============================================================" << std::endl;
  std::cout << "========================Results of Recon Noise Tagger==========" << std::endl;

  std::cout << "  Noise rate: " << static_cast<double>(num_removed_events)/(run_end_time-run_start_time) << " Hz" << std::endl;
  std::cout << "Num with a Bad Chi2 Candidate: " << num_bad_chi2       << std::endl;  
  std::cout << "Num Events Fully Removed: "      << num_removed_events << std::endl;
  std::cout << "Num Events Paritally Removed: "  << num_adjust_events  << std::endl;
  std::cout << "Num Events Ignored: "            << num_ignored        << std::endl;
  std::cout << "Rate Removed: "                  << static_cast<double>(num_removed_events)/(run_end_time-run_start_time) << " Hz " << std::endl;
  std::cout << "Length of Run "                  << (run_end_time-run_start_time) << " s " << std::endl;
  
  std::cout << "===============================================================" << std::endl;
  std::cout << "===============================================================" << std::endl;

  return kOk;
}



int EXOReconNoiseTagger::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/reconnoisetag/chi2Cut",
                             "Set the Chi-Square Cut to select suspicous events.",
                             this,
                             fChiSquareCut,
                             &EXOReconNoiseTagger::SetChiSquareCut);

  talktoManager->CreateCommand("/reconnoisetag/energyCut",
                             "Set the Energy Cut to define noise signals.",
                             this,
                             fEnergyCut,
                             &EXOReconNoiseTagger::SetEnergyCut);

  talktoManager->CreateCommand("/reconnoisetag/multCut",
                             "Set the cut for number of Noise events.",
                             this,
                             fMultCut,
                             &EXOReconNoiseTagger::SetMultCut);

    talktoManager->CreateCommand("/reconnoisetag/timeCut",
                             "Set the cut for the time window to use (ns).",
                             this,
                             fTimeCut,
                             &EXOReconNoiseTagger::SetTimeCut);

  return 0;
}
