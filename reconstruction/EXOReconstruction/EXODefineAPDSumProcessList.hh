#ifndef EXODefineAPDSumProcessList_hh
#define EXODefineAPDSumProcessList_hh

#include "EXOReconstruction/EXOVDefineReconProcessList.hh"
#include "EXOUtilities/EXOWaveform.hh"

class EXODefineAPDSumProcessList : public EXOVDefineReconProcessList {

  public:
    EXOReconProcessList GetProcessList(const EXOReconProcessList& data) const;
  
    EXODefineAPDSumProcessList();
    void SetSumBothPlanes(bool val)
      { fSumBothAPDPlanes = val; }
    bool SumBothPlanes() const { return fSumBothAPDPlanes; }

  protected:
    mutable EXOWaveform fAPDSumNorthOrBoth;
    mutable EXOWaveform fAPDSumSouth;
    bool fSumBothAPDPlanes;

};

#endif /* EXODefineAPDSumProcessList_hh */
