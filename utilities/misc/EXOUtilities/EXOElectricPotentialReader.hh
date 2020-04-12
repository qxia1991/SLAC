#ifndef EXOElectricPotentialReader_hh
#define EXOElectricPotentialReader_hh

#include "EXOUtilities/EXOPotentialReader.hh"
#include <cstddef> //for size_t

class EXOElectricPotentialReader : public EXOPotentialReader
{
public:
  EXOElectricPotentialReader(const std::string& bin_file = "");
  void GetEField(double x, double z, double& ex, double& ez) const;

  void LoadFieldDataFromFile(const std::string& aFilename);
private:
  double GetPotentialAt(size_t ix, size_t iz) const;

  DblVec fPotential;
  // fPotential stores grid points for the ranges [fXMin, fXMax) and [fZMin, fZMax].
};

#endif
