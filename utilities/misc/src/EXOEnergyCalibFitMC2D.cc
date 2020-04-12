#include "EXOUtilities/EXOEnergyCalibFitMC2D.hh"

ClassImp(EXOEnergyCalibFitMC2D)

EXOEnergyCalibFitMC2D::~EXOEnergyCalibFitMC2D()
{
  if(fFitIonizResolFunction) delete fFitIonizResolFunction;
  if(fFitIonizCalibFunction) delete fFitIonizCalibFunction;
  if(fFitScintResolFunction) delete fFitScintResolFunction;
  if(fFitScintCalibFunction) delete fFitScintCalibFunction;
  if(fFitCorrelationFunction) delete fFitCorrelationFunction;
}

EXOEnergyCalibFitMC2D::EXOEnergyCalibFitMC2D(Double_t *mcPoints, Double_t *mcWeights, Int_t nMC, Double_t *ionizDataPoints, Double_t *scintDataPoints, Int_t nData):EXOEnergyCalibFitMCBase(mcPoints,mcWeights,nMC)
{
  fFitIonizResolFunction = NULL;
  fFitIonizCalibFunction = NULL;
  fFitScintResolFunction = NULL;
  fFitScintCalibFunction = NULL;
  fFitCorrelationFunction = NULL;

  fDataHisto2D = NULL;
  fSmearedMCHisto2D = NULL;
  fScaleHisto2D = NULL;
  
  for(Int_t i = 0; i < nData; i++)
  {
    fIonizDataEnergyPoints.push_back(ionizDataPoints[i]);
    fScintDataEnergyPoints.push_back(scintDataPoints[i]);
  //fDataEnergyPoints.push_back(0.95*dataPoints[i]+100);
  }

  fMinFastExp = -25;
  fVecFastExp.resize(-fMinFastExp+1);
  for(Int_t i = 0; i >= fMinFastExp; i--)
    fVecFastExp[-i] = exp(i);
}

float EXOEnergyCalibFitMC2D::FastExp(float x) // returns exponential divided by 0.0001984127f to save 1 multiplication per call
{
  if(x < 0)
  {
    if(x < fMinFastExp)
      return 0.; 
    int xi = (int) (x-0.5f);
    float xf = x - xi;
    return fVecFastExp[-xi]*(5040+xf*(5040+xf*(2520+xf*(840+xf*(210+xf*(42+xf*(7+xf)))))));//return fExp[-xi]*(720+xf*(720+xf*(360+xf*(120+xf*(30+xf*(6+xf))))));//(120+xf*(120+xf*(60+xf*(20+xf*(5+xf)))));//(24+x*(24+x*(12+x*(4+x))));//(120+xf*(120+xf*(60+xf*(20+xf*(5+xf)))));
  }
  
  if(x > -fMinFastExp)
    return 1e+11; 
  int xi = (int) (x+0.5f);
  float xf = x - xi;
  return (5040+xf*(5040+xf*(2520+xf*(840+xf*(210+xf*(42+xf*(7+xf)))))))/fVecFastExp[xi];//return (720+xf*(720+xf*(360+xf*(120+xf*(30+xf*(6+xf))))))/fExp[xi];//(120+xf*(120+xf*(60+xf*(20+xf*(5+xf)))));      
}

float EXOEnergyCalibFitMC2D::GaussTerm2(float xx0, float yy0, float corr)
{
  float mix = xx0 * yy0 * corr;
  xx0 *= xx0;
  yy0 *= yy0;
  return -xx0 -yy0 + mix;
}

