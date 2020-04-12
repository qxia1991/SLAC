#include "EXOComptonImager/EXOComptonImager.hh"
#include "EXOUtilities/EXOComptonInfo.hh"
#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TFormula.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using TMath::Abs;
using TMath::Sin;
using TMath::Cos;
using TMath::Sqrt;
using TMath::ACos;
using TMath::ASin;
using namespace ComptonIm;

const Double_t EXOComptonImager::ComptonCone::fTolerance = 0.001;

const Double_t EXOComptonImager::fPROXFACTOR = 2.;
const Double_t EXOComptonImager::fCONENORM = 100.;
//Note: all lengths are millimeters
const Double_t EXOComptonImager::fTPCRADIUS = 183.4;
const Double_t EXOComptonImager::fTPCLENGTH = 408.814;
const Double_t EXOComptonImager::fMINBINWIDTH = 1.;
const int EXOComptonImager::fMAXCONES = 1000;

EXOComptonImager::ComptonCone::ComptonCone(const EXOComptonImager & imager) : 
fImager(imager),
fIsSet(false)
{}

EXOComptonImager::ComptonCone::~ComptonCone()
{}

void EXOComptonImager::ComptonCone::SetCone(Double_t cone[7])
{
  for (int i = 0; i < 3; i++)
  { 
    fCone[i] = cone[i];
    fCone[i+3] = cone[i+3];
    fU[i] = fCone[i] - fCone[i+3];
  }
  fCone[6] = cone[6];

  //Make fU into a unit vector. Note that it points from 
  //the second scatter back towards the first.
  Normalize(fU);

  //Make sure that the given phi value is in the 
  //appropriate range.
  while (fCone[6] < 0.) 
    fCone[6] += twoPi;
  while (fCone[6] >= twoPi) 
    fCone[6] -= twoPi;
  if (fCone[6] > pi) fCone[6] = twoPi - fCone[6];

  fSinPhi = Sin(fCone[6]);
  fCosPhi = Cos(fCone[6]);

  //Check for when cone is along z-axis--in this case we must 
  //use a different formula for the cone to avoid dividing by 
  //zero. This is an artifact of deriving the general equations 
  //by rotating a cone initially aligned with the z-axis.
  fTrivFlag = ((fU[0]*fU[0] + fU[1]*fU[1]) <= fTolerance);

  //Calculate some constants for later use
  Double_t sqsum;
  if (!fTrivFlag)
  {
    sqsum = fU[0]*fU[0] + fU[1]*fU[1];

    fCC[0] = fU[1]*fU[1]*(1.-fU[2])/sqsum;
    fCC[0] += fU[2];
    fCC[0] *= fSinPhi;

    fCC[1] = fU[0]*fU[1]*(fU[2]-1.)/sqsum;
    fCC[1] *= fSinPhi;

    fCC[2] = fU[0]*fU[0]*(1.-fU[2])/sqsum;
    fCC[2] += fU[2];
    fCC[2] *= fSinPhi;

    fCC[3] = -1.*fU[0]*fSinPhi;
    fCC[4] = -1.*fU[1]*fSinPhi;
  }
  else
  {
    //Switch to parametrization based on starting from x-axis
    sqsum = fU[1]*fU[1] + fU[2]*fU[2];

    fCC[0] = -1.*fU[1]*fSinPhi;
    fCC[1] = -1.*fU[2]*fSinPhi;

    fCC[2] = (1.-fU[0])*fU[2]*fU[2]/sqsum;
    fCC[2] += fU[0];
    fCC[2] *= fSinPhi;

    fCC[3] = (fU[0]-1.)*fU[1]*fU[2]/sqsum;
    fCC[3] *= fSinPhi;

    fCC[4] = (1.-fU[0])*fU[1]*fU[1]/sqsum;
    fCC[4] += fU[0];
    fCC[4] *= fSinPhi;
  }
  
  fIsSet = true;
  return;
}

void EXOComptonImager::ComptonCone::Evaluate(Double_t len, Double_t theta, 
                                             Double_t & x, Double_t & y, 
                                             Double_t & z)
{
  if (!fIsSet) 
  {
    x = y = z = 0.;
    return;
  }

  if (fTrivFlag)
  {
    x = len*(fU[0]*fCosPhi + fCC[0]*Cos(theta) + fCC[1]*Sin(theta)) + fCone[0];
    y = len*(fU[1]*fCosPhi + fCC[2]*Cos(theta) + fCC[3]*Sin(theta)) + fCone[1];
    z = len*(fU[2]*fCosPhi + fCC[3]*Cos(theta) + fCC[4]*Sin(theta)) + fCone[2];
  }
  else
  {
    x = len*(fU[0]*fCosPhi + fCC[0]*Cos(theta) + fCC[1]*Sin(theta)) + fCone[0];
    y = len*(fU[1]*fCosPhi + fCC[1]*Cos(theta) + fCC[2]*Sin(theta)) + fCone[1];
    z = len*(fU[2]*fCosPhi + fCC[3]*Cos(theta) + fCC[4]*Sin(theta)) + fCone[2];
  }

  return;
}

bool EXOComptonImager::ComptonCone::CheckProximity(const Double_t * point) const
{
  if (!fIsSet) return false;

  Double_t r[3];
  r[0] = point[0] - fCone[0];
  r[1] = point[1] - fCone[1];
  r[2] = point[2] - fCone[2];

  Double_t norm = Normalize(r);

  Double_t uDotr = fU[0]*r[0] + fU[1]*r[1] + fU[2]*r[2];

  if ((uDotr >= 0. && fCone[6] <= piBy2) || (uDotr <= 0. && fCone[6] >= piBy2))
  {
    if (norm*Sin(Abs(ACos(uDotr) - fCone[6])) < fImager.Close())
      return true;
  }

  return false;
}

