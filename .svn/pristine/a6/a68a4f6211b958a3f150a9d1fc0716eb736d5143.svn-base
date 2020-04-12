#ifndef EXORefitAPDs_hh
#define EXORefitAPDs_hh

#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "TStopwatch.h"
#include <string>
#include <vector>
#include <map>

class EXOTalkToManager;
class EXOEventData;
class EXOWaveformFT;
class TFile;
class TH3D;
class TGraph;

class EXORefitAPDs : public EXOAnalysisModule
{
 public:
  EXORefitAPDs() : fLightmapFile(NULL),
                   fRThreshold(0.1),
                   fThoriumEnergy_keV(2615),
                   fNumEntriesSolved(0),
                   fTotalNumberOfIterationsDone(0) {}
  int TalkTo(EXOTalkToManager *tm);
  int Initialize();
  EXOAnalysisModule::EventStatus ProcessEvent(EXOEventData *ED);
  int ShutDown();

  void SetNoiseFilename(std::string name) { fNoiseFilename = name; }
  void SetLightmapFilename(std::string name) { fLightmapFilename = name; }
  void SetRThreshold(double threshold) { fRThreshold = threshold; }

 protected:
  std::string fNoiseFilename;

  std::map<std::pair<unsigned char, unsigned char>, std::vector<double> > fNoiseRRandII; // symmetric
  std::map<std::pair<unsigned char, unsigned char>, std::vector<double> > fNoiseRI; // asymmetric

  std::string fLightmapFilename;
  TFile* fLightmapFile;
  std::vector<unsigned char> fAPDs;
  std::map<unsigned char, TH3D*> fLightMaps;
  std::map<unsigned char, TGraph*> fGainMaps;

  double GetGain(unsigned char channel) const;

  double fRThreshold;

  // Various stopwatches, to understand the fraction of time spent actually solving the matrix.
  TStopwatch fWatch_ProcessEvent;
  TStopwatch fWatch_Solve;
  mutable TStopwatch fWatch_MatrixMul;
  mutable TStopwatch fWatch_MatrixMul_NoiseTerms;

  std::vector<unsigned char> fChannelsToUse;
  std::map<unsigned char, double> fExpectedYieldPerGang;
  std::vector<double> fmodel_realimag;
  double fUnixTimeOfEvent;
  double fExpectedEnergy_keV;
  const double fThoriumEnergy_keV;

  // Collect statistics on the number of iterations required.
  unsigned long int fNumEntriesSolved;
  unsigned long int fTotalNumberOfIterationsDone;

  struct BiCGSTAB_iter {
    // Holds the output from a particular BiCGSTAB iteration.
    // Follows notation of the original H.A. VAN DER VORST paper.
    double alpha;
    double rho;
    double omega;
    std::vector<double> x;
    std::vector<double> r;
    std::vector<double> p;
    std::vector<double> v;
  };
  BiCGSTAB_iter BiCGSTAB_iteration(const BiCGSTAB_iter& in,
                                   const std::vector<double>& r0hat) const;

  std::vector<double> MatrixTimesVector(const std::vector<double>& in) const;

  EXOWaveformFT GetModelForTime(double time) const;

 DEFINE_EXO_ANALYSIS_MODULE(EXORefitAPDs)
};
#endif
