#ifndef EXODataQualityModule_hh
#define EXODataQualityModule_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include <string>

class EXOTalkToManager;
class EXOEventData;
class TFile;
class TH1F;
class TH2F;
class TProfile;

class EXODataQualityModule : public EXOAnalysisModule 
{

private :

  // These pointers will reference data to be read-in from the command line

  bool   fWriteOffline;
  bool   fWriteOnline;
  std::string fFilename;

  TFile *f;

  TH1F  *h101;

  TH1F  *h201;
  TH1F  *h202;
  TH1F  *h203;

  TH1F  *h301;
  TH1F  *h302;
  TH1F  *h303;
  TH1F  *h304;
  TH1F  *h305;
  TH1F  *h306;

  TH2F  *h307;
  TH2F  *h308;
  TH2F  *h309;
  TH2F  *h310;

  TH1F  *h401;
  TH1F  *h402;
  TH1F  *h403;
  TH1F  *h404;
  TH2F  *h405;

  TH1F  *h501;
  TH1F  *h502;
  TH1F  *h503;
  TH1F  *h504;
  TH1F  *h505;

  TProfile *h601;
  TH2F     *h602;
  TH2F     *h603;
  TProfile *h604;
  TH2F     *h605;
  TH2F     *h606;

  TProfile *h611;
  TH2F     *h612;
  TH2F     *h613;
  TProfile *h614;
  TH2F     *h615;
  TH2F     *h616;

  TProfile *h701;
  TH2F     *h702;
  TProfile *h703;
  TH2F     *h704;

  TProfile *h711;
  TH2F     *h712;
  TProfile *h713;
  TH2F     *h714;

  TH1F  *h801;
  TH1F  *h802;
  TH1F  *h803;
  TH1F  *h804;
  TH1F  *h805;
  TH1F  *h806;

protected:

public :

  EXODataQualityModule();
  ~EXODataQualityModule() {};

  int Initialize();
  EventStatus ProcessEvent(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetWriteOffline(bool offline) { fWriteOffline = offline; }
  void SetWriteOnline(bool online) { fWriteOnline = online; }
  void SetOutputFilename(std::string afile) { fFilename = afile; }

  DEFINE_EXO_ANALYSIS_MODULE( EXODataQualityModule )

};
#endif

  