Double_t EXOComptonImager::ComptonCone::GetParam(int idx)
{ 
  if (idx < 0 || idx > 6) return 0.;
  return fCone[idx];
}

EXOComptonImager::EXOComptonImager() : 
  fCompCone(ComptonCone(*this))
{
  Init();
}

EXOComptonImager::EXOComptonImager(TTree * Events, std::string TreeCut) :
  fCompCone(ComptonCone(*this))
{
  Init();
  fEvents = Events;
  fEvents->SetBranchAddress("fCompton", &fComptonInfo);
  fTreeCutForm = new TTreeFormula("EXOComptonImager::fTreeCutForm", TreeCut.c_str(), fEvents);
  fEvents->SetNotify(fTreeCutForm);
}

EXOComptonImager::~EXOComptonImager()
{
  if (fSpace) delete fSpace;
  if (fCylEnd1) delete fCylEnd1;
  if (fCylEnd2) delete fCylEnd2;
  if (fCylSide) delete fCylSide;
  if (fComboCyl) delete fComboCyl;
  if (fSphereProj) delete fSphereProj;
  if (fCirc1) delete fCirc1;
  if (fCirc2) delete fCirc2;
  if (fCirc3) delete fCirc3;
  if (fTPCcut) delete fTPCcut;
  if (fUserSpatialCut) delete fUserSpatialCut;
  if (fSpatialCutForm) delete fSpatialCutForm;
  if (fTreeCutForm) delete fTreeCutForm;
  if (fComptonInfo) delete fComptonInfo;
}

void EXOComptonImager::Init()
{
  fEvents = 0;
  fComptonInfo = NULL;
  fSpace = 0;
  fCylEnd1 = 0;
  fCylEnd2 = 0;
  fCylSide = 0;
  fComboCyl = 0;
  fCathode = 0;
  fSphereProj = 0;
  fCirc1 = 0;
  fCirc2 = 0;
  fCirc3 = 0;
  fTPCcut = 0;
  fUserSpatialCut = 0;
  fSpatialCutForm = 0;
  fTreeCutForm = 0;
  fBinWidth = 15.;
  fNumToAccept = 0;
  fPtsPerBin = 4;
  fExcludeTPC = true;
  fDoPlusZEndcap = true;
  fDoMinusZEndcap = true;
  fDoLateralArea = true;

  std::stringstream cut("");
  cut << "(x*x + y*y > ";
  cut.precision(15);
  cut << fTPCRADIUS*fTPCRADIUS << ") || (z*z > ";
  cut << fTPCLENGTH*fTPCLENGTH/4. << ")";
  fTPCcut = new TCut("EXOComptonImager::fTPCcut",cut.str().c_str());
  fUserSpatialCut = new TCut("EXOComptonImager::fUserSpatialCut","1");
  fSpatialCutForm = new TFormula("EXOComptonImager::fSpatialCutForm",
                             fTPCcut->GetTitle());
  fTreeCutForm = NULL;
}

int EXOComptonImager::GetFilled3DHist(TH3D & hist, Double_t size, 
                                      bool regen)
{
  int ret = 0;
  if (regen || fSpace == 0) ret = Fill3DHistMC(size);
  if (ret != 0) return ret;
  hist = *fSpace;
  return 0;
}

int EXOComptonImager::GetTPCPart(TH2D & hist, CylPart part, 
                                 bool regen)
{
  int ret = GetCylPart(hist, part, fTPCLENGTH, fTPCRADIUS, regen);
  if (ret != 0) 
  {
    ReportError("EXOComptonImager::GetTPCPart", ret);
    return ret;
  }

  if (part == PlusZEndcap)
  {}
  else if (part == MinusZEndcap)
  {}
  else if (part == LateralArea)
  {
    hist.SetTitle("Side of TPC");
  }
  else
  {}

  return 0;
}

int EXOComptonImager::GetCylPart(TH2D & hist, CylPart part, 
                                 Double_t length, Double_t radius, 
                                 bool regen)
{
  switch (part)
  {
    case PlusZEndcap:
      fDoPlusZEndcap = true;
      fDoMinusZEndcap = fDoLateralArea = false;
      break;

    case MinusZEndcap:
      fDoMinusZEndcap = true;
      fDoPlusZEndcap = fDoLateralArea = false;
      break;

    case LateralArea:
      fDoLateralArea = true;
      fDoPlusZEndcap = fDoMinusZEndcap = false;
      break;

    default:
      return 3;
  }

  int ret = DoCylinder(length, radius);
  if (ret != 0) 
  {
    ReportError("EXOComptonImager::GetCylPart", ret);
    return ret;
  }

  switch (part)
  {
    case PlusZEndcap:
      hist = *fCylEnd1;
      Int_t xBins; xBins = hist.GetNbinsX();
      Int_t yBins; yBins = hist.GetNbinsY();
      Double_t temp;
      for (Int_t i = 1; i <= yBins; i++)
      {
        for (Int_t j = 1; j <= xBins/2; j++)
        {
          temp = hist.GetBinContent(j, i);
          hist.SetBinContent(j, i, hist.GetBinContent(xBins - j + 1, i));
          hist.SetBinContent(xBins - j + 1, i, temp);
        }
      }
      hist.GetXaxis()->SetTitle("x [mm]");
      hist.GetYaxis()->SetTitle("y [mm]");
      hist.GetYaxis()->SetTitleOffset(1.2);
      hist.SetTitle("+z Endcap");
      break;

    case MinusZEndcap:
      hist = *fCylEnd2;
      hist.GetXaxis()->SetTitle("x [mm]");
      hist.GetYaxis()->SetTitle("y [mm]");
      hist.GetYaxis()->SetTitleOffset(1.2);
      hist.SetTitle("-z Endcap");
      break;

    case LateralArea:
      hist = *fCylSide;
      hist.GetXaxis()->SetTitle("z [mm]");
      hist.GetYaxis()->SetTitle("");
      break;
  }

  fDoPlusZEndcap = fDoMinusZEndcap = fDoLateralArea = true;
  return 0;
}

