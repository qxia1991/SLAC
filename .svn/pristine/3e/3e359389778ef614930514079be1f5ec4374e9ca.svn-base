#ifndef EXOTreeSaverModule_hh
#define EXOTreeSaverModule_hh
#include "EXOAnalysisManager/EXOAnalysisModule.hh"
#include "Rtypes.h"
class TTree;
class EXOTreeSaverModule : public EXOAnalysisModule
{
  public:
    EXOTreeSaverModule();
    ~EXOTreeSaverModule();

  protected:
    void FillTree(EXOEventData *ED);

    virtual void SetupBranches(TTree* atree) {}

    virtual std::string GetTreeName() { return ""; }
    virtual std::string GetTreeDescription() { return ""; }

  private:

    Int_t          fEventNumber;
    Int_t          fRunNumber;
    TTree*         fTree;
    Bool_t         fShouldFillTree;

    std::string    fRegisteredTreeName;

};

#endif /* EXOTreeSaverModule_hh */
