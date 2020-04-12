//Module to attempt matching UWire IsInduction tagged signals to a cluster so the energy from the mixed signal
//fit  (EXOUWieMixedSignalModule) could be added to the charge cluster energy. This would run before the correction modules.
//______________________________________________________________________________
#include "EXOAnalysisManager/EXOUWireMixSigAddModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOSignalFitter.hh"
#include "EXOReconstruction/EXOUWireSignalModelBuilder.hh"
#include "EXOReconstruction/EXOUWireIndSignalModelBuilder.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignal.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TImage.h"
#include <iostream>
//#include<tuple>
//#include<vector>
using namespace std;


IMPLEMENT_EXO_ANALYSIS_MODULE( EXOUWireMixSigAddModule, "umixedsignaladd" )

EXOUWireMixSigAddModule::EXOUWireMixSigAddModule()
{
	//Talk to command to set this. This threshold is what makes the final decision if the Ind tag gets added to the CC. The difference in signal is checked between the Ind tagged signal and the closet u-wire of a cluster
	fSignalTimeDiffLimit = 0.001e6;
	//Talk to command to set this. When the Induction Tag is "re-fit" this cuts any returned with an energy below the cut value. Set to 25 keV as default. This should be just above the 800 electron noise.
	fTagEnergyCut = 25.0;
  //  cout << "constructing EXOUWireMixSigAddModule with name " << NAME << endl;

}

int EXOUWireMixSigAddModule::Initialize()
{

  return 0;
}

EXOAnalysisModule::EventStatus EXOUWireMixSigAddModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  return kOk;
}
//Method to identify the closest CC. The infotup structure is defined below
void EXOUWireMixSigAddModule::FindCluster(EXOEventData *ED, infotup *t){
	
	int ncc = ED->GetNumChargeClusters();
	//will keep track of the closest usig time difference for a cluster
	double besttimediff = 2e6;
	int bestcc = -1;
	for(int k=0; k<ncc; k++){ //loop through all the cluster
		EXOChargeCluster* cc = ED->GetChargeCluster(k);
		int nucc = cc->GetNumUWireSignals();
		double IsIndUsigtimediff = 2e6;
		int bestUmatch = -1;
		double cctim = cc->fCollectionTime;
		for(int l=0; l<nucc; l++){ //loop through all the u-wires in the CC
			EXOUWireSignal* cu = cc->GetUWireSignalAt(l);
			if(abs(cu->fChannel - t->channel) > 1){ //must be the neighboring channel
				continue;
			}
			double timediff = cu->fTime - t->MixColTime;
			if(abs(timediff) < abs(IsIndUsigtimediff)){
				IsIndUsigtimediff = timediff;
				bestUmatch = l;
			}
		}
		//cout << "BeforeComparison " << IsIndUsigtimediff << ", " << besttimediff << bestcc << endl; 
		if(abs(IsIndUsigtimediff) < abs(besttimediff)){
			besttimediff = IsIndUsigtimediff;
			bestcc = k;
		}
	}
	//Set the closest cluster info in the infotup structure
	t->ClusterBelong = bestcc;
	t->DiffTimeCluster = besttimediff;
}

struct infotup{int count; int channel; double MixColPar; double MixColParEr; double MixColTime; double MixColTimeEr; int ClusterBelong; double DiffTimeCluster; bool IsDuplicate; int UInCclose;} ;
//TH1D* timedifferences = new TH1D("timedifferences","Time to Cluster",120,-.06e6,.06e6);
//TImage* img = TImage::Create();

