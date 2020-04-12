{
  gStyle->SetPalette(100);

  TFile * f;
  f = new TFile("/exo/scratch1/tests/test_acompton/newacompton.root","READ");

  TTree * Events = static_cast<TTree *>(f->Get("compton"));
  if (!Events) Events = (TTree*) f->Get("tree");

  gSystem->Load("./lib/libEXOComptonImager.so");
  EXOComptonImager im(Events);

  im.SetBinWidth(50.);
  //If we set NumToAccept to zero, the imager will not apply any limit to the 
  //number of events to accept.
  im.SetNumToAccept(0);

  TCut badEventCut("fCompton.fHalfAnglePhi > -998.");
  TCut dphiCut("fCompton.fHalfAnglePhiErr <= TMath::TwoPi()/36.");;
  TCut peakOnlyCut("((fCompton.fEnergy < 1213.2 && fCompton.fEnergy > 1133.2) || (fCompton.fEnergy < 1362. && fCompton.fEnergy > 1302.))");
  TCut threeSiteOnlyCut("fCompton.fNumClustersUsed < 3.1 && fCompton.fNumClustersUsed > 2.9");

  TCut treeCut("");
  treeCut = badEventCut && dphiCut && peakOnlyCut && threeSiteOnlyCut;
  im.SetTreeCut(treeCut.GetTitle());

  im.DisplaySphere(304.);
}

