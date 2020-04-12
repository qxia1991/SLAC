#ifndef EXOAPDSignalModelBuilder_hh
#define EXOAPDSignalModelBuilder_hh

#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

class EXOAPDSignalModelBuilder : public EXOVSignalModelBuilder
{
  public:
    EXOAPDSignalModelBuilder(const EXOTransferFunction& trans) : 
      fTransferFunction(trans) {}

  protected:
    const EXOTransferFunction& fTransferFunction;

    bool InitializationIsNeeded(const EXOSignalModel& model) const;

    EXOReconUtil::ESignalBehaviorType GetBehaviorType() const
      { return EXOReconUtil::kAPD; }

    void InitializeSignalModel(
       EXODoubleWaveform& signalModel,
       EXOTransferFunction& trans) const;
    
};

#endif /* EXOAPDSignalModelBuilder_hh */
