#ifndef EXOPotentialReader_hh
#define EXOPotentialReader_hh

#include <vector>
#include <string>

class EXOPotentialReader
{
protected:
  typedef std::vector<double> DblVec;
  // fWeightFieldUWire and fWeightFieldVWire store grid points for the ranges [fXMin, fXMax) and [fZMin, fZMax).
  EXOPotentialReader(double xmin, 
                     double xmax,
                     double zmin,
                     double zmax,
                     double dx,
                     double dz,
                     size_t nx,
                     size_t nz);
  double fXMin;
  double fXMax;
  double fZMin;
  double fZMax;
  double fDX;
  double fDZ;
  size_t fNX;
  size_t fNZ;
  void LoadDataFromFile(const std::string& Filename, 
    DblVec& PotentialVector) const;

private:
  EXOPotentialReader();
};

#endif
