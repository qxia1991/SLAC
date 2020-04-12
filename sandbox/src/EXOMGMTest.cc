#include "EXOMGMTest.hh" 
#include <iostream>
#ifdef HAVE_ROOT_ROOFIT
#include "RooRealVar.h"
#include "RooGaussian.h"
#endif

int EXOMGMTest::Initialize() { std::cout << "Hello" << std::endl; return 0; }
EXOAnalysisModule::EventStatus EXOMGMTest::BeginOfRun(EXOEventData *ED) {

#ifdef HAVE_ROOT_ROOFIT
  RooRealVar mean("mean", "mean", 0.2);
  RooRealVar sigma("sigma", "sigma", 0.5);
  RooRealVar energy("energy", "energy", 0, 3);

  RooGaussian gauss("gauss", "gauss", energy, mean, sigma);
  gauss.Print();
#endif
  
  std::cout << "Plugin Initialize" << std::endl; return kOk; 

}
