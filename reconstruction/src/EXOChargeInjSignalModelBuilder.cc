//______________________________________________________________________________
// EXOChargeInjSignalModelBuilder
//  Builds signal models for Charge Injection signals.  See the implementation of
//  InitializeSignalModel for more details.
//  8/9/2016  Copied APDSignalModelBuilder and Renamed where neccesary
//______________________________________________________________________________
#include "EXOReconstruction/EXOChargeInjSignalModelBuilder.hh"

//______________________________________________________________________________
bool EXOChargeInjSignalModelBuilder::InitializationIsNeeded(const EXOSignalModel& model) const
{
  // Check is Initialization is needed.  In this case, we check if the transfer
  // functions are the same.
  return (model.GetTransferFunction() != fTransferFunction);
}

//______________________________________________________________________________
void EXOChargeInjSignalModelBuilder::InitializeSignalModel(
    EXODoubleWaveform& shapeModel, 
    EXOTransferFunction& transfer) const
{
  // Initialize the signal model with an Charge Injection signal type.  This builder builds
  // the signal of an Charge Injection signal using the assumption that the initial signal
  // input into the electronics is a step function, which is a generally good
  // approximation for Charge Injection signals.  This step function is then transformed by
  // the input transfer function.  The results are stored in shapeModel which
  // is an EXODoubleWaveform. 

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
  size_t ndiffs = fTransferFunction.GetNumDiffStages();
  size_t nintegs = fTransferFunction.GetNumIntegStages();
  Double_t totalshapingtimes = 0.0;
  for(size_t i=0; i<ndiffs; i++){
    totalshapingtimes += fTransferFunction.GetDiffTime(i);
  }
  for(size_t i=0; i<nintegs; i++){
    totalshapingtimes += fTransferFunction.GetIntegTime(i);
  }
  Double_t shaped_model_tmax = std::min(double(nsample-2)*CLHEP::microsecond, 4*totalshapingtimes);
  shapeModel.SetTOffset(0.0);
  shapeModel.SetLength((int) ( (shaped_model_tmax - shapeModel.GetMinTime())/shapeModel.GetSamplingPeriod() + 2));

  // Set up the step function
  for(size_t i=0; i<init_length; i++) {
    if (i<669) {
      unshaped_model_initializer[i] = 0.0;
    }
    else {
      unshaped_model_initializer[i] = 1.0;
    }
  }

  // Transform the step function.
  Double_t gain = fTransferFunction.GetGain();
  fTransferFunction.Transform(&unshaped_model_initializer);
  
  // Save the transformed function in shapeModel.
  Int_t shaped_model_length = shapeModel.GetLength();
  Double_t tmin = shapeModel.GetMinTime();
  Double_t per = shapeModel.GetSamplingPeriod();
  for(Int_t i = 0; i<shaped_model_length; i++) {
    shapeModel[i] = unshaped_model_initializer.InterpolateAtPoint(tmin + i*per + 66.9*CLHEP::microsecond) / gain;
  }

}

