//__________________________________________________________________________________
// EXOWeightedAPDEnergy
//
// Steve's mainline light map correction module is called EXOAPDPosCorrModule, and
// has the nickname "apdposcorr."  You probably (6/19/2013) want that one.  This class is a newer
// version written by Clayton, and uses apd-by-apd lightmaps.  It is not known yet
// whether this will be adopted in the main analysis.
//
// For each apd, there is a lightmap (TH3D) and gainmap (TGraph).  The product
// of the two gives a prediction of the signal expected on an APD from a 2615-keV cluster.

#include "EXOAnalysisManager/EXOWeightedAPDEnergy.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TGraph.h"
#include "TH3D.h"
#include "TArrayI.h"
#include "TFile.h"
#include <iomanip>
#include <sstream>

IMPLEMENT_EXO_ANALYSIS_MODULE(EXOWeightedAPDEnergy, "weighted-apd")

EXOWeightedAPDEnergy::EXOWeightedAPDEnergy()
: fRootFile(NULL)
{
  SetFilename("data/lightmap/LightMaps.root");
}

int EXOWeightedAPDEnergy::Initialize()
{
  // Read in the lightmaps and gain corrections.

  // Ensure we retain current directory, so we can switch back to it at the end.
  TDirectory* CurrentDir = gDirectory;

  std::string FullFilePath = EXOMiscUtil::SearchForFile(fFilename);
  if(FullFilePath == "") LogEXOMsg("Unable to find lightmap file " + fFilename, EEAlert);
  fRootFile = TFile::Open(FullFilePath.c_str());

  // Get the list of active APDs.
  // I'm not sure what format I'll save them with; but I know what format I'll save them into.
  // temp code:
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

int EXOWeightedAPDEnergy::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/weighted-apd/file",
 	          "The root file containing the lightmap and gainmap.",
 	          this,
 	          fFilename,
                  &EXOWeightedAPDEnergy::SetFilename);
  return 0;
}

EXOAnalysisModule::EventStatus EXOWeightedAPDEnergy::ProcessEvent(EXOEventData* ED)
{
  // Fills EXOScintillationCluster::fWeightedAPDEnergy.
  // Where we are unable to fill it properly, set it to zero.
  for(size_t i = 0; i < ED->GetNumScintillationClusters(); i++) {
    EXOScintillationCluster* scint = ED->GetScintillationCluster(i);
    scint->fWeightedAPDEnergy = 0;
    std::map<unsigned char, double> YieldFromAPD;

    // Collect clusters, for convenience.
    std::vector<const EXOChargeCluster*> Clusters;
    double TotalCharge = 0;
    for(size_t iclu = 0; iclu < scint->GetNumChargeClusters(); iclu++) {
      const EXOChargeCluster* clu = scint->GetChargeClusterAt(iclu);
      if(std::abs(clu->fX) > 200 or std::abs(clu->fY) > 200 or std::abs(clu->fZ) > 200) continue;
      TotalCharge += clu->fPurityCorrectedEnergy;
      Clusters.push_back(clu);
    }
    if(TotalCharge <= 1) continue;

    for(size_t iapd = 0; iapd < fAPDs.size(); iapd++) {
      unsigned char gang = fAPDs[iapd];
      YieldFromAPD[gang] = 0.;

      // Get the weighted-average position contributions of all clusters.
      for(size_t iclu = 0; iclu < Clusters.size(); iclu++) {
        const EXOChargeCluster* clu = Clusters[iclu];

        // Make sure cluster is in the proper range for interpolation -- else return 0.
        Double_t LightMapYield;
        TAxis* Xaxis = fLightMaps[gang]->GetXaxis();
        TAxis* Yaxis = fLightMaps[gang]->GetYaxis();
        TAxis* Zaxis = fLightMaps[gang]->GetZaxis();
        if(Xaxis->GetBinCenter(1) <= clu->fX and clu->fX < Xaxis->GetBinCenter(Xaxis->GetNbins()) and
           Yaxis->GetBinCenter(1) <= clu->fY and clu->fY < Yaxis->GetBinCenter(Yaxis->GetNbins()) and
           Zaxis->GetBinCenter(1) <= clu->fZ and clu->fZ < Zaxis->GetBinCenter(Zaxis->GetNbins())) {
          LightMapYield = fLightMaps[gang]->Interpolate(clu->fX, clu->fY, clu->fZ);
        }
        else {
          // Interpolate would return 0, and I'm actually OK with that -- but I still want to kill the warning.
          LightMapYield = 0;
        }

        YieldFromAPD[gang] += LightMapYield*clu->fPurityCorrectedEnergy;
      }
      YieldFromAPD[gang] /= TotalCharge;

      // Factor in the gain of this APD.
      YieldFromAPD[gang] *= fGainMaps[gang]->Eval(ED->fEventHeader.fTriggerSeconds);

      // YieldFromAPD will equal the magnitude (peak-baseline) expected from a 2615keV cluster.
    }

    // Do a weighted sum of the APD signals.
    double WeightedSum = 0;
    double Denom = 0;
    for(size_t iapd = 0; iapd < fAPDs.size(); iapd++) {
      unsigned char gang = fAPDs[iapd];
      EXOAPDSignal* sig = scint->GetAPDSignal(EXOAPDSignal::kGangFit, gang);
      if(sig == NULL) continue;
      double RawCounts = sig->fRawCounts;
      WeightedSum += RawCounts * YieldFromAPD[gang];
      Denom += YieldFromAPD[gang]*YieldFromAPD[gang];
    }
    scint->fWeightedAPDEnergy = WeightedSum/Denom;
    scint->fWeightedAPDEnergy *= 2615; // Rescale so fWeightedAPDEnergy is roughly in units of keV.

    // Undo reconstruction scaling factor.
    scint->fWeightedAPDEnergy /= APD_ADC_FULL_SCALE_ELECTRONS /(ADC_BITS * APD_GAIN);
  }

  return kOk;
}

EXOWeightedAPDEnergy::~EXOWeightedAPDEnergy()
{
  delete fRootFile; // Should delete objects read from the root file too, although this doesn't always work.
}
