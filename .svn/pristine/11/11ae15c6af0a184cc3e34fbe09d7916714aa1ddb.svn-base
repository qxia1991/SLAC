#ifndef EXOSignalModel_hh
#define EXOSignalModel_hh

#include "EXOUtilities/EXOWaveform.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOTransferFunction.hh"

#include <cstddef> //for size_t


class TF1;
class EXOSignalModel
{

private:
  // EXOVSignalModelBuilder is a friend class because it is the only one that
  // should be allowed to Initialize this class.  The public interface is
  // entirely const.
  friend class EXOVSignalModelBuilder;

  Int_t fChannelOrTag;
  EXOReconUtil::ESignalBehaviorType fBehaviorType;

  // Transform applied by this channel
  EXOTransferFunction fTransferFunction;
  EXODoubleWaveform fShapedModel;

  mutable TF1* fSignalModelFunction;

public:
  
  EXOSignalModel();
  ~EXOSignalModel();

  EXOReconUtil::ESignalBehaviorType GetBehaviorType() const 
    { return fBehaviorType; }
  Int_t GetChannelOrTag() const { return fChannelOrTag; }

  size_t GetNumRCs() const 
    { return fTransferFunction.GetNumIntegStages(); }
  size_t GetNumCRs() const 
    { return fTransferFunction.GetNumDiffStages(); }
  Double_t GetRCTau(size_t i) const 
    { return fTransferFunction.GetIntegTime(i); }
  Double_t GetCRTau(size_t i) const 
    { return fTransferFunction.GetDiffTime(i); }

  TF1& GetFunction( Double_t begin, Double_t end, Int_t npar ) const;

  Double_t SignalSum(const Double_t *x, const Double_t *par) const;
  template<typename IterT>
  void AddSignalToArray(IterT ArrayStart, IterT ArrayEnd,
                        double ArrayStartTime, double ArrayPeriod,
                        double SignalEnergy, double SignalTime) const;

  const EXOTransferFunction& GetTransferFunction() const 
    { return fTransferFunction; }
  const EXODoubleWaveform& GetModelWaveform() const 
    { return fShapedModel; }
};

//______________________________________________________________________________
inline
Double_t EXOSignalModel::SignalSum( const Double_t *x, const Double_t *par ) const
{
  // Returns the sum of the collection signals WITHOUT BASELINE!
  // par[0] = number of collection signals
  // par[1] = energy of the first collection signal
  // par[2] = time of the first collection signal, in CLHEP standard units
  // par[3], par[4]... = energy and time of remaining collection signals

  Int_t nstep = (Int_t)par[0];
  Double_t return_value = 0.0;

  for(Int_t i=0; i<nstep; i++) {
    // Call through to AddSignalToArray.
    AddSignalToArray(&return_value, &return_value + 1,
                     x[0], 0. /* arbitrary*/,
                     par[1+2*i], par[2+2*i]);
  }
  return return_value;
}

//______________________________________________________________________________
template<typename IterT>
inline
void EXOSignalModel::AddSignalToArray(IterT ArrayStart, IterT ArrayEnd,
                                      double ArrayStartTime, double ArrayPeriod,
                                      double SignalEnergy, double SignalTime) const
{
  // Add the signal to the array, from start (inclusive) to end (exclusive).
  // For long ranges typical in fits, this is more efficient than the old version.
  // Templates allow this function to accept iterator or pointer forms of ArrayStart/End.
  // Note that we avoid using EXOTemplWaveform::InterpolateAtPoint because it would repeat
  // certain checks -- instead do them here so the loop can be tighter.

  // If the length of the model is zero, nothing we can do -- return.
  if(fShapedModel.GetLength() == 0) return;

  // Convert ArrayStartTime to a fractional index number.
  ArrayStartTime -= SignalTime;
  ArrayStartTime -= fShapedModel.GetTOffset();
  ArrayStartTime *= fShapedModel.GetSamplingFreq();

  // Compute the amount to increment the bin index.
  ArrayPeriod *= fShapedModel.GetSamplingFreq();

  // As long as ArrayStartTime is negative (if at all), just use fShapedModel[0].
  while(ArrayStart != ArrayEnd and ArrayStartTime < 0) {
    *ArrayStart += SignalEnergy*fShapedModel[0];
    ArrayStart++;
    ArrayStartTime += ArrayPeriod;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Now ArrayStartTime is non-negative -- we should interpolate until/unless we get to the end of the model.
  // This is where most time is spent, so optimize heavily.
  // In particular, the common case is where the array period is some integer multiple
  // of the period of fShapedModel, and we can shorten the loop in this case.
  /////////////////////////////////////////////////////////////////////////////

  // If ArrayPeriod is an integer, we can omit some steps.
  size_t IntPartOfArrayPeriod = static_cast<size_t>(ArrayPeriod);
  if(ArrayPeriod - IntPartOfArrayPeriod > 0.5) IntPartOfArrayPeriod++; // In case it's just below int.
  if(std::fabs(ArrayPeriod - IntPartOfArrayPeriod)*(ArrayEnd - ArrayStart) < 0.01) {
    // IntPartOfArrayPeriod is close enough to an integer -- over the entire length of the array,
    // drift will be limited to 1% of a period.
    size_t entry = static_cast<size_t>(ArrayStartTime);
    ArrayStartTime -= entry;
    while(ArrayStart != ArrayEnd and entry + 1 < fShapedModel.GetLength()) {
      double interp = (1.0-ArrayStartTime)*fShapedModel[entry] + ArrayStartTime*fShapedModel[entry+1];
      *ArrayStart += SignalEnergy*interp;
      ArrayStart++;
      entry += IntPartOfArrayPeriod;
      // Note that we don't need to update ArrayStartTime.
    }

    while(ArrayStart != ArrayEnd) { // Any entries past the end of fShapedModel.
      *ArrayStart += SignalEnergy*fShapedModel[fShapedModel.GetLength()-1];
      ArrayStart++;
    }
  }
  else {
    // ArrayPeriod is not close to an integer -- do the slow simple thing.
    while(ArrayStart != ArrayEnd) {
      *ArrayStart += SignalEnergy*fShapedModel.InterpolateAtPoint(ArrayStartTime);
      ArrayStart++;
      ArrayStartTime += ArrayPeriod;
    }
  }
}

#endif
