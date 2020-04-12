#ifndef EXOEventSelectionModule_hh
#define EXOEventSelectionModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
#include <vector>
#include <utility>

class EXOEventSelectionModule : public EXOAnalysisModule 
{

private :
  std::string fEventListFilename;
  std::vector<std::pair<int,int> > fEventList; // list of events in selection
    // here we suppose that fRun:fEvent pair defines event uniquely
  std::string fSelection; // line will be passed to ROOT for processing

  int nell; 
  int neul;
  int nrll;
  int nrul;
  int nscll;
  int nscul;
  int nclll;
  int nclul;
  double ecrecll; 
  double ecrecul;
  int trigsecll;
  int trigsecul;
  int trigsubll;
  int trigsubul;
  int trigdriftll;
  int trigdriftul;
  int snll;
  int snul;
  int srll;
  int srul;   
  int trigsumll; 
  int trigsumul; 
  int inll;
  int inul;
  int irll;
  int irul;
  int indsumll; 
  int indsumul;
  int trigoffll;
  int trigofful;
  int srcll;
  int srcul;
  int muon_inc;

  int LoadEventList(const std::string &filename);

public :

  EXOEventSelectionModule();
  ~EXOEventSelectionModule() {};

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);

  int TalkTo(EXOTalkToManager *talktoManager);
  void SetEventListFilename(const std::string s) { fEventListFilename = s; }
  const std::string& GetEventListFilename() const { return fEventListFilename; }
  void SetSelection(const std::string s) { fSelection = s; }
  const std::string& GetSelection() const { return fSelection; }
  EventStatus CheckEvent(const EXOEventData *ED) const;
  void Setnell(int aval) { nell = aval; } 
  void Setneul(int aval) { neul = aval; }
  void Setnrll(int aval) { nrll = aval; }
  void Setnrul(int aval) { nrul = aval; }
  void Setnscll(int aval) { nscll = aval; }
  void Setnscul(int aval) { nscul = aval; }
  void Setnclll(int aval) { nclll = aval; }
  void Setnclul(int aval) { nclul = aval; }
  void Setecrecll(double aval) { ecrecll = aval; } 
  void Setecrecul(double aval) { ecrecul = aval; }
  void Settrigsecll(int aval) { trigsecll = aval; }
  void Settrigsecul(int aval) { trigsecul = aval; }
  void Settrigsubll(int aval) { trigsubll = aval; }
  void Settrigsubul(int aval) { trigsubul = aval; }
  void Settrigdriftll(int aval) { trigdriftll = aval; }
  void Settrigdriftul(int aval) { trigdriftul = aval; }
  void Setsnll(int aval) { snll = aval; }
  void Setsnul(int aval) { snul = aval; }
  void Setsrll(int aval) { srll = aval; }
  void Setsrul(int aval) { srul = aval; }   
  void Settrigsumll(int aval) { trigsumll = aval; } 
  void Settrigsumul(int aval) { trigsumul = aval; } 
  void Setinll(int aval) { inll = aval; }
  void Setinul(int aval) { inul = aval; }
  void Setirll(int aval) { irll = aval; }
  void Setirul(int aval) { irul = aval; }
  void Setindsumll(int aval) { indsumll = aval; } 
  void Setindsumul(int aval) { indsumul = aval; }
  void Settrigoffll(int aval) { trigoffll = aval; }
  void Settrigofful(int aval) { trigofful = aval; }
  void Setsrcll(int aval) { srcll = aval; }
  void Setsrcul(int aval) { srcul = aval; }
  void Setmuon_inc(int aval) { muon_inc = aval; }



  DEFINE_EXO_ANALYSIS_MODULE( EXOEventSelectionModule )
};
#endif

  


