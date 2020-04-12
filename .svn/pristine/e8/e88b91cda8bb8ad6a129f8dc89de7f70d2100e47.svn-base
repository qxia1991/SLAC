#ifndef EXODataTrimModule_hh
#define EXODataTrimModule_hh
#include "EXOUtilities/EXODimensions.hh"

#include "EXOAnalysisModule.hh"

class EXODataTrimModule : public EXOAnalysisModule 
{

private :

  // These pointers will reference data to be read-in from the command line

  bool keepit[NUMBER_READOUT_CHANNELS];
  bool neighbor[NUMBER_READOUT_CHANNELS];

public :

  EventStatus ProcessEvent(EXOEventData *ED);

  DEFINE_EXO_ANALYSIS_MODULE( EXODataTrimModule )
};
#endif
