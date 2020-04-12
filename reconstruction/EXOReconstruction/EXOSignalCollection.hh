#ifndef EXOSignalCollection_hh
#define EXOSignalCollection_hh
#include "TObject.h"
#include "EXOReconstruction/EXOChannelSignals.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <map>
#include <cstddef> //for size_t

class EXOSignalCollection: public TObject
{
  public:
    typedef std::map<int,EXOChannelSignals> ChannelSignalsMap;

  public:
    void Clear(Option_t * /*opt*/ = "");
    

    void Add(const EXOSignalCollection& cs);
    void AddChannelSignal(const EXOChannelSignals& cs);
    void AddSignalOnWaveform(const EXOSignal& sig, const EXOReconProcessList::WaveformWithType* wfWithType);
    void SetCacheInformationForChannel(std::string infoName, double info, int channel);
    size_t GetNumChannelSignals() const;

    void ResetIterator() const;
    const EXOChannelSignals* Next() const;
    const EXOChannelSignals* NextOfType(EXOReconUtil::ESignalBehaviorType type) const;

    const EXOChannelSignals* GetSignalsForChannel(int channel) const;
    EXOSignalCollection GetCollectionForType(EXOMiscUtil::EChannelType type) const;

  protected:
    ChannelSignalsMap fChannelSignals; // Contains all ChannelSignals of this event 
    mutable ChannelSignalsMap::const_iterator fIter;

  ClassDef( EXOSignalCollection, 1 )
};

#endif