void EXOEnergyCalibFitMC2D::FillSmearedMCHistogram(const double* x, TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf)
{
  if(fVerboseLevel > 1)
    std::cout << "Creating 2D smearing...\n";
  
  SetFitFunctionParameters(x);

  TAxis *ccAxis = histo.GetXaxis();
  TAxis *scAxis = histo.GetYaxis();

  // get histo properties
  Int_t ccBins = ccAxis->GetNbins();
  Int_t scBins = scAxis->GetNbins();

  std::vector<float> content(ccBins*scBins, 1.e-12);

  std::vector<Float_t> widthCC;
  std::vector<Float_t> sumCC, subCC, midCC;
  for(int i = 1; i < ccBins+1; i++)
  {
    float mean = (ccAxis->GetBinUpEdge(i) + ccAxis->GetBinLowEdge(i))/2.;
    float width = (ccAxis->GetBinUpEdge(i) - ccAxis->GetBinLowEdge(i))/2.;

    float sum = fFitIonizCalibFunction->Eval(mean + width*0.7745966692414834);
    float sub = fFitIonizCalibFunction->Eval(mean - width*0.7745966692414834);
    float mid = fFitIonizCalibFunction->Eval(mean);
    
    sumCC.push_back(sum);
    subCC.push_back(sub);
    midCC.push_back(mid);
    widthCC.push_back(width);
  }

  std::vector<Float_t> widthSC;
  std::vector<Float_t> sumSC, subSC, midSC;
  for(int i = 1; i < scBins+1; i++)
  {
    float mean = (scAxis->GetBinUpEdge(i) + scAxis->GetBinLowEdge(i))/2.;
    float width = (scAxis->GetBinUpEdge(i) - scAxis->GetBinLowEdge(i))/2.;

    float sum = fFitScintCalibFunction->Eval(mean + width*0.7745966692414834);
    float sub = fFitScintCalibFunction->Eval(mean - width*0.7745966692414834);
    float mid = fFitScintCalibFunction->Eval(mean);
    
    sumSC.push_back(sum);
    subSC.push_back(sub);
    midSC.push_back(mid);
    widthSC.push_back(width);
  }
  
  // calculate bivariate gaussian energy variables 
  std::vector<Float_t> normGauss, sigmaCC, sigmaSC, corrTerm;
  std::vector<float> emcCC, emcSC;
  
  for(size_t e = 0; e < energy.size(); e++)
  {
    float ene = energy[e];
    emcCC.push_back(ene);
    emcSC.push_back(ene);

    float tempCC = fFitIonizResolFunction->Eval(ene);
    float tempSC = fFitScintResolFunction->Eval(ene);
    float tempCorr = fFitCorrelationFunction->Eval(ene);
    
    float tempCorr2 = tempCorr*tempCorr;

    if(tempCorr2 >= 1)
    {
      std::cout << "Correlation is set to greater than 1, please set limits to the correlation fitting function!\n";
      exit(1);
    }
    
    if(!std::isfinite(tempCC) || !std::isfinite(tempSC) || !std::isfinite(tempCorr))
    {
      std::cout << "Resolution functions diverge. Quitting...\n";
      exit(1);
    }
      
    float tempSqrt = sqrt(2.*(1-tempCorr2));

    normGauss.push_back(0.225079079039277f / tempCC / tempSC / tempSqrt);
    sigmaCC.push_back(1. / tempCC / tempSqrt);
    sigmaSC.push_back(1. / tempSC / tempSqrt);
    corrTerm.push_back(2.*tempCorr);
  }

  // main loop, fill bins with contribution from each energy
  for(size_t e = 0; e < energy.size(); e++)
  {
    float eneCC = emcCC[e];
    float eneSC = emcSC[e];
    float corr = corrTerm[e];
    float sigmai = sigmaCC[e];
    float sigmaj = sigmaSC[e];  
    float normXpdf = normGauss[e]*pdf[e];

    for(int i = 0; i < ccBins; i++)
    {
      float sumi = sumCC[i];
      float subi = subCC[i];
      float midi = midCC[i];
      
      float pi = (sumi-eneCC)*sigmai;
      float mi = (subi-eneCC)*sigmai;
      float ci = (midi-eneCC)*sigmai;

      for(int j = 0; j < scBins; j++)
      {
        float subj = subSC[j];
        float sumj = sumSC[j];
        float midj = midSC[j];        

        float pj = (sumj-eneSC)*sigmaj;
        float mj = (subj-eneSC)*sigmaj;
        float cj = (midj-eneSC)*sigmaj;

        float texpp = FastExp(GaussTerm2(pi,pj,corr));//exp(gaussTerm2(pi,pj,corr));
        float texpm = FastExp(GaussTerm2(pi,mj,corr));//exp(gaussTerm2(pi,mj,corr));
        float texpc = FastExp(GaussTerm2(pi,cj,corr));//exp(gaussTerm2(pi,cj,corr));

        float texcp = FastExp(GaussTerm2(ci,pj,corr));//exp(gaussTerm2(ci,pj,corr));
        float texcm = FastExp(GaussTerm2(ci,mj,corr));//exp(gaussTerm2(ci,mj,corr));
        float texcc = FastExp(GaussTerm2(ci,cj,corr));//exp(gaussTerm2(ci,cj,corr));

        float texmp = FastExp(GaussTerm2(mi,pj,corr));//exp(gaussTerm2(mi,pj,corr));
        float texmm = FastExp(GaussTerm2(mi,mj,corr));//exp(gaussTerm2(mi,mj,corr));
        float texmc = FastExp(GaussTerm2(mi,cj,corr));//exp(gaussTerm2(mi,cj,corr));

        float sum = 0.308641975308f*(texpp + texpm + texmp + texmm) + 0.493827160493f*(texpc + texcp + texcm + texmc) + 0.79012345679f*texcc;
        
        content[j + scBins*i] += sum*normXpdf;
      }
    }
  }

  // set histo bin contents
  for(int i = 1; i < ccBins+1; i++)
  {
    float widthi = widthCC[i-1];
    for(int j = 1; j < scBins+1; j++){
      //std::cout << i << " " << j << " " << content[(j-1)+scBins*(i-1)] << std::endl;
      float widthj = widthSC[j-1];
      float correction = widthi*widthj*0.0001984127f;
      histo.SetBinContent(i,j,content[(j-1)+scBins*(i-1)]*correction);//0.0083333333f);
    }
  }
  
  return ;
}

