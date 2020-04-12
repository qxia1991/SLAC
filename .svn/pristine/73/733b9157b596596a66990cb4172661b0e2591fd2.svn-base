//__________________________________________________________________________________
// EXOFastLightSim
//
//9/26/2017
//
//Light Simulation for EXO-200 MC
//This will replace the current simulation (fast_APD_simulation) in EXOEventAction.cc which 
//used a fit to the light map to extract the yield as a function of position for each APD plane.
//
//This new simulation correctly determines the channel by channel response for each event using
//the location of each PCD and looking up the response in Clayton's Channel Light Map.
//
//Also at the moment the fast_APD_simulation still runs so need to Clear out all that info first.
//Probably could get around this if we rewrite fast_APD_simulation here as a switch but than not back
//compatible.
//
//This also looks up the Gains and the Dead channels so we should be careful about the time we
//put into the EventHeader in the TalkTo Commands
//

#include "EXOAnalysisManager/EXOFastLightSim.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "TGraph.h"
#include "TH3D.h"
#include "TArrayI.h"
#include "TFile.h"
#include "TRandom.h"
#include <iomanip>
#include <sstream>
#include <iostream>
using CLHEP::mm;
using CLHEP::MeV;

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOFastLightSim, "fast-light")

EXOFastLightSim::EXOFastLightSim()
: fRootFile(NULL), 
  fScintYield(1521.74),
  fDoFastSim(true),
  fDBTime(1348000000),
  fLMAvg(1883.0),
  fAddShotNoise(true),
  fSkipLightMap(false)
{
    SetFilename("data/lightmap/LightMaps.root");
}


int EXOFastLightSim::Initialize()
{
  // Read in the lightmaps and gain corrections.

  //Ensure we retain current directory, so we can switch back to it at the end.
  TDirectory* CurrentDir = gDirectory;
  std::string FullFilePath = EXOMiscUtil::SearchForFile(fFilename);
  if(FullFilePath == "") LogEXOMsg("Unable to find lightmap file " + fFilename, EEAlert);
  std::cout << "Using LM File in Fast Sim: " << FullFilePath.c_str() << std::endl;
  fRootFile = TFile::Open(FullFilePath.c_str());

  TArrayI* APDs = (TArrayI*)fRootFile->GetObjectUnchecked("APDs");
  for(Int_t i = 0; i < APDs->GetSize(); i++) fAPDs.push_back(APDs->At(i));
  for(size_t i = 0; i < fAPDs.size(); i++) {
    unsigned char gang = fAPDs[i];
    // Get the lightmaps.
    std::ostringstream lightmapname;
    lightmapname << "lightmap_" << std::setw(3) << std::setfill('0') << int(gang);
    fLightMaps[fAPDs[i]] = (TH3D*)fRootFile->Get(lightmapname.str().c_str());

    // Get the gainmaps.
    std::ostringstream gainmapname;
    gainmapname << "gainmap_" << std::setw(3) << std::setfill('0') << int(gang);
    fGainMaps[fAPDs[i]] = (TGraph*)fRootFile->Get(gainmapname.str().c_str());
  }

  // We certainly don't want to claim current directory here, so revert back to what it formerly was.
  if(CurrentDir and CurrentDir != gDirectory) CurrentDir->cd();

  return 0;
}

EXOAnalysisModule::EventStatus EXOFastLightSim::BeginOfRun(EXOEventData *ED)
{
    std::cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber
        << " status = " << status << std::endl;

    return kOk;
}


