#include "EXOCalibUtilities/EXOEnergyResol.hh"

std::set<EXOEnergyResol*> EXOEnergyResol::fInstances;

EXOEnergyResol::EXOEnergyResol(std::string resolFlavor, std::string dbTableName){ 
  fResolutionFlavor = resolFlavor;
  fDBTableName = dbTableName;
  if(dbTableName == "energy-resolution")
    fCalibrationType = "AverageRatioGausErfcFit";
  else if(dbTableName == "energy-mcbased-fit")
    fCalibrationType = "AverageMCBasedFit";
}

EXOEnergyResol::~EXOEnergyResol()
{
  fInstances.erase(this);
}

EXOEnergyResol* EXOEnergyResol::GetInstanceForFlavor(std::string resolFlavor, std::string dbTableName)
{
  for(std::set<EXOEnergyResol*>::iterator iter=fInstances.begin(); iter != fInstances.end(); iter++){
    if((*iter)->fResolutionFlavor != resolFlavor){
      continue;
    }
    if((*iter)->fDBTableName != dbTableName){
      continue;
    }
    
    return *iter;
  }
  EXOEnergyResol* newInstance = new EXOEnergyResol(resolFlavor,dbTableName);
  fInstances.insert(newInstance);
  return newInstance;
}

const char* EXOEnergyResol::GetCalibrationType() const
{
  return fCalibrationType.c_str();
}

const char* EXOEnergyResol::GetResolutionDBTableName() const
{
  return fDBTableName.c_str();
}

void EXOEnergyResol::SetUserValues(const std::string dbTable, const std::string channel, double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS)
{
  if(dbTable == "energy-resolution")
  {
    if(channel == "Ionization")
      EXOEnergyResCalib::SetUserValuesForCharge(p0SS,p1SS,p2SS,p0MS,p1MS,p2MS);
    else if(channel == "Scintillation")
      EXOEnergyResCalib::SetUserValuesForScint(p0SS,p1SS,p2SS,p0MS,p1MS,p2MS);
    else if(channel == "Rotated")
      EXOEnergyResCalib::SetUserValuesForRotated(p0SS,p1SS,p2SS,p0MS,p1MS,p2MS);
     else
      LogEXOMsg("Readout channel not found by EXOEnergyResol, use one of the following: Rotated, Ionization, Scintillation", EEError);
  }
  else if(dbTable == "energy-mcbased-fit")
  {
    std::map<std::string, double> userValuesSS;
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p0",channel.c_str()),p0SS));
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p1",channel.c_str()),p1SS));
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p2",channel.c_str()),p2SS));
    EXOEnergyMCBasedFit::SetUserValues(userValuesSS,1);
    std::map<std::string, double> userValuesMS;
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p0",channel.c_str()),p0MS));
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p1",channel.c_str()),p1MS));
    userValuesSS.insert(std::pair<std::string,double>(Form("%s_Resolution_p2",channel.c_str()),p2MS));
    EXOEnergyMCBasedFit::SetUserValues(userValuesMS,2);    
  }
  else
      LogEXOMsg("DB table name not found by EXOEnergyResol, use one of the following: energy-resolution , energy-mcbased-fit", EEError);
  
}


double EXOEnergyResol::Resolution(const std::string& channel, double energy, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return Resolution(channel, energy, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyResol::Resolution(const std::string& channel, double energy, int multiplicity, long int seconds, int nano) const
{
  return Resolution(channel, energy, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyResol::Resolution(const std::string& channel, double energy, int multiplicity, const EXOTimestamp& time) const
{
  double resol = 0.;
  if(fDBTableName == "energy-resolution")
  {
    EXOEnergyResCalib* dbResol = GetCalibrationFor(EXOEnergyResCalib,EXOEnergyResCalibHandler,fResolutionFlavor,time);
    if(channel == "Rotated")
      resol = dbResol->RotatedResolution(energy,multiplicity);
    else if(channel == "Ionization")
      resol = dbResol->ChargeResolution(energy,multiplicity);
    else if(channel == "Scintillation")
      resol = dbResol->ScintillationResolution(energy,multiplicity);
    else
      LogEXOMsg("Readout channel not found by EXOEnergyResol, use one of the following: Rotated, Ionization, Scintillation", EEError);
  }
  else if(fDBTableName == "energy-mcbased-fit")
  {
    EXOEnergyMCBasedFit* dbResol = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fResolutionFlavor,time);
    resol = MCBasedFitResolution(channel,energy,dbResol,multiplicity);
  }
  else
      LogEXOMsg("DB table name not found by EXOEnergyResol, use one of the following: energy-resolution , energy-mcbased-fit", EEError);
    
  return resol;
}

std::string EXOEnergyResol::ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return ResolutionString(channel, eLabel, multiplicity, seconds, 1000*microseconds);
}

std::string EXOEnergyResol::ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, long int seconds, int nano) const
{
  return ResolutionString(channel, eLabel, multiplicity, EXOTimestamp(seconds,nano));
}

std::string EXOEnergyResol::ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, const EXOTimestamp& time) const
{
  std::string resolString = "";
  if(fDBTableName == "energy-resolution")
  {
    EXOEnergyResCalib* dbResol = GetCalibrationFor(EXOEnergyResCalib,EXOEnergyResCalibHandler,fResolutionFlavor,time);
    if(channel == "Rotated")
      resolString = dbResol->RotatedResolutionString(eLabel,multiplicity);
    else if(channel == "Ionization")
      resolString = dbResol->ChargeResolutionString(eLabel,multiplicity);
    else if(channel == "Scintillation")
      resolString = dbResol->ScintillationResolutionString(eLabel,multiplicity);
    else
      LogEXOMsg("Readout channel not found by EXOEnergyResol, use one of the following: Rotated, Ionization, Scintillation", EEError);    
  }
  else if(fDBTableName == "energy-mcbased-fit")
  {
    EXOEnergyMCBasedFit* dbResol = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fResolutionFlavor,time);
    resolString = MCBasedFitResolutionString(channel,dbResol,multiplicity,eLabel);
  }
  else
      LogEXOMsg("DB table name not found by EXOEnergyResol, use one of the following: energy-resolution , energy-mcbased-fit", EEError);
  
  return resolString;
}