void EXOEnergyCalibFitMC2D::GetScaleHistogram(TH2F& hScale, const TH2F& hData, const TH2F& hMC)
{
  hScale.Reset();
  
  int minBinXMC = 1;//hMC.FindFixBin(2450);// 1;//
  int maxBinXMC = hMC.GetNbinsX();//hMC.FindFixBin(2750);// hMC.GetNbinsX();//
  int minBinYMC = 1;//hMC.FindFixBin(2450);// 1;//
  int maxBinYMC = hMC.GetNbinsY();//hMC.FindFixBin(2750);// hMC.GetNbinsX();//

  int minBinXData = 1;//hData.FindFixBin(2450);// 1;//
  int maxBinXData = hData.GetNbinsX();//hData.FindFixBin(2750);// hData.GetNbinsX();//
  int minBinYData = 1;//hData.FindFixBin(2450);// 1;//
  int maxBinYData = hData.GetNbinsY();//hData.FindFixBin(2750);// hData.GetNbinsX();//

  double totMC = hMC.Integral(minBinXMC,maxBinXMC,minBinYMC,maxBinYMC);
  double totData = hData.Integral(minBinXData,maxBinXData,minBinYData,maxBinYData);
  double scaleDataMC = totData/totMC;

  for(Int_t i = 1; i < hScale.GetNbinsX()+1; i++)
    for(Int_t j = 1; j < hScale.GetNbinsY()+1; j++)
      hScale.SetBinContent(i,j,scaleDataMC);

  return;
}

