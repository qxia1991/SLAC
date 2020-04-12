#include "EXOUtilities/EXOEnergyMCBasedFit2D.hh"

ClassImp(EXOEnergyMCBasedFit2D)

EXOEnergyMCBasedFit2D::~EXOEnergyMCBasedFit2D()
{
  if(fFitIonizResolFunction) delete fFitIonizResolFunction;
  if(fFitIonizCalibFunction) delete fFitIonizCalibFunction;
  if(fFitScintResolFunction) delete fFitScintResolFunction;
  if(fFitScintCalibFunction) delete fFitScintCalibFunction;
  if(fFitCorrelationFunction) delete fFitCorrelationFunction;
}

EXOEnergyMCBasedFit2D::EXOEnergyMCBasedFit2D() : EXOEnergyMCBasedFitBase()
{
  fDimension = 2;

  fFitIonizResolFunction = NULL;
  fFitIonizCalibFunction = NULL;
  fFitScintResolFunction = NULL;
  fFitScintCalibFunction = NULL;
  fFitCorrelationFunction = NULL;

  fMinFastExp = -25;
  fVecFastExp.resize(-fMinFastExp+1);
  for(Int_t i = 0; i >= fMinFastExp; i--)
    fVecFastExp[-i] = exp(i);

  SetOffset(0.);
  SetMaxOffset(0.1,true,true);
  SetMaxIterations(500);
}

float EXOEnergyMCBasedFit2D::FastExp(float x) // returns exponential divided by 0.0001984127f to save 1 multiplication per call
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

float EXOEnergyMCBasedFit2D::GaussTerm2(float xx0, float yy0, float corr)
{
  float mix = xx0 * yy0 * corr;
  xx0 *= xx0;
  yy0 *= yy0;
  return -xx0 -yy0 + mix;
}

void EXOEnergyMCBasedFit2D::FillSmearedMCHistogram(const double* x, TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf)
{
  SetFitFunctionParameters(x);

  return FillSmearedMCHistogram(histo,energy,pdf);
}
  
void EXOEnergyMCBasedFit2D::FillSmearedMCHistogram(TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf)
{

  if(fVerboseLevel > 1)
    std::cout << "Creating 2D smearing...\n";
  
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
      //std::cout << "Correlation is set to greater than 1, please set limits to the correlation fitting function!\n";
      //exit(1);
      //std::cout << "Correlation is set to greater than 1, will use maximum correlation!\n";
      //tempCorr = tempCorr > 0 ? 0.99999 : -0.99999;
      //tempCorr2 = tempCorr*tempCorr;

      std::cout << "Correlation is set to greater than 1, MC bin contents set to minimum!\n";
      // set histo bin contents
      for(int i = 1; i < ccBins+1; i++)
      {
        for(int j = 1; j < scBins+1; j++)
        {
          histo.SetBinContent(i,j,0.);
        }
      }      
      histo.SetEntries(histo.Integral(1,histo.GetNbinsX(),1,histo.GetNbinsY()));
      return ;

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

  histo.SetEntries(histo.Integral(1,histo.GetNbinsX(),1,histo.GetNbinsY()));
  
  return ;
}

void EXOEnergyMCBasedFit2D::FillSmearedMC2DHistogram(const double* x, TH2F& histo, const std::vector<std::pair<Double_t,Double_t> >& energy, const std::vector<Double_t>& pdf)
{
  SetFitFunctionParameters(x);

  return FillSmearedMC2DHistogram(histo,energy,pdf);
}

