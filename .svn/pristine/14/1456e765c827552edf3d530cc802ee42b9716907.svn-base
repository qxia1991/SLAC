//______________________________________________________________________________
// EXOTransferFunction
//
// DESCRIPTION: 
//
// A class handling the transfer function of a waveform with an abitrary number
// of preamp stages (RC diff and Integration).  This class is meant to be a
// convenience class for modeling a full preamp with multiple integration and
// differentiation stages. 
//
// AUTHOR: M. Marino
// FIRST SUBMISSION: Jun 2011 
// 
// REVISION:
//______________________________________________________________________________

#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TMath.h"
#include <cmath>
#include <cassert>
#include <numeric>

EXOTransferFunction::EXOTransferFunction()
: EXOVWaveformTransformer("EXOTransferFunction" )
{}

void EXOTransferFunction::AddDiffStageWithTime(Double_t time)
{
  // The differentiation time must be > 0
  assert(time > 0);
  fDiffStages.push_back(time);
  fCachedNoiseSpectra.clear();
}

void EXOTransferFunction::AddIntegStageWithTime(Double_t time)
{
  // The integration time must be >= 0
  assert(time >= 0);
  fIntegStages.push_back(time);
  fCachedNoiseSpectra.clear();
}

void EXOTransferFunction::TransformInPlace(EXODoubleWaveform& wf) const
{
  // Perform the transformation in place 
  for (size_t i=0;i<GetNumIntegStages();i++) {
    fInteg.SetTimeConstant(GetIntegTime(i));
    fInteg.Transform(&wf);
  } 

  size_t numOutOfPlaceStages = GetNumDiffStages()/2;
  for (size_t i=0;i<numOutOfPlaceStages;i++) {
    // Doing this because it's a little faster to go out-of-place if there's
    // more than one transformation.
    fDiff.SetTimeConstant(GetDiffTime(2*i));
    fDiff.Transform(&wf, &fScratch);
    fDiff.SetTimeConstant(GetDiffTime(2*i+1)); 
    fDiff.Transform(&fScratch, &wf);
  }
  if (numOutOfPlaceStages*2 != GetNumDiffStages() ) {
    // Handle the last one if necessary.
    fDiff.SetTimeConstant(GetDiffTime(numOutOfPlaceStages*2)); 
    fDiff.Transform(&wf);
  }
}

//______________________________________________________________________________
Double_t EXOTransferFunction::GetGain(EXODoubleWaveform wf) const
{
  // Calculate the gain given the properties of this transform.
  // If a waveform is input, it will use the frequency and length allotted by that waveform.
  // Note:  We use the standard definition of gain, the maximum output given a unit-step input.
  // If this function is unable to obtain an answer, -1 is returned and an error is logged.

  if (GetNumDiffStages() == 0 ||
      GetNumIntegStages() == 0) {
    // If either of these is 0, it means that only integration or
    // differentiation has been requested, and the effective gain is 1.
    return 1.0;
  }
  size_t len = wf.GetLength();
  if ( len < 4) {
    // OK, we need to calculate, there is a mix 
    // User has not passed in a waveform to calculate the gain on.

    if(len != 0) {
      // Or, in this case, the user passed in a bad waveform (too short).  Replace it, with an error.
      LogEXOMsg("User passed a waveform into EXOTransferFunction::GetGain that was too short; overwriting it.", EEError);
    }

    Double_t minDiffConstant = GetDiffTime(0);
    for (size_t i = 1; i<GetNumDiffStages();i++) {
      if (minDiffConstant < GetDiffTime(i)) {
        minDiffConstant = GetDiffTime(i);
      }
    }

    // Estimating the peaking time, in general it should be close to the
    // rise-times which are added in quadrature.
    Double_t peakingTime = GetIntegTime(0)*GetIntegTime(0);
    for (size_t i = 1; i<GetNumIntegStages();i++) {
        peakingTime += GetIntegTime(i)*GetIntegTime(i);
    }
    peakingTime = 2.2*std::sqrt(peakingTime);
    
    // Getting the decay time, this defines the high frequency reduction and so
    // tells us how much we should sample to get the curvature of the waveform.
    Double_t decayTime = GetDiffTime(0);
    for (size_t i = 1; i<GetNumDiffStages();i++) {
        decayTime = std::sqrt(decayTime*GetDiffTime(i));
    }
    
    // Gives us the frequency that we should use.  
    Double_t freq = 4./(TMath::TwoPi()*decayTime);
    wf.SetSamplingFreq(1.);
    len = (size_t)(3.*peakingTime) + 4; // Add 4 for small integration times; in limiting case, consider no integrators.
    wf.SetLength(len); 
    wf[0] = 0.0;
    wf[1] = 0.0;
    for (size_t i=2;i<len;i++) wf[i] = 1;
  }

  TransformInPlace(wf);
  
  for ( size_t m = 1; m < len; m++ ) {
    if (wf[m] < wf[m-1] - 0.0000001) {
      if (wf[m-1] == 0) {
        LogEXOMsg("Couldn't calculate shaper gain, waveform not fine enough. Setting equal to -1.", EEError);
        break;
      }
      else {
        return wf[m-1];
      }
    }
    if ( m == len - 1 ) {
      LogEXOMsg("Couldn't calculate shaper gain, waveform not long enough. Setting equal to -1.", EEError);
    }
  }

  return -1.0; // Error state
}

