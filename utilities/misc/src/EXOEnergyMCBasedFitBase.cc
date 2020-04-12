#include "EXOUtilities/EXOEnergyMCBasedFitBase.hh"

ClassImp(EXOEnergyMCBasedFitBase)

ROOT::Math::IMultiGenFunction* EXOEnergyMCBasedFitBase::fFCN;

EXOEnergyMCBasedFitBase::~EXOEnergyMCBasedFitBase()
{
  if(fFitter) delete fFitter;
  if(fCov) delete fCov;
  if(fChol) delete fChol;
  if(fRandom) delete fRandom;
  if(fFitFunction) delete fFitFunction;

  
  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    MCEntry &mc = mcEntry->second;
    if(mc.MCSmearedHisto) delete mc.MCSmearedHisto;
  }
  for(std::map<TString, DataEntry>::iterator dataEntry = fDataList.begin(); dataEntry != fDataList.end(); dataEntry++)
  {
    DataEntry &data = dataEntry->second;
    if(data.DataHisto) delete data.DataHisto;
    if(data.ScaleHisto) delete data.ScaleHisto;
    if(data.MCSmearedHisto) delete data.MCSmearedHisto;
  }
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);
    if(histo.DataHisto) delete histo.DataHisto;
    if(histo.MCSmearedHisto) delete histo.MCSmearedHisto;
  }
  
}

EXOEnergyMCBasedFitBase::EXOEnergyMCBasedFitBase()
{
  Initialize();
}

void EXOEnergyMCBasedFitBase::Initialize()
{
  fVerboseLevel = 0;;

  fDimension = 0;
  fAllFitCalculations = true;
  fFitType = 0;
  fNpar = 0;

  fFitter = NULL;
  fCov = NULL;
  fChol = NULL;
  fRandom = new TRandom3();

  fMCList.clear();
  fDataList.clear();

  fFitFunction = 0;
  fFitFunctions.clear();

  TH1::SetDefaultSumw2();
  
  return;
}

void EXOEnergyMCBasedFitBase::Fcn(int &, double *, double &f, double *x, int /* iflag */)
{
  f = fFCN->operator()(x);
}

void EXOEnergyMCBasedFitBase::SetVerboseLevel(Int_t level)
{
  fVerboseLevel = level;
}

bool EXOEnergyMCBasedFitBase::ShouldSetTriggerEffFunction()
{
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);
    if(histo.UsePrescaleTriggerEff)
      return true;
  }
  return false;
}

bool EXOEnergyMCBasedFitBase::AddMC(const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPoints, Double_t *mcWeights, Int_t nMC)
{
  if(fVerboseLevel > 0)
    std::cout << "Adding MC to MCList: " << mcId << std::endl;
  
  if(fMCList.count(mcId))
  {
    std::cerr << "MC id already exists, given MC not added to the MC list!\n";
    return false;
  }
  
  MCEntry newMCEntry;
  newMCEntry.InitializeMembers(sourceAtomicNumber,sourceMassNumber,sourceX,sourceY,sourceZ);
  newMCEntry.SetEnergy(mcPoints,mcWeights,nMC);
  
  fMCList[mcId] = newMCEntry;
  
  return true;
}

const TFitter* EXOEnergyMCBasedFitBase::GetFitter() const
{
  if(fFitter)
    return fFitter;
  return 0;
}

const TMinuit* EXOEnergyMCBasedFitBase::GetMinuit() const
{
  if(fFitter)
    return fFitter->GetMinuit();
  return 0;
}

