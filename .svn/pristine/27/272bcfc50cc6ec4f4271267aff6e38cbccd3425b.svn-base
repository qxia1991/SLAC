#ifndef EXO3DView_h
#define EXO3DView_h
#ifdef BUILD_VIEWER3D

#include <iostream>
#include "TCanvas.h"
#include "TObjArray.h"
#include "TGeoVolume.h"
class EXOEventData;
class EXOChargeCluster;
class EXOScintillationCluster;
class EXOMCPixelatedChargeDeposit;
class TGLViewer;
class TH1D;
class TH2I;
class TH3D;
class TGeoManager;
class TGeoMedium;

class EXO3DView
{
  private:
     TCanvas *NonGLCanvas;
     TGLViewer *v;

     TGeoManager *mgr;
     TGeoVolume *TopVolume;
     TGeoVolume *WireVolumePZ;
     TGeoVolume *WireVolumeNZ;
     TGeoVolume *APDVolumePZ;
     TGeoVolume *APDVolumeNZ;
     TGeoVolume *ChargeClusterVolume;
     TGeoVolume *PCDVolume;
   
     TGeoVolume *XeVessel;
     TGeoVolume *Cathode;
     TGeoVolume *FieldShapingRings;
     TGeoVolume *Reflector;
     
     TObjArray *VesselVolumes;
     TObjArray *CathodeVolumes;
     TObjArray *FieldShapingRingsVolumes;
     TObjArray *ReflectorVolumes;
   
     TGeoMedium *MediumHFE;
     TGeoMedium *MediumTeflon;
     TGeoMedium *MediumWireSupport;
     TGeoMedium *MediumSilicon;
     TGeoMedium *MediumXenon;
     TGeoMedium *MediumCopper;
     TGeoMedium *MediumVessel;
     TGeoMedium *MediumCathode;
     TGeoMedium *MediumRings;
     TGeoMedium *MediumWires;

     TObjArray *uWiresPZ;
     TObjArray *vWiresPZ;
     TObjArray *uWiresNZ;
     TObjArray *vWiresNZ;
     TObjArray *APDGANG;

     TObjArray *ChargeClusters;
     TObjArray *FieldRings;
     TObjArray *VisibleFieldRings;

     int NCont;     
     int ColorRangeMin;
     int ColorRangeMax;
     int *colorpalette;
     
     double *xAPDNew;
     double *yAPDNew;
     
     TH3D *StackedEvents;
     TH1D *StackedAPD;
     double StackedHistogramDepth;

     TH2I *hWire;
     TH2I *hAPD;
     int CurrentHistEvent;
     
     EXOEventData *CurrentEventData;
     int nVisibleChargeClusters;
     int nVisiblePCDs;
     
     void Init();
     void CreateDetectorMedium();
     void CreateBaseDetectorElements();
     void CreateActiveDetectorElements();
     void CreateWires();
     void CreateAPDs();
     void CreateChargeCluster(EXOEventData *ED, int col = kBlue);

  public:
     EXO3DView();
     ~EXO3DView();
     
     void AddData(EXOEventData *ED, int col = kBlue);
     void AddData(EXOChargeCluster *chargeCluster, int col = kBlue);
     void Draw();
     void Draw(EXOEventData *ED);
     void Draw(EXOChargeCluster *chargeCluster);
     void Draw(EXOScintillationCluster *scintCluster);
     void Draw(EXOMCPixelatedChargeDeposit *pcd);
     void SelectChargeCluster(int ccID = -1);
     void SelectScintillationCluster(int scID = -1);
     void StackData(EXOEventData *ED);
     void StackData(EXOChargeCluster *CC);
     void StackData(EXOScintillationCluster *SC);
     void DrawStackedData(const char *option);
     void Clear();
     void ClearAPDs();
     void SetWireWaveformSample(EXOEventData *ED, int sample);
     void SetAPDWaveformSample(EXOEventData *ED, int sample);
     void SetWireColor(int chan, int col);
     void FillWaveformHist(EXOEventData *ED);
     void Save(const char *name);

// ---- inline functions --------------------------------------------------------------------------

     inline TGeoVolume *GetVesselVolume()
     {
        // Returns the vessel volume
        return XeVessel;
     }
     
     inline TGeoVolume *GetCathodeVolume()
     {
        // Returns the cathode volume
        return Cathode;
     }
     
     inline TGeoVolume *GetFieldRingVolume()
     {
        // Returns the field shaping rings volume
        return FieldShapingRings;
     }
     
     inline TObjArray *GetFieldRings()
     {
        // Returns all field ring objects in a TObjArray
        return VisibleFieldRings;
     }
     
     inline TGeoVolume *GetReflectorVolume()
     {
        // Returns the reflector volume
        return Reflector;
     }
     
     inline TH2I *GetWireHist()
     {
        // Returns the 2D wire waveform histogram. The histogram is filled when SetWireWaveformSample()
        // is called.
        return hWire;
     }
     
     inline TH2I *GetAPDHist()
     {
        // Returns the 2D APD waveform histogram. The histogram is filled when SetAPDWaveformSample()
        // is called.
        return hAPD;
     }
     
     inline void SetVesselInvisible(bool visible = kTRUE)
     {
        // Set the visibility of the vessel. kTRUE = invisible, kFALSE = visible
        XeVessel->InvisibleAll(visible);
     }
     
     inline void SetCathodeInvisible(bool visible = kTRUE)
     {
        // Set the visibility of the Cathode. kTRUE = invisible, kFALSE = visible
        Cathode->InvisibleAll(visible);
     }
     
