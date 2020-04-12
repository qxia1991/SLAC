#ifndef EXOEnergyCalibFitMCBase_hh
#define EXOEnergyCalibFitMCBase_hh

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <string>
#include <algorithm>

#include "TROOT.h"
#include "TObject.h"
#include "TF1.h"
#include "TString.h"
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

class EXOEnergyCalibFitMCBase : public TObject
{
public:
  EXOEnergyCalibFitMCBase(Double_t *mcPoints = NULL, Double_t *mcWeights = NULL,Int_t nMC = 0);
  virtual ~EXOEnergyCalibFitMCBase();

  void SetVerboseLevel(Int_t level);
  void BinMCEnergy(Int_t gran = 1); // granularity = 1/gran keV
  void SaveHistosIn(const char* name, const char* option);

  const TH1* GetHisto(const TString type) const;
  const TFitter* GetFitter() const;
  const TMinuit* GetMinuit() const;
  
protected:
  //TMinuit *fMinuit;
  TFitter *fFitter;
  TMatrixDSym *fCov;
  TMatrixD *fChol;
  TRandom3 *fRandom;

  Int_t fVerboseLevel;
  Int_t fFitType; //1 = NLL, 2 = Chi2
  Int_t fNpar;

  std::vector<Double_t> fMCEnergyPoints;
  std::vector<Double_t> fMCEnergyPDF;
  
  TH1 *fDataHisto;
  TH1 *fSmearedMCHisto;
  TH1 *fScaleHisto;

  std::vector<TF1*> fFitFunctions;
  
  void Initialize();
  Int_t GetNumberOfFreeParameters();
  void DefineInitialParameters();
  void SetFitFunctionParameters(const double* x);
  std::vector<Double_t> GetFittedParameters();

  std::vector<Double_t> DrawGausFitPars();

  static void Fcn(int &, double *, double& f, double *, int);
  static ROOT::Math::IMultiGenFunction *fFCN;

  ClassDef(EXOEnergyCalibFitMCBase,1)
};

#endif
