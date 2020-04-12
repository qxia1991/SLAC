#ifndef EXOCoordinates_hh
#define EXOCoordinates_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif

#ifndef EXOCoordinateKey_hh
#include "EXOUtilities/EXOCoordinateKey.hh"
#endif
#include "EXOUtilities/EXOMiscUtil.hh"

class EXOCoordinateKey;

class EXOCoordinates : public TObject
{

    enum ECoordinateStatusBits{
      kIsInitialized = BIT(14) // Stored in TObject; set if coordinates have been initialized.
    };

  public:


    // Constructors
    EXOCoordinates();
    EXOCoordinates(const EXOCoordinates& Coord);
    EXOCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY, Double_t UorX, Double_t VorY, Double_t Z, Double_t T);

    // Accessors
    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetU() const;
    Double_t GetV() const;
    Double_t GetZ() const;
    Double_t GetT() const;
    EXOCoordinateKey GetCoordinateKey(EXOMiscUtil::ECoordinateSystem UVorXY) const;
    Bool_t IsInitialized() const {return TestBit(kIsInitialized);}

    // Modifiers
    void Clear(Option_t* opt = "");
    void SetCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY,
                        Double_t UorX, Double_t VorY, Double_t Z, Double_t T);
    void ChangeX(Double_t X);
    void ChangeY(Double_t Y);
    void ChangeU(Double_t U);
    void ChangeV(Double_t V);
    void ChangeZ(Double_t Z);
    void ChangeT(Double_t T);
    void RotateZDeg(Double_t degrees);
    void RotateZRad(Double_t radians);

    // Convenience functions
    friend Double_t Distance(const EXOCoordinates& Coord1, const EXOCoordinates& Coord2);
    Double_t PolarRadius() const;
    Double_t PolarAngleDeg() const;
    Double_t PolarAngleRad() const;

  protected:

    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fT;

  ClassDef(EXOCoordinates,1) // Increment this number whenever the data members of EXOCoordinates are changed!

};

#endif
