#ifndef EXOVSignalModelBuilder_hh
#define EXOVSignalModelBuilder_hh

#include "EXOReconstruction/EXOSignalModel.hh"
class EXOVSignalModelBuilder 
{
  public:
    virtual ~EXOVSignalModelBuilder() {}
    bool InitializeSignalModelIfNeeded(EXOSignalModel& model, int channelOrTag) const;

  protected:
    
    virtual bool InitializationIsNeeded(const EXOSignalModel& model) const = 0; 
    virtual EXOReconUtil::ESignalBehaviorType GetBehaviorType() const = 0;

    virtual void InitializeSignalModel(EXODoubleWaveform& signalModel, 
                                       EXOTransferFunction& trans) const = 0; 

};

#endif /* EXOVSignalModelBuilder_hh */
