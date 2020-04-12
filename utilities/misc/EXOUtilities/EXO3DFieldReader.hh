#ifndef EXO3DFieldReader_hh
#define EXO3DFieldReader_hh

#include <vector>
#include <string>
#include "TH3F.h"
#include "TFile.h"

class EXO3DFieldReader
{
public:
    TFile *fFieldReader;

protected:
  EXO3DFieldReader();

  void CloseFieldFile();

  //Get the Electric Field from the file
  void LoadFieldFromFile(const std::string& Filename,
    TH3F *&fPotentialX, TH3F *&fPotentialY, TH3F *&fPotentialZ) ;

  bool fFileLoaded;
private:
};

#endif