const TH1* EXOEnergyMCBasedFitBase::GetHisto(const TString list, const TString idName, const TString type) const
{
  if(list == "MC")
  {
    if(fMCList.count(idName))
    {
      return fMCList.at(idName).MCSmearedHisto;
    }
  }
  
  else if(list == "data")
  {
    if(fDataList.count(idName))
    {
      if(type == "data")
        return fDataList.at(idName).DataHisto;
      else if(type == "smeared" || type == "MC")
        return fDataList.at(idName).MCSmearedHisto;
      else if(type == "scale" || type == "normalization")
        return fDataList.at(idName).ScaleHisto;
      else
        return NULL;
    }
  }

  else if(list == "fit" || list == "histo")
  {
    for(std::vector<HistoEntry>::const_iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
    {
      const HistoEntry &histo = *histoEntry;

      TString histoName = histo.DataHisto->GetName();
      if(!histoName.BeginsWith(idName))
        continue;

      if(type == "data")
        return histo.DataHisto;
      else if(type == "smeared" || type == "MC")
        return histo.MCSmearedHisto;
    }
  }
  
  return NULL;
}

void EXOEnergyMCBasedFitBase::BinMCEnergy(Int_t gran, std::string mcId, Double_t cutoff) // granularity = 1/gran keV
{
  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    if(mcId != "all" && mcId != mcEntry->first)
      continue;
    
    MCEntry &mc = mcEntry->second;

    if(mc.Is2D)
    {
      if(mc.MCEnergyPoints2D.empty())
        continue;

      std::cout << "Length before binning: " << mc.MCEnergyPoints2D.size() << std::endl;
      
      std::vector<Double_t> eneX;
      std::vector<Double_t> eneY;
      for(std::vector<std::pair<Double_t, Double_t> >::iterator energy = mc.MCEnergyPoints2D.begin(); energy != mc.MCEnergyPoints2D.end();  energy++)
      {
        eneX.push_back(energy->first);
        eneY.push_back(energy->second);
      }

      Double_t maxEnergyX = *std::max_element(eneX.begin(),eneX.end());
      Int_t intMaxEnergyX = static_cast<int> (ceil(maxEnergyX));
      //intMaxEnergyX = (intMaxEnergyX % 2 == 0) ? intMaxEnergyX: intMaxEnergyX+1;
      Double_t maxEnergyY = *std::max_element(eneY.begin(),eneY.end());
      Int_t intMaxEnergyY = static_cast<int> (ceil(maxEnergyY));
      //intMaxEnergyY = (intMaxEnergyY % 2 == 0) ? intMaxEnergyY: intMaxEnergyY+1;

      TH2D tempEnergy2D("tempEnergy2D","",intMaxEnergyX*gran,0,intMaxEnergyX,intMaxEnergyY*gran,0,intMaxEnergyY);
      for(size_t i = 0; i < mc.MCEnergyPoints2D.size(); i++)
        tempEnergy2D.Fill(eneX.at(i),eneY.at(i),mc.MCEnergyPDF.at(i));

      mc.MCEnergyPoints2D.clear();
      mc.MCEnergyPDF.clear();
      
      for(int xb = 1; xb < tempEnergy2D.GetXaxis()->GetNbins()+1; xb++)
      {
        for(int yb = 1; yb < tempEnergy2D.GetYaxis()->GetNbins()+1; yb++)
        {
          float content = tempEnergy2D.GetBinContent(xb,yb);
          if(content > cutoff)
          {
            mc.MCEnergyPoints2D.push_back(std::make_pair(tempEnergy2D.GetXaxis()->GetBinCenter(xb),tempEnergy2D.GetYaxis()->GetBinCenter(yb)));
            mc.MCEnergyPDF.push_back(content);
          }
        }
      }
      std::cout << "Length after binning: " << mc.MCEnergyPoints2D.size() << std::endl;
      //TFile ftemp("bin.root","RECREATE");
      //tempEnergy2D.Write();
      //ftemp.Close();
    }
  
    else
    {
      if(mc.MCEnergyPoints.empty())
        continue;
      
      Double_t maxEnergy = *std::max_element(mc.MCEnergyPoints.begin(),mc.MCEnergyPoints.end());
      Int_t intMaxEnergy = static_cast<int> (ceil(maxEnergy));
  
      TH1D tempEnergy("tempEnergy","",intMaxEnergy*gran,0,intMaxEnergy);
      for(size_t i = 0; i < mc.MCEnergyPoints.size(); i++)
        tempEnergy.Fill(mc.MCEnergyPoints.at(i),mc.MCEnergyPDF.at(i));
      
      mc.MCEnergyPoints.clear();
      mc.MCEnergyPDF.clear();

      for(int b = 1; b < tempEnergy.GetNbinsX()+1; b++)
      {
        float content = tempEnergy.GetBinContent(b);
        if(content > 0)
        {
          mc.MCEnergyPoints.push_back(tempEnergy.GetBinCenter(b));
          mc.MCEnergyPDF.push_back(content);
        }
      }
    }
    
  }  
  return;
}