int EXOComptonImager::GetCathode(TH2D & hist, bool regen)
{
  if (regen || fCathode == 0) 
  {
    int ret = DoCathode(fTPCRADIUS);
    if (ret != 0)
    {
      ReportError("EXOComptonImager::GetCathode", ret);
      return ret;
    }
  }
  
  hist = *fCathode;
  hist.SetStats(kFALSE);
  hist.GetXaxis()->SetTitle("x [mm]");
  hist.GetYaxis()->SetTitle("y [mm]");
  hist.SetTitle("Cathode");

  return 0;
}

int EXOComptonImager::DisplaySphere(Double_t radius, bool regen)
{
  int ret;
  if (regen && (ret = Fill3DHistMC(radius + 2.*fBinWidth))) 
  {
    ReportError("EXOComptonImager::DisplaySphere", ret);
    return ret;
  }

  if (fSphereProj) delete fSphereProj;

  //Sinusoidal projection
  Int_t xBins = static_cast<Int_t>(radius*twoPi/fBinWidth) + 1;
  Int_t yBins = xBins/2 + 1;
  fSphereProj = new TH2D("sphereProj","Sphere Projection",xBins,-1.*pi,pi,
                           yBins,-1.*pi/2.,pi/2.);
  fSphereProj->SetDirectory(0);
  Double_t phi, theta, x, y, z;
  TRandom3 * random = new TRandom3();
  const Double_t ptsPerUnitA = 100.;
  const Double_t projBinW_x = fSphereProj->GetXaxis()->GetBinWidth(1);
  const Double_t projBinW_y = fSphereProj->GetYaxis()->GetBinWidth(1);
  const int numPts = static_cast<int>(
                       4.*pi*ptsPerUnitA/(projBinW_x*projBinW_y));

  Double_t weight;
  for (int i = 0; i < numPts; i++)
  {
    phi = random->Uniform(twoPi);
    theta = random->Uniform(-1.,1.);
    theta = ASin(theta) + piBy2;
    x = radius*Cos(phi)*Sin(theta);
    y = radius*Sin(phi)*Sin(theta);
    z = radius*Cos(theta);
    weight = fSpace->GetBinContent(
      fSpace->GetXaxis()->FindBin(x),
      fSpace->GetYaxis()->FindBin(y),
      fSpace->GetZaxis()->FindBin(z)
      );
    if (phi > pi) phi -= twoPi;
    fSphereProj->Fill(phi*Cos(-1.*theta + piBy2), 
                      -1.*theta + piBy2, 
                      weight);
  }

  fSphereProj->GetXaxis()->SetTitle("x = longitude*cos(y)");
  fSphereProj->GetYaxis()->SetTitle("y = pi/2 - theta");
  fSphereProj->Scale(1./ptsPerUnitA);
  fSphereProj->SetStats(kFALSE);
  new TCanvas("EXOCI Sphere Projection", "Sphere Projection", 700, 500);
  fSphereProj->Draw("COLZ");
  return 0;
}

int EXOComptonImager::ArbitrarySurface(Long64_t nPoints, 
                                       Double_t * x, 
                                       Double_t * y, 
                                       Double_t * z, 
                                       Double_t (* xProj) 
                                         (Double_t x, Double_t y, Double_t z), 
                                       Double_t (* yProj) 
                                         (Double_t x, Double_t y, Double_t z), 
                                       TH2D & hist, bool regen)
{
  if (fEvents == 0) return 1;
  double size = 0., util;
  for (Long64_t i = 0; i < nPoints; i++)
  {
    util = Sqrt(x[i]*x[i] + y[i]*y[i] + z[i]*z[i]);
    size = (size > util) ? size : util;
  }
  int ret = 0; 
  if (fSpace == 0 || regen) ret = Fill3DHistMC(size + 2.*fBinWidth);
  if (ret != 0) return ret;

  Double_t weight;
  for (Long64_t i = 0; i < nPoints; i++)
  {
    weight = fSpace->GetBinContent(
      fSpace->GetXaxis()->FindBin(x[i]),
      fSpace->GetYaxis()->FindBin(y[i]),
      fSpace->GetZaxis()->FindBin(z[i])
      );
    
    hist.Fill(xProj(x[i], y[i], z[i]), yProj(x[i], y[i], z[i]), weight);
  }

  return 0;
}


