#ifndef EXOChannelSignals_hh
#define EXOChannelSignals_hh
#include "EXOReconstruction/EXOSignal.hh"
#include "EXOReconstruction/EXOReconUtil.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "TObject.h"
#include <set>
#include <cstddef> //for size_t

class EXOChannelSignals : public TObject
{
  public:
    typedef std::set<EXOSignal> AllSignals;
    typedef std::map<std::string, double> InformationCache;

    EXOChannelSignals();
    void Clear(Option_t * /*opt*/ = "");
    void Print(Option_t* /*opt*/ = "") const;

    void AddAndSaveChannelData(const EXOChannelSignals& other);
    void Add(const EXOChannelSignals& other);

    void AddSignal(const EXOSignal& sig);
    size_t GetNumSignals() const;

    const EXOSignal* Next() const;
    void RemoveSignal(const EXOSignal& sig);
    void ResetIterator() const;

    void SetWaveform(const EXOWaveform * wf);
    const EXOWaveform* GetWaveform() const;

    void SetChannel(int chan) { fChannel = chan; }
    int GetChannel() const { return fChannel; }

    void SetBehaviorType(EXOReconUtil::ESignalBehaviorType type) { fBehaviorType = type; }
    EXOReconUtil::ESignalBehaviorType GetBehaviorType() const { return fBehaviorType; }

    void SetCacheInformationFor(std::string infoName, double info); 
    bool HasCacheInformationFor(std::string infoName) const;
    double GetCacheInformationFor(std::string infoName) const;
    const InformationCache& GetAllCacheInformation() const
      { return fCacheInformation; }

    double GetTimeToSignal(double Time) const;
    void MakeSimilarTo(const EXOChannelSignals& other);
    bool IsSimilarTo(const EXOChannelSignals& other) const;

  protected:

    int fChannel;        // The software channel of these signals
    EXOReconUtil::ESignalBehaviorType fBehaviorType; // The type of behavior of this channel.
    AllSignals fSignals; // Contains all signals of this channel.
    InformationCache fCacheInformation; // Information the fitters, etc. can cache. 
    mutable AllSignals::const_iterator fIter;    //! Iterator to next position
    const EXOWaveform *fRefWf; // A pointer to the waveform of this channel.

  ClassDef( EXOChannelSignals, 0 )
};

#endif