int EXOFastLightSim::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/fast-light/file",
                               "The root file containing the lightmap and gainmap.",
                               this,
                               fFilename,
                               &EXOFastLightSim::SetFilename);

  talktoManager->CreateCommand("/fast-light/ScintYield",
                               "The Scintillation Yield to convert Energy to ...",
                               this,
                               fScintYield,
                               &EXOFastLightSim::SetScintYield);
  
    talktoManager->CreateCommand("/fast-light/SetLMAvg",
                                 "The AVG LM response to reproduce the correct Counts Scale ...",
                                 this,
                                 fLMAvg,
                                 &EXOFastLightSim::SetLMAvg);

  talktoManager->CreateCommand("/fast-light/DoFastSim",
                               "Whether to do the old fast APD sim or not.",
                               this,
                               fDoFastSim,
                               &EXOFastLightSim::SetDoFastSim);
  
  talktoManager->CreateCommand("/fast-light/SetDBTime",
                               "DB Time to use when looking up LM Gains (Should match one in Digi)",
                               this,
                               fDBTime,
                               &EXOFastLightSim::SetDBTime);

  talktoManager->CreateCommand("/fast-light/AddShotNoise",
                               "Whether to Add shot noise to the APD Hits when calculating",
                               this,
                               fAddShotNoise,
                               &EXOFastLightSim::SetAddShotNoise);

  talktoManager->CreateCommand("/fast-light/SkipLightMap",
                               "Whether to add in the Light Map when doing the APD sim.",
                               this,
                               fSkipLightMap,
                               &EXOFastLightSim::SetSkipLightMap);

  return 0;
}

void EXOFastLightSim::ClearAPDHits(EXOMonteCarloData& MonteCarloData)
{
  size_t nhits = MonteCarloData.GetNumAPDHits();
  if (nhits == 0) {return;}
  std::vector<EXOMCAPDHitInfo*> ApdHitInfoOld;
  for (size_t i=0; i < nhits; i++) {
    EXOMCAPDHitInfo* apdInfo = const_cast<EXOMCAPDHitInfo*>(MonteCarloData.GetAPDHit(i));
    ApdHitInfoOld.push_back(apdInfo);
  }
  for(size_t i=0;i<ApdHitInfoOld.size();i++){
    MonteCarloData.Remove(ApdHitInfoOld[i]);
  }
  return;
}

void EXOFastLightSim::GetAcceptance(double z, double r, double& acceptance1, double& acceptance2)
{
    //Acceptance is ~1.0 flucuates around 1.0
    if (fSkipLightMap){
        acceptance1 = 0.5;
        acceptance2 = 0.5;
    }

    double tpc_1[19] = {-3.04490419e-06,  -1.14552389e-05,   4.06603102e-04,   1.10250692e-03,
                        -1.00402797e-06,   2.58881357e-04,   2.49008046e-03,   9.80160799e-03,
                         1.72719919e-03,   1.32738693e-03,   5.42197108e-01,   1.23273173e-08,
                        -4.55571441e-07,   7.53887444e-08,   1.43239443e-08,  -2.71258542e-10,
                        -1.32014091e-08,   1.03377322e-09,   1.10489232e-10};

    double tpc_2[19] = {-5.08213328e-06,  -2.17202349e-06,   4.09534631e-04,   4.73603365e-04,
                        -2.88758584e-06,  -2.25485917e-04,  -8.65997007e-04,  -9.74660326e-03,
                        -3.45256397e-03,  -3.30521012e-03,   5.15677779e-01,   7.05382849e-09,
                         3.73849026e-07,  -1.00465687e-07,  -2.99732744e-08,   7.08318613e-11,
                         1.24872754e-08,  -1.25193358e-09,  -3.48909121e-10};
 
    // new fitting function with four sets of parameters for different values of z
    // fit_1 = a1 * r * r + b1 * r + c1 * z * z + d1 * z + e  + f1 * r * r * z +  g1 * z * z * r * r (>0)
    // fit_2 = a2 * r * r + b2 * r + c2 * z * z + d2 * z + e  + f2 * r * r * z +  g2 * z * z * r * r [-25, 0)
    // fit_3 = a1 * r * r + b1 * r + c1 * z * z + d3 * z + e  + f3 * r * r * z +  g3 * z * z * r * r [-85, -25)
    // fit_4 = a1 * r * r + b1 * r + c1 * z * z + d4 * z + e  + f4 * r * r * z +  g4 * z * z * r * r (< -85)
    // {a1, a2, b1, b2, c1, c2, d1, d2, d3, d4, e, f1, f2, f3, f4, g1, g2, g3, g4}


    if (z > 0.) {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[6] * z + tpc_1[10] +
        tpc_1[11] * r * r * z + tpc_1[15] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[6] * z + tpc_2[10] +
        tpc_2[11] * r * r * z + tpc_2[15] * r * r * z * z;

    } else if (z < 0. && z >= -25.){
      acceptance1 = tpc_1[1] * r * r + tpc_1[3] * r + tpc_1[5] * z * z + tpc_1[7] * z + tpc_1[10] +
        tpc_1[12] * r * r * z + tpc_1[16] * r * r * z * z;

      acceptance2 = tpc_2[1] * r * r + tpc_2[3] * r + tpc_2[5] * z * z + tpc_2[7] * z + tpc_2[10] +
        tpc_2[12] * r * r * z + tpc_2[16] * r * r * z * z;

    } else if (z < -25. && z >= -85.) {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[8] * z + tpc_1[10] +
        tpc_1[13] * r * r * z + tpc_1[17] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[8] * z + tpc_2[10] +
        tpc_2[13] * r * r * z + tpc_2[17] * r * r * z * z;

    } else {
      acceptance1 = tpc_1[0] * r * r + tpc_1[2] * r + tpc_1[4] * z * z + tpc_1[9] * z + tpc_1[10] +
        tpc_1[14] * r * r * z + tpc_1[18] * r * r * z * z;

      acceptance2 = tpc_2[0] * r * r + tpc_2[2] * r + tpc_2[4] * z * z + tpc_2[9] * z + tpc_2[10] +
        tpc_2[14] * r * r * z + tpc_2[18] * r * r * z * z;
    }

}

