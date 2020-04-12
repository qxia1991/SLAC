// ------------------------------------------------------------------------------------------------
// EXO3DView
//
// This class allows drawing the basic detector geometry and provides tools to visualize events
// in 3D. The dimensions for the visible detector parts are read from EXODimensions. All 3D objects
// are drawn in a TGLViewer.
//
// Detector elements which are part of the event visualization are defined as active elements.
// These are:
//
//   U,V wires negative z half
//   U,V wires positive z half
//   APDs negative z half
//   APDs positive z half
//   The bulk of the detector containing the charge clusters
//
// All other elements are for geometry visualization purposes and are defined inactive.
// These include:
//
//   XeVessel
//   Cathode
//   FieldShapingRings
//   Reflector
//
// Both the active and inactive objects can be set visible or invisible by calling the appropriate
// functions (see function descriptions). Properties like transparency or color of the detector
// elements can be changed by getting a pointer to the corresponding objects. For instance
// GetVesselVolume() will return the TGeoVolume of the vessel. In order to see inside the detector,
// the inactive detector elements can be clipped along a plane by calling Clip(int plane) where plane
// can be
//
//   1  : negative y half
//   2  : positive x half
//   3  : positive y half
//   4  : negative x half
//
// NoClipping() is the same as Clip(5). Note that the TGLViewer provides clipping functionalities
// as well. But there all volumes are clipped.
//
// For the visualization of events there are several tools. The most simple and straightforward one
// is
//
//   Draw(EXOEventData *ED)
//
// This will display all charge clusters for the current event as blue boxes. After the event is
// drawn, the visible clusters can be selected by calling
//
//   SelectChargeCluster(int ccID)
//
// This highlights the correspondig box as well as the hit wires and sets the APD data of the
// associated scintillation cluster. Analog a scintillation cluster can be selected which sets the
// APD data and marks all associated charge clusters.
// If you want to draw a specific charge or scintillation cluster you can just call
//
//   Draw(EXOChargeCluster *chargeCluster)      or
//   Draw(EXOScintillationCluster *scintCluster)
//
// which draws the specified charge / scintillation cluster and it's associated objects. The hit wires
// are highlighted. If you want to draw multiple charge clusters, call
//
//   AddData(EXOChargeCluster *chargeCluster, int col)
//
// which will not draw the event immediately but waits for the Draw() command. This allows adding
// several charge clusters from different events with separate colors.
//
// Data can also be stacked which is analog to a TH3. Adding data to the stack is done by calling
//
//   StackData(EXOEventData *ED);               or
//   StackData(EXOChargeCluster *CC);           or
//   StackData(EXOScintillationCluster *SC);
//
// The stacked data can then be drawn by calling
//
//   DrawStackedData()
//
// The detector volume will be split into 1cm3 bins and and a box is drawn for each bin with a
// size proportional to the bin content. The color of the boxes is as well set accoring to the 
// bin content. For the APDs, the color is set according to the summed counts in each of the gangs.
//
// Below are some examples which point out the different features. Before using the class in a
// script or directly from the ROOT command line make sure to load the EXOUtilities library
//
//   gSystem->Load("libEXOUtilities.so");
//
// Example 1:
//
// Let's just draw the detector geometry with no events and change some display properties
//
//   EXO3DView *My3DView = new EXO3DView();                     // create new EXO3DView
//   My3DView->Draw();                                          // draw the geometry
//
//   My3DView->SetVesselInvisible();                            // we don't want to see the vessel
//   My3DView->GetReflectorVolume()->SetTransparency(20);       // set the reflecor transparency to 20%
//
// Example 2:
//
// Let's assume we have a run in which we would like to visually separate charge clusters above a
// certain energy threshold. This could be done like this
//
//   TFile *f = new TFile("MyFile.root","READ");                // open the run
//   TTree *t = (TTree*)f->Get("tree");                         // get the tree
//
//   EXOEventData *ED = 0;                                      // get handle on the event data
//   t->SetBranchAddress("EventBranch",&ED);                    // set the branch address
//
//   EXO3DView *My3DView = new EXO3DView();                     // create new EXO3DView
//
//   double thresh = 1000.0                                     // energy threshold
//   for (int i = 0; i < t->GetEntries(); i++) {                // loop over all events in tree
//     t->GetEntry(i);
//
//     for (int k = 0; k < ED->GetNumChargeClusters(); k++) {   // loop over all charge clusters in this event
//       EXOChargeCluster *charge_cluster = ED->GetChargeCluster(k);
//
//       if (charge_cluster->fCorrectedEnergy > thresh) {My3DView->AddData(charge_cluster, kRed);}
//       else {My3DView->AddData(charge_cluster, kBlue);}
//     }
//   }
//
//   My3DView->Draw();                                          // Draw the data
//
// Example 3:
//
// We have a source calibration run and would like to draw the positions of the charge clusters
// analog to a 3D histogram and the sum of the APD gang counts. This is done by stacking the
// events and draw the stacked data
//
//   TFile *f = new TFile("MyFile.root","READ");                // open the run
//   TTree *t = (TTree*)f->Get("tree");                         // get the tree
//
//   EXOEventData *ED = 0;                                      // get handle on the event data
//   t->SetBranchAddress("EventBranch",&ED);                    // set the branch address
//
//   EXO3DView *My3DView = new EXO3DView();                     // create new EXO3DView
//
//   for (int i = 0; i < t->GetEntries(); i++) {                // loop over all events in tree
//     t->GetEntry(i);
//
//     My3DView->StackData(ED);                                 // add the event to the stack
//   }
//
//   My3DView->DrawStackedData();                               // draw the stacked data
#ifdef BUILD_VIEWER3D
#include "EXOUtilities/EXO3DView.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TColor.h"
#include "TGLViewer.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TGeoPgon.h"
#include <sstream>
#include <set>

#define APD_CHANNEL0 (152)
#define cos30 (0.866035)
#define tan30 (0.577350)

using namespace std;

EXO3DView::EXO3DView()
{
  Init();
}

EXO3DView::~EXO3DView() {}

