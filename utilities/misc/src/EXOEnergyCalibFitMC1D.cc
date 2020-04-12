#include "EXOUtilities/EXOEnergyCalibFitMC1D.hh"

ClassImp(EXOEnergyCalibFitMC1D)

EXOEnergyCalibFitMC1D::~EXOEnergyCalibFitMC1D()
{
  if(fFitResolFunction) delete fFitResolFunction;
  if(fFitCalibFunction) delete fFitCalibFunction;
}

EXOEnergyCalibFitMC1D::EXOEnergyCalibFitMC1D(Double_t *mcPoints, Double_t *mcWeights, Int_t nMC, Double_t *dataPoints, Int_t nData):EXOEnergyCalibFitMCBase(mcPoints,mcWeights,nMC)
{
  fFitResolFunction = NULL;
  fFitCalibFunction = NULL;

  fDataHisto1D = NULL;
  fSmearedMCHisto1D = NULL;
  fScaleHisto1D = NULL;
  
  for(Int_t i = 0; i < nData; i++)
    fDataEnergyPoints.push_back(dataPoints[i]);
  //fDataEnergyPoints.push_back(0.95*dataPoints[i]+100);

}

void EXOEnergyCalibFitMC1D::FillSmearedMCHistogram(const double* x, TH1D& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf)
{
  if(fVerboseLevel > 1)
    std::cout << "Creating 1D smearing...\n";

  SetFitFunctionParameters(x);

  int nBins = histo.GetNbinsX();

  std::vector<double> e_up;
  std::vector<double> weight(nBins, 0.);
  for(int b = 1; b <= nBins; b++){
    e_up.push_back(fFitCalibFunction->Eval(histo.GetBinLowEdge(b) + histo.GetBinWidth(b)));
  }
  double elow = fFitCalibFunction->Eval(histo.GetBinLowEdge(1));

  long int n = fMCEnergyPoints.size();
  for(long int i = 0; i < n; i++){
    double e_mc = energy[i];
    double res = fFitResolFunction->Eval(e_mc);//sqrt(p0*p0*e_mc + p1*p1 + p2*p2*e_mc*e_mc);
    double const_res = 0.7071067811865474 / res;
    double erflow = erf(const_res * (elow - e_mc) );
    for(int b = 0; b < nBins; b++){
      double erfup = erf(const_res * (e_up[b] - e_mc) );
      weight[b] += (erfup - erflow)*pdf[i];
      erflow = erfup;
    }
  }

  for(int b = 0; b < nBins; b++){
    histo.SetBinContent(b+1, weight[b]/2.);
  }
    
  histo.SetEntries(n);

  return;
}

void EXOEnergyCalibFitMC1D::GetTriggerScale(TH1D& hScale, const TH1D& hData, const TH1D& hMC, double eCut, double eResolution, int prescale, bool hasAPDthres)
{
  if(prescale < 1 || !hasAPDthres)
    eCut = 0;

  int minBinMC = 1;//hMC.FindFixBin(2450);// 1;//
  int maxBinMC = hMC.GetNbinsX();//hMC.FindFixBin(2750);// hMC.GetNbinsX();//
  int minBinData = 1;//hData.FindFixBin(2450);// 1;//
  int maxBinData = hData.GetNbinsX();//hData.FindFixBin(2750);// hData.GetNbinsX();//
  
  if(minBinMC < hMC.FindFixBin(eCut) && hMC.FindFixBin(eCut) < maxBinMC){
  
    double totDataLow =  hData.Integral(minBinData,hData.FindFixBin(eCut)-1);
    double totDataHigh = hData.Integral(hData.FindFixBin(eCut),maxBinData);
    double totMCLow = hMC.Integral(minBinMC,hMC.FindFixBin(eCut)-1);
    double totMCHigh = hMC.Integral(hMC.FindFixBin(eCut),maxBinMC);

    double scaleLow = 1.;//totDataLow/totMCLow;//
    double scaleHigh = 1.;//totDataHigh/totMCHigh;//

    if(totMCLow > 0)
      scaleLow = totDataLow/totMCLow;
    if(totMCHigh > 0)
      scaleHigh = totDataHigh/totMCHigh;
    
    double constRes = 0.7071067811865474 / (eResolution*eCut);

    for(int b = 1; b < hScale.GetNbinsX()+1; b++){
      if(scaleLow < scaleHigh)
        hScale.SetBinContent(b,scaleLow + (scaleHigh - scaleLow)*(1. + erf(constRes * (hScale.GetBinCenter(b) - eCut)))/2.);
      else
        hScale.SetBinContent(b,scaleHigh + (scaleLow - scaleHigh)*(1. - erf(constRes * (hScale.GetBinCenter(b) - eCut)))/2.);
    }
    
    return;
  }

  double totData =  hData.Integral(minBinData,maxBinData);
  double totMC = hMC.Integral(minBinMC,maxBinMC);
  
  double scale = 1.;
  if(totMC > 0)
    scale = totData/totMC;

  for(int b = 1; b < hScale.GetNbinsX()+1; b++)
    hScale.SetBinContent(b,scale);
  
  return;
  
}


