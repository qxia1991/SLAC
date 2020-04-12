//___________________________________________________
//
// Fit for scintillation magnitudes in a noise-tolerant fashion.
// For more information, please see the slides and derivation from the Energy Meeting
// on July 22, 2013:
// https://confluence.slac.stanford.edu/display/exo/Energy+Meeting+22+July+2013
// (A full note will be written up in the near future.)
//
// Limitations:
// * Although this algorithm can be extended to handle events with more than one scintillation cluster,
//   for simplicity it does not currently do so.
// * This algorithm could also be extended to simultaneously refit wires.  Again, not done yet.
// * The APD gains may not be right, which results in a suboptimal energy resolution.
// * Need to make noise correlations and APD gains time-dependent parameters, somehow or other.
// * Speed is a limiting factor in implementation of this method; try to optimize more.
// * In principle we could extract cluster-by-cluster light yield down the road.
// * It should be fairly cheap to also extract the estimated fit error due to electronic and Poisson noise.

#include "EXOAnalysisManager/EXORefitAPDs.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXONoiseCorrelations.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOWaveformFT.hh"
#include "EXOUtilities/EXOFastFourierTransformFFTW.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "TFile.h"
#include "TArrayI.h"
#include "TH3D.h"
#include "TGraph.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cassert>

IMPLEMENT_EXO_ANALYSIS_MODULE(EXORefitAPDs, "refit-apds")

int EXORefitAPDs::TalkTo(EXOTalkToManager* tm)
{
  tm->CreateCommand("/refit-apds/lightfile",
                    "The root file containing the lightmap and gainmap.",
                    this,
                    fLightmapFilename,
                    &EXORefitAPDs::SetLightmapFilename);
  tm->CreateCommand("/refit-apds/noisefile",
                    "The root file containing the relevant noise correlations.",
                    this,
                    fNoiseFilename,
                    &EXORefitAPDs::SetNoiseFilename);
  tm->CreateCommand("/refit-apds/termination_thresh",
                    "Terminate when |r| < threshold",
                    this,
                    fRThreshold,
                    &EXORefitAPDs::SetRThreshold);
  return 0;
}

int EXORefitAPDs::Initialize()
{
  // Open the noise and lightmap files, and extract their information.

  // Neither of these files should become the current directory.  We'll need to switch back.
  TDirectory* FormerDir = gDirectory;

  // Read in the noise correlations.
  TFile* NoiseFile = TFile::Open(fNoiseFilename.c_str());
  EXONoiseCorrelations* NoiseCorr =
    dynamic_cast<EXONoiseCorrelations*>(NoiseFile->Get("EXONoiseCorrelations"));

  // Read in the lightmap stuff.
  fLightmapFile = TFile::Open(fLightmapFilename.c_str());

  // Get the list of active APDs.
  TArrayI* APDs = (TArrayI*)fLightmapFile->GetObjectUnchecked("APDs");
  for(Int_t i = 0; i < APDs->GetSize(); i++) {
    // All gangs in APDs are tracked by the lightmap.
    // Ensure they're also tracked by EXONoiseCorrelations.
    if(not NoiseCorr->HasChannel(APDs->At(i))) continue;
    fAPDs.push_back(APDs->At(i));
  }

  for(size_t i = 0; i < fAPDs.size(); i++) {
    unsigned char gang = fAPDs[i];

    // Get the lightmaps.
    std::ostringstream lightmapname;
    lightmapname << "lightmap_" << std::setw(3) << std::setfill('0') << int(gang);
    fLightMaps[gang] = (TH3D*)fLightmapFile->Get(lightmapname.str().c_str());

    // Get the gainmaps.
    std::ostringstream gainmapname;
    gainmapname << "gainmap_" << std::setw(3) << std::setfill('0') << int(gang);
    fGainMaps[gang] = (TGraph*)fLightmapFile->Get(gainmapname.str().c_str());
  }

  // EXONoiseCorrelations is actually kind of inefficient for me, as it turns out.
  // (A little embarrassing, since I designed it to be used here.)
  // Anyway, translate it once into a more useful format, and think later about if there's a better option.
  // Note that we drop the 0-frequency component, which isn't used.
  for(size_t i = 0; i < fAPDs.size(); i++) {
    for(size_t j = 0; j < fAPDs.size(); j++) {
      unsigned char gangi = fAPDs[i];
      unsigned char gangj = fAPDs[j];
      unsigned char noiseindex_i = NoiseCorr->GetIndexOfChannel(gangi);
      unsigned char noiseindex_j = NoiseCorr->GetIndexOfChannel(gangj);

      // Fill fNoiseRI.
      std::vector<double>& ri = fNoiseRI[std::make_pair(gangi, gangj)];
      ri.resize(1024);
      for(size_t f = 0; f < 1024; f++) ri[f] = NoiseCorr->GetRI(f+1)[noiseindex_i][noiseindex_j];

      if(gangi > gangj) continue; // The other matrices are symmetric, so force gangi <= gangj.

      // Fill fNoiseRRandII, alternating between RR and II.
      std::vector<double>& rr_and_ii = fNoiseRRandII[std::make_pair(gangi, gangj)];
      rr_and_ii.resize(2*1024-1);
      for(size_t f = 0; f < 1024; f++) {
        rr_and_ii[2*f] = NoiseCorr->GetRR(f+1)[noiseindex_i][noiseindex_j];
        if(f != 1023) rr_and_ii[2*f+1] = NoiseCorr->GetII(f+1)[noiseindex_i][noiseindex_j];
      }
    }
  }
  delete NoiseFile; // Should delete NoiseCorr as well; but worth confirming if I can.  

  // In spite of opening new files, we need to switch the current directory back to what it was before.
  if(FormerDir) FormerDir->cd();

  fWatch_ProcessEvent.Reset();
  fWatch_Solve.Reset();
  fWatch_MatrixMul.Reset();
  fWatch_MatrixMul_NoiseTerms.Reset();
  return 0;
}