void EXO3DView::Init()
{
   // Generates the geometry and initializes variables and histograms
   
   gStyle->SetOptStat(0);
   gROOT->SetStyle("Plain");

   // create color palette for waveform plots
   const Int_t NRGBs = 6;
   NCont = 200;

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

   // APD geometry
   double xAPDNewTMP[74] = {-3.327, 2.218, 7.764, -2.218, 3.327, -1.109, 13.309, 14.418, 15.527, 8.873, 9.982, 4.436, 16.636, 12.2, 7.764, 11.091, 6.655, 5.545, 3.327, -2.218, -7.764, 2.218, -3.327, 1.109, -13.309, -14.418, -15.527, -8.873, -9.982, -4.436, 0, -16.636, -12.2, -7.764, -11.091, -6.655, -5.545, 3.327, -2.218, -7.764, 2.218, -3.327, 1.109, -13.309, -14.418, -15.527, -8.873, -9.982, -4.436, 0, -16.636, -12.2, -7.764, -11.091, -6.655, -5.545, -3.327, 2.218, 7.764, -2.218, 3.327, -1.109, 13.309, 14.418, 15.527, 8.873, 9.982, 4.436, 16.636, 12.2, 7.764, 11.091, 6.655, 5.545};
   double yAPDNewTMP[74] = {17.289, 15.368, 13.447, 11.526, 9.605, 5.763, 11.526, 5.763, 0, 7.684, 1.921, 3.842, -5.763, -9.605, -13.447, -3.842, -7.684, -1.921, -17.289, -15.368, -13.447, -11.526, -9.605, -5.763, -11.526, -5.763, 0, -7.684, -1.921, -3.842, 0, 5.763, 9.605, 13.447, 3.842, 7.684, 1.921, -17.289, -15.368, -13.447, -11.526, -9.605, -5.763, -11.526, -5.763, 0, -7.684, -1.921, -3.842, 0, 5.763, 9.605, 13.447, 3.842, 7.684, 1.921, 17.289, 15.368, 13.447, 11.526, 9.605, 5.763, 11.526, 5.763, 0, 7.684, 1.921, 3.842, -5.763, -9.605, -13.447, -3.842, -7.684, -1.921};
   int APDGangTable[74] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
   int APDChannelTable[74] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73};
   
   xAPDNew = xAPDNewTMP;
   yAPDNew = yAPDNewTMP;
   
   // create new TGeoManager
   mgr = new TGeoManager("GM","GeoManager");

   // create detecotor mediums
   CreateDetectorMedium();
   
   // create top volume
   TopVolume = mgr->MakeBox("TOP",0,50*CLHEP::cm,50*CLHEP::cm,50*CLHEP::cm);
   mgr->SetTopVolume(TopVolume);
   
   // create wire volumes
   WireVolumePZ = mgr->MakeBox("UWVPZ",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);
   WireVolumeNZ = mgr->MakeBox("UWVNZ",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);
   
   // create APD volumes
   APDVolumePZ = mgr->MakeBox("APDVPZ",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,2*CLHEP::mm);
   APDVolumeNZ = mgr->MakeBox("APDVNZ",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,2*CLHEP::mm);

   // create charge cluster volume
   ChargeClusterVolume = mgr->MakeBox("CLV",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,CATHODE_ANODE_DISTANCE);
   PCDVolume = mgr->MakeBox("PCDV",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,CATHODE_ANODE_DISTANCE);

   CreateBaseDetectorElements();
   CreateActiveDetectorElements();
   
   // create rotation matrices for APD planes
   TGeoRotation *RAPDPZ = new TGeoRotation("RAPDPZ",0,0,-10.7);
   TGeoRotation *RAPDNZ = new TGeoRotation("RAPDNZ",0,180,-10.7);
   
   // add wire and APD volumes at positive z to top volume
   TopVolume->AddNode(WireVolumePZ,1,new TGeoTranslation("WiresVPZ",0,0,CATHODE_WIRESUPPORT_DISTANCE));
   TopVolume->AddNode(APDVolumePZ,1,new TGeoCombiTrans("APDVPZ",0,0,CATHODE_WIRESUPPORT_DISTANCE + WIRESUPPORT_THICKNESS + APDFRAME_WIRESUPPORT_DISTANCE + 1*CLHEP::mm,RAPDPZ));
   
   // add wire and APD volumes at negative z to top volume
   TopVolume->AddNode(WireVolumeNZ,1,new TGeoTranslation("WiresVNZ",0,0,-CATHODE_WIRESUPPORT_DISTANCE));
   TopVolume->AddNode(APDVolumeNZ,1,new TGeoCombiTrans("APDVNdZ",0,0,-1*(CATHODE_WIRESUPPORT_DISTANCE + WIRESUPPORT_THICKNESS + APDFRAME_WIRESUPPORT_DISTANCE + 1*CLHEP::mm),RAPDNZ));

   // add charge cluster volume to top volume
   TopVolume->AddNode(ChargeClusterVolume,1);
   TopVolume->AddNode(PCDVolume,1);

   mgr->SetNsegments(200);
   mgr->CloseGeometry();
   
   // initialize histograms for stacked events
   StackedEvents = new TH3D("StackedEvents","Stacked charge clusters",40,-200,200,40,-200,200,40,-200,200);
   StackedAPD = new TH1D("StackedAPD","Stacked APD gangs",74,0,74);
   
   Clear();
   
   // set default value for stacked histogram depth
   StackedHistogramDepth = 0.0;
   
   // initialize current histogram event number
   CurrentHistEvent = -1;
   
   // initialize the non GL canvas
   NonGLCanvas = 0;
   
   return;
}

void EXO3DView::AddData(EXOEventData *ED, int col)
{
   // This adds all charge clusters in the current event to the cluster volume. If col is specified,
   // the boxes will have the corresponding color. The event will not be drawn immediately to allow
   // adding several events. Call Draw() to display all added events.
   // The charge clusters are removed from the volume by calling Clear();
   
   CreateChargeCluster(ED, col);
   
   return;
}

void EXO3DView::AddData(EXOChargeCluster *chargeCluster, int col)
{
   // This adds a specific charge cluster to the cluster volume. If col is specified, the drawn box
   // will have the corresponding color. The event will not be drawn immediately to allow adding
   // several events. Call Draw() to display all added events.
   // The charge clusters are removed from the volume by calling Clear();
   
   ChargeClusters = new TObjArray();
   ChargeClusters->Expand(1);
  
   // create box for current charge cluster
   char ClusterName[50];
   if (TMath::Abs(chargeCluster->fX) < 250 && TMath::Abs(chargeCluster->fY) < 250 && TMath::Abs(chargeCluster->fZ) < 250) { // only keep clusters within world coordinates
      sprintf(ClusterName,"CCL_%i",nVisibleChargeClusters);
      TGeoVolume *CurrentCluster = mgr->MakeBox(ClusterName,0,5*CLHEP::mm,5*CLHEP::mm,5*CLHEP::mm);
        
      CurrentCluster->SetLineColor(col);
      ChargeClusters->AddAt(CurrentCluster,0);
      ChargeClusterVolume->AddNode(CurrentCluster,1,new TGeoTranslation(chargeCluster->fX, chargeCluster->fY, chargeCluster->fZ));

      nVisibleChargeClusters++;
   }
   else {ChargeClusters->AddAt(NULL,0);}
  
   if (nVisibleChargeClusters == 0) {ChargeClusterVolume->InvisibleAll(kTRUE);}
   else {ChargeClusterVolume->InvisibleAll(kFALSE);}
   
   return;
}

void EXO3DView::Draw()
{
   // Draws the current geometry including the charge cluster volume. Properties of the different
   // detector elements can be changed before or after the drawing.
   
   // reset the non GL canvas if it is not found
   TObject *cGL = gROOT->FindObject("cNonGL");
   if (!cGL) {NonGLCanvas = 0;}
   
   // create new GL viewer if canvas does not exist
   // just do an update otherwise
   if (!NonGLCanvas) {
      NonGLCanvas = new TCanvas("cNonGL","non GL canvas");
      
      try {TopVolume->Draw("ogl");}
      catch (std::exception& e) {
         std::cout << "Exception caught: " << e.what() << std::endl;
         std::cout << "Type " << typeid(e).name() << std::endl;
      }
      
      v = (TGLViewer*)gPad->GetViewer3D();
      v->SetResetCamerasOnUpdate(kFALSE);
   }
   else {
      if (!v) {
         try {TopVolume->Draw("ogl");}
         catch (std::exception& e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
            std::cout << "Type " << typeid(e).name() << std::endl;
         }
         v = (TGLViewer*)gPad->GetViewer3D();
         v->SetResetCamerasOnUpdate(kFALSE);
      }
      NonGLCanvas->Modified();
      NonGLCanvas->Update();
   }
   
   return;
}

void EXO3DView::Draw(EXOEventData *ED)
{
   // Draws the detector geometry and the event specified. The charge cluster volume is cleared before
   // the drawing. If you want to draw multiple events or charge clusters from different events, call
   // AddData() and do a Draw() afterwards.
   
   // remove all charge clusters
   Clear();
   
   // add all charge clusters from current event to the cluster volume
   CreateChargeCluster(ED);
   CurrentEventData = ED;
   
   Draw();
   
   return;
}

