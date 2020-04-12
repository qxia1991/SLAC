#ifndef EXOPluginUtilities_hh
#define EXOPluginUtilities_hh

#include <sstream>
#include <iostream>
#include <string>
/* Header file for utilities related to importing
   and dealing with module plugins.  
   M. Marino Nov 2010
  
*/
class EXOAnalysisModule;
namespace EXOPlugin {

  /**
    Type definitions for EXOAnalysisModule Plugins.
   */
  typedef int                init_exo_plugin_t(const char*, const char*);
  typedef EXOAnalysisModule* create_exo_plugin_t();
  typedef void               destroy_exo_plugin_t(EXOAnalysisModule*);
  typedef void               fini_exo_plugin_t();
  typedef void*              dlsym_exo_file_handle; 
  typedef const char*        get_exo_plugin_nickname_t();
  typedef const char*        get_exo_plugin_name_t();

}

/* The following defines an overall plugin version
   which must be modified after significant changes
   to the EXOAnalysis system.  This is of course a 
   manual control but can be used to force users to
   recompile their plugins with the newer version. 
*/
#define EXO_PLUGIN_VERSION 0x3

/* The following define possible errors that a plugin
   init or fini function can return.  As more functionality
   becomes desired, and more error checking possible, this
   list can grow.  
 */
#define EXO_PLUGIN_VERSION_ERROR -1
#define EXO_PLUGIN_SVN_ERROR     -2
#define EXO_PLUGIN_MISC_ERROR    -256

/* By putting the following function in a defines, you insure that it is
   exanded in the scope of the function at run-time.  This is essential,
   because if it were in a namespace, it is indeterminate which version would be
   called when a plugin is loaded.  The visibility and always inline attribute is
   to ensure that this symbol doesn't get exported which would cause interesting
   behaviour at load.
*/
#define EXO_PLUGIN_VERSION_STRING                                   \
inline std::string get_plugin_version_string()                      \
__attribute__((visibility("hidden"),always_inline,used));           \
std::string get_plugin_version_string()                             \
{                                                                   \
    std::ostringstream output;                                      \
    output.width(50);                                               \
    output << std::left << "EXOPlugin_InterfaceVersion: "           \
           << EXO_PLUGIN_VERSION       << std::endl;                \
    output.width(50);                                               \
    output << "EXOModule_InterfaceRevision: "                       \
           << EXOAnalysisModule::irev  << std::endl;                \
    output.width(50);                                               \
    return output.str();                                            \
}           


/* The following is an essential macro for all classes which 
   can be loaded as a plugin. The loaded functions are:

   exo_init_plugin_t
   exo_create_plugin_t
   exo_destroy_plugin_t
   exo_fini_plugin_t

   The init function is called to do any pre-initialization.  If
   it fails, it should return one of the negative error codes  
   defined above.  Failure will stop further load of the plugin. 

   The create function returns a pointer to an EXOAnalysisModule
   object.
   The destroy function deletes an EXOAnalysisModule object at
   a particular address.  

   The fini function is called to do any post-plugin cleanup.
   It will be called before the plugin file is closed, but 
   after objects are deleted.

   It is possible to define custom functions to be called 
   in addition at the init and fini calls. These functions
   must have the correct call footprint, in particular:

   int     my_custom_init( const char * version_str, const char * svn_rev);
   void    my_custom_fini( ); 
   
   Then your code would have the following instead:

   EXO_DEFINE_PLUGIN_MODULE_CUSTOM(ClassName, "nickname", my_custom_init, my_custom_fini) 

   If only one of the two is needed, please use one of the two following macros:
   
   EXO_DEFINE_PLUGIN_MODULE_CUSTOM_INIT(ClassName, "nickname", my_custom_init) 
   EXO_DEFINE_PLUGIN_MODULE_CUSTOM_FINI(ClassName, "nickname", my_custom_fini) 
*/



    
#define EXO_DEFINE_PLUGIN_MODULE_CUSTOM(MODULE,                         \
        MODSHORTNAME, CUSTOM_INIT_FUNCTION, CUSTOM_FINI_FUNCTION)       \
EXO_PLUGIN_VERSION_STRING                                               \
extern "C" inline int exo_init_plugin_t( const char* version_str,       \
                                         const char* svn_revision)      \
