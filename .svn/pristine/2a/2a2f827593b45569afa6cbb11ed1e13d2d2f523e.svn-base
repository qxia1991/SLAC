{
  gStyle->SetPalette(100);

  TFile * f;
  f = new TFile("/exo/scratch1/tests/test_acompton/newacompton.root","READ");

  TTree * Events = static_cast<TTree *>(f->Get("compton"));
  if (!Events) Events = (TTree*) f->Get("tree");

  gSystem->Load("./lib/libEXOComptonImager.so");
  EXOComptonImager im(Events);

  im.SetBinWidth(15.);
  im.SetNumToAccept(200);

  TCut badEventCut("fCompton.fHalfAnglePhi > -998.");
  TCut dphiCut("fCompton.fHalfAnglePhiErr <= TMath::TwoPi()/36.");;
  TCut peakOnlyCut("((fCompton.fEnergy < 1223.2 && fCompton.fEnergy > 1123.2) || (fCompton.fEnergy < 1372. && fCompton.fEnergy > 1292.))");
  TCut threeSiteOnlyCut("fCompton.fNumClustersUsed > 2.9 && fCompton.fNumClustersUsed < 3.1");

  TCut treeCut("");
  treeCut = badEventCut && dphiCut && peakOnlyCut && threeSiteOnlyCut;
  im.SetTreeCut(treeCut.GetTitle());

  im.DisplayCylinder(608.,500.,true);
}