void EXOEnergyMCBasedFit2D::FillSmearedMC2DHistogram(TH2F& histo, const std::vector<std::pair<Double_t,Double_t> >& energy, const std::vector<Double_t>& pdf)
{
  if(fVerboseLevel > 1)
    std::cout << "Creating 2D smearing from MC2D ...\n";

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
    float eneX = energy[e].first;
    float eneY = energy[e].second;
    emcCC.push_back(eneX);
    emcSC.push_back(eneY);

    float tempCC = fFitIonizResolFunction->Eval(eneX);
    float tempSC = fFitScintResolFunction->Eval(eneY);
    float tempCorr = fFitCorrelationFunction->Eval(eneX);//Y); eneX = energy_mc is what we've been using to represent the rotated energy
    
    float tempCorr2 = tempCorr*tempCorr;

    if(tempCorr2 >= 1)
    {
      //std::cout << "Correlation is set to greater than 1, please set limits to the correlation fitting function!\n";
      //exit(1);
      //std::cout << "Correlation is set to greater than 1, will use maximum correlation!\n";
      //tempCorr = tempCorr > 0 ? 0.99999 : -0.99999;
      //tempCorr2 = tempCorr*tempCorr;

      std::cout << "Correlation is set to greater than 1, MC bin contents set to minimum!\n";
      // set histo bin contents
      for(int i = 1; i < ccBins+1; i++)
      {
        for(int j = 1; j < scBins+1; j++)
        {
          histo.SetBinContent(i,j,0.);
        }
      }      
      histo.SetEntries(histo.Integral(1,histo.GetNbinsX(),1,histo.GetNbinsY()));
      return ;

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

  histo.SetEntries(histo.Integral(1,histo.GetNbinsX(),1,histo.GetNbinsY()));
  
  return ;
}



void EXOEnergyMCBasedFit2D::GetScaleHistogram(TH2F& hScale, const TH2F& hData, const TH2F& hMC)
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

bool EXOEnergyMCBasedFit2D::AddMC2D(const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPointsX, Double_t *mcPointsY, Double_t *mcWeights, Int_t nMC)
{
  if(fVerboseLevel > 0)
    std::cout << "Adding MC 2D to MCList: " << mcId << std::endl;
  
  if(fMCList.count(mcId))
  {
    std::cerr << "MC id already exists, given MC not added to the MC list!\n";
    return false;
  }
  
  MCEntry newMCEntry;
  newMCEntry.InitializeMembers(sourceAtomicNumber,sourceMassNumber,sourceX,sourceY,sourceZ);
  newMCEntry.SetEnergy2D(mcPointsX,mcPointsY,mcWeights,nMC);
  
  fMCList[mcId] = newMCEntry;
  
  return true;
}

bool EXOEnergyMCBasedFit2D::AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataIonizPoints, Double_t *dataScintPoints, Int_t nData, Int_t *runs, Int_t nRuns, Int_t week)
{
  if(fDataList.count(dataId))
  {
    std::cout << "Given data Id already exists, given data not added to data list!\n";
    return false;
  }
  
  if(!fMCList.count(mcId))
  {
    std::cout << "No MC corresponds to given MC id, given data not added to data list!\n";
    return false;
  }

  DataEntry newDataEntry;
  newDataEntry.InitializeMembers(mcId,sourceAtomicNumber,sourceMassNumber,sourceX,sourceY,sourceZ,prescale,runs,nRuns,week);
  newDataEntry.SetEnergy("2d-ioniz",dataIonizPoints,nData);
  newDataEntry.SetEnergy("2d-scint",dataScintPoints,nData);

  fDataList[dataId] = newDataEntry;
    
  return true;
}

