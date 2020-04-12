//______________________________________________________________________________
/* EXOWeightPotentialReader

This class provides an interface to the pre-computed weight potential that
EXODigitizer uses.

Since the weight potential is computed in a 2D space, there is a disconnect
between the 2D geometry used here and the true 3D geometry of the detector.
This class does not assist much in that regard.  In particular, users are
expected to convert to the 2D coordinates used here.

The 2D model uses x-z coordinates, where z corresponds to z-position in the
true detector and x is some projection of the u-v coordinates.  (Again, the
calling class should implement an appropriate projection.)  Throughout, V will
denote the weight potential.  Units are cm and kV.  [To be clear:  units used
at runtime, as arguments and return values of the public functions, are all
CLHEP, ie. mm and MV.]

The weight potential is zero at z = 0 and z = 21.6 cm.  These correspond to the
APD plane and a z-distance sufficiently far from the v-wire grid.  x is
computed from x = -1.2 cm to x = 2.1 cm, also with a zero boundary condition.

Wires are placed at x = -1.15, -0.75, -0.45, -0.15, 0.15, 0.45, 0.75, 1.05,
1.35, 1.65, 1.95 cm, and at z = 0.6 cm and 1.2 cm.  The weights are with
reference to the central wire triplet at 0.15, 0.45, 0.75 cm, where the voltage
is equal to 1 keV for the appropriate set of wires (u or v wires).  On all
wires other than the reference wire, the voltage is set to zero.

*/
//______________________________________________________________________________

#include "EXOUtilities/EXO3DWeightPotentialFinder.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "TMath.h"
#include "TAxis.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

//______________________________________________________________________________
double EXO3DWeightPotentialFinder::GetWeightPotential(const EXOMiscUtil::EChannelType type, double chpos, double x, double y, double z) const
{
  // type should equal kUWire or kVWire.
  // x, z are in the 2D coordinate frame.
  // If these points lie outside the precalculated range, 0 is returned as the
  // best approximation.
  if(type != EXOMiscUtil::kUWire and type != EXOMiscUtil::kVWire) {
    LogEXOMsg("Invalid channel type received as argument.", EEAlert);
  }

  double u;
  double v;
  EXOMiscUtil::XYToUVCoords(u, v, x, y, z);
  
  double uhold = u;
  double vhold = v;

  //Want Relative Position for Channel that is looking for Weight
  //Middle of the file is the reference channel position at 4.5mm
  //Other position is just relative to the closest channel
  if(type == EXOMiscUtil::kUWire)
  {
    u = (u-chpos) + (umax+umin)/2.0;

    //If outside the range set to 0
    //Only outside the range if very far so weight should be small here anyways
    if(u > umax)
    {
        //LogEXOMsg("u > umax in the U Weight Shouldn't happen " + Stream.str(), EEWarning);
        return 0.0;
    }
    else if (u <= umin)
    {
        //LogEXOMsg("u < umin in the U Weight Shouldn't happen " + Stream.str(), EEWarning);
        return 0.0;
    }
    
    while(v <= vmin*CLHEP::mm) v += WIRE_PITCH*2;
    while(v > vmax*CLHEP::mm) v -= WIRE_PITCH*2;
    
  }
  else if(type == EXOMiscUtil::kVWire)
  {
    v = (v-chpos) + (vmax+vmin)/2.0;

    if(v > vmax)
    {
        //LogEXOMsg("v > vmax in the V Weight Shouldn't happen " + Stream.str(), EEWarning);
        return 0.0;
    }
    else if (v <= vmin)
    {
        //LogEXOMsg("v < vmin in the V Weight Shouldn't happen " + Stream.str(), EEWarning);
        return 0.0;
    }
    while(u <= umin*CLHEP::mm) u+= WIRE_PITCH*2;
    while(u > umax*CLHEP::mm) u-= WIRE_PITCH*2;
  }
  else
  {
      LogEXOMsg("Invalid channel type received as argument.", EEAlert);
      return 0.0;
  }


  //File only has positive z
  double z_hold = abs(z);
  z=abs(z);
  if(z >= zmax) z=zmax;
  else if(z <= zmin) z = zmin;

  //Find the correct Z-index.
  z-=zmin;
  z /= dz;
  z += 0.5; // to make rounding correct for positive z.
  size_t iz = (size_t)z;

  v -= vmin;
  v /= dv;
  v += 0.5; // to make rounding correct.
  size_t iv = (size_t)v;
  if(iv < 0 or iv > nv-1) {
      std::stringstream Stream;
      Stream << "V index off Weights: " << v << " " << iv  << std::endl;
      LogEXOMsg(Stream.str(), EEWarning);
  }

  u-=umin;
  u /= du;
  u += 0.5;
  size_t iu = (size_t)u;
  if(iu<0 or iu > nu-1){
      std::stringstream Stream;
      Stream << "U index off Weights: " << u << "  " << iu  <<std::endl;
      LogEXOMsg(Stream.str(), EEWarning);
  }

  size_t weight_index = iz*nv*nu + iu*nv + iv;

  if(weight_index > (nv*nu*nz - 1))
  {
      std::stringstream Stream;
      Stream << "Total index off " << weight_index << " " << x << " " << y << " " << z << std::endl;
      Stream << "Hold z =" << z_hold << " zmin = " << zmin << " dz = " << dz << std::endl;
      LogEXOMsg(Stream.str(), EEAlert);
  }
  if(type == EXOMiscUtil::kUWire)
  {
      return fWeightPotUWire->GetBinContent(weight_index);
  }
  else if(type == EXOMiscUtil::kVWire)
  {
      return fWeightPotVWire->GetBinContent(weight_index);
  }

  //Should never reach here 
  return 0.0;

}