void EXOEnergyCalibFitMC2D::SetDataHisto(Int_t nIonizBins, Double_t lowIonizEnergy, Double_t upIonizEnergy, Int_t nScintBins, Double_t lowScintEnergy, Double_t upScintEnergy)
{
  if(fDataHisto)
    delete fDataHisto;  
  fDataHisto = new TH2F("fDataHisto","",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
  if(fDataHisto2D)
    delete fDataHisto2D;
  fDataHisto2D = dynamic_cast<TH2F*>(fDataHisto);

  if(!fDataHisto2D)
  {
    std::cout << "There was a problem converting TH2 to TH2F. Data histogram has not been properly created.\n";
    return;
  }

  for(size_t i = 0; i < fIonizDataEnergyPoints.size(); i++)
    fDataHisto2D->Fill(fIonizDataEnergyPoints[i],fScintDataEnergyPoints[i]);

  if(fSmearedMCHisto)
    delete fSmearedMCHisto;
  fSmearedMCHisto = new TH2F("fSmearedMCHisto","",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
  if(fSmearedMCHisto2D)
    delete fSmearedMCHisto2D;
  fSmearedMCHisto2D = dynamic_cast<TH2F*>(fSmearedMCHisto);

  if(fScaleHisto)
    delete fScaleHisto;
  fScaleHisto = new TH2F("fScaleHisto","",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
  if(fScaleHisto2D)
    delete fScaleHisto2D;
  fScaleHisto2D = dynamic_cast<TH2F*>(fScaleHisto);
}

void EXOEnergyCalibFitMC2D::SetFunction(TString type, TString channel, TF1* func)
{
  TF1 **function;
  
  if(type == "calib" || type == "calibration")
  {
    if(channel == "ioniz" || channel == "ionization" || channel == "charge")
      function = &fFitIonizCalibFunction;
    else if(channel == "scint" || channel == "scintillation" || channel == "light")
      function = &fFitScintCalibFunction;
    else
    {
      std::cout << "Function type not found, exiting EXOEnergyCalibFitMC2D::SetFunction procedure w/o setting function...\n";
      return;
    }      
  }    
  else if(type == "resol" || type == "resolution")
  {
    if(channel == "ioniz" || channel == "ionization" || channel == "charge")
      function = &fFitIonizResolFunction;
    else if(channel == "scint" || channel == "scintillation" || channel == "light")
      function = &fFitScintResolFunction;
    else
    {
      std::cout << "Function type not found, exiting EXOEnergyCalibFitMC2D::SetFunction procedure w/o setting function...\n";
      return;
    }      
  }
  else if(type == "corr" || type == "correlation")
  {
    function = &fFitCorrelationFunction;
  }
  else
  {
    std::cout << "Function type not found, exiting EXOEnergyCalibFitMC2D::SetFunction procedure w/o setting function...\n";
    return;
  }

  if(*function)
    delete *function;

  *function = func;

  if(fVerboseLevel > 0)
    (*function)->Print();
  
  return;
}

bool EXOEnergyCalibFitMC2D::ExecuteFit(Int_t fitType)
{
  // check consistency of info used in fit
  if(fMCEnergyPoints.empty() || fMCEnergyPDF.empty()){std::cout << "MC energy PDF not found, please enter this info before fitting...\n";return false;}
  if(!fDataHisto){std::cout << "Histogram of data energy not found, please set this info before fitting...\n";return false;}

  if(!fFitIonizCalibFunction){std::cout << "Ionization calibration function not found, please set this info before fitting...\n";return false;}
  if(!fFitIonizResolFunction){std::cout << "Ionization resolution function not found, please set this info before fitting...\n";return false;}
  if(!fFitScintCalibFunction){std::cout << "Scintillation calibration function not found, please set this info before fitting...\n";return false;}
  if(!fFitScintResolFunction){std::cout << "Scintillation resolution function not found, please set this info before fitting...\n";return false;} 

  // add functions
  fFitFunctions.clear();
  fFitFunctions.push_back(fFitIonizCalibFunction);
  fFitFunctions.push_back(fFitIonizResolFunction);
  fFitFunctions.push_back(fFitScintCalibFunction);
  fFitFunctions.push_back(fFitScintResolFunction);
  fFitFunctions.push_back(fFitCorrelationFunction);
  
  fNpar = GetNumberOfFreeParameters();//fFitResolFunction->GetNpar() + fFitCalibFunction->GetNpar();
    
  // create MINUIT
  if(fVerboseLevel > 0)
    std::cout << "Creating TMinuit object...\n";
  
  ROOT::Math::Functor function(this, &EXOEnergyCalibFitMC2D::FitFunction,fNpar);
  const ROOT::Math::IMultiGenFunction& func = function;
  fFCN = const_cast<ROOT::Math::IMultiGenFunction *>(&func);

  double commandList[10];
  int errorFlag;

  if(fFitter)
    delete fFitter;

  fFitter = new TFitter(fNpar);
  fFitter->GetMinuit()->SetPrintLevel(fVerboseLevel);
  fFitter->SetFCN(&EXOEnergyCalibFitMCBase::Fcn);
    
  DefineInitialParameters();

  commandList[0] = 1e-6;
  fFitter->ExecuteCommand("SET EPS",commandList,1);

  fFitter->ExecuteCommand("SHOw FCNvalue",commandList,0);

  commandList[0] = 100000;
  commandList[1] = 0.1;
  fFitter->ExecuteCommand("MINImize",commandList,2);
  fFitter->ExecuteCommand("HESSe",commandList,1);
  
  FitFunction(&GetFittedParameters()[0]);

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

double EXOEnergyCalibFitMC2D::FitFunction(const double* x)
{
  if(fVerboseLevel > 1){
    std::cout << "Using parameters: ";
    for(int p = 0; p < fNpar; p++)
      std::cout << "p" << p << " = " << x[p] << " ";
    std::cout << "\n";
  }

  FillSmearedMCHistogram(x,*fSmearedMCHisto2D,fMCEnergyPoints,fMCEnergyPDF); 
  GetScaleHistogram(*fScaleHisto2D,*fDataHisto2D,*fSmearedMCHisto2D);

  int NDF = -fNpar;
  double Chi2 = 0.;
  double NLL = 0.;
  
  //double scale = fDataHisto1D->Integral(1,fDataHisto1D->GetNbinsX())/fSmearedMCHisto1D->Integral(1,fSmearedMCHisto1D->GetNbinsX());

  for(int xb = 1; xb < fDataHisto2D->GetNbinsX()+1; xb++)
  {
    for(int yb = 1; yb < fDataHisto2D->GetNbinsY()+1; yb++)
    {
      double observed = fDataHisto2D->GetBinContent(xb,yb);
      double expected = fSmearedMCHisto2D->GetBinContent(xb,yb);
      double scale = fScaleHisto2D->GetBinContent(xb,yb);

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
  }

  double FCN = (fFitType == 1) ? NLL : Chi2;
  
  if(fVerboseLevel > 1)
    std::cout << "FCN = " << FCN << " (Chi2/NDF = " << Chi2/NDF << ")" << std::endl;
  
  return FCN;
}

Double_t EXOEnergyCalibFitMC2D::GetPeakPosition(Double_t energy, TString channel, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);

  TF1 **function;
  
  if(channel == "ioniz" || channel == "ionization" || channel == "charge")
    function = &fFitIonizCalibFunction;
  else if(channel == "scint" || channel == "scintillation" || channel == "light")
    function = &fFitScintCalibFunction;
  else
  {
    std::cout << "Function channel not found...\n";
    return 0.;
  }
  
  return (*function)->GetX(energy);
}

Double_t EXOEnergyCalibFitMC2D::GetPeakPositionError(Double_t energy, TString channel, Int_t nDraws, TH1D *saveDraws)
{
  std::vector<double> vDraws;
  for(Int_t i = 0; i < nDraws; i++)
  {
    SetFitFunctionParameters(&DrawGausFitPars()[0]);
    vDraws.push_back(GetPeakPosition(energy,channel,false));
  }
  
  TH1D hDraws("hDraws","",nDraws/10,*std::min_element(vDraws.begin(),vDraws.end()),*std::max_element(vDraws.begin(),vDraws.end()));
  for(Int_t i = 0; i < nDraws; i++)
    hDraws.Fill(vDraws.at(i));

  hDraws.Fit("gaus","LQ0");
  double relError = hDraws.GetFunction("gaus")->GetParameter(2)/hDraws.GetFunction("gaus")->GetParameter(1);
    
  if(saveDraws)
    *saveDraws = hDraws;
  
  return relError*GetPeakPosition(energy,channel,true);
}

Double_t EXOEnergyCalibFitMC2D::GetPeakWidth(Double_t energy, TString channel, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);

  TF1 **function;
  
  if(channel == "ioniz" || channel == "ionization" || channel == "charge")
    function = &fFitIonizCalibFunction;
  else if(channel == "scint" || channel == "scintillation" || channel == "light")
    function = &fFitScintCalibFunction;
  else
  {
    std::cout << "Function channel not found...\n";
    return 0.;
  }
  
  return (*function)->Eval(energy);
}

Double_t EXOEnergyCalibFitMC2D::GetPeakWidthError(Double_t energy, TString channel, Int_t nDraws, TH1D *saveDraws)
{
  std::vector<double> vDraws;
  for(Int_t i = 0; i < nDraws; i++)
  {
    SetFitFunctionParameters(&DrawGausFitPars()[0]);
    vDraws.push_back(GetPeakWidth(energy,channel,false));
  }
  
  TH1D hDraws("hDraws","",nDraws/10,*std::min_element(vDraws.begin(),vDraws.end()),*std::max_element(vDraws.begin(),vDraws.end()));
  for(Int_t i = 0; i < nDraws; i++)
    hDraws.Fill(vDraws.at(i));

  hDraws.Fit("gaus","LQ0");
  double relError = hDraws.GetFunction("gaus")->GetParameter(2)/hDraws.GetFunction("gaus")->GetParameter(1);
    
  if(saveDraws)
    *saveDraws = hDraws;
  
  return relError*GetPeakWidth(energy,channel,true);
}
  
Double_t EXOEnergyCalibFitMC2D::GetCorrelation(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitCorrelationFunction->Eval(energy);
}

Double_t EXOEnergyCalibFitMC2D::GetCorrelationError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
{
  std::vector<double> vDraws;
  for(Int_t i = 0; i < nDraws; i++)
  {
    SetFitFunctionParameters(&DrawGausFitPars()[0]);
    vDraws.push_back(GetCorrelation(energy,false));
  }
  
  TH1D hDraws("hDraws","",nDraws/10,*std::min_element(vDraws.begin(),vDraws.end()),*std::max_element(vDraws.begin(),vDraws.end()));
  for(Int_t i = 0; i < nDraws; i++)
    hDraws.Fill(vDraws.at(i));

  hDraws.Fit("gaus","LQ0");
  double relError = hDraws.GetFunction("gaus")->GetParameter(2)/hDraws.GetFunction("gaus")->GetParameter(1);
    
  if(saveDraws)
    *saveDraws = hDraws;
  
  return relError*GetCorrelation(energy,true);
}
