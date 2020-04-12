#ifndef EXODelegates_hh
#define EXODelegates_hh
#ifndef __CINT__
#if __cplusplus >= 201103L
#include <functional>
namespace std_new = std;
#else
#include <tr1/functional>
namespace std_new = std::tr1;
#endif
#endif

#define EXO_DELEGATED_FUNCTION(className, funcName)            \
className::_ ## funcName

#ifndef __CINT__
#define EXO_DEFINE_DELEGATED_TMPL(className, retVal, funcName) \
    typedef EXODelegate::Delegate<className, retVal> funcName ## Type; \
    static funcName ## Type _ ## funcName;                     
#else
#define EXO_DEFINE_DELEGATED_TMPL(className, retVal, funcName) 
#endif

#define EXO_DEFINE_DELEGATED_FUNCTION(className, retVal, funcName) \
  public:                                                      \
    retVal funcName() const;                                   \
  EXO_DEFINE_DELEGATED_TMPL(className, retVal, funcName)

#define EXO_IMPLEMENT_DELEGATED_FUNCTION(className, funcName)  \
className::funcName ## Type                                    \
  EXO_DELEGATED_FUNCTION(className, funcName);                 \
                                                               \
className::funcName ## Type::retType className::funcName() const\
    {                                                          \
      return _ ## funcName(*this);                             \
    }                                                          \
                                                               
#define EXO_BIND_STATIC_FUNCTION(fn)                           \
std_new::bind(&fn, std_new::placeholders::_1)                
                                                               
#define EXO_BIND_MEMBER_FUNCTION(fn, obj)                      \
std_new::bind(&fn, obj, std_new::placeholders::_1)

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXODelegate:                                                         //
//   Namespace to deal with delegates.                                  //
//                                                                      //
// Outline on how to use delegates:                                     //
//                                                                      //
//   With delegates, there is always a class which allows particular    //
// functions to be delegated (delegator), and class/functions which     //
// register themselves to be called.  As an example:                    //
//                                                                      //
//   class EXOEventData {                                               //
//    ...                                                               //
//    EXO_DEFINE_DELEGATED_FUNCTION(EXOEventData, bool, IsVetoed)       //
//                                                                      //
//  defines a delegating function called IsVetoed that returns a bool.  //
//  This means when the following code is called:                       //
//                                                                      //
//    EXOEventData obj;                                                 //
//    obj.IsVetoed();                                                   //
//                                                                      //
//  obj will "delegate" the function called to whatever delegate has    //
//  registered itself to be called when EXOEventData::IsVetoed is       //
//  called.  If nothing has been registered, this will return a         //
//  default value (e.g. bool() ).                                       //
//                                                                      //
//  There are a few options to register a function to be called.  The   //
//  function may be a non-static class member function, a static member //
//  function, or a normal function.  It must have the following         //
//  signature:                                                          //
//                                                                      //
//    return_type function( const class_type& obj );                    //
//                                                                      //
//  where return_type, class_type are bool, EXOEventData in the above   //
//  example.  To give an example, we may have the following functions:  //
//                                                                      //
//    class DelClass {                                                  //
//                                                                      //
//      public:                                                         //
//        static bool first(const EXOEventData&);                       //
//        bool second(const EXOEventData&);                             //
//    };                                                                //
//    bool third(const EXOEventData&);                                  //
//                                                                      //
//  To register these functions to be called, we would need to do the   //
//  following:                                                          //
//                                                                      //
//  // Registers first to be called whenever EXOEventData::IsVetoed     //
//  // is called                                                        //
//  EXODelegate::RegisterDelegate del(                                  //
//                   EXO_DELEGATED_FUNCTION(EXOEventData, IsVetoed),    //
//                   EXO_BIND_STATIC_FUNCTION(DelClass::first) );       //
//                                                                      //
//  // Registers third to be called whenever EXOEventData::IsVetoed     //
//  // is called                                                        //
//  EXODelegate::RegisterDelegate del(                                  //
//                   EXO_DELEGATED_FUNCTION(EXOEventData, IsVetoed),    //
//                   EXO_BIND_STATIC_FUNCTION(third) );                 //
//                                                                      //
//  // Registers second to be called whenever EXOEventData::IsVetoed    //
//  // is called                                                        //
//  DelClass delObj;                                                    //
//  EXODelegate::RegisterDelegate del(                                  //
//               EXO_DELEGATED_FUNCTION(EXOEventData, IsVetoed),        //
//               EXO_BIND_MEMBER_FUNCTION(DelClass::second, &delObj) ); //
//                                                                      //
//  An important thing to note is that the delegation functions remain  //
//  registered until either the EXODelegate::RegisterDelegate variable  //
//  goes out of scope (or is deleted when it is created on the heap     //
//  via new) or if another EXODelegate::RegisterDelegate registers a    //
//  another function.                                                   //
//                                                                      //
//  At the moment, one can only register delegates to work for all      //
//  class instances of a particular class and not only for a particular //
//  instance.  Also, all registration can only happen at the moment in  // 
//  compiled code and not in scripts.                                   //
//                                                                      //
//  EXOCoincidences gives an example of this (see, e.g.                 //
//    EXOCoincidences::RegisterDelegation() and                         //
//    EXOCoincidences::DeregisterDelegation() and                       //
//                                                                      //
//  Adding a delegation function to a class.                            //
//                                                                      //
//  In the header:                                                      //
//                                                                      //
//   class EXOEventData {                                               //
//    ...                                                               //
//    EXO_DEFINE_DELEGATED_FUNCTION(EXOEventData, bool, IsVetoed)       //
//                                                                      //
//  In the source file:                                                 //
//                                                                      //
//    EXO_IMPLEMENT_DELEGATED_FUNCTION(EXOEventData, IsVetoed)          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
                                                                        
