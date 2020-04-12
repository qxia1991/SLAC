#ifndef EXODriftVelocityCalib_hh
#define EXODriftVelocityCalib_hh

#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOCalibBase.hh"
#include "EXOCalibUtilities/EXOCalibHandlerBase.hh"

class EXODriftVelocityCalib : public EXOCalibBase {
friend class EXODriftVelocityHandler;

private:

  class DriftVel {
    public:
      // If anyone ever cares about other values in the drift_velocity table -- GRID44, or whatever --
      // just add them here along with member functions to access them.
      // They get filled in the EXODriftVelocityHandler::read function.
      double drift_velocity_TPC1;
      double drift_velocity_TPC2;
      // collection between wireplanes
      double collection_velocity_TPC1;
      double collection_velocity_TPC2;
      double collection_time_TPC1;
      double collection_time_TPC2;
  };
  DriftVel m_DriftVelocityHolder;

public:

  double get_drift_velocity_TPC1() {return m_DriftVelocityHolder.drift_velocity_TPC1;}
  double get_drift_velocity_TPC2() {return m_DriftVelocityHolder.drift_velocity_TPC2;}

  double get_collection_velocity_TPC1() {return m_DriftVelocityHolder.collection_velocity_TPC1;} // collection velocity between wire planes.
  double get_collection_velocity_TPC2() {return m_DriftVelocityHolder.collection_velocity_TPC2;}
  double get_collection_time_TPC1() {return m_DriftVelocityHolder.collection_time_TPC1;} // should match collection time... believe in the DB.
  double get_collection_time_TPC2() {return m_DriftVelocityHolder.collection_time_TPC2;}

  EXODriftVelocityCalib() : EXOCalibBase() {}

};

class EXODriftVelocityHandler : public EXOCalibHandlerBase {

public:

  EXODriftVelocityHandler() : EXOCalibHandlerBase() {}

protected:
  virtual EXOCalibBase* readDB(const std::string& dataIdent,
                               const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("drift")

};

#endif
