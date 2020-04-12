//______________________________________________________________________________
//
//  EXOVSignalModelBuilder provides an interface that enables the building of a
//  signal model.  The point is to allow for different types of signal models
//  that users in the future might create, for example a signal model for
//  summed signals, a signal model for complex channel systems.  Classes must
//  overload the following functions of this class:
//
//    GetBehaviorType()  // Returns the behavior type, for example kAPD,
//                       // kVWire, kUWire, kDerived.  This allows some
//                       // processors to treat this signal as this type.  For
//                       // example, if we want a process to treat this as an
//                       // APD signal, then this behavior type should be set
//                       // as such.
//    InitializationIsNeeded() // checks to see if (re)-initialization is
//                             // needed.  Should return true if so, false if
//                             // not.
//    InitializeSignalModel() // Actually initializes the input signal model
//                            // given the input transfer function.  Classes
//                            // should here save in the signal model waveform the
//                            // expected shaped of the channel. 
//
//  A signal model is then built by calling the InitializeSignalModelIfNeeded
//  function of the builder.  How this works in practice is the following:
//
//    class EXOMySignalBuilder : public EXOVSignalModelBuilder {
//      ..  // Class defining how a certain signal should be built
//    };
//  
//    int mytag = -1;  // Can be any integer, should be the channel number for
//                     // hardware, or a negative integer for derived waveforms
//    EXOSignalModel model;
//    EXOMySignalBuilder builder;
//
//    // Actually build the signal model
//    builder.InitializeSignalModelIfNeeded(model, mytag);
//
// Originally built: W. Feldmeier, M. Marino, Dec 2011
//______________________________________________________________________________
#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

//______________________________________________________________________________
bool EXOVSignalModelBuilder::InitializeSignalModelIfNeeded(
  EXOSignalModel& model, 
  int channelOrTag) const
{
  // Initialize the signal model if we need to and in this case return true.
  // If we don't need to (re-)initialize then return false. 

  // Check to see if initialization is needed or if the channel/tag or behavior
  // are different.
  if (not InitializationIsNeeded(model) && 
      model.GetChannelOrTag() == channelOrTag && 
      model.GetBehaviorType() == GetBehaviorType()) return false;

  model.fChannelOrTag = channelOrTag;
  model.fBehaviorType = GetBehaviorType();

  InitializeSignalModel(model.fShapedModel, model.fTransferFunction);
  return true;
}