EXODoubleWaveform EXOTransferFunction::GetNoiseSpectrum(size_t Length /*= 2048*/) const
{
  // Given the shaping times currently in place and the assumption that unshaped noise is white,
  // compute the noise power spectrum E(N*(w)N(w)).  Details are in the note on confluence, but note that
  // there are (of course) significant differences between the noise computed here and what we see in the detector.
  // The sampling period is hard-coded, but it should work for an arbitrary number
  // of shapers of various time constants.  Accuracy may degrade if the parameters used in the numerical
  // integration are not tuned.
  // Note that the waveform returned will have a length of L/2 + 1 (rounded down),
  // to match the length of EXOWaveformFT objects.

  std::map<size_t, EXODoubleWaveform>::iterator Iter = fCachedNoiseSpectra.find(Length);
  if(Iter != fCachedNoiseSpectra.end()) {return Iter->second;}

  // Tunable parameters -- these values control how accurate the integral will be.
  double MinX = -0.001;         // We integrate from MinX to MaxX -- note that it is not necessary to integrate symmetrically.
  double MaxX = 0.004;          //        See the confluence note for an explanation.
  size_t NumPoints = 3*Length;  // Number of sample points along integration -- see the confluence note.
  ///////////////////////////////////////////////////////////////////////

  using std::cos;
  using std::pow;

  double StepSize = (MaxX-MinX) / (NumPoints-1);
  size_t TransformLength = Length/2 + 1;

  // Create waveform to return.
  EXODoubleWaveform NoiseSpectrum;
  NoiseSpectrum.SetLength(TransformLength);
  NoiseSpectrum.Zero();

  // This part of the integrand is the same for every point in the NoiseSpectrum.
  // Also fold in the factor from the step size.
  double Cache_Part1[NumPoints];
  for(size_t i = 0; i < NumPoints; i++) {
    double x = MinX + StepSize*i;
    Cache_Part1[i] = pow(x, (int)(2*GetNumDiffStages()));
    Cache_Part1[i] *= (1.0 - cos(x * SAMPLE_TIME * Length));
    for(size_t j = 0; j < GetNumDiffStages(); j++) {
      Cache_Part1[i] /= (pow(x,2) + 1.0/pow(GetDiffTime(j),2));
    }
    for(size_t j = 0; j < GetNumIntegStages(); j++) {
      Cache_Part1[i] /= (pow(x,2) + 1.0/pow(GetIntegTime(j),2));
    }
    Cache_Part1[i] *= StepSize;
  }

  for(size_t i = 0; i < TransformLength; i++) {

    for(size_t j = 0; j < NumPoints; j++) {
      double x = MinX + StepSize*j;
      double intermediate = 1.0 - cos(x * SAMPLE_TIME - TMath::TwoPi()*i/Length);

      if(intermediate == 0.0) {
        // Uh-oh -- division by zero.  But it's OK, it's a removable singularity.
        double result = Length*Length;
        result *= pow(x, (int)(2*GetNumDiffStages()));
        for(size_t k = 0; k < GetNumDiffStages(); k++) {
          result /= (pow(x,2) + 1.0/pow(GetDiffTime(k),2));
        }
        for(size_t k = 0; k < GetNumIntegStages(); k++) {
          result /= (pow(x,2) + 1.0/pow(GetIntegTime(k),2));
        }
        result *= StepSize;
        NoiseSpectrum[i] += result;
      }

      else {
        NoiseSpectrum[i] += Cache_Part1[j] / intermediate;
      }
    }
  }

  fCachedNoiseSpectra[Length] = NoiseSpectrum;
  return NoiseSpectrum;
}

bool EXOTransferFunction::operator==(const EXOTransferFunction& other) const
{
  return (fDiffStages == other.fDiffStages and fIntegStages == other.fIntegStages);
}

bool EXOTransferFunction::operator!=(const EXOTransferFunction& other) const
{
  return not (*this == other);
}

Double_t EXOTransferFunction::GetEffectiveIntegTime() const
{
  // Combine the integration times to get an effective overall integration time.
  // The approximation is valid for \omega << 2/t^I_max, or f << 1/(pi * t^I_max).
  return std::accumulate(fIntegStages.begin(), fIntegStages.end(), 0.0);
}

Double_t EXOTransferFunction::GetEffectiveDiffTime() const
{
  // Combine the differentiation times to get an effective overall differentiation time.
  // The approximation is valid for \omega >> 1/(2*t^D_min), or f >> 1/(4pi*t^D_min).
  double temp = 0.0;
  for(size_t i = 0; i < fDiffStages.size(); i++) {
    temp += 1.0/fDiffStages[i];
  }
  return 1.0/temp;
}
