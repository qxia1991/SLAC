#ifndef EXONewYMatchExtractor_hh
#define EXONewYMatchExtractor_hh

#include "EXOReconstruction/EXOVSignalParameterExtractor.hh"

class EXONewYMatchExtractor: public EXOVSignalParameterExtractor
{
  public:
    EXONewYMatchExtractor(); virtual EXOSignalCollection Extract(
      const EXOReconProcessList& processList,
      const EXOSignalCollection& inputSignals) const;

  protected:
    void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

};




#endif