void EXOFastLightSim::FastAPDSim(EXOMonteCarloData& MonteCarloData)
{
  //This is copied over from EXOEventAction.cc fast_APD_simulation 
  //the parameters come from a fit to the Light Map (Clayton's)

  // Fast APD simulation, returns the total photons and the number of hits on
  // the two APD planes.
  // Coefficients are normalized so that the acceptance is between 0 and 1
  // parameters = {a1, a2, b1, b2, c1, c2, d1, d2, d3, d4, e, f1, f2, f3, f4, g1, g2, g3, g4}

  double num_APD1_hits = 0, num_APD2_hits = 0;
  double total_energy = 0.0;

  size_t numberLXehits = MonteCarloData.GetNumPixelatedChargeDeposits();
  for (size_t i=0; i<numberLXehits; i++) {
    const EXOMCPixelatedChargeDeposit* pcd = MonteCarloData.GetPixelatedChargeDeposit(i);
    double hit_etotal = pcd->fTotalEnergy;
    if ( hit_etotal == 0.0 ) continue;

    EXOCoordinates coords  = pcd->GetPixelCenter();
    double hit_time      = coords.GetT();
    double r = sqrt(coords.GetX()*coords.GetX() + coords.GetY()*coords.GetY())/mm;
    double z = coords.GetZ()/mm;
    double acceptance1 = 0.0, acceptance2 = 0.0;
    GetAcceptance(z,r,acceptance1,acceptance2);

    //std::cout << "Accept "<< acceptance1 << " " << acceptance2 << std::endl;
    
    total_energy += hit_etotal;
    int tempNum_APD1_hits = (int)(fScintYield * hit_etotal * acceptance1/MeV);
    int tempNum_APD2_hits = (int)(fScintYield * hit_etotal * acceptance2/MeV);
    num_APD1_hits += tempNum_APD1_hits;
    num_APD2_hits += tempNum_APD2_hits;

    // Now add these on to the different planes
    EXOMCAPDHitInfo apdHit;
    apdHit.fTime = hit_time;
    if ( tempNum_APD1_hits > 0) {
      double APD1_smear = gRandom->Gaus(tempNum_APD1_hits, sqrt((double)(tempNum_APD1_hits)));
      apdHit.fCharge = APD1_smear/NUMBER_APD_CHANNELS_PER_PLANE;
      for ( int iApd = 0; iApd < NUMBER_APD_CHANNELS_PER_PLANE; iApd++ ) {
        apdHit.fGangNo = iApd;
        MonteCarloData.AddAPDHitInfo(apdHit);
      }
    }
    if ( tempNum_APD2_hits > 0) {
      double APD2_smear = gRandom->Gaus(tempNum_APD2_hits,sqrt((double)(tempNum_APD2_hits)));
      apdHit.fCharge = APD2_smear/NUMBER_APD_CHANNELS_PER_PLANE;
      for ( int iApd = 0; iApd < NUMBER_APD_CHANNELS_PER_PLANE; iApd++ ) {
        apdHit.fGangNo = NUMBER_APD_CHANNELS_PER_PLANE + iApd;
        MonteCarloData.AddAPDHitInfo(apdHit);
      }
    }


  }
  MonteCarloData.fTotalPhotons = (int)(total_energy * fScintYield/MeV);

  // Add in a gaussian smearing term

  double APD1_smear = 0.0;
  double APD2_smear = 0.0;

  if ( num_APD1_hits > 0 ) {
    APD1_smear = gRandom->Gaus(0.0, sqrt((double)(num_APD1_hits)));
  }
  if ( num_APD2_hits > 0 ) {
    APD2_smear = gRandom->Gaus(0.0, sqrt((double)(num_APD2_hits)));
  }

  num_APD1_hits += (int)(APD1_smear);
  num_APD2_hits += (int)(APD2_smear);
  
  MonteCarloData.fTotalHitsArrayOne = num_APD1_hits;
  MonteCarloData.fTotalHitsArrayTwo = num_APD2_hits;

}