//______________________________________________________________________________
void EXO3DWeightPotentialFinder::LoadUDataFromFile(const std::string& aFilename)
{
  // Load U Data from a binary file
  LoadWeightFromFile(aFilename, fWeightPotUWire);
  ResetVariables(EXOMiscUtil::kUWire);
}

//______________________________________________________________________________
void EXO3DWeightPotentialFinder::LoadVDataFromFile(const std::string& aFilename)
{
  // Load V Data from a binary file
  LoadWeightFromFile(aFilename, fWeightPotVWire);
  ResetVariables(EXOMiscUtil::kVWire);
}

void EXO3DWeightPotentialFinder::CloseDataFromFile()
{
    CloseWeightFromFile();
}


//______________________________________________________________________________
EXO3DWeightPotentialFinder::EXO3DWeightPotentialFinder(const std::string& afile,
  EXOMiscUtil::EChannelType type) : EXO3DWeightReader()
{
  // Fill the weight potential vectors from data files.
  if (afile != "")
  {
      if(type==EXOMiscUtil::kUWire)
      {
          LoadUDataFromFile(afile);
          return;
      }
      else if(type==EXOMiscUtil::kVWire)
      {
          LoadVDataFromFile(afile);
          return;
      }
      LogEXOMsg("File is not VWire or UWire file = " +afile, EEAlert);
  }
}

//______________________________________________________________________________
void EXO3DWeightPotentialFinder::ResetVariables(EXOMiscUtil::EChannelType type)
{
    //Get the size of the histograms that where just loaded
    //These are the parameters used to index the hists
    if(type == EXOMiscUtil::kUWire)
    {
        umin = fWeightPotUWire->GetYaxis()->GetXmin()*CLHEP::mm;
        umax = fWeightPotUWire->GetYaxis()->GetXmax()*CLHEP::mm;
        vmin = fWeightPotUWire->GetZaxis()->GetXmin()*CLHEP::mm;
        vmax = fWeightPotUWire->GetZaxis()->GetXmax()*CLHEP::mm;
        zmin = fWeightPotUWire->GetXaxis()->GetXmin()*CLHEP::mm;
        zmax = fWeightPotUWire->GetXaxis()->GetXmax()*CLHEP::mm;
        nu = fWeightPotUWire->GetNbinsY();
        nv = fWeightPotUWire->GetNbinsZ();
        nz = fWeightPotUWire->GetNbinsX();
        du = (umax-umin)/(nu-1)*CLHEP::mm;
        dv = (vmax-vmin)/(nv-1)*CLHEP::mm;
        dz = (zmax-zmin)/(nz-1)*CLHEP::mm;
        return;
    }
    else if (type == EXOMiscUtil::kVWire)
    {
        umin = fWeightPotVWire->GetYaxis()->GetXmin()*CLHEP::mm;
        umax = fWeightPotVWire->GetYaxis()->GetXmax()*CLHEP::mm;
        vmin = fWeightPotVWire->GetZaxis()->GetXmin()*CLHEP::mm;
        vmax = fWeightPotVWire->GetZaxis()->GetXmax()*CLHEP::mm;
        zmin = fWeightPotVWire->GetXaxis()->GetXmin()*CLHEP::mm;
        zmax = fWeightPotVWire->GetXaxis()->GetXmax()*CLHEP::mm;
        nu = fWeightPotVWire->GetNbinsY();
        nv = fWeightPotVWire->GetNbinsZ();
        nz = fWeightPotVWire->GetNbinsX();
        du = (umax-umin)/(nu-1)*CLHEP::mm;
        dv = (vmax-vmin)/(nv-1)*CLHEP::mm;
        dz = (zmax-zmin)/(nz-1)*CLHEP::mm;
        return;
    }
    else
    {
        LogEXOMsg("Reseting Weight Potential Reader Failed", EEAlert);
    }
    return;
}
