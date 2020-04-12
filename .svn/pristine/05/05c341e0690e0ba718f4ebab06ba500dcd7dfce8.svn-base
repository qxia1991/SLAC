//Let's use the ArbitrarySurface method of the EXOComptonImager to analyze a 
//spherical surface and compare this to the DisplaySphere method.
//
//Note that we will need to do a normalization based on the number of points 
//per unit area to make our result math the built-in utilities.

{
  const Double_t piBy2 = TMath::PiOver2();
  const Double_t twoPi = TMath::TwoPi();
  const Double_t pi = TMath::Pi();

  const Long64_t nPoints = 100000;
  const Double_t binWidth = 15.; //millimeters
  const Double_t radius = 304.;
  const Long64_t numEvents = 500;

  Double_t * x = new Double_t[nPoints];
  Double_t * y = new Double_t[nPoints];
  Double_t * z = new Double_t[nPoints];

  Double_t phi, theta;
  TRandom3 * random = new TRandom3(42);
  for (Long64_t i = 0; i < nPoints; i++)
  {
    phi = random->Uniform(twoPi);
    theta = random->Uniform(-1.,1.);
    theta = asin(theta) + piBy2;
    x[i] = radius*cos(phi)*sin(theta);
    y[i] = radius*sin(phi)*sin(theta);
    z[i] = radius*cos(theta);
  }

  TFile * f;
  f = new TFile("/exo/scratch1/tests/test_acompton/newacompton.root","READ");
  TTree * Events = static_cast<TTree *>(f->Get("compton"));
  if (!Events) Events = (TTree*) f->Get("tree");

  gSystem->Load("./lib/libEXOComptonImager.so");
  EXOComptonImager im(Events);
  im.SetBinWidth(binWidth);
  im.SetNumToAccept(numEvents);

  TCut badEventCut("fCompton.fHalfAnglePhi > -998.");
  TCut dphiCut("fCompton.fHalfAnglePhiErr <= TMath::TwoPi()/36.");;
  TCut peakOnlyCut("((fCompton.fEnergy < 1223.2 && fCompton.fEnergy > 1123.2) || (fCompton.fEnergy < 1372. && fCompton.fEnergy > 1292.))");
  TCut threeSiteOnlyCut("fCompton.fNumClustersUsed > 2.9 && fCompton.fNumClustersUsed < 3.1");

  TCut treeCut("");
  treeCut = badEventCut && dphiCut && peakOnlyCut && threeSiteOnlyCut;
  im.SetTreeCut(treeCut.GetTitle());

  const Int_t xBins = static_cast<Int_t>(1.*radius*twoPi/binWidth) + 1;
  const Int_t yBins = xBins/2 + 1;
  TH2D * sphere = new TH2D("sphere", "Arbitrary Surface (sphere)",
                           xBins, -1.*TMath::Pi(), TMath::Pi(),
                           yBins, -1.*piBy2, piBy2);

  im.ArbitrarySurface(nPoints, x, y, z, ComptonIm::SinusoidalX, 
                      ComptonIm::SinusoidalY, *sphere);
  //Note--if you do the math, using "1 square BinWidth" as the unit of area, 
  //we have about 19.4 points per unit area.
  Double_t ptsPerUnitA = nPoints/(4.*pi*radius/binWidth*radius/binWidth);
  sphere->Scale(1./ptsPerUnitA);

  gStyle->SetPalette(1);
  sphere->SetStats(kFALSE);
  sphere->Draw("COLZ");

  //No point in regenerating if we haven't changed any parameters.
  const bool regen = false;
  im.DisplaySphere(radius, regen);

  return;
}


