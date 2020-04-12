#ifndef EXOTimingStatisticInfo_hh
#define EXOTimingStatisticInfo_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TNamed
#include "TNamed.h"
#endif
#ifndef ROOT_TStopwatch
#include "TStopwatch.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif
#ifndef ROOT_TParameter
#include "TParameter.h"
#endif

class EXOStopwatch : public TStopwatch
{
  public:
    EXOStopwatch() : TStopwatch() {}
    void SetName(const std::string& aname) 
      { fWatchName = aname; }
    const char* GetName() const { return fWatchName; }
    const char* GetTitle() const { return GetName(); }
    void Clear( Option_t* opt = "" )
      { TStopwatch::Clear(opt); fWatchName = ""; }

    void Print(Option_t* /*opt*/ = "") const;

  protected:
    TString fWatchName; // Stopwatch name
    
  ClassDef(EXOStopwatch,1) 
};

class TClonesArray;
class EXOTimingStatisticInfo : public TNamed
{
  public:
    EXOTimingStatisticInfo();
    EXOTimingStatisticInfo(const EXOTimingStatisticInfo& obj);
    EXOTimingStatisticInfo& operator=(const EXOTimingStatisticInfo& obj);
    ~EXOTimingStatisticInfo();

    void Print( Option_t * /*opt*/ = "" ) const;
    void Clear( Option_t* /*opt*/ = "" );
    void Reset();

    void StartTimerForTag(const std::string& tag, bool reset = true);
    void StopTimerForTag(const std::string& tag);
    void ResetTimerForTag(const std::string& tag);

    void SetStatisticForTag(const std::string& tag, double aVal);
    double GetStatisticForTag(const std::string& tag) const;

  protected:
    EXOStopwatch* FindTimerForTag(const std::string& tag) const;
    EXOStopwatch& GetOrCreateTimerForTag(const std::string& tag);

    TParameter<Double_t>* FindStatisticForTag(const std::string& tag) const;
    TParameter<Double_t>& GetOrCreateStatisticForTag(const std::string& tag);

    TClonesArray* fTimingInfo; //-> Timing Info
    TClonesArray* fStatInfo; //-> Timing Info

  ClassDef(EXOTimingStatisticInfo, 1)
};
#endif