EXOAnalysisModule::EventStatus EXORefitAPDs::ProcessEvent(EXOEventData *ED)
{
  fWatch_ProcessEvent.Start(false);

  // Start by making sure to reset any old values to zero.
  for(size_t i = 0; i < ED->GetNumScintillationClusters(); i++) {
    ED->GetScintillationCluster(i)->fDenoisedEnergy = 0;
  }

  // If we don't have previously-established scintillation times, we can't do anything -- skip.
  if(ED->GetNumScintillationClusters() == 0) {
    fWatch_ProcessEvent.Stop();
    return kOk;
  }

  // If the waveforms aren't full-length, skip for now (although we should be able to handle them later).
  if(ED->fEventHeader.fSampleCount != 2047) {
    fWatch_ProcessEvent.Stop();
    return kDrop;
  }

  // For now, we also only deal with events containing *exactly* one scintillation cluster.
  // There's nothing theoretical that warrants this; it's just easier to code up.
  if(ED->GetNumScintillationClusters() != 1) {
    fWatch_ProcessEvent.Stop();
    return kDrop;
  }
  EXOScintillationCluster* scint = ED->GetScintillationCluster(0);

  // If there are no fully-reconstructed clusters, then we can't do anything -- so, skip then too.
  // Otherwise, extract a list of clusters for future convenience.
  std::vector<EXOChargeCluster*> FullClusters;
  for(size_t i = 0; i < scint->GetNumChargeClusters(); i++) {
    EXOChargeCluster* clu = scint->GetChargeClusterAt(i);
    if(std::abs(clu->fX) > 200 or std::abs(clu->fY) > 200 or std::abs(clu->fZ) > 200) continue;
    if(clu->fPurityCorrectedEnergy < 1) continue;
    FullClusters.push_back(clu);
  }
  if(FullClusters.empty()) {
    fWatch_ProcessEvent.Stop();
    return kDrop;
  }

  // Save the unix time of the event (as a double, since ROOT will convert it anyway).
  fUnixTimeOfEvent = double(ED->fEventHeader.fTriggerSeconds);
  fUnixTimeOfEvent += double(ED->fEventHeader.fTriggerMicroSeconds)/1e6;

  // Given the positions of the clusters, estimate how the light should be distributed among gangs.
  // ExpectedYieldPerGang will be the expected peak-baseline (ADC counts) of a 2615 keV event.
  fExpectedYieldPerGang.clear();
  fExpectedEnergy_keV = 0;
  //double TotalPurityCorrectedEnergy = 0;
  for(size_t i = 0; i < fAPDs.size(); i++) fExpectedYieldPerGang[fAPDs[i]] = 0;
  for(size_t i = 0; i < FullClusters.size(); i++) {
    EXOChargeCluster* clu = FullClusters[i];
    //TotalPurityCorrectedEnergy += clu->fPurityCorrectedEnergy;
    fExpectedEnergy_keV += clu->fPurityCorrectedEnergy;
    for(size_t j = 0; j < fAPDs.size(); j++) {
      unsigned char gang = fAPDs[j];
      Double_t GainFuncVal = fGainMaps[gang]->Eval(fUnixTimeOfEvent);

      // Make sure cluster is in the proper range for interpolation -- else return 0.
      Double_t LightMapVal;
      TAxis* Xaxis = fLightMaps[gang]->GetXaxis();
      TAxis* Yaxis = fLightMaps[gang]->GetYaxis();
      TAxis* Zaxis = fLightMaps[gang]->GetZaxis();
      if(Xaxis->GetBinCenter(1) <= clu->fX and clu->fX < Xaxis->GetBinCenter(Xaxis->GetNbins()) and
         Yaxis->GetBinCenter(1) <= clu->fY and clu->fY < Yaxis->GetBinCenter(Yaxis->GetNbins()) and
         Zaxis->GetBinCenter(1) <= clu->fZ and clu->fZ < Zaxis->GetBinCenter(Zaxis->GetNbins())) {
        LightMapVal = fLightMaps[gang]->Interpolate(clu->fX, clu->fY, clu->fZ);
      }
      else {
        // Interpolate would return 0, and I'm actually OK with that -- but I still want to kill the warning.
        LightMapVal = 0;
      }

      fExpectedYieldPerGang[gang] += LightMapVal*GainFuncVal*clu->fPurityCorrectedEnergy;
    }
  }
  // We just want to weight the clusters appropriately when we guess where light should be collected.
  // Divide out to ensure that at the end, a result of 1 corresponds to a 2615 keV event (roughly).
  //for(size_t i = 0; i < fAPDs.size(); i++) fExpectedYieldPerGang[fAPDs[i]] /= TotalPurityCorrectedEnergy;
  for(size_t i = 0; i < fAPDs.size(); i++) fExpectedYieldPerGang[fAPDs[i]] /= fExpectedEnergy_keV;

  // If we don't expect any yield, then clearly there will be a degenerate matrix.
  // So, instead drop such events.
  // (Specifically, if a 2615 keV event would produce less than 1ADC on every gang, drop it.)
  bool HasYield = false;
  for(size_t i = 0; i < fAPDs.size(); i++) {
    if(fExpectedYieldPerGang[fAPDs[i]] > 1) HasYield = true;
  }
  if(not HasYield) {
    fWatch_ProcessEvent.Stop();
    return kDrop;
  }

  // Generate the expected signal shape (normalized), given the time of the scintillation.
  // Alternate between real and imaginary parts, mimicking the variable ordering we use throughout.
  // Also drop the zero-frequency component (which isn't used)
  // and the last imaginary component (which is identically zero).
  EXOWaveformFT modelFT = GetModelForTime(scint->fTime);
  fmodel_realimag.resize(2*modelFT.GetLength() - 3);
  for(size_t i = 1; i < modelFT.GetLength(); i++) {
    fmodel_realimag[2*i - 2] = modelFT[i].real();
    if(i != modelFT.GetLength()-1) fmodel_realimag[2*i - 1] = modelFT[i].imag();
  }

  // Form a list of the actual APDs we'll use, fixing an indexing in the process.
  fChannelsToUse.clear();
  const EXOChannelMap& ChannelMap = GetChanMapForHeader(ED->fEventHeader);
  for(int i = 152; i < 226; i++) { // Loop through all APD gangs.

    // If it's a bad or known-missing channel, just skip it.
    if(ChannelMap.channel_suppressed_by_daq(i) or not ChannelMap.good_channel(i)) continue;

    // If it's missing from EXOWaveformData, we don't have it -- so skip it.
    if(ED->GetWaveformData()->GetWaveformWithChannel(i) == NULL) continue;

    // If we don't have noise or lightmap information about the channel, skip it.
    // But warn -- this isn't an ideal situation, and indicates an oversight in how those
    // structures are being generated.
    if(std::find(fAPDs.begin(), fAPDs.end(), i) == fAPDs.end()) {
      LogEXOMsg("An APD channel with waveforms could not be used.", EEWarning);
      continue;
    }

    // Passes all tests -- add it.
    fChannelsToUse.push_back(i);
  }

  // Initialize the BiCGSTAB solver.
  BiCGSTAB_iter solver_step;

  // Do a simple, but not quite crazy, initial guess for x.
  size_t nrows = fChannelsToUse.size()*size_t(2*1024 - 1) + 1;
  solver_step.x.resize(nrows);
  double norm_model = std::inner_product(fmodel_realimag.begin(), fmodel_realimag.end(),
                                         fmodel_realimag.begin(), double(0));
  double SumSqYieldExpected = 0;
  for(size_t i = 0; i < fChannelsToUse.size(); i++) {
    SumSqYieldExpected += std::pow(fExpectedYieldPerGang[fChannelsToUse[i]], 2);
  }
  for(size_t i = 0; i < fChannelsToUse.size(); i++) {
    double ExpectedYieldOnChannel = fExpectedYieldPerGang[fChannelsToUse[i]];
    double LeadingFactor = ExpectedYieldOnChannel/(SumSqYieldExpected*norm_model);
    for(size_t f = 0; f < 2*1024-1; f++) {
      solver_step.x[size_t(2*1024-1)*i + f] = LeadingFactor*fmodel_realimag[f];
    }
  }
  solver_step.x.back() = 0; // Don't know how to guess the lagrange multiplier.

  // r_0 = b - Ax_0.  So compute Ax_0, and then rearrange in an awkward fashion.
  solver_step.r = MatrixTimesVector(solver_step.x);
  solver_step.r.back() -= 1; // E_thorium = 1 in the result.
  for(size_t i = 0; i < solver_step.r.size(); i++) solver_step.r[i] = -solver_step.r[i];

  const std::vector<double> r0hat = solver_step.r;
  solver_step.rho = 1;
  solver_step.alpha = 1;
  solver_step.omega = 1;

  solver_step.v.assign(nrows, 0);
  solver_step.p.assign(nrows, 0);

  // Solve the system.  Do a maximum of 10000 iterations, but expect to terminate much sooner.
  fWatch_Solve.Start(false);
  for(size_t i = 0; i < 10000; i++) {
    solver_step = BiCGSTAB_iteration(solver_step, r0hat);
    fTotalNumberOfIterationsDone++;
    // solver_step.r is the residual at this iteration.
    // So, we should use it to test if the result is good enough.
    // b is the same every time (not dependent on fExpectedEnergy_keV),
    // so the permissible value |r| should be something we can find with trial and error.
    // |b| = 1, which gives some sense of scale for what is reasonable.
    // I do NOT test |r|/|x|, because this ratio is not unitless.
    double r_norm = std::inner_product(solver_step.r.begin(), solver_step.r.end(),
                                       solver_step.r.begin(), double(0));
    if(r_norm < fRThreshold*fRThreshold) break;
  }
  fNumEntriesSolved++;
  fWatch_Solve.Stop();

  std::vector<double>& X = solver_step.x;

  // Collect the fourier-transformed waveforms.  Save them split into real and complex parts.
  // Skip channels which aren't included in our noise or lightmap models, but warn.
  std::vector<EXODoubleWaveform> WF_real, WF_imag;
  for(size_t i = 0; i < fChannelsToUse.size(); i++) {
    const EXOWaveform* wf = ED->GetWaveformData()->GetWaveformWithChannel(fChannelsToUse[i]);
    if(not wf) LogEXOMsg("A waveform disappeared!", EEAlert);

    // Take the Fourier transform.
    EXODoubleWaveform dwf = wf->Convert<Double_t>();
    EXOWaveformFT fwf;
    EXOFastFourierTransformFFTW::GetFFT(dwf.GetLength()).PerformFFT(dwf, fwf);

    // Extract the real part.
    EXODoubleWaveform rwf;
    rwf.SetLength(fwf.GetLength());
    for(size_t f = 0; f < fwf.GetLength(); f++) rwf[f] = fwf[f].real();
    WF_real.push_back(rwf);

    // Extract the imaginary part.
    // Note that the first and last components are strictly real (though we get them anyway).
    EXODoubleWaveform iwf;
    iwf.SetLength(fwf.GetLength());
    for(size_t f = 0; f < fwf.GetLength(); f++) iwf[f] = fwf[f].imag();
    WF_imag.push_back(iwf);
  }

  // Produce an estimate of light produced.
  double LightResult = 0;
  long NextIndex = 0;
  for(size_t index_ch = 0; index_ch < fChannelsToUse.size(); index_ch++) {
    for(size_t f = 1; f <= 1024; f++) {
      // Add the real part.
      LightResult += X[NextIndex] * WF_real[index_ch][f];
      NextIndex++;

      // Add the imag part.
      if(f == 1024) continue; // Last fourier component is strictly-real.
      LightResult += X[NextIndex] * WF_imag[index_ch][f];
      NextIndex++;
    }
  }

  // Save the result, rescaling so that the value is roughly in units of keV.
  scint->fDenoisedEnergy = LightResult*fThoriumEnergy_keV;

  fWatch_ProcessEvent.Stop();
  return kOk;
}

