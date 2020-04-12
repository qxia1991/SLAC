#include "EXOUtilities/EXORCDifferentiator.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <cmath>
//______________________________________________________________________________
// EXOWFRCDifferentiation
//
// DESCRIPTION: 
//
// A class handling the rc differentiating of a waveform.  This is useful to
// simulate the passing of a waveform through a simple preamp.  Perform RC
// differentiation.  The following algorithm is used:
//   
//   w = exp(-1./(tau*sf);
//   b = (1+w)/2.;
//   y[n] = (x[n] - x[n-1])*b + y[n-1]*w;
//
// where sf is the sampling frequency of the input waveform, tau is the
// time constant of the filter.  y[n] and x[n] are the output and input
// waveforms at time n, respectively. 
// 
// AUTHOR: M. Marino
// FIRST SUBMISSION: Jun 2011 
// 
// REVISION:


//______________________________________________________________________________
void EXORCDifferentiator::TransformInPlace(EXODoubleWaveform& anInput) const
{
  // Perform RC differentiation.  The following algorithm is used:
  //   
  //   w = exp(-1./(tau*sf);
  //   b = (1+w)/2.;
  //   y[n] = (x[n] - x[n-1])*b + y[n-1]*w;
  //   
  //   where sf is the sampling frequency of the input waveform, tau is the
  //   time constant of the filter.  This transformation can be performed in
  //   place, but is faster out-of-place if more than one transformation is
  //   necessary, see e.g.  TransformOutOfPlace 

  if ( anInput.GetLength() <= 1 ) {
    LogEXOMsg(" anInput of length zero or one", EEError);
    return;
  }

  Double_t w = exp( -anInput.GetSamplingPeriod()/fTimeConstant );
  Double_t b = (1+w)/2.;

  Double_t dummy;
  Double_t dummy2 = anInput[0];
  size_t n = anInput.GetLength();
  for ( size_t i = 1; i < n; i++ ) {
    dummy = anInput[i];
    anInput[i] = (anInput[i] - dummy2)*b + (anInput[i-1])*w;
    dummy2 = dummy;
  }

}
//______________________________________________________________________________
void EXORCDifferentiator::TransformOutOfPlace(const EXODoubleWaveform& anInput, 
                                              EXODoubleWaveform& anOutput) const
{
  // Implementation of optimized out-of-place transformation.  See
  // TransformInPlace for details on the algorithm. 
  if ( anInput.GetLength() <= 1 ) {
    LogEXOMsg(" anInput of length zero or one", EEError);
    return;
  }

  Double_t w = exp( -anInput.GetSamplingPeriod()/fTimeConstant );
  Double_t b = (1+w)/2.;

  size_t n = anInput.GetLength();
  anOutput[0] = anInput[0];
  for ( size_t i = 1; i < n; i++ ) {
    anOutput[i] = (anInput[i] - anInput[i-1])*b + anOutput[i-1]*w;
  }
}
