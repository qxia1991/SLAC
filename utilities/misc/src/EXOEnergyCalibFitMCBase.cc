#include "EXOUtilities/EXOEnergyCalibFitMCBase.hh"

ClassImp(EXOEnergyCalibFitMCBase)

ROOT::Math::IMultiGenFunction* EXOEnergyCalibFitMCBase::fFCN;

EXOEnergyCalibFitMCBase::~EXOEnergyCalibFitMCBase()
{
  if(fFitter) delete fFitter;
  if(fCov) delete fCov;
  if(fChol) delete fChol;
  if(fRandom) delete fRandom;

  if(fDataHisto) delete fDataHisto;
  if(fSmearedMCHisto) delete fSmearedMCHisto;
  if(fScaleHisto) delete fScaleHisto;                 
}

EXOEnergyCalibFitMCBase::EXOEnergyCalibFitMCBase(Double_t *mcPoints, Double_t *mcWeights, Int_t nMC)
{
  Initialize();
  
  for(Int_t i = 0; i < nMC; i++)
  {
    fMCEnergyPoints.push_back(mcPoints[i]);
    fMCEnergyPDF.push_back(mcWeights[i]);
  }
}

void EXOEnergyCalibFitMCBase::Initialize()
{
  fVerboseLevel = 0;;
  fFitType = 0;
  fNpar = 0;

  //fMinuit = NULL;
  fFitter = NULL;
  fCov = NULL;
  fChol = NULL;
  fRandom = new TRandom3();


  fMCEnergyPoints.clear();
  fMCEnergyPDF.clear();
  
  fDataHisto = NULL;
  fSmearedMCHisto = NULL;
  fScaleHisto = NULL;

  fFitFunctions.clear();
  
  return;
}

void EXOEnergyCalibFitMCBase::Fcn(int &, double *, double &f, double *x, int /* iflag */)
{
  f = fFCN->operator()(x);
}

void EXOEnergyCalibFitMCBase::SetVerboseLevel(Int_t level)
{
  fVerboseLevel = level;
}
/*
const TMinuit* EXOEnergyCalibFitMCBase::GetMinuit() const
{
  return fMinuit;
}
*/
const TFitter* EXOEnergyCalibFitMCBase::GetFitter() const
{
  return fFitter;
}

const TMinuit* EXOEnergyCalibFitMCBase::GetMinuit() const
{
  return fFitter->GetMinuit();
}

const TH1* EXOEnergyCalibFitMCBase::GetHisto(const TString type) const
{
  if(type == "data")
    return fDataHisto;
  if(type == "smeared" || type == "MC")
    return fSmearedMCHisto;
  if(type == "scale" || type == "normalization")
    return fScaleHisto;

  return NULL;
}

void EXOEnergyCalibFitMCBase::BinMCEnergy(Int_t gran) // granularity = 1/gran keV
{
  
  Double_t maxEnergy = *std::max_element(fMCEnergyPoints.begin(),fMCEnergyPoints.end());
  Int_t intMaxEnergy = static_cast<int> (ceil(maxEnergy));
  
  TH1D tempEnergy("tempEnergy","",intMaxEnergy*gran,0,intMaxEnergy);
  for(size_t i = 0; i < fMCEnergyPoints.size(); i++)
    tempEnergy.Fill(fMCEnergyPoints.at(i),fMCEnergyPDF.at(i));
  
  fMCEnergyPoints.clear();
  fMCEnergyPDF.clear();

  for(int b = 1; b < tempEnergy.GetNbinsX()+1; b++)
  {
    float content = tempEnergy.GetBinContent(b);
    if(content > 0)
    {
      fMCEnergyPoints.push_back(tempEnergy.GetBinCenter(b));
      fMCEnergyPDF.push_back(content);
    }
  }

  return;
}

Int_t EXOEnergyCalibFitMCBase::GetNumberOfFreeParameters()
{
  if(fFitFunctions.empty())
    return 0;
  
  Int_t totFreePars = 0;
  for(std::vector<TF1*>::iterator function = fFitFunctions.begin(); function != fFitFunctions.end(); function++)
    totFreePars += (*function)->GetNumberFreeParameters();
  
  return totFreePars;
}

void EXOEnergyCalibFitMCBase::DefineInitialParameters()
{
  Int_t fitP = 0;
  for(std::vector<TF1*>::iterator function = fFitFunctions.begin(); function != fFitFunctions.end(); function++)
  {
    for(Int_t p = 0; p < (*function)->GetNpar(); p++)
    {
      Double_t pMin(0), pMax(0);
      (*function)->GetParLimits(p,pMin,pMax);
      if((pMin != 0 || pMax != 0) && (pMin == pMax)) // pMin == pMax -> fixed parameter, unless if they are both zero's
        continue;
      //fMinuit->DefineParameter(fitP++,Form("%s_p%i",(*function)->GetName(),p),(*function)->GetParameter(p),(*function)->GetParError(p),pMin,pMax);
      fFitter->SetParameter(fitP++,Form("%s_p%i",(*function)->GetName(),p),(*function)->GetParameter(p),(*function)->GetParError(p),pMin,pMax);
    }
  }

  return;
}