void EXOComptonImager::DisplayTPC(bool regen)
{
  int ret = 0;
  if (regen || fCylSide == 0 || fCylEnd1 == 0 || fCylEnd2 == 0) 
    ret = DoCylinder(fTPCLENGTH, fTPCRADIUS);
  if (ret != 0) 
  {
    ReportError("EXOComptonImager::DisplayTPC", ret);
    return;
  }
  Int_t width = 600, height = 600;
  TCanvas * c1 = new TCanvas("TPC", "TPC", width, height);
  c1->cd();
  fComboCyl->SetTitle("TPC");
  fComboCyl->SetStats(kFALSE);
  fComboCyl->Draw("COLZ");
  MakeCirclesForEndcaps(fTPCRADIUS);
  fCirc1->Draw("SAMEL");
  fCirc2->Draw("SAMEL");
  return;
}

void EXOComptonImager::DisplayCylinder(Double_t length, 
                                       Double_t radius, 
                                       bool regen)
{
  int ret = 0;
  if (regen || fCylSide == 0 || fCylEnd1 == 0 || fCylEnd2 == 0) 
    ret = DoCylinder(length, radius);
  if (ret != 0) 
  {
    ReportError("EXOComptonImager::DisplayCylinder", ret);
    return;
  }
  Int_t width = 600, height = 600;
  TCanvas * c1 = new TCanvas("Cylinder", "Cylinder", width, height);
  c1->cd();
  fComboCyl->SetTitle("Cylinder");
  fComboCyl->SetStats(kFALSE);
  fComboCyl->Draw("COLZ");
  MakeCirclesForEndcaps(radius);
  fCirc1->Draw("SAMEL");
  fCirc2->Draw("SAMEL");
  return;
}

void EXOComptonImager::DisplayCathode(bool regen)
{
  int ret = 0;;
  if (regen || fCathode == 0) ret = DoCathode(fTPCRADIUS);
  if (ret != 0) 
  {
    ReportError("EXOComptonImager::DisplayCathode", ret);
    return;
  }

  Int_t width = 600, height = 600;
  TCanvas * c1 = new TCanvas("EXOCICathode","Cathode",width,height);

  fCathode->SetStats(kFALSE);
  fCathode->GetXaxis()->SetTitle("x [mm]");
  fCathode->GetYaxis()->SetTitle("y [mm]");
  fCathode->SetTitle("Cathode");
  c1->cd();
  fCathode->Draw("COLZ");
  const Int_t numPts = static_cast<Int_t>(twoPi*fTPCRADIUS/0.001);
  if (fCirc3) delete fCirc3;
  fCirc3 = new TGraph(numPts);
  Double_t theta;
  for (Int_t i = 0; i < numPts; i++)
  {
    theta = static_cast<Double_t>(i)/static_cast<Double_t>(numPts);
    theta *= twoPi;
    fCirc3->SetPoint(i, fTPCRADIUS*Cos(theta), fTPCRADIUS*Sin(theta));
  }
  fCirc3->SetLineColor(2);
  fCirc3->SetLineWidth(2);
  fCirc3->Draw("SAMEL");
  return;
}

void EXOComptonImager::SetBinWidth(Double_t binWidth)
{ fBinWidth = (binWidth < fMINBINWIDTH) ? fMINBINWIDTH : binWidth; return; }

void EXOComptonImager::SetNumToAccept(unsigned int num)
{ fNumToAccept = num; return; }

void EXOComptonImager::SetExcludeTPC(bool exclude)
{
  if (exclude == fExcludeTPC) return;
  fExcludeTPC = exclude;
  if (fExcludeTPC) 
    fSpatialCutForm->Compile(((*fUserSpatialCut)+(*fTPCcut)).GetTitle());
  else fSpatialCutForm->Compile(fUserSpatialCut->GetTitle());
  return;
}

void EXOComptonImager::SetSpatialCut(const char * cut)
{
  if (cut[0] == '\0') fUserSpatialCut->SetTitle("1");
  else fUserSpatialCut->SetTitle(cut);

  if (fExcludeTPC)
    fSpatialCutForm->Compile(((*fUserSpatialCut)+(*fTPCcut)).GetTitle());
  else fSpatialCutForm->Compile(fUserSpatialCut->GetTitle());
  return;
}

Double_t EXOComptonImager::GetBinWidth() const
{ return fBinWidth; }

unsigned int EXOComptonImager::GetNumToAccept() const
{ return fNumToAccept; }

bool EXOComptonImager::GetExcludeTPC() const
{ return fExcludeTPC; }

TString EXOComptonImager::GetSpatialCut() const
{ return fSpatialCutForm->GetExpFormula(); }

TString EXOComptonImager::GetTreeCut() const
{ return fTreeCutForm->GetExpFormula(); }

Double_t EXOComptonImager::Close() const
{ return fBinWidth*fPROXFACTOR; }