int EXORefitAPDs::ShutDown()
{
  // Print statistics and timing information.
  std::cout<<"Timer information for refit-apds module."<<std::endl;
  std::cout<<"Whole process:"<<std::endl;
  fWatch_ProcessEvent.Print();
  std::cout<<"Solving the matrix:"<<std::endl;
  fWatch_Solve.Print();
  std::cout<<"Multiplying the matrix by vectors (excluding allocation of return vector):"<<std::endl;
  fWatch_MatrixMul.Print();
  std::cout<<"Handling noise correlation part of matrix (the bottleneck):"<<std::endl;
  fWatch_MatrixMul_NoiseTerms.Print();
  std::cout<<std::endl;
  std::cout<<"Average number of iterations to solve: "
           <<double(fTotalNumberOfIterationsDone)/fNumEntriesSolved<<std::endl;
  return 0;
}

EXOWaveformFT EXORefitAPDs::GetModelForTime(double time) const
{
  // Return an EXOWaveformFT corresponding to a scintillation signal at time T.
  // The magnitude should be normalized so peak-baseline = 1.
  // The baseline itself is zero.
  // Note that at the moment, this assumes a waveform of length 2048 is required.
  // time is in ns.
  // No accounting for APD-by-APD shaping time variations is currently made.
  //
  // It might seem reasonable to do this just once, and apply a time shift in fourier space.
  // However, generating it in real space allows us to deal with signals near the end of
  // the trace, where periodicity is violated.
  // There is still some potential for caching the time-domain waveform, though, if needed.

  EXODoubleWaveform timeModel_fine;
  int refinedFactor = 5;
  timeModel_fine.SetLength(2048*refinedFactor);
  timeModel_fine.SetSamplingFreq(refinedFactor*CLHEP::megahertz);
  timeModel_fine.Zero();
  size_t NonzeroIndex = size_t(time/(CLHEP::microsecond/refinedFactor));
  for(size_t i = NonzeroIndex; i < timeModel_fine.GetLength(); i++) timeModel_fine[i] = 1;

  EXOTransferFunction tf;
  tf.AddIntegStageWithTime(3.*CLHEP::microsecond);
  tf.AddIntegStageWithTime(3.*CLHEP::microsecond);
  tf.AddDiffStageWithTime(10.*CLHEP::microsecond);
  tf.AddDiffStageWithTime(10.*CLHEP::microsecond);
  tf.AddDiffStageWithTime(300.*CLHEP::microsecond);

  tf.Transform(&timeModel_fine);
  timeModel_fine /= tf.GetGain();

  EXODoubleWaveform timeModel;
  timeModel.SetLength(2048);
  for(size_t i = 0; i < timeModel.GetLength(); i++) timeModel[i] = timeModel_fine[i*refinedFactor];

  EXOWaveformFT fwf;
  EXOFastFourierTransformFFTW::GetFFT(timeModel.GetLength()).PerformFFT(timeModel, fwf);
  assert(fwf.GetLength() == 1025); // Just to make sure I'm reasoning properly.
  return fwf;
}