EXOAnalysisModule::EventStatus EXOFastLightSim::ProcessEvent(EXOEventData* ED)
{
  EXOMonteCarloData& MonteCarloData = ED->fMonteCarloData;    
  //At the moment fast_APD_sim runs in EventAction.cc so need to clear out that sim first
  ClearAPDHits(MonteCarloData);
  
  //std::cout << "Energy " << MonteCarloData.fTotalEnergyInLiquidXe << std::endl;
  //std::cout << " Pre  " << MonteCarloData.fTotalPhotons << "  " << MonteCarloData.fTotalHitsArrayOne  << " " << MonteCarloData.fTotalHitsArrayTwo << std::endl;

  if (fDoFastSim){
      //Do the old simulation and skip the new one
      FastAPDSim(MonteCarloData);
      return kOk;
  }

  //std::cout << " Post1  " << MonteCarloData.fTotalPhotons << "  " << MonteCarloData.fTotalHitsArrayOne  << " " << MonteCarloData.fTotalHitsArrayTwo << " " << MonteCarloData.fTotalHitsArrayOne+MonteCarloData.fTotalHitsArrayTwo << std::endl;
  
  FullLightMapSim(MonteCarloData);
  
  //std::cout << " Post2  " << MonteCarloData.fTotalPhotons << "  " << MonteCarloData.fTotalHitsArrayOne  << " " << MonteCarloData.fTotalHitsArrayTwo << std::endl;
  //std::cout << std::endl;

  return kOk;
}

