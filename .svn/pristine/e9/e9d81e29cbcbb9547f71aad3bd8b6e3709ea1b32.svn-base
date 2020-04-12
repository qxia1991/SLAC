//////////////////////////////////////////////////////////////////////
//
// This class is designed to perform three functions:
// 1. Store the fiducial cuts being used for an analysis
// 2. For a given event, determine if it falls inside the initialized fiducial volume
// 3. Determine the total volume of the initialized fiducial volume
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <cmath>
#include "TRandom3.h"
#include "EXOCalibUtilities/EXOFiducialVolume.hh"
#include "EXOUtilities/EXOChargeCluster.hh"
#include "EXOUtilities/EXOCoordinates.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
//#include "EXOUtilities/EXODimensions.hh"

using namespace std;

ClassImp(EXOFiducialVolume)

bool EXOFiducialVolume::fUseDatabase = true;
bool EXOFiducialVolume::fUserIsRadial = false;
double EXOFiducialVolume::fUserRadialOrHexCut = 162;
double EXOFiducialVolume::fUserMinZ = 5;
double EXOFiducialVolume::fUserMaxZ = 182;
bool EXOFiducialVolume::fRestrictToPhysicalRegion = true;

double EXOFiducialVolume::fUserRadialMax = REFLECTORINNERRAD;


EXOFiducialVolume::EXOFiducialVolume()
: fIsRadial(false),
  fRadialOrHexCut(162),
  fMinZ(5),
  fMaxZ(182)
{

}

void EXOFiducialVolume::RestrictToPhysicalRegion(bool val /*=true*/)
{
  //If val is true, the fiducial volume is restricted to the physical region.
  //I.e. everything outside the teflon reflector OR wire grid is considered
  //non-fiducial also if the cuts are set to larger values.
  //This is applied to IsFiducial() as well as GetVolumeInMillimeters3()

  fRestrictToPhysicalRegion = val;
}

void EXOFiducialVolume::SetUserRadialCut(double rad, double zmin, double zmax)
{
  //Set a radial cut with values in mm.
  //This overrides database values globally (for all EXOFiducialVolume objects).
  //To undo this (i.e. to use database values again) call UseDatabase()
  fUseDatabase = false;
  fUserIsRadial = true;
  fUserRadialOrHexCut = rad;
  fUserMinZ = zmin;
  fUserMaxZ = zmax;
  fUserRadialMax = rad;
}

void EXOFiducialVolume::SetUserHexCut(double hex, double zmin, double zmax, double rmax)
{
  //Set a hexagonal cut with values in mm.
  //This overrides database values globally (for all EXOFiducialVolume objects).
  //To undo this (i.e. to use database values again) call UseDatabase()
  fUseDatabase = false;
  fUserIsRadial = false;
  fUserRadialOrHexCut = hex;
  fUserMinZ = zmin;
  fUserMaxZ = zmax;
  if((rmax < 0) or (fRestrictToPhysicalRegion && rmax > REFLECTORINNERRAD))
    fUserRadialMax = REFLECTORINNERRAD;
  else
    fUserRadialMax = rmax;
}

double EXOFiducialVolume::GetVolumeInMillimeters3() const
{
  //Return the volume of the initialized fiducial cut in mm^3
  double r = GetRadialOrHexCut();
  double dz = GetMaxZ()-GetMinZ();
  if(IsRadial()){
    double circle = M_PI*r*r;
    double segment = 0.;
    if(fRestrictToPhysicalRegion){
      r = min(r,REFLECTORINNERRAD);
      circle = M_PI*r*r;
      if(r > WIREPLANE_RADIUS){
        //If the cut radius is larger than the inner circle of the wire grid hexagon
        //we need to subtract the circular segments that lie outside the hexagon
        double h = r - WIREPLANE_RADIUS;
        segment = SegmentArea(r,h);
      }
    }
    return 2. * (circle - 6*segment) * dz;
  }
  else{
    double a = r;
    double rmax = GetRadialMax();
    double d = a*2/sqrt(3);
    if(rmax < a) // if radial cut < apothem, just like radial cut
    {
      double circle = M_PI*rmax*rmax;
      double segment = 0.;
      if(fRestrictToPhysicalRegion){
        r = min(rmax,REFLECTORINNERRAD);
        circle = M_PI*r*r;
        if(r > WIREPLANE_RADIUS)
        {
          double h = r - WIREPLANE_RADIUS;
          segment = SegmentArea(r,h);
        }
      }
      return 2. * (circle - 6*segment) * dz;
    }
    else // radial cut > apothem, consider edges
    {
      if(rmax >= d) // if radial choice is larger than hexagon diagonal, returns to old FV cut
      {
        if(fRestrictToPhysicalRegion){
          r = min(a,WIREPLANE_RADIUS);
          if(d > REFLECTORINNERRAD){
            //If the corners of the cut hexagon lie outside the teflon radius
            //but the cut hexagon has smaller or same size as the grid hexagon,
            //we take the circle defined by the teflon reflector and subtract
            //the segments that lie outside the cut hexagon.
            double circle = M_PI*REFLECTORINNERRAD*REFLECTORINNERRAD;
            double h = REFLECTORINNERRAD - r;
            double segment = SegmentArea(REFLECTORINNERRAD,h);
            return 2. * (circle - 6*segment) * dz;
          }
        }
        // 2 TPCs * Area of hexagon * Length of volume
        return 2.* 6/sqrt(3)*r*r * dz;
      }
      else // must consider hexagon edges now: rmax > a but rmax < d
      {
        if(fRestrictToPhysicalRegion){
          r = min(a,WIREPLANE_RADIUS);
          rmax = min(rmax,REFLECTORINNERRAD);
        }
        double circle = M_PI*rmax*rmax;
        double h = rmax - r;
        double segment = SegmentArea(rmax,h);
        return 2. * (circle - 6*segment) * dz;
      }
    }    
  }
}

