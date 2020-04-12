//______________________________________________________________________________
//  
//  EXOAnalysisModuleFactory, Created M. Marino Nov 2010
//
//   EXOAnalysisModuleFactory handles all the building and destroying of all
//   EXOAnalysisModules.  Modules self- register, meaning that this class does
//   not need to actively keep track of who has added which class to the
//   repository.  This class also keeps track of plugins, though through a
//   mostly abstracted interface.  That is, instead of automatically
//   registering, plugins must be registered themselves using
//   RegisterOnePlugin.  
//
//   Example usage of this class:
//
//   EXOAnalysisModule* mod = EXOAnalysisModuleFactory::GetInstance().BuildAnalysisModule( "ClassName" );
//
//   where "ClassName" could also be "nickname".  
//
//   Any AnalysisModule built this way *must* be destroyed using
//   EXOAnalysisModuleFactory::DestroyAnalysisModule().  This is to ensure that
//   the correct destructor is called and properly abstracts the handling of
//   objects created from plugins.
//
//   It is also possible to find the correct module to open a particular file by doing:
//  
//    EXOInputModule* input = EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile( "myfile");
//  
//   This will return the correct input module for the particular file. 
//
//______________________________________________________________________________
#include "EXOAnalysisManager/EXOAnalysisModuleFactory.hh"
#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include <dlfcn.h>
#include "EXOUtilities/EXOPluginUtilities.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TROOT.h"
//______________________________________________________________________________
//
// "Hidden" class for Plugin building.  This is essentially a private class
// used only internally by EXOAnalysisModuleFactory so we put it in the source
// file.
//
class EXOAnalysisPluginBuilder : public EXOVAnalysisModuleFactoryBuilder
{
/*! Concrete implementation of builder classes for Plugins. */
  public:
    EXOAnalysisPluginBuilder( const std::string& name, 
                              const std::string& nickname,
                              EXOPlugin::create_exo_plugin_t*  create,
                              EXOPlugin::destroy_exo_plugin_t* destroy,
                              EXOPlugin::fini_exo_plugin_t*    fini,
                              EXOPlugin::dlsym_exo_file_handle openfile) : 
      EXOVAnalysisModuleFactoryBuilder( name, nickname ), 
      fPluginCreator( create ),
      fPluginDestroyer( destroy ),
      fPluginFini( fini ),
      fOpenFile( openfile ) {} 
    virtual ~EXOAnalysisPluginBuilder();
    virtual EXOAnalysisModule* BuildAnalysisModule() 
      { return fPluginCreator(); } 
    virtual void DestroyAnalysisModule( EXOAnalysisModule* mod ) 
      { fPluginDestroyer( mod ); } 

    /* To check whether it's an input module, we have to create one and see...
       We delete it right away, and save static information so it's only done once
       (or not at all, if we never ask). */
  virtual bool CreatesInputModule( )
    { static bool ret = CheckIsInputModule(); return ret; } 
  private:
    bool CheckIsInputModule()
    {
      // We should only do this once, since it's costly.
      EXOAnalysisModule* mod = BuildAnalysisModule();
      bool ret = (dynamic_cast<EXOInputModule*>(mod) != NULL);
      DestroyAnalysisModule(mod);
      return ret;
    }
    EXOPlugin::create_exo_plugin_t*  fPluginCreator;
    EXOPlugin::destroy_exo_plugin_t* fPluginDestroyer;
    EXOPlugin::fini_exo_plugin_t*    fPluginFini;
    EXOPlugin::dlsym_exo_file_handle fOpenFile;
};



//______________________________________________________________________________
EXOAnalysisModuleFactory& EXOAnalysisModuleFactory::GetInstance()
{
  /* Get the instance of the singleton. */

  static EXOAnalysisModuleFactory fgModuleFactory;
  return fgModuleFactory;
}

//______________________________________________________________________________
void EXOAnalysisModuleFactory::Register( EXOVAnalysisModuleFactoryBuilder* builder,
                                         const std::string& name,
                                         const std::string& nickname )
{
  // Register a builder class.  Users will gnenerally never call
  // this function directly, but the function registers with the 
  // factory a builder which is known by a name and a nickname.  

  if ( fBuilderMap.find( name ) != fBuilderMap.end() ) {
    std::stringstream output;
    output << "EXOAnalysisModuleFactory duplicate name (" << name 
           << ")" << std::endl;
    LogEXOMsg(output.str(), EEError);
    return;
  } 
  fBuilderMap[ name ] = builder;
  if ( nickname != "" ) {
    if ( fNicknameMap.find( nickname ) != fNicknameMap.end() ) {
      std::stringstream output;
      output << "EXOAnalysisModuleFactory duplicate nickname (" 
             << nickname <<"), replacing." << std::endl;
      LogEXOMsg(output.str(), EEWarning);
    }
    fNicknameMap[ nickname ] = name;
  }
}

