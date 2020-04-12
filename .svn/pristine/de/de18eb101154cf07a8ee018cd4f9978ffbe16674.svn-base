//______________________________________________________________________________
//  EXOAnalysisModule
// 
//  Base class for all analysis modules, providing an processing interface,
//  executed in the following order:
//
//  Initialize(): called to initialize the module
//  BeginOfRun(): called at beginning of the run 
//  BeginOfRunSegment(): called at beginning of the run segment 
//  ProcessEvent(): called to process an event 
//  EndOfRunSegment(): called at the end of run segment 
//  EndOfRun(): called at the end of run 
//  ShutDown(): called at the end of processing loop
//
//  As well, the TalkTo function is called once before any processing occurs
//  to register any TalkTo commands that a module would like to register.
//
//  EXOAnalysisModules can share TObjects between themselves in particular by
//  register objects using the following functions:
//
//    RegisterObject : Register/Store a copy of an TObject
//    RegisterSharedObject : Register a shared TObject
//  
//  Objects registered in this way may be found using 
//
//    FindObject 
//    FindSharedObject
//
//  Objects can be retracted using RetractObject.  For more details, please see
//  the documentation of those functions.  
//  ====================================================================================== 

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TObject.h"
#include "TObjArray.h"

/*_______________________________________
  Definition of nested class ObjClass
  fObj holds the pointer to a TObject class.
  fShared determines whether or not the object is can be shared (passed out as
non-const)
*/

//______________________________________________________________________________
EXOAnalysisModule::ObjClass::ObjClass()
: fObj(NULL), fShared(false)
{}

//______________________________________________________________________________
EXOAnalysisModule::ObjClass::ObjClass(const ObjClass& obj)
: fObj(NULL), fShared(obj.fShared)
{
  // Copy constructor
  if (not obj.fShared) SaveObject(obj.fObj);
  else SaveSharedObject(obj.fObj);
}

//______________________________________________________________________________
EXOAnalysisModule::ObjClass::~ObjClass()
{
  // Destructor: Do not delete if we don't own the data.
  if (not fShared) delete fObj;
}

//______________________________________________________________________________
EXOAnalysisModule::ObjClass& EXOAnalysisModule::ObjClass::operator=(const ObjClass& obj)
{
  // Copy operator
  if (not obj.fShared) SaveObject(obj.fObj);
  else SaveSharedObject(obj.fObj);
  return *this;
} 

//______________________________________________________________________________
void EXOAnalysisModule::ObjClass::SaveObject(const TObject* obj)
{
  // Save an object which we will take a copy of.

  // Delete if we own the current object.
  if (not fShared) delete fObj;
  if (obj) fObj = obj->Clone();
  fShared = false;
}
//______________________________________________________________________________
void EXOAnalysisModule::ObjClass::SaveSharedObject(TObject* obj)
{
  // Save an object which we will only copy the pointer of.

  // Delete if we own the current object.
  if (not fShared) delete fObj;
  fObj = obj; 
  fShared = true;
}

/*_______________________________________
  Definition of EXOAnalysisModule
*/

//______________________________________________________________________________
const int EXOAnalysisModule::crev = EXOAnalysisModule::irev;
EXOAnalysisModule::ObjMap EXOAnalysisModule::fMapOfObjects;
//______________________________________________________________________________
EXOAnalysisModule::EXOAnalysisModule() : fIsFiltered(false)
{
  status = kCreated;
}