void EXOEnergyCalibFitMC1D::SetDataHisto(Int_t nBins, Double_t lowEnergy, Double_t upEnergy)
{
  if(fDataHisto)
    delete fDataHisto;  
  fDataHisto = new TH1D("fDataHisto","",nBins,lowEnergy,upEnergy);
  if(fDataHisto1D)
    delete fDataHisto1D;
  fDataHisto1D = dynamic_cast<TH1D*>(fDataHisto);

  if(!fDataHisto1D)
  {
    std::cout << "There was a problem converting TH1 to TH1D. Data histogram has not been properly created.\n";
    return;
  }

  for(size_t i = 0; i < fDataEnergyPoints.size(); i++)
    fDataHisto1D->Fill(fDataEnergyPoints[i]);

  if(fSmearedMCHisto)
    delete fSmearedMCHisto;
  fSmearedMCHisto = new TH1D("fSmearedMCHisto","",nBins,lowEnergy,upEnergy);
  if(fSmearedMCHisto1D)
    delete fSmearedMCHisto1D;
  fSmearedMCHisto1D = dynamic_cast<TH1D*>(fSmearedMCHisto);

  if(fScaleHisto)
    delete fScaleHisto;
  fScaleHisto = new TH1D("fScaleHisto","",nBins,lowEnergy,upEnergy);
  if(fScaleHisto1D)
    delete fScaleHisto1D;
  fScaleHisto1D = dynamic_cast<TH1D*>(fScaleHisto);
  
}

void EXOEnergyCalibFitMC1D::SetFunction(TString type, TF1* func)
{
  TF1 **function;
  if(type == "calib" || type == "calibration")
    function = &fFitCalibFunction;
  else if(type == "resol" || type == "resolution")
    function = &fFitResolFunction;
  else
  {
    std::cout << "Function type not found, exiting EXOEnergyCalibFitMC1D::SetFunction procedure w/o setting function...\n";
    return;
  }

  if(*function)
    delete *function;

  *function = func;

  if(fVerboseLevel > 0)
    (*function)->Print();
  
  return;
}

bool EXOEnergyCalibFitMC1D::ExecuteFit(Int_t fitType)
{
  // check consistency of info used in fit
  if(fMCEnergyPoints.empty() || fMCEnergyPDF.empty())
  {
    std::cout << "MC energy PDF not found, please enter this info before fitting...\n";
    return false;
  }
    
  if(!fDataHisto)
  {
    std::cout << "Histogram of data energy not found, please set this info before fitting...\n";
    return false;
  }

  if(!fFitCalibFunction)
  {
    std::cout << "Calibration function not found, please set this info before fitting...\n";
    return false;
  }

  if(!fFitResolFunction)
  {
    std::cout << "Resolution function not found, please set this info before fitting...\n";
    return false;
  }

  fFitFunctions.clear();
  fFitFunctions.push_back(fFitCalibFunction);
  fFitFunctions.push_back(fFitResolFunction);

  fNpar = GetNumberOfFreeParameters();//fFitResolFunction->GetNpar() + fFitCalibFunction->GetNpar();
    
  // create MINUIT
  if(fVerboseLevel > 0)
    std::cout << "Creating TMinuit object...\n";
  
  ROOT::Math::Functor function(this, &EXOEnergyCalibFitMC1D::FitFunction,fNpar);
  const ROOT::Math::IMultiGenFunction& func = function;
  fFCN = const_cast<ROOT::Math::IMultiGenFunction *>(&func);
  
  double commandList[10];
  int errorFlag;

  /*
  if(fMinuit)
    delete fMinuit;
  
  fMinuit = new TMinuit(fNpar);
  fMinuit->SetFCN(&EXOEnergyCalibFitMCBase::Fcn);
  */

  if(fFitter)
    delete fFitter;

  fFitter = new TFitter(fNpar);
  fFitter->GetMinuit()->SetPrintLevel(fVerboseLevel);
  fFitter->SetFCN(&EXOEnergyCalibFitMCBase::Fcn);
    
  DefineInitialParameters();

  /*
  fMinuit->mnexcm("SHOw FCNvalue",commandList,0,errorFlag);

  commandList[0] = 100000;
  commandList[1] = 0.1;
  fMinuit->mnexcm("MINImize",commandList,2,errorFlag);
  */

  //fFitter->ExecuteCommand("SHOw FCNvalue",commandList,0);

  commandList[0] = 100000;
  commandList[1] = 0.1;
  fFitter->ExecuteCommand("MINImize",commandList,2);
  fFitter->ExecuteCommand("HESSe",commandList,1);

  //SetFitResultToFunctionParameters();
  //Double_t *xFit = GetFittedParameters();
  
  FitFunction(&GetFittedParameters()[0]);
  //delete xFit;

  if(fCov)
    delete fCov;
  if(fChol)
    delete fChol;

  fCov = new TMatrixDSym(fNpar);
  for(Int_t i = 0; i < fNpar; i++)
    for(Int_t j = 0; j < fNpar; j++)
      (*fCov)[i][j] = fFitter->GetCovarianceMatrixElement(i,j);

  //fCov->Print();

  TDecompChol decompChol(*fCov);
  bool okChol = decompChol.Decompose();
  if(fVerboseLevel > 0)
    std::cout << "Cholesky decomposition is " << okChol << std::endl;
  TMatrixD cholT = decompChol.GetU();
  fChol = new TMatrixD(cholT.Transpose(cholT));

  //fChol->Print();
  
  return true;
}

