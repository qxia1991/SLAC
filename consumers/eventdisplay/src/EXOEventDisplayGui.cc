//#include <TGClient.h>
#include <TROOT.h> 
#include <TSystem.h> 
#include <TCanvas.h> 
#include <TF1.h> 
#include <TH2.h>
#include <TRandom.h>
#include <TGButton.h> 
#include <TRootEmbeddedCanvas.h> 
#include <TStyle.h>
#include <TColor.h>
#include <TPaletteAxis.h>
#include <TGFileDialog.h>
#include <TObjString.h>
#include "EXOEventDisplayGui.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOUtilities/EXODimensions.hh" 
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include <iostream>
#include <cmath>
#include <ctime> // for strftime
using namespace std;

void EXOEventApp::StartIdleing()
{
  if (fHasBeenChecked && !gXDisplay) {
    // Means an X display has gone away, quit
    Fatal("EXOEventApp::StartIdleing()", "The X11 display is gone!  Aborting..."); 
    gSystem->Abort(1);
  } else if (!fHasBeenChecked && gXDisplay) {
    fHasBeenChecked = true;
  }
  TApplication::StartIdleing();
}


// I'm declaring this stuff as global variables here, because
// the geant4 stuff screws up the generation of the dictionary file
// when it's located in the header file.

EXOEventData *eventData = 0;
EXOInputModule *inputModule = 0;