double EXORefitAPDs::GetGain(unsigned char channel) const
{
  // Return the gain of an apd channel.  This is the conversion factor from number
  // of photons incident on the APD to number of ADC counts (peak-baseline) in the
  // digitized signal.
  // It's a rough estimate, since this number isn't well-known, but we only need it to
  // set the scale for how important Poisson noise is, relative to electronic noise.
  // We currently use laser data from run 4540, and extract time-dependence from the
  // gainmap (the time-dependence of the lightmap).  It would be interesting to
  // do a fit of laser and gainmap data for the times when they overlap,
  // and get a higher-quality set of values.

  double Gain = 1.9; // 1.9 electron-hole pairs per photon, on average.

  // APD gains from the laser run 4540.
  switch(channel) {
    case 152: Gain *= 201.230438146; break;
    case 153: Gain *= 178.750438779; break;
    case 154: Gain *= 194.228589338; break;
    case 155: Gain *= 183.33801615; break;
    case 156: Gain *= 218.485999976; break;
    case 157: Gain *= 222.139259152; break;
    case 158: Gain *= 169.982559736; break;
    case 159: Gain *= 140.385120552; break;
    case 160: Gain *= 137.602725389; break;
    case 161: Gain *= 197.78183714; break;
    case 162: Gain *= 155.478773762; break;
    // case 163: Gain *= 0; // Bad channel, omitted.
    case 164: Gain *= 175.875067527; break;
    case 165: Gain *= 160.014408865; break;
    case 166: Gain *= 183.408055613; break;
    case 167: Gain *= 189.600819126; break;
    case 168: Gain *= 160.339214431; break;
    case 169: Gain *= 168.547991045; break;
    case 170: Gain *= 182.670039836; break;
    case 171: Gain *= 205.567802982; break;
    case 172: Gain *= 195.87450621; break;
    case 173: Gain *= 224.956647122; break;
    case 174: Gain *= 232.062359991; break;
    case 175: Gain *= 241.822881767; break;
    case 176: Gain *= 194.740435753; break;
    case 177: Gain *= 189.867775084; break;
    // case 178: Gain *= 0; // Bad channel, omitted.
    case 179: Gain *= 206.755206938; break;
    case 180: Gain *= 207.822617603; break;
    case 181: Gain *= 207.501985741; break;
    case 182: Gain *= 218.213137769; break;
    case 183: Gain *= 234.369354843; break;
    case 184: Gain *= 99.908111992; break;
    case 185: Gain *= 238.381809313; break;
    case 186: Gain *= 225.118270743; break;
    case 187: Gain *= 199.078450518; break;
    case 188: Gain *= 221.863823239; break;
    case 189: Gain *= 177.032783679; break;
    case 190: Gain *= 196.787332164; break;
    // case 191: Gain *= 0; // Bad channel, omitted.
    case 192: Gain *= 194.923448865; break;
    case 193: Gain *= 197.027984846; break;
    case 194: Gain *= 202.757086104; break;
    case 195: Gain *= 194.432937658; break;
    case 196: Gain *= 208.992809367; break;
    case 197: Gain *= 224.762562055; break;
    case 198: Gain *= 217.696006443; break;
    case 199: Gain *= 222.380158829; break;
    case 200: Gain *= 218.358804472; break;
    case 201: Gain *= 209.573057132; break;
    case 202: Gain *= 194.684536629; break;
    case 203: Gain *= 182.543842783; break;
    case 204: Gain *= 193.469930111; break;
    // case 205: Gain *= 0; // Bad channel, omitted.
    case 206: Gain *= 193.627191472; break;
    case 207: Gain *= 196.073150574; break;
    case 208: Gain *= 189.597962521; break;
    case 209: Gain *= 198.824317108; break;
    case 210: Gain *= 222.747770671; break;
    case 211: Gain *= 216.928470825; break;
    case 212: Gain *= 223.437239807; break;
    case 213: Gain *= 224.316404923; break;
    case 214: Gain *= 216.26783603; break;
    case 215: Gain *= 209.612423384; break;
    case 216: Gain *= 223.041660884; break;
    case 217: Gain *= 202.642254512; break;
    case 218: Gain *= 213.904993632; break;
    case 219: Gain *= 221.988942321; break;
    case 220: Gain *= 201.427174798; break;
    case 221: Gain *= 196.689200146; break;
    case 222: Gain *= 191.457656123; break;
    case 223: Gain *= 186.183873541; break;
    case 224: Gain *= 217.033080346; break;
    case 225: Gain *= 205.858374653; break;
    default: Gain *= 0; // Bad or non-existent channel.
  }
  // Time-dependence from the gainmap.
  const TGraph* GainGraph = fGainMaps.at(channel);
  Gain *= GainGraph->Eval(fUnixTimeOfEvent)/GainGraph->Eval(1355409118.254096);

  Gain *= 32.e-9; // Convert from electrons to volts in the preamp. Roughly 1/(5 pF) gain.
  Gain *= 12.10; // Gain from shapers (amplification factor, and gain from transfer function.
  Gain *= 4096./2.5; // Conversion from volts to ADC counts -- full-scale is 2.5 volts.

  return Gain;
}