int EXOComptonImager::Fill3DHistMC(Double_t size)
{
  if (fEvents == 0) return 1;

  if (size < fBinWidth) return 2;

  Double_t utility = 0.;
  Int_t nBins = 0;

  while (utility < size) { utility += fBinWidth; nBins += 2; }
  const Double_t xmax = static_cast<Double_t>(nBins)*fBinWidth;
  const Double_t xmin = -1.*xmax;
  const Double_t ymax = xmax;
  const Double_t ymin = xmin;
  const Double_t zmax = xmax;
  const Double_t zmin = xmin;

  const Int_t xbins = nBins;
  const Int_t ybins = nBins;
  const Int_t zbins = nBins;

  if (fSpace) delete fSpace;
  fSpace = new TH3D("fSpace", "Space", xbins, xmin, xmax, 
                    ybins, ymin, ymax, zbins, zmin, zmax);
  fSpace->SetDirectory(0);
  
  const unsigned int numEvents 
    = static_cast<unsigned int>(fEvents->GetEntries());
  //ordering: x1, y1, z1, x2, y2, z2, phi
  Double_t cone[7];

  TRandom3 * random = new TRandom3();

  //len and theta are the parameters for the cones. len is distance 
  //along the cone's side and theta is the azimuthal angle.
  Double_t len, theta, lenMax, x, y, z, w;
  //The parameter width determines how closely the point-mesh 
  //generating algorithm places points on the cone. As can be 
  //seen, width is determined by the bin widths of fSpace.
  const Double_t width = fBinWidth;
  int numCones, numPoints, numTheta;
  bool allOut, allIn; 
  const Double_t ptsPerUnitArea = 5.;
  const Double_t length = 2.*Sqrt(xmax*xmax + ymax*ymax + zmax*zmax);;
  const unsigned int numToAccept 
    = (fNumToAccept == 0) ? numEvents : fNumToAccept;
  for (unsigned int i = 0, numAccepted = 0; 
       numAccepted < numToAccept && i < numEvents; i++)
  {
    fEvents->GetEntry(i);
    if (fTreeCutForm->EvalInstance() < 0.5) continue;

    numCones = CalcNumCones(length);
    if (numCones > fMAXCONES) continue;
    numAccepted++;
    if (numAccepted%10 == 0) std::cout << std::setw(5) << numAccepted 
                                       << " events accepted." << std::endl;
    w = fCONENORM/static_cast<Double_t>(numCones);

    //Loop over randomly generated cones
    for (int j = 0; j < numCones; j++)
    {
      //Randomize cone variables.
      cone[0] = random->Gaus(fComptonInfo->fX1, fComptonInfo->fX1Err);
      cone[1] = random->Gaus(fComptonInfo->fY1, fComptonInfo->fY1Err);
      cone[2] = random->Gaus(fComptonInfo->fZ1, fComptonInfo->fZ1Err);
      cone[3] = random->Gaus(fComptonInfo->fX2, fComptonInfo->fX2Err);
      cone[4] = random->Gaus(fComptonInfo->fY2, fComptonInfo->fY2Err);
      cone[5] = random->Gaus(fComptonInfo->fZ2, fComptonInfo->fZ2Err);
      cone[6] = random->Gaus(fComptonInfo->fHalfAnglePhi, fComptonInfo->fHalfAnglePhiErr);
      fCompCone.SetCone(cone);

      //Determine lenMax -- very important
      lenMax = 4.*Sqrt(xmax*xmax + ymax*ymax + zmax*zmax);
      allIn = false;
      while (!allIn && lenMax > width)
      {
        allIn = true;
        lenMax /= 1.5;
        utility = 1.1*lenMax*Sin(fCompCone.GetParam(6))*twoPi;
        utility /= width;
        numTheta = static_cast<int>(Abs(utility));
        ++numTheta;
        for (int k = 0; k < numTheta; k++)
        {
          theta = twoPi*static_cast<Double_t>(k);
          theta /= static_cast<Double_t>(numTheta);
          fCompCone.Evaluate(lenMax,theta,x,y,z);
          if (x > xmax || x < xmin || y > ymax || y < ymin 
              || z > zmax || z < zmin)
          {
            allIn = false;
            break;
          }
        }
      }
      allOut = false;
      while (!allOut && lenMax < 2.*size)
      {
        allOut = true;
        lenMax *= 1.05;
        utility = 1.1*lenMax*Sin(fCompCone.GetParam(6))*twoPi;
        utility /= width;
        numTheta = static_cast<int>(Abs(utility));
        ++numTheta;
        for (int k = 0; k < numTheta; k++)
        {
          theta = twoPi*static_cast<Double_t>(k);
          theta /= static_cast<Double_t>(numTheta);
          fCompCone.Evaluate(lenMax,theta,x,y,z);
          if (x < xmax && x > xmin && y < ymax && y > ymin 
              && z < zmax && z > zmin)
          {
            allOut = false;
            break;
          }
        }
      }

      //constant #/unit surface area
      utility = Abs(pi*lenMax*lenMax*Sin(fCompCone.GetParam(6)));
      utility /= width*width;
      utility *= ptsPerUnitArea;
      numPoints = static_cast<int>(utility);

      for (int k = 0; k < numPoints; k++)
      {
        len = random->Uniform(lenMax*lenMax);
        len = Sqrt(len);
        theta = random->Uniform(twoPi);

        fCompCone.Evaluate(len,theta,x,y,z);

        if (fSpatialCutForm->Eval(x,y,z) < 0.5) continue;
        if (x > xmax || x < xmin || y > ymax || y < ymin 
            || z > zmax || z < zmin)
        { continue; }

        fSpace->Fill(x,y,z,w);
      }
    }
  }

  fSpace->Scale(1./ptsPerUnitArea);
  delete random;
  return 0;
}

