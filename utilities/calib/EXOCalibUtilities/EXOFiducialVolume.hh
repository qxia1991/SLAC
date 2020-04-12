#ifndef EXOFiducialVolume_hh
#define EXOFiducialVolume_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOUtilities/EXODimensions.hh"

#include <string>

class EXOFiducialVolumeHandler : public EXOCalibHandlerBase
{
public:
  EXOFiducialVolumeHandler(): EXOCalibHandlerBase() {};

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("fiducial-volume")
};

class EXOChargeCluster;
class EXOCoordinates;

class EXOFiducialVolume : public EXOCalibBase
{
  friend class EXOFiducialVolumeHandler;

  public:
    EXOFiducialVolume();

    static void SetUserRadialCut(double rad, double zmin, double zmax);
  static void SetUserHexCut(double hex, double zmin, double zmax, double rmax = REFLECTORINNERRAD);
    static void UseDatabase() {fUseDatabase = true;}
    static void RestrictToPhysicalRegion(bool val = true);

    double GetVolumeInMillimeters3() const;
    double CheckFiducialVolumeWithMC(unsigned long N, int seed = 0) const;
    bool IsFiducial(const EXOCoordinates& coord) const;
    bool IsFiducial(const EXOChargeCluster& cc) const;
    bool IsRadial() const;
    bool IsHexagonal() const;
    double GetRadialCut() const;
    double GetHexCut() const;
    double GetMaxZ() const;
    double GetMinZ() const;
  double GetRadialMax() const;
    std::string __str__() const;
    std::string GetFidVolString(const std::string& minZ, 
                                const std::string& maxZ,
                                const std::string& maxAbsX,
                                const std::string& maxAbsU,
                                const std::string& maxAbsV,
                                const std::string& maxRsq) const;

  private:
    double GetRadialOrHexCut() const;
    double SegmentArea(double r, double h) const;

    static bool fUseDatabase; //Whether to read from database or use user provided values

    bool   fIsRadial;  //Radial cut if true, Hexagonal cut if false. Read from database
    double fRadialOrHexCut;    //Cut on absolute value of X,U,V for hex cut, or on R for radial cut. Read from database
    double fMinZ;   //Read from database
    double fMaxZ;   //Read from database
    static bool fUserIsRadial; //Radial cut if true, Hexagonal cut if false. User provided

    static double fUserRadialOrHexCut; //Cut on absolute value of X,U,V for hex cut, or on R for radial cut. Read from database. User provided

    static double fUserMinZ; //User provided
    static double fUserMaxZ; //User provided
  
    static double fUserRadialMax; //User provided

    static bool fRestrictToPhysicalRegion; //If this is true, the fiducial volume is ALWAYS restricted to within the teflon reflector and wire grid
};

#endif /* EXOFiducialVolume_hh */
