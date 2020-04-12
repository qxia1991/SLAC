// ------------------------------------------------------------------------------------------------
// Class Name:  EXO3DViewer
// Author:      Manuel Weber (manuel.weber@lhep.unibe.ch)
// Description: EXO3DViewer is an event display designed to view events in the real detector
//              geometry in 3D. The user can load EXO ROOT data files and browse through the events
//              one after the other or search for a particular event using the search functions.
//              It has two modes: If the ROOT file contains the waveforms also 2D histograms are
//              plotted and the user can ineractively choose the time sample to be displayed in 3D.
//              Alternatively, or if no waveforms are available, the user can select reconstructed
//              objects to be displayed. Several object informaions will be printed out.

#include "EXO3DViewerGUI.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh" 
#include "EXOUtilities/SystemOfUnits.hh"
#include <algorithm>

EXOEventData *eventData;
EXOInputModule *inputModule;

enum ECommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_URL,
   M_FILE_EXIT,
   M_NEW_GL,
   M_NEW_CANVAS,
   M_NEW_BROWSER,
   M_SAVE_WIRE,
   M_SAVE_APD,
   M_OPTIONS_RESET,
   M_OPTIONS_COLOR,
   M_OPTIONS_GRAY,
   M_OPTIONS_LOG,
   M_OPTIONS_COLORED_WIRES,
   M_OPTIONS_WIRE_HITS,
   CB_UWIRESPZ,
   CB_VWIRESPZ,
   CB_UWIRESNZ,
   CB_VWIRESNZ,
   CB_APDPZ,
   CB_APDNZ,
   CB_VESSEL,
   CB_FIELDRINGS,
   CB_REFLECTOR,
   CB_CATHODE,
   CB_ALL,
   CB_CCL,
   CB_CC,
   CB_SHOW_SAMPLE_INDICATORS,
   CB_MARK_CLUSTER_3D,
   CB_MARK_CLUSTER_ALL,
   CB_SET_LINE_W,
   CB_SET_LINE_APD,
   NE_SAMPLE_WIRE,
   NE_SAMPLE_APD,
   CB_CCLUSTER,
   CB_SCLUSTER,
   NE_EVENT,
   B_GO,
   B_SEARCH,
   B_NEXT,
   B_PREV,
};