EXOEventDisplayGui::EXOEventDisplayGui(const TGWindow *p,UInt_t w,UInt_t h, const std::string& FILENAME) 
{ 
  filename = FILENAME;

  greyscale = false;
  logplot = false;
  channel_filter = 1;
  plots_shown = 1; 
  zscale = 1; 
  channumber = 1;
  subtract_baseline = true;
  split_plots = false;
  suppress_bad_channels = true;
  itimeslice = 0;
  time_zoom = false;
  fewer_colors = false;
  print_message = false;
  show_hits = false;

  superimpose = false;
  
  threshold = 0;
  nplex = 1;
  imisc = 0;

  // Color Palette

  const Int_t NRGBs = 5;
  
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  Int_t FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, 999);
  for (int i=0;i<999;i++) colorpalette[i] = FI+i;



  // APDGeometry

  float xtemp[259] = {-4.436, -2.218, -5.545, -3.327, -1.109,  1.109,  3.327, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, -17.745, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, -18.855, -16.636, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, 16.636, -17.745, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, -16.636, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, 16.636, -17.745, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, 17.745, -16.636, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, 16.636, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, 17.745, -16.636, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, 16.636, 18.855, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, 15.527, 17.745, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, 12.200, 14.418, -15.527, -13.309, -11.091, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, 11.091, 13.309, -14.418, -12.200, -9.982, -7.764, -5.545, -3.327, -1.109,  1.109,  3.327,  5.545,  7.764,  9.982, -8.873, -6.655, -4.436, -2.218,  0.000,  2.218,  4.436,  6.655,  8.873, -3.327, -1.109,  1.109,  3.327,  5.545,  2.218,  4.436 };

  float ytemp[259] = { 19.210, 19.210, 17.289, 17.289, 17.289, 17.289, 17.289, 15.368, 15.368, 15.368, 15.368, 15.368, 15.368, 15.368, 15.368, 15.368, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 13.447, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526, 11.526,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  9.605,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  7.684,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  5.763,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  3.842,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  1.921,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -1.921, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -3.842, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -5.763, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -7.684, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -9.605, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -11.526, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -13.447, -15.368, -15.368, -15.368, -15.368, -15.368, -15.368, -15.368, -15.368, -15.368, -17.289, -17.289, -17.289, -17.289, -17.289, -19.210, -19.210};

  for ( int i = 0; i < 2*259; i++ ) {
    int index;
    if ( i < 259 ) {
      index = i;
    }
    else {
      index = i - 259;
    }

    float theta = (3.1415926-0.9)/12;
    float xapd = cos(theta)*xtemp[index] + sin(theta)*ytemp[index];
    float yapd = -1.0*sin(theta)*xtemp[index] + cos(theta)*ytemp[index];
    if ( i >= 259 ) yapd *= -1.0; 
    xapd *= CLHEP::cm;
    yapd *= CLHEP::cm;

    APDcircle[i].SetX1(xapd);
    APDcircle[i].SetY1(yapd);
    APDtext[i].SetX(xapd-2.0*CLHEP::cm);
    APDtext[i].SetY(yapd-0.7*CLHEP::cm);
    APDtext[i].SetTextSize(0.06);
    APDcircle[i].SetR1(APDRADIUS);
    APDcircle[i].SetR2(APDRADIUS);
    APDcircle[i].SetLineStyle(1);
    APDcircle[i].SetLineColor(1);
    APDcircle[i].SetFillStyle(1001);
    APDcircle[i].SetFillColor(1);

  }

  int temp1[NUMBER_APDS_PER_PLANE] = {1,1,
				      1,1,1,2,2,
				      34,34,1,1,2,2,2,3,3,
				      34,34,34,4,4,2,2,3,3,3,7,7,
				      33,33,34,34,4,4,4,5,5,3,3,7,7,7,
				      33,33,33,36,36,4,4,5,5,5,10,10,7,7,
				      32,32,33,33,36,36,36,6,6,5,5,10,10,10,8,8,
				      32,32,32,35,35,36,36,6,6,6,12,12,10,10,8,8,8,
				      32,32,35,35,35,37,37,6,6,12,12,12,11,11,8,8,
				      27,27,35,35,37,37,37,31,31,12,12,11,11,11,9,9,
				      27,27,27,29,29,37,37,31,31,31,18,18,11,11,9,9,9,
				      27,27,29,29,29,30,30,31,31,18,18,18,16,16,9,9,
				      26,26,29,29,30,30,30,24,24,18,18,16,16,16,13,13,
				      26,26,26,28,28,30,30,24,24,24,17,17,16,16,13,13,13,
				      26,26,28,28,28,23,23,24,24,17,17,17,14,14,13,13,
				      25,25,28,28,23,23,23,22,22,17,17,14,14,14,
				      25,25,25,21,21,23,23,22,22,22,15,15,14,14,
				      25,25,21,21,21,20,20,22,22,15,15,15,
				      21,21,20,20,20,19,19,15,15,
				      20,20,19,19,19,
				      19,19};
  
  for ( int i = 0; i < NUMBER_APDS_PER_PLANE; i++ ) APDPlane_gang_table[i] = temp1[i];

  int temp2[NUMBER_APDS_PER_PLANE] = {20,20,
				      20,20,20,21,21,
				      16,16,20,20,21,21,21,22,22,
				      16,16,16,23,23,21,21,22,22,22,26,26,
				      15,15,16,16,23,23,23,24,24,22,22,26,26,26,
				      15,15,15,18,18,23,23,24,24,24,29,29,26,26,
				      14,14,15,15,18,18,18,25,25,24,24,29,29,29,27,27,
				      14,14,14,17,17,18,18,25,25,25,31,31,29,29,27,27,27,
				      14,14,17,17,17,19,19,25,25,31,31,31,30,30,27,27,
				      9,9,17,17,19,19,19,13,13,31,31,30,30,30,28,28,
				      9,9,9,11,11,19,19,13,13,13,37,37,30,30,28,28,28,
				      9,9,11,11,11,12,12,13,13,37,37,37,35,35,28,28,
				      8,8,11,11,12,12,12,6,6,37,37,35,35,35,32,32,
				      8,8,8,10,10,12,12,6,6,6,36,36,35,35,32,32,32,
				      8,8,10,10,10,5,5,6,6,36,36,36,33,33,32,32,
				      7,7,10,10,5,5,5,4,4,36,36,33,33,33,
				      7,7,7,3,3,5,5,4,4,4,34,34,33,33,
				      7,7,3,3,3,2,2,4,4,34,34,34,
				      3,3,2,2,2,1,1,34,34,
				      2,2,1,1,1,
				      1,1};
  
  for ( int i = NUMBER_APDS_PER_PLANE; i < 2*NUMBER_APDS_PER_PLANE; i++ ) {
    APDPlane_gang_table[i] = temp2[i-NUMBER_APDS_PER_PLANE];
  }


  hit_halfwidth = 10.0;
  hit_halfheight = 2.0;
  for ( int j = 0; j < MAXHITS; j++ ) {

    UHit[j].SetLineStyle(1);
    UHit[j].SetLineWidth(2);
    UHit[j].SetFillStyle(0);

    VHit[j].SetLineStyle(1);
    VHit[j].SetLineWidth(2);
    VHit[j].SetFillStyle(0);

    APDHit[2*j].SetLineStyle(1);
    APDHit[2*j].SetLineWidth(2);
    APDHit[2*j].SetFillStyle(0);

    APDHit[2*j+1].SetLineStyle(1);
    APDHit[2*j+1].SetLineWidth(2);
    APDHit[2*j+1].SetFillStyle(0);

  }


  // Create a main frame

  fMain = new TGMainFrame(p,w,h, kHorizontalFrame);
  ULong_t redcolor;
  gClient->GetColorByName("red",redcolor);
  //create frame widget for page buttons and canvas
  fLeftFrame = new TGVerticalFrame(fMain,400,600);
  fBottomFrame = new TGHorizontalFrame(fLeftFrame,400,200);

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fLeftFrame, 400, 400); 

  ////////////Upper-right (radio button) frame////////////////////////////////////////////////////

  // Create Radio button subframe
  fRadioFrame = new TGVerticalFrame(fMain,150,600);

  greyscalebutton = new TGCheckButton(fRadioFrame, "Greyscale");
  greyscalebutton->SetOn(greyscale);
  greyscalebutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedGreyscaleButton(Bool_t)");
  fRadioFrame->AddFrame(greyscalebutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));


  basesubbutton = new TGCheckButton(fRadioFrame, "Subtract Baseline");
  basesubbutton->SetOn(true);
  basesubbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedBaseSubButton(Bool_t)");
  fRadioFrame->AddFrame(basesubbutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));

  //Radio Button to switch log/lin scale

  logbutton = new TGCheckButton(fRadioFrame, "Log Scale");
  logbutton->SetOn(false);
  logbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedLogButton(Bool_t)");
  fRadioFrame->AddFrame(logbutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));


  splitbutton = new TGCheckButton(fRadioFrame, "Split Subdetectors");
  splitbutton->SetOn(split_plots);
  splitbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedSplitButton(Bool_t)");
  fRadioFrame->AddFrame(splitbutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));


  badchannelbutton = new TGCheckButton(fRadioFrame, "Suppress Bad Channels");
  badchannelbutton->SetOn(suppress_bad_channels);
  badchannelbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedBadChannelButton(Bool_t)");
  fRadioFrame->AddFrame(badchannelbutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));


  showhitsbutton = new TGCheckButton(fRadioFrame, "Show Hits");
  showhitsbutton->SetOn(show_hits);
  showhitsbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedShowHitsButton(Bool_t)");
  fRadioFrame->AddFrame(showhitsbutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));

  timezoombutton = new TGCheckButton(fRadioFrame, "Time Zoom");
  timezoombutton->SetOn(time_zoom);
  timezoombutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "PushedTimeZoomButton(Bool_t)");
  fRadioFrame->AddFrame(timezoombutton, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));





  //Radio Button to filter channels
  chanfilterbg = new TGHButtonGroup(fRadioFrame, "Channel filter");
  fchanfilter[0] = new TGRadioButton(chanfilterbg, new TGHotString("APDs"));
  fchanfilter[1] = new TGRadioButton(chanfilterbg, new TGHotString("Wires"));
  fchanfilter[2] = new TGRadioButton(chanfilterbg, new TGHotString("Both"));
  fchanfilter[2]->SetState(kButtonDown);
  chanfilterbg->Connect("Clicked(Int_t)","EXOEventDisplayGui",
  		     this,"PushedChannelFilterButton(Int_t)");
  chanfilterbg->Show();
  fRadioFrame->AddFrame(chanfilterbg, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));




  //Radio Buttons to switch between different plot types SS 01/14/2010
  plotsbg = new TGVButtonGroup(fRadioFrame, "Plot Type");
  fplots[0] = new TGRadioButton(plotsbg, new TGHotString("Heat Plot"));
  fplots[1] = new TGRadioButton(plotsbg, new TGHotString("RMS"));
  fplots[2] = new TGRadioButton(plotsbg, new TGHotString("Mean"));
  fplots[3] = new TGRadioButton(plotsbg, new TGHotString("Misc Channels")); 
  fplots[4] = new TGRadioButton(plotsbg, new TGHotString("Derivative Signals"));
  fplots[5] = new TGRadioButton(plotsbg, new TGHotString("Threshold")); 
  fplots[6] = new TGRadioButton(plotsbg, new TGHotString("Summed APD planes")); 
  fplots[7] = new TGRadioButton(plotsbg, new TGHotString("Time slice"));
  fplots[7] = new TGRadioButton(plotsbg, new TGHotString("APD planes")); 
  fplots[0]->SetState(kButtonDown);
  //int booldummy = fplots[1]->IsDown();
  //cout << booldummy << endl;
  plotsbg->Connect("Clicked(Int_t)","EXOEventDisplayGui",
		   this,"PushedAvailablePlotButton(Int_t)");
  plotsbg->Show();		      
  
  // cout << plots_shown << endl;
  fRadioFrame->AddFrame(plotsbg, new TGLayoutHints( kLHintsExpandX, 2, 2, 2, 2));



  //Radio Button to make all plots have same color scale
  //Add a frame that can be hidden when not in split mode
  fScaleFrame = new TGHorizontalFrame(fRadioFrame,150,150); 

  scalebg = new TGVButtonGroup(fScaleFrame, "Color scale");
  fscale[0] = new TGRadioButton(scalebg, new TGHotString("Same for all plots"));
  fscale[1] = new TGRadioButton(scalebg, new TGHotString("Scale separately"));
  fscale[0]->SetState(kButtonDown);
  scalebg->Connect("Clicked(Int_t)", "EXOEventDisplayGui",
		   this, "PushedBinContentButton(Int_t)");
  scalebg->Show();
 
  fScaleFrame->AddFrame(scalebg, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

  fRadioFrame->AddFrame(fScaleFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2)); 

  // Create entry field for requested channel number. A TGNumberEntry has up and down arrows 
		    
  fChannelFrame = new TGHorizontalFrame(fRadioFrame, 150, 150); 
  //  TGLabel *fchannellabel = new TGLabel(fChannelFrame, "Chan #");
  fRequestChannel = new TGNumberEntry(fChannelFrame); 
  fRequestChannel->SetLimits(TGNumberFormat::kNELLimitMinMax, 0,1000);
  fRequestChannel->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  fRequestChannel->SetWidth(50);
  fRequestChannel->Connect("ValueSet(Long_t)","EXOEventDisplayGui",this,"ChannelUpdate()" );
  channelgo = new TGTextButton(fChannelFrame, "Go");
  channelgo->SetToolTipText("View Channel");
  channelgo->Connect("Clicked()","EXOEventDisplayGui",this,"RequestChannel(Int_t)");
  superimposebutton = new TGCheckButton(fChannelFrame, "Superimp.");
  superimposebutton->SetOn(false);
  superimposebutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "SetSuperimpose(Bool_t)");
  
  //  fChannelFrame->AddFrame(fchannellabel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fChannelFrame->AddFrame(fRequestChannel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fChannelFrame->AddFrame(channelgo, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fChannelFrame->AddFrame(superimposebutton, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fRadioFrame->AddFrame(fChannelFrame, new TGLayoutHints(kLHintsExpandX,2,2,2,2)); 

  // Create entry field for requested time slice
		    
  fTimesliceFrame = new TGHorizontalFrame(fRadioFrame, 150, 150); 
  TGLabel *timeslicelabel = new TGLabel(fTimesliceFrame, "Sample #");
  fRequestTimeslice = new TGNumberEntry(fTimesliceFrame); 
  fRequestTimeslice->SetLimits(TGNumberFormat::kNELLimitMinMax, 0,10000);
  fRequestTimeslice->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  fRequestTimeslice->SetWidth(50);
  fRequestTimeslice->Connect("ValueSet(Long_t)","EXOEventDisplayGui",this,"TimesliceUpdate()" );
  timeslicego = new TGTextButton(fTimesliceFrame, "Go");
  timeslicego->SetToolTipText("View time slice");
  timeslicego->Connect("Clicked()","EXOEventDisplayGui",this,"RequestTimeslice(Int_t)");



  fTimesliceFrame->AddFrame(timeslicelabel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fTimesliceFrame->AddFrame(fRequestTimeslice, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fTimesliceFrame->AddFrame(timeslicego, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fRadioFrame->AddFrame(fTimesliceFrame, new TGLayoutHints(kLHintsExpandX,2,2,2,2)); 

  // Create check box to draw APD labels

  fDrawAPDlabelsFrame = new TGHorizontalFrame(fRadioFrame, 150, 150); 
  DrawAPDlabelsbutton = new TGCheckButton(fDrawAPDlabelsFrame, "Draw APD labels");
  DrawAPDlabelsbutton->SetOn(false);
  DrawAPDlabelsbutton->Connect("Toggled(Bool_t)", "EXOEventDisplayGui", this, "SetDrawAPDlabels(Bool_t)");
  fDrawAPDlabelsFrame->AddFrame(DrawAPDlabelsbutton, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fRadioFrame->AddFrame(fDrawAPDlabelsFrame, new TGLayoutHints(kLHintsExpandX,2,2,2,2)); 
  

  //Radio Button to choose the number of plots to show in
  // multi-channel mode.  
  //Add a frame that can be hidden when not in split mode
  fNplexFrame = new TGHorizontalFrame(fRadioFrame,150,150); 

  nplexbg = new TGHButtonGroup(fNplexFrame, "Number of channels to plot");
  fnplex[0] = new TGRadioButton(nplexbg, new TGHotString("1"));
  fnplex[1] = new TGRadioButton(nplexbg, new TGHotString("2"));
  fnplex[2] = new TGRadioButton(nplexbg, new TGHotString("4"));
  fnplex[3] = new TGRadioButton(nplexbg, new TGHotString("9"));
  fnplex[4] = new TGRadioButton(nplexbg, new TGHotString("16"));
  fnplex[0]->SetState(kButtonDown);
  nplexbg->Connect("Clicked(Int_t)", "EXOEventDisplayGui",
		   this, "PushedNplexButton(Int_t)");
  nplexbg->Show();
 
  fNplexFrame->AddFrame(nplexbg, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

  fRadioFrame->AddFrame(fNplexFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2)); 




  // Create entry field for the threshold. A TGNumberEntry has up and down arrows 
		       
  fThreshFrame = new TGHorizontalFrame(fRadioFrame, 150, 150); 
  TGLabel *fthreshlabel = new TGLabel(fThreshFrame, "Thresh:");
  fRequestThresh = new TGNumberEntry(fThreshFrame); 
  fRequestThresh->SetLimits(TGNumberFormat::kNELLimitMinMax, 0,9999);
  fRequestThresh->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  fRequestThresh->SetWidth(50);
  fRequestThresh->Connect("ValueSet(Long_t)","EXOEventDisplayGui",this,"ThreshUpdate()" );
  threshgo = new TGTextButton(fThreshFrame, "Set ");
  threshgo->SetToolTipText("Redraw with new threshold");
  threshgo->Connect("Clicked()","EXOEventDisplayGui",this,"ThreshUpdateAndDraw()");
  morego = new TGTextButton(fThreshFrame, "More");
  morego->SetToolTipText("Show more plots");
  morego->Connect("Clicked()","EXOEventDisplayGui",this,"DoDraw()");
  
  fThreshFrame->AddFrame(fthreshlabel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fThreshFrame->AddFrame(fRequestThresh, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fRadioFrame->AddFrame(fThreshFrame, new TGLayoutHints(kLHintsExpandX,2,2,2,2)); 
  fThreshFrame->AddFrame(threshgo, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  fThreshFrame->AddFrame(morego, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
		      
  ////////////////////////Bottom frame///////////////////////////////////////////////////////////////////

  // Create a frame widget with general use buttons   

  //  hframe = new TGHorizontalFrame(fBottomFrame,1200,40);
  // fBottomFrame->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX,50,2,2,2));
    
  fRequestFrame = new TGVerticalFrame(fBottomFrame, 600, 200);  
  fBottomFrame->AddFrame(fRequestFrame, new TGLayoutHints(kLHintsExpandX,2,2,2,2));

  //Create a horizontal frame widget for number entry fields for requested run and event numbers

  evinfoframe = new TGHorizontalFrame(fRequestFrame,500,70);
  fRequestFrame->AddFrame(evinfoframe, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  //Create number entry field for requested event number
  TGLabel *fevnumlabel = new TGLabel(evinfoframe, "Event #");
  evinfoframe->AddFrame(fevnumlabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
  fRequestEvent = new TGNumberEntry(evinfoframe);
  fRequestEvent->SetLimits(TGNumberFormat::kNELLimitMinMax, 0,10000000);
  fRequestEvent->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  fRequestEvent->SetWidth(70);
  fRequestEvent->SetButtonToNum(false);
  fRequestEvent->Connect("ValueChanged(Long_t)","EXOEventDisplayGui",this,"cb_EventNumChanged(Long_t)" );
  fRequestEvent->Connect("ValueSet(Long_t)","EXOEventDisplayGui",this,"cb_EventNumSet(Long_t)" );
  fRequestEventClick = false;
  fRequestEventOld = 0;
  evinfoframe->AddFrame(fRequestEvent, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

  fEventInfo = new TGLabel(evinfoframe, " ");
  evinfoframe->AddFrame(fEventInfo, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
  

  TGLabel *fline2 = new TGLabel(fRequestFrame,"----------------------------------------------------");
  fRequestFrame->AddFrame(fline2, new TGLayoutHints(kLHintsLeft,1,1,1,1));

  // Create filename text entry field

  fileframe = new TGHorizontalFrame(fRequestFrame,210,70);
  fRequestFrame->AddFrame(fileframe, new TGLayoutHints(kLHintsLeft,2,2,2,2));
  TGLabel *ffilenamelabel = new TGLabel(fileframe, "Input File:");
  fileframe->AddFrame(ffilenamelabel, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));
  fRequestFilename = new TGTextEntry(fileframe, new TGTextBuffer(100));
  fRequestFilename->SetToolTipText("Enter filename");
  fRequestFilename->Resize(300,fRequestFilename->GetDefaultHeight() );
  fileframe->AddFrame(fRequestFilename, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  TGTextButton *FindFileButton = new TGTextButton(fileframe, " &Find... ");
  FindFileButton->Connect("Clicked()","EXOEventDisplayGui",this,"FileDialog()");
  fileframe->AddFrame(FindFileButton, new TGLayoutHints(kLHintsRight,2,2,2,2));
  TGTextButton *OpenFileButton = new TGTextButton(fileframe, " &Open ");
  OpenFileButton->Connect("Clicked()","EXOEventDisplayGui",this,"GetDataSource()");
  fileframe->AddFrame(OpenFileButton, new TGLayoutHints(kLHintsRight,2,2,2,2));

   // Create entry field for requested channel number. A TGNumberEntry has up and down arrows 
  // Moved to Radio Frame		       
  //fChannelFrame = new TGHorizontalFrame(fBottomFrame, 210, 70); 
  //TGLabel *fchannellabel = new TGLabel(fChannelFrame, "Channel #");
  //fRequestChannel = new TGNumberEntry(fChannelFrame); 
  // // fRequestChannel->SetToolTipText("Enter channel number"); //Doesn't exists for TGNumberEntry
  //fRequestChannel->SetLimits(TGNumberFormat::kNELLimitMinMax, 0,1000);
  //fRequestChannel->SetFormat(TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  //fRequestChannel->SetWidth(60);
  //fRequestChannel->Connect("ValueSet(Long_t)","EXOEventDisplayGui",this,"ChannelUpdate()" );
  //channelgo = new TGTextButton(fChannelFrame, "Go to channel");
  //channelgo->SetToolTipText("View Channel");
  //channelgo->Connect("Clicked()","EXOEventDisplayGui",this,"RequestChannel(Int_t)");
  
  // fChannelFrame->AddFrame(fchannellabel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  //fChannelFrame->AddFrame(fRequestChannel, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
  //fBottomFrame->AddFrame(fChannelFrame, new TGLayoutHints(kLHintsExpandX|kLHintsCenterY,2,2,2,2)); 
  //fChannelFrame->AddFrame(channelgo, new TGLayoutHints(kLHintsCenterY,2,2,2,2));
 
  // Create "Save As gif" button
  //savegif = new TGTextButton(fBottomFrame, "Save As (GIF)");
  //savegif->Connect("Clicked()","EXOEventDisplayGui",this,"SaveAsGif()");
  //fBottomFrame->AAddFrame(exo_refresh, new TGLayoutHints(kLHintsRight,2,2,2,2));


  // Create Refresh Button 
  //exo_refresh = new TGPictureButton(fBottomFrame,"exologo_small.jpg");
  exo_refresh = new TGTextButton(fBottomFrame, "Refresh");
  exo_refresh->SetToolTipText("Refresh");
  exo_refresh->Connect("Clicked()","EXOEventDisplayGui",this,"DoDraw()"); //this command is also temporary - needs to work with page layout
  fBottomFrame->AddFrame(exo_refresh, new TGLayoutHints(kLHintsBottom,2,2,2,2));
   
  // Create exit button

  exit = new TGTextButton(fBottomFrame," &Exit ", "gApplication->Terminate(0)"); 
  exit->SetBackgroundColor(redcolor);
  fBottomFrame->AddFrame(exit, new TGLayoutHints(kLHintsBottom|kLHintsRight,2,2,2,2));

  //draw first entry of file when display is launched

  if ( filename != "" ) GetDataSource();
  
  // Set a name to the main frame 
  fMain->SetWindowName("EXO Event Display"); 

  fLeftFrame->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,2,2,2,2)); 
  fLeftFrame->AddFrame(fBottomFrame, new TGLayoutHints(kLHintsExpandX, 2,2,2,2));

  fMain->AddFrame(fLeftFrame, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 2, 2, 2, 2)); 
  fMain->AddFrame(fRadioFrame, new TGLayoutHints(kLHintsExpandY, 2, 2, 2, 2)); 

  // Map all subwindows of main frame 
  fMain->MapSubwindows(); 
  
  fRadioFrame->HideFrame(fScaleFrame);
  fRadioFrame->HideFrame(fChannelFrame);
  fRadioFrame->HideFrame(fNplexFrame);
  fRadioFrame->HideFrame(fThreshFrame);
  fRadioFrame->HideFrame(fTimesliceFrame);
  fRadioFrame->HideFrame(fDrawAPDlabelsFrame);
  //  fRadioFrame->HideFrame(fMoreFrame);


  // Initialize the layout algorithm 
  fMain->Resize(fMain->GetDefaultSize()); 

  // Map main frame 
  fMain->MapWindow(); 

} 


void EXOEventDisplayGui::PushedGreyscaleButton( Bool_t value )
{

  greyscale = value;

  DoDraw();
}

void EXOEventDisplayGui::PushedChannelFilterButton( Int_t value)
{

  // Don't ask why we do this:

  if ( value == 1 ) {
    channel_filter = 2;
  }
  else if ( value == 2 ) {
    channel_filter = 3;
  }
  else if ( value == 3 ) {
    channel_filter = 1;
  }
  else {
    cout << "unknown channel filter value  = " << value << endl;
  }

  // Yuck!

  DoDraw();
}


void EXOEventDisplayGui::PushedBaseSubButton( Bool_t value)
{

  if ( value == subtract_baseline ) return;

  subtract_baseline = value;

  FillSignalHistograms();

  DoDraw();
}

void EXOEventDisplayGui::PushedSplitButton( Bool_t value)
{
  if ( value == true ) {
    split_plots = true;
    if ( plots_shown == 1 ) fRadioFrame->ShowFrame(fScaleFrame);
  }
  else {
    split_plots = false;
    fRadioFrame->HideFrame(fScaleFrame);
  }
  DoDraw();
}

void EXOEventDisplayGui::PushedBadChannelButton( Bool_t value)
{

  suppress_bad_channels = value;

  FillSignalHistograms();

  DoDraw();
}


void EXOEventDisplayGui::PushedShowHitsButton( Bool_t value)
{
  show_hits = value;

  DoDraw();
}


void EXOEventDisplayGui::PushedTimeZoomButton( Bool_t value)
{

  if ( eventData->GetWaveformData()->fNumSamples < 200 ) {
    time_zoom = false;
  }
  else {
    time_zoom = value;
  }

  FillSignalHistograms();

  DoDraw();
}






void EXOEventDisplayGui::SetSuperimpose( Bool_t value)
{
  superimpose = value;
}



void EXOEventDisplayGui::SetDrawAPDlabels( Bool_t value)
{
  DrawAPDlabels = value;
}

void EXOEventDisplayGui::PushedLogButton( Bool_t value)
{

  if ( value == logplot ) return;

  logplot = value;

  FillSignalHistograms();

  DoDraw();
}

void EXOEventDisplayGui::PushedAvailablePlotButton( Int_t button_id_3)
{
  //cout << "Pushed available plot button number " << button_id_3  << endl;
  plots_shown = button_id_3;

  fRadioFrame->HideFrame(fScaleFrame);
  if ( plots_shown == 1 && split_plots == true ) {
    fRadioFrame->ShowFrame(fScaleFrame);
  }

  if ( plots_shown == 1 || plots_shown == 2 || plots_shown == 3 || plots_shown == 8 ) {
    fRadioFrame->ShowFrame(fSplitFrame);
  } 
  else {
    fRadioFrame->HideFrame(fSplitFrame);
  }

  if (plots_shown == 4){ //Show the channel selector entry field if channel select mode radio button is down.
    fRadioFrame->ShowFrame(fChannelFrame);
    ChannelUpdate(); //Contains DoDraw(), so don't call it twice
  }
  else{
    fRadioFrame->HideFrame(fChannelFrame);
  }

  if (plots_shown == 6) { // Show the nplex button group if threshold mode is down
    fRadioFrame->ShowFrame(fNplexFrame);
    fRadioFrame->ShowFrame(fThreshFrame);
    
    //    fRadioFrame->ShowFrame(fMoreFrame);
  } 
  else{ 
    fRadioFrame->HideFrame(fNplexFrame);
    fRadioFrame->HideFrame(fThreshFrame);
    //    fRadioFrame->HideFrame(fMoreFrame);
  }

  if (plots_shown == 8 ){ //Show the time slice selector entry field
    fRadioFrame->ShowFrame(fTimesliceFrame);
    fRadioFrame->HideFrame(fDrawAPDlabelsFrame);
    TimesliceUpdate(); //Contains DoDraw(), so don't call it twice
  }
  else if (plots_shown == 9 ){ //Show the time slice selector entry field

    fRadioFrame->ShowFrame(fTimesliceFrame);
    fRadioFrame->ShowFrame(fDrawAPDlabelsFrame);

    // Find the timeslice with the maximal APD signal

    TH1F *h4000 = (TH1F*)gROOT->FindObject("h4000");
    if ( h4000 == NULL ) {
      cout << "Error: can't find signal histogram h4000" << endl;
      return;}
    TH1F *h4100 = (TH1F*)gROOT->FindObject("h4100");
    if ( h4100 == NULL ) {
      cout << "Error: can't find signal histogram h4100" << endl;
      return;}

    float APDsummax = h4000->GetBinContent(1) + h4100->GetBinContent(1);
    int imax = 0;
    for ( int i = 1; i < eventData->GetWaveformData()->fNumSamples; i++ ) {
      float APDsum = h4000->GetBinContent(i+1) + h4100->GetBinContent(i+1);
      if ( APDsum > APDsummax ) {
	APDsummax = APDsum;
	imax = i;
      }
    }

    itimeslice = imax;
    fRequestTimeslice->SetIntNumber(itimeslice);
    FillSignalHistograms();
    DoDraw();
  }
  else  {
    fRadioFrame->HideFrame(fTimesliceFrame);
    fRadioFrame->HideFrame(fDrawAPDlabelsFrame);
    DoDraw();
  }

}
 
void EXOEventDisplayGui::PushedBinContentButton( Int_t button_id_4)
{
  //cout <<"Pushed bin content button number " << button_id_4 << endl;
  zscale = button_id_4;
  DoDraw();
}



void EXOEventDisplayGui::RequestChannel( Int_t channum_id)
{
  channumber = fRequestChannel->GetIntNumber();
  
  if (channumber < 0 || channumber > (int)NUMBER_READOUT_CHANNELS){ //error catching
    cout << "Channel selection is out of range. Choose a channel between 0 and " 
	 << NUMBER_READOUT_CHANNELS << " inclusive." << endl;
  }
  else { 
    DoDraw();
  } 
}


void EXOEventDisplayGui::RequestTimeslice( Int_t value)
{
  itimeslice = fRequestTimeslice->GetIntNumber();
  
  if (itimeslice < 0 ) {
    cout << "Requested time slice should not be negative" << endl;
    itimeslice = 0;
  }
  else if ( itimeslice >= eventData->GetWaveformData()->fNumSamples ) {
    cout << "Requested time slice is greater than or equal to nsample = " << 
      eventData->GetWaveformData()->fNumSamples << endl;
    itimeslice = 0;
  }
  else {
    FillSignalHistograms();
    DoDraw();
  } 
}



static int ithreshloop_start = 0;

void EXOEventDisplayGui::PushedNplexButton( Int_t value)
{
  if ( value == 1 ) nplex = 1;
  else if ( value == 2 ) nplex = 2;
  else if ( value == 3 ) nplex = 4;
  else if ( value == 4 ) nplex = 9;
  else if ( value == 5 ) nplex = 16;
  else nplex = 1;

  ithreshloop_start = 0;

  //  cout << "nplex = " << nplex << endl;

  DoDraw();
}



/////////////////////////////////////////////////////////////////////////////////////////////
///////////////DRAW HISTOGRAMS FUNCTION//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void EXOEventDisplayGui::DoDraw()
{


  // Get the pointers to the histrograms

  TH2F *h1000 = (TH2F*)gROOT->FindObject("h1000");
  if ( h1000 == NULL ) { 
    cout << "Error: can't find signal histogram h1000" << endl;
    return;}
  TH2F *h1100 = (TH2F*)gROOT->FindObject("h1100");
  if ( h1100 == NULL ) {
    cout << "Error: can't find signal histogram h1100" << endl;
    return;}
  TH2F *h1200 = (TH2F*)gROOT->FindObject("h1200");
  if ( h1200 == NULL ) {
    cout << "Error: can't find signal histogram h1200" << endl;
    return;}
  TH2F *h1300 = (TH2F*)gROOT->FindObject("h1300");
  if ( h1300 == NULL ) {
    cout << "Error: can't find signal histogram h1300" << endl;
    return;}
  TH2F *h1400 = (TH2F*)gROOT->FindObject("h1400");
  if ( h1400 == NULL ) {
    cout << "Error: can't find signal histogram h1400" << endl;
    return;}
  TH2F *h1500 = (TH2F*)gROOT->FindObject("h1500");
  if ( h1500 == NULL ) {
    cout << "Error: can't find signal histogram h1500" << endl;
    return; }
  TH2F *h1600 = (TH2F*)gROOT->FindObject("h1600");
  if ( h1600 == NULL ) {
    cout << "Error: can't find signal histogram h1600" << endl;
    return;}
  TH2F *h1700 = (TH2F*)gROOT->FindObject("h1700");
  if ( h1700 == NULL ) {
    cout << "Error: can't find signal histogram h1700" << endl;
    return;}
  TH2F *h2000 = (TH2F*)gROOT->FindObject("h2000");
  if ( h2000 == NULL ) {
    cout << "Error: can't find signal histogram h2000" << endl;
    return;}
  TH2F *h2100 = (TH2F*)gROOT->FindObject("h2100");
  if ( h2100 == NULL ) {
    cout << "Error: can't find signal histogram h2100" << endl;
    return;}
  TH1F *h4000 = (TH1F*)gROOT->FindObject("h4000");
  if ( h4000 == NULL ) {
    cout << "Error: can't find signal histogram h4000" << endl;
    return;}
  TH1F *h4100 = (TH1F*)gROOT->FindObject("h4100");
  if ( h4100 == NULL ) {
    cout << "Error: can't find signal histogram h4100" << endl;
    return;}
  TH1F *h5000 = (TH1F*)gROOT->FindObject("h5000");
  if ( h5000 == NULL ) {
    cout << "Error: can't find signal histogram h5000" << endl;
    return;}
  TH1F *h5100 = (TH1F*)gROOT->FindObject("h5100");
  if ( h5100 == NULL ) {
    cout << "Error: can't find signal histogram h5100" << endl;
    return;}
  TH1F *h5200 = (TH1F*)gROOT->FindObject("h5200");
  if ( h5200 == NULL ) {
    cout << "Error: can't find signal histogram h5200" << endl;
    return;}
  TH1F *h5300 = (TH1F*)gROOT->FindObject("h5300");
  if ( h5300 == NULL ) {
    cout << "Error: can't find signal histogram h5300" << endl;
    return;}
  TH1F *h5400 = (TH1F*)gROOT->FindObject("h5400");
  if ( h5400 == NULL ) {
    cout << "Error: can't find signal histogram h5400" << endl;
    return;}
  TH1F *h5500 = (TH1F*)gROOT->FindObject("h5500");
  if ( h5500 == NULL ) {
    cout << "Error: can't find signal histogram h5500" << endl;
    return;}
  TH1F *h5600 = (TH1F*)gROOT->FindObject("h5600");
  if ( h5600 == NULL ) {
    cout << "Error: can't find signal histogram h5600" << endl;
    return;}
  TH1F *h5700 = (TH1F*)gROOT->FindObject("h5700");
  if ( h5700 == NULL ) {
    cout << "Error: can't find signal histogram h5700" << endl;
    return;}
  TH1F *h6000 = (TH1F*)gROOT->FindObject("h6000");
  if ( h6000 == NULL ) {
    cout << "Error: can't find signal histogram h6000" << endl;
    return;}
  TH1F *h6100 = (TH1F*)gROOT->FindObject("h6100");
  if ( h6100 == NULL ) {
    cout << "Error: can't find signal histogram h6100" << endl;
    return;}
  TH1F *h6200 = (TH1F*)gROOT->FindObject("h6200");
  if ( h6200 == NULL ) {
    cout << "Error: can't find signal histogram h6200" << endl;
    return;}
  TH1F *h6300 = (TH1F*)gROOT->FindObject("h6300");
  if ( h6300 == NULL ) {
    cout << "Error: can't find signal histogram h6300" << endl;
    return;}
  TH1F *h6400 = (TH1F*)gROOT->FindObject("h6400");
  if ( h6400 == NULL ) {
    cout << "Error: can't find signal histogram h6400" << endl;
    return;}
  TH1F *h6500 = (TH1F*)gROOT->FindObject("h6500");
  if ( h6500 == NULL ) {
    cout << "Error: can't find signal histogram h6500" << endl;
    return;}
  TH1F *h6600 = (TH1F*)gROOT->FindObject("h6600");
  if ( h6600 == NULL ) {
    cout << "Error: can't find signal histogram h6600" << endl;
    return;}
  TH1F *h6700 = (TH1F*)gROOT->FindObject("h6700");
  if ( h6700 == NULL ) {
    cout << "Error: can't find signal histogram h6700" << endl;
    return;}
  TH1F *h7000 = (TH1F*)gROOT->FindObject("h7000");
  if ( h7000 == NULL ) {
    cout << "Error: can't find signal histogram h7000" << endl;
    return;}
  TH1F *h7100 = (TH1F*)gROOT->FindObject("h7100");
  if ( h7100 == NULL ) {
    cout << "Error: can't find signal histogram h7100" << endl;
    return;}
  TH1F *h7200 = (TH1F*)gROOT->FindObject("h7200");
  if ( h7200 == NULL ) {
    cout << "Error: can't find signal histogram h7200" << endl;
    return;}
  TH1F *h7300 = (TH1F*)gROOT->FindObject("h7300");
  if ( h7300 == NULL ) {
    cout << "Error: can't find signal histogram h7300" << endl;
    return;}
  TH1F *h7400 = (TH1F*)gROOT->FindObject("h7400");
  if ( h7400 == NULL ) {
    cout << "Error: can't find signal histogram h7400" << endl;
    return;}
  TH1F *h7500 = (TH1F*)gROOT->FindObject("h7500");
  if ( h7500 == NULL ) {
    cout << "Error: can't find signal histogram h7500" << endl;
    return;}
  TH1F *h7600 = (TH1F*)gROOT->FindObject("h7600");
  if ( h7600 == NULL ) {
    cout << "Error: can't find signal histogram h7600" << endl;
    return;}
  TH1F *h7700 = (TH1F*)gROOT->FindObject("h7700");
  if ( h7700 == NULL ) {
    cout << "Error: can't find signal histogram h7700" << endl;
    return;}

  // set color or greyscale
  if ( greyscale == false ) {
    if ( fewer_colors == true ) {
      gStyle->SetPalette(1); // crude color 
    }
    else {
      gStyle->SetPalette(999,colorpalette);
      gStyle->SetNumberContours(999);
    }  
  }
  else {
    gStyle->SetPalette(9); // greyscale
  }

  gStyle->SetOptLogz(logplot);

  if (split_plots == true){ //Set color scale mode.
    if (zscale < 1 || zscale > 2){
      cout << "zscale undefined. cannot properly set color scale." << endl;
    }
       
    if (zscale == 1){ // set color scale same for all plots
      // Get maximum of apd and wire signals
      double zmax_wires = 0;
      double zmax_apds = max( h1200->GetMaximum(),  h1300->GetMaximum() );
      double zmaxes_wires[4] = {  h1400->GetMaximum(),  h1500->GetMaximum(),
				 h1600->GetMaximum(),  h1700->GetMaximum() };
      for (int i = 0; i < 4; i++){
	zmax_wires = max(zmax_wires, zmaxes_wires[i]);
      }
      h1200->SetMaximum(zmax_apds);
      h1300->SetMaximum(zmax_apds);
      h1400->SetMaximum(zmax_wires);
      h1500->SetMaximum(zmax_wires);
      h1600->SetMaximum(zmax_wires);
      h1700->SetMaximum(zmax_wires);
     }
  }

  double APDsum1max = h4000->GetMaximum();
  double APDsum2max = h4100->GetMaximum();
  double APDsum1min = h4000->GetMinimum();
  double APDsum2min = h4100->GetMinimum();
  double APDsumdelta = max(APDsum1max - APDsum1min, APDsum2max - APDsum2min);

  h4000->SetMaximum(max(APDsum1max, APDsum2max) + 0.15*APDsumdelta);
  h4100->SetMaximum(max(APDsum1max, APDsum2max) + 0.15*APDsumdelta);
  h4000->SetMinimum(min(APDsum1min, APDsum2min) - 0.15*APDsumdelta);
  h4100->SetMinimum(min(APDsum1min, APDsum2min) - 0.15*APDsumdelta);

  // Don't print statistics on the plot

  gStyle->SetOptStat(0000000);

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->Clear();
 
  if (plots_shown == 1){

    if ( split_plots == false ) {

      if ( channel_filter == 1 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(2);
	h1000->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1000);
	fCanvas->cd(1);
	h1100->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(2);
      }
      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h1100->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(2);
      }
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h1000->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1000);
      }

    }

    else { // this is the split_plots == true case

      if ( channel_filter == 1 ) {
	fCanvas->Divide(2,3);
	fCanvas->cd(1);
	h1200->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(0);
	fCanvas->cd(2);
	h1300->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(1);
	fCanvas->cd(3);
	h1400->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1400);
	fCanvas->cd(5);
	h1500->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1500);
	fCanvas->cd(4);
	h1600->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1600);
	fCanvas->cd(6);
	h1700->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1700);
      }

      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(1);
	h1200->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(0);
	fCanvas->cd(2);
	h1300->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedAPDHits(1);
      }
      
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,4);
	fCanvas->cd(1);
	h1400->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1400);
	fCanvas->cd(2);
	h1500->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1500);
	fCanvas->cd(3);
	h1600->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1600);
	fCanvas->cd(4);
	h1700->DrawCopy("colz");
	if ( show_hits == true ) DrawReconstructedWireHits(h1700);
      }

    }

  }

  if ( plots_shown == 2 ) { // RMS case

    if ( split_plots == false ) {

      if ( channel_filter == 1 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(2);
	h5000->DrawCopy();
	fCanvas->cd(1);
	h5100->DrawCopy();
      }
      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h5100->DrawCopy();
      }
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h5000->DrawCopy();
      }

    }

    else { // this is the split_plots == true case

      if ( channel_filter == 1 ) {
	fCanvas->Divide(2,3);
	fCanvas->cd(1);
	h5200->DrawCopy();
	fCanvas->cd(2);
	h5300->DrawCopy();
	fCanvas->cd(3);
	h5400->DrawCopy();
	fCanvas->cd(5);
	h5500->DrawCopy();
	fCanvas->cd(4);
	h5600->DrawCopy();
	fCanvas->cd(6);
	h5700->DrawCopy();
      }

      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(1);
	h5200->DrawCopy();
	fCanvas->cd(2);
	h5300->DrawCopy();
      }
      
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,4);
	fCanvas->cd(1);
	h5400->DrawCopy();
	fCanvas->cd(2);
	h5500->DrawCopy();
	fCanvas->cd(3);
	h5600->DrawCopy();
	fCanvas->cd(4);
	h5700->DrawCopy();
      }

    }

  }

  // Plot the means

  if ( plots_shown == 3 ) { 
    if ( split_plots == false ) {

      if ( channel_filter == 1 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(2);
	h6000->DrawCopy();
	fCanvas->cd(1);
	h6100->DrawCopy();
      }
      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h6100->DrawCopy();
      }
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h6000->DrawCopy();
      }

    }

    else { // this is the split_plots == true case

      if ( channel_filter == 1 ) {
	fCanvas->Divide(2,3);
	fCanvas->cd(1);
	h6200->DrawCopy();
	fCanvas->cd(2);
	h6300->DrawCopy();
	fCanvas->cd(3);
	h6400->DrawCopy();
	fCanvas->cd(5);
	h6500->DrawCopy();
	fCanvas->cd(4);
	h6600->DrawCopy();
	fCanvas->cd(6);
	h6700->DrawCopy();
      }

      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(1);
	h6200->DrawCopy();
	fCanvas->cd(2);
	h6300->DrawCopy();
      }
      
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,4);
	fCanvas->cd(1);
	h6400->DrawCopy();
	fCanvas->cd(2);
	h6500->DrawCopy();
	fCanvas->cd(3);
	h6600->DrawCopy();
	fCanvas->cd(4);
	h6700->DrawCopy();
      }

    }


  }

  if (plots_shown == 4){

    // Find the data for the new histogram

    Int_t isig = -1;
    const EXOWaveform* used_wf = NULL;
    for ( UInt_t j = 0; j < eventData->GetWaveformData()->GetNumWaveforms(); j++ ) {
      const EXOWaveform* wf = eventData->GetWaveformData()->GetWaveform(j);
      if ( wf->fChannel == channumber ) {
	isig = j;
        used_wf = wf;
	break;
      }
    }
    
    if ( isig == -1 || !used_wf) {
      cout << "Channel " << channumber << " not found in this data." << endl;
      return;
    }


    // If we aren't superimposing, then delete all existing histograms

    TH1F *htemp;
    if ( superimpose == false ) {
      for ( int i = 0; i < MAXMISC; i++ ) {
	char name[20];
	sprintf(name,"hmisc%d",imisc);
	htemp = (TH1F*)gROOT->FindObject(name);
	if ( htemp != NULL ) delete htemp;
      }
    }

    // Get the new value for imisc

    if ( superimpose == false ) {
      imisc = 0;
    }
    else {  
      imisc++;
      if ( imisc >= MAXMISC ) imisc = 0;
    }

    // Delete the current histogram, if it exists    

    char name[20];
    sprintf(name,"hmisc%d",imisc);
    htemp = (TH1F*)gROOT->FindObject(name);
    if ( htemp != NULL ) delete htemp;
        

    // Declare the new histogram

    const EXOChannelMap& channelMap = GetChanMapForHeader(eventData->fEventHeader);
    int nsamp = eventData->GetWaveformData()->fNumSamples;
    hmisc[imisc] = new TH1F(name,"",nsamp,0,nsamp);

    int izoom0 = 0;
    int izoom1 = 0;
    if ( time_zoom == true ) {
      izoom0 = (int)(nsamp*0.48);
      izoom1 = (int)(nsamp*0.56);
    }

    char chanchar[50];
    sprintf(chanchar, "Channel %d", channumber);
    if ( channelMap.good_channel(channumber) == false ) {
      sprintf(chanchar, "Channel %d: bad channel", channumber);
    }
  
    hmisc[imisc]->SetTitle(chanchar);
    hmisc[imisc]->GetXaxis()->SetTitle("Time (#mus)");
    if ( time_zoom == true ) hmisc[imisc]->GetXaxis()->SetRange(izoom0,izoom1);
    hmisc[imisc]->GetYaxis()->SetTitle("ADC counts");   
    
    // Get the baseline ADC counts for the requested channel      
    
    float base = GetBaselineCounts(*used_wf); 
    //Double_t 
    for (Int_t i = 0; i < nsamp; i++) {
      hmisc[imisc]->SetBinContent(i+1, (Double_t)((*used_wf)[i]-base));
    }

    fCanvas->Divide(1,1);
    fCanvas->cd(1);
    hmisc[0]->SetLineStyle(1);
    hmisc[0]->DrawCopy();

    for ( int i = 1; i < imisc+1; i++ ) {
      hmisc[i]->SetLineStyle(i+1);
      hmisc[i]->DrawCopy("same");
    }
    fCanvas->Update();
    

  }

  if (plots_shown == 5){

    if ( channel_filter == 1 ) {
      fCanvas->Divide(1,2);
      fCanvas->cd(2);
      h2000->DrawCopy("colz"); 
      if ( show_hits == true ) DrawReconstructedWireHits(h2000);
      fCanvas->cd(1); 
      h2100->DrawCopy("colz");
      if ( show_hits == true ) DrawReconstructedAPDHits(2);
    }

    if ( channel_filter == 2 ) {
      fCanvas->Divide(1,1);
      fCanvas->cd(1);
      h2100->DrawCopy("colz"); 
      if ( show_hits == true ) DrawReconstructedAPDHits(2);
    }

    if ( channel_filter == 3 ) {
      fCanvas->Divide(1,1);
      fCanvas->cd(1);
      h2000->DrawCopy("colz");
      if ( show_hits == true ) DrawReconstructedWireHits(h2000);
    }
  
  }

  if ( plots_shown == 6 ) {

    if ( nplex == 1 ) fCanvas->Divide(1,1);
    if ( nplex == 2 ) fCanvas->Divide(1,2);
    if ( nplex == 4 ) fCanvas->Divide(2,2);
    if ( nplex == 9 ) fCanvas->Divide(3,3);
    if ( nplex == 16 ) fCanvas->Divide(4,4);

    int ipad = 0;

    TH1F* hnplex[16];
    Int_t nsamp = eventData->GetWaveformData()->fNumSamples; //number of time samples in the data

    int ithreshloop = ithreshloop_start;
    while ( ithreshloop < (int)NUMBER_READOUT_CHANNELS ) {

      if ( (channel_filter == 2 && ithreshloop <  NCHANNEL_PER_WIREPLANE*NWIREPLANE ) ||
	   (channel_filter == 3 && ithreshloop >= NCHANNEL_PER_WIREPLANE*NWIREPLANE ) ) {
	ithreshloop++;
	continue; 
      }

      // ithreshloop is an offline channel number. Need to find the corresponding
      // entry in the data array.

      int ichannel = -1;
      const EXOWaveform* used_wf = NULL;
      for  ( size_t j = 0;  j < eventData->GetWaveformData()->GetNumWaveforms(); j++ ) {
        const EXOWaveform* wf = eventData->GetWaveformData()->GetWaveform(j);
	if ( wf->fChannel == ithreshloop ) {
	  ichannel = j;
          used_wf = wf;
	  break;
	}
      }

      if ( ichannel == -1 || !used_wf) {
	ithreshloop++;
	continue;
      }

      bool above_thresh = true;
      float base = GetBaselineCounts(*used_wf); 
      if ( threshold > 0 ) {
	above_thresh = false;
	for ( int j = 0; j < nsamp; j++ ) {
	  if ( fabs((float)((*used_wf)[j]) - base) > float(threshold) ) {
	    above_thresh = true;
	    break;
	  }
	}

      }

      if ( above_thresh == false ) {
	ithreshloop++;
	continue;
      }

      if ( ipad >= nplex ) {
	ithreshloop_start = ithreshloop;
	break;
      }      

      char name[20];
      sprintf(name,"hpad%d",ipad);

      hnplex[ipad] = (TH1F*)gROOT->FindObject(name);
      if ( hnplex[ipad] != NULL ) delete hnplex[ipad];

      char title[20];
      sprintf(title,"Channel %d", ithreshloop );
      hnplex[ipad] = new TH1F(name,title,nsamp,0,nsamp);

      for ( int j = 0; j < nsamp; j++ ) {
	hnplex[ipad]->SetBinContent(j+1,((*used_wf)[j]) - base);
      }
      fCanvas->cd(ipad+1);
      hnplex[ipad]->DrawCopy();
      ipad++;
    
      ithreshloop++;

    } // End loop over signals
  
    if ( ithreshloop == (int)NUMBER_READOUT_CHANNELS ) ithreshloop_start = 0;

  }

  if ( plots_shown == 7 ) { // Summed APD signals
    fCanvas->Divide(1,2);
    fCanvas->cd(1);
    h4000->DrawCopy();
    fCanvas->cd(2);
    h4100->DrawCopy();
  }

  if ( plots_shown == 8 ) { // timeslice view

    if ( split_plots == false ) {

      if ( channel_filter == 1 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(2);
	h7000->DrawCopy();
	fCanvas->cd(1);
	h7100->DrawCopy();
      }
      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h7100->DrawCopy();
      }
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,1);
	fCanvas->cd(1);
	h7000->DrawCopy();
      }

    }

    else { // this is the split_plots == true case

      if ( channel_filter == 1 ) {
	fCanvas->Divide(2,3);
	fCanvas->cd(1);
	h7200->DrawCopy();
	fCanvas->cd(2);
	h7300->DrawCopy();
	fCanvas->cd(3);
	h7400->DrawCopy();
	fCanvas->cd(5);
	h7500->DrawCopy();
	fCanvas->cd(4);
	h7600->DrawCopy();
	fCanvas->cd(6);
	h7700->DrawCopy();
      }

      else if ( channel_filter == 2 ) {
	fCanvas->Divide(1,2);
	fCanvas->cd(1);
	h7200->DrawCopy();
	fCanvas->cd(2);
	h7300->DrawCopy();
      }
      
      else if ( channel_filter == 3 ) {
	fCanvas->Divide(1,4);
	fCanvas->cd(1);
	h7400->DrawCopy();
	fCanvas->cd(2);
	h7500->DrawCopy();
	fCanvas->cd(3);
	h7600->DrawCopy();
	fCanvas->cd(4);
	h7700->DrawCopy();
      }

    }

  }

  if ( plots_shown == 9 ) {
    fCanvas->Divide(2,2,0.0105,0.005);

    float max_value = h7200->GetMaximum();
    if ( h7300->GetMaximum() > max_value ) max_value = h7300->GetMaximum();
    float min_value = h7200->GetMinimum();
    if ( h7300->GetMinimum() < min_value ) min_value = h7300->GetMinimum();
    if ( max_value <= min_value ) max_value = min_value + 1.0;

    Int_t ncolors = 50;
    Color_t fill_color[50];
    for (Color_t i = 0; i < ncolors; i++) {
      // this is the default "pretty" ROOT palette
      // you get when you call gStyle->SetPalette(1)
      fill_color[i] = 51+i;
    }  

    fCanvas->cd(1);
    gPad->Range(-21.0*CLHEP::cm,-21.0*CLHEP::cm,21.0*CLHEP::cm,21.0*CLHEP::cm);

    for ( int i = 0; i < 259; i++ ) {

      // Don't draw the APD which was replaced by the light pulser.
      if ( i == 61 ) continue; 

      int apdch = APDPlane_gang_table[i] - 1;
      float apd_value = (h7200->GetBinContent(1+apdch));
      float color_value = (apd_value - min_value)/(max_value - min_value);
      int icolor = int(color_value*ncolors);
      if ( icolor < 0 ) icolor = 0;
      if ( icolor >= ncolors ) icolor = ncolors - 1;

      const EXOChannelMap& channelMap = GetChanMapForHeader(eventData->fEventHeader);
      if ( suppress_bad_channels == true  && channelMap.good_channel(apdch+152) == false ) {
	// fill bad channels with gray
	APDcircle[i].SetFillColor(kGray);
	APDcircle[i].SetLineColor(kGray);
      } else {
	APDcircle[i].SetFillColor(fill_color[icolor]);
	APDcircle[i].SetLineColor(fill_color[icolor]);
      }

      APDcircle[i].Draw();

    }

    // Draw flasher

    APDflasher1.SetText(APDcircle[61].GetX1()-0.4*CLHEP::cm,APDcircle[61].GetY1()-1.2*CLHEP::cm,"*");
    APDflasher1.Draw();

    if ( DrawAPDlabels == true ) {
      for ( int i = 3; i < 259-3; i++ ) {
	if ( APDPlane_gang_table[i-1] == APDPlane_gang_table[i+1] ) { 
	  char mytext[20];
	  int apdch = APDPlane_gang_table[i] - 1;
	  sprintf(mytext,"%d",apdch+152);
	  float xtext = APDtext[i].GetX();
	  float ytext = APDtext[i].GetY();
	  APDtext[i].SetText(xtext,ytext,mytext);
	  APDtext[i].Draw();
	}
      }
    }
	

    APDlabel1.SetText(-19.0*CLHEP::cm,19.0*CLHEP::cm,"APD Plane 1");
    APDlabel1.SetTextSize(0.04);
    APDlabel1.Draw();

    char apdlabeltext2[100];
    sprintf(apdlabeltext2,"Sample # %d",itimeslice);
    APDlabel2.SetText(-19.0*CLHEP::cm,17.0*CLHEP::cm,apdlabeltext2);
    APDlabel2.SetTextSize(0.04);
    APDlabel2.Draw();

    APDlabel3.SetText(5.0*CLHEP::cm,19.0*CLHEP::cm,"View facing south");
    APDlabel3.SetTextSize(0.04);
    APDlabel3.Draw();

    APDlabel4.SetText(-19.0*CLHEP::cm,-10.0*CLHEP::cm,"CRYOSTAT DOOR");
    APDlabel4.SetTextSize(0.06);
    APDlabel4.SetTextAngle(90.0);
    APDlabel4.Draw();

    APDlabel5.SetText(-12.0*CLHEP::cm,-19.8*CLHEP::cm,"+x");
    APDlabel5.SetTextSize(0.04);
    APDlabel5.Draw();

    APDlabel6.SetText(-19.8*CLHEP::cm,-12.0*CLHEP::cm,"+y");
    APDlabel6.SetTextSize(0.04);
    APDlabel6.Draw();


    APDaxisarrow1.SetFillColor(1);
    APDaxisarrow1.SetLineWidth(2);
    APDaxisarrow1.DrawArrow(-19.0*CLHEP::cm,-19.0*CLHEP::cm,-13.0*CLHEP::cm,-19.0*CLHEP::cm,0.02);
    APDaxisarrow2.SetFillColor(1);
    APDaxisarrow2.SetLineWidth(2);
    APDaxisarrow2.DrawArrow(-19.0*CLHEP::cm,-19.0*CLHEP::cm,-19.0*CLHEP::cm,-13.0*CLHEP::cm,0.02);

    fCanvas->cd(2);
    gPad->Range(-21.0*CLHEP::cm,-21.0*CLHEP::cm,21.0*CLHEP::cm,21.0*CLHEP::cm);

    for ( int i = 259; i < 2*259; i++ ) {

      // Don't draw the APD which was replaced by the light pulser.
      if ( i == 61+259 ) continue; 

      int apdch = APDPlane_gang_table[i] - 1;
      float apd_value = (h7300->GetBinContent(1+apdch));
      float color_value = (apd_value - min_value)/(max_value - min_value);
      int icolor = int(color_value*ncolors);
      if ( icolor < 0 ) icolor = 0;
      if ( icolor >= ncolors ) icolor = ncolors - 1;


      const EXOChannelMap& channelMap = GetChanMapForHeader(eventData->fEventHeader);
      if ( suppress_bad_channels == true  && channelMap.good_channel(apdch+152+37) == false ) {
	// fill bad channels with gray
	APDcircle[i].SetFillColor(kGray);
	APDcircle[i].SetLineColor(kGray);
      } else {
	APDcircle[i].SetFillColor(fill_color[icolor]);
	APDcircle[i].SetLineColor(fill_color[icolor]);
      }
      APDcircle[i].Draw();
    
    } 

    if ( DrawAPDlabels == true ) {
      for ( int i = 259+3; i < 2*259-3; i++ ) {
	if ( APDPlane_gang_table[i-1] == APDPlane_gang_table[i+1] ) {
	  char mytext[20];
	  int apdch = APDPlane_gang_table[i] - 1;
	  sprintf(mytext,"%d",apdch+152+37);
	  float xtext = APDtext[i].GetX();
	  float ytext = APDtext[i].GetY();
	  APDtext[i].SetText(xtext,ytext,mytext); 
	  APDtext[i].Draw();
	}
      }
    }

    // Draw flasher

    APDflasher2.SetText(APDcircle[61+259].GetX1()-0.4*CLHEP::cm,APDcircle[61+259].GetY1()-1.2*CLHEP::cm,"*");
    APDflasher2.Draw();


    APDlabel11.SetText(-19.0*CLHEP::cm,19.0*CLHEP::cm,"APD Plane 2");
    APDlabel11.SetTextSize(0.04);
    APDlabel11.Draw();

    char apdlabeltext12[100];
    sprintf(apdlabeltext12,"Sample # %d",itimeslice);
    APDlabel12.SetText(-19.0*CLHEP::cm,17.0*CLHEP::cm,apdlabeltext12);
    APDlabel12.SetTextSize(0.04);
    APDlabel12.Draw();

    APDlabel13.SetText(5.0*CLHEP::cm,19.0*CLHEP::cm,"View facing south");
    APDlabel13.SetTextSize(0.04);
    APDlabel13.Draw();

    APDlabel14.SetText(-19.0*CLHEP::cm,-10.0*CLHEP::cm,"CRYOSTAT DOOR");
    APDlabel14.SetTextSize(0.06);
    APDlabel14.SetTextAngle(90.0);
    APDlabel14.Draw();

    APDlabel15.SetText(-12.0*CLHEP::cm,-19.8*CLHEP::cm,"+x");
    APDlabel15.SetTextSize(0.04);
    APDlabel15.Draw();

    APDlabel16.SetText(-19.8*CLHEP::cm,-12.0*CLHEP::cm,"+y");
    APDlabel16.SetTextSize(0.04);
    APDlabel16.Draw();


    APDaxisarrow11.SetFillColor(1);
    APDaxisarrow11.SetLineWidth(2);
    APDaxisarrow11.DrawArrow(-19.0*CLHEP::cm,-19.0*CLHEP::cm,-13.0*CLHEP::cm,-19.0*CLHEP::cm,0.02);
    APDaxisarrow12.SetFillColor(1);
    APDaxisarrow12.SetLineWidth(2);
    APDaxisarrow12.DrawArrow(-19.0*CLHEP::cm,-19.0*CLHEP::cm,-19.0*CLHEP::cm,-13.0*CLHEP::cm,0.02);

 
    fCanvas->cd(3);
    h1200->DrawCopy("colz");
    if ( show_hits == true ) DrawReconstructedAPDHits(0);
    fCanvas->cd(4);
    h1300->DrawCopy("colz");
    if ( show_hits == true ) DrawReconstructedAPDHits(1);
      
  }

  if ( print_message ) cout << "about to update canvas" << endl;
  
  fCanvas->cd();
  fCanvas->Update();

  if ( print_message ) cout << "done updating canvas, and done with DoDraw" << endl;

}


