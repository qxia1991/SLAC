#include <cmath>
#include <limits>
#include <iostream>
#include "EXOUtilities/EXOWireCrossing.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

using namespace std;

EXOWireCrossing::EXOWireCrossing()
: fHalfDiameter(NCHANNEL_PER_WIREPLANE * CHANNEL_WIDTH / 2.0),
  fHalfSide(fHalfDiameter / sqrt(3.0))
{
  fEdgePlus30Deg.SetCoordinates(EXOMiscUtil::kXYCoordinates,fHalfDiameter,fHalfSide,CATHODE_ANODE_DISTANCE,0.0);
  fEdgeMinus30Deg.SetCoordinates(EXOMiscUtil::kXYCoordinates,fHalfDiameter,-fHalfSide,CATHODE_ANODE_DISTANCE,0.0);
}

EXOWireCrossing& EXOWireCrossing::GetInstance()
{
  static EXOWireCrossing instance;
  return instance;
}

double EXOWireCrossing::GetDistanceFromHexagon(const EXOCoordinates &coord) const
{
  //Calculate the 2d-distance of coord from the wire hexagon.
  //If coord is inside the hexagon, zero is returned.

  EXOCoordinates position(coord);
  //Make use of the 6-fold rotational symmetry of the hexagon.
  //Rotate the position, so that its vector has an angle between -30° and 30°
  //with the X-axis.
  double angle = position.PolarAngleDeg();
  if(angle > 330.){
    //Do nothing
  }
  else if(angle > 270.){
    position.RotateZDeg(-300.);
  }
  else if(angle > 210.){
    position.RotateZDeg(-240.);
  }
  else if(angle > 150.){
    position.RotateZDeg(-180.);
  }
  else if(angle > 90.){
    position.RotateZDeg(-120.);
  }
  else if(angle > 30.){
    position.RotateZDeg(-60.);
  }

  if(position.GetX() < fHalfDiameter){
    //In this case the position is inside the hexagon.
    return 0.0;
  }
  if(fabs(position.GetY()) <= fHalfSide){
    //In this case the shortest distance is from the side of the hexagon.
    return position.GetX() - fHalfDiameter;
  }
  else{
    //In this case the shortest distance is from the corner of the hexagon.
    double xdiff = position.GetX();
    double ydiff = position.GetY();
    if(position.GetY() > 0.0){
      xdiff -= fEdgePlus30Deg.GetX();
      ydiff -= fEdgePlus30Deg.GetY();
    }
    else{
      xdiff -= fEdgeMinus30Deg.GetX();
      ydiff -= fEdgeMinus30Deg.GetY();
    }
    return sqrt(xdiff*xdiff + ydiff*ydiff);
  }
  //Should never get here
  return 0.0;
}

