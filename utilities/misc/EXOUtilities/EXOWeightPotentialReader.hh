#ifndef EXOWeightPotentialReader_hh
#define EXOWeightPotentialReader_hh

#include "EXOUtilities/EXOMiscUtil.hh" // To forward-declare an enum, we have to specify the underlying type.
#include "EXOUtilities/EXOPotentialReader.hh" 
#include "EXOUtilities/EXODimensions.hh"

class EXOWeightPotentialReader : public EXOPotentialReader
{
public:
  EXOWeightPotentialReader(const std::string& ufile = "",
                           const std::string& vfile = "",
                           double xmin = -12.0*CLHEP::mm, 
                           double xmax = 21.0*CLHEP::mm, 
                           double zmin = 0.0, 
                           double zmax = 216.0*CLHEP::mm, 
                           double dx = 0.1*CLHEP::mm, 
                           double dz = 0.1 *CLHEP::mm, 
                           size_t nx = 330, 
                           size_t nz = 2160 );
  double GetWeightPotential(EXOMiscUtil::EChannelType type, double x, double z) const;

  void LoadUDataFromFile(const std::string& aFilename);
  void LoadVDataFromFile(const std::string& aFilename);

private:
  void ResetVariables(const std::string& fileName);
  DblVec fWeightPotUWire;
  DblVec fWeightPotVWire;
  // fWeightFieldUWire and fWeightFieldVWire store grid points for the ranges [fXMin, fXMax) and [fZMin, fZMax).
};

#endif
