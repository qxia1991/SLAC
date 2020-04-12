#ifndef EXOWFCrossProduct_hh
#define EXOWFCrossProduct_hh

#include "EXOUtilities/EXOWaveformFT.hh"

class EXOWFCrossProduct
{
  public:
    EXOWFCrossProduct();
    virtual ~EXOWFCrossProduct() {}
    virtual int Transform(const EXOWaveformFT& input1, const EXOWaveformFT& input2, EXOWaveformFT& output) const;

  private:
    double Sq(double a) const {return a*a;}
};

#endif
