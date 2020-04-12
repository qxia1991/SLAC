#ifndef EXOAnalysisModule_hh
#define EXOAnalysisModule_hh

#ifndef EXOAnalysisModuleFactory_hh
#include "EXOAnalysisModuleFactory.hh"
#endif
#include <string>
#include <map>

class EXOTalkToManager;
class EXOEventData;
class TClass;
class TObject;
class TObjArray;

class EXOAnalysisModule 
{
  friend class EXOAnalysisManager;
public:
  enum ModuleStatus {
    kNone = 0,
    kCreated,      // created ok
    kInit,         // init ok
    kGood,         // processing ok
    kErrorEvent,   // 
    kErrorSegment, //
    kErrorRun,     // 
    kAbort         // abort processing
  };

  enum EventStatus {
    kError  = -1, // failed
    kOk = 0,
    kDrop         // want to drop
  };

  enum {
    irev = 20 // do not remove 44fac454
  };
  static const int crev;

protected:

  std::string        fAlias;
  ModuleStatus       status;
  int                verbose;

public :

  EXOAnalysisModule();
  virtual ~EXOAnalysisModule() {}
  
  // main processing chain
  // These virtual methods are intended to be overloaded by derived classes

  virtual int Initialize();

  virtual EventStatus BeginOfRun(EXOEventData *ED);
  virtual EventStatus BeginOfRunSegment(EXOEventData *ED);
  virtual EventStatus ProcessEvent(EXOEventData *ED);
  virtual EventStatus EndOfRunSegment(EXOEventData *ED);
  virtual EventStatus EndOfRun(EXOEventData *ED);

  virtual int ShutDown();

  // 
  virtual int TalkTo(EXOTalkToManager *tm);
  
  virtual ModuleStatus GetStatus() const { return status; }
  virtual ModuleStatus ClearStatus() { return status; }
  int GetVerbose() const { return verbose; }
  void SetVerbose(int v) { verbose = v; }

  /*! By default, only one instance per class may exist because of how
      TalkTos are handled, etc.  If a module will allow multiple copies 
      of itself. the module should overload this to return true. 
      Then, for example, the class must ensure that configuration 
      commands can have unique names.  This can be done using
      the following *Alias() functions.  */
  virtual bool CanHaveMultipleClassInstances() const { return false; } 

  /*! The following provide an ability for classes to differentiate
      between different instances of the same class.  For example, 
      the TalkTo function can call these to prepend the alias name
      to a command.  Aliases are set using the 'usemoduleas' command. */
  virtual void SetAlias( const std::string& anAlias ) { fAlias = anAlias; }
  virtual std::string GetAlias() const { return fAlias; }

  /*! The following is an essential macro for each module.
      It defines the GetName() function which returns the
      class name and also automatically registers the
      class with the factory. It is also necessary to include
      the directive:

      IMPLEMENT_EXO_ANALYSIS_MODULE( ClassName )

      at the top of the cc file after include statements 
      where ClassName should be replaced with the implemented
      class name.  */
  //DEFINE_EXO_ANALYSIS_MODULE( EXOAnalysisModule )
  virtual std::string GetName() const = 0;

  void SetFiltered(bool aVal = true);
  bool IsFiltered();

  // Clear shared objects, eg. if the same program needs to do two independent sequences of processing.
  static void ClearSharedObjects() {fMapOfObjects.clear();}

  protected:
 
    static bool RegisterObject( const std::string& nameToRegister, const TObject& obj ); 
    static bool RegisterSharedObject( const std::string& nameToRegister, TObject& obj ); 
    static TObject* RetractObject( const std::string& name ); 
    static const TObject* FindObject( const std::string& name );
    static TObject* FindSharedObject( const std::string& name );
    static TObjArray FindSharedObjectsOfType( const TClass* classType );

  private:
    bool fIsFiltered;

    class ObjClass {
      public:
        ObjClass();
        ObjClass(const ObjClass& obj);
        ~ObjClass();
        ObjClass& operator=(const ObjClass& obj);
        void SaveObject(const TObject* obj);
        void SaveSharedObject(TObject* obj);
        TObject* fObj;
        bool fShared;
    };
    typedef std::map<std::string, ObjClass> ObjMap; 
    static ObjMap fMapOfObjects;
    


};

//--------------------inlines----------------------------------------

inline void EXOAnalysisModule::SetFiltered(bool aVal)
{
  // Set whether or not this module should be filtered.  EXOAnalysisManager
  // uses this value to determine if it should not continue further processing
  fIsFiltered = aVal;
}

inline bool EXOAnalysisModule::IsFiltered()
{
  // Check whether or not this module is filtered. EXOAnalysisManager
  // uses this value to determine whether or not to not continue
  // processing when a module returns failure on processing. 
  return fIsFiltered; 
}


#endif