double EXOFiducialVolume::SegmentArea(double r, double h) const
{
  //Calculate area of circular segment with height h and radius r
  double segment = r*r*acos(1.-(h/r));
  segment -= sqrt((2*r*h)-(h*h)) * (r-h);
  return segment;
}

double EXOFiducialVolume::CheckFiducialVolumeWithMC(unsigned long N, int seed/* = 0*/) const
{
  //Determine the size of the fiducial volume using MC.
  //Shoot N points and use acceptance / rejection to determine the fiducial volume.
  //The returned value is in mm^3
  TRandom3 randgen(seed);
  int nInside = 0;
  double xrange = 400;
  double yrange = 400;
  double zrange = 500;
  for(unsigned long i=0; i<N; i++){
    double x = xrange*(randgen.Rndm()-0.5);
    double y = yrange*(randgen.Rndm()-0.5);
    double z = zrange*(randgen.Rndm()-0.5);
    EXOCoordinates coord(EXOMiscUtil::kXYCoordinates,x,y,z,0.0);
    if(IsFiducial(coord)){
      nInside++;
    }
  }
  return xrange*yrange*zrange*double(nInside)/double(N);
}

bool EXOFiducialVolume::IsFiducial(const EXOCoordinates& coord) const
{
  //Return whether given coordinate is inside the fiducial volume.
  double absZ = fabs(coord.GetZ());
  if(absZ > GetMaxZ() or absZ < GetMinZ()){
    return false;
  }
  if(IsRadial()){
    double R = sqrt(coord.GetX()*coord.GetX() + coord.GetY()*coord.GetY());
    //Check whether coordinate is in the provided boundaries
    bool val = R < GetRadialOrHexCut();
    if(not fRestrictToPhysicalRegion){
      return val;
    }
    //If we restrict the volume to physical regions we also need to check
    //whether the cooridinate is within the teflon reflector and wire grid
    return val and R < REFLECTORINNERRAD and fabs(coord.GetX()) < WIREPLANE_RADIUS and fabs(coord.GetU()) < WIREPLANE_RADIUS and fabs(coord.GetV()) < WIREPLANE_RADIUS;
  }
  else{
    //Check whether coordinate is in the provided boundaries
    bool val =  fabs(coord.GetX()) < GetRadialOrHexCut() and fabs(coord.GetU()) < GetRadialOrHexCut() and fabs(coord.GetV()) < GetRadialOrHexCut();
    if(not fRestrictToPhysicalRegion){
      return val;
    }
    //If we restrict the volume to physical regions we also need to check
    //whether the cooridinate is within the teflon reflector and wire grid
    double R = sqrt(coord.GetX()*coord.GetX() + coord.GetY()*coord.GetY());
    return val and R < REFLECTORINNERRAD and fabs(coord.GetX()) < WIREPLANE_RADIUS and fabs(coord.GetU()) < WIREPLANE_RADIUS and fabs(coord.GetV()) < WIREPLANE_RADIUS;
  }
  return false;
}

bool EXOFiducialVolume::IsFiducial(const EXOChargeCluster& cc) const
{
  //Return whether given charge cluster lies inside the fiducial volume
  EXOCoordinates coord(EXOMiscUtil::kUVCoordinates,cc.fU,cc.fV,cc.fZ,cc.fCollectionTime);
  return IsFiducial(coord);
}

bool EXOFiducialVolume::IsRadial() const
{
  //Return whether this cut is radial
  if(!fUseDatabase){
    return fUserIsRadial;
  }
  return fIsRadial;
}

bool EXOFiducialVolume::IsHexagonal() const
{
  //Return whether this cut is hexagonal
  return !IsRadial();
}

double EXOFiducialVolume::GetRadialOrHexCut() const
{
  if(!fUseDatabase){
    return fUserRadialOrHexCut;
  }
  return fRadialOrHexCut;
}

double EXOFiducialVolume::GetHexCut() const
{
  //Return the hexagonal cut (center to side) in mm. 
  if(IsRadial()){
    return 0;
  }
  return GetRadialOrHexCut();
}

