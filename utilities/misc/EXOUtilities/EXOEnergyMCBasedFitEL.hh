#ifndef EXOEnergyMCBasedFitEL_hh
#define EXOEnergyMCBasedFitEL_hh

#include "EXOUtilities/EXOEnergyMCBasedFitBase.hh"
#include "EXOUtilities/EXOEnergyMCBasedFit1D.hh"

class EXOEnergyMCBasedFitEL : public EXOEnergyMCBasedFit1D
{
public:
  EXOEnergyMCBasedFitEL();
  virtual ~EXOEnergyMCBasedFitEL();

  bool AddMC(const char* id, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Double_t *mcPoints, Double_t *mcWeights,  Double_t *mcZpos, Int_t nMC);
  bool AddData(const char* dataId, const char* mcId, Int_t sourceAtomicNumber, Int_t sourceMassNumber, Float_t sourceX, Float_t sourceY, Float_t sourceZ, Int_t prescale, Double_t *dataPoints, Double_t *dataZpos, Int_t nData, Int_t *runs = NULL, Int_t nRuns = 0, Int_t week = 0);

  bool SplitZbins(Double_t *zBins, Int_t nBins);

  bool SetDataHisto(const char* histoName, std::string histoTitle, std::vector<TString>& dataIds, Int_t nBins, Double_t lowEnergy, Double_t upEnergy, double weight = 1.);  

  Int_t GetFitType();
  
protected:

  bool SmearedMCHistoCalib(std::vector<double>& e_up, double& elow, const TH1D& histo);

  bool fHasSplittedZbins;
  std::vector<Double_t> fZbins; // low edges
  std::vector<Double_t> fZbinCenter;
  
  ClassDef(EXOEnergyMCBasedFitEL,1)
};

#endif

