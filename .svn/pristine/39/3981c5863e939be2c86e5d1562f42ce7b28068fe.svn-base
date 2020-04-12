
#include "EXOUtilities/EXORCIntegrator.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <cmath>
//______________________________________________________________________________
//      
// EXORCIntegrator.hh
//
// DESCRIPTION: 
//
// A class handling the rc integration of a waveform.  This is useful to
// simulate the passing of a waveform through a simple preamp.  The
// following algororithm is used:
//
//    const = exp(-1./(sf*tau));
//    y[n] = (1-const)*x[n] + const*y[n-1]
//    
// where sf is the sampling frequency of the waveform and tau is the time
// constant of the filter.  y[n] is the output waveform, x[n] is the input
// waveform at time(point) n.
//
// This algorithm is optimized for in-place transformation.
//
// AUTHOR: M. Marino
// CONTACT: 
// FIRST SUBMISSION: 
// 
// REVISION:
// 
// 
//______________________________________________________________________________


//______________________________________________________________________________
void EXORCIntegrator::TransformInPlace(EXODoubleWaveform& anInput) const
{
  //  Returns the waveform after passes it through an integrator with an
  //  RC time constant.  . The following algororithm is used:
  //
  //  const = exp(-1./(sf*tau));
  //  y[n] = (1-const)*x[n] + const*y[n-1]
  //
  //  where sf is the sampling frequency of the waveform and tau is the time
  //  constant of the filter.  y[n] is the output waveform, x[n] is the input
  //  waveform at time(point) n.
  //  

  if ( anInput.GetLength() <= 1 ) {
    LogEXOMsg(" anInput of length zero or one",EEError);
    return;
  }

  Double_t x = exp( -anInput.GetSamplingPeriod()/fTimeConstant );
  double a0 = (1.-x);
  double b1 = x;
  //anInput[0] = a0*anInput[0];
  anInput[0] = anInput[0];
  size_t n = anInput.GetLength();
  for( size_t i=1;i<n;i++ ) {
    anInput[i] = a0*anInput[i] + b1*anInput[i-1];  
  }

}
