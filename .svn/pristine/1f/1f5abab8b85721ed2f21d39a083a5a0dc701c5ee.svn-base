#ifndef EXOTreeInputModule_hh
#define EXOTreeInputModule_hh

#include "EXOInputModule.hh"
#include <list>
#include <string>
#include "EXOUtilities/EXOControlRecordList.hh"

class TTree;
class TFile;
class EXOTalkToManager;
class EXOTreeInputModule: public EXOInputModule 
{

protected :

  TFile     *fRootFile;          // current root file 
  TTree     *fRootTree;          // root tree
  EXOEventData *fEventData;      // event data
  //Long64_t  fProcessedEvents;    // number of processed events 
  long int fCurrentEventID;      // flags current serial ID
  std::list<std::string> fFiles;
  EXOControlRecordList   fControlRecords;

  virtual void OpenFile(const std::string& aFile);
  virtual void CloseCurrentFile();
public :

  EXOTreeInputModule();
  virtual ~EXOTreeInputModule();

  EXOEventData* GetNextEvent(); 
  bool reads_from_file() const {return true;}
  bool FileIsOpen() const;

  virtual int TalkTo(EXOTalkToManager* tm);
  
  // out of order access interface
  virtual EXOEventData* GetEvent(unsigned int event_serialID); 
  virtual EXOEventData* GetNext(); 
  virtual EXOEventData* GetPrevious(); 
  virtual EXOEventData* SearchEvent(int event_number); 
 
  TTree* GetTree() { return fRootTree; }
  int  get_run_number();
  int  get_event_number();

  void AddFilesToProcess(std::string fileOrFiles);

protected:
  bool CheckNextFile();

  DEFINE_EXO_ANALYSIS_MODULE( EXOTreeInputModule )

};
#endif
