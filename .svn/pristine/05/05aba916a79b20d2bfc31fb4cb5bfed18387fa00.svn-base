#ifndef _EXORCIntegrator_HH
#define _EXORCIntegrator_HH

#include "EXOUtilities/EXOVWaveformTransformer.hh"

class EXORCIntegrator : public EXOVWaveformTransformer 
{
  public:
    EXORCIntegrator() : 
      EXOVWaveformTransformer("EXORCIntegrator" ),
      fTimeConstant(50.) {} 
    virtual ~EXORCIntegrator() {}
 
    // This transform is optimized for an in-place transformation
    virtual bool IsInPlace() const { return true; }

    ///* Set the RC Time constant (tau). */
    virtual void SetTimeConstant( double aVal ) { fTimeConstant = aVal; }

   protected: 
    virtual void TransformInPlace(EXODoubleWaveform& anInput) const;
    double fTimeConstant;

};

#endif
