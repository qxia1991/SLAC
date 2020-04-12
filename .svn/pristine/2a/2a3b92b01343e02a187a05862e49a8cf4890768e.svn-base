#ifndef EXOEnergyCalibFitMC1D_hh
#define EXOEnergyCalibFitMC1D_hh

#include "EXOUtilities/EXOEnergyCalibFitMCBase.hh"

class EXOEnergyCalibFitMC1D : public EXOEnergyCalibFitMCBase
{
public:
  EXOEnergyCalibFitMC1D(Double_t *mcPoints = NULL, Double_t *mcWeights = NULL, Int_t nMC = 0, Double_t *dataPoints = NULL, Int_t nData = 0);
  virtual ~EXOEnergyCalibFitMC1D();

  void SetDataHisto(Int_t nBins, Double_t lowEnergy, Double_t upEnergy);
  void SetFunction(TString type, TF1* func);

  bool ExecuteFit(Int_t fitType = 1); // NLL by default

  Double_t GetPeakPosition(Double_t energy, bool fitted = true);
  Double_t GetPeakPositionError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetPeakWidth(Double_t energy, bool fitted = true);
  Double_t GetPeakWidthError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);

protected:
  TF1 *fFitResolFunction;
  TF1 *fFitCalibFunction;

  TH1D* fDataHisto1D;
  TH1D* fSmearedMCHisto1D;
  TH1D* fScaleHisto1D;
  std::vector<Double_t> fDataEnergyPoints;
  
  void FillSmearedMCHistogram(const double* x, TH1D& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf);
  void GetTriggerScale(TH1D& hScale, const TH1D& hData, const TH1D& hMC, double eCut = 0, double eResolution = 0, int prescale = 0, bool hasAPDthres = false);
  double FitFunction(const double* x);

  
  ClassDef(EXOEnergyCalibFitMC1D,1)
};

#endif

