#ifndef EXOVWireSignalModelBuilder_hh
#define EXOVWireSignalModelBuilder_hh

#include "EXOReconstruction/EXOVSignalModelBuilder.hh"

class EXOVWireSignalModelBuilder : public EXOVSignalModelBuilder
{
  public:
    EXOVWireSignalModelBuilder(const EXOTransferFunction& trans,
                               double DriftVelocity,
                               double CollectionDriftVelocity = -1.0,
                               double Z_Separation = 0.0) : 
      fTransferFunction(trans),
      fDriftVelocity(DriftVelocity),
      fCollectionDriftVelocity(CollectionDriftVelocity >= 0.0 ? CollectionDriftVelocity : fDriftVelocity),
      fZ_Separation(Z_Separation)
      {}

  protected:
    const EXOTransferFunction& fTransferFunction;
    const double fDriftVelocity;
    const double fCollectionDriftVelocity;
    const double fZ_Separation;

    bool InitializationIsNeeded(const EXOSignalModel& model) const;

    EXOReconUtil::ESignalBehaviorType GetBehaviorType() const
      { return EXOReconUtil::kVWire; }

    void InitializeSignalModel(
           EXODoubleWaveform& signalModel, 
           EXOTransferFunction& trans) const;
    
};

#endif /* EXOVWireSignalModelBuilder_hh */