EXO3DViewer::EXO3DViewer(const TGWindow *p, UInt_t w, UInt_t h, char *fname) : TGMainFrame(p, w, h)
{
   if (!strcmp(fname,"NoFile")) {filename = NULL;}
   else {filename = fname;}

// **** MENU BAR **********************************************************************************
   // popup menu "File"
   TGPopupMenu *fMenuFile = new TGPopupMenu(gClient->GetRoot());
   TGPopupMenu *fSaveCanvas = new TGPopupMenu(gClient->GetRoot());
   TGPopupMenu *fNew= new TGPopupMenu(gClient->GetRoot());
   fMenuOptions = new TGPopupMenu(gClient->GetRoot());

   // adding menu entries
   fMenuFile->AddEntry("&Open file...",M_FILE_OPEN);
   fMenuFile->AddEntry("&Open URL...",M_FILE_URL);
   fMenuFile->AddPopup("&New",fNew);

   fMenuFile->AddSeparator();

   fMenuFile->AddPopup("&Save canvas",fSaveCanvas);

   fMenuFile->AddSeparator();

   fMenuFile->AddEntry("E&xit",M_FILE_EXIT);

   fMenuFile->Associate(this);

   fNew->AddEntry("GL &Viewer",M_NEW_GL);
   fNew->AddEntry("&Canvas",M_NEW_CANVAS);
   fNew->AddEntry("&Browser",M_NEW_BROWSER);

   fSaveCanvas->AddEntry("&Wire channels...",M_SAVE_WIRE);
   fSaveCanvas->AddEntry("&APD channels...",M_SAVE_APD);

   fMenuOptions->AddEntry("&Reset Histograms",M_OPTIONS_RESET);

   fMenuOptions->AddSeparator();

   fMenuOptions->AddEntry("&Color scale",M_OPTIONS_COLOR);
   fMenuOptions->AddEntry("&Gray scale",M_OPTIONS_GRAY);

   fMenuOptions->AddSeparator();

   fMenuOptions->AddEntry("&Log scale",M_OPTIONS_LOG);

   fMenuOptions->AddSeparator();

   fMenuOptions->AddEntry("Colored wires",M_OPTIONS_COLORED_WIRES);
   fMenuOptions->AddEntry("Show wire hits",M_OPTIONS_WIRE_HITS);

   fMenuOptions->CheckEntry(M_OPTIONS_COLOR);
   fMenuOptions->CheckEntry(M_OPTIONS_COLORED_WIRES);

   fMenuOptions->Associate(this);

   // menu bar
   TGMenuBar *fMenuBar = new TGMenuBar(this,w,20,kHorizontalFrame);

   // adding popup menus
   fMenuBar->AddPopup("&File",fMenuFile,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
   fMenuBar->AddPopup("&Options",fMenuOptions,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

   TGHorizontal3DLine *lh = new TGHorizontal3DLine(this);

   // create horizontal frame containing menu tab and 2D plots
   TGHorizontalFrame *hFrameMain = new TGHorizontalFrame(this);
   hFrameMain->SetLayoutManager(new TGHorizontalLayout(hFrameMain)); // set horizontal arrangement in this frame

// *** Menu Tabs **********************************************************************************
   // create menu tab
   TGTab *fTabMenu = new TGTab(hFrameMain);

   // container for tab "Run control"
   TGCompositeFrame *fCompositeFrameRun;
   fCompositeFrameRun = fTabMenu->AddTab("Run control");
   fCompositeFrameRun->SetLayoutManager(new TGVerticalLayout(fCompositeFrameRun)); // set vertical arrangement within the tab

   // container for tab "Display"
   TGCompositeFrame *fCompositeFrameDisplay;
   fCompositeFrameDisplay = fTabMenu->AddTab("Display");
   fCompositeFrameDisplay->SetLayoutManager(new TGVerticalLayout(fCompositeFrameDisplay)); // set vertical arrangement within the tab
// ************************************************************************************************

// *** 2D Plots and object info tab ***************************************************************
   TGTab *fTabData = new TGTab(hFrameMain);
   
   // container for tab "2D plots"
   TGCompositeFrame *fCompositeFramePlots;
   fCompositeFramePlots = fTabData->AddTab("2D plots");
   fCompositeFramePlots->SetLayoutManager(new TGVerticalLayout(fCompositeFramePlots)); // set vertical arrangement within the tab
   
   // container for tab "Object info"
   TGCompositeFrame *fCompositeFrameObjectInfo;
   fCompositeFrameObjectInfo = fTabData->AddTab("Object info");
   fCompositeFrameObjectInfo->SetLayoutManager(new TGHorizontalLayout(fCompositeFrameObjectInfo)); // set horizontal arrangement within the tab
// ************************************************************************************************

// *** 2D plots ***********************************************************************************
   // create vertical frame for 2D plots
   //TGVerticalFrame *hFramePlots = new TGVerticalFrame(hFrameMain);
   //hFramePlots->SetLayoutManager(new TGVerticalLayout(hFramePlots)); // set vertical arrangement in this frame

   // create vertical frame for nonGL 3D plot (this frame will not be displayed)
   TGVerticalFrame *hFrame3DnonGL = new TGVerticalFrame(hFrameMain);
   hFrame3DnonGL->SetLayoutManager(new TGVerticalLayout(hFrame3DnonGL)); // set vertical arrangement in this frame
// ************************************************************************************************

   // add menu tab and data tab to horizontal frame
   hFrameMain->AddFrame(fTabMenu, new TGLayoutHints(kLHintsExpandY,2,2,2,2));
   hFrameMain->AddFrame(fTabData, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   //hFrameMain->AddFrame(hFramePlots, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));

// **** Content of Run Tab ************************************************************************
   // create sample ind group frame
   TGGroupFrame *GFSampleInd = new TGGroupFrame(fCompositeFrameRun,"Sample Ind");
   GFSampleInd->SetLayoutManager(new TGXYLayout(GFSampleInd));
   GFSampleInd->MoveResize(0,0,180,130);

   // create sample number entry for wires
   fNSampleWire = new TGNumberEntry(GFSampleInd,(int)0,5,NE_SAMPLE_WIRE,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 0, (TGNumberFormat::ELimit) 3,0,2000);
   fNSampleWire->SetNumber(0);
   fNSampleWire->Associate(this);

   // create sample number entry for APD
   fNSampleAPD = new TGNumberEntry(GFSampleInd,(int)0,5,NE_SAMPLE_APD,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 0, (TGNumberFormat::ELimit) 3,0,2000);
   fNSampleAPD->SetNumber(0);
   fNSampleAPD->Associate(this);

   TGLabel *SampleNumberWiresLabel = new TGLabel(GFSampleInd,"Wires");
   SampleNumberWiresLabel->SetTextJustify(kTextLeft);

   TGLabel *SampleNumberAPDLabel = new TGLabel(GFSampleInd,"APD");
   SampleNumberAPDLabel->SetTextJustify(kTextLeft);

   TGLabel *SampleDiffLabel = new TGLabel(GFSampleInd,"Diff:");
   SampleDiffLabel->SetTextJustify(kTextLeft);

   SampleDiffValue = new TGLabel(GFSampleInd,"0");
   SampleDiffValue->SetTextJustify(kTextLeft);

   // show sample indicators
   fShowSampleIndicators = new TGCheckButton(GFSampleInd,"Show sample indicators",CB_SHOW_SAMPLE_INDICATORS);
   fShowSampleIndicators->SetTextJustify(kTextRight);
   fShowSampleIndicators->Associate(this);
   fShowSampleIndicators->SetState(kButtonDown);

   GFSampleInd->AddFrame(SampleNumberWiresLabel, new TGXYLayoutHints(1,2,8,1.8));
   GFSampleInd->AddFrame(fNSampleWire, new TGXYLayoutHints(15,2,8,1.8));
   GFSampleInd->AddFrame(SampleNumberAPDLabel, new TGXYLayoutHints(1,4,8,1.8));
   GFSampleInd->AddFrame(fNSampleAPD, new TGXYLayoutHints(15,4,8,1.8));
   GFSampleInd->AddFrame(SampleDiffLabel, new TGXYLayoutHints(1,6,8,1.8));
   GFSampleInd->AddFrame(SampleDiffValue, new TGXYLayoutHints(15,6,8,1.8));
   GFSampleInd->AddFrame(fShowSampleIndicators, new TGXYLayoutHints(1,8,22,1.8));

   // create cluster selection group frame
   TGGroupFrame *GFCluster = new TGGroupFrame(fCompositeFrameRun,"Cluster Selection");
   GFCluster->SetLayoutManager(new TGXYLayout(GFCluster));
   GFCluster->MoveResize(0,0,180,200);

   TGLabel *SelectChargeClusterLabel = new TGLabel(GFCluster,"Charge:");
   SelectChargeClusterLabel->SetTextJustify(kTextLeft);

   fCBChargeCluster = new TGComboBox(GFCluster,CB_CCLUSTER);
   fCBChargeCluster->Associate(this);
   
   TGLabel *SelectScintClusterLabel = new TGLabel(GFCluster,"Scintillation:");
   SelectScintClusterLabel->SetTextJustify(kTextLeft);
   
   fCBScintCluster = new TGComboBox(GFCluster,CB_SCLUSTER);
   fCBScintCluster->Associate(this);

   // mark 3D cluster
   fCheckMark3DCluster = new TGCheckButton(GFCluster,"Mark 3D cluster",CB_MARK_CLUSTER_3D);
   fCheckMark3DCluster->SetTextJustify(kTextRight);
   fCheckMark3DCluster->Associate(this);
   fCheckMark3DCluster->SetState(kButtonDown);

   // mark all clusters
   fCheckMarkAllCluster = new TGCheckButton(GFCluster,"Mark all clusters",CB_MARK_CLUSTER_ALL);
   fCheckMarkAllCluster->SetTextJustify(kTextRight);
   fCheckMarkAllCluster->Associate(this);
   fCheckMarkAllCluster->SetState(kButtonUp);

   // set sample indicator line on charge cluster
   fCheckSetIndWire = new TGCheckButton(GFCluster,"Set sample ind. (Wires)",CB_SET_LINE_W);
   fCheckSetIndWire->SetTextJustify(kTextRight);
   fCheckSetIndWire->Associate(this);
   fCheckSetIndWire->SetState(kButtonDown);

   // set sample indicator line on scintillation cluster
   fCheckSetIndAPD = new TGCheckButton(GFCluster,"Set sample ind. (APDs)",CB_SET_LINE_APD);
   fCheckSetIndAPD->SetTextJustify(kTextRight);
   fCheckSetIndAPD->Associate(this);
   fCheckSetIndAPD->SetState(kButtonDown);

   GFCluster->AddFrame(SelectChargeClusterLabel, new TGXYLayoutHints(1,2,12,1.8));
   GFCluster->AddFrame(fCBChargeCluster, new TGXYLayoutHints(15,2,8,1.8));
   GFCluster->AddFrame(SelectScintClusterLabel, new TGXYLayoutHints(1,4,12,1.8));
   GFCluster->AddFrame(fCBScintCluster, new TGXYLayoutHints(15,4,8,1.8));
   GFCluster->AddFrame(fCheckMark3DCluster, new TGXYLayoutHints(1,6,20,1.8));
   GFCluster->AddFrame(fCheckMarkAllCluster, new TGXYLayoutHints(1,8,20,1.8));
   GFCluster->AddFrame(fCheckSetIndWire, new TGXYLayoutHints(1,10,22,1.8));
   GFCluster->AddFrame(fCheckSetIndAPD, new TGXYLayoutHints(1,12,22,1.8));

   // create selected cluster group frame
   /*TGGroupFrame *GFSelectedCluster = new TGGroupFrame(fCompositeFrameRun,"Selected Cluster");
   GFSelectedCluster->SetLayoutManager(new TGXYLayout(GFSelectedCluster));
   GFSelectedCluster->MoveResize(0,0,180,120);

   TGLabel *SelClusterLabel[5];
   SelClusterLabel[0] = new TGLabel(GFSelectedCluster,"ncl:");
   SelClusterLabel[1] = new TGLabel(GFSelectedCluster,"tcl:");
   SelClusterLabel[2] = new TGLabel(GFSelectedCluster,"dtcl:");
   SelClusterLabel[3] = new TGLabel(GFSelectedCluster,"pos:");
   SelClusterLabel[4] = new TGLabel(GFSelectedCluster,"eccl:");

   SelClusterVal[0] = new TGLabel(GFSelectedCluster,"");
   SelClusterVal[1] = new TGLabel(GFSelectedCluster,"");
   SelClusterVal[2] = new TGLabel(GFSelectedCluster,"");
   SelClusterVal[3] = new TGLabel(GFSelectedCluster,"");
   SelClusterVal[4] = new TGLabel(GFSelectedCluster,"");

   for (int i = 0; i < 5; i++) {
      SelClusterLabel[i]->SetTextJustify(kTextLeft);
      SelClusterVal[i]->SetTextJustify(kTextLeft);
      GFSelectedCluster->AddFrame(SelClusterLabel[i], new TGXYLayoutHints(1,1.5*(i+1),6,1.2));
      GFSelectedCluster->AddFrame(SelClusterVal[i], new TGXYLayoutHints(6,1.5*(i+1),19,1.2));
   }*/

   // create event selection group frame
   TGGroupFrame *GFEventSelection = new TGGroupFrame(fCompositeFrameRun,"Event Selection");
   GFEventSelection->SetLayoutManager(new TGXYLayout(GFEventSelection));
   GFEventSelection->MoveResize(0,0,180,160);

   // go to entry
   TGLabel *GoToEntryLabel = new TGLabel(GFEventSelection,"Go to entry #: ");
   GoToEntryLabel->SetTextJustify(kTextLeft);

   fGoToTreeEntry = new TGNumberEntryField(GFEventSelection);
   fGoToTreeEntry->SetNumber(0);
   fGoToTreeEntry->MoveResize(50,40,50,22);

   TGTextButton *goToEventButton = new TGTextButton(GFEventSelection,"Go",B_GO);
   goToEventButton->MoveResize(110,40,45,22);
   goToEventButton->Associate(this);

   // search event number
   TGLabel *SearchEventLabel = new TGLabel(GFEventSelection,"Search event #: ");
   SearchEventLabel->SetTextJustify(kTextLeft);

   fGoToEventEntry = new TGNumberEntryField(GFEventSelection);
   fGoToEventEntry->SetNumber(0);
   fGoToEventEntry->MoveResize(50,86,50,22);

   TGTextButton *searchButton = new TGTextButton(GFEventSelection,"Search",B_SEARCH);
   searchButton->MoveResize(110,86,45,22);
   searchButton->Associate(this);

   // next and prevoius buttons
   TGTextButton *NextEventButton = new TGTextButton(GFEventSelection,"Next",B_NEXT);
   TGTextButton *PrevEventButton = new TGTextButton(GFEventSelection,"Prev",B_PREV);

   NextEventButton->Associate(this);
   PrevEventButton->Associate(this);

   NextEventButton->MoveResize(100,120,50,25);
   PrevEventButton->MoveResize(30,120,50,25);

   GFEventSelection->AddFrame(GoToEntryLabel, new TGXYLayoutHints(1,1.5,15,1.8));
   GFEventSelection->AddFrame(SearchEventLabel, new TGXYLayoutHints(1,5.5,15,1.8));

   // add group frames to tab
   fCompositeFrameRun->AddFrame(GFSampleInd, new TGLayoutHints(kLHintsLeft,2,2,2,2));
   fCompositeFrameRun->AddFrame(GFCluster, new TGLayoutHints(kLHintsLeft,2,2,2,2));
   //fCompositeFrameRun->AddFrame(GFSelectedCluster, new TGLayoutHints(kLHintsLeft,2,2,2,2));
   fCompositeFrameRun->AddFrame(GFEventSelection, new TGLayoutHints(kLHintsLeft,2,2,2,2));
// ************************************************************************************************

// **** Content of Display Tab ********************************************************************
   // create detector elements group frame
   TGGroupFrame *GFDetectorElements = new TGGroupFrame(fCompositeFrameDisplay,"Detector elements");
   GFDetectorElements->SetLayoutManager(new TGXYLayout(GFDetectorElements));
   GFDetectorElements->MoveResize(0,0,180,180);

   // U wires +Z
   fCheckUWiresPZ = new TGCheckButton(GFDetectorElements,"U wires +Z",CB_UWIRESPZ);
   fCheckUWiresPZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckUWiresPZ, new TGXYLayoutHints(1,2,20,1.8));
   fCheckUWiresPZ->Associate(this);
   fCheckUWiresPZ->SetState(kButtonDown);

   // V wires +Z
   fCheckVWiresPZ = new TGCheckButton(GFDetectorElements,"V wires +Z",CB_VWIRESPZ);
   fCheckVWiresPZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckVWiresPZ, new TGXYLayoutHints(1,4,20,1.8));
   fCheckVWiresPZ->Associate(this);
   fCheckVWiresPZ->SetState(kButtonDown);

   // U wires -Z
   fCheckUWiresNZ = new TGCheckButton(GFDetectorElements,"U wires -Z",CB_UWIRESNZ);
   fCheckUWiresNZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckUWiresNZ, new TGXYLayoutHints(1,6,20,1.8));
   fCheckUWiresNZ->Associate(this);
   fCheckUWiresNZ->SetState(kButtonDown);

   // V wires -Z
   fCheckVWiresNZ = new TGCheckButton(GFDetectorElements,"V wires -Z",CB_VWIRESNZ);
   fCheckVWiresNZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckVWiresNZ, new TGXYLayoutHints(1,8,20,1.8));
   fCheckVWiresNZ->Associate(this);
   fCheckVWiresNZ->SetState(kButtonDown);

   // APD +Z
   fCheckAPDPZ = new TGCheckButton(GFDetectorElements,"APD +Z",CB_APDPZ);
   fCheckAPDPZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckAPDPZ, new TGXYLayoutHints(1,10,20,1.8));
   fCheckAPDPZ->Associate(this);
   fCheckAPDPZ->SetState(kButtonDown);

   // APD -Z
   fCheckAPDNZ = new TGCheckButton(GFDetectorElements,"APD -Z",CB_APDNZ);
   fCheckAPDNZ->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckAPDNZ, new TGXYLayoutHints(1,12,20,1.8));
   fCheckAPDNZ->Associate(this);
   fCheckAPDNZ->SetState(kButtonDown);
   
   // Vessel
   fCheckVessel = new TGCheckButton(GFDetectorElements,"Vessel",CB_VESSEL);
   fCheckVessel->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckVessel, new TGXYLayoutHints(14,2,11,1.8));
   fCheckVessel->Associate(this);
   fCheckVessel->SetState(kButtonDown);
   
   // Field shaping rings
   fCheckFieldRings = new TGCheckButton(GFDetectorElements,"Field Ring",CB_FIELDRINGS);
   fCheckFieldRings->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckFieldRings, new TGXYLayoutHints(14,4,11,1.8));
   fCheckFieldRings->Associate(this);
   fCheckFieldRings->SetState(kButtonDown);
   
   // Teflon reflector
   fCheckReflector = new TGCheckButton(GFDetectorElements,"Reflector",CB_REFLECTOR);
   fCheckReflector->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckReflector, new TGXYLayoutHints(14,6,11,1.8));
   fCheckReflector->Associate(this);
   fCheckReflector->SetState(kButtonDown);
   
   // Cathode
   fCheckCathode = new TGCheckButton(GFDetectorElements,"Cathode",CB_CATHODE);
   fCheckCathode->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckCathode, new TGXYLayoutHints(14,8,11,1.8));
   fCheckCathode->Associate(this);
   fCheckCathode->SetState(kButtonDown);
   
   // All detector elements
   fCheckAllDetectorElements = new TGCheckButton(GFDetectorElements,"All",CB_ALL);
   fCheckAllDetectorElements->SetTextJustify(kTextRight);
   GFDetectorElements->AddFrame(fCheckAllDetectorElements, new TGXYLayoutHints(14,12,11,1.8));
   fCheckAllDetectorElements->Associate(this);
   fCheckAllDetectorElements->SetState(kButtonUp);

   // create reconstruction group frame
   TGGroupFrame *GFReconstruction = new TGGroupFrame(fCompositeFrameDisplay,"Reconstruction");
   GFReconstruction->SetLayoutManager(new TGXYLayout(GFReconstruction));
   GFReconstruction->MoveResize(0,0,180,90);

   // Charge clusters
   fCheckCCL = new TGCheckButton(GFReconstruction,"Charge Cluster",CB_CCL);
   fCheckCCL->SetTextJustify(kTextRight);
   GFReconstruction->AddFrame(fCheckCCL, new TGXYLayoutHints(2,2,20,1.8));
   fCheckCCL->Associate(this);
   fCheckCCL->SetState(kButtonDown);

   // Compton cone
   fCheckComptCone = new TGCheckButton(GFReconstruction,"Compton Cone",CB_CC);
   fCheckComptCone->SetTextJustify(kTextRight);
   GFReconstruction->AddFrame(fCheckComptCone, new TGXYLayoutHints(2,4,20,1.8));
   fCheckComptCone->Associate(this);
   fCheckComptCone->SetState(kButtonUp);
   fCheckComptCone->SetState(kButtonDisabled);

   // add group frames to tab
   fCompositeFrameDisplay->AddFrame(GFDetectorElements, new TGLayoutHints(kLHintsLeft,2,2,2,2));
   fCompositeFrameDisplay->AddFrame(GFReconstruction, new TGLayoutHints(kLHintsLeft,2,2,2,2));
