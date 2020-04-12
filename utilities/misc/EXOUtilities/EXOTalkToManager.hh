#ifndef EXOTalkToManager_hh 
#define EXOTalkToManager_hh

#include <string>
#include <fstream>
#include <vector>
#include <map>
#ifndef EXOTalkToManagerTmpl_hh
#include "EXOUtilities/EXOTalkToManagerTmpl.hh"
#endif

class EXOAnalysisModule;

class EXOTalkToManager
{
  public:
    class TalkToCommand {
      public:
        TalkToCommand() : fCmd(NULL) {}
        TalkToCommand(const std::string& cmdName,
                      const std::string& help) : 
                      fCommandName(cmdName), fHelpMsg(help),
                      fCmd(NULL) {}
        std::string fCommandName;
        std::string fHelpMsg;
        EXOTalkTo::EXOVCommand* fCmd;
        const EXOAnalysisModule* fBaseModule; // We don't own this!
    };

    typedef std::map<std::string, TalkToCommand> EXOTalkMap;
    typedef std::vector<TalkToCommand> EXOTalkVec;
    typedef std::vector<std::string> StrVec;
  protected:
    EXOTalkMap     fMap;
    
    bool           fEcho;
    bool           fExit;
    bool           fEchoEXO;
    
    std::istream  *finputStream;
    std::ifstream  finputFileStream;
    
    StrVec         fProcessedCommands;
    void AddCommandToMap(const TalkToCommand& cmd);

    const EXOAnalysisModule* fCurrentBaseModule;
  
  public:

    EXOTalkToManager(const char* filename = 0);
    ~EXOTalkToManager();
    
    template <class T, typename retVal> 
    void CreateCommand(const std::string& commandName, 
                       const std::string& helpmsg,
                       T* klass, 
                       retVal defaultVal,
                       void (T::*afunc)(retVal));
    
    // The following declaration forces const char* to be interpreted
    // as std::string
    template <class T> 
    void CreateCommand(const std::string& commandName, 
                       const std::string& helpmsg,
                       T* klass, 
                       const std::string& defaultVal,
                       void (T::*afunc)(std::string));
    
    // For void commands
    template <class T> 
    void CreateCommand(const std::string& commandName, 
                       const std::string& helpmsg,
                       T* klass, 
                       void (T::*afunc)());  


  // For optional functions
    template <class T, typename retVal> 
    void CreateCommand(const std::string& commandName, 
                       const std::string& helpmsg,
                       T* klass, 
                       retVal defaultVal,
                       void (T::*afunc)(std::string, retVal),
                       const char* funcOption);
    
    template <class T> 
    void CreateCommand(const std::string& commandName, 
                       const std::string& helpmsg,
                       T* klass, 
                       const std::string& defaultVal,
                       void (T::*afunc)(std::string, std::string),
                       const char* funcOption);

  
    void show_commands();
    void InterpretCommand( const std::string& command );
    void InterpretStream();
    bool IsInteractive() const;
    
    int SetFilename(const std::string& aFileName );
    void SetInputStream(std::istream& input);
    
    void SetEchoEXO(bool aVal = true) { fEchoEXO = aVal; }
    void SetEchoCommands(bool aVal = true) { fEcho = aVal; }
    void SetExit() { fExit = true; }
    void NonOp() { /* A Generic non-operation function */ }
    void RemoveObjectCommands(void* objAddress);
    const EXOTalkVec& GetAllCommands() const;
    void SetCurrentBaseModule(const EXOAnalysisModule* mod) {fCurrentBaseModule = mod;}

    // Returns a vector of the processed commands since the call to
    // InterpretStream.
    const StrVec& GetProcessedCommands() const { return fProcessedCommands; }
};

//---------------------- template inlines --------------------------------------
template <class T, typename retVal> 
void EXOTalkToManager::CreateCommand(
                     const std::string& commandName, 
                     const std::string& helpmsg,
                     T* klass, 
                     retVal defaultVal,
                     void (T::*afunc)(retVal))
{
  TalkToCommand cmd(commandName, helpmsg);
  cmd.fBaseModule = fCurrentBaseModule;
  // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so that the whole faulty module gets deleted.
  cmd.fCmd = new EXOTalkTo::EXOTCommand<T, retVal>(klass, defaultVal, afunc);
  AddCommandToMap(cmd);
}

template <class T> 
void EXOTalkToManager::CreateCommand(
                     const std::string& commandName, 
                     const std::string& helpmsg,
                     T* klass, 
                     const std::string& defaultVal,
                     void (T::*afunc)(std::string))
{
  CreateCommand<T, std::string>(commandName,
                                helpmsg, 
                                klass, 
                                defaultVal, 
                                afunc);
}

template <class T> 
void EXOTalkToManager::CreateCommand(const std::string& commandName, 
                     const std::string& helpmsg,
                     T* klass, 
                     void (T::*afunc)())
{
  TalkToCommand cmd(commandName, helpmsg);
  cmd.fBaseModule = fCurrentBaseModule;
  // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so that the whole faulty module gets deleted.
  cmd.fCmd = new EXOTalkTo::EXOTCommand<T, void>(klass, afunc); 
  AddCommandToMap(cmd);
}

template <class T, typename retVal> 
void EXOTalkToManager::CreateCommand(
                     const std::string& commandName, 
                     const std::string& helpmsg,
                     T* klass, 
                     retVal defaultVal,
                     void (T::*afunc)(std::string, retVal),
                     const char* funcOption)
{
  TalkToCommand cmd(commandName, helpmsg);
  cmd.fBaseModule = fCurrentBaseModule;
  // Don't catch EXOBadCommand here -- let it propagate to EXOAnalysisManager, so that the whole faulty module gets deleted.
  cmd.fCmd = new EXOTalkTo::EXOTOptCommand<T, retVal>(klass, defaultVal, afunc, funcOption);
  AddCommandToMap(cmd);
}

template <class T> 
void EXOTalkToManager::CreateCommand(
                     const std::string& commandName, 
                     const std::string& helpmsg,
                     T* klass, 
                     const std::string& defaultVal,
                     void (T::*afunc)(std::string, std::string),
                     const char* funcOption)
{
  CreateCommand<T, std::string>(commandName,
                                helpmsg, 
                                klass, 
                                defaultVal, 
                                afunc,
                                funcOption);
}

#endif


