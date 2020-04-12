#ifndef EXOReconProcessList_hh
#define EXOReconProcessList_hh
#include <map>
#include <cstddef> //for size_t
#include "EXOReconstruction/EXOReconUtil.hh"

class EXOWaveformData;
class EXOWaveform;
class EXOReconProcessList 
{
  public:
    struct WaveformWithType {
      const EXOWaveform* fWf;
      EXOReconUtil::ESignalBehaviorType fType;
    };

    void Add(const EXOReconProcessList& list, bool force = true); 
    void Add(const EXOWaveformData& list, bool force = true); 
    void Add(const EXOWaveform& wf, 
             EXOReconUtil::ESignalBehaviorType type, 
             bool force = true);

    size_t GetSize() const { return fProcessWFList.size(); }

    // Following are for looping over the waveforms in the process list
    void ResetIterator() const;

    const EXOWaveform* GetNextWaveform() const;
    const EXOWaveform* GetWaveformWithChannelOrTag(int chanOrTag) const;

    const WaveformWithType* GetNextWaveformAndType() const;
    const WaveformWithType* GetWaveformAndTypeWithChannelOrTag(int chanOrTag) const;

    bool IsChannelOrTagInList(int chanOrTag) const
      { return (fProcessWFList.find(chanOrTag) != fProcessWFList.end() ); }

    EXOReconProcessList GetSubProcessListOfType(EXOReconUtil::ESignalBehaviorType type) const;

    EXOReconProcessList();

  protected:
    typedef std::map<int, WaveformWithType> ProcMap;

    ProcMap fProcessWFList; // List of waveforms that need/can be processed.
    mutable ProcMap::const_iterator fIter;
     

};

#endif /* EXOReconProcessList_hh */
