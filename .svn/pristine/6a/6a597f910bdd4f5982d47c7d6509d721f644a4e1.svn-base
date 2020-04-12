#ifndef EXOUWireIndSignalModelBuilder_hh
#define EXOUWireIndSignalModelBuilder_hh

#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

class EXOUWireIndSignalModelBuilder : public EXOVSignalModelBuilder
{
  public:
    EXOUWireIndSignalModelBuilder(const EXOTransferFunction& trans) : 
      fTransferFunction(trans) {}

  protected:
    const EXOTransferFunction& fTransferFunction;

    bool InitializationIsNeeded(const EXOSignalModel& model) const;

    EXOReconUtil::ESignalBehaviorType GetBehaviorType() const
      { return EXOReconUtil::kUWire; }

    void InitializeSignalModel(
           EXODoubleWaveform& signalModel, 
           EXOTransferFunction& trans) const;
    
};

#endif /* EXOUWireIndSignalModelBuilder_hh */