double EXOFiducialVolume::GetRadialCut() const
{
  //Return the radial cut (radius) in mm. 
  if(!IsRadial()){
    return 0;
  }
  return GetRadialOrHexCut();
}

double EXOFiducialVolume::GetMinZ() const
{
  //Return the minimum |Z| allowed by the cut in mm.
  if(!fUseDatabase){
    return fUserMinZ;
  }
  return fMinZ;
}

double EXOFiducialVolume::GetMaxZ() const
{
  //Return the maximum |Z| allowed by the cut in mm.
  if(!fUseDatabase){
    return fUserMaxZ;
  }
  return fMaxZ;
}

double EXOFiducialVolume::GetRadialMax() const
{
  //Return the maximum R allowed by the cut in mm.
  if(!fUseDatabase){
    return fUserRadialMax;
  } 

  return REFLECTORINNERRAD;
}


std::string EXOFiducialVolume::GetFidVolString(const std::string& minZ, 
                                               const std::string& maxZ,
                                               const std::string& maxAbsX,
                                               const std::string& maxAbsU,
                                               const std::string& maxAbsV,
                                               const std::string& maxRsq

) const {
  std::ostringstream cut;
  cut << minZ << " > " << GetMinZ();
  cut << " && " << maxZ <<  " < " << GetMaxZ();

  double hexcut = WIREPLANE_RADIUS;
  if(not IsRadial()) hexcut = min(hexcut, GetRadialOrHexCut());

  cut << " && " << maxAbsX << " < " << hexcut;
  cut << " && " << maxAbsV << " < " << hexcut;
  cut << " && " << maxAbsU << " < " << hexcut;

  //double rcut = REFLECTORINNERRAD;
  double rcut = min(REFLECTORINNERRAD, GetRadialMax());
  if(IsRadial()) rcut = min(rcut, GetRadialOrHexCut());

  cut << " && " << maxRsq  << " < " << rcut*rcut;

  return "(" + cut.str() + ")";
}

string EXOFiducialVolume::__str__() const
{
  stringstream sstream;
  sstream << "-----------------------EXOFiducialVolume------------------------" << endl;
  sstream << "The fiducial volume is " << (IsRadial() ? "RADIAL " : "HEXAGONAL ") << "with the following dimensions:" << endl;
  sstream << (IsRadial() ? "R" : "Inner r") << "adius = " << GetRadialOrHexCut() << " mm" << endl;
  sstream << "Minimum |Z| = " << GetMinZ() << " mm" << endl;
  sstream << "Maximum |Z| = " << GetMaxZ() << " mm" << endl;
  sstream << "This is a volume of " << GetVolumeInMillimeters3()/1000. << " ccm" << endl;
  sstream << (fRestrictToPhysicalRegion ? "The volume was restricted to physical values. Above numbers might be misleading.\n" : "");
  sstream << "EXOFiducialVolume is set to use " << (fUseDatabase ? "database " : "user provided ") << "values" << endl;
  sstream << "----------------------------------------------------------------" << endl;
  return sstream.str();
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOFiducialVolumeHandler)

EXOCalibBase* EXOFiducialVolumeHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  //Return default values if the database can't be read

  // Once newData gets returned, EXOCalibManager owns it.
  EXOFiducialVolume* newData = new EXOFiducialVolume();
  return newData;
}

EXOCalibBase* EXOFiducialVolumeHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOFiducialVolume* newData = new EXOFiducialVolume();

  // parse dataIdent.  It should have form
  // TABLE_NAME:COLUMN_NAME:VALUE
  unsigned delim1  = dataIdent.find(':');
  unsigned delim2  = dataIdent.rfind(':');

  // if (delim1 == delim2) { we've got a problem }
  std::string tbl = std::string(dataIdent, 0, delim1);
  std::string col = std::string(dataIdent, delim1 + 1, delim2 - delim1 - 1);
  std::string val = std::string(dataIdent, delim2 + 1);

  // Select which columns to read from TABLE_NAME where COLUMN_NAME=VALUE
  std::vector<std::string> selectColumns, orderColumns;
  selectColumns.push_back("radial");
  selectColumns.push_back("R");
  selectColumns.push_back("minZ");
  selectColumns.push_back("maxZ");

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() > 1){
    LogEXOMsg("Received more rows than expected", EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  res.getRow(fields, 0);
  try {
    stringstream sstream;
    sstream.str(fields[0]);
    sstream >> newData->fIsRadial;
    newData->fRadialOrHexCut = EXOMiscUtil::stringToDouble(fields[1]);
    newData->fMinZ = EXOMiscUtil::stringToDouble(fields[2]);
    newData->fMaxZ = EXOMiscUtil::stringToDouble(fields[3]);
  }
  catch (EXOExceptWrongType ex) {
    stringstream message;
    message << "Bad row in calibration data: " << ex.what();
    LogEXOMsg(message.str(), EEError);
    delete newData;
    return 0;
  }
  return newData;
}

