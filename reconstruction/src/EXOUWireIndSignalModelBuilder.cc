//______________________________________________________________________________
//
//  EXOUWireIndSignalModelBuilder builds a signal model for induction signals on U-wires.
//  For more details see InitializeSignalModel.
//
//______________________________________________________________________________
#include "EXOReconstruction/EXOUWireIndSignalModelBuilder.hh"

//______________________________________________________________________________
bool EXOUWireIndSignalModelBuilder::InitializationIsNeeded(const EXOSignalModel& model) const
{
  // Returns whether or not initialization with the given signal model is
  // necessary.  Only checks if the transfer functions of the given
  // EXOSignalModel and in this Builder are equivalent.
  return (model.GetTransferFunction() != fTransferFunction);
}

//______________________________________________________________________________
void EXOUWireIndSignalModelBuilder::InitializeSignalModel(
    EXODoubleWaveform& shapeModel, 
    EXOTransferFunction& transfer) const
{
  // Initialize the signal model with a U-wire signal type.  This function uses
  // a model waveform defined by EXOMiscUtil::unshaped_signal, and shapes it
  // with the given transfer function.  The output of this is saved in
  // shapeModel.

  // Save the transfer function
  transfer = fTransferFunction;

  ////////////////////////////////////////////////////////////////////
  // Initialize parameters of the model here.  There are many hard-coded values
  // which should be somehow centralized/removed in the future.
  const int nsample = 2048;
  const size_t init_length = 100000;
  EXODoubleWaveform unshaped_model_initializer;

  unshaped_model_initializer.SetLength(init_length);
  unshaped_model_initializer.SetSamplingPeriod(0.1*CLHEP::microsecond);
  shapeModel.SetSamplingPeriod(CLHEP::microsecond);
  ////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////
  const int uwireoffset = 50;
  Double_t totalshapingtimes = 0.0;
  size_t ndiffs = fTransferFunction.GetNumDiffStages();
  size_t nintegs = fTransferFunction.GetNumIntegStages();
  for(size_t i=0; i<ndiffs; i++){
    totalshapingtimes += fTransferFunction.GetDiffTime(i);
  }
  for(size_t i=0; i<nintegs; i++){
    totalshapingtimes += fTransferFunction.GetIntegTime(i);
  }
  Double_t shaped_model_tmax = 
    std::min((nsample-uwireoffset-2)*CLHEP::microsecond, 
             4*totalshapingtimes);
  shapeModel.SetTOffset(-uwireoffset * CLHEP::microsecond);
  shapeModel.SetLength((size_t) ( (shaped_model_tmax - shapeModel.GetMinTime())/
                                     shapeModel.GetSamplingPeriod() + 2));

  // Here we use the unshaped signal for induction pulses specified in EXOMiscUtil
  const EXODoubleWaveform& unshaped_sig = EXOMiscUtil::unshaped_ind_signal();
  for(size_t i=0; i<init_length; i++) {
    unshaped_model_initializer[i] = (i >= unshaped_sig.GetLength()) ? 
      unshaped_sig[unshaped_sig.GetLength()-1] : unshaped_sig[i]; 
  }
  Double_t gain = fTransferFunction.GetGain();
  fTransferFunction.Transform(&unshaped_model_initializer);
  
  Int_t shaped_model_length = shapeModel.GetLength();
  Double_t tmin = shapeModel.GetMinTime();
  Double_t per = shapeModel.GetSamplingPeriod();
  for(Int_t i = 0; i<shaped_model_length; i++) {
    shapeModel[i] = 
      unshaped_model_initializer.InterpolateAtPoint(tmin + i*per + 66.9*CLHEP::microsecond) / gain;
  }

}

