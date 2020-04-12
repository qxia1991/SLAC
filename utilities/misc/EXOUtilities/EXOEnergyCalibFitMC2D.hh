#ifndef EXOEnergyCalibFitMC2D_hh
#define EXOEnergyCalibFitMC2D_hh

#include "EXOUtilities/EXOEnergyCalibFitMCBase.hh"

class EXOEnergyCalibFitMC2D : public EXOEnergyCalibFitMCBase
{
public:
  EXOEnergyCalibFitMC2D(Double_t *mcPoints = NULL, Double_t *mcWeights = NULL, Int_t nMC = 0, Double_t *ionizDataPoints = NULL, Double_t *scintDataPoints = NULL, Int_t nData = 0);
  virtual ~EXOEnergyCalibFitMC2D();

  void SetDataHisto(Int_t nIonizBins, Double_t lowIonizEnergy, Double_t upIonizEnergy, Int_t nScintBins, Double_t lowScintEnergy, Double_t upScintEnergy);
  void SetFunction(TString type, TString channel, TF1* func);

  bool ExecuteFit(Int_t fitType = 1); // NLL by default

  Double_t GetPeakPosition(Double_t energy, TString channel, bool fitted = true);
  Double_t GetPeakPositionError(Double_t energy, TString channel, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetPeakWidth(Double_t energy, TString channel, bool fitted = true);
  Double_t GetPeakWidthError(Double_t energy, TString channel, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetCorrelation(Double_t energy, bool fitted = true);
  Double_t GetCorrelationError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  

  
protected:
  TF1 *fFitIonizResolFunction;
  TF1 *fFitIonizCalibFunction;
  TF1 *fFitScintResolFunction;
  TF1 *fFitScintCalibFunction;
  TF1 *fFitCorrelationFunction;

  // 2D fit is currently performed using float precision, not double
  TH2F* fDataHisto2D;
  TH2F* fSmearedMCHisto2D;
  TH2F* fScaleHisto2D;
  
  std::vector<Double_t> fIonizDataEnergyPoints;
  std::vector<Double_t> fScintDataEnergyPoints;
  
  void FillSmearedMCHistogram(const double* x, TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf);
  void GetScaleHistogram(TH2F& hScale, const TH2F& hData, const TH2F& hMC);
  double FitFunction(const double* x);

  int fMinFastExp;
  std::vector<float> fVecFastExp;
  float FastExp(float x);
  float GaussTerm2(float xx0, float yy0, float corr);

  ClassDef(EXOEnergyCalibFitMC2D,1)
};

#endif
