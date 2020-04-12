#ifndef EXOEnergyMCBasedFitBase_hh
#define EXOEnergyMCBasedFitBase_hh

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <string>
#include <algorithm>
#include <limits>

#include "TROOT.h"
#include "TObject.h"
#include "TF1.h"
#include "TF2.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TDecompChol.h"
#include "TRandom3.h"

#include "TFitter.h"
#include "Math/Functor.h"
#include "TMinuit.h"

typedef struct MCEntry {
  Int_t SourceAtomicNumber, SourceMassNumber;
  Float_t SourceX, SourceY, SourceZ;
  std::map<std::string, std::vector<Double_t> > MCOtherInfo;
  std::vector<Double_t> MCEnergyPoints;
  std::vector<std::pair<Double_t,Double_t> > MCEnergyPoints2D; // energy_mc: charge collection, Sum$(pcd_energy): scintillation
  std::vector<Double_t> MCEnergyPDF;
  TH1* MCSmearedHisto;
  Bool_t Is2D;

  void InitializeMembers(Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ){
    SourceAtomicNumber = sourceAtomicNumber;
    SourceMassNumber = sourceMassNumber;
    SourceX = sourceX;
    SourceY = sourceY;
    SourceZ = sourceZ;
    MCOtherInfo.clear();
    MCEnergyPoints.clear();
    MCEnergyPoints2D.clear();
    MCEnergyPDF.clear();
    MCSmearedHisto = 0;
    Is2D = false;
  }

  void SetEnergy(Double_t *mcPoints, Double_t *mcWeights, Int_t nMC){
    for(Int_t i = 0; i < nMC; i++)
    {
      MCEnergyPoints.push_back(mcPoints[i]);
      MCEnergyPDF.push_back(mcWeights[i]);
      Is2D = false;
    }
  }

  void SetEnergy2D(Double_t *mcPointsX, Double_t *mcPointsY, Double_t *mcWeights, Int_t nMC){
    for(Int_t i = 0; i < nMC; i++)
    {
      MCEnergyPoints2D.push_back(std::make_pair(mcPointsX[i],mcPointsY[i]));
      MCEnergyPDF.push_back(mcWeights[i]);
      Is2D = true;
    }
  }

  void SetOtherInfo(std::string infoName, Double_t *mcInfo, Int_t nMC){
    MCOtherInfo[infoName].clear();
    for(Int_t i = 0; i < nMC; i++)
    {
      MCOtherInfo.at(infoName).push_back(mcInfo[i]);
    }
  }
  
} MCEntry;

typedef struct DataEntry {
  TString MCId;
  Int_t SourceAtomicNumber, SourceMassNumber;
  Float_t SourceX, SourceY, SourceZ;
  Bool_t UsePrescaleTriggerEff;
  std::map<TString, std::vector<Double_t> > DataEnergyPoints;
  std::map<std::string, std::vector<Double_t> > DataOtherInfo;
  TH1* DataHisto;
  TH1* ScaleHisto;
  TH1* MCSmearedHisto;
  std::vector<Int_t> Runs;
  Int_t Week;

  void InitializeMembers(const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Int_t *runs, Int_t nRuns, Int_t week){
    MCId = mcId;
    SourceAtomicNumber = sourceAtomicNumber;
    SourceMassNumber = sourceMassNumber;
    SourceX = sourceX;
    SourceY = sourceY;
    SourceZ = sourceZ;
    UsePrescaleTriggerEff = false;
    if(prescale >= 1 && (SourceAtomicNumber != 55 && SourceMassNumber != 137))
      UsePrescaleTriggerEff = true;
    DataEnergyPoints.clear();
    DataOtherInfo.clear();
    for(Int_t i = 0; i < nRuns; i++)
      Runs.push_back(runs[i]);
    Week = week;
    DataHisto = 0;
    ScaleHisto = 0;
    MCSmearedHisto = 0;
  }

  void SetEnergy(TString name, Double_t *dataPoints, Int_t nData){
    std::vector<Double_t> energy;
    for(Int_t i = 0; i < nData; i++)
      energy.push_back(dataPoints[i]);
    DataEnergyPoints[name] = energy;
  }

  void SetOtherInfo(std::string infoName, Double_t *dataInfo, Int_t nData){
    std::vector<Double_t> infoPoints;
    for(Int_t i = 0; i < nData; i++)
      infoPoints.push_back(dataInfo[i]);
    DataOtherInfo[infoName] = infoPoints;
  }
    
} DataEntry;