//______________________________________________________________________________
void EXOAnalysisModuleFactory::Unregister( EXOVAnalysisModuleFactoryBuilder* builder )
{

  // For completeness, include the deregistration.  Again, this
  // will likely never be called directly, but this removes 
  // a designated builder from being known by the factory 
  EXOFactoryNameMap::iterator iter;
  std::string name;
  for ( iter = fBuilderMap.begin(); iter != fBuilderMap.end(); iter++ ) {
    if ( iter->second == builder ) {
      name = iter->first;
      fBuilderMap.erase( iter );
      break;
    } 
  }

  /* In principle, the following could happen if the deregistration happens
     at the end of execution.  Therefore, we ignore the cases when the builder
     was not found. */
  if ( name == "" ) return;

  EXOFactoryNicknameMap::iterator nick_iter;
  for ( nick_iter = fNicknameMap.begin(); nick_iter != fNicknameMap.end(); nick_iter++ ) {
    if ( nick_iter->second == name ) {
      fNicknameMap.erase( nick_iter );
      break;
    } 
  }

}

//______________________________________________________________________________
EXOAnalysisModule* EXOAnalysisModuleFactory::BuildAnalysisModule( const std::string& nameOrNickname ) 
{

  // Generate an analysis module using the name of the class or
  // the nickname.  All modules generated this way should be 
  // deleted using EXOAnalysisModuleFactory::DestroyAnalysisModule.  
  std::string localName = nameOrNickname;
  if ( fNicknameMap.find( nameOrNickname ) != fNicknameMap.end() ) localName = fNicknameMap[ localName ]; 
  if ( fBuilderMap.find( localName ) == fBuilderMap.end() ) return NULL;
  EXOAnalysisModule* mod = fBuilderMap[ localName ]->BuildAnalysisModule( );
  fModuleMap[ mod ] = fBuilderMap[ localName ];
  return mod;
}
 
//______________________________________________________________________________
EXOInputModule* EXOAnalysisModuleFactory::FindInputModuleAndOpenFile( const std::string& file ) 
{
  // Search for a registered input module that can handle this file. This
  // allows EXOAnalysisManager to *find* a proper input module at run-time.
  // The caller OWNS this data and this should be destructed using
  // DestroyAnalysisModule.  Returns NULL if a suitable input module cannot be
  // found. 

  for ( EXOFactoryNameMap::iterator iter = fBuilderMap.begin(); 
        iter != fBuilderMap.end(); iter++ ) {

    if ( iter->second->CreatesInputModule() ) {
      EXOInputModule* retMod = 
        dynamic_cast< EXOInputModule* >(iter->second->BuildAnalysisModule());
      if ( !retMod ) continue;
      if ( !retMod->reads_from_file() ) {
        iter->second->DestroyAnalysisModule( retMod );
        continue;
      }
      try {
        retMod->SetFilename(file); // If it fails, it should do so with an exception.  Otherwise, it must have worked.
        fModuleMap[retMod] = iter->second;
        return retMod;
      }
      catch ( EXOMiscUtil::EXOBadCommand& badCommand ) {
        // OK, this module didn't work.
        iter->second->DestroyAnalysisModule( retMod );
      }
    } 
  }
  return NULL; 
}
 
//______________________________________________________________________________
void EXOAnalysisModuleFactory::DestroyAnalysisModule( EXOAnalysisModule* module ) 
{
  // Destroy analysis module.  This should be called to destroy all 
  // analysis modules created using BuildAnalysisModule() 

  if ( fModuleMap.find( module ) == fModuleMap.end() ) { 
    LogEXOMsg("EXOAnalysisModuleFactory trying to delete unknown module", EEError);
    return;
  }
  fModuleMap[ module ]->DestroyAnalysisModule( module );
  fModuleMap.erase( module );
} 

//______________________________________________________________________________
void EXOAnalysisModuleFactory::ShowRegisteredModules()
{
  // Utility function to show which classes are available for creation.   

  EXOFactoryNicknameMap::const_iterator iter;
  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "EXOAnalysisModuleFactory: Registered modules and nicknames are:" << std::endl;
  std::cout << Form("\t%-30.30s(%s)\n", "EXOInputModule", "input");
  for (iter = fNicknameMap.begin(); iter != fNicknameMap.end(); iter++ ) {
    std:: cout << "\t";
    std::cout << Form("%-30.30s(%s)\n", iter->second.c_str() , iter->first.c_str());
  }
  std::cout << "----------------------------------------------------------" << std::endl;

}