EXOAnalysisModule::EventStatus EXOUWireMixSigAddModule::ProcessEvent(EXOEventData *ED)
{
	typedef vector<infotup> IsIndL; // make a vector of infotup ; one for each IsInduction tagged signal
  
	IsIndL Uinfo;
	int countIsInd = 0;
	size_t nuw = ED->GetNumUWireSignals();
	for(size_t i=0; i<nuw; i++){
		//cout << "Into For loop Is Ind" << endl;
		EXOUWireSignal* u = ED->GetUWireSignal(i);
		if(u->fIsInduction == true){
			int ccbelong = -1; //Will be set in later method when most suitable cluster is found.
			int uincbelong = -1; //Set later when uwire in the cc closest to the IsInd tag is found.
			double closesttime = 2e6; // Will be set later method time difference to the UWire in most suitable cluster
			bool duplicate = false; // Set later, True if the IsInduction Signal is a duplicate, this avoids double counting the same fit results
			//double clusttime = 2e6; //Set Later
			//double closeutime = 2e6; //Set Later
			//build the tuple
			infotup t = { countIsInd, u->fChannel, u->fCorrectedEnergy, u->fMixSigColErr, u->fMixSigColTime, u->fMixSigColErrTime, ccbelong, closesttime, duplicate, uincbelong}; //make_tuple(countIsInd, u->fChannel, u->fMixSigColPar, u->fMixSigColErr, u->fMixSigColTime, u->fMixSigColErrTime);
			countIsInd ++; // increment after so first stored IsInd has count 0
			//add it to the vector
			Uinfo.push_back(t);
		}//end if(IsInd)
	}// end for(uwire)
	
	//Now for each infotup (IsInduction tag) find the closest cluster
	for(size_t j=0; j<Uinfo.size(); j++){
		if(Uinfo.size() == 0){
			continue;
		}
		FindCluster(ED,&(Uinfo[j])); //finds a "best" cc for each IsInd Tag
	}
	
	// Now have a vector called Uinfo with each entry a tuple with info for each IsInd signal found
	// So far observed multiple IsInd tags in an event have been on the same chan within a couple micro seconds. The comnbined fit
	// returns the same fit parameters for each. Let's crop the vector to be only the signals to add for the relevant clusters.
	//This means turning the IsDuplicate tag to True.
	if( Uinfo.size() > 1){
		for(size_t k=0; k<Uinfo.size(); k++){
			for(size_t l=k+1; l<Uinfo.size(); l++){
				if((Uinfo[k].channel == Uinfo[l].channel) && (Uinfo[k].ClusterBelong == Uinfo[l].ClusterBelong)){ //assumes duplicates must be on same channel and they were related to the same CC
					if(Uinfo[k].DiffTimeCluster <= Uinfo[l].DiffTimeCluster){
						Uinfo[k].IsDuplicate = true;
					}
					else{
						Uinfo[l].IsDuplicate = false;
					}
				}
			}
		}
	}
	
	//Let's add the bestcluster and Uwire in the cluster info back to the IsInd Tagged UWireSignal
	int count1 = 0;
	for(size_t n=0; n<nuw; n++){
		EXOUWireSignal* U = ED->GetUWireSignal(n);
		if(U->fIsInduction == true){
			infotup b = Uinfo[count1];
			U->fMixSigClusterFound = b.ClusterBelong;
			U->fMixSigUinClustFound = b.UInCclose;
			U->fMixSigDuplicateTag = b.IsDuplicate;
			count1++;
		}
	}
	
	//Diagnostic: Gather all the time differences for non-duplicate tags. Fkor plotting the distributions of time differences to tune the threshold parameter.
	//if(Uinfo.size() > 0){
		//for(size_t n=0; n<Uinfo.size(); n++){
			//if(Uinfo[n].IsDuplicate == false){
				//timedifferences.push_back(Uinfo[n].DiffTimeCluster);
				//timedifferences->Fill(Uinfo[n].DiffTimeCluster);
			//}
		//}
	//}
	
	// Picked the best charge cluster and marked duplicate tags, now add the IsInd parameters to the relevant cluster.
	if(Uinfo.size() > 0){
		for(size_t m=0; m<Uinfo.size(); m++){
			infotup t = Uinfo[m];
			//cout << "ClusterBelong " << t.ClusterBelong << endl;
			cout << "TimeDiffLimit " << fSignalTimeDiffLimit << endl;
			if((t.ClusterBelong >= 0) && (abs(t.DiffTimeCluster) <= fSignalTimeDiffLimit) && (t.IsDuplicate == false)){ //the fSignalTimeDiffLimit is the maximum time difference between the IsInduction tag and CC can be and still be added.
				EXOChargeCluster* ncc = ED->GetChargeCluster(t.ClusterBelong);
				
                                //ncc->fCorrectedEnergy += t.MixColPar; //the MixColPar is actually the UWire->fCorrectedEnergy if you look back where the infotup is constructed.
				//ncc->fPurityCorrectedEnergy += t.MixColPar;
				if(t.MixColPar >= fTagEnergyCut){
                                ncc->fCorrectedEnergyMix       = ncc->fCorrectedEnergy       + t.MixColPar;
                                ncc->fPurityCorrectedEnergyMix = ncc->fPurityCorrectedEnergy + t.MixColPar;
				}
				else{
					ncc->fCorrectedEnergyMix = 0.0;
					ncc->fPurityCorrectedEnergyMix = 0.0;
				}
			}
		}
	}
	
	return kOk;
}//end process event


EXOAnalysisModule::EventStatus EXOUWireMixSigAddModule::EndOfRun(EXOEventData *ED)
{
  cout << "At EndOfRun for " << GetName() << endl;
  
  //TFile f("/nfs/slac/g/exo-userdata/users/mrcoon/TimeDistr.root","new");
  //TH1D* ClosestClusterTimes = new TH1D(timedifferences);
  
  //TCanvas* c1 = new TCanvas;
  //TString fout = "/nfs/slac/g/exo-userdata/users/mrcoon/TimeDiff.png";
  //timedifferences->Draw();
  //TImage* img = TImage::Create();
  //img->FromPad(c1);
  //img->WriteImage(fout.Data());
  
  return kOk;
}

int EXOUWireMixSigAddModule::TalkTo(EXOTalkToManager *talktoManager)
{

  // Create some example commands

  // example double command

  talktoManager->CreateCommand("/umixedsignaladd/SignalTimeDiffLimit","Set the maximum time difference for signal to be added to cluster", 
           this, fSignalTimeDiffLimit, &EXOUWireMixSigAddModule::SetSignalTimeDiffLimit);

  // example int command

  talktoManager->CreateCommand("/umixedsignaladd/TagEnergyCut","Sets minimum tag energy required to be included", 
           this, fTagEnergyCut, &EXOUWireMixSigAddModule::SetTagEnergyCut );
  
  // example bool command

  talktoManager->CreateCommand("ANewBoolCommand","read in a bool", 
           this, false, &EXOUWireMixSigAddModule::SetBoolParam );
  
  // example string command

  talktoManager->CreateCommand("ANewStringCommand","read in a string", 
           this, "", &EXOUWireMixSigAddModule::SetStringParam );

  // example generic call-back 

  talktoManager->CreateCommand("ANewCallBackCommand","generic call-back", 
           this, &EXOUWireMixSigAddModule::CallBack );

  return 0;
}

int EXOUWireMixSigAddModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}

void EXOUWireMixSigAddModule::CallBack()
{
  std::cout << "EXOUWireMixSigAddModule::CallBack() called. " << std::endl;
}


