
/*  Layout is:

|***********************************************************************|
|                             fMain                                     |
| |*********************************************| |*******************| |
| |                 fLeftFrame                  | |     fRadioFrame   | |
| | |*****************************************| | |                   | |
| | |                fEcanvas                 | | |                   | |
| | ******************************************* | |                   | |
| |                                             | |                   | |
| | |*****************************************| | |                   | |
| | |              fBottomFrame               | | |                   | |
| | ******************************************* | |                   | |
| |                                             | |                   | |
| *********************************************** ********************* |
|                                                                       |
*************************************************************************
*/

#include <TQObject.h>
#include <RQ_OBJECT.h> 
#include <TGButtonGroup.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGTab.h>
#include <TH1.h>
#include <TH2.h>
#include <TEllipse.h>
#include <TText.h>
#include <TArrow.h>
#include <TBox.h>
#include <string>
#include "TApplication.h"

#define MAXMISC (10)
#define MAXHITS (100)

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;
class EXOWaveform;

class EXOEventApp : public TApplication {
  public:
    EXOEventApp(const char* appClassName, Int_t* argc, char** argv, void* options = 0, Int_t numOptions = 0) :
      TApplication(appClassName, argc, argv, options, numOptions),
      fHasBeenChecked(false) {}
    void StartIdleing();

  protected:
    bool fHasBeenChecked;
};

class EXOEventDisplayGui { 
  RQ_OBJECT("EXOEventDisplayGui") 
private: 

  TGMainFrame *fMain;
  TRootEmbeddedCanvas *fEcanvas;
  //TGTab *fTab;           // Tab Widget S.S. 01/13/2010
  TGHorizontalFrame *fBottomFrame;
  TGVerticalFrame *fLeftFrame;
  //TGHorizontalFrame *hframe;    //frame containing control buttons
  TGHorizontalFrame *evinfoframe; // frame for run number and event number for each entry
  TGHorizontalFrame *fileframe; // frame for filename
  TGVerticalFrame *fRequestFrame; // frame containing request event and entry boxes
  TGVerticalFrame *fRadioFrame; //frame containing radio buttons
  //TGPictureButton *exo_refresh;
  TGHorizontalFrame *fChannelFrame; // frame containing channel request entry box
  TGHorizontalFrame *fThreshFrame; // frame containing threshold requrest entry
  TGHorizontalFrame *fTimesliceFrame; // frame containing threshold requrest entry
  TGHorizontalFrame *fDrawAPDlabelsFrame; 
  TGHorizontalFrame *fScaleFrame;
  TGHorizontalFrame *fSplitFrame;
  TGHorizontalFrame *fNplexFrame;
  TGTextButton *exo_refresh;
  TGTextButton *exit;
  TGTextButton *channelgo;
  TGTextButton *timeslicego;
  TGTextButton *morego;
  TGTextButton *threshgo;

  TGCheckButton *greyscalebutton; 
  TGCheckButton *timezoombutton; 
  TGCheckButton *basesubbutton; 
  TGCheckButton *splitbutton; 
  TGCheckButton *badchannelbutton; 
  TGCheckButton *showhitsbutton; 
  TGVButtonGroup *plotsbg;
  TGCheckButton *logbutton; 
  TGHButtonGroup *chanfilterbg;
  TGVButtonGroup *scalebg;
  TGHButtonGroup *nplexbg;
  TGRadioButton *fplots[50];
  TGRadioButton *fchanfilter[3];
  TGRadioButton *fscale[2];
  TGRadioButton *fnplex[6];
  TGNumberEntry *fRequestChannel;
  TGNumberEntry *fRequestTimeslice;
  TGNumberEntry *fRequestThresh;
  //TGNumberEntryField *fRequestRun;
  TGNumberEntry *fRequestEvent;
  bool fRequestEventClick;
  int  fRequestEventOld;
  TGLabel *fEventInfo;
  TGTextEntry *fRequestFilename;
  TGCheckButton *superimposebutton;
  TGCheckButton *DrawAPDlabelsbutton;


  std::string filename;
  bool greyscale;
  bool logplot;
  int channel_filter;
  int plots_shown;
  bool split_plots;
  int zscale;
  int channumber;
  bool subtract_baseline;
  int threshold;
  int nplex;
  bool superimpose;
  bool DrawAPDlabels;
  int imisc;
  int itimeslice;
  int colorpalette[999];
  bool suppress_bad_channels;
  bool show_hits;
  bool time_zoom;
  bool fewer_colors;
  bool print_message;

  TH1F *hmisc[MAXMISC];

  TEllipse APDcircle[2*259];
  TText APDtext[2*259];
  TText APDflasher1;
  TText APDflasher2;
  int APDPlane_gang_table[2*259];
  TText APDlabel1;
  TText APDlabel2;
  TText APDlabel3;
  TText APDlabel4;
  TText APDlabel5;
  TText APDlabel6;
  TArrow APDaxisarrow1;
  TArrow APDaxisarrow2;

  TText APDlabel11;
  TText APDlabel12;
  TText APDlabel13;
  TText APDlabel14;
  TText APDlabel15;
  TText APDlabel16;
  TArrow APDaxisarrow11;
  TArrow APDaxisarrow12;

  double hit_halfwidth;
  double hit_halfheight;
  TBox UHit[MAXHITS];
  TBox VHit[MAXHITS];
  TBox APDHit[2*MAXHITS];

public: 

  EXOEventDisplayGui(const TGWindow *p,UInt_t w,UInt_t h, const std::string& FILENAME ); 
  virtual ~EXOEventDisplayGui(); 
  // drawing routines
  void DoDraw();
  void DrawReconstructedWireHits(TH2F *hist);
  void DrawReconstructedAPDHits(int APDplane);
  // interface callbacks
  void PushedGreyscaleButton(Bool_t value);
  void PushedTimeZoomButton(Bool_t value);
  void PushedBadChannelButton(Bool_t value);
  void PushedShowHitsButton(Bool_t value);
  void PushedChannelFilterButton(Int_t value);
  void PushedBaseSubButton(Bool_t value);
  void PushedSplitButton(Bool_t value);
  void PushedLogButton(Bool_t value);
  void PushedAvailablePlotButton(Int_t button_id_3);
  void PushedBinContentButton(Int_t button_id_4);
  void PushedNplexButton(Int_t value);
  void SetSuperimpose(Bool_t value);
  void SetDrawAPDlabels(Bool_t value);
  void cb_EventNumChanged(Long_t value);
  void cb_EventNumSet(Long_t value);

  // parsing data
  void FillSignalHistograms();
  void GetNextEvent();
  //  voiPreviousEntry();
  void RequestChannel(Int_t channum_id);
  void RequestTimeslice(Int_t value);
  void RequestEvent(int request_runnumber, int request_eventnumber);
  void EventUpdate();
  void ChannelUpdate();
  void TimesliceUpdate();
  void ThreshUpdateAndDraw();
  void ThreshUpdate();
  void FileDialog();
  void GetDataSource();
  double GetBaselineCounts( const EXOWaveform& );
  //  void SaveAsGif();
}; 

