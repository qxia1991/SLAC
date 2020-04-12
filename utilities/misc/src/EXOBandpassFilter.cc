//______________________________________________________________________________
//      
// CLASS DECLARATION:  EXOBandpassFilter.hh
//
// DESCRIPTION: 
//
// A class performing a bandpass filter.   Zeroes all frequencies below
// fLowerBandpass and above fUpperBandpass and returns the inverse transformed
// waveform.  The returned waveform is multiplied by GetLength().
//
// AUTHOR: M. Marino
// CONTACT: 
// FIRST SUBMISSION: 
// 
// REVISION:
// 2 Dec 2012 Back-ported to EXOUtilities by M. Marino 
//
//______________________________________________________________________________

#include "EXOUtilities/EXOBandpassFilter.hh"
#include "EXOUtilities/EXOFastFourierTransformFFTW.hh"
#include <limits>

//______________________________________________________________________________
EXOBandpassFilter::EXOBandpassFilter() : 
  EXOVWaveformTransformer("EXOBandpassFilter"), 
  fLowerBandpass(0), 
  fUpperBandpass(std::numeric_limits<double>::max())
{
  // Default constructor
}


//______________________________________________________________________________
void EXOBandpassFilter::TransformInPlace(EXODoubleWaveform& input) const
{
  // Perform the bandpass filter transformation.  This will remove (zero) all
  // frequencies below fLowerBandpass and above fUpperBandpass.  The returned
  // waveform is multiplied by GetLength(), this is a consequence of using
  // FFTW.

  EXOFastFourierTransformFFTW& fft = 
    EXOFastFourierTransformFFTW::GetFFT(input.GetLength());

  // Fill internal array and perform in-place fft on it.
  memcpy(fft.GetInternalArray<void>(), reinterpret_cast<const void*>(&input[0]),
         sizeof(double)*input.GetLength());
  fft.PerformFFT_inplace();

  // Do filter directly on fft internal array.
  std::complex<double>* complex_data = fft.GetInternalArray<std::complex<double> >();
  size_t i = 0;
  while (i < fft.GetFreqDomainLength()) {
    double theFreq = i*(input.GetSamplingFreq()/(2.*(fft.GetFreqDomainLength()-1)));
    if (theFreq < fLowerBandpass) complex_data[i] = std::complex<double>(0,0);
    if (theFreq > fUpperBandpass) complex_data[i] = std::complex<double>(0,0);
    i++;
  }

  // Perform inverse fft, then copy result back into input.
  fft.PerformInverseFFT_inplace();
  input.SetData(fft.GetInternalArray<double>(), fft.GetTimeDomainLength());
}
