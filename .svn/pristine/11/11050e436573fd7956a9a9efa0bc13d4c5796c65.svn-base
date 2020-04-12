#ifndef EXOEnergyResol_hh
#define EXOEnergyResol_hh

#include <cmath>
#include <iostream>
#include <set>

#include "TH1D.h"
#include "TFormula.h"

#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOEnergyResCalib.hh"
#include "EXOCalibUtilities/EXOEnergyMCBasedFit.hh"

#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTimestamp.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOEnergyMCBasedFit1D.hh"

class EXOEnergyResol
{
public:
  ~EXOEnergyResol();
  static EXOEnergyResol* GetInstanceForFlavor(std::string resolFlavor, std::string dbTableName);

  const char* GetCalibrationType() const;
  const char* GetResolutionDBTableName() const;

  double Resolution(const std::string& channel, double energy, int multiplicity, const EXOEventHeader& header) const;
  double Resolution(const std::string& channel, double energy, int multiplicity, long int seconds, int nano) const;
  double Resolution(const std::string& channel, double energy, int multiplicity, const EXOTimestamp& time) const;
  std::string ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, const EXOEventHeader& header) const;
  std::string ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, long int seconds, int nano) const;
  std::string ResolutionString(const std::string& channel, const std::string& eLabel, int multiplicity, const EXOTimestamp& time) const;

  TH1D SmearedMC(const std::string& channel,double* energies, double* weights, int length, int multiplicity, int nBins, double xLow, double xUp, long int seconds, int nano, int binMC = 1) const;
  bool GetEventWeights(const std::string& channel, double* energies, double* weights, int length, int multiplicity, double xLow, double xUp, long int seconds, int nano) const;
  bool FillSmearedMCHisto1D(TH1D& histo, const std::string& channel, double* energies, double* weights, int length, int multiplicity, long int seconds, int nano, int binMC = 1) const;
  
  static void SetUserValues(const std::string dbTable, const std::string channel, double p0SS, double p1SS, double p2SS, double p0MS, double p1MS, double p2MS);
  
private:
  EXOEnergyResol() {;}
  EXOEnergyResol(std::string resolFlavor, std::string dbTableName);
  
  static std::set<EXOEnergyResol*> fInstances;

  std::string fCalibrationType;
  std::string fResolutionFlavor;
  std::string fDBTableName;

  double MCBasedFitResolution(const std::string& channel, double energy, EXOEnergyMCBasedFit* dbTable, int multiplicity) const;
  std::string MCBasedFitResolutionString(const std::string& channel, EXOEnergyMCBasedFit* dbTable, int multiplicity, std::string eLabel) const;
  


};

#endif