// ************************************************************************************************

// *** Content of 2D plots tab ********************************************************************
   // create two embedded canvas for wire signals and apd signals
   fCanvasWires = new TRootEmbeddedCanvas("cWires",fCompositeFramePlots);
   fCanvasAPD = new TRootEmbeddedCanvas("cAPD",fCompositeFramePlots);
   fCanvasSingleChannel = new TRootEmbeddedCanvas("cSingle",fCompositeFramePlots,600,100);

   fCompositeFramePlots->AddFrame(fCanvasWires, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   fCompositeFramePlots->AddFrame(fCanvasAPD, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   fCompositeFramePlots->AddFrame(fCanvasSingleChannel, new TGLayoutHints(kLHintsExpandX,2,2,2,2));
// ************************************************************************************************

// *** Content of object info tab *****************************************************************
   fCanvasChargeInfo = new TRootEmbeddedCanvas("cChargeInfo",fCompositeFrameObjectInfo);
   fCanvasScintillationInfo = new TRootEmbeddedCanvas("cScintillationInfo",fCompositeFrameObjectInfo);

   fCompositeFrameObjectInfo->AddFrame(fCanvasChargeInfo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   fCompositeFrameObjectInfo->AddFrame(fCanvasScintillationInfo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
// ************************************************************************************************

// *** Content 3D nonGL frame *********************************************************************
   //fCanvas3DnonGL = new TRootEmbeddedCanvas("c3DnonGL",fCompositeFrame3DnonGL);
   fCanvas3DnonGL = new TRootEmbeddedCanvas("c3DnonGL",hFrame3DnonGL);

   //fCompositeFrame3DnonGL->AddFrame(fCanvas3DnonGL,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   hFrame3DnonGL->AddFrame(fCanvas3DnonGL,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
// ************************************************************************************************

// *** Status Bar *********************************************************************************
   int nrStatParts2 = 8;
   int statPartsSize2[] = {16, 10, 14, 15, 15, 10, 10, 10};
   StatBar = new TGStatusBar(this,w,30);
   StatBar->SetParts(statPartsSize2, nrStatParts2);
// ************************************************************************************************

// **** ADD WIDGETS TO MAINFRAIM ******************************************************************
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
   AddFrame(lh, new TGLayoutHints(kLHintsTop | kLHintsExpandX,0,0,2,2));
   AddFrame(hFrameMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   AddFrame(StatBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
// ************************************************************************************************

   MapSubwindows();

   Layout();

   SetWindowName("EXO3DViewer");
   SetIconName("EXO3DViewer");

   MapWindow();
}

EXO3DViewer::~EXO3DViewer()
{

}

void EXO3DViewer::Init()
{
   gStyle->SetOptStat(0);
   gROOT->SetStyle("Plain");

   const Int_t NRGBs = 6;
   const Int_t NCont = 200;

   Double_t stopsGS[] = { 0.00, 0.20, 0.40, 0.60, 0.80, 1.00};

   Double_t redGS[]   = { 1.00, 0.80, 0.60, 0.40, 0.20, 0.00};
   Double_t greenGS[] = { 1.00, 0.80, 0.60, 0.40, 0.20, 0.00};
   Double_t blueGS[]  = { 1.00, 0.80, 0.60, 0.40, 0.20, 0.00};

   int FIGS = TColor::CreateGradientColorTable(NRGBs, stopsGS, redGS, greenGS, blueGS, NCont);

   colorpaletteGS = new int[NCont];
   for (int i = 0; i < NCont; i++) {colorpaletteGS[i] = FIGS + i;}

   Double_t stops[] = { 0.00, 0.20, 0.40, 0.60, 0.80, 1.00};

   Double_t red[]   = { 0.57, 0.00, 0.00, 0.00, 1.00, 1.00};
   Double_t green[] = { 0.00, 0.00, 1.00, 1.00, 1.00, 0.00};
   Double_t blue[]  = { 0.76, 1.00, 1.00, 0.00, 0.00, 0.00};

   int FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);

   ColorRangeMin = FI;
   ColorRangeMax = FI + NCont - 1;
   colorpalette = new int[NCont];
   for (int i = 0; i < NCont; i++) {colorpalette[i] = FI + i;}

   gStyle->SetPalette(NCont,colorpalette);
   gStyle->SetNumberContours(NCont);

   // create new EXO3DView
   viewer = new EXO3DView();
   
   // initialize canvases
   cWireChannels = fCanvasWires->GetCanvas();
   cAPDChannels = fCanvasAPD->GetCanvas();

   // set canvas properties
   cWireChannels->SetBorderMode(0);
   cWireChannels->SetFillColor(0);
   cAPDChannels->SetBorderMode(0);
   cAPDChannels->SetFillColor(0);

   cWireChannels->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","EXO3DViewer",this,"CursorPos(Int_t,Int_t,Int_t,TObject*)");
   cAPDChannels->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","EXO3DViewer",this,"CursorPos(Int_t,Int_t,Int_t,TObject*)");

   cSingleChannel = fCanvasSingleChannel->GetCanvas();

   cSingleChannel->SetBorderMode(0);
   cSingleChannel->SetFillColor(0);

   cChargeInfo = fCanvasChargeInfo->GetCanvas();
   cScintillationInfo = fCanvasScintillationInfo->GetCanvas();
   
   cChargeInfo->SetBorderMode(0);
   cChargeInfo->SetFillColor(0);
   cScintillationInfo->SetBorderMode(0);
   cScintillationInfo->SetFillColor(0);

   c = fCanvas3DnonGL->GetCanvas();
   viewer->SetCanvas(c);
   viewer->Draw();

   // initialize run and event number
   RunID = 0;
   EventID = 0;

   // initialize histograms
   hWires = NULL;
   hAPD = NULL;

   // initialize sample indicator lines
   posWireSample = NULL;
   posAPDSample = NULL;

   // initialize variables for reconstructed wire hits
   hit_halfwidth = 10.0;
   hit_halfheight = 2.0;
   for ( int j = 0; j < MAXHITS; j++ ) {
      UHit[j].SetLineStyle(1);
      UHit[j].SetLineWidth(2);
      UHit[j].SetFillStyle(0);

      VHit[j].SetLineStyle(1);
      VHit[j].SetLineWidth(2);
      VHit[j].SetFillStyle(0);
  }

   // if a file was specified get data source and load first event
   if (filename != NULL) {
      GetDataSource();
      GoToEvent(0,kTRUE);

      // set initial value of number entry
      int SliderInitVal = int(nrSample / 2);
      fNSampleWire->SetNumber(SliderInitVal);
      fNSampleAPD->SetNumber(SliderInitVal);
   }
   else {
      StatBar->AddText("No file opened",0);
   }
}

void EXO3DViewer::OpenFile()
{
   string InputFile;
   TGFileInfo fi;
   fi.fIniDir = StrDup(".");
   const char * filetypes[] = { "Root file", "*.root", "Binary file", "*.bin", "All files", "*", 0, 0};
   fi.fFileTypes = filetypes;
   new TGFileDialog(gClient->GetRoot(), 0, kFDOpen, &fi);

   if(fi.fFilename == NULL) {
      StatBar->AddText("No file opened",0);
   }
   else {
      filename = fi.fFilename;
      GetDataSource();
      GoToEvent(0,kTRUE);

      // set initial value of number entry
      int SliderInitVal = int(nrSample / 2);
      fNSampleWire->SetNumber(SliderInitVal);
      fNSampleAPD->SetNumber(SliderInitVal);
   }

   return;
}

URLDialog::URLDialog(const TGWindow *p, const TGWindow *main, TString *_result)
{
  fMain = new TGTransientFrame(p, main, 300, 400, kVerticalFrame);
  fMain->Connect("CloseWindow()", "URLDialog", this, "CloseDialog()");
  fMain->DontCallClose(); // to avoid double deletion
  fMain->SetCleanup(kDeepCleanup); // clean hierarchical

  TGHorizontalFrame *hframe;

  fMain->AddFrame(new TGLabel(fMain,"Server:"),
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 5, 0));
  hframe = new TGHorizontalFrame(fMain);
  hframe->AddFrame(new TGLabel(hframe,""),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 5, 3, 0));
  fSrv = new TGTextEntry(hframe, "root://exo-rdr.slac.stanford.edu");
  fSrv->SetWidth(300);
  hframe->AddFrame(fSrv,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));
  fMain->AddFrame(hframe,
		  new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 5, 5, 2, 0));
  fAuth = new TGCheckButton(fMain, "Authenticate");
  fAuth->SetEnabled(false);
  //fAuth->SetState(false);
  fMain->AddFrame(fAuth,
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 15, 5, 2, 0));
  hframe = new TGHorizontalFrame(fMain);
  hframe->AddFrame(new TGLabel(hframe,"User"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fUser = new TGTextEntry(hframe, "");
  fUser->SetWidth(100);
  fUser->SetEnabled(false);
  hframe->AddFrame(fUser,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0));
  hframe->AddFrame(new TGLabel(hframe,"Pass"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fPass = new TGTextEntry(hframe, "");
  fPass->SetWidth(100);
  fPass->SetEnabled(false);
  hframe->AddFrame(fPass,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 15, 0, 0, 0));
  fMain->AddFrame(hframe,
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 2, 0));
  fMain->AddFrame(new TGHorizontal3DLine(fMain),
		  new TGLayoutHints(kLHintsExpandX | kLHintsTop, 0, 0, 2, 2));

  fMain->AddFrame(new TGLabel(fMain,"Standard path:"),
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 5, 0));
  hframe = new TGHorizontalFrame(fMain);
  hframe->AddFrame(new TGLabel(hframe,"Run type"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fType = new TGComboBox(hframe);
  //fType->AddEntry("dat",0);
  //fType->AddEntry("cxd",1);
  fType->AddEntry("run",2);
  fType->AddEntry("proc",3);
  fType->AddEntry("recon",4);
  fType->AddEntry("masked",5);
  fType->Resize(70,fUser->GetHeight());
  hframe->AddFrame(fType,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0));
  hframe->AddFrame(new TGLabel(hframe,"Run number"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fRunI = new TGNumberEntryField(hframe, -1, 0, TGNumberFormat::kNESInteger);
  fRunI->SetWidth(50);
  hframe->AddFrame(fRunI,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0));
  hframe->AddFrame(new TGLabel(hframe,"File number"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fRunJ = new TGNumberEntryField(hframe, -1, 0, TGNumberFormat::kNESInteger);
  fRunJ->SetWidth(50);
  hframe->AddFrame(fRunJ,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0));
  fMain->AddFrame(hframe,
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 2, 0));
  fMain->AddFrame(new TGHorizontal3DLine(fMain),
		  new TGLayoutHints(kLHintsExpandX | kLHintsTop, 0, 0, 2, 2));

  fMain->AddFrame(new TGLabel(fMain,"Manual path:"),
		  new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 5, 0));
  hframe = new TGHorizontalFrame(fMain);
  hframe->AddFrame(new TGLabel(hframe,"Path"),
		   new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 5, 3, 0));
  fPath = new TGTextEntry(hframe, "");
  //fPath->SetWidth(300);
  hframe->AddFrame(fPath,
		   new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));
  fMain->AddFrame(hframe,
		  new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 5, 5, 2, 0));
  fMain->AddFrame(new TGHorizontal3DLine(fMain),
		  new TGLayoutHints(kLHintsExpandX | kLHintsTop, 0, 0, 2, 2));

  hframe = new TGHorizontalFrame(fMain);
  fOk = new TGTextButton(hframe, "&Ok", 1);
  fOk->ChangeOptions(fOk->GetOptions() | kFixedWidth);
  fOk->Connect("Clicked()", "URLDialog", this, "HandleButton()");
  hframe->AddFrame(fOk, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 0, 0));
  fCancel = new TGTextButton(hframe, "&Cancel", 2);
  fCancel->ChangeOptions(fOk->GetOptions() | kFixedWidth);
  fCancel->Connect("Clicked()", "URLDialog", this, "CloseDialog()");
  hframe->AddFrame(fCancel, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 0, 0));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsRight | kLHintsTop, 2, 2, 5, 5));

  // I know how to make use of tab, but too lazy to do that now.
  // Also I hope we could benefit from REST interface here
  //   and that someone knows how to do that.

  result = _result;
  if (result) result->Clear();

  fMain->MapSubwindows();
  fMain->Resize();
  // now black magic
  int bw = 3*fCancel->GetWidth()/2; fCancel->SetWidth(bw); fOk->SetWidth(bw); fMain->Resize();
  fMain->CenterOnParent();
  fMain->SetWindowName("URL open");
  fMain->MapWindow();
  gClient->WaitFor(fMain); // be modal
}
void URLDialog::HandleButton()
{
  if (fPath->GetText()[0]!='\0') {
    (*result) += fSrv->GetText();
    (*result) += fPath->GetText();
    //(*result) = "root://exo-rdr.slac.stanford.edu//exo_data/data/WIPP/root/3219/run00003219-000.root";
  } else {
    switch (fType->GetSelected()) {
    case 0: // dat
      break;
    case 1: // cxd
      result->Form("%s//exo_data/data/WIPP/cxd/%ld/run%08ld-%03ld.cxd",fSrv->GetText(),fRunI->GetIntNumber(),fRunI->GetIntNumber(),fRunJ->GetIntNumber());
      break;
    case 2: // run
      result->Form("%s//exo_data/data/WIPP/root/%ld/run%08ld-%03ld.root",fSrv->GetText(),fRunI->GetIntNumber(),fRunI->GetIntNumber(),fRunJ->GetIntNumber());
      break;
    case 3: // proc
      result->Form("%s//exo_data/data/WIPP/processed/%ld/proc%08ld-%03ld.root",fSrv->GetText(),fRunI->GetIntNumber(),fRunI->GetIntNumber(),fRunJ->GetIntNumber());
      break;
    case 4: // recon
      result->Form("%s//exo_data/data/WIPP/recon/%ld/recon%08ld-%03ld.root",fSrv->GetText(),fRunI->GetIntNumber(),fRunI->GetIntNumber(),fRunJ->GetIntNumber());
      break;
    case 5: // masked
      result->Form("%s//exo_data/data/WIPP/masked/%ld/masked%08ld-%03ld.root",fSrv->GetText(),fRunI->GetIntNumber(),fRunI->GetIntNumber(),fRunJ->GetIntNumber());
      break;
    default:
      ;
    }
  }
  CloseDialog();
}
void EXO3DViewer::OpenURL()
{
  TString URL;
  new URLDialog(gClient->GetRoot(), this, &URL);
  cout << __func__ <<" "<< URL << endl;
  if(URL=="") {
    StatBar->AddText("No file opened",0);
  } else {
    filename = new char[URL.Length()+1];
    strcpy(filename,URL.Data());
    GetDataSource();
    GoToEvent(0,kTRUE);

    // set initial value of number entry
    int SliderInitVal = int(nrSample / 2);
    fNSampleWire->SetNumber(SliderInitVal);
    fNSampleAPD->SetNumber(SliderInitVal);
  }
  return;
}

