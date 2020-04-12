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

#include "EXOUtilities/EXOWeightPotentialReader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "TMath.h"

#include <cstdlib>
#include <string>
#include <fstream>

//______________________________________________________________________________
double EXOWeightPotentialReader::GetWeightPotential(const EXOMiscUtil::EChannelType type, double x, double z) const
{
  // type should equal kUWire or kVWire.
  // x, z are in the 2D coordinate frame.
  // If these points lie outside the precalculated range, 0 is returned as the
  // best approximation.
  if(type != EXOMiscUtil::kUWire and type != EXOMiscUtil::kVWire) {
    LogEXOMsg("Invalid channel type received as argument.", EEAlert);
  }


  x -= fXMin;
  if(x < 0.0) return 0.0; // Get this out of the way now.
  x /= fDX;
  x += 0.5; // To make rounding correct.
  size_t ix = (size_t)x;

  z -= fZMin;
  if(z < 0.0) return 0.0; // Get this out of the way now.
  z /= fDZ;
  z += 0.5; // To make rounding correct.
  size_t iz = (size_t)z;

  if(ix >= fNX or iz >= fNZ) return 0.0;

  if(type == EXOMiscUtil::kUWire) return fWeightPotUWire[fNZ*ix + iz];
  else /*type == EXOMiscUtil::kVWire*/ return fWeightPotVWire[fNZ*ix + iz];
}

//______________________________________________________________________________
void EXOWeightPotentialReader::LoadUDataFromFile(const std::string& aFilename)
{
  // Load U Data from a binary file
  ResetVariables(aFilename);
  LoadDataFromFile(aFilename, fWeightPotUWire);
}

//______________________________________________________________________________
void EXOWeightPotentialReader::LoadVDataFromFile(const std::string& aFilename)
{
  // Load V Data from a binary file
  ResetVariables(aFilename);
  LoadDataFromFile(aFilename, fWeightPotVWire);
}

//______________________________________________________________________________
EXOWeightPotentialReader::EXOWeightPotentialReader(const std::string& ufile,
                                                   const std::string& vfile,
  double xmin,
  double xmax,
  double zmin,
  double zmax,
  double dx,
  double dz,
  size_t nx,
  size_t nz)
  
: EXOPotentialReader(xmin,
    xmax, 
    zmin, 
    zmax, 
    dx, 
    dz, 
    nx, // (fXMax - fXMin)/fDX, since the range is [fXMin, fXMax)
    nz), // (fZMax - fZMin)/fDZ, since the range is [fZMin, fZMax)
    fWeightPotUWire(fNX*fNZ),
    fWeightPotVWire(fNX*fNZ)
{
  // Fill the weight potential vectors from data files.
  if (ufile != "") LoadUDataFromFile(ufile);
  if (vfile != "") LoadVDataFromFile(vfile);
}

//______________________________________________________________________________
void EXOWeightPotentialReader::ResetVariables(const std::string& Filename)
{
  // First check for a full path
  std::string FullFilename = EXOMiscUtil::SearchForFile(Filename);
  if(FullFilename == "") {
    // then check for a relative path in the data directory
    FullFilename = EXOMiscUtil::SearchForFile("data/maxwell/" + Filename);
    if(FullFilename == "") {
      LogEXOMsg("Failed to find " + Filename, EEAlert);
    }
  }

  std::ifstream Reader(FullFilename.c_str(), std::ios_base::binary | std::ios_base::in);
  if(Reader.fail()) {
    LogEXOMsg("Failed to open " + Filename, EEAlert);
  }
#define READ_AND_CHECK(reader, var)     \
  reader.read((char*)&var, sizeof(var));\
  if(reader.bad() || reader.eof()) {    \
    LogEXOMsg("An error was encountered reading " + Filename, EEAlert); \
  }

  READ_AND_CHECK(Reader, fXMin)
  READ_AND_CHECK(Reader, fZMin)
  READ_AND_CHECK(Reader, fXMax)
  READ_AND_CHECK(Reader, fZMax)
  READ_AND_CHECK(Reader, fDX)
  READ_AND_CHECK(Reader, fDZ)
  fNX = TMath::Nint((fXMax - fXMin)/fDX);
  fNZ = TMath::Nint((fZMax - fZMin)/fDZ);

}
