#ifndef EXO3DWeightPotentialFinder_hh
#define EXO3DWeightPotentialFinder_hh

#include "EXOUtilities/EXOMiscUtil.hh" // To forward-declare an enum, we have to specify the underlying type.
#include "EXOUtilities/EXO3DWeightReader.hh" 
#include "EXOUtilities/EXODimensions.hh"
#include "TH3F.h"


class EXO3DWeightPotentialFinder : public EXO3DWeightReader
{
public:
  EXO3DWeightPotentialFinder(const std::string& afile = "", EXOMiscUtil::EChannelType type = EXOMiscUtil::kOtherTag);
  double GetWeightPotential(EXOMiscUtil::EChannelType type, double chpos, double x, double y, double z) const;

  void LoadUDataFromFile(const std::string& aFilename);
  void LoadVDataFromFile(const std::string& aFilename);

  void CloseDataFromFile();

private:
  void ResetVariables(EXOMiscUtil::EChannelType type);
  TH3F *fWeightPotUWire;
  TH3F *fWeightPotVWire;
  
  double umin;
  double umax;
  double vmin;
  double vmax;
  double zmin;
  double zmax;
  double du ;
  double dv;
  double dz;
  size_t nu;
  size_t nv;
  size_t nz;
  // fWeightFieldUWire and fWeightFieldVWire store grid points for the ranges [fXMin, fXMax) and [fZMin, fZMax).
};

#endif