void EXO3DViewer::GetDataSource()
{
  inputModule = EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile( filename );

  if ( inputModule == NULL ) {
    LogEXOMsg("no input module ", EECritical);
    return;
  }

  // Update status bar
  string fnameSplit;
  string fnameTMP = string(filename);
  int cutAt = fnameTMP.find_last_of("/");
  fnameSplit = fnameTMP.substr(cutAt+1);
  if (fnameSplit.size() == 0) {fnameSplit = fnameTMP;}
  StatBar->AddText(fnameSplit.c_str(),0);

  return;
}

void EXO3DViewer::GoToEvent(int opt, bool init)
{
   // Make sure that we have an input module
   static EXOEventData clearedEventData;

   if ( inputModule == NULL ) {
     LogEXOMsg("no input module ", EECritical);
     return;
   }

   // Clear eventData.

   int evtID = 0;
   bool result = false;

   if (init) opt = 2;
   
   switch (opt) {
      case 0: // go to specific tree entry
         evtID = int(fGoToTreeEntry->GetNumber());
         eventData = inputModule->GetEvent(evtID);
      break;
      case 1: // search for specific event number
         evtID = int(fGoToEventEntry->GetNumber());
         eventData = inputModule->SearchEvent(evtID);
      break;
      case 2: // get next event
         eventData = inputModule->GetNext();
      break;
      case 3: // get precious event
         eventData = inputModule->GetPrevious();
      break;
   }

   if (!eventData) {
      cout << "Unable to find event " << evtID << " using input module: " 
           << inputModule->GetName() << endl;
      eventData = &clearedEventData;
      return;
   }
   //just to make sure all event related code gets run
   //call begin and end of event functions
   //inputModule->BeginOfEvent(eventData);
   //inputModule->EndOfEvent(eventData);

   RunID = eventData->fRunNumber;
   EventID = eventData->fEventNumber;
   nrSample = eventData->GetWaveformData()->fNumSamples;
   nrSig = eventData->GetWaveformData()->GetNumWaveforms();
   if (nrSig == 0) {HasWaveforms = false; std::cout << "No waveforms in this file!" << std::endl;}
   else {HasWaveforms = true;}

   // set range of number entry
   fNSampleWire->SetLimitValues(0,nrSample);
   fNSampleAPD->SetLimitValues(0,nrSample);

   // update run and event number
   char RunNumber[50];
   char EventNumber[50];
   sprintf(RunNumber,"Run: %i", RunID);
   sprintf(EventNumber,"Event: %i", EventID);
   StatBar->AddText(RunNumber,1);
   StatBar->AddText(EventNumber,2);

   if (HasWaveforms) {FillHistograms();}
   CreateChargeCluster();
   SetObjectInfo();
   if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),int(fNSampleAPD->GetNumber()));}
   if (fCheckMarkAllCluster->GetState() == kButtonDown) {DrawReconstructedWireHits(kTRUE);}

   return;
}

