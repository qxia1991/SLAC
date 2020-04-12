#include "EXOUtilities/EXOWFCrossProduct.hh"
#include "TMath.h"
#include <complex>

using namespace TMath;
using namespace std;

EXOWFCrossProduct::EXOWFCrossProduct()
{

}

int EXOWFCrossProduct::Transform(const EXOWaveformFT& input1, const EXOWaveformFT& input2, EXOWaveformFT& output) const
{
  if( not input1.IsSimilarTo(input2) ){
    LogEXOMsg("Waveforms do not Match!",EEError);
    return -1;
  }
  output.MakeSimilarTo(input1);
  size_t N = input1.GetLength();
  if(N){
    output[0] = complex<double>(0,0);
  }
  double absDen = 0.0;
  for(size_t i=1; i<N; i++){
    absDen += Sqrt(Sq(input1[i].real()) + Sq(input1[i].imag())) + Sqrt(Sq(input2[i].real()) + Sq(input2[i].imag()));
  }
  for(size_t i=1; i<N; i++){
    double abs = Sqrt(Sq(input1[i].real() + input2[i].real()) + Sq(input1[i].imag() + input2[i].imag()));
    output[i] = complex<double>(abs/absDen,0);
  }
  return 0;
}