void EXO3DView::Draw(EXOChargeCluster *chargeCluster)
{
   // Draws the specified charge cluster and its associated scintillation cluster. The hit wires
   // are highlighted.
   
   // remove all charge clusters and reset APDs
   Clear();
   
   ChargeClusters = new TObjArray();
   ChargeClusters->Expand(1);
  
   // create box for current charge cluster
   char ClusterName[50];
   if (TMath::Abs(chargeCluster->fX) < 250 && TMath::Abs(chargeCluster->fY) < 250 && TMath::Abs(chargeCluster->fZ) < 250) { // only keep clusters within world coordinates
      sprintf(ClusterName,"CCL_%i",nVisibleChargeClusters);
      TGeoVolume *CurrentCluster = mgr->MakeBox(ClusterName,0,5*CLHEP::mm,5*CLHEP::mm,5*CLHEP::mm);
        
      CurrentCluster->SetLineColor(kBlue);
      ChargeClusters->AddAt(CurrentCluster,0);
      ChargeClusterVolume->AddNode(CurrentCluster,1,new TGeoTranslation(chargeCluster->fX, chargeCluster->fY, chargeCluster->fZ));

      nVisibleChargeClusters++;
   }
   else {ChargeClusters->AddAt(NULL,0);}
  
   if (nVisibleChargeClusters == 0) {ChargeClusterVolume->InvisibleAll(kTRUE);}
   else {ChargeClusterVolume->InvisibleAll(kFALSE);}
   
   // get the associated scintillation cluster
   EXOScintillationCluster *scintCluster = chargeCluster->GetScintillationCluster();
   if (!scintCluster) {std::cout << "This charge cluster has no associated scintillation cluster" << std::endl; return;}
   
   TList listOfGangs;
   scintCluster->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
   TIter next(&listOfGangs);
   const EXOAPDSignal* sig;
   
   // get min and max of the APD data
   double minAPD = 1000000;
   double maxAPD = -1000000;
   double counts = 0.0;
   while ((sig = (const EXOAPDSignal*)next())) {   
      counts = sig->fCounts;
      if (counts > maxAPD) {maxAPD = counts;}
      if (counts < minAPD) {minAPD = counts;}
   }
   
   // set the APD data for each gang
   int ColorID = 0;
   int cl = 0;
   for (int i = 0; i < 74; i++) {
      const EXOAPDSignal* sig = scintCluster->GetGangSignal(i);
      if (!sig) continue;
      counts = sig->fCounts;
      
      // get the color ID of the current gang
      ColorID = int((counts - minAPD) / (maxAPD - minAPD) * (NCont-1));
      cl = colorpalette[ColorID];
      
      ((TGeoVolume*)(APDGANG->At(i)))->SetLineColor(cl);
   }
   
   // mark the hit wires (first reset al wires)
   for (int i = 0; i < NCHANNEL_PER_WIREPLANE; i++) {
      ((TGeoVolume*)(uWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(uWiresNZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresNZ->At(i)))->SetLineColor(12);
   }
   
   int nUWires = chargeCluster->GetNumUWireSignals();
   for (int i = 0; i < nUWires; i++) {
      int UWireCHID = chargeCluster->GetUWireSignalAt(i)->fChannel;
      
      if (UWireCHID >= 0 && UWireCHID < NCHANNEL_PER_WIREPLANE) {
         int chID = UWireCHID;
         ((TGeoVolume*)(uWiresPZ->At(chID)))->SetLineColor(2);
      }
      
      if (UWireCHID >= 2*NCHANNEL_PER_WIREPLANE && UWireCHID < 3*NCHANNEL_PER_WIREPLANE) {
         int chID = UWireCHID - 2*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(uWiresNZ->At(chID)))->SetLineColor(2);
      }
   }

   int nVWires = chargeCluster->GetNumVWireSignals();
   for (int i = 0; i < nVWires; i++) {
      int VWireCHID = chargeCluster->GetVWireSignalAt(i)->fChannel;
      
      if (VWireCHID >= NCHANNEL_PER_WIREPLANE && VWireCHID < 2*NCHANNEL_PER_WIREPLANE) {
         int chID = VWireCHID - NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(vWiresPZ->At(chID)))->SetLineColor(2);
      }
      if (VWireCHID >= 3*NCHANNEL_PER_WIREPLANE && VWireCHID < 4*NCHANNEL_PER_WIREPLANE) {
         int chID = VWireCHID - 3*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(vWiresNZ->At(chID)))->SetLineColor(2);
      }
   }
  
   Draw();
   
   return;
}

void EXO3DView::Draw(EXOMCPixelatedChargeDeposit *pcd)
{
   // Draws the specified pixelated charge deposit. The affected wires
   // are highlighted.
   
   // remove all charge clusters and pcds and reset APDs
   Clear();

   EXOCoordinates coord = pcd->GetPixelCenter();
   
   // create box for pcd
   stringstream Name;
   if (TMath::Abs(coord.GetX()) < 250 && TMath::Abs(coord.GetY()) < 250 && TMath::Abs(coord.GetZ()) < 250) { // only keep clusters within world coordinates
      Name << "PCD_" << nVisiblePCDs;
      TGeoVolume *CurrentPCD = mgr->MakeBox(Name.str().c_str(),0,5*CLHEP::mm,5*CLHEP::mm,5*CLHEP::mm);
        
      CurrentPCD->SetLineColor(kBlue);
      PCDVolume->AddNode(CurrentPCD,1,new TGeoTranslation(coord.GetX(), coord.GetY(), coord.GetZ()));

      nVisiblePCDs++;
   }
  
   if (nVisiblePCDs == 0) {PCDVolume->InvisibleAll(kTRUE);}
   else {PCDVolume->InvisibleAll(kFALSE);}
   
   // mark the affected wires (first reset al wires)
   for (int i = 0; i < 4*NCHANNEL_PER_WIREPLANE; i++) {
     SetWireColor(i,12);
   }
   
   for(set<int>::iterator iter=pcd->fWireChannelsAffected.begin(); iter != pcd->fWireChannelsAffected.end(); iter++){
     SetWireColor(*iter,kGreen);
   }
   SetWireColor(pcd->fDepositChannel,kRed);
  
   Draw();
   
   return;
}

void EXO3DView::SetWireColor(int chan, int col)
{
  if(chan < 0 or chan >= 4*NCHANNEL_PER_WIREPLANE){
    return;
  }
  int whichplane = chan/NCHANNEL_PER_WIREPLANE;
  switch(whichplane){
    case 0:
      ((TGeoVolume*)(uWiresPZ->At(chan%NCHANNEL_PER_WIREPLANE)))->SetLineColor(col);
      break;
    case 1:
      ((TGeoVolume*)(vWiresPZ->At(chan%NCHANNEL_PER_WIREPLANE)))->SetLineColor(col);
      break;
    case 2:
      ((TGeoVolume*)(uWiresNZ->At(chan%NCHANNEL_PER_WIREPLANE)))->SetLineColor(col);
      break;
    case 3:
      ((TGeoVolume*)(vWiresNZ->At(chan%NCHANNEL_PER_WIREPLANE)))->SetLineColor(col);
      break;
  }
}

void EXO3DView::Draw(EXOScintillationCluster *scintCluster)
{
   // Draws the specified scintillation cluster and its associated charge clusters.
   
   // remove all charge clusters and reset APDs
   Clear();
   
   TList listOfGangs;
   scintCluster->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
   TIter next(&listOfGangs);
   const EXOAPDSignal* sig;
   
   // get min and max of the APD data
   double minAPD = 1000000;
   double maxAPD = -1000000;
   double counts = 0.0;
   while ((sig = (const EXOAPDSignal*)next())) { 
      counts = sig->fCounts;
      if (counts > maxAPD) {maxAPD = counts;}
      if (counts < minAPD) {minAPD = counts;}
   }
   
   // set the APD data for each gang
   int ColorID = 0;
   int cl = 0;
   for (int i = 0; i < 74; i++) {
      const EXOAPDSignal* sig = scintCluster->GetGangSignal(i);
      if (!sig) continue;
      counts = sig->fCounts;
      
      // get the color ID of the current gang
      ColorID = int((counts - minAPD) / (maxAPD - minAPD) * (NCont-1));
      cl = colorpalette[ColorID];
      
      ((TGeoVolume*)(APDGANG->At(i)))->SetLineColor(cl);
   }
   
   // get all associated charge clusters
   int nAssocChargeClusters = scintCluster->GetNumChargeClusters();
   
   ChargeClusters = new TObjArray();
   ChargeClusters->Expand(nAssocChargeClusters);
  
   // create box for each of the charge clusters
   char ClusterName[50];
   for (int i = 0; i < nAssocChargeClusters; i++) {
      EXOChargeCluster *chargeCluster = scintCluster->GetChargeClusterAt(i);
      
      if (TMath::Abs(chargeCluster->fX) < 250 && TMath::Abs(chargeCluster->fY) < 250 && TMath::Abs(chargeCluster->fZ) < 250) { // only keep clusters within world coordinates
         sprintf(ClusterName,"CCL_%i",nVisibleChargeClusters);
         TGeoVolume *CurrentCluster = mgr->MakeBox(ClusterName,0,5*CLHEP::mm,5*CLHEP::mm,5*CLHEP::mm);
        
         CurrentCluster->SetLineColor(kBlue);
         ChargeClusters->AddAt(CurrentCluster,i);
         ChargeClusterVolume->AddNode(CurrentCluster,1,new TGeoTranslation(chargeCluster->fX, chargeCluster->fY, chargeCluster->fZ));

         nVisibleChargeClusters++;
      }
      else {ChargeClusters->AddAt(NULL,0);}
   }
  
   if (nVisibleChargeClusters == 0) {ChargeClusterVolume->InvisibleAll(kTRUE);}
   else {ChargeClusterVolume->InvisibleAll(kFALSE);}
   
   Draw();
   
   return;
}

