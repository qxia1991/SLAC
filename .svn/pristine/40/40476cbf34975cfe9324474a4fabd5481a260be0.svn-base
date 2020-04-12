//Jason Chaves
//Preliminary header for EXOFiniteCuspFilter


#ifndef _EXOFiniteCuspFilter_HH
#define _EXOFiniteCuspFilter_HH

#include "EXOUtilities/EXOVWaveformTransformer.hh"
#include <vector>

class EXOFiniteCuspFilter : public EXOVWaveformTransformer 
{
  public:
    EXOFiniteCuspFilter(); 
    
    virtual inline bool IsInPlace() const { return false; }

    /// Set the ramp time of the filter.
    virtual inline void SetRampTime(double aVal) { fRampTime = aVal; }

    /// Set the flat time of the filter.
    virtual inline void SetFlatTime(double aVal) { fFlatTime = aVal; }

    /// Set the decay constant (tau) for the pole-zero correction (=0 by default for no correction (tau -> infinity))
    virtual void SetDecayConstant(double aVal) { fDecayConstant = aVal; }

    /// Put trap filter output in same units as input wf (set to false for marginally improved performance)
    virtual void SetDoNormalize(bool doNormalize = true) { fDoNormalize = doNormalize; }

    virtual inline double GetRampTime() const { return fRampTime; }
    virtual inline double GetFlatTime() const { return fFlatTime; }
    virtual inline double GetDecayConstant() const { return fDecayConstant; }
    virtual inline bool   GetDoNormalize() const { return fDoNormalize; }

  protected:
    virtual void TransformOutOfPlace(const EXODoubleWaveform& anInput, EXODoubleWaveform& anOutput) const;
    double fRampTime;      ///< duration of rising edge of trapezoid (CLHEP time units)
    double fFlatTime;      ///< duration of flat top of trapezoid (CLHEP time units)
    double fDecayConstant; ///< decay constant for pole-zero correction (CLHEP time units)
    bool fDoNormalize;     ///< flag to set whether trapezoid will be normalized to the input wf y-scale
    mutable std::vector<double> fVector; ///< internal vector to implement efficent recursive method

};

#endif
