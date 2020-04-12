#include "EXOUtilities/EXOEnergyMCBasedFit1D.hh"

ClassImp(EXOEnergyMCBasedFit1D)

EXOEnergyMCBasedFit1D::~EXOEnergyMCBasedFit1D()
{
  if(fFitResolFunction) delete fFitResolFunction;
  if(fFitCalibFunction) delete fFitCalibFunction;
  if(fFitTrigEffFunction) delete fFitTrigEffFunction;
  if(fIgnoreBounds) delete fIgnoreBounds;
}

EXOEnergyMCBasedFit1D::EXOEnergyMCBasedFit1D():EXOEnergyMCBasedFitBase()
{
  fDimension = 1;
  fFitResolFunction = NULL;
  fFitCalibFunction = NULL;
  fFitTrigEffFunction = NULL;
  fUseHistoWeights = true;
  fIgnoreBounds = NULL;
}

bool EXOEnergyMCBasedFit1D::SmearedMCHistoCalib(std::vector<double>& e_up, double& elow, const TH1D& histo)
{
  for(int b = 1; b <= histo.GetNbinsX(); b++){
    e_up.push_back(fFitCalibFunction->Eval(histo.GetBinLowEdge(b) + histo.GetBinWidth(b)));
  }
  elow = fFitCalibFunction->Eval(histo.GetBinLowEdge(1));

  return true;
}

bool EXOEnergyMCBasedFit1D::SmearedMCHistoResol(std::vector<double>& weight, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf, const std::vector<double>& e_up, double elow)
{
  long int n = energy.size();
  int nBins = e_up.size();
  
  for(long int i = 0; i < n; i++){
    double e_mc = energy.at(i);
    double res = fFitResolFunction->Eval(e_mc);//sqrt(p0*p0*e_mc + p1*p1 + p2*p2*e_mc*e_mc);
    double const_res = 0.7071067811865474 / res;
    double erflow = erf(const_res * (elow - e_mc) );
    for(int b = 0; b < nBins; b++){
      double erfup = erf(const_res * (e_up.at(b) - e_mc) );
      weight[b] += (erfup - erflow)*pdf.at(i);
      erflow = erfup;
    }
  }
  return true;
}

bool EXOEnergyMCBasedFit1D::FillSmearedMCHistogram(const double* x, TH1D& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf)
{
  if(fVerboseLevel > 1)
    std::cout << "Creating 1D smearing...\n";

  if(!SetFitFunctionParameters(x))
    return false;

  std::vector<double> e_up;
  double elow;
  SmearedMCHistoCalib(e_up,elow,histo);
  
  int nBins = histo.GetNbinsX();
  std::vector<double> weight(nBins, 0.);
  SmearedMCHistoResol(weight,energy,pdf,e_up,elow);

  for(int b = 0; b < nBins; b++){
    histo.SetBinContent(b+1, weight[b]/2.);
    histo.SetBinError(b+1,sqrt(histo.GetBinContent(b+1)));
  }
    
  histo.SetEntries(histo.Integral(1,histo.GetNbinsX()));

  return true;
}

