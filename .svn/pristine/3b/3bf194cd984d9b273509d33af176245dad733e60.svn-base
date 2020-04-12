//______________________________________________________________________________
//
// EXOSignalModel encapsulates the signal model for a class, saving the
// transfer function, an expected signal waveform and the channel/tag of a
// signal.  The content of this class is built by a user-defined signal builder
// (EXOVSignalModelBuilder). 
//
// SignalSum provides functionality to calculate the expectation value of an
// arbitrary number of signals with different amplitudes that have the shape
// defined by this signal model.
//______________________________________________________________________________
#include "EXOReconstruction/EXOSignalModel.hh"
#include <math.h>
#include <iostream>
#include <algorithm>
#include "EXOUtilities/SystemOfUnits.hh"
#include "TF1.h"
using namespace std;

//______________________________________________________________________________
EXOSignalModel::EXOSignalModel() :
  fChannelOrTag(0),
  fBehaviorType(EXOReconUtil::kUndefined),
  fSignalModelFunction(NULL)
{

}

//______________________________________________________________________________
EXOSignalModel::~EXOSignalModel()
{
  // Delete possible TF1 that has been created.
  delete fSignalModelFunction;
}

//______________________________________________________________________________
TF1& EXOSignalModel::GetFunction( Double_t begin, Double_t end, Int_t npar ) const
{
  // Returns a TF1 function which calls through to SignalSum.  The range of
  // this function is defined by begin and end.  The npar variable defines the
  // number of *waveforms* to expect, see SignalSum for an explanation of the
  // parameter ordering and number.

  if (!fSignalModelFunction || fSignalModelFunction->GetNpar() != 2*npar + 1) {
    delete fSignalModelFunction; // safe for NULL
    ROOT::Math::ParamFunctor memberFunction(this, &EXOSignalModel::SignalSum);
    fSignalModelFunction = 
      new TF1( TString::Format( "EXOSignalModel_%d", fChannelOrTag ),
                                 memberFunction,
                                 begin, end, 2*npar+1 ); 
  }
  fSignalModelFunction->SetRange(begin, end); 
  fSignalModelFunction->SetParameter(0, npar);
  return *fSignalModelFunction;
}
