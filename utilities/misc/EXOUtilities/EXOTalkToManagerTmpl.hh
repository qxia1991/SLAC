#ifndef EXOTalkToManagerTmpl_hh 
#define EXOTalkToManagerTmpl_hh

#include "EXOUtilities/EXOMiscUtil.hh"
#include <algorithm>
#include <string>
#include <sstream>
#include <typeinfo>

namespace EXOTalkTo {
// Normally avoiding defines, but in this case, it helps to get the cumbersome
// syntax for calling a member function correct.  
// The version for void functions
#define EXOTALKTO_CALLPTR_TO_MEMBER_NOARG(ptr, member) if(ptr && member) ((ptr)->*(member))(); \
   else throw EXOMiscUtil::EXOBadCommand("Failed to call command -- a pointer was unexpectedly NULL")
// The version for functions with (one or more) arguments.
#define EXOTALKTO_CALLPTR_TO_MEMBER(ptr, member, ...) if(ptr && member) ((ptr)->*(member))(__VA_ARGS__); \
   else throw EXOMiscUtil::EXOBadCommand("Failed to call command -- a pointer was unexpectedly NULL")
// The version for functions with two (or more) arguments: optional string and type value(s)
#define EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(ptr, member, opt, ...) if(ptr && member) ((ptr)->*(member))(opt,__VA_ARGS__); \
   else throw EXOMiscUtil::EXOBadCommand("Failed to call command -- a pointer was unexpectedly NULL")

//////////////////////////////////////////////////////////////////////////
// EXOVCommand provides a virtual interface for commands 
//////////////////////////////////////////////////////////////////////////
class EXOVCommand
{
  public:
    virtual ~EXOVCommand() {}
    virtual void ProcessCommand(const std::string&) = 0;  
    virtual std::string GetCurrentValue() = 0;  
    virtual void* GetObjectPtr() { return NULL; }
};

//////////////////////////////////////////////////////////////////////////
// Template class handling almost all types (int, double, float, short, etc.) 
//////////////////////////////////////////////////////////////////////////
template <class TCallBack, typename TType> 
class EXOTCommand : public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)(TType setval);

    EXOTCommand(TCallBack* aclass, TType aDefault, TMemFn fun) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault)
    { 
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, fCurrentVal); 
    }
     
    virtual void ProcessCommand(const std::string& command)
    { 
      std::istringstream temp(command); 
      TType tempVal;
      temp >> tempVal;
      if(temp.fail()) {
        throw EXOMiscUtil::EXOBadCommand("Cannot convert " + command + " to " + typeid(TType).name());
      }
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, tempVal);
      fCurrentVal = tempVal; // Set after all calling -- if EXOBadCommand is thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      std::ostringstream os; 
      os << fCurrentVal; 
      return os.str(); 
    } 

    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    TType      fCurrentVal;
};

//////////////////////////////////////////////////////////////////////////
// Template class handling Set functions with particular
// types, strings and bools
//////////////////////////////////////////////////////////////////////////
template <class TCallBack>
class EXOTCommand<TCallBack, std::string>: public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)(std::string setval);

    EXOTCommand(TCallBack* aclass, std::string aDefault, TMemFn fun) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault)
    { 
      // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so the whole faulty module gets deleted.
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, fCurrentVal); 
    }
     
    virtual void ProcessCommand(const std::string& command)
    {
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, command);
      fCurrentVal = command; // Set after call -- if EXOBadCommand gets thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      return fCurrentVal; 
    } 

    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    std::string fCurrentVal;
};

//////////////////////////////////////////////////////////////////////////
// Template handling bool specialization
//////////////////////////////////////////////////////////////////////////
template <class TCallBack>
class EXOTCommand<TCallBack, bool>: public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)(bool setval);

    EXOTCommand(TCallBack* aclass, bool aDefault, TMemFn fun) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault)
    { 
      // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so the whole faulty module gets deleted.
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, fCurrentVal); 
    }
     
    virtual void ProcessCommand(const std::string& command)
    { 
      bool tempVal;
      std::istringstream istr(command); 
      if( !(istr >> tempVal) ) { 
        // Could not convert to bool -- but we also accept text.  Check for this.
        std::string temp = command;     
        std::transform( temp.begin(), temp.end(), temp.begin(), ::tolower );
        if (temp == "true") tempVal = true;
        else if (temp == "false") tempVal = false;
        else throw EXOMiscUtil::EXOBadCommand("Cannot convert " + command + " to bool.");
      }
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, tempVal);
      fCurrentVal = tempVal; // Set after calling; if EXOBadCommand is thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      return (fCurrentVal) ? "true" : "false"; 
    } 
    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    bool fCurrentVal;
};