void EXOEventDisplayGui::DrawReconstructedWireHits(TH2F *hist)
{

  //  cout << "found " << eventData->GetNumReconstructedChargeClusters()  << " charge clusters" << endl;

  double x1 = hist->GetXaxis()->GetBinLowEdge(hist->GetXaxis()->GetFirst());
  double x2 = hist->GetXaxis()->GetBinLowEdge(hist->GetXaxis()->GetLast());
  double y1 = hist->GetYaxis()->GetBinLowEdge(hist->GetYaxis()->GetFirst());
  double y2 = hist->GetYaxis()->GetBinLowEdge(hist->GetYaxis()->GetLast());

  double xtweak = 8.0;
  double x1hit = 0.0;
  double x2hit = 0.0;
  for ( unsigned int j = 0; j < eventData->GetNumChargeClusters(); j++ ) {
    if ( j >= MAXHITS ) break;
    //    cout << "wire hit # " << j << " time, iu1cl, iv1cl = " << eventData->GetChargeCluster(j)->fCollectionTime/1000.0 << " " << 
    //      eventData->GetChargeCluster(j)->fChannelNumberOfFirstSignal << " " << eventData->GetChargeCluster(j)->GetUWireSignalChannelAt(0) << endl;

    UHit[j].SetX1(x1);
    UHit[j].SetX2(x2);

    EXOChargeCluster* charge_cluster = eventData->GetChargeCluster(j);
    x1hit = charge_cluster->fCollectionTime/1000.0 - hit_halfwidth + xtweak;
    x2hit = charge_cluster->fCollectionTime/1000.0 + hit_halfwidth + xtweak;

    if ( x1hit > x1 ) UHit[j].SetX1(x1hit);
    if ( x2hit < x2 ) UHit[j].SetX2(x2hit);

    UHit[j].SetY1( (Double_t)charge_cluster->GetUWireSignalChannelAt(0) + 0.5 - hit_halfheight );
    UHit[j].SetY2( (Double_t)charge_cluster->GetUWireSignalChannelAt(0) + 0.5 + hit_halfheight );

    UHit[j].SetLineColor(j%10);

    if ( UHit[j].GetX2() > x1 && UHit[j].GetX1() < x2 &&
	 UHit[j].GetY1() < y2 && UHit[j].GetY2() > y1 ) UHit[j].Draw();

    if ( charge_cluster->GetNumVWireSignals() <= 0 ) continue;

    VHit[j].SetX1(x1);
    VHit[j].SetX2(x2);

    x1hit = charge_cluster->fCollectionTime/1000.0 - hit_halfwidth + xtweak;
    x2hit = charge_cluster->fCollectionTime/1000.0 + hit_halfwidth + xtweak;
    
    if ( x1hit > x1 ) VHit[j].SetX1(x1hit);
    if ( x2hit < x2 ) VHit[j].SetX2(x2hit);
    
    VHit[j].SetY1(charge_cluster->GetUWireSignalChannelAt(0)+0.5 - hit_halfheight);
    VHit[j].SetY2(charge_cluster->GetUWireSignalChannelAt(0)+0.5 + hit_halfheight);
    
    VHit[j].SetLineColor(j%10);
      
    if ( VHit[j].GetX2() > x1 && VHit[j].GetX1() < x2 &&
	 VHit[j].GetY1() < y2 && VHit[j].GetY2() > y1) VHit[j].Draw();
    
  }


}