EXORefitAPDs::BiCGSTAB_iter EXORefitAPDs::BiCGSTAB_iteration(const EXORefitAPDs::BiCGSTAB_iter& in,
                                                             const std::vector<double>& r0hat) const
{
  // Accept input in; do another iteration, and return the next iteration.
  // Not terribly efficient with re-allocating memory for vectors.  To address later, maybe.
  // No pre-conditioning.  I just wanted something simple to implement here.

  BiCGSTAB_iter ret;
  ret.rho = std::inner_product(r0hat.begin(), r0hat.end(), in.r.begin(), double(0));
  double beta = (ret.rho/in.rho)*(in.alpha/in.omega);

  ret.p.resize(r0hat.size());
  for(size_t i = 0; i < r0hat.size(); i++) ret.p[i] = in.r[i] + beta*(in.p[i] - in.omega*in.v[i]);

  ret.v = MatrixTimesVector(ret.p);
  ret.alpha = ret.rho/std::inner_product(r0hat.begin(), r0hat.end(), ret.v.begin(), double(0));

  std::vector<double> s;
  s.resize(r0hat.size());
  for(size_t i = 0; i < r0hat.size(); i++) s[i] = in.r[i] - ret.alpha*ret.v[i];

  std::vector<double> t = MatrixTimesVector(s);
  ret.omega = 0;
  double t_acc = 0;
  for(size_t i = 0; i < t.size(); i++) {
    ret.omega += s[i]*t[i];
    t_acc += t[i]*t[i];
  }
  ret.omega /= t_acc;

  ret.x.resize(r0hat.size());
  for(size_t i = 0; i < r0hat.size(); i++) ret.x[i] = in.x[i] + ret.alpha*ret.p[i] + ret.omega*s[i];

  // This last bit is wasted if this is the last iteration, but we plan on doing lots of iterations,
  // so who cares?
  ret.r.resize(r0hat.size());
  for(size_t i = 0; i < r0hat.size(); i++) ret.r[i] = s[i] - ret.omega*t[i];

  return ret;
}