TH1D EXOEnergyResol::SmearedMC(const std::string& channel, double* energies, double* weights, int length, int multiplicity, int nBins, double xLow, double xUp, long int seconds, int nano, int binMC) const
{
  EXOEnergyMCBasedFit1D smearFitter;
  smearFitter.SetVerboseLevel(-1);
  smearFitter.AddMC("MC",0,0,0.,0.,0.,energies,weights,length);
  std::vector<double> fakeData(1,0.);
  smearFitter.AddData("Data","MC",0,0,0.,0.,0.,0,&fakeData[0],fakeData.size());
  smearFitter.BinMCEnergy(binMC);
  std::vector<TString> fitHistos;
  fitHistos.push_back("Data");
  smearFitter.SetDataHisto("FitHisto","",fitHistos,nBins,xLow,xUp);
  TF1 *calib = new TF1("calib","x",xLow,xUp);
  smearFitter.SetFunction("calib",calib);
  std::string resolString = ResolutionString(channel,"x",multiplicity,seconds,nano);
  TF1 *resol = new TF1("resol",resolString.c_str(),xLow,xUp);
  smearFitter.SetFunction("resol",resol);
  smearFitter.ApplyFittedCalibration(nBins,xLow,xUp);
  const TH1D *smearedMC = dynamic_cast<const TH1D*>(smearFitter.GetHisto("MC","MC",""));
  const TAxis *xAxis = smearedMC->GetXaxis();
  TH1D result("MC","",xAxis->GetNbins(),xAxis->GetXmin(),xAxis->GetXmax());
  for(int b = 0; b <= xAxis->GetNbins(); b++)
  {
    result.SetBinContent(b,smearedMC->GetBinContent(b));
    result.SetBinError(b,smearedMC->GetBinError(b));
  }

  if(calib)
    delete calib;
  if(resol)
    delete resol;  
  
  return result;
}

bool EXOEnergyResol::GetEventWeights(const std::string& channel, double* energies, double* weights, int length, int multiplicity, double xLow, double xUp, long int seconds, int nano) const
{
  EXOEnergyMCBasedFit1D smearFitter;
  smearFitter.SetVerboseLevel(-1);
  std::string resolString = ResolutionString(channel,"x",multiplicity,seconds,nano);
  TF1 *resol = new TF1("resol",resolString.c_str(),xLow,xUp);
  smearFitter.SetFunction("resol",resol);

  //double *result = new double[length];
  for(int i = 0; i < length; i++)
    weights[i] = smearFitter.GetEventContribution(energies[i],xUp,xLow,weights[i]);

  return true;  
}

bool EXOEnergyResol::FillSmearedMCHisto1D(TH1D& histo, const std::string& channel, double* energies, double* weights, int length, int multiplicity, long int seconds, int nano, int binMC) const
{
  // no binMC support yet...
  EXOEnergyMCBasedFit1D smearFitter;
  smearFitter.SetVerboseLevel(-1);

  std::vector<double> energy(energies, energies + length);
  std::vector<double> weight(weights, weights + length);

  double minE = *std::min_element(energy.begin(),energy.end());
  double maxE = *std::max_element(energy.begin(),energy.end());
  int intMaxE = static_cast<int> (ceil(maxE));

  if(binMC > 0)
  {
    TH1D temp("temp","",intMaxE*binMC,0,intMaxE);
    temp.FillN(energy.size(),&energy[0],&weight[0]);
    energy.clear();
    weight.clear();
    for(int b = 1; b <= temp.GetNbinsX(); b++)
    {
      double content = temp.GetBinContent(b);
      if(content > 0)
      {
        energy.push_back(temp.GetBinCenter(b));
        weight.push_back(content);
      }
    }
  }

  TF1 *calib = new TF1("calib","x",minE*0.99,maxE*1.01);
  smearFitter.SetFunction("calib",calib);
  std::string resolString = ResolutionString(channel,"x",multiplicity,seconds,nano);
  TF1 *resol = new TF1("resol",resolString.c_str(),minE*0.99,maxE*1.01);
  smearFitter.SetFunction("resol",resol);


  bool result = smearFitter.FillSmearedMCHistogram(0,histo,energy,weight);
  
  if(calib)
    delete calib;
  if(resol)
    delete resol;  

  return result;
}

double EXOEnergyResol::MCBasedFitResolution(const std::string& channel, double energy, EXOEnergyMCBasedFit* dbTable, int multiplicity) const
{
  TFormula resolForm("resolFormula",MCBasedFitResolutionString(channel,dbTable,multiplicity,"x").c_str());
  return resolForm.Eval(energy);
}

std::string EXOEnergyResol::MCBasedFitResolutionString(const std::string& channel, EXOEnergyMCBasedFit* dbTable, int multiplicity, std::string eLabel) const
{
  int nPar = 3;
  std::vector<double> resolP(nPar);
  for(int i = 0; i < nPar; i++)
  {
    resolP[i] = dbTable->GetParByName(Form("%s_Resolution_p%d",channel.c_str(),i),multiplicity);
    resolP[i] *= resolP[i];
  }

  return Form("sqrt(%f * %s + %f + %f * %s * %s)",resolP.at(0),eLabel.c_str(),resolP.at(1),resolP.at(2),eLabel.c_str(),eLabel.c_str());
}