/* Following expands to code which we absolutely need to be inlined here.
   Compiler inlines don't always succeed.
*/
EXO_PLUGIN_VERSION_STRING

//______________________________________________________________________________
std::string EXOAnalysisModuleFactory::RegisterOnePlugin( const std::string& libraryName ) 
{
  // Registration of a plugin, which is slightly different. 
  // This function will likely becalled by EXOAnalsysManager
  // to try to load a plugin at run-time. 

  LogEXOMsg("Loading module from file: " + libraryName, EEDebug); 

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

  /* Try to obtain the initialization function. */
  EXOPlugin::init_exo_plugin_t*  init_mod = 
    (EXOPlugin::init_exo_plugin_t*)  dlsym( open_file, "exo_init_plugin_t" );

  if (!init_mod || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'init' in file: "   << libraryName << std::endl;
    output << "Output error: "                            << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
    return "";
  }
 
  int ret_value = init_mod( get_plugin_version_string().c_str(), SVN_REV );
  if ( ret_value < 0 ) {
    if ( ret_value == EXO_PLUGIN_SVN_ERROR ) {
      LogEXOMsg("Plugin and Module Factory svn version numbers do not match, you may encounter problems."
                "  Trying to proceed, but if you see issues please recompile your plugin with revision: " SVN_REV , EEError);
    } else {
      LogEXOMsg("Failure initializing module, abort further loading,", EEError);
      dlclose( open_file );
      return "";
    }
  }

  /* Try to obtain the generator and destruction functions. */
  EXOPlugin::create_exo_plugin_t*  create_mod = 
    (EXOPlugin::create_exo_plugin_t*)  dlsym( open_file, "exo_create_plugin_t" );

  if (!create_mod || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'create' in file: " << libraryName << std::endl;
    output << "Output error: "                            << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
    return "";
  }

  EXOPlugin::destroy_exo_plugin_t* delete_mod = 
    (EXOPlugin::destroy_exo_plugin_t*) dlsym( open_file, "exo_destroy_plugin_t" );

  if (!delete_mod || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'destroy' in file: " << libraryName << std::endl;
    output << "Output error: "                             << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
    return "";
  }

  /* Try to obtain the fini function. */
  EXOPlugin::fini_exo_plugin_t*  fini_mod = 
    (EXOPlugin::fini_exo_plugin_t*)  dlsym( open_file, "exo_fini_plugin_t" );

  if (!fini_mod || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'fini' in file: "    << libraryName << std::endl;
    output << "Output error: "                             << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
    return "";
  }

  /* Try to obtain the nickname function. */
  EXOPlugin::get_exo_plugin_nickname_t*  nickname_function = 
    (EXOPlugin::get_exo_plugin_nickname_t*)  dlsym( open_file, "exo_get_plugin_nickname_t" );

  if (!nickname_function || (error = dlerror()) != NULL ) {
    std::stringstream output;
    output << "Error accessing symbol 'get_nickname' in file: " << libraryName << std::endl;
    output << "Output error: "                                  << error << std::endl;
    LogEXOMsg(output.str(), EEError);
    dlclose( open_file );
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

  /* The constructor registers itself with the module. */
  fOpenPlugins.push_back( new EXOAnalysisPluginBuilder( name_function(),
                                nickname_function(), create_mod,
                                delete_mod, fini_mod, open_file ) );
  return std::string(name_function());
}

//______________________________________________________________________________
EXOAnalysisModuleFactory::~EXOAnalysisModuleFactory()
{
  // Explicitly calling the delete for registered plugins. 
  // This ensures that plugins are dealt with in the appropriate
  // order.  Since EXOAnalysisModuleFactory was created on the heap
  // as a global, this should occur after normal execution, i.e.
  // after all objects created from the plugins have been destroyed. 

  for ( size_t i = 0; i<fOpenPlugins.size(); i++ ) delete fOpenPlugins[i];
  fOpenPlugins.clear();
}

//______________________________________________________________________________
EXOAnalysisPluginBuilder::~EXOAnalysisPluginBuilder()
{
  fPluginFini();
  dlclose( fOpenFile );
}

//______________________________________________________________________________
void EXOVAnalysisModuleFactoryBuilder::DestroyAnalysisModule( EXOAnalysisModule* mod )
{
  // Provide the default destruction of an analysis module (just calling delete).
  delete mod;
}