void EXO3DView::SelectChargeCluster(int ccID)
{
   // Select a charge cluster after drawing an event (Draw(EXOEventData *ED)). The corresponding box
   // as well as the hit wires will be highlighted and the APD data of the associated scintillation
   // cluster is set.
   
   // make sure we have data available
   if (!CurrentEventData) {std::cout << "No event data available. Do a draw before you select a charge cluster" << std::endl; return;}
   
   // check if selected charge cluster ID is within bounds
   int ncl = CurrentEventData->GetNumChargeClusters();
   if (ccID >= ncl || ccID < 0) {std::cout << "Your selected charge cluster ID is out of bounds (ncl = " << ncl << ")" << std::endl; return;}
   
   // mark the charge cluster box (first reset all clusters)
   for (int i = 0; i < ChargeClusters->GetEntriesFast(); i++) {
      if ((TGeoVolume*)ChargeClusters->At(i) != NULL) {((TGeoVolume*)ChargeClusters->At(i))->SetLineColor(kBlue);}
   }
   if ((TGeoVolume*)ChargeClusters->At(ccID) != NULL) {((TGeoVolume*)ChargeClusters->At(ccID))->SetLineColor(kRed);}
   
   EXOScintillationCluster *scintCluster = CurrentEventData->GetChargeCluster(ccID)->GetScintillationCluster();
   if (!scintCluster) {std::cout << "Selected charge cluster has no associated scintillation cluster" << std::endl; return;}
   
   // get min and max of the APD data
   double minAPD = 1000000;
   double maxAPD = -1000000;
   double counts = 0.0;

   TList listOfGangs;
   scintCluster->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
   TIter next(&listOfGangs);
   const EXOAPDSignal* sig;

   while ((sig = (const EXOAPDSignal*)next())) {  
      counts = sig->fCounts; 
      if (counts > maxAPD) {maxAPD = counts;}
      if (counts < minAPD) {minAPD = counts;}
   }
   
   // set the APD data for each gang
   int ColorID = 0;
   int cl = 0;
   int GangID = 0;
   next.Reset();
   while ((sig = (const EXOAPDSignal*)next())) {   
      counts = sig->fCounts;  
      GangID = sig->fChannel - APD_CHANNEL0;
      
      if (GangID < 0 || GangID >= NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE) {continue;}
      
      // get the color ID of the current gang
      ColorID = int((counts - minAPD) / (maxAPD - minAPD) * (NCont-1));
      cl = colorpalette[ColorID];
      
      ((TGeoVolume*)(APDGANG->At(GangID)))->SetLineColor(cl);
   }
   
   // mark the hit wires (first reset al wires)
   for (int i = 0; i < NCHANNEL_PER_WIREPLANE; i++) {
      ((TGeoVolume*)(uWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(uWiresNZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresNZ->At(i)))->SetLineColor(12);
   }
   
   int nUWires = CurrentEventData->GetChargeCluster(ccID)->GetNumUWireSignals();
   for (int i = 0; i < nUWires; i++) {
      int UWireCHID = CurrentEventData->GetChargeCluster(ccID)->GetUWireSignalAt(i)->fChannel;
      
      if (UWireCHID >= 0 && UWireCHID < NCHANNEL_PER_WIREPLANE) {
         int chID = UWireCHID;
         ((TGeoVolume*)(uWiresPZ->At(chID)))->SetLineColor(2);
      }
      
      if (UWireCHID >= 2*NCHANNEL_PER_WIREPLANE && UWireCHID < 3*NCHANNEL_PER_WIREPLANE) {
         int chID = UWireCHID - 2*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(uWiresNZ->At(chID)))->SetLineColor(2);
      }
   }

   int nVWires = CurrentEventData->GetChargeCluster(ccID)->GetNumVWireSignals();
   for (int i = 0; i < nVWires; i++) {
      int VWireCHID = CurrentEventData->GetChargeCluster(ccID)->GetVWireSignalAt(i)->fChannel;
      
      if (VWireCHID >= NCHANNEL_PER_WIREPLANE && VWireCHID < 2*NCHANNEL_PER_WIREPLANE) {
         int chID = VWireCHID - NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(vWiresPZ->At(chID)))->SetLineColor(2);
      }
      if (VWireCHID >= 3*NCHANNEL_PER_WIREPLANE && VWireCHID < 4*NCHANNEL_PER_WIREPLANE) {
         int chID = VWireCHID - 3*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)(vWiresNZ->At(chID)))->SetLineColor(2);
      }
   }
   
   Draw();
   
   return;
}