int EXOComptonImager::DoCylinder(Double_t length, Double_t radius)
{
  if (fEvents == 0) return 1;

  if (fCylEnd1) delete fCylEnd1;
  if (fCylEnd2) delete fCylEnd2;
  if (fCylSide) delete fCylSide;
  if (fComboCyl) delete fComboCyl;

  const Double_t lenBy2 = length/2.;
  const Double_t minusLenBy2 = -1.*lenBy2;

  Double_t util;

  int nBinsX=0, nBinsY=0;
  Double_t xMax=0., yMax=0.;
  while (xMax < radius) {xMax+=fBinWidth; nBinsX+=2;}
  yMax = xMax; nBinsY = nBinsX;
  const int nBinsCirc = nBinsY*nBinsX;
  fCylEnd1 = new TH2D("cylEnd1","cylEnd1",
                      nBinsX,-1.*xMax,xMax,
                      nBinsY,-1.*yMax,yMax);
  fCylEnd1->SetDirectory(0);
  fCylEnd2 = new TH2D("cylEnd2","cylEnd2",
                      nBinsX,-1.*xMax,xMax,
                      nBinsY,-1.*yMax,yMax);
  fCylEnd2->SetDirectory(0);
  nBinsX = nBinsY = 0;
  xMax = yMax = 0.;
  while (xMax < lenBy2) {xMax+=fBinWidth; nBinsX+=2;}
  while (yMax < radius*pi) {yMax+=fBinWidth; nBinsY+=2;}
  const int nBinsSide = nBinsX*nBinsY;
  //"x-axis" of fCylSide is real z-axis
  //"y-axis" is arc length along circumference
  fCylSide = new TH2D("cylSide","cylSide",
                      nBinsX,-1.*xMax,xMax,
                      nBinsY,-1.*yMax,yMax);
  fCylSide->SetDirectory(0);

  nBinsX = fCylSide->GetXaxis()->GetNbins() 
           + 2*fCylEnd1->GetXaxis()->GetNbins();
  xMax = fCylSide->GetXaxis()->GetXmax() + 2.*fCylEnd1->GetXaxis()->GetXmax();
  nBinsY = fCylSide->GetYaxis()->GetNbins();
  yMax = fCylSide->GetYaxis()->GetXmax();
  if (yMax > xMax) { xMax = yMax; nBinsX = nBinsY; }
  else { yMax = xMax; nBinsY = nBinsX; }
  fComboCyl = new TH2D("comboCyl","comboCyl",
                       nBinsX,-1.*xMax,xMax,
                       nBinsY,-1.*yMax,yMax);
  fComboCyl->SetDirectory(0);

  TRandom3 * random = new TRandom3();

  std::vector<Double_t> & vxC = *(new std::vector<Double_t>());
  vxC.reserve(fPtsPerBin*nBinsCirc);
  std::vector<Double_t> & vyC = *(new std::vector<Double_t>());
  vyC.reserve(fPtsPerBin*nBinsCirc);
  std::vector<Double_t> & vsS = *(new std::vector<Double_t>());
  vsS.reserve(fPtsPerBin*nBinsSide);
  std::vector<Double_t> & vzS = *(new std::vector<Double_t>());
  vzS.reserve(fPtsPerBin*nBinsSide);

  const Double_t rsq = radius*radius;

  TAxis * xAxis = fCylEnd1->GetXaxis();
  const int numCircXBins = xAxis->GetNbins();
  TAxis * yAxis = fCylEnd1->GetYaxis();
  const int numCircYBins = yAxis->GetNbins();
  util = rsq + 3.*fBinWidth + 2.25*fBinWidth*fBinWidth;
  for (int i = 0; i < numCircXBins; i++)
  {
    xMax = xAxis->GetBinUpEdge(i+1);

    for (int j = 0; j < numCircYBins; j++)
    {
      yMax = yAxis->GetBinUpEdge(j+1);
      if ((xMax-fBinWidth/2.)*(xMax-fBinWidth/2.) 
          + (yMax-fBinWidth/2.)*(yMax-fBinWidth/2.) > util)
        continue;
      for (unsigned int k = 0; k < fPtsPerBin; k++)
      {
        vxC.push_back(xMax - random->Uniform(fBinWidth));
        vyC.push_back(yMax - random->Uniform(fBinWidth));
      }
    }
  }
  const unsigned int numCircPoints = vxC.size();

  xAxis = fCylSide->GetXaxis();
  const int numSideXBins = xAxis->GetNbins();
  yAxis = fCylSide->GetYaxis();
  const int numSideYBins = yAxis->GetNbins();
  for (int i = 0; i < numSideXBins; i++)
  {
    xMax = xAxis->GetBinUpEdge(i+1);

    for (int j = 0; j < numSideYBins; j++)
    {
      yMax = yAxis->GetBinUpEdge(j+1);
      for (unsigned int k = 0; k < fPtsPerBin; k++)
      {
        vsS.push_back(yMax - random->Uniform(fBinWidth));
        vzS.push_back(xMax - random->Uniform(fBinWidth));
      }
    }
  }
  const unsigned int numSidePoints = vzS.size();

  int numCones;
  unsigned int numEvents = static_cast<unsigned int>(fEvents->GetEntries());
  Double_t cone[7], w;
  const Double_t comboOffset = fCylEnd1->GetXaxis()->GetXmax() 
                               + fCylSide->GetXaxis()->GetXmax();
  Double_t cur[3];
  const Double_t numcones_length = Sqrt(length*length + 4.*rsq);
  const unsigned int numToAccept = 
   (fNumToAccept == 0) ? numEvents : fNumToAccept;
  for (unsigned int i = 0, numAccepted = 0; 
       numAccepted < numToAccept && i < numEvents; i++)
  {
    fEvents->GetEntry(i);
    if (fTreeCutForm->EvalInstance() < 0.5) continue;

    numCones = CalcNumCones(numcones_length);
    if (numCones > fMAXCONES) continue;
    numAccepted++;
    if (numAccepted%10 == 0) std::cout << std::setw(5) << numAccepted 
                                       << " events accepted." << std::endl;
    w = fCONENORM/static_cast<Double_t>(numCones);

    for (int j = 0; j < numCones; j++)
    {
      cone[0] = random->Gaus(fComptonInfo->fX1, fComptonInfo->fX1Err);
      cone[1] = random->Gaus(fComptonInfo->fY1, fComptonInfo->fY1Err);
      cone[2] = random->Gaus(fComptonInfo->fZ1, fComptonInfo->fZ1Err);
      cone[3] = random->Gaus(fComptonInfo->fX2, fComptonInfo->fX2Err);
      cone[4] = random->Gaus(fComptonInfo->fY2, fComptonInfo->fY2Err);
      cone[5] = random->Gaus(fComptonInfo->fZ2, fComptonInfo->fZ2Err);
      cone[6] = random->Gaus(fComptonInfo->fHalfAnglePhi, fComptonInfo->fHalfAnglePhiErr);
      fCompCone.SetCone(cone);

      if (fDoPlusZEndcap)
      {
        for (unsigned int k = 0; k < numCircPoints; k++)
        {
          if (vxC[k]*vxC[k] + vyC[k]*vyC[k] > rsq) continue;

          cur[0] = vxC[k];
          cur[1] = vyC[k];
          cur[2] = lenBy2;
          if (fCompCone.CheckProximity(cur))
          {
            fCylEnd1->Fill(-1.*vxC[k],vyC[k],w);
            fComboCyl->Fill(comboOffset - vxC[k],vyC[k],w);
          }
        }
      }

      if (fDoMinusZEndcap)
      {
        for (unsigned int k = 0; k < numCircPoints; k++)
        {
          cur[0] = vxC[k];
          cur[1] = vyC[k];
          cur[2] = minusLenBy2;
          if (fCompCone.CheckProximity(cur))
          {
            fCylEnd2->Fill(vxC[k],vyC[k],w);
            fComboCyl->Fill(vxC[k] - comboOffset,vyC[k],w);
          }
        }
      }

      if (fDoLateralArea)
      {
        for (unsigned int k = 0; k < numSidePoints; k++)
        {
          if (vsS[k] > twoPi*radius) continue;

          cur[0] = -1.*radius*Cos(vsS[k]/radius + pi);
          cur[1] = -1.*radius*Sin(vsS[k]/radius + pi);
          cur[2] = vzS[k];
          if (fCompCone.CheckProximity(cur))
          {
            fCylSide->Fill(vzS[k],vsS[k],w);
            fComboCyl->Fill(vzS[k],vsS[k],w);
          }
        }
      }
    }
  }

  fCylEnd1->Scale(1./static_cast<Double_t>(fPtsPerBin));
  fCylEnd2->Scale(1./static_cast<Double_t>(fPtsPerBin));
  fCylSide->Scale(1./static_cast<Double_t>(fPtsPerBin));
  fComboCyl->Scale(1./static_cast<Double_t>(fPtsPerBin));

  delete &vxC;
  delete &vyC;
  delete &vsS;
  delete &vzS;
  delete random;
  return 0;
}