void EXOEnergyMCBasedFit1D::GetScaleHistogram(TH1D& hScale, const TH1D& hData, const TH1D& hMC, bool usePrescaleModel)
{
  double eCut = 0;
  if(usePrescaleModel && fFitTrigEffFunction)
    if(fFitTrigEffFunction->GetNpar() > 0)
        eCut = fFitTrigEffFunction->GetParameter(0);

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
    
    //double constRes = 0.7071067811865474 / (eResolution*eCut);

    for(int b = 1; b < hScale.GetNbinsX()+1; b++){
      // if(scaleLow < scaleHigh)
      hScale.SetBinContent(b,scaleLow + (scaleHigh - scaleLow)*fFitTrigEffFunction->Eval(hScale.GetBinCenter(b)));//*(1. + erf(constRes * (hScale.GetBinCenter(b) - eCut)))/2.);
      //else
      //  hScale.SetBinContent(b,scaleHigh + (scaleLow - scaleHigh)*(1. - erf(constRes * (hScale.GetBinCenter(b) - eCut)))/2.);
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

void EXOEnergyMCBasedFit1D::ApplyFittedCalibration(Int_t nBins, Double_t lowEnergy, Double_t upEnergy, bool useFitResults)
{
  if(!fFitCalibFunction)
  {
    std::cerr << "No fitted calibration! Please set calibration function.\n";
    return;
  }

  if(useFitResults)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  
  for(std::map<TString, DataEntry>::iterator dataEntry = fDataList.begin(); dataEntry != fDataList.end(); dataEntry++)
  {
    DataEntry & data = dataEntry->second;
    if(fFitCalibFunction)
      ApplyCalibration(data.DataEnergyPoints.at("1d"),fFitCalibFunction);
  }

  // for each fitted histogram, form a new one calibrated
  size_t n = fHistoList.size();
  for(size_t i = 0; i < n; i++)
  {
    //std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
    
    HistoEntry &histo = fHistoList.at(i);//(*histoEntry);
    TH1D* dataHisto1D = dynamic_cast<TH1D*>(histo.DataHisto);
    if(dataHisto1D)
    {
      TAxis *xAxis = dataHisto1D->GetXaxis();
      SetDataHisto(Form("calib_%s",histo.HistoName.Data()),"",histo.DataIds,nBins,lowEnergy,upEnergy,histo.Weight,true);
    }
  }
  fHistoList.erase(fHistoList.begin(),fHistoList.begin()+n);

  // now that we have calibrated data, replace calibration function by identity for MC smearing
  TF1 x("identity","x",fFitCalibFunction->GetXmin(),fFitCalibFunction->GetXmax());
  TF1 *tempFitCalibFunction = fFitCalibFunction;
  fFitCalibFunction = &x;

  // calculate fit function
  FitFunction(0); // 0 will not change calibration function settings

  fFitCalibFunction = tempFitCalibFunction;
  
  return;  
}

bool EXOEnergyMCBasedFit1D::AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataPoints, Int_t nData, Int_t *runs, Int_t nRuns, Int_t week)
{
  if(fVerboseLevel > 0)
    std::cout << "Adding Data to DataList: " << dataId << " associated to MC id = " << mcId << std::endl;

  if(fDataList.count(dataId))
  {
    std::cerr << "Given data Id already exists, given data not added to data list!\n";
    return false;
  }
  
  if(!fMCList.count(mcId))
  {
    std::cerr << "No MC corresponds to given MC id, given data not added to data list!\n";
    return false;
  }

  DataEntry newDataEntry;
  newDataEntry.InitializeMembers(mcId,sourceAtomicNumber,sourceMassNumber,sourceX,sourceY,sourceZ,prescale,runs,nRuns,week);
  newDataEntry.SetEnergy("1d",dataPoints,nData);

  fDataList[dataId] = newDataEntry;
    
  return true;
}

bool EXOEnergyMCBasedFit1D::SetDataHisto(const char* histoName, std::string histoTitle, std::vector<TString>& dataIds, Int_t nBins, Double_t lowEnergy, Double_t upEnergy, double weight, bool forceNewMC)
{
  if(fVerboseLevel > 0)
  {
    std::cout << "Setting fit Histo to FitHistoList: " << histoName << " with associated data ids:" << " and weight = " << weight << std::endl;
    for(std::vector<TString>::iterator dataId = dataIds.begin(); dataId != dataIds.end(); dataId++)
      std::cout << *dataId << std::endl;
  }

  HistoEntry newHistoEntry;
  newHistoEntry.HistoName = histoName;
  newHistoEntry.Weight = weight;

  std::set<TString> forceNewMCIds;

  for(std::vector<TString>::iterator dataId = dataIds.begin(); dataId != dataIds.end(); dataId++)
  {
    
    if(!fDataList.count((*dataId)))
    {
      std::cerr << "Fail to find " << dataId->Data() << " in given data list, histogram will not be set up for fit!\n";
      return false;
    }
    
    newHistoEntry.DataIds.push_back((*dataId));

    DataEntry &dataEntry = fDataList.at(*dataId);
    
    if(dataEntry.UsePrescaleTriggerEff && (dataEntry.SourceAtomicNumber != 55 && dataEntry.SourceMassNumber != 137))
    {
      if(fVerboseLevel > 1)
        std::cout << "Setting prescale because " << dataEntry.SourceAtomicNumber << " and " << dataEntry.SourceMassNumber << " id " << *dataId << std::endl;
      newHistoEntry.UsePrescaleTriggerEff = true;
    }

    if(dataEntry.DataHisto)
      delete dataEntry.DataHisto;
    dataEntry.DataHisto = new TH1D(Form("%s_%s_data",newHistoEntry.HistoName.Data(),dataId->Data()),histoTitle.c_str(),nBins,lowEnergy,upEnergy);
    TH1D *dataHisto1D = dynamic_cast<TH1D*>(dataEntry.DataHisto);
    if(dataHisto1D && dataEntry.DataEnergyPoints.count("1d"))
    {
      for(size_t i = 0; i < dataEntry.DataEnergyPoints.at("1d").size(); i++)
        dataHisto1D->Fill(dataEntry.DataEnergyPoints.at("1d")[i]);
    }
    else
    {
      std::cerr << "Data points not found to fill 1D histogram, histogram will not be set up for fit!\n";
      return false;
    }
    
    if(dataEntry.ScaleHisto)
      delete dataEntry.ScaleHisto;
    dataEntry.ScaleHisto = new TH1D(Form("%s_%s_scale",newHistoEntry.HistoName.Data(),dataId->Data()),histoTitle.c_str(),nBins,lowEnergy,upEnergy);

    if(dataEntry.MCSmearedHisto)
      delete dataEntry.MCSmearedHisto;
    dataEntry.MCSmearedHisto = new TH1D(Form("%s_%s_smearedmc",newHistoEntry.HistoName.Data(),dataId->Data()),histoTitle.c_str(),nBins,lowEnergy,upEnergy);
    
    MCEntry &mcEntry = fMCList.at(dataEntry.MCId);
    if(mcEntry.MCSmearedHisto)
    {
      TAxis *xAxis = mcEntry.MCSmearedHisto->GetXaxis();
      if(xAxis->GetNbins() != nBins || xAxis->GetXmax() != upEnergy || xAxis->GetXmin() != lowEnergy)
      {
        if(!forceNewMC)
        {
          std::cerr << "MC histogram does not match given properties, this histogram will not be set up for the fit!\n";
          return false;
        }
        else
          forceNewMCIds.insert(dataEntry.MCId);
      }
    }
    else
    {
      mcEntry.MCSmearedHisto  = new TH1D(Form("%s_histo_smearedmc",dataEntry.MCId.Data()),histoTitle.c_str(),nBins,lowEnergy,upEnergy);
    }
  }

  if(!forceNewMCIds.empty())
  {
    for(std::set<TString>::iterator forceNewMCId = forceNewMCIds.begin(); forceNewMCId != forceNewMCIds.end(); forceNewMCId++)
    {
      MCEntry &mcEntry = fMCList.at(*forceNewMCId);
      if(mcEntry.MCSmearedHisto)
        delete mcEntry.MCSmearedHisto;
      mcEntry.MCSmearedHisto  = new TH1D(Form("%s_histo_smearedmc",forceNewMCId->Data()),"",nBins,lowEnergy,upEnergy);
    }
  }      

  newHistoEntry.MCSmearedHisto = new TH1D(Form("%s_smearedmc",newHistoEntry.HistoName.Data()),"",nBins,lowEnergy,upEnergy);
  
  newHistoEntry.DataHisto = new TH1D(Form("%s_data",newHistoEntry.HistoName.Data()),"",nBins,lowEnergy,upEnergy);
  for(std::vector<TString>::iterator dataId = newHistoEntry.DataIds.begin(); dataId != newHistoEntry.DataIds.end(); dataId++)
  {
    DataEntry &data = fDataList.at(*dataId);
    newHistoEntry.DataHisto->Add(data.DataHisto);
  }
  newHistoEntry.NumberOfEvents = newHistoEntry.DataHisto->Integral(1,nBins);
  /*
  if(newHistoEntry.NumberOfEvents <= 0)
  {
    std::cerr << "Trying to set a fitting histogram without events! Histogram not set...\n";
    return false;
  }
  */
  newHistoEntry.FillFitBins1D(fIgnoreBounds);
  fHistoList.push_back(newHistoEntry);

  return true; 
}

void EXOEnergyMCBasedFit1D::SetFunction(TString type, TF1* func)
{
  TF1 **function;
  if(type == "calib" || type == "calibration")
    function = &fFitCalibFunction;
  else if(type == "resol" || type == "resolution")
    function = &fFitResolFunction;
  else if(type == "trig" || type == "trigger" || type == "trigeff" || type == "prescale")
    function = &fFitTrigEffFunction;
  else
  {
    std::cerr << "Function type not found, exiting EXOEnergyMCBasedFit1D::SetFunction procedure w/o setting function...\n";
    return;
  }

  if(*function)
    delete *function;

  TF2* testF2 = dynamic_cast<TF2*>(func);
  if(testF2)
    *function = new TF2(*testF2);
  else
    *function = new TF1(*func);

  if(fVerboseLevel > 0)
    (*function)->Print();
  
  return;
}

TF1 *EXOEnergyMCBasedFit1D::GetFunction(TString type)
{
  
  if(type == "calib" || type == "calibration")
    return fFitCalibFunction;
  else if(type == "resol" || type == "resolution")
    return fFitResolFunction;
  else if(type == "trig" || type == "trigger" || type == "trigeff" || type == "prescale")
    return fFitTrigEffFunction;
  

  return 0;
}

bool EXOEnergyMCBasedFit1D::BuildFitter()
{
  // check consistency of info used in fit
  if(fHistoList.empty())
  {
    std::cerr << "Fitting histograms are not setup, please set histograms before the fit procedure. Skipping fit...\n";
    return false;
  }
    
  if(!fFitCalibFunction)
  {
    std::cerr << "Calibration function not found, please set this info before fitting...\n";
    return false;
  }

  if(!fFitResolFunction)
  {
    std::cerr << "Resolution function not found, please set this info before fitting...\n";
    return false;
  }

  fFitFunctions.clear();
  fFitFunctions.push_back(fFitCalibFunction);
  fFitFunctions.push_back(fFitResolFunction);
  if(fFitTrigEffFunction)
    fFitFunctions.push_back(fFitTrigEffFunction);

  fNpar = GetNumberOfFreeParameters();//fFitResolFunction->GetNpar() + fFitCalibFunction->GetNpar();
    
  // create MINUIT
  if(fVerboseLevel > 0)
    std::cout << "Creating TMinuit object...\n";
  
  //ROOT::Math::Functor function(this, &EXOEnergyMCBasedFit1D::FitFunction,fNpar);
  //const ROOT::Math::IMultiGenFunction& func = function;
  //fFCN = const_cast<ROOT::Math::IMultiGenFunction *>(&func);
  fFitFunction = new ROOT::Math::Functor(this, &EXOEnergyMCBasedFit1D::FitFunction,fNpar);
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
  commandList[0] = (fFitType == 2) ? GetErrorDef(1) : GetErrorDef(0.5);
  fFitter->ExecuteCommand("SET ERR",commandList,1);

  return true;
}

bool EXOEnergyMCBasedFit1D::RunFitter(const char* minimizer, Float_t fitPrecision)
{
  if(!fFitter)
  {
    std::cerr << " You must build the fitter before running the fit!!!\n";
    return false;
  }
  
  double commandList[10];
  int errorFlag;

  commandList[0] = 100000;
  commandList[1] = fitPrecision;

  fFitter->ExecuteCommand(minimizer,commandList,2);

  if(!fAllFitCalculations)
    return true;
  
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

bool EXOEnergyMCBasedFit1D::ExecuteFit(Int_t fitType, const char* minimizer, Float_t fitPrecision)
{
  fFitType = fitType;

  if(BuildFitter())
    return RunFitter(minimizer,fitPrecision);

  return false;
}

double EXOEnergyMCBasedFit1D::FitFunction(const double* x)
{

  //print out starting parameters
  if(fVerboseLevel > 1 && x){
    std::cout << "Using parameters: ";
    for(int p = 0; p < fNpar; p++)
      std::cout << "p" << p << " = " << x[p] << " ";
    std::cout << "\n";
  }

  // loop through MC entries and smearing histograms
  bool allOk = true;
  for(std::map<TString, MCEntry>::iterator mcEntry = fMCList.begin(); mcEntry != fMCList.end(); mcEntry++)
  {
    MCEntry &mc = mcEntry->second;
    if(fVerboseLevel > 2)
      std::cout << "Smearing MC id " << mcEntry->first.Data() << std::endl;
    TH1D* mcHisto1D = dynamic_cast<TH1D*>(mc.MCSmearedHisto);
    if(!mcHisto1D)
      continue;
    if(!FillSmearedMCHistogram(x,*mcHisto1D,mc.MCEnergyPoints,mc.MCEnergyPDF))
      allOk = false;
  }

  if(!allOk)
    return std::numeric_limits<double>::max();
  
  //GetTriggerScale(*fScaleHisto1D,*fDataHisto1D,*fSmearedMCHisto1D);
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);

    if(histo.DataHisto)
      histo.DataHisto->Reset("ICES");
    if(histo.MCSmearedHisto)       
      histo.MCSmearedHisto->Reset("ICES");

    TH1D* dataHisto1D = dynamic_cast<TH1D*>(histo.DataHisto);
    TH1D* mcHisto1D = dynamic_cast<TH1D*>(histo.MCSmearedHisto);

    for(std::vector<TString>::iterator dataId = histo.DataIds.begin(); dataId != histo.DataIds.end(); dataId++)
    {
      DataEntry &data = fDataList.at(*dataId);
      MCEntry &mc = fMCList.at(data.MCId);

      TH1D* idScaleHisto1D = dynamic_cast<TH1D*>(data.ScaleHisto);
      TH1D* idDataHisto1D = dynamic_cast<TH1D*>(data.DataHisto);
      TH1D* idMCSmearedHisto1D = dynamic_cast<TH1D*>(data.MCSmearedHisto);
      TH1D* fullMCSmearedHisto1D = dynamic_cast<TH1D*>(mc.MCSmearedHisto);

      GetScaleHistogram(*idScaleHisto1D,*idDataHisto1D,*fullMCSmearedHisto1D,data.UsePrescaleTriggerEff);
      for(int b = 1; b <= idDataHisto1D->GetNbinsX(); b++)
      {
        double observed = idDataHisto1D->GetBinContent(b);
        double expected = fullMCSmearedHisto1D->GetBinContent(b);
        double scale = idScaleHisto1D->GetBinContent(b);
        idMCSmearedHisto1D->SetBinContent(b,scale*expected);
        idMCSmearedHisto1D->SetBinError(b,scale*sqrt(expected));
      }
      idMCSmearedHisto1D->SetEntries(idMCSmearedHisto1D->Integral(1,idMCSmearedHisto1D->GetNbinsX()));

      dataHisto1D->Add(idDataHisto1D);
      mcHisto1D->Add(idMCSmearedHisto1D);
    }
  }
    
  double NDF = 0;//-fNpar;
  double Chi2 = 0.;
  double NLL = 0.;

  double weightNorm = 0.;    
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
  {
    HistoEntry &histo = (*histoEntry);

    TH1D* dataHisto1D = dynamic_cast<TH1D*>(histo.DataHisto);
    TH1D* mcHisto1D = dynamic_cast<TH1D*>(histo.MCSmearedHisto);

    if(histo.Weight != 0)
      weightNorm += histo.NumberOfEvents / histo.Weight;
    else
      continue;
    
    double histoNDF = 0.;
    double histoChi2 = 0;
    double histoNLL = 0;
    for(std::set<int>::iterator pb = histo.FitBins.begin(); pb != histo.FitBins.end(); pb++)//int b = 1; b <= dataHisto1D->GetNbinsX(); b++)
    {
      int b = (*pb);
      double observed = dataHisto1D->GetBinContent(b);
      double expected = mcHisto1D->GetBinContent(b);
      double scaledError = mcHisto1D->GetBinError(b);
      double scaledError2 = scaledError*scaledError;      

      if(observed > 0)
      {
        double diff = observed - expected;//scale*expected;
        double chi2bin = diff*diff / (observed + scaledError2);
        histoChi2 += chi2bin;// * histo.Weight / histo.NumberOfEvents;
        histoNDF += 1.;
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
      
      histoNLL += nllBin;// * histo.Weight / histo.NumberOfEvents;
    }
    if (fUseHistoWeights == false){
      Chi2 += histoChi2; //* histo.Weight / histo.NumberOfEvents;
      NDF += histoNDF; //* histo.Weight / histo.NumberOfEvents;
      NLL += histoNLL; //* histo.Weight / histo.NumberOfEvents;
    }
    else {
      Chi2 += histoChi2*histo.Weight / histo.NumberOfEvents;
      NDF += histoNDF* histo.Weight / histo.NumberOfEvents;
      NLL += histoNLL* histo.Weight / histo.NumberOfEvents;
    
    }
  }
  if (fUseHistoWeights == true){
    
  Chi2 *= weightNorm;
  NDF *= weightNorm;
  NLL *= weightNorm;
  }
  //NDF -= fNpar;
    
  double FCN = (fFitType == 2) ? Chi2 : NLL;
  
  if(fVerboseLevel > 1)
    std::cout << "FCN = " << FCN << " (Chi2/NDF = " << Chi2/NDF << ")" << std::endl;

  if(!std::isfinite(FCN))
    FCN = std::numeric_limits<double>::max();

  fChi2 = Chi2/NDF;
  
  return FCN;
}

Double_t EXOEnergyMCBasedFit1D::GetPeakPosition(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitCalibFunction->GetX(energy);
}

Double_t EXOEnergyMCBasedFit1D::GetPeakPositionError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
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

Double_t EXOEnergyMCBasedFit1D::GetPeakWidth(Double_t energy, bool fitted)
{
  if(fitted)
    SetFitFunctionParameters(&GetFittedParameters()[0]);
  return fFitResolFunction->Eval(energy);
}

Double_t EXOEnergyMCBasedFit1D::GetPeakWidthError(Double_t energy, Int_t nDraws, TH1D *saveDraws)
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

double EXOEnergyMCBasedFit1D::GetEventContribution(double ene, double up, double e_low, double weight)
{    
  std::vector<Double_t> energy(1); energy[0] = ene;
  std::vector<Double_t> e_up(1); e_up[0] = up;
  std::vector<Double_t> pdf(1); pdf[0] = weight;
  std::vector<Double_t> result(1);

  if(fFitResolFunction)
    SmearedMCHistoResol(result,energy,pdf,e_up,e_low);
  return result.at(0)/2.;
}

bool EXOEnergyMCBasedFit1D::IgnoreBins(double min, double max)
{
  if(fIgnoreBounds)
    delete fIgnoreBounds;
  
  fIgnoreBounds = new std::pair<double,double>(min,max);
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
    histoEntry->FillFitBins1D(fIgnoreBounds);

  return true;    
}

void EXOEnergyMCBasedFit1D::FitOnlyPeaks()
{
  if(fIgnoreBounds)
    delete fIgnoreBounds;
  fIgnoreBounds = 0;
  for(std::vector<HistoEntry>::iterator histoEntry = fHistoList.begin(); histoEntry != fHistoList.end(); histoEntry++)
    histoEntry->SetPeakBins1D();
  return;
}