bool EXOEnergyMCBasedFit2D::SetDataHisto(const char* histoName, std::vector<TString>& dataIds, Int_t nIonizBins, Double_t lowIonizEnergy, Double_t upIonizEnergy, Int_t nScintBins, Double_t lowScintEnergy, Double_t upScintEnergy)
{
  HistoEntry newHistoEntry;

  for(std::vector<TString>::iterator dataId = dataIds.begin(); dataId != dataIds.end(); dataId++)
  {
    if(!fDataList.count((*dataId)))
    {
      std::cout << "Fail to find " << dataId->Data() << " in given data list, histogram will not be set up for fit!\n";
      return false;
    }

    newHistoEntry.DataIds.push_back((*dataId));

    DataEntry &dataEntry = fDataList.at(*dataId);

    if(dataEntry.UsePrescaleTriggerEff)
      newHistoEntry.UsePrescaleTriggerEff = true;

    if(dataEntry.DataHisto)
      delete dataEntry.DataHisto;
    dataEntry.DataHisto = new TH2F(Form("%s_%s_data",histoName,dataId->Data()),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
    TH2F *dataHisto2D = dynamic_cast<TH2F*>(dataEntry.DataHisto);
    if(dataHisto2D && dataEntry.DataEnergyPoints.count("2d-ioniz") && dataEntry.DataEnergyPoints.count("2d-scint"))
    {
      size_t n = dataEntry.DataEnergyPoints.at("2d-ioniz").size();
      for(size_t i = 0; i < n; i++)
        dataHisto2D->Fill(dataEntry.DataEnergyPoints.at("2d-ioniz")[i],dataEntry.DataEnergyPoints.at("2d-scint")[i]);
    }
    else
    {
      std::cout << "Data points not found to fill 2D histogram, histogram will not be set up for fit!\n";
      return false;
    }

    if(dataEntry.ScaleHisto)
      delete dataEntry.ScaleHisto;
    dataEntry.ScaleHisto = new TH2F(Form("%s_%s_scale",histoName,dataId->Data()),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);

    if(dataEntry.MCSmearedHisto)
      delete dataEntry.MCSmearedHisto;
    dataEntry.MCSmearedHisto = new TH2F(Form("%s_%s_smearedmc",histoName,dataId->Data()),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);

    MCEntry &mcEntry = fMCList.at(dataEntry.MCId);
    if(mcEntry.MCSmearedHisto)
    {
      TAxis *xAxis = mcEntry.MCSmearedHisto->GetXaxis();
      if(xAxis->GetNbins() != nIonizBins || xAxis->GetXmax() != lowIonizEnergy || xAxis->GetXmin() != upIonizEnergy)
      {
        std::cout << "MC histogram does not match given properties, this histogram will not be set up for the fit!\n";
        return false;
      }
      TAxis *yAxis = mcEntry.MCSmearedHisto->GetYaxis();
      if(yAxis->GetNbins() != nScintBins || yAxis->GetXmax() != lowScintEnergy || yAxis->GetXmin() != upScintEnergy)
      {
        std::cout << "MC histogram does not match given properties, this histogram will not be set up for the fit!\n";
        return false;
      }      
    }
    else
    {
      mcEntry.MCSmearedHisto  = new TH2F(Form("%s_histo_smearedmc",dataEntry.MCId.Data()),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
    }
  }

  newHistoEntry.MCSmearedHisto = new TH2F(Form("%s_smearedmc",histoName),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);
  newHistoEntry.DataHisto = new TH2F(Form("%s_data",histoName),"",nIonizBins,lowIonizEnergy,upIonizEnergy,nScintBins,lowScintEnergy,upScintEnergy);

  fHistoList.push_back(newHistoEntry);

  return true; 
}

void EXOEnergyMCBasedFit2D::SetFunction(TString type, TString channel, TF1* func)
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
      std::cout << "Function type not found, exiting EXOEnergyMCBasedFit2D::SetFunction procedure w/o setting function...\n";
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
      std::cout << "Function type not found, exiting EXOEnergyMCBasedFit2D::SetFunction procedure w/o setting function...\n";
      return;
    }      
  }
  else if(type == "corr" || type == "correlation")
  {
    function = &fFitCorrelationFunction;
  }
  else
  {
    std::cout << "Function type not found, exiting EXOEnergyMCBasedFit2D::SetFunction procedure w/o setting function...\n";
    return;
  }

  if(*function)
    delete *function;

  *function = func;

  if(fVerboseLevel > 0)
    (*function)->Print();
  
  return;
}

bool EXOEnergyMCBasedFit2D::ExecuteFit(Int_t fitType, const char* minimizer, Float_t fitPrecision, Int_t minEventBin)
{
  fFitType = fitType;
  fMinEventBin = minEventBin;
  
  if(BuildFitter())
  {
    bool fitStatus = false;
    double fmin, fedm, errdef;
    int npari, nparx, istat;
    do
    {
      if(fitStatus)
      {
        SetOffset(fOffset+GetMinuit()->fAmin);
        fFitter->GetMinuit()->mnrset(1);
      }
      fitStatus = RunFitter(minimizer,fitPrecision);
      if(fMaxOffsetActivated)
      {
        std::cout << "*** Comparing MINIMUM found " << GetMinuit()->fAmin << " to MAX OFFSET " << fMaxOffset << std::endl;
        fFitter->GetMinuit()->mnstat(fmin,fedm,errdef,npari,nparx,istat);
      }
    }
    while(fMaxOffsetActivated && (!(std::abs(GetMinuit()->fAmin) < fMaxOffset) && ((istat < 3 || !fMaxOffsetStopSuccessCov)))) ;
    return fitStatus;
  }
  
  return false;
}

void EXOEnergyMCBasedFit2D::SetOffset(double offset)
{
  std::cout << "***** Setting fit function OFFSET to: " << offset << std::endl;
  fOffset = offset;
}

void EXOEnergyMCBasedFit2D::SetMaxOffset(double maxOffset,bool activate, bool stopaccov)
{
  fMaxOffset = maxOffset;
  fMaxOffsetActivated = activate;
  fMaxOffsetStopSuccessCov = stopaccov;
  std::cout << "***** Setting max offset to: " << fMaxOffset << ", activation: " << fMaxOffsetActivated << " and stop for good covariance matrix: " << fMaxOffsetStopSuccessCov  << std::endl;
}

