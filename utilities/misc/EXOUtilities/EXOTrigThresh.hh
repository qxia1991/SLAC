
#ifndef EXOTrigThresh_h
#define EXOTrigThresh_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class EXOTrigThresh
{
public:
  
  EXOTrigThresh();
  EXOTrigThresh(int p, int t);
  ~EXOTrigThresh();
  int Enabled();
  void State(int trigIn, int trigEn, int trigSumEn, int trigSumHyst, int trigSum, int &trigAct, int &trigOut);
  void Reset();

public:
 
  int trigPScale;  //the prescale value
  int trigTHold; //the threshold
  int preScaleCnt; //the current prescale counter
  int nxtAct; //if the trigger has been activated
  int nxtExceed; //if the threshold has been exceeded
  int preScaleCntRst; //flag to reset prescale
  int preScaleCntInc; //flag to increment prescale
  int preScaleCntDec; //flag to decrement prescale
  char curState; //state machine: current state
  char nxtState; //state machine: next state
  int _enabled; //is the threshold active?

};

#endif