std::vector<double> EXORefitAPDs::MatrixTimesVector(const std::vector<double>& in) const
{
  // Do A*in; return the result.
  // Notice that means we're doing some unnecessary heap allocations;
  // consider in the future reusing the memory of these vectors.
  // Note that we never explicitly save A; in principle we could and might be slightly faster, but
  // sparse matrix formats are non-trivial and it's really much easier this way.

  assert(in.size() == (2*1024-1)*fChannelsToUse.size() + 1);
  std::vector<double> out;
  out.assign(in.size(), 0);

  // Pointers we'll use to extract sections of the arrays.
  double* out_ptr;
  const double* in_ptr;

  // Note that I want to exclude vector allocation in the time --
  // because in principle that could be eliminated if the computational part ever became sub-dominant.
  fWatch_MatrixMul.Start(false);

  for(size_t i  = 0; i < fChannelsToUse.size(); i++) {
    unsigned char channel_i = fChannelsToUse[i];
    double gain_i = GetGain(channel_i); // ADC counts (max-baseline) per photon.

    // The expected signal magnitude, in ADC, from a Th gamma line event.
    double adc_yield_thorium_i = fExpectedYieldPerGang.at(channel_i);

    // The expected signal magnitude, in ADC, from this event (based on charge energy).
    // This serves to calibrate how significant Poisson noise is compared to electronic noise.
    double exp_yield_adc_i = adc_yield_thorium_i * fExpectedEnergy_keV/fThoriumEnergy_keV;

    // First deal with electronic noise terms.
    fWatch_MatrixMul_NoiseTerms.Start(false);
    for(size_t j = 0; j < fChannelsToUse.size(); j++) {
      unsigned char channel_j = fChannelsToUse[j];

      // Get the appropriate noise information for this i,j pair.
      const std::vector<double>& RRII_ij = fNoiseRRandII.at(std::make_pair(std::min(channel_i, channel_j),
                                                                           std::max(channel_i, channel_j)));
      const std::vector<double>& RI_ij = fNoiseRI.at(std::make_pair(channel_i, channel_j));
      const std::vector<double>& IR_ij = fNoiseRI.at(std::make_pair(channel_j, channel_i));

      // Handle all of the diagonals at once, allowing the combination of a few instructions.
      out_ptr = out.data() + size_t(2*1024-1)*i;
      in_ptr = in.data() + size_t(2*1024-1)*j;
      for(size_t f_index = 0; f_index < 1024-1; f_index++) {
        out_ptr[2*f_index] += RRII_ij[2*f_index]*in_ptr[2*f_index] + RI_ij[f_index]*in_ptr[2*f_index+1];
        out_ptr[2*f_index+1] += RRII_ij[2*f_index+1]*in_ptr[2*f_index+1] + IR_ij[f_index]*in_ptr[2*f_index];
      }
      // Handle the last term specially.
      out_ptr[2*1024-2] += RRII_ij[2*1024-2]*in_ptr[2*1024-2];
    } // end electronic noise.
    fWatch_MatrixMul_NoiseTerms.Stop();

    // Then handle the Poisson terms, and lagrange and constraint terms, in one go.

    // inner product of fmodel with in[channel i].
    double innerprod_fmodel_i = std::inner_product(fmodel_realimag.begin(),
                                                   fmodel_realimag.end(),
                                                   in.begin() + size_t(2*1024-1)*i,
                                                   double(0));

    // Do the real and imaginary equations together.
    out_ptr = out.data() + size_t(2*1024-1)*i;
    for(unsigned short f = 0; f < 2*1024-1; f++) {
      out_ptr[f] += fmodel_realimag[f] * exp_yield_adc_i * gain_i * innerprod_fmodel_i;
    }

    // Lagrange multiplier terms.
    out_ptr = out.data() + size_t(2*1024-1)*i;
    for(unsigned short f = 0; f < 2*1024-1; f++) {
      out_ptr[f] += fmodel_realimag[f] * adc_yield_thorium_i * in.back();
    }

    // Constraint equation.
    out.back() += adc_yield_thorium_i * innerprod_fmodel_i;
  } // End Poisson+lagrange+constraint.

  fWatch_MatrixMul.Stop();
  return out;
}