void EXO3DViewer::FillHistograms()
{
   if (hWires != NULL) {hWires->Delete();}
   if (hAPD != NULL) {hAPD->Delete();}

   viewer->FillWaveformHist(eventData);
   
   hWires = viewer->GetWireHist();
   hAPD = viewer->GetAPDHist();
   
   hWires->GetXaxis()->SetTitle("time [#mus]");
   hWires->GetYaxis()->SetTitle("wire channel");
   
   hAPD->GetXaxis()->SetTitle("time [#mus]");
   hAPD->GetYaxis()->SetTitle("APD channel");

   cWireChannels->Clear();
   cAPDChannels->Clear();

   cWireChannels->cd();
   hWires->Draw("colz");

   cAPDChannels->cd();
   hAPD->Draw("colz");

   cWireChannels->Update();
   cAPDChannels->Update();

   return;
}

void EXO3DViewer::SetCurrentData()
{
  int sWireID = int(fNSampleWire->GetNumber());
  int sAPDID = int(fNSampleAPD->GetNumber());
  
  viewer->SetWireWaveformSample(eventData,sWireID);
  viewer->SetAPDWaveformSample(eventData,sAPDID);

  return;
}

void EXO3DViewer::CreateChargeCluster()
{
  // Draw the current event
  viewer->Draw(eventData);
  
  // Remove all clusters from select boxes
  fCBChargeCluster->RemoveAll();
  fCBScintCluster->RemoveAll();
  
  // Add the charge and scintillation cluster to the select box
  int nChargeCluster = eventData->GetNumChargeClusters();
  // add cluster to select box
  char CCEntryLabel[50];
  for (int i = 0; i < nChargeCluster; i++) {
     sprintf(CCEntryLabel,"#%i",i);
     fCBChargeCluster->AddEntry(CCEntryLabel,i);
  }
  
  int nScitnillationCluster = eventData->GetNumScintillationClusters();
  // add scintillation clusters to select box
  char SCEntryLabel[50];
  for (int i = 0; i < nScitnillationCluster; i++) {
     sprintf(SCEntryLabel,"#%i",i);
     fCBScintCluster->AddEntry(SCEntryLabel,i);
  }
  
  if (nChargeCluster == 0) {fCBChargeCluster->SetEnabled(kFALSE);}
  else {fCBChargeCluster->SetEnabled(kTRUE);}
  
  if (nScitnillationCluster == 0) {fCBScintCluster->SetEnabled(kFALSE);}
  else {fCBScintCluster->SetEnabled(kTRUE);}

  return;
}

void EXO3DViewer::DrawReconstructedWireHits(bool draw)
{
  cWireChannels->cd();

  if (!draw) {
     cWireChannels->Clear();
     hWires->Draw("colz");
     SetSampleInd(int(fNSampleWire->GetNumber()),-1);
  }
  else {
     double x1 = hWires->GetXaxis()->GetBinLowEdge(hWires->GetXaxis()->GetFirst());
     double x2 = hWires->GetXaxis()->GetBinLowEdge(hWires->GetXaxis()->GetLast());
     double y1 = hWires->GetYaxis()->GetBinLowEdge(hWires->GetYaxis()->GetFirst());
     double y2 = hWires->GetYaxis()->GetBinLowEdge(hWires->GetYaxis()->GetLast());

     double xtweak = 8.0;
     double x1hit = 0.0;
     double x2hit = 0.0;
     for ( unsigned int j = 0; j < eventData->GetNumChargeClusters(); j++ ) {
        if ( j >= MAXHITS ) break;

        EXOChargeCluster* charge_cluster = eventData->GetChargeCluster(j); 
        UHit[j].SetX1(x1);
        UHit[j].SetX2(x2);

        x1hit = charge_cluster->fCollectionTime/1000.0 - hit_halfwidth + xtweak;
        x2hit = charge_cluster->fCollectionTime/1000.0 + hit_halfwidth + xtweak;

        if ( x1hit > x1 ) UHit[j].SetX1(x1hit);
        if ( x2hit < x2 ) UHit[j].SetX2(x2hit);

        UHit[j].SetY1(charge_cluster->GetUWireSignalChannelAt(0) + 0.5 - hit_halfheight);
        UHit[j].SetY2(charge_cluster->GetUWireSignalChannelAt(0) + 0.5 + hit_halfheight);

        UHit[j].SetLineColor(j%10);

        if ( UHit[j].GetX2() > x1 && UHit[j].GetX1() < x2 && UHit[j].GetY1() < y2 && UHit[j].GetY2() > y1 ) {UHit[j].Draw("same");}

        if ( charge_cluster->GetNumVWireSignals() <= 0 ) continue;

        VHit[j].SetX1(x1);
        VHit[j].SetX2(x2);

        x1hit = charge_cluster->fCollectionTime/1000.0 - hit_halfwidth + xtweak;
        x2hit = charge_cluster->fCollectionTime/1000.0 + hit_halfwidth + xtweak;

        if ( x1hit > x1 ) VHit[j].SetX1(x1hit);
        if ( x2hit < x2 ) VHit[j].SetX2(x2hit);

        VHit[j].SetY1(charge_cluster->GetVWireSignalChannelAt(0)+0.5 - hit_halfheight);
        VHit[j].SetY2(charge_cluster->GetVWireSignalChannelAt(0)+0.5 + hit_halfheight);

        VHit[j].SetLineColor(j%10);

        if ( VHit[j].GetX2() > x1 && VHit[j].GetX1() < x2 && VHit[j].GetY1() < y2 && VHit[j].GetY2() > y1) {VHit[j].Draw("same");}
      }
  }
  cWireChannels->Update();

  return;
}

