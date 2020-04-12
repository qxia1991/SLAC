#include "EXOUtilities/EXOEnergyMCBasedFitEL.hh"

ClassImp(EXOEnergyMCBasedFitEL)

EXOEnergyMCBasedFitEL::~EXOEnergyMCBasedFitEL()
{
}

EXOEnergyMCBasedFitEL::EXOEnergyMCBasedFitEL():EXOEnergyMCBasedFit1D()
{
  fHasSplittedZbins = false;
  fZbins.clear();
}

bool EXOEnergyMCBasedFitEL::SmearedMCHistoCalib(std::vector<double>& e_up, double& elow, const TH1D& histo)
{
  std::string zTitle = histo.GetTitle();
  size_t pos = zTitle.find("Z = ");
  std::string zVal = zTitle.substr(pos+4);
  float z = atof(zVal.c_str());
  //std::cout << "In title " << zTitle << " using z = " << z << std::endl;
  for(int b = 1; b <= histo.GetNbinsX(); b++){
    e_up.push_back(fFitCalibFunction->Eval(histo.GetBinLowEdge(b) + histo.GetBinWidth(b),z));
  }
  elow = fFitCalibFunction->Eval(histo.GetBinLowEdge(1),z);


  return true;
}

bool EXOEnergyMCBasedFitEL::AddMC(const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPoints, Double_t *mcWeights, Double_t *mcZpos, Int_t nMC)
{
  if(fHasSplittedZbins)
  {
    std::cout << "Z bins have already been defined, cannot add more MC!!!\n";
    return false;
  }

  if(!EXOEnergyMCBasedFitBase::AddMC(mcId, sourceAtomicNumber, sourceMassNumber, sourceX, sourceY, sourceZ, mcPoints, mcWeights,nMC))
    return false;

  fMCList.at(mcId).SetOtherInfo("MCZPosition",mcZpos,nMC);

  return true;  
}

bool EXOEnergyMCBasedFitEL::AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataPoints, Double_t *dataZpos, Int_t nData, Int_t *runs, Int_t nRuns, Int_t week)
{
  if(fHasSplittedZbins)
  {
    std::cout << "Z bins have already been defined, cannot add more data!!!\n";
    return false;
  }

  //for(int i = 0; i < nData; i++)
  //  dataPoints[i] = dataPoints[i]*(exp(-(115458 - 584.94*abs(dataZpos[i]))/1000000.));
  if(!EXOEnergyMCBasedFit1D::AddData(dataId,mcId,sourceAtomicNumber,sourceMassNumber,sourceX,sourceY,sourceZ,prescale,dataPoints,nData,runs,nRuns,week))
    return false;

  fDataList.at(dataId).SetOtherInfo("DataZPosition",dataZpos,nData);

  return true;     
}

