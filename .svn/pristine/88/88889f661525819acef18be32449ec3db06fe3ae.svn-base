// If you need to edit this file, please do so in src/EXOROOTModules.cc or
// make/makeEXOROOT.py so that it will propagate to the automatically built
// source file.
#include "EXOROOT/EXOROOTModules.hh"

#include "TSystem.h"
#include <dlfcn.h>
#include "EXOUtilities/EXOPluginUtilities.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
//______________________________________________________________________________
std::string EXOROOT::LoadPlugin( const std::string& libraryName )
{
  // Trys to load an EXOAnalysis plugin located at name and, when successful
  // returns a the name of new module.  This allows the user to use this name
  // to get a new object from the root interpreter (either CINT or python).
  // Additionally, since this makes use of the ROOT dictionary built for the
  // module during the plugin build phase, this functionality allows the user
  // to access functions in the plugin module not defined in the
  // EXOAnalysisModule interface as is required in compiled C++ code.  Returns
  // "" on failure. 
  //
  // The correct way to do this is to do the following, 
  // In python:
  // 
  // plugin_name = ROOT.EXOROOT.LoadPlugin(libName)
  // if plugin_name == "": print "Error"
  // new_analysis_module = getattr(ROOT, plugin_name)()
  //
  // The last line is to properly pass in the EXOAnalysisManager pointer since
  // these plugins can't be automatically wrapped like the rest of the modules
  // in the EXOROOT namespace.
  //
  // In CINT, you must explicitly generate the analysis module so you must know
  // the name of the plugin class inside the plugin.  For example, for exosim
  // (EXOGeant4Module), you must do:
  //
  // if (EXOROOT::LoadPlugin(libName) == "" ) cout << "Error" << endl;
  // EXOGeant4Module mymod;
  //
  // which will work since the loading of the plugin properly lets ROOT know
  // about the class.

  LogEXOMsg(Form("Loading module from file: %s", libraryName.c_str()), EEDebug); 

  char* error = NULL;
  /* Try to open the plugin file */
  EXOPlugin::dlsym_exo_file_handle open_file = (EXOPlugin::dlsym_exo_file_handle) dlopen( libraryName.c_str(), RTLD_LAZY );

  if (!open_file) {
    std::stringstream output;
    output << "Unable to load file: " << libraryName << std::endl;
    output << "Output error: "        << dlerror() << std::endl;
    LogEXOMsg(output.str(), EEError);
    return "";
  }

  /* Try to obtain the name function. */
  EXOPlugin::get_exo_plugin_name_t*  name_function = 
    (EXOPlugin::get_exo_plugin_name_t*)  dlsym( open_file, "exo_get_plugin_name_t" );

  if (!name_function || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'get_name' in file: "     << libraryName << std::endl;
    output << "Output error: "                                  << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
    return "";
  }

  int result = gSystem->Load(libraryName.c_str());
  if (result < 0) return "";

  return std::string(name_function());
}