     inline void SetFieldRingInvisible(bool visible = kTRUE)
     {
        // Set the visibility of the field shaping rings. kTRUE = invisible, kFALSE = visible
        FieldShapingRings->InvisibleAll(visible);
     }
     
     inline void SetReflectorInvisible(bool visible = kTRUE)
     {
        // Set the visibility of the reflector. kTRUE = invisible, kFALSE = visible
        Reflector->InvisibleAll(visible);
     }
     
     inline void SetInvisibleAll(bool visible = kTRUE)
     {
        // Set the visibility of all inactive detector elements (vessel, cathode, field shaping rings,
        // reflector)
        XeVessel->InvisibleAll(visible);
        Cathode->InvisibleAll(visible);
     	FieldShapingRings->InvisibleAll(visible);
     	Reflector->InvisibleAll(visible);
     }
     
     inline void SetWiresInvisible(int dhalf = 2, bool visible = kTRUE)
     {
        // Set the visibility of the wires for both planes individually by specifying the detector
        // half:
        //		dhalf = 0:		1st half (negative Z)
        //		dhalf = 1:		2nd half (positive Z)
        //		dhalf = 2:		both halves
        if (dhalf < 0 || dhalf > 2) {std::cout << "dhalf must be 0,1 or 2. You have chosen " << dhalf << std::endl;}
        
        if (dhalf == 0) {WireVolumeNZ->InvisibleAll(visible);}
        if (dhalf == 1) {WireVolumePZ->InvisibleAll(visible);}
        if (dhalf == 2) {
           WireVolumePZ->InvisibleAll(visible);
           WireVolumeNZ->InvisibleAll(visible);
        }
     }
     
     inline void SetAPDInvisible(int dhalf = 2, bool visible = kTRUE)
     {
        // Set the visibility of the APDs for both planes individually by specifying the detector
        // half:
        //		dhalf = 0:		1st half (negative Z)
        //		dhalf = 1:		2nd half (positive Z)
        //		dhalf = 2:		both halves
        if (dhalf < 0 || dhalf > 2) {std::cout << "dhalf must be 0,1 or 2. You have chosen " << dhalf << std::endl;}
        
        if (dhalf == 0) {APDVolumeNZ->InvisibleAll(visible);}
        if (dhalf == 1) {APDVolumePZ->InvisibleAll(visible);}
        if (dhalf == 2) {
           APDVolumePZ->InvisibleAll(visible);
           APDVolumeNZ->InvisibleAll(visible);
        }
     }

     inline void SetStackedDataDepth(double depth = 0.0)
     {
        // Set the depth of the stacked data dislplay. Bins that have entries less than depth*maxBin
        // are not displayed. This can in particular be usefull if there is some hot spots in the data
        // and many bins with few entries. To get a better view on the hot spots just increas the depth
        // value.
        StackedHistogramDepth = depth;
     }
     
     inline void Clip(int plane)
     {
        // Clip the inactive detector volumes along a plane. The allowed values for plane are
        //
        //		1	: negative y half
        //		2	: positive x half
        //		3	: positive y half
        //		4	: negative x half
        //
        // To switch off clipping call NoClipping() which is the same as Clip(5).
        if (plane < 1 || plane > 6) {std::cout << "Allowed values for clip planes are 1-6. You have chosen " << plane << std::endl; return;}
        
        bool isVesselVisible = XeVessel->IsVisible();
        bool isCathodeVisible = Cathode->IsVisible();
        bool isFieldShapingRingsVisible = FieldShapingRings->IsVisible();
        bool isReflectorVisible = Reflector->IsVisible();
        
        for (int i = 0; i < 5; i++) {
           ((TGeoVolume*)VesselVolumes->At(i))->InvisibleAll(kTRUE);
           ((TGeoVolume*)CathodeVolumes->At(i))->InvisibleAll(kTRUE);
           ((TGeoVolume*)FieldShapingRingsVolumes->At(i))->InvisibleAll(kTRUE);
           ((TGeoVolume*)ReflectorVolumes->At(i))->InvisibleAll(kTRUE);
        }
        
        if (isVesselVisible) {((TGeoVolume*)VesselVolumes->At(plane - 1))->InvisibleAll(kFALSE);}
        if (isCathodeVisible) {((TGeoVolume*)CathodeVolumes->At(plane - 1))->InvisibleAll(kFALSE);}
        if (isFieldShapingRingsVisible) {((TGeoVolume*)FieldShapingRingsVolumes->At(plane - 1))->InvisibleAll(kFALSE);}
        if (isReflectorVisible) {((TGeoVolume*)ReflectorVolumes->At(plane - 1))->InvisibleAll(kFALSE);}
        
        XeVessel = (TGeoVolume*)VesselVolumes->At(plane - 1);
        Cathode = (TGeoVolume*)CathodeVolumes->At(plane - 1);
        FieldShapingRings = (TGeoVolume*)FieldShapingRingsVolumes->At(plane - 1);
        Reflector = (TGeoVolume*)ReflectorVolumes->At(plane - 1);
        
        for (int i = 0; i < 20; i++) {
           VisibleFieldRings->AddAt((TGeoVolume*)FieldRings->At((plane-1)*20+i),i);
        }
     }
     
     inline void NoClipping()
     {
        // Turn off clipping. This is the same as calling Clip(5)
        Clip(5);
     }
     
     inline void SetCanvas(TCanvas* canvas = 0)
     {
        // Specify the canvas in whitch the non GL geometry should be drawn. This is only to be used
        // if you want to have a specific canvas, otherwise it will just create a default one.
        if (!canvas) {return;}
        
        NonGLCanvas = canvas;
        NonGLCanvas->SetName("cNonGL");
        
        return;
     }
// ------------------------------------------------------------------------------------------------

};
#endif
#endif