bool EXOEnergyMCBasedFitEL::SplitZbins(Double_t *zBins, Int_t nBins)
{
  if(fHasSplittedZbins)
  {
    std::cout << "Z bins have already been defined, cannot reset Z bins!!!\n";
    return false;
  }
  
  fZbins.resize(nBins+1);
  fZbinCenter.resize(nBins+2);
  for(Int_t b = 0; b <= nBins; b++)
  {
    fZbins[b] = zBins[b];
    if(b < nBins)
      fZbinCenter[b+1] = (zBins[b] + zBins[b+1])/2;
  }

  std::sort(fZbins.begin(),fZbins.end());
  
  std::vector<TString> mcIds;
    
  for(std::map<TString, MCEntry>::iterator mcMap = fMCList.begin(); mcMap != fMCList.end(); mcMap++)
    mcIds.push_back(mcMap->first);

  for(std::vector<TString>::iterator mcId = mcIds.begin(); mcId != mcIds.end(); mcId++)
  {
    MCEntry& mcEntry = fMCList.at(*mcId);

    std::vector<Double_t>& mcZpos = mcEntry.MCOtherInfo.at("MCZPosition");

    std::vector<std::vector<Double_t> > MCEnergyPoints(nBins+2);
    std::vector<std::vector<Double_t> > MCEnergyPDF(nBins+2);    
    
    for(size_t i = 0; i < mcZpos.size(); i++)
    {
      Double_t zPos = mcZpos.at(i);
      Int_t zBin = std::upper_bound(fZbins.begin(),fZbins.end(),zPos) - fZbins.begin();
      MCEnergyPoints.at(zBin).push_back(mcEntry.MCEnergyPoints.at(i));
      MCEnergyPDF.at(zBin).push_back(mcEntry.MCEnergyPDF.at(i));
    }

    for(Int_t b = 0; b <= nBins+1; b++)
    {
      if(fVerboseLevel > 0)
        std::cout << "Split with " << MCEnergyPoints.at(b).size() << " event in bin " << b << std::endl;
      if(MCEnergyPoints.at(b).empty())
        continue;
      
      if(!EXOEnergyMCBasedFitBase::AddMC(Form("%s_Z%d",mcId->Data(),b),mcEntry.SourceAtomicNumber, mcEntry.SourceMassNumber, mcEntry.SourceX, mcEntry.SourceY, mcEntry.SourceZ, &MCEnergyPoints.at(b)[0], &MCEnergyPDF.at(b)[0], MCEnergyPoints.at(b).size()))
        return false;
    }

    fMCList.erase(*mcId);
  }

  std::vector<TString> dataIds;
    
  for(std::map<TString, DataEntry>::iterator dataMap = fDataList.begin(); dataMap != fDataList.end(); dataMap++)
    dataIds.push_back(dataMap->first);

  for(std::vector<TString>::iterator dataId = dataIds.begin(); dataId != dataIds.end(); dataId++)
  {
    DataEntry& dataEntry = fDataList.at(*dataId);

    std::vector<Double_t>& dataZpos = dataEntry.DataOtherInfo.at("DataZPosition");

    std::vector<std::vector<Double_t> > DataEnergyPoints(nBins+2); 
    
    for(size_t i = 0; i < dataZpos.size(); i++)
    {
      Double_t zPos = dataZpos.at(i);
      Int_t zBin = std::upper_bound(fZbins.begin(),fZbins.end(),zPos) - fZbins.begin();
      DataEnergyPoints.at(zBin).push_back(dataEntry.DataEnergyPoints.at("1d").at(i));
    }

    for(Int_t b = 0; b <= nBins+1; b++)
    {
      if(fVerboseLevel > 0)
        std::cout << "Split with " << DataEnergyPoints.at(b).size() << " event in bin " << b << std::endl;
      if(DataEnergyPoints.at(b).empty())
        continue;
      if(fMCList.count(Form("%s_Z%d",dataEntry.MCId.Data(),b)) < 1)
        continue;
      
      if(!EXOEnergyMCBasedFit1D::AddData(Form("%s_Z%d",dataId->Data(),b),Form("%s_Z%d",dataEntry.MCId.Data(),b),dataEntry.SourceAtomicNumber,dataEntry.SourceMassNumber,dataEntry.SourceX,dataEntry.SourceY,dataEntry.SourceZ,(int) dataEntry.UsePrescaleTriggerEff,&DataEnergyPoints.at(b)[0],DataEnergyPoints.at(b).size(),&dataEntry.Runs[0],dataEntry.Runs.size(),dataEntry.Week))
        return false;
    }

    fDataList.erase(*dataId);
  }  

  fHasSplittedZbins = true;
  return true;
}

bool EXOEnergyMCBasedFitEL::SetDataHisto(const char* histoName, std::string histoTitle, std::vector<TString>& dataIds, Int_t nBins, Double_t lowEnergy, Double_t upEnergy, double weight)
{
  if(!fHasSplittedZbins)
  {
    std::cout << "Please, split Z bins before setting the histograms for fitting!\n";
    return false;
  }
  
  if(!histoTitle.empty())
    histoTitle += " |";
  
  for(Int_t b = 1; b <= static_cast<Int_t>(fZbins.size()); b++)
  {
    std::vector<TString> zDataIds;
    for(std::vector<TString>::iterator dataId = dataIds.begin(); dataId != dataIds.end(); dataId++)
      zDataIds.push_back(Form("%s_Z%d",dataId->Data(),b));

    if(!EXOEnergyMCBasedFit1D::SetDataHisto(Form("%s_Z%d",histoName,b),Form("%s Z = %.1f",histoTitle.c_str(),fZbinCenter.at(b)),zDataIds,nBins,lowEnergy,upEnergy,weight))
      return false;      
  }

  return true;
}

Int_t EXOEnergyMCBasedFitEL::GetFitType()
{

  return fFitType;
  
}
