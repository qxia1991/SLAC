/* EXOAnalysisModuleFactory.hh, Created M. Marino Nov 2010*/
#ifndef EXOAnalysisModuleFactory_hh
#define EXOAnalysisModuleFactory_hh
#include <map>
#include <string>
#include <vector>

/* The following enables TR1 functionality which    
   is not yet *officially* in the C++ standard,      
   However, it has been included in gcc builds > 4  
   and including it saves *a lot* of coding, and    
   in particular allows us to automatically         
   distinguish between EXOAnalysisModules and       
   EXOInputModules.                                */
#ifndef __CINT__
#if HAVE_TYPE_TRAITS
#include <tr1/type_traits>
#else

// Define our own

namespace std {
namespace tr1 {
typedef char (&yes)[1];
typedef char (&no)[2];

template <typename B, typename D>
struct Host
{
  operator B*() const;
  operator D*();
};

template <typename B, typename D>
struct is_base_of
{
  template <typename T>
  static yes check(D*, T);
  static no check(B*, int);

  static const bool value = sizeof(check(Host<B,D>(), int())) == sizeof(yes);
};

}
}
#endif
#endif

class EXOVAnalysisModuleFactoryBuilder;
class EXOAnalysisPluginBuilder;
class EXOAnalysisModule;
class EXOInputModule;

class EXOAnalysisModuleFactory 
{

  public :
  
    static EXOAnalysisModuleFactory& GetInstance();

    void Register( EXOVAnalysisModuleFactoryBuilder* builder,
                   const std::string& name,
                   const std::string& nickname ); 

    void Unregister( EXOVAnalysisModuleFactoryBuilder* builder ); 

    std::string RegisterOnePlugin( const std::string& libraryName ); 


    // The following two functions provide the most basic
    // access and functionality of the Factory class:
    // building and destroying modules.    

    EXOAnalysisModule* BuildAnalysisModule( const std::string& nameOrNickname ); 

    void DestroyAnalysisModule( EXOAnalysisModule* mod ); 

    void ShowRegisteredModules(); 

    EXOInputModule* FindInputModuleAndOpenFile( const std::string& afile ); 

  private:
    /* Singleton: limit any instantiation */
    EXOAnalysisModuleFactory()                                           {}
    EXOAnalysisModuleFactory(const EXOAnalysisModuleFactory&)            {}
    EXOAnalysisModuleFactory& operator=(const EXOAnalysisModuleFactory&);
    ~EXOAnalysisModuleFactory();

    /*! Typedef our container maps. */
    typedef std::map< std::string, EXOVAnalysisModuleFactoryBuilder* > EXOFactoryNameMap; 
    typedef std::map< std::string, std::string >                       EXOFactoryNicknameMap; 
    typedef std::map< EXOAnalysisModule*, EXOVAnalysisModuleFactoryBuilder* > EXOFactoryModuleMap; 

    /*! BuilderMap holds a map to the build classes. */
    EXOFactoryNameMap     fBuilderMap;
    /*! NicknameMap holds a map to the nicknames. */
    EXOFactoryNicknameMap fNicknameMap;
    /*! ModuleMap holds a map to the created modules 
        so that a proper delete may be called. */
    EXOFactoryModuleMap   fModuleMap;

    /*! All open plugins build classes are maintained to 
        ensure that we properly close the plugins at destruction. */
    std::vector< EXOAnalysisPluginBuilder* > fOpenPlugins;
    
};


class EXOVAnalysisModuleFactoryBuilder 
{
/*! Utilitiy class.  This provides an interface for building
    and destroying AnalysisModule classes.  The three,
    pure virtual function provide the necessary building 
    functionality.  The constructor and destructor perform
    automatic registration with the factory. */
  public:
    EXOVAnalysisModuleFactoryBuilder( const std::string& name, 
                                      const std::string& nickname ) 
      { EXOAnalysisModuleFactory::GetInstance().Register( this, name, nickname ); }

    virtual ~EXOVAnalysisModuleFactoryBuilder() 
      { EXOAnalysisModuleFactory::GetInstance().Unregister( this ); } 
    
    virtual EXOAnalysisModule* BuildAnalysisModule( ) = 0; 
    virtual bool CreatesInputModule()                 = 0;
    virtual void DestroyAnalysisModule( EXOAnalysisModule* mod ); 
};


#ifndef __CINT__ /* rootcint can't handle tr1 functionality. */
template <class T, bool B = std::tr1::is_base_of< EXOInputModule, T >::value > 
class EXOAnalysisModuleBuilder : public EXOVAnalysisModuleFactoryBuilder 
{
/*! Template implementation of builder classes for compiled in modules. 
    We handle classes which are an input module specially, which is the
    bool B term above.  Essentially, if an EXOInputModule is used in this
    template class, it will properly broadcast that it creates input modules. */ 

  public:
    EXOAnalysisModuleBuilder( const std::string& name, 
                              const std::string& nickname ) : 
      EXOVAnalysisModuleFactoryBuilder( name, nickname ) {} 
    virtual T* BuildAnalysisModule( ) { return new T( ); } 
    virtual bool CreatesInputModule() { return B; }
};
#endif

/* Macros to be included in the definition and implementation files of
   EXOAnalysisModules. */

/*! The following should be defined for example in th hedaer file:

    class MyClass : public EXOAnalysisModules {
    ...
      DEFINE_EXO_ANALYSIS_MODULE( MyClass )
    };
*/
#define DEFINE_EXO_ANALYSIS_MODULE(ClassName)                                                 \
  public:                                                                                     \
    virtual std::string GetName() const { return #ClassName; }                                

/*! The following provides the *magic* automatic registration and must
    be called at the top of implementation (.cc) files.  This essentially works
    by exporting a global variable which is constructed automatically at run-time.  
    The constructor of EXOVAnalysisModuleFactoryBuilder automatically registers
    itself, which is what we want. */
#define IMPLEMENT_EXO_ANALYSIS_MODULE(ClassName, nickname)                                    \
EXOAnalysisModuleBuilder< ClassName > gEXOAnalysisBuilder_##ClassName( #ClassName, nickname );


#endif /* EXOAnalysisModuleFactory_hh */