void EXOEnergyMCBasedFit2D::SetMaxIterations(int maxIter)
{
  fMaxIterations = maxIter;
}


bool EXOEnergyMCBasedFit2D::BuildFitter()
{ 
  // check consistency of info used in fit
  if(fHistoList.empty())
  {
    std::cout << "Fitting histograms are not setup, please set histograms before the fit procedure. Skipping fit...\n";
    return false;
  }

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
  
  //ROOT::Math::Functor function(this, &EXOEnergyMCBasedFit2D::FitFunction,fNpar);
  //const ROOT::Math::IMultiGenFunction& func = function;
  //fFCN = const_cast<ROOT::Math::IMultiGenFunction *>(&func);
  fFitFunction = new ROOT::Math::Functor(this, &EXOEnergyMCBasedFit2D::FitFunction,fNpar);
  const ROOT::Math::IMultiGenFunction& func = *fFitFunction;
  fFCN = const_cast<ROOT::Math::IMultiGenFunction *>(&func);

  double commandList[10];
  int errorFlag;

  if(fFitter)
    delete fFitter;

  fFitter = new TFitter(fNpar);
  fFitter->GetMinuit()->SetPrintLevel(fVerboseLevel);
  fFitter->SetFCN(&EXOEnergyMCBasedFitBase::Fcn);
    
  DefineInitialParameters();

  //fFitter->ExecuteCommand("SHOw FCNvalue",commandList,0);
  commandList[0] = (fFitType == 2) ? 1 : 0.5; //GetErrorDef(1) : GetErrorDef(0.5);
  fFitter->ExecuteCommand("SET ERR",commandList,1);

  commandList[0] = 1e-6;
  fFitter->ExecuteCommand("SET EPS",commandList,1);

  return true;
}