int EXOComptonImager::DoCathode(Double_t radius)
{
  if (fEvents == 0) return 1;

  if (fCathode) delete fCathode;

  Double_t util;

  int nBinsX=0, nBinsY=0;
  Double_t xMax=0., yMax=0.;
  while (xMax < radius) {xMax+=fBinWidth; nBinsX+=2;}
  yMax = xMax; nBinsY = nBinsX;
  const int nBinsCirc = nBinsY*nBinsX;
  fCathode = new TH2D("Cathode","Cathode",
                      nBinsX,-1.*xMax,xMax,
                      nBinsY,-1.*yMax,yMax);
  fCathode->SetDirectory(0);

  TRandom3 * random = new TRandom3();

  std::vector<Double_t> & vxC = *(new std::vector<Double_t>());
  vxC.reserve(fPtsPerBin*nBinsCirc);
  std::vector<Double_t> & vyC = *(new std::vector<Double_t>());
  vyC.reserve(fPtsPerBin*nBinsCirc);

  const Double_t rsq = radius*radius;
  const Double_t numcones_length = Sqrt(0.5*fTPCLENGTH*fTPCLENGTH + 2.*rsq);

  TAxis * xAxis = fCathode->GetXaxis();
  const int numCircXBins = xAxis->GetNbins();
  TAxis * yAxis = fCathode->GetYaxis();
  const int numCircYBins = yAxis->GetNbins();
  util = rsq + 3.*fBinWidth + 2.25*fBinWidth*fBinWidth;
  for (int i = 0; i < numCircXBins; i++)
  {
    xMax = xAxis->GetBinUpEdge(i+1);

    for (int j = 0; j < numCircYBins; j++)
    {
      yMax = yAxis->GetBinUpEdge(j+1);
      if ((xMax - fBinWidth/2.)*(xMax - fBinWidth/2.) 
          + (yMax - fBinWidth/2.)*(yMax - fBinWidth/2.) > util)
        continue;
      for (unsigned int k = 0; k < fPtsPerBin; k++)
      {
        vxC.push_back(xMax - random->Uniform(fBinWidth));
        vyC.push_back(yMax - random->Uniform(fBinWidth));
      }
    }
  }
  const unsigned int numCircPoints = vxC.size();

  int numCones;
  unsigned int numEvents = static_cast<unsigned int>(fEvents->GetEntries());
  Double_t cone[7], cur[3], w;

  const unsigned int numToAccept = 
   (fNumToAccept == 0) ? numEvents : fNumToAccept;
  for (unsigned int i = 0, numAccepted = 0; 
       numAccepted < numToAccept && i < numEvents; i++)
  {
    fEvents->GetEntry(i);
    if (fTreeCutForm->EvalInstance() < 0.5) continue;

    numCones = CalcNumCones(numcones_length);
    if (numCones > fMAXCONES) continue;
    numAccepted++;
    if (numAccepted%10 == 0) std::cout << std::setw(5) << numAccepted 
                                       << " events accepted." << std::endl;
    w = fCONENORM/static_cast<Double_t>(numCones);

    for (int j = 0; j < numCones; j++)
    {
      cone[0] = random->Gaus(fComptonInfo->fX1, fComptonInfo->fX1Err);
      cone[1] = random->Gaus(fComptonInfo->fY1, fComptonInfo->fY1Err);
      cone[2] = random->Gaus(fComptonInfo->fZ1, fComptonInfo->fZ1Err);
      cone[3] = random->Gaus(fComptonInfo->fX2, fComptonInfo->fX2Err);
      cone[4] = random->Gaus(fComptonInfo->fY2, fComptonInfo->fY2Err);
      cone[5] = random->Gaus(fComptonInfo->fZ2, fComptonInfo->fZ2Err);
      cone[6] = random->Gaus(fComptonInfo->fHalfAnglePhi, fComptonInfo->fHalfAnglePhiErr);
      fCompCone.SetCone(cone);

      for (unsigned int k = 0; k < numCircPoints; k++)
      {
        if (vxC[k]*vxC[k] + vyC[k]*vyC[k] > rsq) continue;

        cur[0] = vxC[k];
        cur[1] = vyC[k];
        cur[2] = 0.;
        if (fCompCone.CheckProximity(cur))
        { fCathode->Fill(vxC[k],vyC[k],w); }
      }
    }
  }

  fCathode->Scale(1./static_cast<Double_t>(fPtsPerBin));

  delete &vxC;
  delete &vyC;
  delete random;
  return 0;
}