void EXO3DViewer::SetSampleInd(int posWire, int posAPD)
{
   int nwires = NCHANNEL_PER_WIREPLANE*NWIREPLANE;
   int napd = NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;

   if (posWire != -1) {
      if (posWireSample != NULL) {posWireSample->Delete();}
      posWireSample = new TLine(posWire,0,posWire,nwires);
      posWireSample->SetLineColor(kRed);
      cWireChannels->cd();
      posWireSample->Draw("same");
      cWireChannels->Update();
   }

   if (posAPD != -1) {
      if (posAPDSample != NULL) {posAPDSample->Delete();}
      posAPDSample = new TLine(posAPD,0,posAPD,napd);
      posAPDSample->SetLineColor(kRed);
      cAPDChannels->cd();
      posAPDSample->Draw("same");
      cAPDChannels->Update();
   }

   return;
}

void EXO3DViewer::CursorPos(Int_t event, Int_t x, Int_t y, TObject *selected)
{
   int CurrentHisto = 0;
   if (!selected) {return;}
   if (!selected->InheritsFrom("TH2")) {gPad->Update(); gPad->SetUniqueID(0); return;}
   TH2 *h = (TH2*)selected;

   if (!strcmp(h->GetName(),"hWire")) {gPad = cWireChannels; CurrentHisto = 1;}
   if (!strcmp(h->GetName(),"hAPD")) {gPad = cAPDChannels; CurrentHisto = 2;}
   gPad->GetCanvas()->FeedbackMode(kTRUE);

   int pxold = gPad->GetUniqueID();
   float uymin = gPad->GetUymin();
   float uymax = gPad->GetUymax();
   int pymin = gPad->YtoAbsPixel(uymin);
   int pymax = gPad->YtoAbsPixel(uymax);
   float upx = gPad->AbsPixeltoX(x);
   float upy = gPad->AbsPixeltoY(y);
   float px = gPad->PadtoX(upx);
   float py = gPad->PadtoY(upy);
   int binX = h->GetXaxis()->FindBin(px);
   int binY = h->GetYaxis()->FindBin(py);

   if (event == kButton1Down) {
      if (CurrentHisto == 1) {
         fNSampleWire->SetNumber(binX);
         viewer->SetWireWaveformSample(eventData,binX);
      }
      else {if (CurrentHisto == 2) {
         fNSampleAPD->SetNumber(binX);
         viewer->SetAPDWaveformSample(eventData,binX);
         }
      }

      gPad->Update();
      gPad->SetUniqueID(0);
   }
   else {
      if(pxold) gVirtualX->DrawLine(pxold,pymin,pxold,pymax);
      gVirtualX->DrawLine(x,pymin,x,pymax);
      gPad->SetUniqueID(x);

      // get x projection of current bin
      cSingleChannel->cd();
      cSingleChannel->Clear();
      TH1D *hp = h->ProjectionX("",binY,binY);
      hp->SetStats(0);
      hp->Draw();
      cSingleChannel->Update();

      // set current sample number and bin content in status bar
      char CurrentChannel[50];
      char CurrentSample[50];
      char CurrentBinContent[50];
      if (CurrentHisto == 1) {sprintf(CurrentChannel,"ch: %i",binY-1);}
      else {if (CurrentHisto == 2) {sprintf(CurrentChannel,"ch: %i",binY+APD_CHANNEL0-1);}}
      sprintf(CurrentSample,"Sample: %i",binX);
      sprintf(CurrentBinContent,"Bin: %i",int(h->GetBinContent(binX,binY)));
      StatBar->AddText(CurrentChannel,5);
      StatBar->AddText(CurrentSample,6);
      StatBar->AddText(CurrentBinContent,7);
   }

   return;
}

