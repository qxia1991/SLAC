#include "EXOUtilities/EXO3DView.hh"

#include <TROOT.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TQObject.h>
#include <TGButton.h>
#include <TGLayout.h>
#include <TGXYLayout.h>
#include <TGWindow.h>
#include <TGFileBrowser.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TGSlider.h>
#include <TGLViewer.h>
#include <TGTab.h>
#include <TCanvas.h>
#include <TGToolBar.h>
#include <TGStatusBar.h>
#include <TPostScript.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGComboBox.h>
#include <TG3DLine.h>
#include <TGStatusBar.h>
#include <TGProgressBar.h>
#include <TSystem.h>
#include <TRootCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TText.h>
#include <TGIcon.h>
#include <TGaxis.h>
#include <TColor.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2.h>
#include <TLine.h>
#include <TVirtualX.h>
#include <TBox.h>
#include <TGLUtil.h>
#include <RQ_OBJECT.h>
#include <TGSplitFrame.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <time.h>

#include "TMath.h"

using namespace std;

#define APD_CHANNEL0 (152)
#define MAXHITS (100)

// some useful geometrical expressions
#define pi (3.141593)
#define cos30 (0.866035)
#define cos60 (0.5)
#define cos240 (-cos60)
#define sin30 (0.5)
#define sin60 (0.866025)
#define sin240 (-sin60)
#define tan30 (0.577350)

class EXO3DViewer : public TGMainFrame
{
private:
   TGPopupMenu *fMenuOptions;
   TGCheckButton *fCheckUWiresPZ;
   TGCheckButton *fCheckVWiresPZ;
   TGCheckButton *fCheckUWiresNZ;
   TGCheckButton *fCheckVWiresNZ;
   TGCheckButton *fCheckAPDPZ;
   TGCheckButton *fCheckAPDNZ;
   TGCheckButton *fCheckVessel;
   TGCheckButton *fCheckFieldRings;
   TGCheckButton *fCheckReflector;
   TGCheckButton *fCheckCathode;
   TGCheckButton *fCheckAllDetectorElements;
   TGCheckButton *fCheckCCL;
   TGCheckButton *fCheckComptCone;
   TGCheckButton *fShowSampleIndicators;
   TGCheckButton *fCheckMark3DCluster;
   TGCheckButton *fCheckMarkAllCluster;
   TGCheckButton *fCheckSetIndWire;
   TGCheckButton *fCheckSetIndAPD;
   TGNumberEntryField *fGoToEventEntry;
   TGNumberEntryField *fGoToTreeEntry;
   TGNumberEntry *fNSampleWire;
   TGNumberEntry *fNSampleAPD;
   TGComboBox *fCBChargeCluster;
   TGComboBox *fCBScintCluster;
   TGStatusBar *StatBar;
   TGLabel *SampleDiffValue;
   TGLabel *SelClusterVal[5];

   TRootEmbeddedCanvas *fCanvasWires;
   TRootEmbeddedCanvas *fCanvasAPD;
   TRootEmbeddedCanvas *fCanvasSingleChannel;
   TRootEmbeddedCanvas *fCanvas3DnonGL;
   TRootEmbeddedCanvas *fCanvasChargeInfo;
   TRootEmbeddedCanvas *fCanvasScintillationInfo;
   TCanvas *cWireChannels;
   TCanvas *cAPDChannels;
   TCanvas *cSingleChannel;
   TCanvas *c;
   TCanvas *cChargeInfo;
   TCanvas *cScintillationInfo;
   TGLViewer *v;

   EXO3DView *viewer;

   TLine *posWireSample;
   TLine *posAPDSample;

   TH2I *hWires;
   TH2I *hAPD;

   int ColorRangeMin;
   int ColorRangeMax;
   int *colorpalette;
   int *colorpaletteGS;
   
   bool FirstPlot;

   char *filename;
   int RunID;
   int EventID;
   int nrSample;
   int nrSig;
   bool HasWaveforms;

   double hit_halfwidth;
   double hit_halfheight;
   TBox UHit[MAXHITS];
   TBox VHit[MAXHITS];

public:
   EXO3DViewer(const TGWindow *p, UInt_t w, UInt_t h, char *fname);
   virtual ~EXO3DViewer();
   void Init();
   void OpenFile();
   void OpenURL();
   void GetDataSource();
   void GoToEvent(int opt, bool init);
   void FillHistograms();
   void SetCurrentData();
   void CreateChargeCluster();
   void DrawReconstructedWireHits(bool draw);
   void SetSampleInd(int posWire, int posAPD);
   void CursorPos(Int_t event, Int_t x, Int_t y, TObject *selected);
   void SetObjectInfo();
   void SaveCanvas(int canvasID);

   void CloseWindow() { gApplication->Terminate(0); }
   Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(EXO3DViewer,1)
};

class URLDialog {
  RQ_OBJECT("URLDialog");

private:
  TGTransientFrame *fMain;
  TGTextEntry *fSrv;
  TGCheckButton *fAuth;
  TGTextEntry *fUser, *fPass;
  TGComboBox *fType;
  TGNumberEntryField *fRunI, *fRunJ;
  TGTextEntry *fPath;
  TGTextButton *fOk, *fCancel;

  TString *result;

public:
  URLDialog(const TGWindow *p, const TGWindow *main, TString *res);
  ~URLDialog() { fMain->DeleteWindow(); }
  // slots
  void CloseDialog() { delete this; }
  void HandleButton();
};
