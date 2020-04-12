#ifndef EXOVDefineReconProcessList_hh
#define EXOVDefineReconProcessList_hh
#include "EXOReconstruction/EXOReconProcessList.hh" 
#include "EXOReconstruction/EXOSignalModelRegistrant.hh" 

class EXOVSignalModelBuilder;
class EXOVDefineReconProcessList : public EXOSignalModelRegistrant
{
  public:
    virtual EXOReconProcessList GetProcessList(const EXOReconProcessList& data) const = 0;

  protected:
    void UpdateSignalModelMgrWithBuilder(
      int chanOrTag, 
      const EXOVSignalModelBuilder& transFunc) const;
};

#endif /* EXOVDefineReconProcessList_hh */
