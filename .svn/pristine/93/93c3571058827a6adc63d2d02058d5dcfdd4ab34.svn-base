#ifndef EXOWireCrossing_hh
#define EXOWireCrossing_hh

#include <map>
#include "EXOUtilities/EXOCoordinates.hh"

class EXOWireCrossing
{
  private:
    //This is a singleton
    EXOWireCrossing();
    EXOWireCrossing(const EXOWireCrossing&) {};
    EXOWireCrossing& operator=(const EXOWireCrossing&) {return *this;}
    ~EXOWireCrossing() {};

  public:
    static EXOWireCrossing& GetInstance();
    double GetDistanceFromHexagon(const EXOCoordinates &coord) const;
    double GetHexagonHalfDiameter() const {
      //Return half distance between opposite sides of the hexagon
      return fHalfDiameter;
    }

  protected:
    double fHalfDiameter; //Half the distance between opposite sides
    double fHalfSide; //Half the length of a side
    EXOCoordinates fEdgePlus30Deg; //Position of the corner at +30°
    EXOCoordinates fEdgeMinus30Deg; //Position of the corner at -30°
};

#endif
