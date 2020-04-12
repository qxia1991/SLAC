#ifndef EXOUWireSignalModelBuilder_hh
#define EXOUWireSignalModelBuilder_hh

#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

class EXOUWireSignalModelBuilder : public EXOVSignalModelBuilder
{
  public:
    EXOUWireSignalModelBuilder(const EXOTransferFunction& trans) : 
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

#endif /* EXOUWireSignalModelBuilder_hh */
