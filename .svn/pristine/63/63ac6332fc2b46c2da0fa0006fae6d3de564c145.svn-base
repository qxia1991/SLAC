#ifndef EXOCoordinateKey_hh
#define EXOCoordinateKey_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOCoordinates_hh
#include "EXOUtilities/EXOCoordinates.hh"
#endif
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"

class EXOCoordinates;

class EXOCoordinateKey : public TObject
{

    enum ECoordinateStatusBits{
      kIsInitialized = BIT(14) // Stored in TObject; set if coordinates have been initialized.
    };

  public:

    // Constructors
    EXOCoordinateKey();
    EXOCoordinateKey(const EXOCoordinateKey& Coord);
    EXOCoordinateKey(EXOMiscUtil::ECoordinateSystem UVorXY, Double_t UorX, Double_t VorY, Double_t Z, Double_t T);

    // Modifiers
    void Clear(Option_t* opt = "");
    void SetCoordinates(EXOMiscUtil::ECoordinateSystem UVorXY, Double_t UorX, Double_t VorY, Double_t Z, Double_t T);
    EXOCoordinateKey& operator = (const EXOCoordinateKey& rhs);

    // Pixel size information.
    static Double_t GetPixellationSize() {return MCCHARGE_PIXEL_SIZE;}
    static Double_t GetPixellationTime() {return MCCHARGE_PIXEL_TIME;}
    Bool_t CheckVersion() const;

    // comparison
    Bool_t operator < (const EXOCoordinateKey& rhs) const;
    Bool_t operator == (const EXOCoordinateKey& rhs) const;
    Bool_t operator != (const EXOCoordinateKey& rhs) const {return not (*this == rhs);}
    Bool_t operator > (const EXOCoordinateKey& rhs) const {return (rhs < *this);}
    Bool_t operator <= (const EXOCoordinateKey& rhs) const {return not (*this > rhs);}
    Bool_t operator >= (const EXOCoordinateKey& rhs) const {return (rhs <= *this);}

    // Accessors
    EXOCoordinates GetCenter() const;
    EXOMiscUtil::ECoordinateSystem GetCoordinateSystem() const;
    Bool_t IsInitialized() const;

  protected:

    Int_t fUorX;
    Int_t fVorY;
    Int_t fZ;
    Int_t fT;

    EXOMiscUtil::ECoordinateSystem fCoordinateSystem;
    Double_t fMCChargePixelSize;
    Double_t fMCChargePixelTime;
    const Double_t kTolerance;

  ClassDef(EXOCoordinateKey,1) // Increment this number whenever the data members of EXOCoordinateKey are changed!

};

#endif
