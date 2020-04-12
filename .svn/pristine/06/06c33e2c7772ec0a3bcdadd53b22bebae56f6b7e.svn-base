#ifndef EXOInputModule_hh
#define EXOInputModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
class EXOEventHeader;
class EXOGlitchRecord;
class EXOVetoEventHeader;
class TTree;

class EXOInputModule: public EXOAnalysisModule 
{

  private:
    TTree* fGlitchTree;   // Pointer to a tree of high voltage glitch events.
    TTree* fVetoTree;     // Pointer to a tree of veto events.
    static bool fBreakRequested;

#ifndef __CINT__
    friend void break_handler(int);
#endif

  protected:
  
    // The following functions are called by SetFilename and so should be
    // overloaded by input modules that need to open and close files.  
    virtual void OpenFile(const std::string& /*filename*/);
    virtual void CloseCurrentFile();

    void AddGlitchRecord(EXOGlitchRecord* aGlitch);
    void AddVetoRecord(EXOVetoEventHeader* aVeto);
    bool BreakWasRequested() const { return fBreakRequested; }
 
  public :
  
    EXOInputModule();
    virtual ~EXOInputModule();
  
    // An EXOInputModule must return an EXOEventData object.  This behaves like
    // an iterator and returns NULL when no new events are available. 
    virtual EXOEventData* GetNextEvent() = 0; 
  
    // Derived modules should overload this to identify whether the module needs a file as input.
    virtual bool reads_from_file() const = 0;
  
    virtual int  get_run_number();
    virtual int  get_event_number();
  
    virtual bool is_new_run_segment();
  
    virtual EXOEventData* GetEvent(unsigned int event_serialID);
    virtual EXOEventData* GetNext();
    virtual EXOEventData* GetPrevious();
    virtual EXOEventData* SearchEvent(int event_number);
    
    void SetFilename(std::string aval);
    virtual bool FileIsOpen() const {return false;}

};
#endif
