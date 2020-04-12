//___________________________________________________________________
//  This class is designed to hold the positions and times of an event.
//  It provides automatic conversions between UV and XY coordinates, and
//  in fact is meant to hide from the user any preferences between the two.

#include "EXOUtilities/EXOCoordinates.hh"

#include <cassert>
#include <cmath>
#include "TMath.h"

ClassImp(EXOCoordinates)

EXOCoordinates::EXOCoordinates() : TObject()
{
  Clear();
}

EXOCoordinates::EXOCoordinates(const EXOCoordinates& Coord)
: TObject(Coord)
{
  Clear();
  *this = Coord;
}

EXOCoordinates::EXOCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY, Double_t UorX, Double_t VorY, Double_t Z, Double_t T)
: TObject()
{
  // First argument should be either EXOMiscUtil::kUVCoordinates or EXOMiscUtil::kXYCoordinates.
  Clear();
  SetCoordinates(UVorXY, UorX, VorY, Z, T);
}

void EXOCoordinates::Clear(Option_t*)
{
  TObject::Clear();
  ResetBit(kIsInitialized);
}

void EXOCoordinates::SetCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY,
              Double_t UorX, Double_t VorY, Double_t Z, Double_t T)
{
  // First argument should be either EXOMiscUtil::kUVCoordinates or EXOMiscUtil::kXYCoordinates.
  assert( (UVorXY == EXOMiscUtil::kUVCoordinates) or (UVorXY == EXOMiscUtil::kXYCoordinates) );
  SetBit(kIsInitialized);
  fZ = Z;
  fT = T;
  if(UVorXY == EXOMiscUtil::kUVCoordinates) {
    EXOMiscUtil::UVToXYCoords(UorX, VorY, fX, fY, fZ);
  }
  else /*kXYCoordinates*/ {
    fX = UorX;
    fY = VorY;
  }
}

void EXOCoordinates::ChangeX(Double_t X)
{
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());
  fX = X;
}

void EXOCoordinates::ChangeY(Double_t Y)
{
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());
  fY = Y;
}

void EXOCoordinates::ChangeU(Double_t U)
{
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());

  Double_t TempU, TempV;
  EXOMiscUtil::XYToUVCoords(TempU, TempV, fX, fY, fZ);
  EXOMiscUtil::UVToXYCoords(U, TempV, fX, fY, fZ);
}

void EXOCoordinates::ChangeV(Double_t V)
{
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());

  Double_t TempU, TempV;
  EXOMiscUtil::XYToUVCoords(TempU, TempV, fX, fY, fZ);
  EXOMiscUtil::UVToXYCoords(TempU, V, fX, fY, fZ);
}

void EXOCoordinates::ChangeZ(Double_t Z)
{
  // Note this function can change U and V if we're changing halves of the TPC.
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());
  fZ = Z;
}

void EXOCoordinates::ChangeT(Double_t T)
{
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());
  fT = T;
}

void EXOCoordinates::RotateZDeg(Double_t degrees)
{
  //Rotates the coordinates around the Z-axis.
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  RotateZRad(degrees / 180. * TMath::Pi());
}

void EXOCoordinates::RotateZRad(Double_t radians)
{
  //Rotates the coordinates around the Z-axis.
  // Only call on an initialized object; we do not permit partially-filled coordinates.
  assert(IsInitialized());
  double cosA = cos(radians);
  double sinA = sin(radians);
  double newX = cosA*fX - sinA*fY;
  fY = sinA*fX + cosA*fY;
  fX = newX;
}

Double_t EXOCoordinates::GetX() const
{
  // Only call on an initialized object.
  assert(IsInitialized());
  return fX;
}

Double_t EXOCoordinates::GetY() const
{
  // Only call on an initialized object.
  assert(IsInitialized());
  return fY;
}

Double_t EXOCoordinates::GetU() const
{
  // Only call on an initialized object.
  assert(IsInitialized());

  Double_t U, V;
  EXOMiscUtil::XYToUVCoords(U, V, fX, fY, fZ);
  return U;
}

Double_t EXOCoordinates::GetV() const
{
  // Only call on an initialized object.
  assert(IsInitialized());

  Double_t U, V;
  EXOMiscUtil::XYToUVCoords(U, V, fX, fY, fZ);
  return V;
}

Double_t EXOCoordinates::GetZ() const
{
  // Only call on an initialized object.
  assert(IsInitialized());
  return fZ;
}

Double_t EXOCoordinates::GetT() const
{
  // Only call on an initialized object.
  assert(IsInitialized());
  return fT;
}

EXOCoordinateKey EXOCoordinates::GetCoordinateKey(EXOMiscUtil::ECoordinateSystem UVorXY) const
{
  // Return an EXOCoordinateKey (pixel) containing this position/time.
  // Pixel will be fixed in the coordinate system given by UVorXY.
  // Only call on an initialized object.
  assert(IsInitialized());
  assert( (UVorXY == EXOMiscUtil::kUVCoordinates) or (UVorXY == EXOMiscUtil::kXYCoordinates) );

  if(UVorXY == EXOMiscUtil::kXYCoordinates) {
    return EXOCoordinateKey(UVorXY, fX, fY, fZ, fT);
  }
  else /*kUVCoordinates*/ {
    Double_t U, V;
    EXOMiscUtil::XYToUVCoords(U, V, fX, fY, fZ);
    return EXOCoordinateKey(UVorXY, U, V, fZ, fT);
  }
}

Double_t Distance(const EXOCoordinates& Coord1, const EXOCoordinates& Coord2)
{
  // Returns the distance between the spatial coordinates of the two objects.
  // The two coordinates must both be initialized.
  // The function is a friend of EXOCoordinates.
  assert(Coord1.IsInitialized() and Coord2.IsInitialized());

  Double_t SqX = (Coord1.fX - Coord2.fX)*(Coord1.fX - Coord2.fX);
  Double_t SqY = (Coord1.fY - Coord2.fY)*(Coord1.fY - Coord2.fY);
  Double_t SqZ = (Coord1.fZ - Coord2.fZ)*(Coord1.fZ - Coord2.fZ);

  return sqrt(SqX + SqY + SqZ);
}

Double_t EXOCoordinates::PolarRadius() const
{
  // Gives the polar radius in the UV/XY plane; ie. returns sqrt(x^2+y^2).
  // The object should be initialized.
  assert(IsInitialized());

  return sqrt(fX*fX + fY*fY);
}

Double_t EXOCoordinates::PolarAngleDeg() const
{
  //Return the polar angle in the XY plane in degrees. Zero degree is along X-axis.
  //The object should be initialized.

  return 180. / TMath::Pi() * PolarAngleRad();
}

Double_t EXOCoordinates::PolarAngleRad() const
{
  //Return the polar angle in the XY plane in radians. Zero rad is along X-axis.
  //The object should be initialized.

  double angle = atan2(fY,fX);
  if(fY < 0.0){
    angle += TMath::TwoPi();
  }
  return angle;
}
