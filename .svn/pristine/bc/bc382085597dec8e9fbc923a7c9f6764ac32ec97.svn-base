//______________________________________________________________________________
/* EXOElectricPotentialReader

This class provides an interface to the pre-computed electric potentials that
EXODigitizer uses.  It also is able to generate a first-order approximation to
the gradient to return an electric field vector.

Since the electric potentials are computed in a 2D space with periodic boundary
assumptions, there is a disconnect between the 3D points whose potentials we
want and the 2D electric potentials provided.  However, this class does not
assist in that regard.  The calling classes are expected to decide which 2D
points to query, and this class remains dumb (or "context-blind").

The 2D model uses x-z coordinates, where z corresponds to z-position in the
true detector and x is some projection of the u-v coordinates.  (Again, the
calling class should implement an appropriate projection.)  Throughout, V will
denote the potential.  Units are cm and kV.  [To be clear:  units used at
runtime, as arguments and return values of the public functions, are all CLHEP,
ie. mm and MV.]

The boundary conditions that were used to generate this potential function
were:

V = -1.4 at z = 0		(corresponding to the APD plane)
V = -8.0 at z = 2.5		(beyond that point, the potential is assumed
                                 linear in z and constant in x)
V(x, z) = V(x + 0.3, z)		(periodic boundary conditions in x) 
V = 0 on a circle centered at (0.15, 0.6) with radius 0.005.  V undefined
				 inside this circle.  (a single collection wire
				 -- periodicity will mimic the "u-wire plane")
V = -4.35 on a circle centered at (0.15, 1.2) with radius 0.005.  V undefined
				 inside this circle.  (a single induction wire
                                 -- periodicity will mimic the "v-wire plane")
*/
//______________________________________________________________________________

#include "EXOUtilities/EXOElectricPotentialReader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <fstream>
#include <string>

//______________________________________________________________________________
void EXOElectricPotentialReader::GetEField(double x, double z, double& ex, double& ez) const
{
  // From the potential map, take a [negative] derivative at (x,z) to
  // approximate the electric field (ex, ez).  The position (x,z) is in
  // millimeters, and the electric field is in units of megavolts/millimeter.
  // (CLHEP units!) Apply periodicity in x, but slightly non-trivially.  (I
  // don't know how to describe it, see code, will fix.)
  // For z > fZMax, assume that E(x,z) = E(x,fZMax).
  // For z < 0, log an error and return E = (0, -1000) MV/mm.

  if(z < fDZ) {
    LogEXOMsg("Tried to get electric field too close to the APD face.", EEError);
    ex = 0.0*CLHEP::megavolt/CLHEP::mm;
    ez = -1000.0*CLHEP::megavolt/CLHEP::mm; // very large repulsive electric field -- electrons don't enter here.
    return;
  }
  if(z >= fZMax - 5.0*fDZ) z = fZMax - 5.0*fDZ;

  // We rely on correct periodicity.
  while(x < 0.0*CLHEP::mm) x += WIRE_PITCH;
  while(x >= WIRE_PITCH) x -= WIRE_PITCH;

  // Find the correct X-index.
  x -= fXMin;
  x /= fDX;
  x += 0.5; // to make rounding correct.
  if(x < 1 or x >= fNX-1) {
    LogEXOMsg("Somehow, ended up with a wrong x-index", EEAlert);
  }
  size_t ix = (size_t)x;

  // Find the correct Z-index.
  z /= fDZ;
  z += 0.5; // to make rounding correct for positive z.
  size_t iz = (size_t)z;

  // If we're on the x-boundary, that's OK since we have periodic boundary conditions in x.
  // Remember, we're taking the negative derivative.
  ex = (GetPotentialAt(ix-1, iz) - GetPotentialAt(ix+1, iz))/(2.0*fDX);

  // If iz > fNZ-2, we'll need to translate downward.  Only use adjacent points in this case.
//  if(iz > fNZ-2) ez = (GetPotentialAt(ix, fNZ-2) - GetPotentialAt(ix, fNZ-1))/fDZ;
  if(iz > fNZ-5) ez = (GetPotentialAt(ix, fNZ-6) - GetPotentialAt(ix, fNZ-4))/(2.0*fDZ); // To match current behavior.

  else if(iz == 0) ez = (GetPotentialAt(ix, 0) - GetPotentialAt(ix, 1))/fDZ;
  else ez = (GetPotentialAt(ix, iz-1) - GetPotentialAt(ix, iz+1))/(2.0*fDZ);
}

//______________________________________________________________________________
double EXOElectricPotentialReader::GetPotentialAt(size_t ix, size_t iz) const
{
  // Private helper function -- does the conversion from ix, iz to an index of
  // the array.
  return fPotential[fNZ*ix + iz];
}

//______________________________________________________________________________
void EXOElectricPotentialReader::LoadFieldDataFromFile(const std::string& aFile)
{
  // Load field data from a file
  LoadDataFromFile(aFile, fPotential);
}

//______________________________________________________________________________
EXOElectricPotentialReader::EXOElectricPotentialReader(const std::string& afile)
: EXOPotentialReader(
   -0.5*CLHEP::mm, // xmin
    3.5*CLHEP::mm, // xmax
    0.0*CLHEP::mm, // zmin
    25.0*CLHEP::mm,// zmax
    0.02*CLHEP::mm,//dx
    0.02*CLHEP::mm,// dz
    200, // (fXMax - fXMin)/fDX, since the range is [fXMin, fXMax)
    1251), // (fZMax - fZMin + 1)/fDZ, since the range is [fZMin, fZMax]
  fPotential(fNX*fNZ)
{
  if (afile != "") LoadFieldDataFromFile(afile);
}