Int_t EXOEnergyMCBasedFitBase::GetNumberOfFreeParameters()
{
  if(fFitFunctions.empty())
    return 0;
  
  Int_t totFreePars = 0;
  for(std::vector<TF1*>::iterator function = fFitFunctions.begin(); function != fFitFunctions.end(); function++)
    totFreePars += (*function)->GetNumberFreeParameters();
  
  return totFreePars;
}

void EXOEnergyMCBasedFitBase::DefineInitialParameters()
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

bool EXOEnergyMCBasedFitBase::SetFitFunctionParameters(const double* x)
{
  if(!x)
    return true;
  
  Int_t fitP = 0;
  for(std::vector<TF1*>::iterator function = fFitFunctions.begin(); function != fFitFunctions.end(); function++)
  {
    for(Int_t p = 0; p < (*function)->GetNpar(); p++)
    {
      Double_t pMin(0), pMax(0);
      (*function)->GetParLimits(p,pMin,pMax);
      if((pMin != 0 || pMax != 0) && (pMin == pMax)) // pMin == pMax -> fixed parameter, unless if they are both zero's
        continue;
      if(std::isfinite(x[fitP]))
        (*function)->SetParameter(p,x[fitP++]);
      else
        return false;
    }
  }

  return true;
}

std::vector<Double_t> EXOEnergyMCBasedFitBase::GetFittedParameters()
{
  std::vector<Double_t> x(fNpar);
  
  if(!fFitter)
    return x;

  for(Int_t fitP = 0; fitP < fNpar; fitP++)
    x[fitP] = fFitter->GetParameter(fitP);

  return x;  
}

double EXOEnergyMCBasedFitBase::FitFunction(const double* x)
{
  return 0.;
}

double EXOEnergyMCBasedFitBase::GetErrorDef(double delta)
{
  double errorDef = 0.;
  double weightNorm = 0.;
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);
    if(histo.Weight != 0 && histo.NumberOfEvents > 0)
    {
      weightNorm += histo.NumberOfEvents / histo.Weight;
      errorDef += delta * histo.Weight / histo.NumberOfEvents;
    }
  }
  errorDef *= weightNorm;
  return errorDef;
}

bool EXOEnergyMCBasedFitBase::BuildFitter()
{
  return false;
}

bool EXOEnergyMCBasedFitBase::ApplyFittedParameters()
{
  if(fVerboseLevel > 0)
    std::cout << "Applying fitted parameters to fill the histograms...\n";
  
  if(!fFitter)
    return false;
  
  std::vector<double> x = GetFittedParameters();
  if(!(&x[0]))
    return false;

  FitFunction(&x[0]);

  return true;
}

void EXOEnergyMCBasedFitBase::ApplyCalibration(std::vector<Double_t>& energies, TF1* calibFunction)
{
  for(size_t i = 0; i < energies.size(); i++)
    energies[i] = calibFunction->Eval(energies[i]);
  return;
}


