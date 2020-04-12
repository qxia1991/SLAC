#ifndef EXOLightmapCalibHandler_hh
#define EXOLightmapCalibHandler_hh

#include <string>
#include <vector>
#include "EXOCalibBase.hh"
#include "EXOCalibHandlerBase.hh"
#include "TH3F.h"

class EXOLightmapCalibHandler : public EXOCalibHandlerBase
{
public:
  EXOLightmapCalibHandler() : EXOCalibHandlerBase() {}

protected:
  EXOCalibBase* readDefault(const std::string& dataIdent,
                            const std::string& formatVersion);
  EXOCalibBase* readDB(const std::string& dataIdent,
                       const std::string& formatVersion);

  DEFINE_EXO_CALIB_HANDLER("lightmap")
};

class EXOLightmapCalib : public EXOCalibBase
{
public:

  TH3F* GetLightmap() const {return h_lightmap;};
  TH3F* GetLightmapErrors() const {return h_lightmap_errors;};
  TH3F* GetLightmapCounts() const {return h_lightmap_counts;};
  std::string GetSignalToUse() const {return signal_to_use;};
  std::vector<Double_t>* GetZAxisLightmap() const {return z_axis_lightmap;};
  std::vector<Double_t>* GetZAxisErrors() const {return z_axis_errors;};

  EXOLightmapCalib() : EXOCalibBase() {}
  ~EXOLightmapCalib();

private:
  void normalize_corrections();
  void compute_z_axis_lightmap();

  TH3F* h_lightmap;
  TH3F* h_lightmap_errors;
  TH3F* h_lightmap_counts;
  std::string signal_to_use;
  std::vector<Double_t> *z_axis_lightmap;
  std::vector<Double_t> *z_axis_errors;

  friend class EXOLightmapCalibHandler;
};
#endif
