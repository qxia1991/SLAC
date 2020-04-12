#ifndef EXOEventSummaryCoinc_hh
#define EXOEventSummaryCoinc_hh

#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOCalibUtilities/EXOEventSummary.hh"

class EXOEventSummaryCoinc: public EXOEventSummary
{

public:
  EXOEventSummaryCoinc();
  virtual ~EXOEventSummaryCoinc(){;}
  
  Long64_t time_to_tpc_event  ; // shortest time to another tpc event in nanoseconds, 
  Long64_t time_next_tpc_event; // time to next tpc event in nanoseconds, default to blanked out if no following events found 
  Long64_t time_last_tpc_event; // time since last tpc event in nanoseconds, default to blanked out if no preceeding events found 


  //functions
  virtual void Clear();
  
protected:
  
  ClassDef(EXOEventSummaryCoinc,1)
};

#endif
