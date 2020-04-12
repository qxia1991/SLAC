#ifndef EXOSignalModelRegistrant_hh
#define EXOSignalModelRegistrant_hh
#include "EXOReconstruction/EXORecVerbose.hh"
#include <string>

class EXOSignalModelManager;
class EXOSignalModel;
class EXOTalkToManager;
class EXOTimingStatisticInfo;
class EXOSignalModelRegistrant 
{
  public:
    EXOSignalModelRegistrant() : 
      fSigModelMgr(0), fStatistics(0) {}
    virtual ~EXOSignalModelRegistrant();

    void TalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

    void SetTimingStatisticInfo(EXOTimingStatisticInfo* info)
      { fStatistics = info; }
    void SetPrefixName(const std::string& aname) 
      { fPrefix = aname; }

  protected:

    friend class EXOSignalModelManager;

    // Base classes can overload the following to be notified when a signal
    // model has been changed.  Default is to do nothing.
    virtual void NotifySignalModelHasChanged(
      int /*signalOrTag*/, 
      const EXOSignalModel& /*model*/) {} 

    const EXOSignalModelManager* SigModel() const
      { return fSigModelMgr; }

    virtual void SetupTalkTo(const std::string& /*prefix*/, EXOTalkToManager* /*talkTo*/) {}

    void SetStatistic(const std::string& tag, double aVal) const;
    void StartTimer(const std::string& tag, bool reset = true) const;
    void StopTimer(const std::string& tag) const;
    void ResetTimer(const std::string& tag) const;

    EXORecVerbose fVerbose;


  private:
    EXOSignalModelManager*  fSigModelMgr;
    EXOTimingStatisticInfo* fStatistics;
    std::string             fPrefix;
    
    // Called by EXOSignalModelManager to register.
    virtual void RegisterSignalModels(EXOSignalModelManager* mgr)
      { fSigModelMgr = mgr; }
};

#endif /* EXOSignalModelRegistrant_hh */
