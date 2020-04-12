#ifndef EXOEnergyMCBasedFit2D_hh
#define EXOEnergyMCBasedFit2D_hh

#include "EXOUtilities/EXOEnergyMCBasedFitBase.hh"

class EXOEnergyMCBasedFit2D : public EXOEnergyMCBasedFitBase
{
public:
  EXOEnergyMCBasedFit2D();
  virtual ~EXOEnergyMCBasedFit2D();

  bool AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataIonizPoints, Double_t *dataScintPoints, Int_t nData, Int_t *runs = NULL, Int_t nRuns = 0, Int_t week = 0);
  bool AddMC2D(const char* id, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPointsX, Double_t *mcPointsY, Double_t *mcWeights, Int_t nMC);
  bool SetDataHisto(const char* histoName, std::vector<TString>& dataIds, Int_t nIonizBins, Double_t lowIonizEnergy, Double_t upIonizEnergy, Int_t nScintBins, Double_t lowScintEnergy, Double_t upScintEnergy);
  void SetFunction(TString type, TString channel, TF1* func);

  bool BuildFitter();
  bool RunFitter(const char* minimizer = "MINIMIZE", Float_t fitPrecision = 1.);
  bool ExecuteFit(Int_t fitType = 1, const char* minimizer = "MINIMIZE", Float_t fitPrecision = 1., Int_t minEventBin = 10); // NLL by default
  //bool ExecuteFit(Int_t fitType = 1); // NLL by default
  void SetOffset(double offset = 0.);
  void SetMaxOffset(double maxOffset = 0.001,bool activate = true,bool stopaccov = true);
  void SetMaxIterations(int maxIter= 1000);

  Double_t GetPeakPosition(Double_t energy, TString channel, bool fitted = true);
  Double_t GetPeakPositionError(Double_t energy, TString channel, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetPeakWidth(Double_t energy, TString channel, bool fitted = true);
  Double_t GetPeakWidthError(Double_t energy, TString channel, Int_t nDraws = 1000, TH1D *saveDraws = NULL);
  Double_t GetCorrelation(Double_t energy, bool fitted = true);
  Double_t GetCorrelationError(Double_t energy, Int_t nDraws = 1000, TH1D *saveDraws = NULL);

  TH2F *GetSmearedMCHistogram();

protected:
  TF1 *fFitIonizResolFunction;
  TF1 *fFitIonizCalibFunction;
  TF1 *fFitScintResolFunction;
  TF1 *fFitScintCalibFunction;
  TF1 *fFitCorrelationFunction;
  
  void FillSmearedMCHistogram(const double* x, TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf);
  void FillSmearedMCHistogram(TH2F& histo, const std::vector<Double_t>& energy, const std::vector<Double_t>& pdf);
  void FillSmearedMC2DHistogram(const double* x, TH2F& histo, const std::vector<std::pair<Double_t,Double_t> >& energy, const std::vector<Double_t>& pdf);
  void FillSmearedMC2DHistogram(TH2F& histo, const std::vector<std::pair<Double_t,Double_t> >& energy, const std::vector<Double_t>& pdf);
  void GetScaleHistogram(TH2F& hScale, const TH2F& hData, const TH2F& hMC);
  double FitFunction(const double* x);

  int fMinEventBin;
  int fMinFastExp;
  std::vector<float> fVecFastExp;
  float FastExp(float x);
  float GaussTerm2(float xx0, float yy0, float corr);
  double fOffset;
  double fMaxOffset;
  bool fMaxOffsetActivated;
  bool fMaxOffsetStopSuccessCov;
  int fMaxIterations;

  ClassDef(EXOEnergyMCBasedFit2D,2)
};

#endif
