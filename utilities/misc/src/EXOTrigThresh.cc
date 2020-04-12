//______________________________________________________________________________
// EXOTrigThresh class
// Author: Nicole Ackerman
// Last Modified: 5-24-10
// This class works with EXOTrigger to simulate
// the EXO trigger.  This class instances the 
// state machine from the FPGA's.
// ==================================================
// Changelog:
// 5-24-10: Modified state machine to match new 
//          VHDL code to fix trigger run-away issue.
// 1-22-10: Moved trigger from geant simulation  
//          to individual module. Added the ability
//          to disable a trigger.
// ================================================== 


#include "EXOUtilities/EXOTrigThresh.hh"

//This object lives inside of EXOTrigger
//It defines each of the thresholds independently
// since in the FPGA each exists as its own state
// machine.  The VHDL code has been directly transcribed
// here to C++ for verification and easy modification.  
//Some signals are ignored or hardcoded.

//Default Constructor

EXOTrigThresh::EXOTrigThresh()
{
  trigPScale=0;
  trigTHold=100;
  preScaleCnt = trigPScale;
  curState='d';
  nxtState='d';
  _enabled=1;
}

//3 initializers for the individual trigger
EXOTrigThresh::EXOTrigThresh(int p, int t)
{
  _enabled=1;
  if(t==0) _enabled=0;
  trigPScale = p;
  trigTHold = t;
  preScaleCnt = trigPScale;
  curState='d';
  nxtState='d';
}

EXOTrigThresh::~EXOTrigThresh()
{
  //destructor
}

int EXOTrigThresh::Enabled()
{
  return _enabled;
}

void EXOTrigThresh::Reset()
{
  curState='d';
  nxtState='d';
}

void EXOTrigThresh::State(int trigIn, int trigEn, int trigSumEn, int trigSumHyst, int trigSum, int &trigAct, int &trigOut)
{
  // STATE MACHINE:
  // ST_DIS    0 'd'
  // ST_RUN    1 'r'
  // ST_EXCEED 2 'e'
  // ST_PRE    3 'p'
  // ST_ACT    4 'a'
  // ST_WAIT   5 'w'

  //The clocking speed on the state machine is 20 MHz and samples are collected at 1MHz
  for(int clk=0; clk<19; clk++)
    {
      // State Machine
      curState = nxtState;
      switch(curState)
	{
	  
	  // Trigger is disabled
	case 'd':
	  nxtAct         = 0;
	  nxtExceed      = 0;
	  preScaleCntRst = 0;
	  preScaleCntInc = 0;
	  preScaleCntDec = 0;
	  nxtState       = 'r';
	  break;
	  // Running, wait for trigger
	case 'r' :
	  nxtAct         = 0;
	  preScaleCntRst = 0;
	  preScaleCntInc = 0;
	  preScaleCntDec = 0;
	  
	  // Trigger is exceeded
	  if(trigSumEn == 1 && trigEn == 1 && trigSum >= trigTHold) 	      
	    {
	      nxtExceed = 1;
	      nxtState  = 'e';
	    }
	  else
	    {
	      nxtExceed = 0;
	      nxtState  = curState;
	    }
	  break;
	  // Trigger is exceeded
	case 'e' :
	  nxtExceed      = 1;
	  preScaleCntInc = 0;
	  // Check state of higher priority triggers
	  if( trigIn == 0)
	    {
	      // Prescale is zero, gen trigger
	      if (preScaleCnt == 0)
		{
		  nxtAct         = 1;
		  preScaleCntRst = 1;
		  preScaleCntDec = 0;
		  nxtState       = 'a';
		}
	      // Decrement pre-scale
	      else
		{
		  nxtAct         = 0;
		  preScaleCntRst = 0;
		  preScaleCntDec = 1;
		  nxtState       = 'p';
		}
	    }
	  else
	    {
	      nxtAct         = 0;
	      preScaleCntRst = 0;
	      preScaleCntDec = 0;
	      nxtState       = 'w';
	    }
	  break;
	  
	  // Pre-scale was decremented
	case 'p':
	  nxtAct         = 0;
	  preScaleCntRst = 0;
	  preScaleCntDec = 0;
	  
	  // Trigger case has gone away
	  if((trigSumEn == 1) && (trigSumHyst < trigTHold))
	    {
	      nxtExceed      = 0;
	      preScaleCntInc = 0;
	      nxtState       = 'r';
	    }
	  // A higher priority trigger has gone active
	  else if(trigIn == 1)
	    {
	      nxtExceed      = 1;
	      preScaleCntInc = 1;
	      nxtState       = 'w';
	    }
	  // No Change
	  else
	    {
	      nxtExceed      = 1;
	      preScaleCntInc = 0;
	      nxtState       = curState;
	    }
	  break;
	  
	  // Trigger is active
	case 'a':
	  preScaleCntRst = 0;
	  preScaleCntInc = 0;
	  preScaleCntDec = 0;
	  nxtExceed      = 1; //changed 5/24/10

	  // Trigger case has gone away
	  // trigEn will be zero after a higher trigger has been exceeded OR 
	  //   this threshold has already been registered to trigger
	  if(trigEn == 0) //changed 5/24/10
	    {
	      nxtAct         = 0;
	      nxtState       = 'w'; //changed 5/24/10
	    }
	  else
	    {
	      //nxtExceed      = 1; //changed 5/24/10
	      nxtAct         = 1;
	      nxtState       = curState;
	    }
	  break;
	  
	  // Trigger is not active, trigger is exceeded
	case 'w':
	  nxtAct         = 0;
	  preScaleCntRst = 0;
	  preScaleCntInc = 0;
	  preScaleCntDec = 0;
	  
	  // Trigger case has gone away
	  if( trigSumEn == 1 && trigSumHyst < trigTHold)
	    {
	      nxtExceed      = 0;
	      nxtState       = 'r';
	    }
	  else
	    {
	      nxtExceed      = 1;
	      nxtState       = curState;
	    }
	  break;
	  
	  // Default Case
	default:
	  nxtAct         = 0;
	  preScaleCntRst = 0;
	  preScaleCntInc = 0;
	  preScaleCntDec = 0;
	  nxtExceed      = 0;
	  nxtState       = 'd';
	}
      
      
      // Pre-Scale Counter
      if(preScaleCntRst == 1)
	preScaleCnt = trigPScale;
      else if (preScaleCntInc == 1)
	preScaleCnt = preScaleCnt + 1;
      else if (preScaleCntDec == 1)
	preScaleCnt = preScaleCnt - 1;
      
      // State transition
      
    } //ends the clock loop

  //Output
  trigOut = (nxtExceed || trigIn);
  trigAct = nxtAct;
}