void EXOEnergyMCBasedFitBase::SaveHistosIn(const char* name, const char* option, int rebinX, int rebinY)
{
  TString pdfFileName = name;
  pdfFileName = pdfFileName.ReplaceAll(".root",".pdf");
  TCanvas canvasAgree;
    
  TFile outRootFile(name,option);
  outRootFile.cd();

  if(fFitter)
    fFitter->GetMinuit()->Write();

  if(!fFitFunctions.empty())
    for(std::vector<TF1*>::iterator fitFunction = fFitFunctions.begin(); fitFunction != fFitFunctions.end(); fitFunction++)
      if((*fitFunction))
        (*fitFunction)->Write();

  if(fVerboseLevel > 0)
    std::cout << "Writing MC histograms...\n";
  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    MCEntry &mc = mcEntry->second;
    if(mc.MCSmearedHisto)
    {
      if(fDimension == 1)
      {
        TH1D* smearedMCHisto1D = dynamic_cast<TH1D*>(mc.MCSmearedHisto);
        if(smearedMCHisto1D)
        {
          outRootFile.cd();
          smearedMCHisto1D->Rebin(rebinX,Form("%s_%s","MCEntry",smearedMCHisto1D->GetName()))->Write();
        }
      }
      else if(fDimension == 2)
      {
        TH2F* smearedMCHisto2D = dynamic_cast<TH2F*>(mc.MCSmearedHisto);
        if(smearedMCHisto2D)
        {
          outRootFile.cd();
          smearedMCHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","MCEntry",smearedMCHisto2D->GetName()))->Write();
        }
      }
      else
      {
        mc.MCSmearedHisto->Write(Form("%s_%s","MCEntry",mc.MCSmearedHisto->GetName()));
      }
    }
  }

  if(fVerboseLevel > 0)
    std::cout << "Writing data histograms...\n";

  TString dataPdfFileName = pdfFileName;
  dataPdfFileName = dataPdfFileName.ReplaceAll(".pdf","_data.pdf");
  canvasAgree.Print(Form("%s[",dataPdfFileName.Data()));
  for(std::map<TString, DataEntry>::iterator dataEntry = fDataList.begin(); dataEntry != fDataList.end(); dataEntry++)
  {
    DataEntry &data = dataEntry->second;

    outRootFile.cd();

    if(fDimension == 1)
    {
      TH1D* dataHisto1D = dynamic_cast<TH1D*>(data.DataHisto);
      TH1D* scaleHisto1D = dynamic_cast<TH1D*>(data.ScaleHisto);
      TH1D* smearedMCHisto1D = dynamic_cast<TH1D*>(data.MCSmearedHisto);

      if(dataHisto1D && scaleHisto1D && smearedMCHisto1D)
      {
        dataHisto1D = dynamic_cast<TH1D*>(dataHisto1D->Rebin(rebinX,Form("%s_%s","DataEntry",dataHisto1D->GetName())));
        scaleHisto1D = dynamic_cast<TH1D*>(scaleHisto1D->Rebin(rebinX,Form("%s_%s","DataEntry",scaleHisto1D->GetName())));
        smearedMCHisto1D = dynamic_cast<TH1D*>(smearedMCHisto1D->Rebin(rebinX,Form("%s_%s","DataEntry",smearedMCHisto1D->GetName())));

        outRootFile.cd();
        dataHisto1D->Write();
        scaleHisto1D->Write();
        smearedMCHisto1D->Write();

        double chi2ndf = dataHisto1D->Chi2Test(smearedMCHisto1D,"UW CHI2/NDF");
        
        canvasAgree.Clear();
        canvasAgree.Divide(2,1);
        
        canvasAgree.cd(1);
        //dataHisto1D->SetTitle(Form("%s #Chi^{2}/NDF = %.2f",dataHisto1D->GetName(),chi2ndf));
	dataHisto1D->SetTitle(Form("%s #Chi^{2}/NDF = %.2f; Ionization Energy (keV); Counts",dataHisto1D->GetName(),chi2ndf));
        dataHisto1D->SetLineColor(1);
        dataHisto1D->Draw();
        smearedMCHisto1D->SetLineColor(4);
        smearedMCHisto1D->Draw("hist sames c");

        TH1D residual(*dataHisto1D);
        residual.Add(dataHisto1D,smearedMCHisto1D,1,-1);
        residual.Divide(dataHisto1D);
        //residual.SetTitle(dataHisto1D->GetName());
	residual.SetTitle(Form("Residual for %s; Ionization Energy (keV)",dataHisto1D->GetName()));

        canvasAgree.cd(2);
        residual.GetYaxis()->SetRangeUser(-1.,1.);
        residual.Draw();

        canvasAgree.Print(Form("%s",dataPdfFileName.Data()));
        
      }
    }
    else if(fDimension == 2)
    {
      TH2F* dataHisto2D = dynamic_cast<TH2F*>(data.DataHisto);
      TH2F* scaleHisto2D = dynamic_cast<TH2F*>(data.ScaleHisto);
      TH2F* smearedMCHisto2D = dynamic_cast<TH2F*>(data.MCSmearedHisto);

      if(dataHisto2D && scaleHisto2D && smearedMCHisto2D)
      {
        TH2F* dataHisto2Dtemp = dynamic_cast<TH2F*>(dataHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","DataEntry",dataHisto2D->GetName())));
        TH2F* scaleHisto2Dtemp = dynamic_cast<TH2F*>(scaleHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","DataEntry",scaleHisto2D->GetName())));
        TH2F* smearedMCHisto2Dtemp = dynamic_cast<TH2F*>(smearedMCHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","DataEntry",smearedMCHisto2D->GetName())));

        outRootFile.cd();
        dataHisto2Dtemp->Write();
        scaleHisto2Dtemp->Write();
        smearedMCHisto2Dtemp->Write();
        
        outRootFile.cd();

        dataHisto2Dtemp->ProjectionX(Form("%s_projx",dataHisto2Dtemp->GetName()),1,dataHisto2Dtemp->GetNbinsX())->Write();
        dataHisto2Dtemp->ProjectionY(Form("%s_projy",dataHisto2Dtemp->GetName()),1,dataHisto2Dtemp->GetNbinsY())->Write();

        scaleHisto2Dtemp->ProjectionX(Form("%s_projx",scaleHisto2Dtemp->GetName()),1,scaleHisto2Dtemp->GetNbinsX())->Write();
        scaleHisto2Dtemp->ProjectionY(Form("%s_projy",scaleHisto2Dtemp->GetName()),1,scaleHisto2Dtemp->GetNbinsY())->Write();
        
        smearedMCHisto2Dtemp->ProjectionX(Form("%s_projx",smearedMCHisto2Dtemp->GetName()),1,smearedMCHisto2Dtemp->GetNbinsX())->Write();
        smearedMCHisto2Dtemp->ProjectionY(Form("%s_projy",smearedMCHisto2Dtemp->GetName()),1,smearedMCHisto2Dtemp->GetNbinsY())->Write();
      }
    }
    else
    {
      data.DataHisto->Write(Form("%s_%s","DataEntry",data.DataHisto->GetName()));
      data.ScaleHisto->Write(Form("%s_%s","DataEntry",data.ScaleHisto->GetName()));
      data.MCSmearedHisto->Write(Form("%s_%s","DataEntry",data.MCSmearedHisto->GetName()));
    }
  }
  canvasAgree.Print(Form("%s]",dataPdfFileName.Data()));
  
  if(fVerboseLevel > 0)
    std::cout << "Writing fit histograms...\n";
  
  TString histoPdfFileName = pdfFileName;
  histoPdfFileName = histoPdfFileName.ReplaceAll(".pdf","_histo.pdf");

  if(fDimension == 1)
    canvasAgree.Print(Form("%s[",histoPdfFileName.Data()));
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);

    if(fDimension == 1)
    {
      TH1D* dataHisto1D = dynamic_cast<TH1D*>(histo.DataHisto);
      TH1D* smearedMCHisto1D = dynamic_cast<TH1D*>(histo.MCSmearedHisto);

      if(dataHisto1D && smearedMCHisto1D)
      {        
        dataHisto1D = dynamic_cast<TH1D*>(dataHisto1D->Rebin(rebinX,Form("%s_%s","FitHistoEntry",dataHisto1D->GetName())));
        smearedMCHisto1D = dynamic_cast<TH1D*>(smearedMCHisto1D->Rebin(rebinX,Form("%s_%s","FitHistoEntry",smearedMCHisto1D->GetName())));

        outRootFile.cd();
        dataHisto1D->Write();
        smearedMCHisto1D->Write();

        double chi2ndf = dataHisto1D->Chi2Test(smearedMCHisto1D,"UW CHI2/NDF");

        canvasAgree.Clear();
        canvasAgree.Divide(2,1);
        
        canvasAgree.cd(1);
        dataHisto1D->SetTitle(Form("%s #Chi^{2}/NDF = %.2f; Ionization Energy (keV); Counts",dataHisto1D->GetName(),chi2ndf));
	//dataHisto1D->SetTitle(Form("%s #Chi^{2}/NDF = %.2f",dataHisto1D->GetName(),chi2ndf));
        dataHisto1D->SetLineColor(1);
        dataHisto1D->Draw();
        smearedMCHisto1D->SetLineColor(4);
        smearedMCHisto1D->Draw("hist sames c");

        TH1D residual(*dataHisto1D);
        residual.Add(dataHisto1D,smearedMCHisto1D,1,-1);
        residual.Divide(dataHisto1D);
        residual.SetTitle(Form("Residual for %s; Ionization Energy (keV)",dataHisto1D->GetName()));

        canvasAgree.cd(2);
        residual.GetYaxis()->SetRangeUser(-1.,1.);
        residual.Draw();

        canvasAgree.Print(Form("%s",histoPdfFileName.Data()));        
      }
    }
    else if(fDimension == 2)
    {
      TH2F* dataHisto2D = dynamic_cast<TH2F*>(histo.DataHisto);
      TH2F* smearedMCHisto2D = dynamic_cast<TH2F*>(histo.MCSmearedHisto);

      if(dataHisto2D && smearedMCHisto2D)
      {
        outRootFile.cd();
        
        TH2F* dataHisto2Dtemp = dynamic_cast<TH2F*>(dataHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","FitHistoEntry",dataHisto2D->GetName())));
        TH2F* smearedMCHisto2Dtemp = dynamic_cast<TH2F*>(smearedMCHisto2D->Rebin2D(rebinX,rebinY,Form("%s_%s","FitHistoEntry",smearedMCHisto2D->GetName())));

        outRootFile.cd();
        dataHisto2Dtemp->Write();
        smearedMCHisto2Dtemp->Write();
        
        outRootFile.cd();

        dataHisto2Dtemp->ProjectionX(Form("%s_projx",dataHisto2Dtemp->GetName()),1,dataHisto2Dtemp->GetNbinsX())->Write();
        dataHisto2Dtemp->ProjectionY(Form("%s_projy",dataHisto2Dtemp->GetName()),1,dataHisto2Dtemp->GetNbinsY())->Write();
        
        smearedMCHisto2Dtemp->ProjectionX(Form("%s_projx",smearedMCHisto2Dtemp->GetName()),1,smearedMCHisto2Dtemp->GetNbinsX())->Write();
        smearedMCHisto2Dtemp->ProjectionY(Form("%s_projy",smearedMCHisto2Dtemp->GetName()),1,smearedMCHisto2Dtemp->GetNbinsY())->Write();
      }
    }
    else
    {
      histo.DataHisto->Write(Form("%s_%s","FitHistoEntry",histo.DataHisto->GetName()));
      histo.MCSmearedHisto->Write(Form("%s_%s","FitHistoEntry",histo.MCSmearedHisto->GetName()));
    }
  }
  if(fDimension == 1)
    canvasAgree.Print(Form("%s]",histoPdfFileName.Data()));
  
  outRootFile.Close();


  return;
}

std::vector<Double_t> EXOEnergyMCBasedFitBase::DrawGausFitPars()
{
  std::vector<Double_t> xRandom(fNpar);

  if(!fChol)
  {
    std::cerr << "Cannot perform draws because Cholesky decomposition is " << fChol << std::endl;
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
