#ifndef EXOMGMTest_hh_
#define EXOMGMTest_hh_
#include "EXOAnalysisManager/EXOAnalysisModule.hh"

class EXOMGMTest : public EXOAnalysisModule 
{

  public:
    EXOMGMTest() {} 
    int Initialize();
    EventStatus BeginOfRun(EXOEventData *ED);

  DEFINE_EXO_ANALYSIS_MODULE( EXOMGMTest )
}; 


#include "EXOUtilities/EXOPluginUtilities.hh"
EXO_DEFINE_PLUGIN_MODULE(EXOMGMTest, "test")
#endif
