//______________________________________________________________________________
/* EXO3DFieldReader

This class provides a class to read in Electric Field file that 
EXODigitizer uses. 

File contains 3 TH3F for EX, EY and EZ and each point in a grid of u,v,z 
urange: [-.5, 2.5]
vrange: [.5,  3.5]
zrange: [175, 200]

in steps of 0.02

*/
//______________________________________________________________________________
#include "EXOUtilities/EXO3DFieldReader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TFile.h"
#include "TH3F.h"
#include <fstream>
#include <iostream>

using namespace std;

EXO3DFieldReader::EXO3DFieldReader() 
{
  // Protected constructor for derived classes.
  fFileLoaded=false;
}

//______________________________________________________________________________
void EXO3DFieldReader::LoadFieldFromFile(
  const std::string& Filename, TH3F *&fEfieldX, TH3F *&fEfieldY, TH3F *&fEfieldZ)
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

  fFieldReader = new TFile(FullFilename.c_str(),"READ");
  
  if(fFieldReader->IsZombie()) {
    LogEXOMsg("Failed to open " + Filename, EEAlert);
  }
  

  fEfieldX = (TH3F*)fFieldReader->Get("Efield_histX");
  fEfieldY = (TH3F*)fFieldReader->Get("Efield_histY");
  fEfieldZ = (TH3F*)fFieldReader->Get("Efield_histZ");

  fFileLoaded=true;
}


void EXO3DFieldReader::CloseFieldFile()
{
  //if (not fFieldReader->IsZombie()){
  if (fFileLoaded){
    if(fFieldReader->IsOpen())
    {
        fFieldReader->Close();
        return;
    }
  }  
  return;
}




