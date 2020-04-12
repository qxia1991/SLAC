#ifndef EXO3DWeightReader_hh
#define EXO3DWeightReader_hh

#include <vector>
#include <string>
#include "TH3F.h"
#include "TFile.h"

class EXO3DWeightReader
{
protected:
  EXO3DWeightReader();
  //Get the Electric Weight from the file

  TFile *fWeightReader;

  void CloseWeightFromFile();
 
  void LoadWeightFromFile(const std::string& Filename, TH3F *&fWeight);

private:
  bool fFileLoaded;

};

#endif
