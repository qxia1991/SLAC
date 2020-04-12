#ifndef EXOClusterDebugData_hh
#define EXOClusterDebugData_hh

#include "TObject.h"
#include <vector>

class EXOClusterDebugData : public TObject
{
  public:
    virtual void Clear(Option_t* opt = "");

    int fEventNumber;
    std::vector<double> fZ;
    std::vector<double> fZMatched;
    std::vector<double> fEnergy;
    std::vector<double> fVEnergy;
    std::vector<double> fEnergyMatched;
    std::vector<double> fVEnergyMatched;
    std::vector<double> fTimeDiff;
    std::vector<double> fTime;
    std::vector<double> fTimeDiffMatched;
    std::vector<double> fTimeMatched;

    std::vector<int> fCostMatched;
    std::vector<double> fNLEnergy;
    std::vector<double> fNLEnergyMatched;
    std::vector<double> fNLTime;
    std::vector<double> fNLTimeMatched;
    std::vector<double> fNLPosition;
    std::vector<bool> fIsUCombinedMatched;
    std::vector<bool> fIsVCombinedMatched;

  ClassDef( EXOClusterDebugData, 2 )
};

inline void EXOClusterDebugData::Clear(Option_t* opt)
{
  fZ.clear();
  fZMatched.clear();
  fEnergy.clear();
  fVEnergy.clear();
  fEnergyMatched.clear();
  fVEnergyMatched.clear();
  fTimeDiff.clear();
  fTime.clear();
  fTimeDiffMatched.clear();
  fTimeMatched.clear();
  fCostMatched.clear();
  fNLEnergy.clear();
  fNLEnergyMatched.clear();
  fNLTime.clear();
  fNLTimeMatched.clear();
  fNLPosition.clear();
  fIsUCombinedMatched.clear();
  fIsVCombinedMatched.clear();
  fEventNumber = -1;
}

#endif