double EXOEnergyCalibFitMC1D::FitFunction(const double* x)
{
  if(fVerboseLevel > 1){
    std::cout << "Using parameters: ";
    for(int p = 0; p < fNpar; p++)
      std::cout << "p" << p << " = " << x[p] << " ";
    std::cout << "\n";
  }

  FillSmearedMCHistogram(x,*fSmearedMCHisto1D,fMCEnergyPoints,fMCEnergyPDF);
  GetTriggerScale(*fScaleHisto1D,*fDataHisto1D,*fSmearedMCHisto1D);

  int NDF = -fNpar;
  double Chi2 = 0.;
  double NLL = 0.;
  
  //double scale = fDataHisto1D->Integral(1,fDataHisto1D->GetNbinsX())/fSmearedMCHisto1D->Integral(1,fSmearedMCHisto1D->GetNbinsX());

  for(int b = 1; b < fDataHisto1D->GetNbinsX()+1; b++)
  {
    double observed = fDataHisto1D->GetBinContent(b);
    double expected = fSmearedMCHisto1D->GetBinContent(b);
    double scale = fScaleHisto1D->GetBinContent(b);

    double center = fDataHisto1D->GetBinCenter(b);

    if(observed > 0)
    {
      double diff = observed - scale*expected;
      double chi2bin = diff*diff / (observed + scale*scale*expected);
      Chi2 += chi2bin;
      NDF++;
    }
 
    double nllBin = scale*expected - observed;
    if(scale*expected > 0)
    {
      if(observed > 0)
        nllBin -= observed*log((scale*expected)*1./observed);
      else
        nllBin -= observed*log((scale*expected));
    }
    NLL += nllBin;
  }

  double FCN = (fFitType == 1) ? NLL : Chi2;
  
  if(fVerboseLevel > 1)
    std::cout << "FCN = " << FCN << " (Chi2/NDF = " << Chi2/NDF << ")" << std::endl;
  
  return FCN;
}

Double_t EXOEnergyCalibFitMC1D::GetPeakPosition(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitCalibFunction->GetX(energy);
}

Double_t EXOEnergyCalibFitMC1D::GetPeakPositionError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
{
  std::vector<double> vDraws;
  for(Int_t i = 0; i < nDraws; i++)
  {
    SetFitFunctionParameters(&DrawGausFitPars()[0]);
    vDraws.push_back(GetPeakPosition(energy,false));
  }
  
  TH1D hDraws("hDraws","",nDraws/10,*std::min_element(vDraws.begin(),vDraws.end()),*std::max_element(vDraws.begin(),vDraws.end()));
  for(Int_t i = 0; i < nDraws; i++)
    hDraws.Fill(vDraws.at(i));

  hDraws.Fit("gaus","LQ0");
  double relError = hDraws.GetFunction("gaus")->GetParameter(2)/hDraws.GetFunction("gaus")->GetParameter(1);
    
  if(saveDraws)
    *saveDraws = hDraws;
  
  SetFitFunctionParameters(&GetFittedParameters()[0]);
  return relError*GetPeakPosition(energy);
}

Double_t EXOEnergyCalibFitMC1D::GetPeakWidth(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitResolFunction->Eval(energy);
}

Double_t EXOEnergyCalibFitMC1D::GetPeakWidthError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
{
  std::vector<double> vDraws;
  for(Int_t i = 0; i < nDraws; i++)
  {
    SetFitFunctionParameters(&DrawGausFitPars()[0]);
    vDraws.push_back(GetPeakWidth(energy,false));
  }
  
  TH1D hDraws("hDraws","",nDraws/10,*std::min_element(vDraws.begin(),vDraws.end()),*std::max_element(vDraws.begin(),vDraws.end()));
  for(Int_t i = 0; i < nDraws; i++)
    hDraws.Fill(vDraws.at(i));

  hDraws.Fit("gaus","LQ0");
  double relError = hDraws.GetFunction("gaus")->GetParameter(2)/hDraws.GetFunction("gaus")->GetParameter(1);
    
  if(saveDraws)
    *saveDraws = hDraws;
  
  return relError*GetPeakWidth(energy,true);
}