int EXOComptonImager::CalcNumCones(Double_t length)
{
  Double_t uu[3] = { fComptonInfo->fX1 - fComptonInfo->fX2,
                     fComptonInfo->fY1 - fComptonInfo->fY2,
                     fComptonInfo->fZ1 - fComptonInfo->fZ2 };
  Double_t duu[3] = {0.0, 0.0, 0.0};
  Double_t ErrPos1[3] = {fComptonInfo->fX1Err, fComptonInfo->fY1Err, fComptonInfo->fZ1Err};
  Double_t ErrPos2[3] = {fComptonInfo->fX2Err, fComptonInfo->fY2Err, fComptonInfo->fZ2Err};

  Double_t util = uu[0]*uu[0] + uu[1]*uu[1] + uu[2]*uu[2];
  util = Sqrt(util);
  for (int i = 0; i < 3; i++) 
  {
    uu[i] /= util;
    for (int j = 0; j < 3; j++)
    {
      duu[i] += pow(ErrPos1[j]*((i==j)*util*util + uu[i]*uu[j]), 2.);
      duu[i] += pow(ErrPos2[j]*((i==j)*util*util + uu[i]*uu[j]), 2.);
    }
    duu[i] = Sqrt(duu[i]);
    duu[i] /= util*util*util;
  }

  Double_t alpha = duu[0]*duu[0] + duu[1]*duu[1] + duu[2]*duu[2];
  if (Abs(alpha) < 4.) alpha = ACos((1. - alpha/2.));
  else return fMAXCONES + 1;

  Double_t beta = Sqrt(ErrPos1[0]*ErrPos1[0] + ErrPos1[1]*ErrPos1[1] + ErrPos1[2]*ErrPos1[2]);
  beta /= length;

  Double_t bgthta = 6.*fComptonInfo->fHalfAnglePhiErr + 6.*alpha + 6.*beta;

  Double_t w = fBinWidth/2.;
  Double_t lttlthta = ASin(w/(2.*length));

  int numCones = static_cast<int>(bgthta/lttlthta) + 2;
  return numCones;
}

void EXOComptonImager::MakeCirclesForEndcaps(Double_t radius)
{
  Int_t numPts = static_cast<Int_t>(twoPi*radius/0.001);
  if (fCirc1) delete fCirc1;
  if (fCirc2) delete fCirc2;
  fCirc1 = new TGraph(numPts);
  fCirc2 = new TGraph(numPts);
  Double_t theta;
  const Double_t offset = fCylEnd1->GetXaxis()->GetXmax() 
                          + fCylSide->GetXaxis()->GetXmax();
  for (Int_t i = 0; i < numPts; i++)
  {
    theta = static_cast<Double_t>(i)/static_cast<Double_t>(numPts);
    theta *= twoPi;
    fCirc1->SetPoint(i, radius*Cos(theta) + offset, radius*Sin(theta));
    fCirc2->SetPoint(i, radius*Cos(theta) - offset, radius*Sin(theta));
  }
  fCirc1->SetLineColor(2);
  fCirc1->SetLineWidth(2);
  fCirc2->SetLineColor(2);
  fCirc2->SetLineWidth(2);
  return;
}