void EXO3DViewer::SetObjectInfo()
{
   int CCID = fCBChargeCluster->GetSelected();
   int SCID = fCBScintCluster->GetSelected();
   
   TText *ChargeClusterInfoQuantity[20];
   TText *ChargeClusterInfoValues[20];
   TText *ScintClusterInfoQuantity[20];
   TText *ScintClusterInfoValues[20];
   double step = 0.05;
   double xPos = 0.02;
   double xPos2 = 0.6;
   double xPos3 = 0.7;
   
   if (CCID >= 0) {
      EXOChargeCluster* charge_cluster = eventData->GetChargeCluster(CCID);

      ChargeClusterInfoQuantity[0] = new TText(xPos,1-step,"fCollectionTime:");
      ChargeClusterInfoQuantity[1] = new TText(xPos,1-2*step,"fCorrectedEnergy:");
      ChargeClusterInfoQuantity[2] = new TText(xPos,1-3*step,"fCorrectedEnergyError:");
      ChargeClusterInfoQuantity[3] = new TText(xPos,1-4*step,"fDetectorHalf:");
      ChargeClusterInfoQuantity[4] = new TText(xPos,1-5*step,"fDriftTime:");
      ChargeClusterInfoQuantity[5] = new TText(xPos,1-6*step,"fEnergyInVChannels:");
      ChargeClusterInfoQuantity[6] = new TText(xPos,1-7*step,"fIs3DCluster:");
      ChargeClusterInfoQuantity[7] = new TText(xPos,1-8*step,"fIsFiducial:");
      ChargeClusterInfoQuantity[8] = new TText(xPos,1-9*step,"fPurityCorrectedEnergy:");
      ChargeClusterInfoQuantity[9] = new TText(xPos,1-10*step,"fRawEnergy:");
      ChargeClusterInfoQuantity[10] = new TText(xPos,1-11*step,"fRawEnergyError:");
      ChargeClusterInfoQuantity[11] = new TText(xPos,1-12*step,"fU:");
      ChargeClusterInfoQuantity[12] = new TText(xPos,1-13*step,"fV:");
      ChargeClusterInfoQuantity[13] = new TText(xPos,1-14*step,"fX:");
      ChargeClusterInfoQuantity[14] = new TText(xPos,1-15*step,"fY:");
      ChargeClusterInfoQuantity[15] = new TText(xPos,1-16*step,"fZ:");
      
      char value[50];
      sprintf(value,"%.2f ns",charge_cluster->fCollectionTime);
      ChargeClusterInfoValues[0] = new TText(xPos2,1-step,value);
      sprintf(value,"%.2f keV",charge_cluster->fCorrectedEnergy);
      ChargeClusterInfoValues[1] = new TText(xPos2,1-2*step,value);
      sprintf(value,"%.2f keV",charge_cluster->fCorrectedEnergyError);
      ChargeClusterInfoValues[2] = new TText(xPos2,1-3*step,value);
      sprintf(value,"%i",charge_cluster->fDetectorHalf);
      ChargeClusterInfoValues[3] = new TText(xPos2,1-4*step,value);
      sprintf(value,"%.2f ns",charge_cluster->fDriftTime);
      ChargeClusterInfoValues[4] = new TText(xPos2,1-5*step,value);
      sprintf(value,"%.2f",charge_cluster->fEnergyInVChannels);
      ChargeClusterInfoValues[5] = new TText(xPos2,1-6*step,value);
      sprintf(value,"%i",charge_cluster->Is3DCluster());
      ChargeClusterInfoValues[6] = new TText(xPos2,1-7*step,value);
      sprintf(value,"%i",charge_cluster->IsFiducial());
      ChargeClusterInfoValues[7] = new TText(xPos2,1-8*step,value);
      sprintf(value,"%.2f keV",charge_cluster->fPurityCorrectedEnergy);
      ChargeClusterInfoValues[8] = new TText(xPos2,1-9*step,value);
      sprintf(value,"%.2f keV",charge_cluster->fRawEnergy);
      ChargeClusterInfoValues[9] = new TText(xPos2,1-10*step,value);
      sprintf(value,"%.2f keV",charge_cluster->fRawEnergyError);
      ChargeClusterInfoValues[10] = new TText(xPos2,1-11*step,value);
      sprintf(value,"%.2f mm",charge_cluster->fU);
      ChargeClusterInfoValues[11] = new TText(xPos2,1-12*step,value);
      sprintf(value,"%.2f mm",charge_cluster->fV);
      ChargeClusterInfoValues[12] = new TText(xPos2,1-13*step,value);
      sprintf(value,"%.2f mm",charge_cluster->fX);
      ChargeClusterInfoValues[13] = new TText(xPos2,1-14*step,value);
      sprintf(value,"%.2f mm",charge_cluster->fY);
      ChargeClusterInfoValues[14] = new TText(xPos2,1-15*step,value);
      sprintf(value,"%.2f mm",charge_cluster->fZ);
      ChargeClusterInfoValues[15] = new TText(xPos2,1-16*step,value);
   }
   
   cChargeInfo->cd();
   cChargeInfo->Clear();

   if (CCID >= 0) {
      for (int i = 0; i < 19; i ++) {
         ChargeClusterInfoQuantity[i]->SetTextSize(0.04);
         ChargeClusterInfoQuantity[i]->Draw();
         
         ChargeClusterInfoValues[i]->SetTextSize(0.04);
         ChargeClusterInfoValues[i]->Draw();
      }
   }

   cChargeInfo->Update();

   if (SCID >= 0) {
      EXOScintillationCluster *scint_cluster = eventData->GetScintillationCluster(SCID);
      
      ScintClusterInfoQuantity[0] = new TText(xPos,1-step,"fCountsOnAPDPlaneOne:");
      ScintClusterInfoQuantity[1] = new TText(xPos,1-2*step,"fCountsOnAPDPlaneOneError:");
      ScintClusterInfoQuantity[2] = new TText(xPos,1-3*step,"fCountsOnAPDPlaneTwo:");
      ScintClusterInfoQuantity[3] = new TText(xPos,1-4*step,"fCountsOnAPDPlaneTwoError:");
      ScintClusterInfoQuantity[4] = new TText(xPos,1-5*step,"fCountsSumOnAPDPlaneOne:");
      ScintClusterInfoQuantity[5] = new TText(xPos,1-6*step,"fCountsSumOnAPDPlaneOneError:");
      ScintClusterInfoQuantity[6] = new TText(xPos,1-7*step,"fCountsSumOnAPDPlaneTwo:");
      ScintClusterInfoQuantity[7] = new TText(xPos,1-8*step,"fCountsSumOnAPDPlaneTwoError:");
      ScintClusterInfoQuantity[8] = new TText(xPos,1-9*step,"fX:");
      ScintClusterInfoQuantity[9] = new TText(xPos,1-10*step,"fY:");
      ScintClusterInfoQuantity[10] = new TText(xPos,1-11*step,"fZ:");
      ScintClusterInfoQuantity[11] = new TText(xPos,1-12*step,"fTime:");
      ScintClusterInfoQuantity[12] = new TText(xPos,1-13*step,"fRadius:");
      ScintClusterInfoQuantity[13] = new TText(xPos,1-14*step,"fTheta:");
      ScintClusterInfoQuantity[14] = new TText(xPos,1-15*step,"fEnergy:");
      ScintClusterInfoQuantity[15] = new TText(xPos,1-16*step,"fRawEnergy:");
      ScintClusterInfoQuantity[16] = new TText(xPos,1-17*step,"fSumCounts:");
      ScintClusterInfoQuantity[17] = new TText(xPos,1-18*step,"fAlgorithmUsed:");
      ScintClusterInfoQuantity[18] = new TText(xPos,1-19*step,"Nr associated charge clusters:"); 
      
      char value[50];
      sprintf(value,"%.2f",scint_cluster->GetCountsOnAPDPlane(EXOMiscUtil::kNorth));
      ScintClusterInfoValues[0] = new TText(xPos3,1-step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth));
      ScintClusterInfoValues[1] = new TText(xPos3,1-2*step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsOnAPDPlane(EXOMiscUtil::kSouth));
      ScintClusterInfoValues[2] = new TText(xPos3,1-3*step,value);
      sprintf(value,"%.2F",scint_cluster->GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth));
      ScintClusterInfoValues[3] = new TText(xPos3,1-4*step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth));
      ScintClusterInfoValues[4] = new TText(xPos3,1-5*step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth));
      ScintClusterInfoValues[5] = new TText(xPos3,1-6*step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth));
      ScintClusterInfoValues[6] = new TText(xPos3,1-7*step,value);
      sprintf(value,"%.2f",scint_cluster->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth));
      ScintClusterInfoValues[7] = new TText(xPos3,1-8*step,value);
      sprintf(value,"%.2f mm",scint_cluster->fX);
      ScintClusterInfoValues[8] = new TText(xPos3,1-9*step,value);
      sprintf(value,"%.2f mm",scint_cluster->fY);
      ScintClusterInfoValues[9] = new TText(xPos3,1-10*step,value);
      sprintf(value,"%.2f mm",scint_cluster->fZ);
      ScintClusterInfoValues[10] = new TText(xPos3,1-11*step,value);
      sprintf(value,"%.2f ns",scint_cluster->fTime);
      ScintClusterInfoValues[11] = new TText(xPos3,1-12*step,value);
//      sprintf(value,"%.2f mm",scint_cluster->fRadius);
      ScintClusterInfoValues[12] = new TText(xPos3,1-13*step,value);
//      sprintf(value,"%.2f",scint_cluster->fTheta);
      ScintClusterInfoValues[13] = new TText(xPos3,1-14*step,value);
      sprintf(value,"%.2f keV",scint_cluster->fEnergy);
      ScintClusterInfoValues[14] = new TText(xPos3,1-15*step,value);
      sprintf(value,"%.2f keV",scint_cluster->fRawEnergy);
      ScintClusterInfoValues[15] = new TText(xPos3,1-16*step,value);
      // FixME, should remove fully this ScintClusterInfoQuantity.
      sprintf(value,"%.2f",0.0);
      ScintClusterInfoValues[16] = new TText(xPos3,1-17*step,value);
      sprintf(value,"%i",scint_cluster->GetAlgorithmUsed());
      ScintClusterInfoValues[17] = new TText(xPos3,1-18*step,value);
      sprintf(value,"%i",int(scint_cluster->GetNumChargeClusters()));
      ScintClusterInfoValues[18] = new TText(xPos3,1-19*step,value);
   }
   
   cScintillationInfo->cd();
   cScintillationInfo->Clear();

   if (SCID >= 0) {
      for (int i = 0; i < 19; i ++) {
         ScintClusterInfoQuantity[i]->SetTextSize(0.04);
         ScintClusterInfoQuantity[i]->Draw();
         
         ScintClusterInfoValues[i]->SetTextSize(0.04);
         ScintClusterInfoValues[i]->Draw();
      }
   }

   cScintillationInfo->Update();
   
   return;
}

void EXO3DViewer::SaveCanvas(int canvasID)
{
  TGFileInfo fi;
  fi.fIniDir = StrDup(".");
  const char * filetypes[] = { "eps", "*.eps", "gif", "*.gif", "png", "*.png", "ps", "*.ps", "root", "*root", "All files", "*", 0, 0};
  fi.fFileTypes = filetypes;
  new TGFileDialog(gClient->GetRoot(), 0, kFDSave, &fi);
  if (fi.fFilename) {
     string FileName = fi.fFilename;
     const char *filetype = fi.fFileTypes[fi.fFileTypeIdx];
     size_t pos = FileName.find_last_of(".");
     string ext =  FileName.substr(pos + 1);
     if (ext == filetype) {
        if (canvasID == 1) {cWireChannels->SaveAs(fi.fFilename);}
        if (canvasID == 2) {cAPDChannels->SaveAs(fi.fFilename);}
     }
     else {
        char fname[50];
        sprintf(fname, "%s.%s", fi.fFilename, filetype);
        if (canvasID == 1) {cWireChannels->SaveAs(fname);}
        if (canvasID == 2) {cAPDChannels->SaveAs(fname);}
     }
  }

  return;
}

