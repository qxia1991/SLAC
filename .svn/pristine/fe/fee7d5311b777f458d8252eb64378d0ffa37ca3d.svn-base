#ifndef EXODigitizeSignalModule_hh
#define EXODigitizeSignalModule_hh

#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXOMCAPDHitInfo.hh"
#include <vector>
#include <queue>

class EXOEventData;

class EXODigitizeSignalModule: public EXOInputModule 
{

  protected :
  
    EXOEventData               fEventData;
  
    std::queue<std::vector<EXOMCPixelatedChargeDeposit> > fChargeDeps;
    std::queue<std::vector<EXOMCAPDHitInfo> >             fAPDHits;
    int            fEventNumber;
    
  public :
  
    EXODigitizeSignalModule( );
    virtual ~EXODigitizeSignalModule() {}
  
    EXOEventData* GetNextEvent();
    int TalkTo(EXOTalkToManager*);
    bool reads_from_file() const {return false;}
  
    void AddChargePointWithString(std::string pointString);
    void AddChargePoint(double upos, double vpos, double zpos, 
                        double time, double energy);
    void AddChargePoint(const EXOMCPixelatedChargeDeposit& pcd)
      { if (fChargeDeps.empty()) fChargeDeps.push(std::vector<EXOMCPixelatedChargeDeposit>());
        fChargeDeps.back().push_back(pcd); }
  
    void AddAPDHitWithString(std::string hitString);
    void AddAPDHit(int gangNo, double time, double charge);
    void AddAPDHit(const EXOMCAPDHitInfo& apdhit) 
    { if (fAPDHits.empty()) fAPDHits.push(std::vector<EXOMCAPDHitInfo>());
      fAPDHits.back().push_back(apdhit); }

    void Clear();
  
    // Thie allows multiple events to be filled, very easily:
    // That is, one can Add hits and charge deposites and then
    // call NextExent to continue with the next event.
    void NextEvent() {  fChargeDeps.push(std::vector<EXOMCPixelatedChargeDeposit>()); 
                        fAPDHits.push(std::vector<EXOMCAPDHitInfo>()); }
  
    virtual int  get_run_number() { return 0; }
    virtual int  get_event_number() { return fEventNumber; }

    DEFINE_EXO_ANALYSIS_MODULE( EXODigitizeSignalModule )
  
};
#endif

  