__attribute__((always_inline,used));                                    \
extern "C" inline int exo_init_plugin_t( const char* version_str,       \
                                         const char* svn_revision)      \
{                                                                       \
    std::string check_string( version_str );                            \
    if ( check_string != get_plugin_version_string() ) {                \
        std::cerr << "Version strings of plugin"                        \
                  << " and binary do not match"  << std::endl;          \
        std::cerr << "Plugin version string: "   << std::endl           \
                  << get_plugin_version_string() << std::endl;          \
        std::cerr << "Executable version string:"<< std::endl           \
                  << check_string                << std::endl;          \
        std::cerr << "Aborting load."            << std::endl;          \
        std::cerr << "This indicates the executable and plugin "        \
                  << "were compiled against different versions "        \
                  << "of EXOAnalysis."            << std::endl;         \
        return EXO_PLUGIN_VERSION_ERROR;                                \
    }                                                                   \
    EXOPlugin::init_exo_plugin_t* custom_func =                         \
      CUSTOM_INIT_FUNCTION;                                             \
    if ( custom_func ) {                                                \
      int ret_value =  custom_func( version_str, svn_revision );        \
      if (ret_value != 0) return ret_value;                             \
    }                                                                   \
    std::string svn_check( SVN_REV );                                   \
    if ( svn_check != svn_revision ) return EXO_PLUGIN_SVN_ERROR;       \
    return 0;                                                           \
}                                                                       \
                                                                        \
extern "C" inline EXOAnalysisModule*                                    \
  exo_create_plugin_t()                                                 \
__attribute__((always_inline,used));                                    \
extern "C" inline EXOAnalysisModule*                                    \
  exo_create_plugin_t()                                                 \
{                                                                       \
    return new MODULE();                                                \
}                                                                       \
                                                                        \
extern "C" inline void exo_destroy_plugin_t(EXOAnalysisModule* amod)    \
__attribute__((always_inline,used));                                    \
extern "C" inline void exo_destroy_plugin_t(EXOAnalysisModule* amod)    \
{                                                                       \
    delete amod;                                                        \
}                                                                       \
                                                                        \
extern "C" inline void exo_fini_plugin_t()                              \
__attribute__((always_inline,used));                                    \
extern "C" inline void exo_fini_plugin_t()                              \
{                                                                       \
    EXOPlugin::fini_exo_plugin_t* custom_func =                         \
      CUSTOM_FINI_FUNCTION;                                             \
    if ( custom_func ) custom_func( );                                  \
}                                                                       \
                                                                        \
extern "C" inline const char* exo_get_plugin_nickname_t()               \
__attribute__((always_inline,used));                                    \
extern "C" inline const char* exo_get_plugin_nickname_t()               \
{                                                                       \
    return MODSHORTNAME;                                                \
}                                                                       \
                                                                        \
extern "C" inline const char* exo_get_plugin_name_t()                   \
__attribute__((always_inline,used));                                    \
extern "C" inline const char* exo_get_plugin_name_t()                   \
{                                                                       \
    return #MODULE;                                                     \
}                                                                       
                                                                        
                                                                        
#define EXO_DEFINE_PLUGIN_MODULE_CUSTOM_INIT(MODULE, MODSHORTNAME,      \
    INIT_FUNCTION)                                                      \
    EXO_DEFINE_PLUGIN_MODULE_CUSTOM(MODULE, MODSHORTNAME,               \
    INIT_FUNCTION, 0)                                                   
                                                                        
#define EXO_DEFINE_PLUGIN_MODULE_CUSTOM_FINI(MODULE, MODSHORTNAME,      \
    FINI_FUNCTION)                                                      \
    EXO_DEFINE_PLUGIN_MODULE_CUSTOM(MODULE, MODSHORTNAME,               \
    0, FINI_FUNCTION)                                                   
                                                                        
#define EXO_DEFINE_PLUGIN_MODULE(MODULE, MODSHORTNAME)                  \
    EXO_DEFINE_PLUGIN_MODULE_CUSTOM(MODULE, MODSHORTNAME, 0, 0)


#endif /* EXOPluginUtilities_hh */ 