void EXOFastLightSim::FullLightMapSim(EXOMonteCarloData& MonteCarloData)
{
  size_t numberLXehits = MonteCarloData.GetNumPixelatedChargeDeposits();
  Double_t num_APD1_hits = 0, num_APD2_hits = 0;
  
  for (size_t i=0; i<numberLXehits; i++) {
      const EXOMCPixelatedChargeDeposit* pcd = MonteCarloData.GetPixelatedChargeDeposit(i);
      
      //Only do this if the PCD deposited energy
      Double_t hit_etotal = pcd->fTotalEnergy/MeV;
      if ( hit_etotal == 0.0 ) continue;

      //Extract the PCD coordinates
      EXOCoordinates coords  = pcd->GetPixelCenter();
      Double_t hit_time      = coords.GetT();
      Double_t pcdX = coords.GetX()/CLHEP::mm;;
      Double_t pcdY = coords.GetY()/CLHEP::mm;
      Double_t pcdZ = coords.GetZ()/CLHEP::mm;
      
      //std::cout << "PCD Pos: " << pcdX <<  " " << pcdY << " " << pcdZ << std::endl;

      EXOMCAPDHitInfo apdHit;
      apdHit.fTime = hit_time;
      double total = 0.0;
      
      for(size_t iApd = 0; iApd < 2*NUMBER_APD_CHANNELS_PER_PLANE; iApd++){
          
          apdHit.fGangNo = iApd;
          size_t chNum = iApd + NWIREPLANE*NCHANNEL_PER_WIREPLANE;
          unsigned char gang = static_cast<unsigned char>(chNum);
          if(IsChannelMissing(gang)){
              //This channel is missing so no energy.
              //Do we add a 0 energy hit??
              apdHit.fCharge = 0.0;
              //MonteCarloData.AddAPDHitInfo(apdHit)
              continue;
          }
          
          TAxis* Xaxis = fLightMaps[gang]->GetXaxis();
          TAxis* Yaxis = fLightMaps[gang]->GetYaxis();
          TAxis* Zaxis = fLightMaps[gang]->GetZaxis();
          double ChYield = 0.0 ;
          double ChGain = fGainMaps[gang]->Eval(fDBTime);

          if(Xaxis->GetBinCenter(1) <= pcdX and  pcdX < Xaxis->GetBinCenter(Xaxis->GetNbins()) and
             Yaxis->GetBinCenter(1) <= pcdY and  pcdY < Yaxis->GetBinCenter(Yaxis->GetNbins()) and
             Zaxis->GetBinCenter(1) <= pcdZ and  pcdZ < Zaxis->GetBinCenter(Zaxis->GetNbins())) {     
              ChYield = fLightMaps[gang]->Interpolate(pcdX, pcdY, pcdZ);
          }
          else {
              // Interpolate would return 0, and I'm actually OK with that -- but I still want to kill the warning.
              ChYield = 0;
          }
         
          //std::cout << "HitT " << hit_time << " Zpos " << pcdZ  << " Gang " << chNum << " Yield " << ChYield  << " Gain " << ChGain << std::endl;
          
          double tempHits=0.0;
          if (fSkipLightMap){
              tempHits = hit_etotal*fScintYield/(2*NUMBER_APD_CHANNELS_PER_PLANE);
          }
          else{
              tempHits = hit_etotal*ChYield*ChGain*fScintYield/fLMAvg;
          }

          
          if (fAddShotNoise){
              apdHit.fCharge = gRandom->Gaus(tempHits,sqrt((double)(tempHits)));
          }
          else {
              apdHit.fCharge = tempHits;
          }
          total += apdHit.fCharge;
          MonteCarloData.AddAPDHitInfo(apdHit);

          if(EXOMiscUtil::kNorth == EXOMiscUtil::GetTPCSide(chNum)){num_APD1_hits += apdHit.fCharge;}
          else if (EXOMiscUtil::kSouth == EXOMiscUtil::GetTPCSide(chNum)){num_APD2_hits += apdHit.fCharge;}
          else {LogEXOMsg("Error finding side of the APD", EEAlert);}
      }
      //std::cout << "------- Total Yield = " << total << std::endl;   
  }
  
  MonteCarloData.fTotalHitsArrayOne = num_APD1_hits;
  MonteCarloData.fTotalHitsArrayTwo = num_APD2_hits;
  MonteCarloData.fTotalPhotons      = num_APD1_hits + num_APD2_hits;

}

bool EXOFastLightSim::IsChannelMissing(unsigned char gang)
{
    //Is this channel in the APD list loaded from the LM or is it 
    //missing (I assume a dead channel)
    if(std::find(fAPDs.begin(), fAPDs.end(), gang) != fAPDs.end()){
        return false;
    }
    return true;
}

EXOAnalysisModule::EventStatus EXOFastLightSim::EndOfRun(EXOEventData *ED)
{
    std::cout << "At EndOfRun for " << GetName() << std::endl;
    return kOk;
}

EXOFastLightSim::~EXOFastLightSim()
{
    delete fRootFile; // Should delete objects read from the root file too, although this doesn't always work.
}