//______________________________________________________________________________
int EXOAnalysisModule::Initialize()
{
  if (status==kCreated) status = kInit;
  return 0;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOAnalysisModule::BeginOfRun(EXOEventData *ED)
{
  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOAnalysisModule::BeginOfRunSegment(EXOEventData *ED)
{
  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOAnalysisModule::ProcessEvent(EXOEventData *ED)
{
  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOAnalysisModule::EndOfRunSegment(EXOEventData *ED)
{
  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOAnalysisModule::EndOfRun(EXOEventData *ED)
{
  return kOk;
}

//______________________________________________________________________________
int EXOAnalysisModule::ShutDown()
{
  if (status!=kAbort) status = kCreated;
  return 0;
}

//______________________________________________________________________________
int EXOAnalysisModule::TalkTo(EXOTalkToManager *talkTo)
{
  return 0;
}

//______________________________________________________________________________
bool EXOAnalysisModule::RegisterObject( const std::string& aname, const TObject& obj )
{
  // Register an object that could be used by other EXOAnalysisModules via the
  // FindObject function.  This function returns true if the object could be
  // added, false if not.  This function copies the obj in its current state
  // and so provides a very safe mechanism to share information between
  // EXOAnalysisModules.   

  if ( fMapOfObjects.find( aname ) != fMapOfObjects.end() ) {
    LogEXOMsg(aname + " already exists as registered object", EEWarning);
    return false;
  }
  ObjClass& temp = fMapOfObjects[ aname ];
  temp.SaveObject( &obj );
  return true;
}

//______________________________________________________________________________
bool EXOAnalysisModule::RegisterSharedObject( const std::string& aname, TObject& obj )
{
  // Register an object that could be used by other EXOAnalysisModules via the
  // FindSharedObject function.  This function returns true if the object could
  // be added, false if not.  This only retains the pointer meaning and does
  // *NOT* own the data.  The memory location to the object will *NOT* be
  // deleted by this object. 
  //
  // *NOTE*: Anyone calling this function must ensure that RetractObject is
  // called whenever the object is no longer available.  The caller of this
  // function *RETAINS* ownership over this object.  If static objects are to
  // be shared, use instead RegisterObject which saves a copy of the TObject.

  if ( fMapOfObjects.find( aname ) != fMapOfObjects.end() ) {
    LogEXOMsg(aname + " already exists as registered object", EEWarning);
    return false;
  }
  ObjClass& temp = fMapOfObjects[ aname ];
  temp.SaveSharedObject( &obj );
  return true;
}
//______________________________________________________________________________
TObject* EXOAnalysisModule::RetractObject( const std::string& aname )
{
  // Retract an object using the name that was previously registered.  If the
  // object was registered using RegisterSharedObject, the pointer to the
  // shared object will be returned.  If the object is not found, or was
  // registered using RegisterObject, the function will return NULL.

  ObjMap::iterator iter = fMapOfObjects.find(aname);
  if ( iter == fMapOfObjects.end() ) return NULL;
  TObject* returnObj = NULL;
  if ( iter->second.fShared) returnObj = iter->second.fObj;
  fMapOfObjects.erase( iter ); 
  return returnObj; 
}

//______________________________________________________________________________
const TObject* EXOAnalysisModule::FindObject( const std::string& aname )
{
  // Return an object that may have been previously registered.  The object
  // is const and so must be copied if it is to be edited. 

  ObjMap::iterator iter = fMapOfObjects.find(aname);
  if ( iter == fMapOfObjects.end() ) return NULL;
  if ( iter->second.fShared ) return NULL;
  return iter->second.fObj;
}

//______________________________________________________________________________
TObject* EXOAnalysisModule::FindSharedObject( const std::string& aname )
{
  // Return an object that may have been previously registered.  This returns a
  // non-const pointer and this points to shared memory.  Because of the
  // volatile nature of this, this function should be called *WHENBVER* the
  // shared object is needed, and the return must be checked if it's NULL. 

  ObjMap::iterator iter = fMapOfObjects.find(aname);
  if ( iter == fMapOfObjects.end() ) return NULL;
  if ( not iter->second.fShared ) return NULL;
  return iter->second.fObj;
}
//______________________________________________________________________________
TObjArray EXOAnalysisModule::FindSharedObjectsOfType( const TClass* klass )
{
  // Finds all shared objects of the type specified by the TClass parameter and
  // returns them in a TObjArray.  For example, to get all shared TTrees, do: 
  //
  //   TObjArray arrOfTrees = FindSharedObjectsOfType(TTree::Class());
  //
  TObjArray retList;
  ObjMap::iterator iter = fMapOfObjects.begin();
  for ( ; iter != fMapOfObjects.end(); iter++ ) {
    ObjClass& obj = iter->second;
    if ( obj.fShared and 
         obj.fObj->IsA() == klass ) {
      retList.Add(obj.fObj);
    }
  }
  return retList;
}