void EXOEventDisplayGui::DrawReconstructedAPDHits(int APDplane)
{

  // APDplane = 0 means the north APD plane
  // APDplane = 1 means the south APD plane
  // APDplane = 2 means both APD planes

  //  cout << "found " << eventData->GetNumReconstructedScintillationClusters()  << " APD clusters" << endl;

  double xtweak = 8.0;

  for ( unsigned int j = 0; j < eventData->GetNumScintillationClusters(); j++ ) {

    if ( j >= MAXHITS ) break;

    //    cout << "APD hit # " << j << " time and alg = " << eventData->GetScintillationCluster(j)->fTime/1000.0 << 
    //      " " << eventData->GetScintillationCluster(j)->fAlgorithmUsed << endl;

    EXOScintillationCluster* scint_cluster = eventData->GetScintillationCluster(j);
    cout << "APDplane = " << APDplane << endl;

    Int_t algoUsed = scint_cluster->GetAlgorithmUsed();
    if (  APDplane == 2 ||
	 (APDplane == 0 && algoUsed == 0) ||
	 (APDplane == 1 && algoUsed == 0) ||
	 (APDplane == 0 && algoUsed == 4) ||
	 (APDplane == 1 && algoUsed == 5) ) {

      
      APDHit[2*j].SetX1(scint_cluster->fTime/1000.0 - hit_halfwidth + xtweak);
      APDHit[2*j].SetX2(scint_cluster->fTime/1000.0 + hit_halfwidth + xtweak);
      APDHit[2*j+1].SetX1(scint_cluster->fTime/1000.0 - hit_halfwidth + xtweak);
      APDHit[2*j+1].SetX2(scint_cluster->fTime/1000.0 + hit_halfwidth + xtweak);
      
      APDHit[2*j].SetY1(152);
      APDHit[2*j].SetY2(152+37);
      APDHit[2*j+1].SetY1(152+37);
      APDHit[2*j+1].SetY2(152+37+37);

      APDHit[2*j].SetLineColor(0);
      APDHit[2*j+1].SetLineColor(0);
      
      if ( algoUsed == 0 ) {
	if ( APDplane != 1 ) APDHit[2*j].Draw();
	if ( APDplane != 0 ) APDHit[2*j+1].Draw();
      }
      else if ( algoUsed == 4 ) {
	if ( APDplane != 1 ) APDHit[2*j].Draw();
      }
      else if ( algoUsed == 5 ) {
	if ( APDplane != 0 ) APDHit[2*j+1].Draw();
      }

    }

  }

}



