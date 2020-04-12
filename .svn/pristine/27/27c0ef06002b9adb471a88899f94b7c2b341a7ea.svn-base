{
TH1F *h1 = new TH1F("h1","",100,0.0,5000.0);
TH1F *h2 = new TH1F("h2","",100,0.0,5000.0);
TH1F *h3 = new TH1F("h3","",100,0.0,5000.0);
TH1F *h4 = new TH1F("h4","",100,0.0,5000.0);
TH1F *h5 = new TH1F("h5","",100,0.0,5000.0);
TH1F *h6 = new TH1F("h6","",100,0.0,5000.0);
TH1F *h7 = new TH1F("h7","",100,0.0,5000.0);
TH1F *h8 = new TH1F("h8","",100,0.0,5000.0);

tree->Draw("ecrec>>h1","elxet!=0.0");
tree->Draw("ecrec>>h2","elxet!=0.0&&nsite==1");
tree->Draw("ecrec>>h3","elxet!=0.0&&nsite==1&&tdcl==1");
tree->Draw("ecrec>>h4","elxet!=0.0&&nsite==1&&tdcl==1&&fidev==1");
tree->Draw("ecrec>>h5","elxet!=0.0&&nsite==1&&tdcl==1&&fidev==1&&nsc==1&&algsc[0]==0"); 
tree->Draw("ecrec>>h6","elxet!=0.0&&nsite==1&&tdcl==1&&fidev==1&&nsc==1&&algsc[0]==0&&alpha==0"); 
tree->Draw("ecrec>>h7","elxet!=0.0&&nsite==1&&tdcl==1&&fidev==1&&nsc==1&&algsc[0]==0&&alpha==0&&bb2n==1"); 
tree->Draw("ecrec>>h8","elxet!=0.0&&nsite==1&&tdcl==1&&fidev==1&&nsc==1&&algsc[0]==0&&alpha==0&&bb0n==1"); 

double maximum = 15.0*h1->GetMaximum();

max(1,maximum);
max(2,maximum);
max(3,maximum);
max(4,maximum);
max(5,maximum);
max(6,maximum);
max(6,maximum);
max(7,maximum);
max(8,maximum);
min(1,0.1);
min(2,0.1);
min(3,0.1);
min(4,0.1);
min(5,0.1);
min(6,0.1);
min(7,0.1);
min(8,0.1);
gStyle->SetPaperSize(20,24);
opt_logy();

zon(2,4);
set_opt_stat(1000100);

atitle(1,"keV","events per 50 keV");
atitle(2,"keV","events per 50 keV");
atitle(3,"keV","events per 50 keV");
atitle(4,"keV","events per 50 keV");
atitle(5,"keV","events per 50 keV");
atitle(6,"keV","events per 50 keV");
atitle(7,"keV","events per 50 keV");
atitle(8,"keV","events per 50 keV");

set_chhe(0.05);
set_ndc(true);


hplot(1);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"all events");
hplot(2);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"single site events");
hplot(3);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D reconstruction");
hplot(4);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D + fiducial");
hplot(5);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D + fid + dual APD cluster");
hplot(6);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D + fid + APD + not alpha event");
hplot(7);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D + fid + APD + not `a#");
itx(0.25,0.77,"+  `bb#2`n# energy cut");
hplot(8);
itx(0.25,0.91,"total reconstructed energy");
itx(0.25,0.82,"SSE + 3D + fid + APD + not `a#");
itx(0.25,0.77,"+  `bb#0`n# energy cut");

cout << "******************************************" << endl;
cout << "Total number of events...." << h1->GetSum() << endl;
cout << "Single site events........" << h2->GetSum() << endl;
cout << "+ 3D reconstruction......." << h3->GetSum() << endl;
cout << "+ fiducial cut............" << h4->GetSum() << endl;
cout << "+ dual APD cluster........" << h5->GetSum() << endl;
cout << "+ not an alpha decay......" << h6->GetSum() << endl;
cout << "+ bb2n energy cuts........" << h7->GetSum() << endl;
cout << "+ bb0n energy cuts........" << h8->GetSum() << endl;
cout << "******************************************" << endl;

c1->Print("efficiency.eps");

}


