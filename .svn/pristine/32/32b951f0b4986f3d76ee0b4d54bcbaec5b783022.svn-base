#ifndef EXOMGMPluginTwo_hh_
#define EXOMGMPluginTwo_hh_
#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include <iostream>

/* An example plugin with custom init and fini functions.
   Also, this class can have multiple instantiations. */

class EXOMGMPluginTwo : public EXOAnalysisModule 
{

  public:
    EXOMGMPluginTwo() {}
    ~EXOMGMPluginTwo() { ; }
    int Initialize();
    EventStatus BeginOfRun(EXOEventData *ED);
    int TalkTo(EXOTalkToManager *tm);

    // This must be overloaded since we want to allow multiple
    // copies. 
    bool CanHaveMultipleClassInstances() const { return true; }

  void SetLoggedName(std::string aval) { fMyLoggedName = aval; }
  protected:
    std::string fMyLoggedName;

  DEFINE_EXO_ANALYSIS_MODULE( EXOMGMPluginTwo )
}; 

inline int my_custom_init( const char* input_str, const char* svn_rev ) 
{
    std::cout << "In custom init" << std::endl;
    std::cout << input_str << std::endl;
    std::cout << "Svn rev: " << svn_rev << ", but compiled with: " << SVN_REV << std::endl;
    return 0;
}

inline void my_custom_fini( ) 
{
    std::cout << "In custom fini" << std::endl;
}


#include "EXOUtilities/EXOPluginUtilities.hh"
EXO_DEFINE_PLUGIN_MODULE_CUSTOM(EXOMGMPluginTwo, "test_two", my_custom_init, my_custom_fini)
#endif