//////////////////////////////////////////////////////////////////////////
// Template class handling void function call-backs, functions without
// any included parameters 
//////////////////////////////////////////////////////////////////////////
template <class TCallBack> 
class EXOTCommand<TCallBack, void> : public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)();

    EXOTCommand(TCallBack* aclass, TMemFn fun) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass) {}
     
    virtual void ProcessCommand(const std::string& /*command*/)
    { 
      EXOTALKTO_CALLPTR_TO_MEMBER_NOARG(fClassPtr, fCallBack); 
    }
    virtual std::string GetCurrentValue() { return ""; } 
    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
};

//////////////////////////////////////////////////////////////////////////
// Template class for optional function handling almost all
// types (int, double, float, short, etc.) 
//////////////////////////////////////////////////////////////////////////
template <class TCallBack, typename TType> 
class EXOTOptCommand : public EXOVCommand
{
public:
  typedef void (TCallBack::*TMemFn)(std::string option, TType setval);

    EXOTOptCommand(TCallBack* aclass, TType aDefault, TMemFn fun, const char* funOpt) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault),
      fCallBackOption(funOpt)
    { 
      // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so the whole faulty module gets deleted.
      EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(fClassPtr, fCallBack, fCallBackOption, fCurrentVal);
    }
     
    virtual void ProcessCommand(const std::string& command)
    { 
      std::istringstream temp(command); 
      TType tempVal;
      temp >> tempVal;
      if(temp.fail()) {
        throw EXOMiscUtil::EXOBadCommand("Cannot convert " + command + " to " + typeid(TType).name());
      }
      EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(fClassPtr, fCallBack, fCallBackOption, tempVal);
      fCurrentVal = tempVal; // Set after all calling -- if EXOBadCommand is thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      std::ostringstream os; 
      os << fCurrentVal; 
      return os.str(); 
    }
    virtual std::string GetFunctionOption() { return fCallBackOption;}

    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    TType      fCurrentVal;
    std::string fCallBackOption;
};

//////////////////////////////////////////////////////////////////////////
// Template class handling set optional functions with particular
// types, strings and bools
//////////////////////////////////////////////////////////////////////////
template <class TCallBack>
class EXOTOptCommand<TCallBack, std::string>: public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)(std::string option, std::string setval);

    EXOTOptCommand(TCallBack* aclass, std::string aDefault, TMemFn fun, const char* funOpt) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault),
      fCallBackOption(funOpt)
    { 
      // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so the whole faulty module gets deleted.
      EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(fClassPtr, fCallBack, fCallBackOption, fCurrentVal);
    }
     
    virtual void ProcessCommand(const std::string& command)
    {
      EXOTALKTO_CALLPTR_TO_MEMBER(fClassPtr, fCallBack, fCallBackOption, command);
      fCurrentVal = command; // Set after call -- if EXOBadCommand gets thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      return fCurrentVal; 
    } 
    virtual std::string GetFunctionOption() { return fCallBackOption;}

    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    std::string fCurrentVal;
    std::string fCallBackOption;
};

//////////////////////////////////////////////////////////////////////////
// Template handling bool specialization
//////////////////////////////////////////////////////////////////////////
template <class TCallBack>
class EXOTOptCommand<TCallBack, bool>: public EXOVCommand
{
  public:
    typedef void (TCallBack::*TMemFn)(std::string option, bool setval);

    EXOTOptCommand(TCallBack* aclass, bool aDefault, TMemFn fun, const char* funOpt) : 
      EXOVCommand(), 
      fCallBack(fun), 
      fClassPtr(aclass),
      fCurrentVal(aDefault),
      fCallBackOption(funOpt)
    { 
      // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so the whole faulty module gets deleted.
      EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(fClassPtr, fCallBack, fCallBackOption, fCurrentVal); 
    }
     
    virtual void ProcessCommand(const std::string& command)
    { 
      bool tempVal;
      std::istringstream istr(command); 
      if( !(istr >> tempVal) ) { 
        // Could not convert to bool -- but we also accept text.  Check for this.
        std::string temp = command;     
        std::transform( temp.begin(), temp.end(), temp.begin(), ::tolower );
        if (temp == "true") tempVal = true;
        else if (temp == "false") tempVal = false;
        else throw EXOMiscUtil::EXOBadCommand("Cannot convert " + command + " to bool.");
      }
        EXOTALKTO_CALLPTR_TO_MEMBER_WITH_OPTION(fClassPtr, fCallBack, fCallBackOption, tempVal);
      fCurrentVal = tempVal; // Set after calling; if EXOBadCommand is thrown, generally don't want fCurrentVal touched.
    }
    virtual std::string GetCurrentValue() 
    { 
      return (fCurrentVal) ? "true" : "false"; 
    } 
    virtual std::string GetFunctionOption() { return fCallBackOption;}
    virtual void* GetObjectPtr() { return fClassPtr; }

  protected:
    TMemFn fCallBack;
    TCallBack* fClassPtr;
    bool fCurrentVal;
    std::string fCallBackOption;
};
  
}

#endif /* EXOTalkToManagerTmpl_hh */