namespace EXODelegate                                                   
{

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VRegisterDelegate:                                                   //
// VRegisterDelegate provides a base class for the registration of a    //
// delegate.  Users will generally not use this directly.  For          //
// documentation on how to use delgates, see: EXODelegate.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
class VRegisterDelegate 
{
  // Base class for the registration of a delegate
  public:
    friend class VDelegate;
    VRegisterDelegate() : _m_should_deregister(false) {}

  protected:
    bool is_referenced() { return _m_should_deregister; }

  private:
    bool _m_should_deregister;
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VDelegate:                                                           //
// VDelegate provides a base class for a delegate.  Users will          //
// generally not use this directly.  For documentation on how to use    //
// delgates, see: EXODelegate.                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
class VDelegate 
{
  // Base class for a delegate
  public:
    friend class RegisterDelegate;
    VDelegate() : _m_current_delegate(NULL) {}
    virtual ~VDelegate() {} 

  protected:
    virtual void Deregister() = 0;

    void ResetReference(VRegisterDelegate* del = NULL)
    { 
      if (_m_current_delegate) { _m_current_delegate->_m_should_deregister = false; }
      _m_current_delegate = del;
      if (_m_current_delegate) { _m_current_delegate->_m_should_deregister = true; } 
    }

  private:
    VRegisterDelegate *_m_current_delegate;
   
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Delegate:                                                            //
// Delegate provides a template class for a delegate.  Users will       //
// generally not use this directly.  For documentation on how to use    //
// delgates, see: EXODelegate.                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
template<class _regClass, typename _retVal>
class Delegate : public VDelegate
{
  // Templatized Delegate
  public:
    typedef std_new::function< _retVal( const _regClass & ) > funcType; 
    typedef _retVal retType; 

    void Register( VRegisterDelegate* del, funcType f)
      { ResetReference(del); _m_function = f; } 

    _retVal operator()(const _regClass & obj) 
    {
      if (_m_function) return _m_function(obj);
      return _retVal();
    }

  protected:
    void Deregister()
      { ResetReference(); _m_function = funcType(); }

  private:
    funcType _m_function;
};

//////////////////////////////////////////////////////////////////////////
// RegisterDelegate:                                                    //
//   registers delegate functions with classes that have enabled them.  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class RegisterDelegate : public VRegisterDelegate 
{
  public:
    template< class _delType, class _T >
    RegisterDelegate( 
                _delType& delFn,  
                _T callBackFunc ) : 
      _m_delegated_fn(delFn)
    {
      typename _delType::funcType newFn = callBackFunc;
      delFn.Register(this, newFn); 
    }

    ~RegisterDelegate() 
    {
      if (is_referenced()) _m_delegated_fn.Deregister();
    }

  private:
    RegisterDelegate();
    VDelegate& _m_delegated_fn;
};

}

#endif /* EXODelegates_hh */
