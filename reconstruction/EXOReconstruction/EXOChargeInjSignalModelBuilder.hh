#ifndef EXOChargeInjSignalModelBuilder_hh
#define EXOChargeInjSignalModelBuilder_hh

#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

class EXOChargeInjSignalModelBuilder : public EXOVSignalModelBuilder
{
  public:
    EXOChargeInjSignalModelBuilder(const EXOTransferFunction& trans) : 
      fTransferFunction(trans) {}

  protected:
    const EXOTransferFunction& fTransferFunction;

    bool InitializationIsNeeded(const EXOSignalModel& model) const;

    EXOReconUtil::ESignalBehaviorType GetBehaviorType() const
      { return EXOReconUtil::kChargeInjection; }

    void InitializeSignalModel(
       EXODoubleWaveform& signalModel,
       EXOTransferFunction& trans) const;
    
};

#endif /* EXOChargeInjSignalModelBuilder_hh */
