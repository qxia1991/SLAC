#ifndef EXOXe137Veto_hh
#define EXOXe137Veto_hh

#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOFiducialVolume.hh"
#include "EXOUtilities/EXOMiscUtil.hh"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

#include "TString.h"
#include "TRandom3.h"

class EXOXe137VetoHandler : public EXOCalibHandlerBase {

public:
  EXOXe137VetoHandler(): EXOCalibHandlerBase() {
    fDBisBland = true;
  }

  bool FlavorizeSQLDB(const std::string& flavor) ; // find all valid times for flavor and push them to DB 
  bool fDBisBland  ; // need to flavorize !

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);

  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("137Xe_veto")
};

typedef struct Xe137VetoTrigger {
  int fTime;
  double fEnergy;
  bool fIsMissingPosition;
  double fMinZ, fMaxZ, fMinY, fMaxY, fMinX, fMaxX;
  double fCenterX, fCenterY, fRadiusXY;

  void SetMembers(std::map<std::string,std::string>& vals){

    fTime = EXOMiscUtil::stringToInt(vals["EventTime"]);
    fEnergy = EXOMiscUtil::stringToDouble(vals["Energy"]);
    fIsMissingPosition = EXOMiscUtil::stringToInt(vals["IsMissingPosition"]);
    
    fMinZ = EXOMiscUtil::stringToDouble(vals["MinZ"]);
    fMaxZ = EXOMiscUtil::stringToDouble(vals["MaxZ"]);
    
    if(not fIsMissingPosition) // if has all position simply copy values to trigger list
    {
      fMinY = EXOMiscUtil::stringToDouble(vals["MinY"]);
      fMaxY = EXOMiscUtil::stringToDouble(vals["MaxY"]);
      fMinX = EXOMiscUtil::stringToDouble(vals["MinX"]);
      fMaxX = EXOMiscUtil::stringToDouble(vals["MaxX"]);
    }
    else // copy the values for skipping clusters without 2D position
    {
      double xskip = EXOMiscUtil::stringToDouble(vals["MinXSkip"]);
      double yskip = EXOMiscUtil::stringToDouble(vals["MinYSkip"]);
      
      if(xskip > 990 || yskip > 990)
      {
        fMinY = -999; fMaxY = 999;
        fMinX = -999; fMaxX = 999;          
      }
      else
      {
        fMinY = yskip; fMaxY = EXOMiscUtil::stringToDouble(vals["MaxY"]);
        fMinX = xskip; fMaxX = EXOMiscUtil::stringToDouble(vals["MaxX"]);
      }
    }
  }      
  
  void SetOtherMembers() {
    fCenterX = (fMaxX+fMinX)/2.;
    fCenterY = (fMaxY+fMinY)/2.;
    
    double xrad = (fMaxX-fMinX);
    double yrad = (fMaxY-fMinY);
    fRadiusXY = sqrt(xrad*xrad+yrad*yrad)/2.;
  }

  void SetAllMembers(std::map<std::string,std::string>& vals){
    SetMembers(vals);
    SetOtherMembers();
  }

  bool operator<(const Xe137VetoTrigger& vt) const { return fTime < vt.fTime; }
  
  void Print() {
    std::cout << "Event time: " << fTime << " with energy = " << fEnergy << " keV.\n";
    std::cout << "Is missing position? " << fIsMissingPosition << std::endl;
    std::cout << Form("(x,y,z): min = (%f,%f,%f) and max = (%f,%f,%f)\n",fMinX,fMinY,fMinZ,fMaxX,fMaxY,fMaxZ);
    std::cout << Form("XY: center = (%f,%f) and radius = %f\n",fCenterX,fCenterY,fRadiusXY);
  }


} Xe137VetoTrigger;

class EXOXe137Veto : public EXOCalibBase {
  friend class EXOXe137VetoHandler;

public:

  EXOXe137Veto();
  static std::vector<std::string> GetColumnNames(); // specify the column names in the DB or file

  static void UseDatabase() {fUseDatabase = true;}; // set to use DB
  static void ReadUserFile(std::string filename);  // read veto trigger list from user input file
  
  static void SetEnergyWindow(double e_min, double e_max); // set the energy window allowed for trigger events
  static void SetXe137VetoCuts(double time, double extend, bool useZ = true, bool useXY = true); // set options to veto events
  static void SetXe137VetoSpatialCuts(double extend, bool useZ = true, bool useXY = true); // set spatial options to veto events
  static void Require2DCluster(bool req = true); // set requirement of at least 1 2D cluster in trigger event
  static void IgnoreH1Line(bool ignore=true); // set the ignore trigger events within the H1 line
  bool IsXe137Vetoed(Int_t event_time, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const; // return whether to veto event
  bool RandXe137Vetoed(Int_t event_time, const EXOFiducialVolume& fidVol) const; // return whether to veto event by randomization of volume
  bool MCXe137Vetoed(double prob, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const; // return whether to veto MC event
  bool ConsiderTriggerEvent(const Xe137VetoTrigger& veto) const; // return whether trigger event is considered
  void CheckSpatialConditions(const Xe137VetoTrigger& veto, bool& is_vetoed_137Xe_z, bool& is_vetoed_137Xe_xy, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const;
  Int_t FindTriggerTime(Int_t event_time, bool fast = true, const std::vector<double>* cluster_z = NULL, const std::vector<double>* cluster_y = NULL, const std::vector<double>* cluster_x = NULL) const;

  std::vector<Xe137VetoTrigger>::const_iterator DrawXe137VetoTriggerTrigger() const;
  size_t GetXe137VetoTriggerListSize() const;
  const Xe137VetoTrigger* GetXe137VetoTrigger(size_t pos) const;
  
  std::string __str__() const;
  
private:

  static bool fUseDatabase; // whether to read from DB or user input file
  
  static std::vector<std::string> fColumnNames; // name of columns in DB or file (must be the same structure)

  static double fMinEnergy; // minimum energy of Xe137 veto event
  static double fMaxEnergy; // maximum energy of Xe137 veto event
  static bool fRequire2DCluster;// whether to consider Xe137 veto event w/o any 3D cluster (true requires 1 3D cluster)
  static double fTimeWindow; // max time to Xe137 veto trigger event
  static double fSpatialExtension;  // max distance to Xe137 veto trigger volume
  static bool fUseZ137XeVetoCut; // whether to consider Z positions to veto events
  static bool fUseXY137XeVetoCut;// whether to consider XY positions to veto events
  static bool fIgnoreH1Line; // whether to consider trigger events within 2.1-2.3 MeV 

  static TRandom3 fRandomGenerator;
  
  mutable std::vector<Xe137VetoTrigger> fDBXe137VetoTriggerList;  // selected list (read from DB) of Xe137 veto trigger events
  static std::vector<Xe137VetoTrigger> fUserXe137VetoTriggerList; // selected list (read from user file) of Xe137 veto trigger events

  mutable std::vector<Xe137VetoTrigger>* fXe137VetoTriggerList;   // pointer to choice of list (DB or file)
};

#endif
