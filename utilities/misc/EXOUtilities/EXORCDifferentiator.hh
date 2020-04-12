#ifndef _EXORCDifferentiator_hh
#define _EXORCDifferentiator_hh

#include "EXOUtilities/EXOVWaveformTransformer.hh"

class EXORCDifferentiator : public EXOVWaveformTransformer 
{
  public:
    EXORCDifferentiator() : 
      EXOVWaveformTransformer("EXORCDifferentiator"), 
      fTimeConstant(50.) {} 
    
    // This transformation is optimized for both in- and out-of-place
    // transformation.  If you have to do more than one, consider using the
    // out-of-place transformation
    virtual bool IsInPlace() const { return true; }
    // This transformation is optimized for in-place transformation
    virtual bool IsOutOfPlace() const { return true; }

    ///* Set the RC time constant (tau). */
    void SetTimeConstant( double aVal ) { fTimeConstant = aVal; }

   protected: 
    virtual void TransformInPlace(EXODoubleWaveform& anInput) const;
    virtual void TransformOutOfPlace(const EXODoubleWaveform& anInput, EXODoubleWaveform& anOutput) const;
    double fTimeConstant;

};

#endif