/////////////////////////////////////////////////////////////////////////////////////////////
///////////////FILL HISTOGRAMS FUNCTION//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void EXOEventDisplayGui::FillSignalHistograms()
{


  if ( print_message ) cout << "starting fill signal histograms" << endl;


  //Variables in ALL CAPS are stored in EXODimensions.h

  Int_t nsamp = eventData->GetWaveformData()->fNumSamples; //number of time samples in the data
  Int_t nchannel_per_wireplane = NCHANNEL_PER_WIREPLANE; //38  // 
  Int_t nwires = NCHANNEL_PER_WIREPLANE*NWIREPLANE;
  Int_t napds = NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;
  Int_t napds_per_zplane = NUMBER_APD_CHANNELS_PER_PLANE; //37                            

  //Define the range of channels for each wire/APD plane 
  Int_t nwires_ufirst = nchannel_per_wireplane;    //On 01/14/2010, U wires on TPC1 are ch 0-37
  Int_t nwires_vfirst = nwires_ufirst + nchannel_per_wireplane;      // V wires on TPC1 are ch 38-75 
  Int_t nwires_usecond = nwires_vfirst + nchannel_per_wireplane;     // U wires on TPC2 are ch 76-113
  Int_t nwires_vsecond = nwires_usecond + nchannel_per_wireplane;    // V wires on TPC2 are ch 114-151
  Int_t napds_first = nwires_vsecond + napds_per_zplane;             // APDs on TPC1 are ch 152-188
  Int_t napds_second = napds_first + napds_per_zplane;               // APDs on TPC2 are ch 189-225

  TH2F *htest1 = (TH2F*)gROOT->FindObject("h1000");
  if ( htest1 ) {
    delete htest1;}
  TH2F *htest11 = (TH2F*)gROOT->FindObject("h1100");
  if ( htest11 ) {
    delete htest11;}
  TH2F *htest12 = (TH2F*)gROOT->FindObject("h1200");
  if ( htest12) {
    delete htest12;}
  TH2F *htest13 = (TH2F*)gROOT->FindObject("h1300");
  if ( htest13 ) {
    delete htest13;}
  TH2F *htest14 = (TH2F*)gROOT->FindObject("h1400");
  if ( htest14 ) {
    delete htest14;}
  TH2F *htest15 = (TH2F*)gROOT->FindObject("h1500");
  if ( htest15 ) {
    delete htest15;}
  TH2F *htest16 = (TH2F*)gROOT->FindObject("h1600");
  if ( htest16 ) {
    delete htest16;}
  TH2F *htest17 = (TH2F*)gROOT->FindObject("h1700");
  if ( htest17 ) {
    delete htest17;}
  TH2F *htest2 = (TH2F*)gROOT->FindObject("h2000");
  if ( htest2 ) {
    delete htest2;}
  TH2F *htest21 = (TH2F*)gROOT->FindObject("h2100");
  if ( htest21 ) {
    delete htest21;}
  TH1F *htest40 = (TH1F*)gROOT->FindObject("h4000");
  if ( htest40 ) {
    delete htest40;}
  TH1F *htest41 = (TH1F*)gROOT->FindObject("h4100");
  if ( htest41 ) {
    delete htest41;}
  TH1F *htest50 = (TH1F*)gROOT->FindObject("h5000");
  if ( htest50 ) {
    delete htest50;}
  TH1F *htest51 = (TH1F*)gROOT->FindObject("h5100");
  if ( htest51 ) {
    delete htest51;}
  TH1F *htest52 = (TH1F*)gROOT->FindObject("h5200");
  if ( htest52 ) {
    delete htest52;}
  TH1F *htest53 = (TH1F*)gROOT->FindObject("h5300");
  if ( htest53 ) {
    delete htest53;}
  TH1F *htest54 = (TH1F*)gROOT->FindObject("h5400");
  if ( htest54 ) {
    delete htest54;}
  TH1F *htest55 = (TH1F*)gROOT->FindObject("h5500");
  if ( htest55 ) {
    delete htest55;}
  TH1F *htest56 = (TH1F*)gROOT->FindObject("h5600");
  if ( htest56 ) {
    delete htest56;}
  TH1F *htest57 = (TH1F*)gROOT->FindObject("h5700");
  if ( htest57 ) {
    delete htest57;}
  TH1F *htest60 = (TH1F*)gROOT->FindObject("h6000");
  if ( htest60 ) {
    delete htest60;}
  TH1F *htest61 = (TH1F*)gROOT->FindObject("h6100");
  if ( htest61 ) {
    delete htest61;}
  TH1F *htest62 = (TH1F*)gROOT->FindObject("h6200");
  if ( htest62 ) {
    delete htest62;}
  TH1F *htest63 = (TH1F*)gROOT->FindObject("h6300");
  if ( htest63 ) {
    delete htest63;}
  TH1F *htest64 = (TH1F*)gROOT->FindObject("h6400");
  if ( htest64 ) {
    delete htest64;}
  TH1F *htest65 = (TH1F*)gROOT->FindObject("h6500");
  if ( htest65 ) {
    delete htest65;}
  TH1F *htest66 = (TH1F*)gROOT->FindObject("h6600");
  if ( htest66 ) {
    delete htest66;}
  TH1F *htest67 = (TH1F*)gROOT->FindObject("h6700");
  if ( htest67 ) {
    delete htest67;}
  TH1F *htest70 = (TH1F*)gROOT->FindObject("h7000");
  if ( htest70 ) {
    delete htest70;}
  TH1F *htest71 = (TH1F*)gROOT->FindObject("h7100");
  if ( htest71 ) {
    delete htest71;}
  TH1F *htest72 = (TH1F*)gROOT->FindObject("h7200");
  if ( htest72 ) {
    delete htest72;}
  TH1F *htest73 = (TH1F*)gROOT->FindObject("h7300");
  if ( htest73 ) {
    delete htest73;}
  TH1F *htest74 = (TH1F*)gROOT->FindObject("h7400");
  if ( htest74 ) {
    delete htest74;}
  TH1F *htest75 = (TH1F*)gROOT->FindObject("h7500");
  if ( htest75 ) {
    delete htest75;}
  TH1F *htest76 = (TH1F*)gROOT->FindObject("h7600");
  if ( htest76 ) {
    delete htest76;}
  TH1F *htest77 = (TH1F*)gROOT->FindObject("h7700");
  if ( htest77 ) {
    delete htest77;}

  //Together Plots SS 01/15/2010
  //Wires
  TH2F *h1000 = new TH2F("h1000","",nsamp, 0, nsamp, nwires, 0, nwires);
  //APD's
  TH2F *h1100 = new TH2F("h1100","",nsamp, 0, nsamp, napds, nwires, NUMBER_READOUT_CHANNELS);
 
  //Split Plots SS 01/15/2010
  //APD Histos. One for each z-plane SS 1/14/2010
  TH2F *h1200 = new TH2F("h1200","", nsamp, 0, nsamp,
			 napds_per_zplane, nwires_vsecond, napds_first);
  TH2F *h1300 = new TH2F("h1300","", nsamp, 0, nsamp,
			 napds_per_zplane, napds_first, napds_second);
  //Wire Histos. Two per z-plane (u and v) SS 1/14/2010
  TH2F *h1400 = new TH2F("h1400","", nsamp, 0, nsamp, 
			 nchannel_per_wireplane, 0, nwires_ufirst);
  TH2F *h1500 = new TH2F("h1500","", nsamp, 0, nsamp, 
			 nchannel_per_wireplane, nwires_ufirst, nwires_vfirst);
  TH2F *h1600 = new TH2F("h1600","", nsamp, 0, nsamp, 
			 nchannel_per_wireplane, nwires_vfirst, nwires_usecond);
  TH2F *h1700 = new TH2F("h1700","", nsamp, 0, nsamp, 
			 nchannel_per_wireplane, nwires_usecond, nwires_vsecond);
  
  TH2F *h2000 = new TH2F("h2000","", nsamp, 0, nsamp, nwires, 0, nwires);
  TH2F *h2100 = new TH2F("h2100","", nsamp, 0, nsamp, napds, nwires, nwires + napds);

  // Summed APD signals

  TH1F *h4000 = new TH1F("h4000","", nsamp, 0, nsamp);
  TH1F *h4100 = new TH1F("h4100","", nsamp, 0, nsamp);

  // RMS plots

  //Wires
  TH1F *h5000 = new TH1F("h5000","",nwires,0,nwires);
  //APD's
  TH1F *h5100 = new TH1F("h5100","",napds, nwires, NUMBER_READOUT_CHANNELS);
 
  //Split Plots SS 01/15/2010
  //APD Histos. One for each z-plane 
  TH1F *h5200 = new TH1F("h5200","", napds_per_zplane, nwires_vsecond, napds_first);
  TH1F *h5300 = new TH1F("h5300","", napds_per_zplane, napds_first, napds_second);

  //Wire Histos. Two per z-plane (u and v) 
  TH1F *h5400 = new TH1F("h5400","", nchannel_per_wireplane, 0, nwires_ufirst);
  TH1F *h5500 = new TH1F("h5500","", nchannel_per_wireplane, nwires_ufirst, nwires_vfirst);
  TH1F *h5600 = new TH1F("h5600","", nchannel_per_wireplane, nwires_vfirst, nwires_usecond);
  TH1F *h5700 = new TH1F("h5700","", nchannel_per_wireplane, nwires_usecond, nwires_vsecond);

  // Mean plots

  //Wires
  TH1F *h6000 = new TH1F("h6000","",nwires,0,nwires);
  //APD's
  TH1F *h6100 = new TH1F("h6100","",napds, nwires, NUMBER_READOUT_CHANNELS);
 
  //Split Plots SS 01/15/2010
  //APD Histos. One for each z-plane 
  TH1F *h6200 = new TH1F("h6200","", napds_per_zplane, nwires_vsecond, napds_first);
  TH1F *h6300 = new TH1F("h6300","", napds_per_zplane, napds_first, napds_second);

  //Wire Histos. Two per z-plane (u and v) 
  TH1F *h6400 = new TH1F("h6400","", nchannel_per_wireplane, 0, nwires_ufirst);
  TH1F *h6500 = new TH1F("h6500","", nchannel_per_wireplane, nwires_ufirst, nwires_vfirst);
  TH1F *h6600 = new TH1F("h6600","", nchannel_per_wireplane, nwires_vfirst, nwires_usecond);
  TH1F *h6700 = new TH1F("h6700","", nchannel_per_wireplane, nwires_usecond, nwires_vsecond);

  // Timeslice plots

  //Wires
  TH1F *h7000 = new TH1F("h7000","",nwires,0,nwires);
  //APD's
  TH1F *h7100 = new TH1F("h7100","",napds, nwires, NUMBER_READOUT_CHANNELS);
 
  //Split Plots SS 01/15/2010
  //APD Histos. One for each z-plane 
  TH1F *h7200 = new TH1F("h7200","", napds_per_zplane, nwires_vsecond, napds_first);
  TH1F *h7300 = new TH1F("h7300","", napds_per_zplane, napds_first, napds_second);

  //Wire Histos. Two per z-plane (u and v) 
  TH1F *h7400 = new TH1F("h7400","", nchannel_per_wireplane, 0, nwires_ufirst);
  TH1F *h7500 = new TH1F("h7500","", nchannel_per_wireplane, nwires_ufirst, nwires_vfirst);
  TH1F *h7600 = new TH1F("h7600","", nchannel_per_wireplane, nwires_vfirst, nwires_usecond);
  TH1F *h7700 = new TH1F("h7700","", nchannel_per_wireplane, nwires_usecond, nwires_vsecond);
    

  int izoom0 = 0;
  int izoom1 = 0;
  if ( time_zoom == true ) {
    izoom0 = (int)(nsamp*0.48);
    izoom1 = (int)(nsamp*0.56);
  }

  h1000->SetTitle("Wire signals vs time");;
  h1000->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1000->GetXaxis()->SetRange(izoom0,izoom1);
  h1000->GetYaxis()->SetTitle("Wire Channel");

  h1100->SetTitle("APD signals vs time");;
  h1100->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1100->GetXaxis()->SetRange(izoom0,izoom1);
  h1100->GetYaxis()->SetTitle("APD Channel");

  h1200->SetTitle("APD signals vs time - TPC 1");
  h1200->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1200->GetXaxis()->SetRange(izoom0,izoom1);
  h1200->GetYaxis()->SetTitle("APD Channel");
  
  h1300->SetTitle("APD signals vs time - TPC 2");
  h1300->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1300->GetXaxis()->SetRange(izoom0,izoom1);
  h1300->GetYaxis()->SetTitle("APD Channel"); 
  
  h1400->SetTitle("U-wire signals vs time - TPC 1");
  h1400->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1400->GetXaxis()->SetRange(izoom0,izoom1);
  h1400->GetYaxis()->SetTitle("Wire Channel"); 
  
  h1500->SetTitle("V-wire signals vs time - TPC 1");
  h1500->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1500->GetXaxis()->SetRange(izoom0,izoom1);
  h1500->GetYaxis()->SetTitle("Wire Channel");
  
  h1600->SetTitle("U-wire signals vs time - TPC 2");
  h1600->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1600->GetXaxis()->SetRange(izoom0,izoom1);
  h1600->GetYaxis()->SetTitle("Wire Channel");
  
  h1700->SetTitle("V-wire signals vs time - TPC 2");
  h1700->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h1700->GetXaxis()->SetRange(izoom0,izoom1);
  h1700->GetYaxis()->SetTitle("Wire Channel");
  
  h2000->SetTitle("Derivative of raw wire signals vs time");  
  h2000->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h2000->GetXaxis()->SetRange(izoom0,izoom1);
  h2000->GetYaxis()->SetTitle("Wire channel");
  
  h2100->SetTitle("Derivative of raw APDs signals vs time");
  h2100->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h2100->GetXaxis()->SetRange(izoom0,izoom1);
  h2100->GetYaxis()->SetTitle("APD channel");   
  
  h4000->SetTitle("APD Sum: TPC 1");
  h4000->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h4000->GetXaxis()->SetRange(izoom0,izoom1);
  h4000->GetYaxis()->SetTitle("ADC counts");   

  h4100->SetTitle("APD Sum: TPC 2");
  h4100->GetXaxis()->SetTitle("Time (#mus)");
  if ( time_zoom == true ) h4100->GetXaxis()->SetRange(izoom0,izoom1);
  h4100->GetYaxis()->SetTitle("ADC counts");   

  h5000->SetTitle("RMS fluctuations - Wires");
  h5000->GetXaxis()->SetTitle("Offline channel number");
  h5000->GetYaxis()->SetTitle("ADC counts");   

  h5100->SetTitle("RMS fluctuations - APDs");
  h5100->GetXaxis()->SetTitle("Offline channel number");
  h5100->GetYaxis()->SetTitle("ADC counts");   

  h5200->SetTitle("RMS fluctuations - APDs: TPC 1");
  h5200->GetXaxis()->SetTitle("Offline channel number");
  h5200->GetYaxis()->SetTitle("ADC counts");   

  h5300->SetTitle("RMS fluctuations - APDs: TPC 2");
  h5300->GetXaxis()->SetTitle("Offline channel number");
  h5300->GetYaxis()->SetTitle("ADC counts");   

  h5400->SetTitle("RMS fluctuations - U Wires: TPC 1");
  h5400->GetXaxis()->SetTitle("Offline channel number");
  h5400->GetYaxis()->SetTitle("ADC counts");   

  h5500->SetTitle("RMS fluctuations - V Wires: TPC 1");
  h5500->GetXaxis()->SetTitle("Offline channel number");
  h5500->GetYaxis()->SetTitle("ADC counts");   

  h5600->SetTitle("RMS fluctuations - U Wires: TPC 2");
  h5600->GetXaxis()->SetTitle("Offline channel number");
  h5600->GetYaxis()->SetTitle("ADC counts");   

  h5700->SetTitle("RMS fluctuations - V Wires: TPC 2");
  h5700->GetXaxis()->SetTitle("Offline channel number");
  h5700->GetYaxis()->SetTitle("ADC counts");   

  h6000->SetTitle("Mean - Wires");
  h6000->GetXaxis()->SetTitle("Offline channel number");
  h6000->GetYaxis()->SetTitle("ADC counts");   

  h6100->SetTitle("Mean - APDs");
  h6100->GetXaxis()->SetTitle("Offline channel number");
  h6100->GetYaxis()->SetTitle("ADC counts");   

  h6200->SetTitle("Mean - APDs: TPC 1");
  h6200->GetXaxis()->SetTitle("Offline channel number");
  h6200->GetYaxis()->SetTitle("ADC counts");   

  h6300->SetTitle("Mean - APDs: TPC 2");
  h6300->GetXaxis()->SetTitle("Offline channel number");
  h6300->GetYaxis()->SetTitle("ADC counts");   

  h6400->SetTitle("Mean - U Wires: TPC 1");
  h6400->GetXaxis()->SetTitle("Offline channel number");
  h6400->GetYaxis()->SetTitle("ADC counts");   

  h6500->SetTitle("Mean - V Wires: TPC 1");
  h6500->GetXaxis()->SetTitle("Offline channel number");
  h6500->GetYaxis()->SetTitle("ADC counts");   

  h6600->SetTitle("Mean - U Wires: TPC 2");
  h6600->GetXaxis()->SetTitle("Offline channel number");
  h6600->GetYaxis()->SetTitle("ADC counts");   

  h6700->SetTitle("Mean - V Wires: TPC 2");
  h6700->GetXaxis()->SetTitle("Offline channel number");
  h6700->GetYaxis()->SetTitle("ADC counts");   

  char timeslicetitle[50];

  sprintf(timeslicetitle,"Time slice - Wires - Sample # %d", itimeslice);
  h7000->SetTitle(timeslicetitle);
  h7000->GetXaxis()->SetTitle("Offline channel number");
  h7000->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - APDs - Sample # %d", itimeslice);
  h7100->SetTitle(timeslicetitle);
  h7100->GetXaxis()->SetTitle("Offline channel number");
  h7100->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - APDs - Sample # %d: TPC 1", itimeslice);
  h7200->SetTitle(timeslicetitle);
  h7200->GetXaxis()->SetTitle("Offline channel number");
  h7200->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - APDs - Sample # %d: TPC 2", itimeslice);
  h7300->SetTitle(timeslicetitle);
  h7300->GetXaxis()->SetTitle("Offline channel number");
  h7300->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - U Wires - Sample # %d: TPC 1", itimeslice);
  h7400->SetTitle(timeslicetitle);
  h7400->GetXaxis()->SetTitle("Offline channel number");
  h7400->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - V Wires - Sample # %d: TPC 2", itimeslice);
  h7500->SetTitle(timeslicetitle);
  h7500->GetXaxis()->SetTitle("Offline channel number");
  h7500->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - U Wires - Sample # %d: TPC 2", itimeslice);
  h7600->SetTitle(timeslicetitle);
  h7600->GetXaxis()->SetTitle("Offline channel number");
  h7600->GetYaxis()->SetTitle("ADC counts");   

  sprintf(timeslicetitle,"Time slice - V Wires - Sample # %d: TPC 2", itimeslice);
  h7700->SetTitle(timeslicetitle);
  h7700->GetXaxis()->SetTitle("Offline channel number");
  h7700->GetYaxis()->SetTitle("ADC counts");   


  const EXOChannelMap& channelMap = GetChanMapForHeader(eventData->fEventHeader);
  for ( size_t isig = 0; isig < eventData->GetWaveformData()->GetNumWaveforms(); isig++ ) {
    const EXOWaveform& wf = *eventData->GetWaveformData()->GetWaveform(isig);
    Int_t ch = wf.fChannel; 
    if ( ch < 0 ) continue;
    
    if ( suppress_bad_channels == true  && channelMap.good_channel(ch) == false ) {
      continue;
    }

    // Get the baseline ADC counts for the requested channel      
    float base =  GetBaselineCounts(wf);

    for (Int_t i = 0; i < nsamp; i++ ) {
      
      if (plots_shown <=0){//Error msg
	cout << "plots_shown variable less than zero (out of range)" << endl;
      }
      
      if(ch < nwires){
	h1000->SetBinContent(i+1, ch+1, 
			     (Double_t)(wf[i]-base));
      }
      else {
	h1100->SetBinContent(i+1, ch-nwires+1,
			     (Double_t)(wf[i]-base));
      }
      
      // TPC 1 APDs
      if (ch >= nwires_vsecond && ch < napds_first){
	h1200->SetBinContent(i + 1, ch - nwires_vsecond + 1, 
			     (Double_t)(wf[i]-base));
	h4000->Fill(i, (Double_t)(wf[i]-base));
      }
      // TPC 2 APDs
      else if (ch >= napds_first && ch < napds_second){
	h1300->SetBinContent(i + 1, ch - napds_first + 1,
			     (Double_t)(wf[i]-base));
	h4100->Fill(i, (Double_t)(wf[i]-base));
      }
      // TPC 1 U-wires
      else if (ch >= 0 && ch < nwires_ufirst){
	h1400->SetBinContent(i + 1, ch + 1, 
			     (Double_t)(wf[i]-base));
      }
      // TPC 1 V-wires
      else if (ch >= nwires_ufirst && ch < nwires_vfirst){
	h1500->SetBinContent(i + 1, ch - nwires_ufirst + 1, 
			     (Double_t)(wf[i]-base));
      }
      // TPC 2 U-wires
      else if (ch >= nwires_vfirst && ch < nwires_usecond){
	h1600->SetBinContent(i + 1, ch - nwires_vfirst + 1, 
			     (Double_t)(wf[i]-base));
      }		      
      // TPC 2 V-wires
      else if (ch >= nwires_usecond && ch < nwires_vsecond){
	h1700->SetBinContent(i + 1, ch - nwires_usecond + 1, 
			     (Double_t)(wf[i]-base));
      }     
      
      //plot difference between wire signals at time t+1 us and time t
      if (ch < nwires &&  i < nsamp - 1 ) {	
	h2000->SetBinContent(i+1,ch+1,
			     (Double_t)(wf[i+1]-wf[i]));
      }
      
      if (ch >= nwires && i < nsamp - 1) {
	//plot difference between apd signals at time t+1 us and time t
	h2100->SetBinContent(i+1,ch-nwires+1,
			     (Double_t)(wf[i+1]-wf[i]));
      }
      
    } // End loop over samples in one signal

      // Initialize RMS and sums
    
    double RMS = -1.0;
    double mean = 0.0;
    double signal_sum = 0.0;
    double signal_sqr_sum = 0.0;
    
    // Loop over the digitized values in the signal
    
    for ( int i = 0; i < nsamp; i++ ) {
      signal_sum += wf[i] - base;
      signal_sqr_sum += (wf[i]-base)*
	(wf[i]-base);
    }
    
    // Calculate RMS and mean
    
    RMS = signal_sqr_sum/(float)nsamp - 
      (signal_sum/(float)nsamp*(signal_sum/(float)nsamp));
    RMS = sqrt(RMS);
    
    mean = signal_sum/(float)(nsamp);

    float timeslice = 0;
    if ( itimeslice >= 0 && itimeslice < nsamp ) {
      timeslice = wf[(size_t)itimeslice] - base;
    }

    if(ch < nwires) {
      h5000->SetBinContent(ch+1,RMS);
      h6000->SetBinContent(ch+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7000->SetBinContent(ch+1,timeslice);
    }
    else {
      h5100->SetBinContent(ch-nwires+1,RMS);
      h6100->SetBinContent(ch-nwires+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7100->SetBinContent(ch-nwires+1,timeslice);
    }
    
    // TPC 1 APDs
    if (ch >= nwires_vsecond && ch < napds_first) {
      h5200->SetBinContent(ch-nwires_vsecond+1,RMS);
      h6200->SetBinContent(ch-nwires_vsecond+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7200->SetBinContent(ch-nwires_vsecond+1,timeslice);
    }
    // TPC 2 APDs
    if (ch >= napds_first && ch < napds_second){
      h5300->SetBinContent(ch-napds_first+1,RMS);
      h6300->SetBinContent(ch-napds_first+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7300->SetBinContent(ch-napds_first+1,timeslice);
    }
    // TPC 1 U-wires
    if (ch >= 0 && ch < nwires_ufirst){
      h5400->SetBinContent(ch+1,RMS);
      h6400->SetBinContent(ch+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7400->SetBinContent(ch+1,timeslice);
    }
    // TPC 1 V-wires
    if (ch >= nwires_ufirst && ch < nwires_vfirst){
      h5500->SetBinContent(ch-nwires_ufirst+1,RMS);
      h6500->SetBinContent(ch-nwires_ufirst+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7500->SetBinContent(ch-nwires_ufirst+1,timeslice);
    }
    // TPC 2 U-wires
    if (ch >= nwires_vfirst && ch < nwires_usecond){
      h5600->SetBinContent(ch-nwires_vfirst+1,RMS);
      h6600->SetBinContent(ch-nwires_vfirst+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7600->SetBinContent(ch-nwires_vfirst+1,timeslice);
    }
    // TPC 2 V-wires
    if (ch >= nwires_usecond && ch < nwires_vsecond){
      h5700->SetBinContent(ch-nwires_usecond+1,RMS);
      h6700->SetBinContent(ch-nwires_usecond+1,mean);
      if ( itimeslice >= 0 && itimeslice < nsamp ) h7700->SetBinContent(ch-nwires_usecond+1,timeslice);
    }     


      

    
  } // End loop over signals in data array


  if ( print_message ) cout << "ending fill signal histograms" << endl;


}


  
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////AUXILLIARY FUNCTIONS//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void EXOEventDisplayGui::GetNextEvent()
{
  // Request the very next event using dummy values for request_runnumber and request_eventnumber
  RequestEvent(-1,-1);
}

void EXOEventDisplayGui::RequestEvent(int request_runnumber, int request_eventnumber)
{

  //cout << __func__ <<" "<< request_runnumber <<" "<< request_eventnumber <<" "<< eventData <<" "<< ((eventData)?eventData->fEventNumber:0) << endl;
  // If we are looking for a particular event, then check to see if we have found it, 
  static EXOEventData clearedData;
  if ( request_runnumber >= 0 && request_eventnumber >= 0 ) {

    eventData = inputModule->SearchEvent(request_eventnumber);
    /*if ( eventData && eventData->fRunNumber != request_runnumber ) {
        cout << "Wrong run number found (" << eventData->fRunNumber <<
                "), requested: (" << request_runnumber << ")" << endl;
    }*/
  }
  else { // next()
    if (eventData && eventData->fEventNumber==0) // previous failed
      eventData = inputModule->SearchEvent(fRequestEvent->GetIntNumber());
    else
      eventData = inputModule->GetNextEvent();
  }

  if (eventData) {
    // Update number entry fields for run and event numbers.
    //fRequestRun->SetIntNumber( eventData->fRunNumber );
    fRequestEvent->SetIntNumber( eventData->fEventNumber );
    char buf[256]; const char *trig_name[] = { "UNKN", "APD", "VG", "HV" }; int i = 0;
    struct tm tm; time_t tt = eventData->fEventHeader.fTriggerSeconds; localtime_r(&tt,&tm);
    i += snprintf(buf+i,sizeof(buf)-i," -> Run %d, Event %d, Time ",eventData->fRunNumber,eventData->fEventNumber);
    i += strftime(buf+i,sizeof(buf)-i,"%H:%M:%S",&tm);
    i += snprintf(buf+i,sizeof(buf)-i,".%06d",eventData->fEventHeader.fTriggerMicroSeconds);
    i += snprintf(buf+i,sizeof(buf)-i,"      "); // don't ask
    fEventInfo->SetTitle(buf);
    fEventInfo->SetBackgroundColor(TGFrame::GetDefaultFrameBackground());
  } else {
    cout << "File unable to locate (run, event) ("
         << request_runnumber << ", " << request_eventnumber << ") " 
         << "using input module: " << inputModule->GetName() << endl; 
    eventData = &clearedData;
    fEventInfo->SetTitle(" -> Can't find this event");
    fEventInfo->SetBackgroundColor(0x00ff0000);
  }
  //evinfoframe->Layout();
  fRequestFrame->Layout();

  // Fill signal histograms
  imisc = 0;
  superimposebutton->SetOn(false);
  superimpose = 0;
  FillSignalHistograms();

  //update plot when entry number is updated
  DoDraw();

}

void EXOEventDisplayGui::EventUpdate()
{
  int request_runnumber = 0;//fRequestRun->GetIntNumber();
  int request_eventnumber = fRequestEvent->GetIntNumber();
  RequestEvent(request_runnumber, request_eventnumber);
}

void EXOEventDisplayGui::cb_EventNumChanged(Long_t value)
{
  int sign = (value/10000)?-1:1;
  int step = value%100;
  bool logstep = ((value%10000)/100);
  //cout << __func__ <<" "<< sign <<" "<< step <<" "<< logstep <<" "<< fRequestEvent->GetIntNumber() << endl;
  fRequestEventClick = true;
  fRequestEventOld = fRequestEvent->GetIntNumber();
  fRequestEvent->IncreaseNumber((TGNumberEntry::EStepSize)step,sign,logstep);
}

void EXOEventDisplayGui::cb_EventNumSet(Long_t value)
{
  int event = fRequestEvent->GetIntNumber();
  //cout << __func__ <<" "<< value <<" "<< fRequestEvent->GetIntNumber() << endl;
  if (fRequestEventClick) { // next/prev
    if (event==fRequestEventOld+1) GetNextEvent();
    else RequestEvent(0,event);
    fRequestEventClick = false;
  } else { // random
    RequestEvent(0,event);
  }
}

void EXOEventDisplayGui::ChannelUpdate()
{
  RequestChannel(fRequestChannel->GetIntNumber());
}

void EXOEventDisplayGui::TimesliceUpdate()
{
  RequestTimeslice(fRequestTimeslice->GetIntNumber());
}

void EXOEventDisplayGui::ThreshUpdate()
{
  threshold = fRequestThresh->GetIntNumber();
  
}


void EXOEventDisplayGui::ThreshUpdateAndDraw()
{
  threshold = fRequestThresh->GetIntNumber();
  //  cout << "with parameter.... threshold = " << threshold << endl;
  //  cout << "redrawing" << endl;
  DoDraw();
}



void EXOEventDisplayGui::FileDialog()
{
  static TString dir(".");
  const char* filetypes[] = { "ROOT data files", "*.root",
                    "Binary data files", "*.dat",
                    "All files", "*",
                    0, 0 };
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir = StrDup(dir);
  new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
  if (fi.fFilename) { // have a file
    //cout << "file " << fi.fFilename << endl;
    fRequestFilename->SetText(fi.fFilename);
    dir = fi.fIniDir;
  } else if (fi.fFileNamesList) { // have several files
    // fMultipleSelection is just check button
    TIter next(fi.fFileNamesList);
    TObjString *file = 0;
    while ((file=(TObjString*)next())) {
      //cout << "file " << file->GetName() << endl;
      fRequestFilename->SetText(file->GetName());
    }
    dir = fi.fIniDir;
  } else { // it was canceled
    ;
  }
}

void EXOEventDisplayGui::GetDataSource()
{

  // If the user requested a file on the command line, then
  // filename will not be NULL. If filename is NULL, we 
  // attempt to get the name of the file from the filename
  // field in the GUI. 

  if ( filename == "") filename = fRequestFilename->GetText();
  else if (fRequestFilename->GetDisplayText().Length() != 0
           && fRequestFilename->GetText() != filename ) filename = fRequestFilename->GetText();
  // Make sure the filename is diplayed in the GUI
  fRequestFilename->SetText( filename.c_str() );
    
  inputModule = EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile( filename );
  // In EXOAnalysis, we don't want to add histograms to directories; it leads to issues when files are closed.
  // But here, all of the histograms depend on their ability to be found as part of directories;
  // So, we undo the option set in the TreeInput and TreeOutput modules.
  // Note:  I think this means that EXOEventDisplay will fail to open files sequentially.
  TH1::AddDirectory();

  if ( inputModule == NULL ) {
    LogEXOMsg("no input module ", EECritical);
    return;
  }

  eventData = 0;
  GetNextEvent();
}

double EXOEventDisplayGui::GetBaselineCounts(const EXOWaveform& wf)
{
  if ( subtract_baseline == false ) return 0;
  size_t navg = 50;
  // Sum automatically handles if something is out of range.
  return wf.Sum(0, navg)/navg;
}

//void EXOEventDisplayGui::SaveAsGif()
//{
//  fCanvas->SaveAs
// Needs a dialogbox to enter names

EXOEventDisplayGui::~EXOEventDisplayGui() { 
  // Clean up used widgets: frames, buttons, layout hints 
  fMain->Cleanup(); 
  delete fMain; 
} 


