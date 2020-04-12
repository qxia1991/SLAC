#ifndef EXOEnergyMCBasedFit1D_hh
#define EXOEnergyMCBasedFit1D_hh

#include "EXOUtilities/EXOEnergyMCBasedFitBase.hh"

class EXOEnergyMCBasedFit1D : public EXOEnergyMCBasedFitBase
{
public:
  EXOEnergyMCBasedFit1D();
  virtual ~EXOEnergyMCBasedFit1D();

  bool AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataPoints, Int_t nData, Int_t *runs = NULL, Int_t nRuns = 0, Int_t week = 0);
  bool SetDataHisto(const char* histoName, std::string histoTitle, std::vector<TString>& dataIds, Int_t nBins, Double_t lowEnergy, Double_t upEnergy, double weight = 1., bool forceNewMC = false);  
  void SetFunction(TString type, TF1* func);
  TF1 *GetFunction(TString type);

  bool BuildFitter();
  bool RunFitter(const char* minimizer = "MINIMIZE", Float_t fitPrecision = 1.);
  bool ExecuteFit(Int_t fitType = 1, const char* minimizer = "MINIMIZE", Float_t fitPrecision = 1.); // NLL by default
  bool fUseHistoWeights;

  Double_t GetPeakPosition(Double_t energy, bool fitted = true);
  Double_t GetPeakPositionError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetPeakWidth(Double_t energy, bool fitted = true);
  Double_t GetPeakWidthError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);

  double GetChi2() {return fChi2;}

  void ApplyFittedCalibration(Int_t nBins, Double_t lowEnergy, Double_t upEnergy, bool useFitResults = true);

  double GetEventContribution(double ene, double up, double e_low, double weight = 1);

  bool FillSmearedMCHistogram(const double* x, TH1D& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf);
  bool IgnoreBins(double min, double max);  
  void FitOnlyPeaks();
  
protected:
  TF1 *fFitResolFunction;
  TF1 *fFitCalibFunction;
  TF1 *fFitTrigEffFunction;

  double fChi2;
  std::pair<double,double>* fIgnoreBounds; // ignore bins within given bounds in fit
  
  virtual bool SmearedMCHistoCalib(std::vector<double>& e_up, double& elow, const TH1D& histo);
  virtual bool SmearedMCHistoResol(std::vector<double>& weight, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf, const std::vector<double>& e_up, double elow);  
  void GetScaleHistogram(TH1D& hScale, const TH1D& hData, const TH1D& hMC, bool usePrescaleModel = false);
  double FitFunction(const double* x);

  
  ClassDef(EXOEnergyMCBasedFit1D,2)
};

#endif