void EXO3DView::SelectScintillationCluster(int scID)
{
   // Select a scintillation cluster after drawing an event (Draw(EXOEventData *ED)). The APD data
   // will be set and the associated charge clusters are highlighted.
   
   // make sure we have data available
   if (!CurrentEventData) {std::cout << "No event data available. Do a draw before you select a scintillation cluster" << std::endl; return;}
   
   // check if selected scintillation cluster ID is within bounds
   int nsc = CurrentEventData->GetNumScintillationClusters();
   if (scID >= nsc || scID < 0) {std::cout << "Your selected scintillation cluster ID is out of bounds (nsc = " << nsc << ")" << std::endl; return;}
   
   EXOScintillationCluster *scintCluster = CurrentEventData->GetScintillationCluster(scID);
   
   TList listOfGangs;
   scintCluster->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
   TIter next(&listOfGangs);
   const EXOAPDSignal* sig;
   
   // get min and max of the APD data
   double minAPD = 1000000;
   double maxAPD = -1000000;
   double counts = 0.0;
   while ((sig = (const EXOAPDSignal*)next())) {    
      counts = sig->fCounts;
      if (counts > maxAPD) {maxAPD = counts;}
      if (counts < minAPD) {minAPD = counts;}
   }
   
   // set the APD data for each gang
   int ColorID = 0;
   int cl = 0;
   int GangID = 0;
   next.Reset();
   while ((sig = (const EXOAPDSignal*)next())) {
      counts = sig->fCounts;
      GangID = sig->fChannel - APD_CHANNEL0;
      
      if (GangID < 0 || GangID >= NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE) {continue;}
      
      // get the color ID of the current gang
      ColorID = int((counts - minAPD) / (maxAPD - minAPD) * (NCont-1));
      cl = colorpalette[ColorID];
      
      ((TGeoVolume*)(APDGANG->At(GangID)))->SetLineColor(cl);
   }
   
   // mark all associated charge cluster (first reset all clusters)
   // in order to find the cluster ID in the cluster array, compare positions of the clusters
   for (int i = 0; i < ChargeClusters->GetEntriesFast(); i++) {
      if ((TGeoVolume*)ChargeClusters->At(i) != NULL) {((TGeoVolume*)ChargeClusters->At(i))->SetLineColor(kBlue);}
   }
   
   int nAssocChargeClusters = scintCluster->GetNumChargeClusters();
   for (int i = 0; i < nAssocChargeClusters; i++) {
      double xAssoc = scintCluster->GetChargeClusterAt(i)->fX;
      double yAssoc = scintCluster->GetChargeClusterAt(i)->fY;
      double zAssoc = scintCluster->GetChargeClusterAt(i)->fZ;
      
      int clID = 0;
      for (int k = 0; k < ChargeClusters->GetEntriesFast(); k++) {
         double x = CurrentEventData->GetChargeCluster(k)->fX;
         double y = CurrentEventData->GetChargeCluster(k)->fY;
         double z = CurrentEventData->GetChargeCluster(k)->fZ;
         
         if (xAssoc == x && yAssoc == y && zAssoc == z && (TGeoVolume*)ChargeClusters->At(k) != NULL) {((TGeoVolume*)ChargeClusters->At(k))->SetLineColor(kGreen);}
      }
   }
   
   // reset the wires
   for (int i = 0; i < NCHANNEL_PER_WIREPLANE; i++) {
      ((TGeoVolume*)(uWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(uWiresNZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresNZ->At(i)))->SetLineColor(12);
   }
   
   Draw();
   
   return;
}

void EXO3DView::StackData(EXOEventData *ED)
{
   // All charge clusters and scintillation clusters in this event are added to the stack. After
   // stacking, the data can be drawn by calling DrawStackedData().
   
   // fill the positions of all clusters in this event into the histogram
   int ncl = ED->GetNumChargeClusters();
   for (int i = 0; i < ncl; i++) {
      double x = ED->GetChargeCluster(i)->fX;
      double y = ED->GetChargeCluster(i)->fY;
      double z = ED->GetChargeCluster(i)->fZ;
      
      StackedEvents->Fill(x,y,z);
   }
   
   // add the APD counts of all gangs to the histogram
   int nsc = ED->GetNumScintillationClusters();
   for (int i = 0; i < nsc; i++) {
      const EXOScintillationCluster *sc = ED->GetScintillationCluster(i);

      TList listOfGangs;
      sc->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
      TIter next(&listOfGangs);
      const EXOAPDSignal* sig;
      int k = 0;
      while ((sig = (const EXOAPDSignal*)next())) { 
         double GangCount = sig->fCounts; 
         double CurrentContent = StackedAPD->GetBinContent(k+1);
         StackedAPD->SetBinContent(k+1,CurrentContent + GangCount);
         k++;
      }
   }
   
}

void EXO3DView::StackData(EXOChargeCluster *CC)
{
   // The specified charge cluster and its associated scintillation cluster is added to the stack
   
   double x = CC->fX;
   double y = CC->fY;
   double z = CC->fZ;
   
   // fill the position of the cluster to the stacked histogram
   StackedEvents->Fill(x,y,z);
   
   // add the APD counts of all gangs of the associated scintillation cluster to the histogram
   EXOScintillationCluster *scintCluster = CC->GetScintillationCluster();
   
   if (scintCluster) {
      TList listOfGangs;
      scintCluster->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
      TIter next(&listOfGangs);
      const EXOAPDSignal* sig;
      int k = 0;
      while ((sig = (const EXOAPDSignal*)next())) {  
         double GangCount = sig->fCounts;
         
         double CurrentContent = StackedAPD->GetBinContent(k+1);
         StackedAPD->SetBinContent(k+1,CurrentContent + GangCount);
         k++;
      }
   }
   
   return;
}

void EXO3DView::StackData(EXOScintillationCluster *SC)
{
   // The specified scintillation cluster and its associated charge clustes are added to the stack.
   
   // add the APD counts of all gangs to the histogram
   TList listOfGangs;
   SC->AddAPDSignalsOfTypeToList(EXOAPDSignal::kGangFit, listOfGangs);
   TIter next(&listOfGangs);
   const EXOAPDSignal* sig;
   int i = 0;
   while ((sig = (const EXOAPDSignal*)next())) {  
      double GangCount = sig->fCounts; 
         
      double CurrentContent = StackedAPD->GetBinContent(i+1);
      StackedAPD->SetBinContent(i+1,CurrentContent + GangCount);
      i++;
   }
   
   // fill the positions of the associated charge clusters to the stacked histogram
   int nAssocChargeClusters = SC->GetNumChargeClusters();
   for (int i = 0; i < nAssocChargeClusters; i++) {
     double x = SC->GetChargeClusterAt(i)->fX;
     double y = SC->GetChargeClusterAt(i)->fY;
     double z = SC->GetChargeClusterAt(i)->fZ;
     
     StackedEvents->Fill(x,y,z);
   }
   
   return;
}

void EXO3DView::DrawStackedData(const char *option)
{
  // Draws the stacked data added by calling StackData(). The detector is sliced into 1cm3 bins and
  // a box is drawn for each bin with a size and a color proportional to its bin content. For the
  // APDs a color proportional to the summed counts in each gang is set. If the option 'q' is
  // specified only the 3D histogram is created but the geometry is not drawn. This is in particular
  // useful if you want to save the geometry to a file by calling Save(const char *name) rather than
  // drawing it. The geometry can still be drawn afterwards by just calling Draw().
  
  ChargeClusterVolume->ClearNodes();
  
  double maxEntries = StackedEvents->GetMaximum();
  double minEntries = StackedEvents->GetMinimum();
  
  // draw box for each historam bin. The size of the box is proportional to the bin content
  char ClusterName[50];
  int clusterCount = 0;
  for (int i1 = 1; i1 <= 40; i1++) {
     for (int i2 = 1; i2 <= 40; i2++) {
        for (int i3 = 1; i3 <= 40; i3++) {
           double content = StackedEvents->GetBinContent(i1,i2,i3);
           
           if (content <= 0) {continue;}
           
           // acoount for the data depth
           if (content < StackedHistogramDepth * maxEntries) {continue;}
           
           sprintf(ClusterName,"CL_%i",clusterCount);
           double ClusterVolume = (content - minEntries) / (maxEntries - minEntries) * 1000;
           double ClusterLength = pow(ClusterVolume,0.33333333) / 2.0;
           
           TGeoVolume *CurrentCluster = mgr->MakeBox(ClusterName,MediumXenon,ClusterLength*CLHEP::mm,ClusterLength*CLHEP::mm,ClusterLength*CLHEP::mm);
           
           int ColorID = int((content - minEntries) / (maxEntries - minEntries) * (NCont-1));
           int cl = colorpalette[ColorID];
           
           CurrentCluster->SetLineColor(cl);
           ChargeClusterVolume->AddNode(CurrentCluster,1,new TGeoTranslation(-200 + (i1-0.5)*10, -200 + (i2-0.5)*10, -200 + (i3-0.5)*10));
           
           clusterCount++;
        }
     }
  }
  
  if (clusterCount == 0) {ChargeClusterVolume->InvisibleAll(kTRUE);}
  else {ChargeClusterVolume->InvisibleAll(kFALSE);}
  
  double maxEntriesAPD = StackedAPD->GetMaximum();
  double minEntriesAPD = StackedAPD->GetMinimum();
  
  // draw the stacked APD signal
  for (int i = 0; i < 74; i++) {
     double content = StackedAPD->GetBinContent(i+1);
     
     int ColorID = int((content - minEntriesAPD) / (maxEntriesAPD - minEntriesAPD) * (NCont-1));
     int cl = colorpalette[ColorID];
     
     ((TGeoVolume*)(APDGANG->At(i)))->SetLineColor(cl);
  }

  TString opt = option;
  opt.ToLower();
  if (opt.Contains("q")) {return;}

  Draw();
  
  return;
}

void EXO3DView::Clear()
{
   // Remove all charge clusters and pcds from the cluster and pcd volumes and reset stacked histograms. Set the APD
   // colors to its default value.
   
   // cleat internal event data
   CurrentEventData = 0;
   
   // clear all charge clusters
   ChargeClusterVolume->ClearNodes();
   PCDVolume->ClearNodes();
   nVisibleChargeClusters = 0;
   nVisiblePCDs = 0;
   ChargeClusterVolume->InvisibleAll(kTRUE);
   PCDVolume->InvisibleAll(kTRUE);
   
   // clear the stacked histograms
   StackedEvents->Reset();
   StackedAPD->Reset();
   
   // clear the wires
   for (int i = 0; i < NCHANNEL_PER_WIREPLANE; i++) {
      ((TGeoVolume*)(uWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresPZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(uWiresNZ->At(i)))->SetLineColor(12);
      ((TGeoVolume*)(vWiresNZ->At(i)))->SetLineColor(12);
   }
   
   // clear the APDs
   for (int i = 0; i < 74; i++) {((TGeoVolume*)APDGANG->At(i))->SetLineColor(16);}
   
   return;
}

void EXO3DView::ClearAPDs()
{
   // Reset the APDs
   
   for (int i = 0; i < 74; i++) {((TGeoVolume*)APDGANG->At(i))->SetLineColor(16);}
   
   return;
}

void EXO3DView::SetWireWaveformSample(EXOEventData *ED, int sample)
{
   // Set the waveform data at a specific sample. The wires will be colored according to their
   // amplitude at the specified sample.
   
   // check if there are waveforms in the event data
   int nWaveforms = ED->GetWaveformData()->GetNumWaveforms();
   int nSample = ED->GetWaveformData()->fNumSamples;
   if (nWaveforms == 0) {std::cout << "No waveforms in event data" << std::endl; return;}
   if (sample < 0 || sample > nSample) {std::cout << "Sample must be between 0 and " << nSample << ". You have chosen " << sample << std::endl; return;}
   
   // check if this event is already in the histograms. If not fill the histograms with the current
   // event data
   if (!(ED->fEventNumber == CurrentHistEvent)) {FillWaveformHist(ED); CurrentHistEvent = ED->fEventNumber;}
   
   // get minimum and maximum of all wire channels for current sample
   int max = int(hWire->GetMaximum());
   int min = int(hWire->GetMinimum());
   
   int CurrentSample = 0;
   int ColorID = 0;
   int cl = 0;
   for (int i = 0; i < nWaveforms; i++) {
      EXOWaveform *wf = (EXOWaveform*)ED->GetWaveformData()->GetWaveform(i);
      
      int chID = wf->fChannel;
      if (chID >= APD_CHANNEL0) {continue;}
      
      CurrentSample = int(hWire->GetBinContent(sample+1,chID+1));
      ColorID = int((double(CurrentSample - min) / double(max - min) * (NCont-1)));
      cl = colorpalette[ColorID];
      
      if (chID < NCHANNEL_PER_WIREPLANE) {
         int chIDTMP = chID;
         ((TGeoVolume*)uWiresPZ->At(chID))->SetLineColor(cl);
      }

      if (chID >= NCHANNEL_PER_WIREPLANE && chID < 2*NCHANNEL_PER_WIREPLANE) {
         int chIDTMP = chID - NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)vWiresPZ->At(chIDTMP))->SetLineColor(cl);
      }

      if (chID >= 2*NCHANNEL_PER_WIREPLANE && chID < 3*NCHANNEL_PER_WIREPLANE) {
         int chIDTMP = chID - 2*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)uWiresNZ->At(chIDTMP))->SetLineColor(cl);
      }

      if (chID >= 3*NCHANNEL_PER_WIREPLANE && chID < 4*NCHANNEL_PER_WIREPLANE) {
         int chIDTMP = chID - 3*NCHANNEL_PER_WIREPLANE;
         ((TGeoVolume*)vWiresNZ->At(chIDTMP))->SetLineColor(cl);
      }
   }
   
   Draw();
   
   return;
}

