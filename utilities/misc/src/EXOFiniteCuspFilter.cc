//Jason Chaves
//I made a copy of the EXOTrapezoidalFilter.cc file and I
//replaced the trapezoidal algorithm with the 
//finite cusp with flat top algorithm, taken from the same paper 
//referenced below.
//




//______________________________________________________________________________
//                                                             
//      
// CLASS DECLARATION:  EXOTrapezoidalFilter.hh
// 
// DESCRIPTION: 
// 
// A class handling the trapezoidal filter of a waveform.
// Algorithm from V.T Jordanov, G.F. Knoll, NIM A345 (1994) 337-345.
// 
// AUTHOR: M. Marino
// CONTACT: 
// FIRST SUBMISSION: 
// 
// REVISION:
// 30 Aug 2010 Added get methods, A. Schubert 
// 3 Dec 2011 Back-ported to EXOUtilities by M. Marino 
// 
//______________________________________________________________________________

#include "EXOUtilities/EXOFiniteCuspFilter.hh"

EXOFiniteCuspFilter::EXOFiniteCuspFilter() : 
  EXOVWaveformTransformer("EXOFiniteCuspFilter"), 
  fRampTime(0.),
  fFlatTime(0.),
  fDecayConstant(0.),
  fDoNormalize(true)
{
}

void EXOFiniteCuspFilter::TransformOutOfPlace(const EXODoubleWaveform& anInput, EXODoubleWaveform& anOutput) const 
{
    //! Perform the transformation.  This cannot be done in place. 
    /*!
        The trapezoidal filter performs a pole-zero correction for an
        RC circuit and averages values ( using ramp-time ). 
        Baseline removal should be performed before initiating
        this transformation.

        This transformation multiplies the output waveform by 
        decayTime*rampTime*sf*sf

        where sf is the sampling frequency of the input waveform.
     */



  if(anInput.GetLength() <= 1) return;

  anOutput.MakeSimilarTo(anInput);
  
  size_t rampStep = static_cast<size_t>(fRampTime*anInput.GetSamplingFreq());
  size_t flatStep = static_cast<size_t>(fFlatTime*anInput.GetSamplingFreq());
  double decayConstant = fDecayConstant*anInput.GetSamplingFreq();
  
  if(fVector.size() != anInput.GetLength()) {
    fVector.resize(anInput.GetLength());
  }
  fVector[0] = anInput.At(0);
  anOutput[0] = (decayConstant+1.)*anInput.At(0);
  double scratch = 0.0;

  //I added this
  std::vector<double> pVector;
  pVector.resize(anInput.GetLength() , 0.0);
  double inMax = anInput.GetMaxValue();//in case this is somehow in place, get max value of input before for loop


  for(size_t i=1;i<anInput.GetLength();i++)
  {
    // This is a little tricky with all the ternary operators, but it's faster
    // this way.  We must check the bounds.
    
   // scratch = anInput.At(i)  - ((i>=rampStep) ? anInput.At(i-rampStep) : 0.0)
   //   - ((i>=flatStep+rampStep) ? anInput.At(i-flatStep-rampStep) : 0.0)
   //   + ((i>=flatStep+2*rampStep) ? anInput.At(i-flatStep-2*rampStep) : 0.0);  
    
    //The above in this for loop is the trap code for scatch, 
    //and below is my finite cusp scratch.
    //scratch is all the other terms in the s(n) update eqn, except for s(n-1)

    pVector[i] = pVector[i-1] + anInput.At(i) 
      - ((i>=rampStep) ? anInput.At(i-rampStep) : 0.0) 
      + ((i>=flatStep+rampStep) ? anInput.At(i-flatStep-rampStep) : 0.0)
      - ((i>=flatStep+2*rampStep) ? anInput.At(i-flatStep-2*rampStep) : 0.0); 
    
    scratch = pVector[i] - 
      ( ((i>=rampStep) ? anInput.At(i-rampStep) : 0.0)  
        +  ((i>=flatStep+rampStep) ? anInput.At(i-flatStep-rampStep) : 0.0) 
      )*rampStep
      - ((i>=flatStep+rampStep) ? anInput.At(i-flatStep-rampStep) : 0.0)
      + ((i>=flatStep+2*rampStep) ? anInput.At(i-flatStep-2*rampStep) : 0.0); 

    //This is the pole-zero cancellation
    //if decayConstant != 0, add fraction to output to remove undershoot
    if(decayConstant != 0.0) {
      fVector[i] = fVector[i-1] + scratch; 
      anOutput[i] = anOutput[i-1] + fVector[i] + decayConstant*scratch;
    } 
    else anOutput[i] = anOutput[i-1] + scratch;




  }


  if(fDoNormalize) {
    double norm = rampStep;
    if(decayConstant != 0.0) norm *= decayConstant;
    anOutput /= norm;
  }
}

