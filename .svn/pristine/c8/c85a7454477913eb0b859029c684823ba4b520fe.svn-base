{
  const Double_t length = 608.;
  const Double_t radius = 500.;
  const Double_t binWidth = 20.;

  TFile * f;
  f = new TFile("/exo/scratch1/tests/test_acompton/newacompton.root","READ");

  TTree * Events = static_cast<TTree *>(f->Get("compton"));
  if (!Events) Events = (TTree*) f->Get("tree");

  gSystem->Load("./lib/libEXOComptonImager.so");
  EXOComptonImager im(Events);

  im.SetBinWidth(binWidth);
  im.SetNumToAccept(17);

  TCut badEventCut("fCompton.fHalfAnglePhi > -998.");
  TCut dphiCut("fCompton.fHalfAnglePhiErr <= TMath::TwoPi()/36.");;
  TCut peakOnlyCut("((fCompton.fEnergy < 1223.2 && fCompton.fEnergy > 1123.2) || (fCompton.fEnergy < 1372. && fCompton.fEnergy > 1292.))");
  TCut threeSiteOnlyCut("fCompton.fNumClustersUsed > 2.9 && fCompton.fNumClustersUsed < 3.1");

  TCut treeCut("");
  treeCut = badEventCut && dphiCut && peakOnlyCut && threeSiteOnlyCut;
  im.SetTreeCut(treeCut.GetTitle());

  TH2D * minusZ_base = new TH2D();
  minusZ_base->SetName("minusZ_base");
  im.GetCylPart(*minusZ_base, ComptonIm::MinusZEndcap, length, radius);

  TCanvas * c1 = new TCanvas("-zEndcap","-z Endcap",600,600);
  gStyle->SetPalette(1);
  minusZ_base->SetStats(kFALSE);
  minusZ_base->Draw("COLZ");

  //Now let's compare to the arbitrary surface method.
  const Double_t ptsPerUnitArea = 5.;
  const Long64_t nPoints = TMath::Pi()*radius*radius*ptsPerUnitArea
                             /(binWidth*binWidth);

  Double_t * x = new Double_t[nPoints];
  Double_t * y = new Double_t[nPoints];
  Double_t * z = new Double_t[nPoints];

  const Double_t twoPi = TMath::TwoPi();
  TRandom3 random(42);
  Double_t r, theta;
  for (int i = 0; i < nPoints; i++)
  {
    theta = random->Uniform(twoPi);
    r = random->Uniform(radius*radius);
    r = TMath::Sqrt(r);
    x[i] = r*TMath::Cos(theta);
    y[i] = r*TMath::Sin(theta);
    z[i] = -1.*length/2.;
  }

  const Int_t xBins = static_cast<Int_t>(2.*radius/binWidth) + 1;
  const Int_t yBins = xBins;
  TH2D * minusZ_base_v2 = new TH2D("minusZ_base_v2", 
                                   "-z Endcap (Arbitrary Surface Method)",
                                   xBins, -1.*radius, radius,
                                   yBins, -1.*radius, radius
                                   );

  std::stringstream ss("");
  ss.precision(15);
  ss << "z < " << (-1.*length/2. + 1.1*binWidth) << " && z > " 
     << (-1.*length/2. - 1.1*binWidth);
  im.SetSpatialCut(ss.str().c_str());
  im.ArbitrarySurface(nPoints, x, y, z, ComptonIm::Xcoord, ComptonIm::Ycoord, 
                      *minusZ_base_v2);

  TCanvas * c2 = new TCanvas("-zEndcapArb","-z Endcap Arbitrary",600,600);
  minusZ_base_v2->Scale(1./ptsPerUnitArea);
  minusZ_base_v2->SetStats(kFALSE);
  minusZ_base_v2->Draw("COLZ");
}

