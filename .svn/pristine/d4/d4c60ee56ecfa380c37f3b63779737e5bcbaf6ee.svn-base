// Sample script to visualize the position-dependent light yield.
// Uses the ROOT opengl feature; only tested with ROOT 5.34.
// Running this in batch mode doesn't work due to issues with ROOT opengl.
// Usage:
// root -l -q VizLightMap.C

void ZoomHist(TH3D* inHist, TH3D* outHist)
{
  // Copy inHist into outHist, stripping the most extreme z-bins.
  // Ought to be a better way to do this, but I couldn't make anything else work.
  for(Int_t ix = 1; ix <= 40; ix++) {
    for(Int_t iy = 1; iy <= 40; iy++) {
      for(Int_t iz = 1; iz <= 38; iz++) {
        outHist->SetBinContent(ix, iy, iz, inHist->GetBinContent(ix, iy, iz+1));
      }
    }
  }
}        

void VizLightMap()
{
  gStyle->SetCanvasPreferGL(kTRUE);
  TFile file("LightMaps.root");
  TGLTH3Composition comp;

  TH3D* hist1 = file.Get("lightmap_152");
  hist1->SetFillColor(kYellow);
  TH3D* hist2 = file.Get("lightmap_200");
  hist2->SetFillColor(kBlue);
  TH3D* hist3 = file.Get("lightmap_201");
  hist3->SetFillColor(kGreen);
  TH3D* hist4 = file.Get("lightmap_198");
  hist4->SetFillColor(kOrange);
  TH3D* hist5 = file.Get("lightmap_195");
  hist5->SetFillColor(kRed);

  comp.AddTH3(hist1);
  comp.AddTH3(hist2);
  comp.AddTH3(hist3);
  comp.AddTH3(hist4);
  comp.AddTH3(hist5);

  comp.GetXaxis()->SetTitle("X (mm)");
  comp.GetYaxis()->SetTitle("Y (mm)");
  comp.GetZaxis()->SetTitle("Z (mm)");
  comp.GetXaxis()->SetTitleOffset(1.5);
  comp.GetYaxis()->SetTitleOffset(1.5);
  comp.GetZaxis()->SetTitleOffset(1.5);
  comp.Draw();
  TLegend leg(0.75, 0.7, 0.95, 0.9);
  leg.AddEntry(hist1, "Gang 152");
  leg.AddEntry(hist5, "Gang 195");
  leg.AddEntry(hist4, "Gang 198");
  leg.AddEntry(hist2, "Gang 200");
  leg.AddEntry(hist3, "Gang 201");
  leg.Draw();
  gPad->Modified();
  c1->Update();
  c1->SaveAs("Lightmap_viz.png");

  // Now produce a zoomed-in plot which excludes the extreme anode positions.
  // This makes interior variations more visible by reducing the overall scale of values;
  // Note that our fiducial volume ends at +/- 182mm, so this still is larger than our fiducial volume.
  TGLTH3Composition comp_zoom;

  TH3D* hist1_zoom = new TH3D("hist1_zoom", "hist1_zoom", 40, -200., 200., 40, -200., 200., 38, -190., 190.);
  hist1_zoom->SetFillColor(kYellow);
  ZoomHist(hist1, hist1_zoom);
  TH3D* hist2_zoom = new TH3D("hist2_zoom", "hist2_zoom", 40, -200., 200., 40, -200., 200., 38, -190., 190.);
  hist2_zoom->SetFillColor(kBlue);
  ZoomHist(hist2, hist2_zoom);
  TH3D* hist3_zoom = new TH3D("hist3_zoom", "hist3_zoom", 40, -200., 200., 40, -200., 200., 38, -190., 190.);
  hist3_zoom->SetFillColor(kGreen);
  ZoomHist(hist3, hist3_zoom);
  TH3D* hist4_zoom = new TH3D("hist4_zoom", "hist4_zoom", 40, -200., 200., 40, -200., 200., 38, -190., 190.);
  hist4_zoom->SetFillColor(kOrange);
  ZoomHist(hist4, hist4_zoom);
  TH3D* hist5_zoom = new TH3D("hist5_zoom", "hist5_zoom", 40, -200., 200., 40, -200., 200., 38, -190., 190.);
  hist5_zoom->SetFillColor(kRed);
  ZoomHist(hist5, hist5_zoom);

  comp_zoom.AddTH3(hist1_zoom);
  comp_zoom.AddTH3(hist2_zoom);
  comp_zoom.AddTH3(hist3_zoom);
  comp_zoom.AddTH3(hist4_zoom);
  comp_zoom.AddTH3(hist5_zoom);

  comp_zoom.GetXaxis()->SetTitle("X (mm)");
  comp_zoom.GetYaxis()->SetTitle("Y (mm)");
  comp_zoom.GetZaxis()->SetTitle("Z (mm)");
  comp_zoom.GetXaxis()->SetTitleOffset(1.5);
  comp_zoom.GetYaxis()->SetTitleOffset(1.5);
  comp_zoom.GetZaxis()->SetTitleOffset(1.5);
  comp_zoom.Draw();
  TLegend leg_zoom(0.75, 0.7, 0.95, 0.9);
  leg_zoom.AddEntry(hist1_zoom, "Gang 152");
  leg_zoom.AddEntry(hist5_zoom, "Gang 195");
  leg_zoom.AddEntry(hist4_zoom, "Gang 198");
  leg_zoom.AddEntry(hist2_zoom, "Gang 200");
  leg_zoom.AddEntry(hist3_zoom, "Gang 201");
  leg_zoom.Draw();
  gPad->Modified();
  c1->Update();
  c1->SaveAs("Lightmap_viz_zoom.png");
}
