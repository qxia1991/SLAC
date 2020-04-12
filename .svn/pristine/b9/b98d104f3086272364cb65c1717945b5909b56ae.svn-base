//______________________________________________________________________________
/* EXO3DElectricFieldFinder


*/
//______________________________________________________________________________

#include "EXOUtilities/EXO3DElectricFieldFinder.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>    // for the int form of abs
#include <iostream>

using namespace std;

//______________________________________________________________________________
void EXO3DElectricFieldFinder::GetEField(double x, double y, double z, double& ex, double& ey, double& ez) const
{
  //Finds the Electric field vecotr at x,y,z
  
  //Files are indexed in u,v,z so quickly convert (x,y,z) to (u,v,z)
  double u;
  double v;
  EXOMiscUtil::XYToUVCoords(u, v, x, y, z);

  int ez_sign = 1;
  if (z < 0) {
      ez_sign = -1;
  } 
  z = abs(z);  


  double uhold = u;
  //Check to see how close you are to z=200 the end of the Efield file
  //Carried from old code is this correct since we have an Efield definded
  if((fZMax-z) < fDZ) {
    LogEXOMsg("Tried to get electric field too close to the APD face. ", EEError);
    ex = 0.0*CLHEP::megavolt/CLHEP::mm;
    ey = 0.0*CLHEP::megavolt/CLHEP::mm;
    ez = -1000.0*ez_sign*CLHEP::megavolt/CLHEP::mm; // very large repulsive electric field -- electrons don't enter here.
    return;
  }
  
  //Wires are seperated by 3mm so shift u,v position into the range umin->umax and vmin->max
  //since the file only contains a chuck of the detector spanning 1.5mm on either side of a wire
  //centered at 1mm for uwire and 2mm for vwire
  while(v <= fVMin*CLHEP::mm) v += WIRE_PITCH;
  while(v > fVMax*CLHEP::mm) v -= WIRE_PITCH;

  while(u <= fUMin*CLHEP::mm) u+=WIRE_PITCH;
  while(u > fUMax*CLHEP::mm) u-= WIRE_PITCH;

  //File only extends to z=175mm after which it is assumed to be uniform so just shift back to z=175mm
  if(z < fZMin*CLHEP::mm) z = fZMin;


  // Find the correct index for u,v,z
  //add 0.5 because occasionlly round is messed up (EX: 133.0 would round to 132)
  //Casting to size_t always rounds down  (floors it)
  v -= fVMin;
  v /= fDV;
  v += 0.5; // to make rounding correct.
  size_t iv = (size_t)v;
  if(iv < 0 or iv > fNV-1) {
      std::stringstream Stream;
      Stream << "V index off: " << v  << std::endl;
      LogEXOMsg(Stream.str(), EEWarning);
  }

  uhold = u;
  u-=fUMin;
  u /= fDU;
  u += 0.5;
  size_t iu = (size_t)u;
  if(iu<0 or iu > fNU-1){
      std::stringstream Stream;
      Stream << "U index off: " << uhold << std::endl;
      LogEXOMsg(Stream.str(), EEWarning);
  }

  // Find the correct Z-index.
  z-=fZMin;
  z /= fDZ;
  z += 0.5; // to make rounding correct for positive z.
  size_t iz = (size_t)z;

  size_t field_index = iz*fNV*fNU + iu*fNV + iv;

  if(field_index > (fNV*fNU*fNZ - 1))
  {
      std::stringstream Stream;
      Stream << "Total index off " << field_index << " " << x << " " << y << " " << z << std::endl;
      LogEXOMsg(Stream.str(), EEWarning);

  }

  //Mirror the X-axis (x -> -x for TPC2)
  ex = ez_sign*fEfieldX->GetBinContent(field_index);
  ey = fEfieldY->GetBinContent(field_index);
  //Make sure Ez is pointed in a way to drift electrons to the anode for each TPC Side
  ez = ez_sign*fEfieldZ->GetBinContent(field_index);

}


//______________________________________________________________________________
void EXO3DElectricFieldFinder::LoadFieldDataFromFile(const std::string& aFile)
{
  // Load field data from a file
  LoadFieldFromFile(aFile, fEfieldX, fEfieldY, fEfieldZ);
  ResetVariables();
}

void EXO3DElectricFieldFinder::CloseFieldDataFromFile()
{
    CloseFieldFile();
}

//______________________________________________________________________________
EXO3DElectricFieldFinder::EXO3DElectricFieldFinder(const std::string& afile)
:  EXO3DFieldReader()
{
  if (afile != "") LoadFieldDataFromFile(afile);
}

void EXO3DElectricFieldFinder::ResetVariables()
{
    fUMin = fEfieldX->GetYaxis()->GetXmin()*CLHEP::mm;
    fUMax = fEfieldX->GetYaxis()->GetXmax()*CLHEP::mm;
    fVMin = fEfieldX->GetZaxis()->GetXmin()*CLHEP::mm;
    fVMax = fEfieldX->GetZaxis()->GetXmax()*CLHEP::mm;
    fZMin = fEfieldX->GetXaxis()->GetXmin()*CLHEP::mm;
    fZMax = fEfieldX->GetXaxis()->GetXmax()*CLHEP::mm;
    fNU = fEfieldX->GetNbinsY();
    fNV = fEfieldX->GetNbinsZ();
    fNZ = fEfieldX->GetNbinsX();
    fDU = (fUMax-fUMin)/(fNU-1)*CLHEP::mm;
    fDV = (fVMax-fVMin)/(fNV-1)*CLHEP::mm;
    fDZ = (fZMax-fZMin)/(fNZ-1)*CLHEP::mm;       
    return;
}