void EXOEnergyCalibFitMCBase::SetFitFunctionParameters(const double* x)
{
  if(!x)
    return;
  
  Int_t fitP = 0;
  for(std::vector<TF1*>::iterator function = fFitFunctions.begin(); function != fFitFunctions.end(); function++)
  {
    for(Int_t p = 0; p < (*function)->GetNpar(); p++)
    {
      Double_t pMin(0), pMax(0);
      (*function)->GetParLimits(p,pMin,pMax);
      if((pMin != 0 || pMax != 0) && (pMin == pMax)) // pMin == pMax -> fixed parameter, unless if they are both zero's
        continue;
      (*function)->SetParameter(p,x[fitP++]);
    }
  }

  return;
}

std::vector<Double_t> EXOEnergyCalibFitMCBase::GetFittedParameters()
{
  std::vector<Double_t> x(fNpar);
  
  if(!fFitter)
    return x;

  for(Int_t fitP = 0; fitP < fNpar; fitP++)
    x[fitP] = fFitter->GetParameter(fitP);

  return x;  
}


void EXOEnergyCalibFitMCBase::SaveHistosIn(const char* name, const char* option)
{
  // SetFitFunctionParameters(&GetFittedParameters()[0]);
  // Call derived FitFunction....
  
  TFile outRootFile(name,option);
  outRootFile.cd();

  //fFitter->Write();
  fFitter->GetMinuit()->Write();
  
  fDataHisto->Write();
  fScaleHisto->Write();

  if(fSmearedMCHisto->GetDimension() == 1)
  {
    for(int b = 1; b <= fSmearedMCHisto->GetNbinsX(); b++)
    {
      float expected = fSmearedMCHisto->GetBinContent(b);
      float scale = fScaleHisto->GetBinContent(b);
      fSmearedMCHisto->SetBinContent(b,scale*expected);
    }
    fSmearedMCHisto->SetEntries(fSmearedMCHisto->GetEntries());
    fSmearedMCHisto->Write();
  }
  else if(fSmearedMCHisto->GetDimension() == 2)
  {
    for(int xb = 1; xb <= fSmearedMCHisto->GetNbinsX(); xb++)
    {
      for(int yb = 1; yb <= fSmearedMCHisto->GetNbinsY(); yb++)
      {
        float expected = fSmearedMCHisto->GetBinContent(xb,yb);
        float scale = fScaleHisto->GetBinContent(xb,yb);
        fSmearedMCHisto->SetBinContent(xb,yb,scale*expected);
      }
    }
    fSmearedMCHisto->SetEntries(fSmearedMCHisto->GetEntries());
    fSmearedMCHisto->Write();

    TH2F *smearedMCHisto2D = dynamic_cast<TH2F*>(fSmearedMCHisto);
    TH2F *dataHisto2D = dynamic_cast<TH2F*>(fDataHisto);

    if(smearedMCHisto2D)
    {
      smearedMCHisto2D->ProjectionX(Form("%s_ioniz",smearedMCHisto2D->GetName()),1,smearedMCHisto2D->GetNbinsX())->Write();
      smearedMCHisto2D->ProjectionY(Form("%s_scint",smearedMCHisto2D->GetName()),1,smearedMCHisto2D->GetNbinsY())->Write();
    }
    if(dataHisto2D)
    {
      dataHisto2D->ProjectionX(Form("%s_ioniz",dataHisto2D->GetName()),1,dataHisto2D->GetNbinsX())->Write();
      dataHisto2D->ProjectionY(Form("%s_scint",dataHisto2D->GetName()),1,dataHisto2D->GetNbinsY())->Write();
    }
  }
  else
  {
    fSmearedMCHisto->Write();
  }
  outRootFile.Close();

  return;
}

std::vector<Double_t> EXOEnergyCalibFitMCBase::DrawGausFitPars()
{
  std::vector<Double_t> xRandom(fNpar);

  if(!fChol)
  {
    std::cout << "Cannot perform draws because Cholesky decomposition is " << fChol << std::endl;
    return xRandom;
  }
  
  
  std::vector<double> gaus(fNpar), sigma(fNpar,0.);
  for(Int_t i = 0; i < fNpar; i++)
    gaus[i] = fRandom->Gaus();
  for(Int_t i = 0; i < fNpar; i++)
    for(Int_t j = 0; j < fNpar; j++)
      sigma[i] += (*fChol)(i,j)*gaus.at(j);
  for(Int_t i = 0; i < fNpar; i++)
    xRandom[i] = fFitter->GetParameter(i) + sigma.at(i);
  
  return xRandom;
}
