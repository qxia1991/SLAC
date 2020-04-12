#include "EXOUtilities/EXOFrequencyPeakFilter.hh"

#include <complex>
#include <iostream>
#include <sstream>
#include "EXOUtilities/EXOFastFourierTransformFFTW.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TH1D.h"

using namespace std;

EXOFrequencyPeakFilter::EXOFrequencyPeakFilter()
: EXOVWaveformTransformer("EXOFrequencyPeakFilter"),
  fFilter()
{

}

void EXOFrequencyPeakFilter::SetSpectrum(const EXOWaveformFT& spectrum, double nsigma, size_t nbins)
{
  fFilter.MakeSimilarTo(spectrum);
  fFilter.Zero();
  size_t nchunks = spectrum.GetLength() / nbins;
  size_t lastbins = spectrum.GetLength() % nbins;
  for(size_t chunk=0; chunk<nchunks; chunk++){
    double mean = 0;
    double sigma = 0;
    for(size_t i=0; i<nbins; i++){
      size_t pos = chunk*nbins + i;
      mean += abs(spectrum[pos]);
      sigma += abs(spectrum[pos]) * abs(spectrum[pos]);
    }
    mean /= double(nbins);
    sigma /= double(nbins);
    sigma -= mean*mean;
    if(sigma < 0.0){
      sigma = 0;
    }
    for(size_t i=0; i<nbins; i++){
      size_t pos = chunk*nbins + i;
      if(abs(spectrum[pos]) < mean + nsigma*sigma){
        fFilter[pos] = 1.0;
      }
    }
  }
  double mean = 0.0;
  double sigma = 0.0;
  for(size_t i=0; i<lastbins; i++){
    size_t pos = nchunks*nbins + i;
    mean += abs(spectrum[pos]);
    sigma += abs(spectrum[pos]) * abs(spectrum[pos]);
  }
  mean /= double(lastbins);
  sigma /= double(lastbins);
  sigma -= mean*mean;
  if(sigma < 0.0){
    sigma = 0;
  }
  for(size_t i=0; i<lastbins; i++){
    size_t pos = nchunks*nbins + i;
    if(abs(spectrum[pos]) < mean + nsigma*sigma){
      fFilter[pos] = 1.0;
    }
  }
  //TH1D* hist = fFilter.GimmeHist();
  //EXOMiscUtil::DisplayInProgram(*hist,"Drawing fFilter");
}

void EXOFrequencyPeakFilter::TransformInPlace(EXODoubleWaveform& input) const
{
  TransformOutOfPlace(input,input);
}

void EXOFrequencyPeakFilter::TransformOutOfPlace(const EXODoubleWaveform& input, EXODoubleWaveform& output) const
{
  if(input.GetLength()/2 + 1 != fFilter.GetLength()){
    LogEXOMsg("Waveform length incompatible to filter!",EEError);
    output = input;
    return;
  }
  EXOWaveformFT fourier;
  EXOFastFourierTransformFFTW::GetFFT(input.GetLength()).PerformFFT(input,fourier);
  fourier *= fFilter;
  EXOFastFourierTransformFFTW::GetFFT(input.GetLength()).PerformInverseFFT(output,fourier);
  output /= double(output.GetLength());
}