typedef struct HistoEntry {

  TString HistoName;
  std::vector<TString> DataIds;
  bool UsePrescaleTriggerEff;
  Double_t Weight;
  Double_t NumberOfEvents;
  TH1* MCSmearedHisto;
  TH1* DataHisto;
  std::set<Int_t> FitBins;

  HistoEntry(){
    HistoName = "";
    DataIds.clear();
    UsePrescaleTriggerEff = false;
    Weight = 1.;
    NumberOfEvents = 0;
    MCSmearedHisto = 0;
    DataHisto = 0;
  }

  void FillFitBins1D(std::pair<double,double>* bounds){
    FitBins.clear();
    TH1D* dataHisto1D = dynamic_cast<TH1D*>(DataHisto);
    for(int b = 1; b <= dataHisto1D->GetNbinsX(); b++){
      if(not bounds)
        FitBins.insert(b);
      else{
        double binMin = dataHisto1D->GetBinLowEdge(b);
        double binMax = binMin + dataHisto1D->GetBinWidth(b);
        if(binMax < bounds->first || binMin > bounds->second)
          FitBins.insert(b);
        else
          std::cout << "will not use bin b = " << b << " or " << dataHisto1D->GetBinCenter(b) << std::endl;
      }
    }
  }

  void SetPeakBins1D() {
    std::cout << "Fitting only the hard coded peaks.\n";
    FitBins.clear();
    std::vector<std::pair<double,double> > peakBounds;
    peakBounds.push_back(std::make_pair(1150.,1190.));//peakBounds.push_back(std::make_pair(1130.,1200.));
    peakBounds.push_back(std::make_pair(1310.,1400.));//peakBounds.push_back(std::make_pair(1300.,1600.));
    //peakBounds.push_back(std::make_pair(1580.,1620.));
    peakBounds.push_back(std::make_pair(1720.,1780.));
    peakBounds.push_back(std::make_pair(2590.,2650.));//peakBounds.push_back(std::make_pair(2580.,3500.));
    
    TH1D* dataHisto1D = dynamic_cast<TH1D*>(DataHisto);
    for(int b = 1; b <= dataHisto1D->GetNbinsX(); b++){
      double binCenter = dataHisto1D->GetBinCenter(b);
      for(std::vector<std::pair<double,double> >::iterator bounds = peakBounds.begin(); bounds != peakBounds.end(); bounds++){
        //std::cout << "comparing " << binCenter << " w " << bounds->first << " a " << bounds->second << std::endl;
        if(bounds->first < binCenter && binCenter < bounds->second)
          FitBins.insert(b);
        //else
        //  std::cout << "will not use bin b = " << b << " or " << binCenter << std::endl;
      }
    }
  }

} HistoEntry;
  
class EXOEnergyMCBasedFitBase : public TObject
{
public:
  EXOEnergyMCBasedFitBase();
  virtual ~EXOEnergyMCBasedFitBase();

  void SetVerboseLevel(Int_t level);

  bool AddMC(const char* id, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPoints, Double_t *mcWeights, Int_t nMC);

  void BinMCEnergy(Int_t gran = 1, std::string mcId = "all", Double_t cutoff = 0); // granularity = 1/gran keV
  virtual bool BuildFitter();
  bool ApplyFittedParameters();
  void SaveHistosIn(const char* name, const char* option, int rebinX = 1, int rebinY = 1);

  bool ShouldSetTriggerEffFunction();

  const TH1* GetHisto(const TString list, const TString idName, const TString type) const;
  const TFitter* GetFitter() const;
  const TMinuit* GetMinuit() const;

  // only use this function if you know what you are doing!
  void SetAllFitCalculations(bool onoff){fAllFitCalculations = onoff;}; 
  
protected:
  Int_t fDimension;
  bool fAllFitCalculations;
  
  TFitter *fFitter;
  TMatrixDSym *fCov;
  TMatrixD *fChol;
  TRandom3 *fRandom;

  Int_t fVerboseLevel;
  Int_t fFitType; //1 = NLL, 2 = Chi2
  Int_t fNpar;

  std::map<TString, MCEntry> fMCList;
  std::map<TString, DataEntry> fDataList;
  std::vector<HistoEntry> fHistoList;

  std::vector<TF1*> fFitFunctions;
  
  void Initialize();
  Int_t GetNumberOfFreeParameters();
  void DefineInitialParameters();
  bool SetFitFunctionParameters(const double* x);
  std::vector<Double_t> GetFittedParameters();
  virtual double FitFunction(const double* x);
  virtual double GetErrorDef(double delta);
  void ApplyCalibration(std::vector<Double_t>& energies, TF1* calibFunction);


  std::vector<Double_t> DrawGausFitPars();

  ROOT::Math::Functor *fFitFunction;
  static void Fcn(int &, double *, double& f, double *, int);
  static ROOT::Math::IMultiGenFunction *fFCN;

  ClassDef(EXOEnergyMCBasedFitBase,2)
};

#endif
