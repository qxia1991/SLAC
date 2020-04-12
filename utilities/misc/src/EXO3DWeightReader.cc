//______________________________________________________________________________
/* EXO3DWeightReader

This class provides a class to read in Weight Potential file that 
EXODigitizer uses. 

File contains 3 TH3F for EX, EY and EZ and each point in a grid of u,v,z 
urange: []
vrange: []
zrange: []

in steps of 0.1

*/
//______________________________________________________________________________
#include "EXOUtilities/EXO3DWeightReader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TFile.h"
#include <fstream>


EXO3DWeightReader::EXO3DWeightReader()
{
    fFileLoaded = false;
}

//______________________________________________________________________________
void EXO3DWeightReader::LoadWeightFromFile(
  const std::string& Filename, TH3F *&fWeight) 
{

  // First check for a full path
  std::string FullFilename = EXOMiscUtil::SearchForFile(Filename);
  if(FullFilename == "") {
    // then check for a relative path in the data directory
    FullFilename = EXOMiscUtil::SearchForFile(Filename);
    if(FullFilename == "") {
      LogEXOMsg("Failed to find " + Filename, EEAlert);
    }
  }

  fWeightReader = new TFile(FullFilename.c_str(),"READ");
  if(fWeightReader->IsZombie()) {
    LogEXOMsg("Failed to open " + Filename, EEAlert);
  }


  fWeight = (TH3F*)fWeightReader->Get("Weight_hist");
  fFileLoaded = true;

}

void EXO3DWeightReader::CloseWeightFromFile()
{   
    if (fFileLoaded){
        if(fWeightReader->IsOpen())
        {
            fWeightReader->Close();
        }
    }
}