bool EXOEnergyMCBasedFit2D::RunFitter(const char* minimizer, Float_t fitPrecision)
{
  if(!fFitter)
  {
    std::cerr << " You must build the fitter before running the fit!!!\n";
    return false;
  }

  double commandList[10];
  int errorFlag;
  
  commandList[0] = fMaxIterations; //1000;//00;
  commandList[1] = fitPrecision;//0.1;

  fFitter->ExecuteCommand("SHOw FCNvalue",commandList,0);
  
  fFitter->ExecuteCommand("SIMPLEX",commandList,2);
  //fFitter->ExecuteCommand("MINImize",commandList,2);  
  fFitter->ExecuteCommand(minimizer,commandList,2);
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

double EXOEnergyMCBasedFit2D::FitFunction(const double* x)
{
  if(fVerboseLevel > 1 && x){
    std::cout << "Using parameters: ";
    for(int p = 0; p < fNpar; p++)
      std::cout << "p" << p << " = " << x[p] << " ";
    std::cout << "\n";
  }

  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    MCEntry &mc = mcEntry->second;
    if(mc.Is2D)
      FillSmearedMC2DHistogram(x,*(dynamic_cast<TH2F*>(mc.MCSmearedHisto)),mc.MCEnergyPoints2D,mc.MCEnergyPDF);
    else
      FillSmearedMCHistogram(x,*(dynamic_cast<TH2F*>(mc.MCSmearedHisto)),mc.MCEnergyPoints,mc.MCEnergyPDF);
  }

  //GetScaleHistogram(*fScaleHisto2D,*fDataHisto2D,*fSmearedMCHisto2D);
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);

    histo.DataHisto->Reset("ICES");
    histo.MCSmearedHisto->Reset("ICES");

    TH2F* dataHisto2D = dynamic_cast<TH2F*>(histo.DataHisto);
    TH2F* mcHisto2D = dynamic_cast<TH2F*>(histo.MCSmearedHisto);

    for(std::vector<TString>::iterator dataId = histo.DataIds.begin(); dataId != histo.DataIds.end(); dataId++)
    {
      DataEntry &data = fDataList.at(*dataId);
      MCEntry &mc = fMCList.at(data.MCId);

      TH2F* idScaleHisto2D = dynamic_cast<TH2F*>(data.ScaleHisto);
      TH2F* idDataHisto2D = dynamic_cast<TH2F*>(data.DataHisto);
      TH2F* idMCSmearedHisto2D = dynamic_cast<TH2F*>(data.MCSmearedHisto);
      TH2F* fullMCSmearedHisto2D = dynamic_cast<TH2F*>(mc.MCSmearedHisto);

      GetScaleHistogram(*idScaleHisto2D,*idDataHisto2D,*fullMCSmearedHisto2D);
      for(int xb = 1; xb <= idDataHisto2D->GetNbinsX(); xb++)
      {
        for(int yb = 1; yb <= idDataHisto2D->GetNbinsY(); yb++)
        {
          double observed = idDataHisto2D->GetBinContent(xb,yb);
          double expected = fullMCSmearedHisto2D->GetBinContent(xb,yb);
          double scale = idScaleHisto2D->GetBinContent(xb,yb);

          idMCSmearedHisto2D->SetBinContent(xb,yb,scale*expected);
          idMCSmearedHisto2D->SetBinError(xb,yb,scale*sqrt(expected));
        }
      }
      idMCSmearedHisto2D->SetEntries(idMCSmearedHisto2D->Integral(1,idMCSmearedHisto2D->GetNbinsX(),1,idMCSmearedHisto2D->GetNbinsY()));

      dataHisto2D->Add(idDataHisto2D);
      mcHisto2D->Add(idMCSmearedHisto2D);
    }
  }
  
  double NDF = 0;//-fNpar;
  double Chi2 = -fOffset; //0.;
  double NLL = -fOffset; //0.;
  
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);

    TH2F* dataHisto2D = dynamic_cast<TH2F*>(histo.DataHisto);
    TH2F* mcHisto2D = dynamic_cast<TH2F*>(histo.MCSmearedHisto);

    for(int xb = 1; xb <= dataHisto2D->GetNbinsX(); xb++)
    {
      for(int yb = 1; yb <= dataHisto2D->GetNbinsY(); yb++)
      {
        double observed = dataHisto2D->GetBinContent(xb,yb);
        double expected = mcHisto2D->GetBinContent(xb,yb);
        double scaledError = mcHisto2D->GetBinError(xb,yb);
        double scaledError2 = scaledError*scaledError;
        
        if(observed > fMinEventBin)
        {
          double diff = observed - expected;//scale*expected;
          double chi2bin = diff*diff / observed; //(observed + scaledError2);;//scale*scale*expected);
          Chi2 += chi2bin;
          NDF += 1;
        }

        if(expected <= 0)
          expected = 1e-32;
 
        double nllBin = expected - observed;
        if(expected > 0)
        {
          if(observed > 0)
            nllBin -= observed*log(expected*1./observed);
          else
            nllBin -= observed*log(expected);
        }      
        else if(observed > 0)
        {
          NLL = std::numeric_limits<double>::max();
          if(fFitType == 1)
            return NLL;
        }
        
        NLL += nllBin;
      }
    }
  }

  double FCN = (fFitType == 2) ? Chi2 : NLL;
  
  if(fVerboseLevel > 1)
    std::cout << "FCN (offset) = " << FCN << " (Chi2/NDF = " << (Chi2+fOffset)/NDF << ")" << std::endl;

  if(!std::isfinite(FCN))
    FCN = std::numeric_limits<double>::max()/std::numeric_limits<float>::max();

  //fChi2 = Chi2/NDF;
  
  return FCN;
}

Double_t EXOEnergyMCBasedFit2D::GetPeakPosition(Double_t energy, TString channel, bool fitted)
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

Double_t EXOEnergyMCBasedFit2D::GetPeakPositionError(Double_t energy, TString channel, Int_t nDraws, TH1D *saveDraws)
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

Double_t EXOEnergyMCBasedFit2D::GetPeakWidth(Double_t energy, TString channel, bool fitted)
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

Double_t EXOEnergyMCBasedFit2D::GetPeakWidthError(Double_t energy, TString channel, Int_t nDraws, TH1D *saveDraws)
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
  
Double_t EXOEnergyMCBasedFit2D::GetCorrelation(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitCorrelationFunction->Eval(energy);
}

Double_t EXOEnergyMCBasedFit2D::GetCorrelationError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
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

TH2F* EXOEnergyMCBasedFit2D::GetSmearedMCHistogram()
{
  TH2F* result = new TH2F();
  //TH2F* result;
  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    MCEntry &mc = mcEntry->second;
    TH2F* smearedHisto = dynamic_cast<TH2F*>(mc.MCSmearedHisto);
    
    if(mc.Is2D)
      FillSmearedMC2DHistogram(*smearedHisto,mc.MCEnergyPoints2D,mc.MCEnergyPDF);
    else
      FillSmearedMCHistogram(*smearedHisto,mc.MCEnergyPoints,mc.MCEnergyPDF);

    if(mcEntry == fMCList.begin())
      result = dynamic_cast<TH2F*>(smearedHisto->Clone());
    else
      result->Add(smearedHisto);
  }

  return result;
}