void EXO3DView::SetAPDWaveformSample(EXOEventData *ED, int sample)
{
   // Set the waveform data at a specific sample. The APDs will be colored according to their
   // amplitude at the specified sample.
   
   // check if there are waveforms in the event data
   int nWaveforms = ED->GetWaveformData()->GetNumWaveforms();
   int nSample = ED->GetWaveformData()->fNumSamples;
   if (nWaveforms == 0) {std::cout << "No waveforms in event data" << std::endl; return;}
   if (sample < 0 || sample > nSample) {std::cout << "Sample must be between 0 and " << nSample << ". You have chosen " << sample << std::endl; return;}
   
   // check if this event is already in the histograms. If not fill the histograms with the current
   // event data
   if (!(ED->fEventNumber == CurrentHistEvent)) {FillWaveformHist(ED); CurrentHistEvent = ED->fEventNumber;}
   
   // get minimum and maximum of all wire channels for current sample
   int max = int(hAPD->GetMaximum());
   int min = int(hAPD->GetMinimum());
   
   int CurrentSample = 0;
   int ColorID = 0;
   int cl = 0;
   for (int i = 0; i < nWaveforms; i++) {
      EXOWaveform *wf = (EXOWaveform*)ED->GetWaveformData()->GetWaveform(i);
      
      int chIDTMP = wf->fChannel;
      if (chIDTMP < APD_CHANNEL0) {continue;}
      
      int chID = chIDTMP - APD_CHANNEL0;
      
      CurrentSample = int(hAPD->GetBinContent(sample+1,chID+1));
      ColorID = int((double(CurrentSample - min) / double(max - min) * (NCont-1)));
      cl = colorpalette[ColorID];
      
      ((TGeoVolume*)APDGANG->At(chID))->SetLineColor(cl);
   }
   
   Draw();
   
   return;
}

void EXO3DView::FillWaveformHist(EXOEventData *ED)
{
   // Fill the 2D histograms of the waveform data.
   
   int nSample = ED->GetWaveformData()->fNumSamples;
   int nWaveforms = ED->GetWaveformData()->GetNumWaveforms();
   
   int nwires = NCHANNEL_PER_WIREPLANE*NWIREPLANE;
   int napds = NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;
   hWire = new TH2I("hWire","",nSample,0,nSample,nwires,0,nwires);
   hAPD = new TH2I("hAPD","",nSample,0,nSample,napds,0,napds);
   
   hWire->Reset();
   hAPD->Reset();
   
   int nrBLSample = nSample / 6;
   
   for (int i = 0; i < nWaveforms; i++) {
      EXOWaveform *wf = (EXOWaveform*)ED->GetWaveformData()->GetWaveform(i);
      wf->Decompress();
      
      int chID = wf->fChannel;
      
      double BLTMP = 0.0;
      int BL = 0;
      for (int k = 0; k < nrBLSample; k++) {BLTMP += wf->At(k);}
      
      if (nrBLSample > 0) {
         BLTMP /= nrBLSample;
         BL = int(BLTMP);
      }

      for (int k = 0; k < nSample; k++) {
         if (chID < APD_CHANNEL0) {hWire->SetBinContent(k+1,chID+1,wf->At(k) - BL);}
         else {hAPD->SetBinContent(k+1,chID-APD_CHANNEL0+1,wf->At(k) - BL);}
      }
   }
   
   return;
}

void EXO3DView::Save(const char *name)
{
  TFile *f = new TFile(name,"RECREATE");
  TopVolume->Write();
  f->Close();

  return;
}

// ---- Private functions -------------------------------------------------------------------------
void EXO3DView::CreateDetectorMedium()
{
   // retrieve elements from element table
   TGeoElementTable *ElementTable = gGeoManager->GetElementTable();
   TGeoElement *elementHydrogen = ElementTable->FindElement("H");
   TGeoElement *elementCarbon = ElementTable->FindElement("C");
   TGeoElement *elementNitrogen = ElementTable->FindElement("N");
   TGeoElement *elementOxygen = ElementTable->FindElement("O");
   TGeoElement *elementFlorine = ElementTable->FindElement("F");
   TGeoElement *elementSodium = ElementTable->FindElement("Na");
   TGeoElement *elementSilicon = ElementTable->FindElement("Si");
   TGeoElement *elementChlorine = ElementTable->FindElement("Cl");
   TGeoElement *elementXenon = ElementTable->FindElement("Xe");
   TGeoElement *elementCopper = ElementTable->FindElement("Cu");
   
   // generate materials / mixtures
   TGeoMixture *matHFE = new TGeoMixture("HFE",4,1.77*CLHEP::g/CLHEP::cm3);
   matHFE->AddElement(elementHydrogen,3);
   matHFE->AddElement(elementOxygen,1);
   matHFE->AddElement(elementCarbon,4);
   matHFE->AddElement(elementFlorine,7);
   TGeoMixture *matTeflon = new TGeoMixture("Teflon",3,2.15*CLHEP::g/CLHEP::cm3);
   matTeflon->AddElement(elementFlorine,10);
   matTeflon->AddElement(elementOxygen,1);
   matTeflon->AddElement(elementCarbon,5);
   TGeoMixture *matWireSupport = new TGeoMixture("WireSupport",3,1.19*CLHEP::g/CLHEP::cm3);
   matWireSupport->AddElement(elementHydrogen,8);
   matWireSupport->AddElement(elementOxygen,2);
   matWireSupport->AddElement(elementCarbon,5);
   TGeoMaterial *matSilicon= new TGeoMaterial("Silicon",elementSilicon,2.33*CLHEP::g/CLHEP::cm3);
   TGeoMaterial *matXenon = new TGeoMaterial("Xenon",elementXenon,3.043*CLHEP::g/CLHEP::cm3);
   TGeoMaterial *matCopper = new TGeoMaterial("Copper",elementCopper,8.96*CLHEP::g/CLHEP::cm3);
   
   // define all detector elements made from the same material separately. All copper elements
   TGeoMaterial *matVessel = new TGeoMaterial("matVessel",elementCopper,8.96*CLHEP::g/CLHEP::cm3);
   TGeoMaterial *matCathode = new TGeoMaterial("matCathode",elementCopper,8.96*CLHEP::g/CLHEP::cm3);
   TGeoMaterial *matRings = new TGeoMaterial("matRings",elementCopper,8.96*CLHEP::g/CLHEP::cm3);
   TGeoMaterial *matWires = new TGeoMaterial("matWires",elementCopper,8.96*CLHEP::g/CLHEP::cm3);
   
   // generate mediums
   MediumHFE = new TGeoMedium("HFE",1,matHFE);
   MediumTeflon = new TGeoMedium("Teflon",2,matTeflon);
   MediumWireSupport = new TGeoMedium("WireSupport",3,matWireSupport);
   MediumSilicon = new TGeoMedium("Silicon",4,matSilicon);
   MediumXenon = new TGeoMedium("Xenon",5,matXenon);
   MediumCopper = new TGeoMedium("Copper",6,matCopper);
   
   // detector elements made from the same material have a separate medium to change properties like transperancy individually
   MediumVessel = new TGeoMedium("VesselMedium",7,matVessel);
   MediumCathode = new TGeoMedium("CathodeMedium",8,matCathode);
   MediumRings = new TGeoMedium("RingMedium",9,matRings);
   MediumWires = new TGeoMedium("WiresMedium",10,matWires);
   
   return;
}


