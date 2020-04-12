#ifndef EXO3DElectricFieldFinder_hh
#define EXO3DElectricFieldFinder_hh

#include "EXOUtilities/EXO3DFieldReader.hh"
#include <cstddef> //for size_t
#include "TH3F.h"

class EXO3DElectricFieldFinder : public EXO3DFieldReader
{
public:
  EXO3DElectricFieldFinder(const std::string& root_file = "");
  void GetEField(double x, double y, double z, double& ex, double& ey, double& ez) const;

  //void GetShiftedPos(double x, double y, double z, double& xshift, double& yshift) const;

  void LoadFieldDataFromFile(const std::string& aFilename);
  void CloseFieldDataFromFile();

private:
  
  void ResetVariables();

  TH3F *fEfieldX;
  TH3F *fEfieldY;
  TH3F *fEfieldZ;

  double fUMin;
  double fUMax;
  double fVMin;
  double fVMax;
  double fZMin;
  double fZMax;
  double fDU;
  double fDV;
  double fDZ;
  size_t fNU;
  size_t fNV;
  size_t fNZ;


};

#endif
