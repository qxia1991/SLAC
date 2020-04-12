#include "EXOUtilities/EXOAddNoise.hh"
#include "EXOUtilities/EXOFastFourierTransformFFTW.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "TMath.h"
#include <cmath>
#include <complex>
#include "TRandom.h"
//______________________________________________________________________________
// EXOAddNoise
//
// DESCRIPTION: 
//
// A class that generates a noise waveform in the transform function with a
// frequency spectrum defined by integration and differentiation stages.
// The input waveform's properties will be used, for example, to determine the
// proper sampling frequency and length.  The transformed waveform will be
// normalized to have an RMS of 1.
//
// AUTHOR: M. Marino
// FIRST SUBMISSION: Jun 2011 
// 
// REVISION:



//______________________________________________________________________________
void EXOAddNoise::LoadTransferFunction(const EXOTransferFunction& transfer)
{
  // Load in an EXOTransferFunction object.
  // And shaping stages currently in this object will be cleared and replaced with those of the transfer function.
  // Gain will also be taken from the transfer function.
  Clear();
  for(size_t i = 0; i < transfer.GetNumDiffStages(); i++) {
    AddDiffStageWithTime(transfer.GetDiffTime(i));
  }
  for(size_t i = 0; i < transfer.GetNumIntegStages(); i++) {
    AddIntegStageWithTime(transfer.GetIntegTime(i));
  }
}
//______________________________________________________________________________
void EXOAddNoise::TransformInPlace(EXODoubleWaveform& wf) const
{
  // Perform the transformation in place.  This adds noise to a waveform given
  // the parameters of the integrators and differentiators input in.  If no
  // parameters have changed, it will calculate and cache a noise waveform of
  // length fCacheLength*wf.GetLength(). and use this subsequently to save
  // calculation time.  If you don't want this, set the cache length to 0 (see
  // SetCacheLength). 

  using std::complex;
  using TMath::TwoPi;

  // If our noise magnitude is 0.0, just return
  if (fNoiseMagnitude == 0.0) return;
  if (fTimeWF.GetLength() == 0 or fTimeWF.GetLength() < wf.GetLength()
      or fTimeWF.GetSamplingFreq() != wf.GetSamplingFreq()) {
    size_t nnoise = (fCacheLength == 0) ? wf.GetLength() : fCacheLength*wf.GetLength();
    size_t len = nnoise/2 + 1;
    EXOWaveformFT scratch;
    // Make similar to makes sure we have the frequency set up correctly
    scratch.MakeSimilarTo(wf);
    scratch.SetLength(len);
    
    double df = scratch.GetSamplingFreq()/nnoise;
    
    for (size_t i=0; i<len; i++ ) {
      double freq = i*df*TwoPi(); 
    
      complex<double> num(1,0);
      for (size_t j=0;j<fIntegStages.size();j++) num *= EXOMiscUtil::H_omega_RC(freq*fIntegStages[j]);
      for (size_t j=0;j<fDiffStages.size();j++) num *= EXOMiscUtil::H_omega_CR(freq*fDiffStages[j]);
    
      double phi = gRandom->Rndm()*TwoPi();
      if ( i == len-1) scratch[i] = complex<double>(num.real(), 0.0); 
      else scratch[i] = num*complex<double>(cos(phi), sin(phi)); 
    } 
    // transform the frequency spectrum into the time domain
      
    EXOFastFourierTransformFFTW::GetFFT(nnoise).PerformInverseFFT(fTimeWF, scratch);
    
    // Normalize the RMS fluctuations to have a width of 1.
    
    double rms = 0.0;
    for ( size_t i = 0; i < nnoise; i++ ) rms += fTimeWF[i]*fTimeWF[i];
    
    rms = sqrt(rms/nnoise);
    
    if ( rms != 0.0 ) fTimeWF /= rms;
    else LogEXOMsg("couldn't normalize noise spectrum", EEWarning);
  }
  size_t readPoint = (size_t) (fTimeWF.GetLength()*gRandom->Rndm());
  for (size_t i=0;i<wf.GetLength();i++) {
    if (readPoint >= fTimeWF.GetLength()) readPoint = 0;
    wf[i] += fTimeWF[readPoint]*fNoiseMagnitude;
    readPoint++;
  }

}

//______________________________________________________________________________
void EXOAddNoise::SetCacheLength(size_t cache)
{
  // Set the cache length, which is the length of waveform (multiplied by the
  // input transform waveform) to save.  Set this to 0 to not cache at all, and
  // calculate a new noise spectrum every time.  
  fCacheLength = cache;
  Reset();
}