void EXO3DView::CreateBaseDetectorElements()
{
// **** Xenon Vessel ******************************************************************************
   VesselVolumes = new TObjArray();
   VesselVolumes->Expand(6);
   
   double phi_start[5] = {0,90,180,270,0};
   double dPhi[5] = {180,180,180,180,360};
   
   char VolumeNameTMP[50];
   char VolumeName[50];
   for (int i = 0; i < 5; i++) {
      sprintf(VolumeNameTMP,"XeVesselTMP%i",i);
      sprintf(VolumeName,"XeVessel%i",i);
      TGeoPcon *XeVesselTMP = new TGeoPcon(VolumeName,phi_start[i],dPhi[i],14);
      
      XeVesselTMP->DefineSection(0,-26.982*CLHEP::cm,21.732*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(1,-22.42*CLHEP::cm,21.732*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(2,-22.42*CLHEP::cm,0.0*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(3,-22.27*CLHEP::cm,0.0*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(4,-22.27*CLHEP::cm,22.725*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(5,-18.523*CLHEP::cm,22.725*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(6,-16.01*CLHEP::cm,19.799*CLHEP::cm,19.949*CLHEP::cm);
      XeVesselTMP->DefineSection(7,16.01*CLHEP::cm,19.799*CLHEP::cm,19.949*CLHEP::cm);
      XeVesselTMP->DefineSection(8,18.523*CLHEP::cm,22.725*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(9,22.27*CLHEP::cm,22.725*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(10,22.27*CLHEP::cm,0.0*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(11,22.42*CLHEP::cm,0.0*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(12,22.42*CLHEP::cm,21.732*CLHEP::cm,22.875*CLHEP::cm);
      XeVesselTMP->DefineSection(13,26.982*CLHEP::cm,21.732*CLHEP::cm,22.875*CLHEP::cm);
      
      TGeoVolume *CurrentVolume = new TGeoVolume(VolumeName,XeVesselTMP,MediumVessel);
      VesselVolumes->AddAt(CurrentVolume,i);
      
      TopVolume->AddNode(CurrentVolume,1);
      CurrentVolume->SetTransparency(50);
      CurrentVolume->InvisibleAll(kTRUE);
   }
   
   ((TGeoVolume*)VesselVolumes->At(1))->InvisibleAll(kFALSE);
   XeVessel = (TGeoVolume*)VesselVolumes->At(1);
// ************************************************************************************************

// **** Cathode ***********************************************************************************
   CathodeVolumes = new TObjArray();
   CathodeVolumes->Expand(6);
   
   for (int i = 0; i < 5; i++) {
      sprintf(VolumeNameTMP,"CathodeTMP%i",i);
      sprintf(VolumeName,"Cathode%i",i);
      TGeoPcon *CathodeTMP = new TGeoPcon(VolumeNameTMP,phi_start[i],dPhi[i],6);
      
      CathodeTMP->DefineSection(0,-1*CATHODE_RING_THICKNESS,CATHODE_RADIUS,REFLECTORINNERRAD);
      CathodeTMP->DefineSection(1,-0.0125/2*CLHEP::cm,CATHODE_RADIUS,REFLECTORINNERRAD);
      CathodeTMP->DefineSection(2,-0.0125/2*CLHEP::cm,0,REFLECTORINNERRAD);
      CathodeTMP->DefineSection(3,0.0125/2*CLHEP::cm,0,REFLECTORINNERRAD);
      CathodeTMP->DefineSection(4,0.0125/2*CLHEP::cm,CATHODE_RADIUS,REFLECTORINNERRAD);
      CathodeTMP->DefineSection(5,CATHODE_RING_THICKNESS,CATHODE_RADIUS,REFLECTORINNERRAD);
      
      TGeoVolume *CurrentVolume = new TGeoVolume(VolumeName,CathodeTMP,MediumCathode);
      CathodeVolumes->AddAt(CurrentVolume,i);
      
      TopVolume->AddNode(CurrentVolume,1);
      CurrentVolume->SetTransparency(0);
      CurrentVolume->InvisibleAll(kTRUE);
   }
   
   ((TGeoVolume*)CathodeVolumes->At(1))->InvisibleAll(kFALSE);
   Cathode = (TGeoVolume*)CathodeVolumes->At(1);
// ************************************************************************************************

// **** Field Shaping Rings ***********************************************************************
   FieldShapingRingsVolumes = new TObjArray();
   FieldShapingRingsVolumes->Expand(6);
   
   FieldRings = new TObjArray();
   FieldRings->Expand(120);
   
   VisibleFieldRings = new TObjArray();
   VisibleFieldRings->Expand(20);
   
   char VolumeName2[50];
   for (int i = 0; i < 5; i++) {
      sprintf(VolumeName2,"FieldShapingRings%i",i);
      TGeoVolume *CurrentFieldRingVolume = mgr->MakeBox(VolumeName2,0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,CATHODE_ANODE_DISTANCE);
      
      for (int k = 0; k < 20; k++) {
         sprintf(VolumeNameTMP,"FieldRingTMP%i_%i",i,k);
         sprintf(VolumeName,"FieldRing%i_%i",i,k);
         TGeoPcon *FieldRingTMP = new TGeoPcon(VolumeNameTMP,phi_start[i],dPhi[i],2);
         FieldRingTMP->DefineSection(0,-1*FIELDRING_WIDTH/2,FIELDRING_INNER_RADIUS,FIELDRING_OUTER_RADIUS);
         FieldRingTMP->DefineSection(1,FIELDRING_WIDTH/2,FIELDRING_INNER_RADIUS,FIELDRING_OUTER_RADIUS);
   
         TGeoVolume *CurrentVolume = new TGeoVolume(VolumeName,FieldRingTMP,MediumRings);
         FieldRings->AddAt(CurrentVolume,i*20+k);
         
         int z;
         if (k < 10) {z = k - 10;}
         else {z = k - 9;}
         
         CurrentFieldRingVolume->AddNode(CurrentVolume,1,new TGeoTranslation(0,0,(CATHODE_ANODE_DISTANCE/11.0)*z));
         CurrentVolume->SetLineColor(18);
         CurrentVolume->SetTransparency(50);
         CurrentVolume->InvisibleAll(kTRUE);
      }
      
      FieldShapingRingsVolumes->AddAt(CurrentFieldRingVolume,i);
      
      TopVolume->AddNode(CurrentFieldRingVolume,1);
      CurrentFieldRingVolume->InvisibleAll(kTRUE);
   }
   
   ((TGeoVolume*)FieldShapingRingsVolumes->At(1))->InvisibleAll(kFALSE);
   FieldShapingRings = (TGeoVolume*)FieldShapingRingsVolumes->At(1);
   
   for (int i = 0; i < 20; i++) {VisibleFieldRings->AddAt((TGeoVolume*)FieldRings->At(20+i),i);}
// ************************************************************************************************

// **** Teflon Reflector **************************************************************************
   ReflectorVolumes = new TObjArray();
   ReflectorVolumes->Expand(6);
   
   for (int i = 0; i < 5; i++) {
      sprintf(VolumeNameTMP,"ReflectorTMP%i",i);
      sprintf(VolumeName,"Reflector%i",i);
      TGeoPcon *ReflectorTMP = new TGeoPcon(VolumeNameTMP,phi_start[i],dPhi[i],2);
      
      ReflectorTMP->DefineSection(0,-1*REFLECTORLENGTH,REFLECTORINNERRAD,REFLECTOROUTERRAD);
      ReflectorTMP->DefineSection(1,REFLECTORLENGTH,REFLECTORINNERRAD,REFLECTOROUTERRAD);
      
      TGeoVolume *CurrentVolume = new TGeoVolume(VolumeName,ReflectorTMP,MediumTeflon);
      ReflectorVolumes->AddAt(CurrentVolume,i);
      
      TopVolume->AddNode(CurrentVolume,1);
      CurrentVolume->SetTransparency(50);
      CurrentVolume->SetLineColor(kBlue);
      CurrentVolume->InvisibleAll(kTRUE);
   }
   
   ((TGeoVolume*)ReflectorVolumes->At(1))->InvisibleAll(kFALSE);
   Reflector = (TGeoVolume*)ReflectorVolumes->At(1);
// ************************************************************************************************

// **** Wire Support Frames ***********************************************************************
  TGeoPgon *FrameTMP = new TGeoPgon("WireSupportFrameTMP",0,360,6,2);
  FrameTMP->DefineSection(0, WIRESUPPORT_THICKNESS, WIRESUPPORT_INNER_RADIUS, WIRESUPPORT_OUTER_RADIUS);
  FrameTMP->DefineSection(1, -1*WIRESUPPORT_THICKNESS, WIRESUPPORT_INNER_RADIUS, WIRESUPPORT_OUTER_RADIUS);
  TGeoVolume *Frame = new TGeoVolume("WireSupportFrame",FrameTMP,MediumWireSupport);
  Frame->SetLineColor(kGreen-2);
  
  WireVolumePZ->AddNode(Frame,1,new TGeoRotation("R1",0,0,30));
  WireVolumeNZ->AddNode(Frame,2,new TGeoRotation("R3",0,0,30));
// ************************************************************************************************

  return;
}

void EXO3DView::CreateActiveDetectorElements()
{
  CreateWires(); // wires
  CreateAPDs(); // APDs
  //CreateChargeCluster(); // charge clusters

  return;
}

void EXO3DView::CreateWires()
{ 
  TGeoRotation *r90 = new TGeoRotation("r90",0,90,0);
  TGeoRotation *r60 = new TGeoRotation("r60",0,0,60);
  TGeoRotation *r120 = new TGeoRotation("r120",0,0,120);
  
  TGeoVolume *WireTMP1 = mgr->MakeBox("WireTMP1",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);
  TGeoVolume *WireTMP2 = mgr->MakeBox("WireTMP2",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);
  TGeoVolume *WireTMP3 = mgr->MakeBox("WireTMP3",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);
  TGeoVolume *WireTMP4 = mgr->MakeBox("WireTMP4",0,LXEVESSELINNERRAD,LXEVESSELINNERRAD,WIRESUPPORT_THICKNESS);

  uWiresPZ = new TObjArray();
  uWiresPZ->Expand(NCHANNEL_PER_WIREPLANE);
  
  vWiresPZ = new TObjArray();
  vWiresPZ->Expand(NCHANNEL_PER_WIREPLANE);
  
  uWiresNZ = new TObjArray();
  uWiresNZ->Expand(NCHANNEL_PER_WIREPLANE);
  
  vWiresNZ = new TObjArray();
  vWiresNZ->Expand(NCHANNEL_PER_WIREPLANE);

  // create wires in wire coordinate system
  double stp = 2*WIRESUPPORT_INNER_RADIUS / (NCHANNEL_PER_WIREPLANE + 1);
  double xWire = 0.0;
  double yWire = 0.0;
  char uWireNamePZ[50];
  char vWireNamePZ[50];
  char uWireNameNZ[50];
  char vWireNameNZ[50];
  char TNameUPZ[50];
  char TNameVPZ[50];
  char TNameUNZ[50];
  char TNameVNZ[50];
  for (int i = 0; i < NCHANNEL_PER_WIREPLANE; i++) {
     xWire = (i+1)*stp - WIRESUPPORT_INNER_RADIUS;
     if (i < NCHANNEL_PER_WIREPLANE/2) {yWire = tan30*(xWire) + WIRESUPPORT_INNER_RADIUS/cos30;}
     else {yWire = -tan30*(xWire) + WIRESUPPORT_INNER_RADIUS/cos30;}

     // names of wires and transformations
     sprintf(uWireNamePZ,"uWirePZ_%i",i);
     sprintf(vWireNamePZ,"vWirePZ_%i",i);
     sprintf(uWireNameNZ,"uWireNZ_%i",i);
     sprintf(vWireNameNZ,"vWireNZ_%i",i);
     sprintf(TNameUPZ,"TUPZ_%i",i);
     sprintf(TNameVPZ,"TVPZ_%i",i);
     sprintf(TNameUNZ,"TUNZ_%i",i);
     sprintf(TNameVNZ,"TVNZ_%i",i);
     
     // create the current wires
     TGeoVolume *CurrentuWirePZ = mgr->MakeTube(uWireNamePZ,MediumWires,0,1*CLHEP::mm,yWire);
     TGeoVolume *CurrentvWirePZ = mgr->MakeTube(vWireNamePZ,MediumWires,0,1*CLHEP::mm,yWire);
     TGeoVolume *CurrentuWireNZ = mgr->MakeTube(uWireNameNZ,MediumWires,0,1*CLHEP::mm,yWire);
     TGeoVolume *CurrentvWireNZ = mgr->MakeTube(vWireNameNZ,MediumWires,0,1*CLHEP::mm,yWire);

     // add wires to TObjArray
     uWiresPZ->AddAt(CurrentuWirePZ,i);
     vWiresPZ->AddAt(CurrentvWirePZ,i);
     uWiresNZ->AddAt(CurrentuWireNZ,i);
     vWiresNZ->AddAt(CurrentvWireNZ,i);
     
     // set initial color
     ((TGeoVolume*)(uWiresPZ->At(i)))->SetLineColor(12);
     ((TGeoVolume*)(vWiresPZ->At(i)))->SetLineColor(12);
     ((TGeoVolume*)(uWiresNZ->At(i)))->SetLineColor(12);
     ((TGeoVolume*)(vWiresNZ->At(i)))->SetLineColor(12);
     
     // add nodes to temprary wire volumes
     WireTMP1->AddNode(CurrentuWirePZ,1,new TGeoCombiTrans(TNameUNZ,xWire,0,WIRESUPPORT_THICKNESS-0.5*CLHEP::mm,r90));
     WireTMP2->AddNode(CurrentvWirePZ,1,new TGeoCombiTrans(TNameVPZ,xWire,0,-1*(WIRESUPPORT_THICKNESS-0.5*CLHEP::mm),r90));
     WireTMP3->AddNode(CurrentuWireNZ,1,new TGeoCombiTrans(TNameUNZ,xWire,0,-1*(WIRESUPPORT_THICKNESS-0.5*CLHEP::mm),r90));
     WireTMP4->AddNode(CurrentvWireNZ,1,new TGeoCombiTrans(TNameVNZ,xWire,0,WIRESUPPORT_THICKNESS-0.5*CLHEP::mm,r90));
  }
  
  // add temporary wire volumes to wire volumes
  WireVolumePZ->AddNode(WireTMP1,1,new TGeoRotation("RTMP1",0,0,120));
  WireVolumePZ->AddNode(WireTMP2,1,new TGeoRotation("RTMP2",0,0,60));
  WireVolumeNZ->AddNode(WireTMP3,1,new TGeoRotation("RTMP3",0,0,60));
  WireVolumeNZ->AddNode(WireTMP4,1,new TGeoRotation("RTMP4",0,0,120));
  
  return;
}

void EXO3DView::CreateAPDs()
{
  APDGANG = new TObjArray();
  APDGANG->Expand(74);
  
  TGeoVolume *APD = mgr->MakeTube("APD",MediumSilicon,0,APDRADIUS,2*CLHEP::mm);
  TGeoTranslation *T1 = new TGeoTranslation("T1",2.218*CLHEP::cm,0,0);
  TGeoTranslation *T2 = new TGeoTranslation("T2",-2.218*CLHEP::cm,0,0);
  TGeoTranslation *T3 = new TGeoTranslation("T3",1.109*CLHEP::cm,-1.921*CLHEP::cm,0);
  TGeoTranslation *T4 = new TGeoTranslation("T4",1.109*CLHEP::cm,1.921*CLHEP::cm,0);
  TGeoTranslation *T5 = new TGeoTranslation("T5",-1.109*CLHEP::cm,-1.921*CLHEP::cm,0);
  TGeoTranslation *T6 = new TGeoTranslation("T6",-1.109*CLHEP::cm,1.921*CLHEP::cm,0);
  
  T1->RegisterYourself();
  T2->RegisterYourself();
  T3->RegisterYourself();
  T4->RegisterYourself();
  T5->RegisterYourself();
  T6->RegisterYourself();
  
  TGeoCompositeShape *APDGangTMP = new TGeoCompositeShape("APDGangTMP","APD:T1 + APD:T2 + APD:T3 + APD:T4 + APD:T5 + APD:T6 + APD");
  //TGeoVolume *APDGang = new TGeoVolume("APDGang",APDGangTMP,MediumSilicon);
  
  char APDName[50];
  for (int i = 0; i < 74; i++) {
     sprintf(APDName,"APDGANG_%i",i);
     TGeoVolume *CurrentAPDGang = new TGeoVolume(APDName,APDGangTMP,MediumSilicon);
    
     APDGANG->AddAt(CurrentAPDGang,i);
     if (i < 37) {APDVolumePZ->AddNode(CurrentAPDGang,1,new TGeoTranslation(xAPDNew[i] * CLHEP::cm, yAPDNew[i] * CLHEP::cm, 0));}
     else {APDVolumeNZ->AddNode(CurrentAPDGang,1,new TGeoTranslation(xAPDNew[i] * CLHEP::cm, yAPDNew[i] * CLHEP::cm, 0));}
    
     // set initial color
     ((TGeoVolume*)(APDGANG->At(i)))->SetLineColor(16);
  }
  
  return;
}

void EXO3DView::CreateChargeCluster(EXOEventData *ED, int col)
{
  int nChargeCluster = ED->GetNumChargeClusters();
  
  ChargeClusters = new TObjArray();
  ChargeClusters->Expand(nChargeCluster);
  
  // create box for each charge cluster
  char ClusterName[50];
  for (int i = 0; i < nChargeCluster; i++) {
     EXOChargeCluster* charge_cluster = ED->GetChargeCluster(i);
     if (TMath::Abs(charge_cluster->fX) < 250 && TMath::Abs(charge_cluster->fY) < 250 && TMath::Abs(charge_cluster->fZ) < 250) { // only keep clusters within world coordinates
        sprintf(ClusterName,"CCL_%i",nVisibleChargeClusters);
        TGeoVolume *CurrentCluster = mgr->MakeBox(ClusterName,0,5*CLHEP::mm,5*CLHEP::mm,5*CLHEP::mm);
        
        CurrentCluster->SetLineColor(col);
        ChargeClusters->AddAt(CurrentCluster,i);
        ChargeClusterVolume->AddNode(CurrentCluster,1,new TGeoTranslation(charge_cluster->fX, charge_cluster->fY, charge_cluster->fZ));

        nVisibleChargeClusters++;
     }
     else {ChargeClusters->AddAt(NULL,i);}
  }
  
  if (nVisibleChargeClusters == 0) {ChargeClusterVolume->InvisibleAll(kTRUE);}
  else {ChargeClusterVolume->InvisibleAll(kFALSE);}
  
  return;
}
#endif
// ------------------------------------------------------------------------------------------------
