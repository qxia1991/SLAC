#ifndef EXOSignalModelManager_hh
#define EXOSignalModelManager_hh
#include <vector>
#include "EXOReconstruction/EXOSignalModel.hh"
#include "EXOReconstruction/EXOChannelSignals.hh"

class EXOSignalModelRegistrant;
class EXOVSignalModelBuilder;
class EXOSignalModelManager
{
  public:
    ~EXOSignalModelManager();

    void BuildSignalModelForChannelOrTag(
      int channelOrTag, 
      const EXOVSignalModelBuilder& signalBuilder);

    const EXOSignalModel* GetSignalModelForChannelOrTag(int channelOrTag) const;

    void AddRegisteredObject(EXOSignalModelRegistrant* reg);

    void RemoveRegisteredObject(EXOSignalModelRegistrant* reg);

  private:
    typedef std::map<int,EXOSignalModel> SignalModelMap;
    typedef std::set<EXOSignalModelRegistrant*> RegObjList;

    SignalModelMap        fSignalModelMap; // Map of the held signal models
    RegObjList            fRegisteredObjs; // list of registered objects
    mutable std::set<int> fDerivedChannelNumbers; // cache which tracks which
						  // channels/tags are set
						  // using
						  // AddDerivedTransferFunctionForChannelOrTag.

    void NotifyRegistrantsOfChange(int channelOrTag, const EXOSignalModel& mod);

    // EXOVDefineReconProcessList is a friend of this class so it can call
    // AddDerivedTransferFunctionForChannelOrTag
    friend class EXOVDefineReconProcessList;

    void AddDerivedSignalModelForChannelOrTag(
      int channelOrTag,
      const EXOVSignalModelBuilder& builder) const;
};

#endif