Bool_t EXO3DViewer::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
     case kC_COMMAND:
        switch (GET_SUBMSG(msg)) {
           case kCM_CHECKBUTTON:
              switch (parm1) {
                 case CB_UWIRESPZ:
                    SetCurrentData();
                 break;
                 case CB_VWIRESPZ:
                    SetCurrentData();
                 break;
                 case CB_UWIRESNZ:
                    SetCurrentData();
                 break;
                 case CB_VWIRESNZ:
                    SetCurrentData();
                 break;
                 case CB_APDPZ:
                    SetCurrentData();
                 break;
                 case CB_APDNZ:
                    SetCurrentData();
                 break;
                 case CB_VESSEL:
                    if (fCheckVessel->GetState() == kButtonDown) {viewer->SetVesselInvisible(false);}
                    else {viewer->SetVesselInvisible(true);}
                 break;
                 case CB_FIELDRINGS:
                    if (fCheckFieldRings->GetState() == kButtonDown) {viewer->SetFieldRingInvisible(false);}
                    else {viewer->SetFieldRingInvisible(true);}
                 break;
                 case CB_REFLECTOR:
                    if (fCheckReflector->GetState() == kButtonDown) {viewer->SetReflectorInvisible(false);}
                    else {viewer->SetReflectorInvisible(true);}
                 break;
                 case CB_CATHODE:
                    if (fCheckCathode->GetState() == kButtonDown) {viewer->SetCathodeInvisible(false);}
                    else {viewer->SetCathodeInvisible(true);}
                 break;
                 case CB_ALL:
                    if (fCheckAllDetectorElements->GetState() == kButtonDown) {viewer->SetInvisibleAll(false);}
                    else {viewer->SetInvisibleAll(true);}
                 break;
                 case CB_SHOW_SAMPLE_INDICATORS:
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),int(fNSampleAPD->GetNumber()));}
                    else {
                       if (posWireSample != NULL) {
                          cWireChannels->cd();
                          posWireSample->Delete();
                          posWireSample = NULL;
                          cWireChannels->Update();
                       }
                       if (posAPDSample != NULL) {
                          cAPDChannels->cd();
                          posAPDSample->Delete();
                          posAPDSample = NULL;
                          cAPDChannels->Update();
                       }
                    }
                 break;
                 case CB_CCL:
                   /* c->cd();
                    if (fCheckCCL->GetState() == kButtonDown && nVisibleChargeClusters > 0) {ChargeClusterVolume->InvisibleAll(kFALSE);}
                    else {ChargeClusterVolume->InvisibleAll(kTRUE);}
                    for (int i = 0; i < ChargeClusters->GetEntriesFast(); i++) {
                       if (fCheckCCL->GetState() == kButtonDown) {((TGeoVolume*)(ChargeClusters->At(i)))->SetVisibility(kTRUE);}
                       else {((TGeoVolume*)(ChargeClusters->At(i)))->SetVisibility(kFALSE);}
                    }
                    c->Update();
                 break;*/
                 case CB_MARK_CLUSTER_ALL:
                    if (fCheckMarkAllCluster->GetState() == kButtonDown) {DrawReconstructedWireHits(kTRUE);}
                    else {DrawReconstructedWireHits(kFALSE);}
                 break;
              }
           break;

           case kCM_BUTTON:
              switch (parm1) {
                 case B_GO:
                    GoToEvent(0,kFALSE);
                 break;
                 case B_SEARCH:
                    GoToEvent(1,kFALSE);
                 break;
                 case B_NEXT:
                    GoToEvent(2,kFALSE);
                 break;
                 case B_PREV:
                    GoToEvent(3,kFALSE);
                 break;
              }
           break;

           case kCM_MENU:
              switch (parm1) {
                 case M_FILE_OPEN:
                    OpenFile();
                 break;
                 case M_FILE_URL:
                    OpenURL();
                 break;
                 case M_FILE_EXIT:
                    gApplication->Terminate(0);
                 break;
                 case M_NEW_CANVAS:
                    new TCanvas();
                 break;
                 case M_NEW_BROWSER:
                    new TBrowser();
                 break;
                 case M_NEW_GL:
                    viewer->Draw();
                 break;
                 case M_SAVE_WIRE:
                    SaveCanvas(1);
                 break;
                 case M_SAVE_APD:
                    SaveCanvas(2);
                 break;
                 case M_OPTIONS_RESET:
                    FillHistograms();
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),int(fNSampleAPD->GetNumber()));}
                    if (fCheckMarkAllCluster->GetState() == kButtonDown) {DrawReconstructedWireHits(kTRUE);}
                 break;
                 case M_OPTIONS_COLOR:
                    fMenuOptions->CheckEntry(M_OPTIONS_COLOR);
                    fMenuOptions->UnCheckEntry(M_OPTIONS_GRAY);

                    gStyle->SetPalette(200,colorpalette);
                    gStyle->SetNumberContours(200);

                    FillHistograms();
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),int(fNSampleAPD->GetNumber()));}
                    if (fCheckMarkAllCluster->GetState() == kButtonDown) {DrawReconstructedWireHits(kTRUE);}
                 break;
                 case M_OPTIONS_GRAY:
                    fMenuOptions->UnCheckEntry(M_OPTIONS_COLOR);
                    fMenuOptions->CheckEntry(M_OPTIONS_GRAY);

                    gStyle->SetPalette(200,colorpaletteGS);
                    gStyle->SetNumberContours(200);

                    FillHistograms();
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),int(fNSampleAPD->GetNumber()));}
                    if (fCheckMarkAllCluster->GetState() == kButtonDown) {DrawReconstructedWireHits(kTRUE);}
                 break;
                 case M_OPTIONS_LOG:
                    if (fMenuOptions->IsEntryChecked(M_OPTIONS_LOG)) {
                       fMenuOptions->UnCheckEntry(M_OPTIONS_LOG);
                       cWireChannels->SetLogz(0);
                       cAPDChannels->SetLogz(0);

                       cWireChannels->Update();
                       cAPDChannels->Update();
                    }
                    else {
                       fMenuOptions->CheckEntry(M_OPTIONS_LOG);
                       cWireChannels->SetLogz(1);
                       cAPDChannels->SetLogz(1);

                       cWireChannels->Update();
                       cAPDChannels->Update();
                    }
                 break;
                 case M_OPTIONS_COLORED_WIRES:
                    fMenuOptions->UnCheckEntry(M_OPTIONS_WIRE_HITS);
                    fMenuOptions->CheckEntry(M_OPTIONS_COLORED_WIRES);
                 break;
                 case M_OPTIONS_WIRE_HITS:
                    fMenuOptions->UnCheckEntry(M_OPTIONS_COLORED_WIRES);
                    fMenuOptions->CheckEntry(M_OPTIONS_WIRE_HITS);
                 break;
              }
           break;

           case kCM_COMBOBOX:
              switch (parm1) {
                 case CB_CCLUSTER:
                    {
                    int ccID = fCBChargeCluster->GetSelected();
                    int nScint = fCBScintCluster->GetNumberOfEntries();
                    
                    // search for associated scintillation cluster ID
                    int ScintAssocID = -1;
                    double ThisScintClusterTime = eventData->GetChargeCluster(ccID)->GetScintillationCluster()->fTime;
                    if (ThisScintClusterTime > 0) {
                       for (int i = 0; i < nScint; i++) {
                          double ScintClusterTime = eventData->GetScintillationCluster(i)->fTime;
                          if (ThisScintClusterTime == ScintClusterTime) {ScintAssocID = i; break;}
                       }
                    }

                    fCBScintCluster->Select(ScintAssocID);
                    
                    viewer->SelectChargeCluster(ccID);
                    
                    int WireSample = int(eventData->GetChargeCluster(ccID)->fCollectionTime / 1000);
                    int APDSample = -1;
                    if (ScintAssocID >= 0) {APDSample = int(eventData->GetScintillationCluster(ScintAssocID)->fTime / 1000);}
                    if (fCheckSetIndWire->GetState() == kButtonDown) {fNSampleWire->SetNumber(WireSample);}
                    if (fCheckSetIndAPD->GetState() == kButtonDown) {fNSampleAPD->SetNumber(APDSample);}
                    
                    SetObjectInfo();
                    }
                 break;
                 case CB_SCLUSTER:
                    if (fCBChargeCluster->GetNumberOfEntries() == 1) {fCBChargeCluster->Select(0);}
                    else {fCBChargeCluster->Select(-1);}
                    
                    int scID = fCBScintCluster->GetSelected();
                    viewer->SelectScintillationCluster(scID);
                    
                    int WireSample = -1;
                    int APDSample = int(eventData->GetScintillationCluster(scID)->fTime / 1000);
                    if (fCheckSetIndWire->GetState() == kButtonDown) {fNSampleWire->SetNumber(WireSample);}
                    if (fCheckSetIndAPD->GetState() == kButtonDown) {fNSampleAPD->SetNumber(APDSample);}
                    
                    SetObjectInfo();
                 break;
              }
           break;
        }
     break;

     case kC_TEXTENTRY:
        switch (GET_SUBMSG(msg)) {
           case kTE_TEXTCHANGED:
              switch (parm1) {
                 char SelectedSample[50];
                 char Diff[50];
                 case NE_SAMPLE_WIRE:
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(int(fNSampleWire->GetNumber()),-1);}
                    sprintf(SelectedSample,"Wire sample: %i",int(fNSampleWire->GetNumber()));
                    StatBar->AddText(SelectedSample,3);
                    sprintf(Diff,"%i",TMath::Abs(int(fNSampleWire->GetNumber()) - int(fNSampleAPD->GetNumber())));
                    SampleDiffValue->SetText(Diff);
                 break;
                 case NE_SAMPLE_APD:
                    if (fShowSampleIndicators->GetState() == kButtonDown) {SetSampleInd(-1,int(fNSampleAPD->GetNumber()));}
                    sprintf(SelectedSample,"APD sample: %i",int(fNSampleAPD->GetNumber()));
                    StatBar->AddText(SelectedSample,4);
                    sprintf(Diff,"%i",TMath::Abs(int(fNSampleWire->GetNumber()) - int(fNSampleAPD->GetNumber())));
                    SampleDiffValue->SetText(Diff);
                 break;
              }
           break;
        }
     break;
  }

  return kTRUE;
}

int main(int argc, char **argv)
{
   char fname[200];

   if (argc == 2) {strncpy(fname,argv[1],199);}
   else {strncpy(fname,"NoFile",199);}

   TApplication theApp("App", &argc, argv);
   EXO3DViewer *EV = new EXO3DViewer(gClient->GetRoot(), 800, 670, fname);
   EV->Init();
   theApp.Run();

   return 0;
}

