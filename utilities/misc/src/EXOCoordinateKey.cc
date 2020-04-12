//______________________________________________________________________
//  This class is designed to identify coordinate/time pixels; the sibling class
//  EXOCoordinates is designed to store fully continuous coordinates.  The sizes
//  of the pixels are defined in EXODimensions.hh as MCCHARGE_PIXEL_SIZE and
//  MCCHARGE_PIXEL_TIME.
//
//  There is a risk here that an EXOCoordinateKey may be written to file, and the 
//  pixel sizes subsequently changed in EXOAnalysis.  The objects will be broken
//  on subsequent reads.
//
//  To address this, the class sets internal doubles storing the values of
//  MCCHARGE_PIXEL_SIZE and MCCHARGE_PIXEL_TIME, which get set in every initialized
//  instance and persists in a file until the next read.  The function CheckVersion()
//  returns false if the pixel size has changed between builds.  Currently we
//  perform the check in every accessor, but if performance is an issue the
//  assertions could be removed (adding burden to the user!).

#include "EXOUtilities/EXOCoordinateKey.hh"
#include <cassert>
#include <cmath>

ClassImp(EXOCoordinateKey)

EXOCoordinateKey::EXOCoordinateKey()
: TObject(),
  kTolerance(0.0001*CLHEP::mm)
{
  Clear();
}

EXOCoordinateKey::EXOCoordinateKey(const EXOCoordinateKey& Coord)
: TObject(Coord),
  kTolerance(0.0001*CLHEP::mm)
{
  Clear();
  *this = Coord;
}

EXOCoordinateKey::EXOCoordinateKey(EXOMiscUtil::ECoordinateSystem UVorXY, 
                                   Double_t UorX, Double_t VorY, Double_t Z, Double_t T)
: TObject(),
  kTolerance(0.0001*CLHEP::mm)
{
  // First argument should be either EXOMiscUtil::kUVCoordinates or EXOMiscUtil::kXYCoordinates.
  Clear();
  SetCoordinates(UVorXY, UorX, VorY, Z, T);
}

EXOCoordinateKey& EXOCoordinateKey::operator = (const EXOCoordinateKey& rhs)
{
  assert(rhs.CheckVersion());
  TObject::operator=(rhs);
  fUorX = rhs.fUorX;
  fVorY = rhs.fVorY;
  fZ = rhs.fZ;
  fT = rhs.fT;
  fCoordinateSystem = rhs.fCoordinateSystem;
  fMCChargePixelSize = rhs.fMCChargePixelSize;
  fMCChargePixelTime = rhs.fMCChargePixelTime;

  return *this;
}

void EXOCoordinateKey::Clear(Option_t*)
{
  TObject::Clear();
  ResetBit(kIsInitialized);
}

void EXOCoordinateKey::SetCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY, 
                                      Double_t UorX, Double_t VorY, Double_t Z, Double_t T)
{
  // Converts coordinates into internally-stored pixel indices.
  // First argument should be either EXOMiscUtil::kUVCoordinates or EXOMiscUtil::kXYCoordinates.
  assert( (UVorXY == EXOMiscUtil::kUVCoordinates) or (UVorXY == EXOMiscUtil::kXYCoordinates) );

  // Store the current pixel dimensions, to detect changes in future builds.
  fMCChargePixelSize = MCCHARGE_PIXEL_SIZE;
  fMCChargePixelTime = MCCHARGE_PIXEL_TIME;

  SetBit(kIsInitialized);
  fCoordinateSystem = UVorXY;

  fUorX = (Int_t)(UorX/MCCHARGE_PIXEL_SIZE);
  if(UorX < 0) fUorX--;
  fVorY = (Int_t)(VorY/MCCHARGE_PIXEL_SIZE);
  if(VorY < 0) fVorY--;

  fZ = (Int_t)(Z/MCCHARGE_PIXEL_SIZE);
  if(Z < 0) fZ--;
  fT = (Int_t)(T/MCCHARGE_PIXEL_TIME);
  if(T < 0) fT--;
}

Bool_t EXOCoordinateKey::CheckVersion() const
{
  // Check that the pixel sizes defined in EXODimensions are the same ones that were used when this object was created.
  // This is useful for EXOCoordinateKeys that are being read from a file.
  // The object is invalid if since initialization, the pixel size has been changed.
  // True means the object is valid.  We also returns true if it is uninitialized.

  if(not IsInitialized()) return true;
  return ( fabs(fMCChargePixelSize - MCCHARGE_PIXEL_SIZE) < kTolerance and 
           fabs(fMCChargePixelTime - MCCHARGE_PIXEL_TIME) < kTolerance );
}

Bool_t EXOCoordinateKey::operator < (const EXOCoordinateKey& rhs) const
{
  // Compare t; then z; then x or u; then v or y.
  // Two objects must be initialized and have the same coordinate system.
  assert(IsInitialized() and rhs.IsInitialized());
  assert(CheckVersion() and rhs.CheckVersion());
  assert(GetCoordinateSystem() == rhs.GetCoordinateSystem());

  if(fT != rhs.fT) return fT < rhs.fT;
  else if(fZ != rhs.fZ) return fZ < rhs.fZ;
  else if(fUorX != rhs.fUorX) return fUorX < rhs.fUorX;
  else return fVorY < rhs.fVorY;
}

Bool_t EXOCoordinateKey::operator == (const EXOCoordinateKey& rhs) const
{
  // Two objects must be initialized and have the same coordinate system.
  assert(IsInitialized() and rhs.IsInitialized());
  assert(CheckVersion() and rhs.CheckVersion());
  assert(GetCoordinateSystem() == rhs.GetCoordinateSystem());

  return ( (fUorX == rhs.fUorX) and (fVorY == rhs.fVorY) and (fZ == rhs.fZ) and (fT == rhs.fT) );
}

EXOCoordinates EXOCoordinateKey::GetCenter() const
{
  // Return the coordinates corresponding to the center of this pixel.
  // Only call on an initialized object.
  assert(IsInitialized());
  assert(CheckVersion());

  Double_t UorX = MCCHARGE_PIXEL_SIZE * (0.5 + (Double_t)fUorX);
  Double_t VorY = MCCHARGE_PIXEL_SIZE * (0.5 + (Double_t)fVorY);
  Double_t Z = MCCHARGE_PIXEL_SIZE * (0.5 + (Double_t)fZ);
  Double_t T = MCCHARGE_PIXEL_TIME * (0.5 + (Double_t)fT);

  return EXOCoordinates(GetCoordinateSystem(), UorX, VorY, Z, T);
}

EXOMiscUtil::ECoordinateSystem EXOCoordinateKey::GetCoordinateSystem() const
{
  // Object must be initialized.
  assert(IsInitialized());
  assert(CheckVersion());

  return fCoordinateSystem;
}

Bool_t EXOCoordinateKey::IsInitialized() const
{
  return TestBit(kIsInitialized);
}
