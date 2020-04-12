// Run like:
// root -b -q 'PlotGainVsTime.C(152, "out.png")'

void PlotGainVsTime(Int_t channel, const char* outfile)
{
  gStyle->SetOptStat(0);
  TFile file("LightMaps.root");
  TGraph* graph = file.Get(Form("gainmap_%i", channel));
  Double_t Min = 1, Max = 1;
  for(Int_t i = 0; i < graph->GetN(); i++) {
    Double_t x,y;
    graph->GetPoint(i, x, y);
    if(y < Min) Min = y;
    if(y > Max) Max = y;
  }
  Min = 0.97*Min;
  Max = 1.02*Max;

  TCanvas c;
  TH2D empty_hist("empty_hist", Form("Gain on Channel %i", channel), 100, 1317427200, 1385856000, 100, Min, Max);
  empty_hist.GetYaxis()->SetTitle("Gain (arb. units)");
  empty_hist.GetXaxis()->SetTitle("Time");
  empty_hist.GetXaxis()->SetTimeDisplay(1);
  empty_hist.GetXaxis()->SetTimeFormat("#splitline{%b %d}{%Y}");
  empty_hist.GetXaxis()->SetTimeOffset(0, "gmt");
  empty_hist.GetXaxis()->SetLabelOffset(0.025);
  empty_hist.GetXaxis()->SetTitleOffset(1.6);
  c.SetBottomMargin(0.12);
  empty_hist.Draw();

  graph->Draw("L,same");
  c.SaveAs(outfile);
}
