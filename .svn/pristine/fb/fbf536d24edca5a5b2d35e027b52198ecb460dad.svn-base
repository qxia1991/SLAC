#include "EXOCalibUtilities/EXOLightmapCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TH3F.h"
#include "TMath.h"
#include <string>
#include <iostream>

IMPLEMENT_EXO_CALIB_HANDLER(EXOLightmapCalibHandler)

EXOCalibBase* EXOLightmapCalibHandler::readDefault(const std::string& dataIdent, const std::string& formatVersion)
{
  EXOLightmapCalib* newCalib = new EXOLightmapCalib;

  newCalib->h_lightmap = new TH3F("h_lightmap",
			      "Light Map",
			      1, 0, ACTIVE_XENON_RADIUS,
			      1, -TMath::Pi(), TMath::Pi(),
			      1, -CATHODE_ANODE_DISTANCE, CATHODE_ANODE_DISTANCE);
  newCalib->h_lightmap->SetBinContent(13, 1); // for a TH3F with one bin, it's number is 13; others are overflow

  newCalib->h_lightmap_errors = new TH3F("h_lightmap_errors",
			      "Light Map Errors",
			      1, 0, ACTIVE_XENON_RADIUS,
			      1, -TMath::Pi(), TMath::Pi(),
			      1, -CATHODE_ANODE_DISTANCE, CATHODE_ANODE_DISTANCE);
  newCalib->h_lightmap_errors->SetBinContent(13, 1);

  newCalib->h_lightmap_counts = new TH3F("h_lightmap_counts",
			      "Light Map Counts",
			      1, 0, ACTIVE_XENON_RADIUS,
			      1, -TMath::Pi(), TMath::Pi(),
			      1, -CATHODE_ANODE_DISTANCE, CATHODE_ANODE_DISTANCE);
  newCalib->h_lightmap_counts->SetBinContent(13, 1);

  newCalib->signal_to_use = "AllGangSum";
  
  return newCalib;
}

EXOCalibBase* EXOLightmapCalibHandler::readDB(const std::string& dataIdent, const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager will own it.
  EXOLightmapCalib* newData = new EXOLightmapCalib;

  // Create a vector of column headings to be fetched.
  std::vector <std::string> getColumns;
  getColumns.push_back("LightmapHistogram");
  getColumns.push_back("ErrorsHistogram");
  getColumns.push_back("CountsHistogram");
  getColumns.push_back("SignalToUse");

  std::string where("where id = '");
  where += dataIdent.substr(dataIdent.find_last_of(':')+1);
  where.push_back('\''); // where = "where id = '#'"
  RowResults res = GetDBRowsWith("lightmap", getColumns, getColumns, where);

  // read values that have just been fetched.
  for(unsigned int i=0; i<res.getNRows(); i++) {
    std::vector <std::string> readline;
    res.getRow(readline,i);

    // Read light map
    TObject *unserialized;

    unserialized = EXOMiscUtil::UnserializeString(readline[0]);
    if (not unserialized) {
      LogEXOMsg("Grabbing the light map failed.", EEAlert);
      delete newData;
      return NULL;
    }
    newData->h_lightmap = dynamic_cast<TH3F*>(unserialized);
    std::cout << "Using lightmap with title: " << newData->h_lightmap->GetTitle() << std::endl;

    unserialized = EXOMiscUtil::UnserializeString(readline[1]);
    if (not unserialized) {
      LogEXOMsg("Grabbing the errors on the light map failed.", EEAlert);
      delete newData;
      return NULL;
    }
    newData->h_lightmap_errors = dynamic_cast<TH3F*>(unserialized);
    std::cout << "Using lightmap errors with title: " << newData->h_lightmap_errors->GetTitle() << std::endl;

    unserialized = EXOMiscUtil::UnserializeString(readline[2]);
    if (not unserialized) {
      LogEXOMsg("Grabbing the counts contributing to the light map failed.", EEAlert);
      delete newData;
      return NULL;
    }
    newData->h_lightmap_counts = dynamic_cast<TH3F*>(unserialized);
    std::cout << "Using lightmap counts with title: " << newData->h_lightmap_counts->GetTitle() << std::endl;

    newData->signal_to_use = readline[3];
  }

  newData->normalize_corrections();
  newData->compute_z_axis_lightmap();

  return newData;
}

void EXOLightmapCalib::normalize_corrections()
{
  // normalize the corrections to be centered around 1
  Double_t sum = h_lightmap->Integral();
  Double_t mean = sum / (h_lightmap->GetNbinsX() * 
			 h_lightmap->GetNbinsY() *
			 h_lightmap->GetNbinsZ());
  h_lightmap->Scale(1./mean);
}

void EXOLightmapCalib::compute_z_axis_lightmap()
{
  //computes the average of the lightmap bins closest to r=0 along
  //the z axis so that interpolation near r = 0 works properly and
  //is continuous across the axis

  z_axis_lightmap = new std::vector<Double_t>(h_lightmap->GetNbinsZ() + 2);
  z_axis_errors = new std::vector<Double_t>(h_lightmap->GetNbinsZ() + 2);
  for (Int_t i = 1; i <= h_lightmap->GetNbinsZ(); i++) {
    Double_t sum = 0;
    Double_t sum_sq = 0;
    for (Int_t j = 1; j <= h_lightmap->GetNbinsY(); j++) {
      sum += h_lightmap->GetBinContent(1, j, i);
      sum_sq += (h_lightmap->GetBinContent(1, j, i)*
		 h_lightmap->GetBinContent(1, j, i));
    }
    z_axis_lightmap->at(i) = sum/h_lightmap->GetNbinsY();
    z_axis_errors->at(i) = TMath::Sqrt(sum_sq/h_lightmap->GetNbinsY() - 
				       z_axis_lightmap->at(i)*z_axis_lightmap->at(i))/TMath::Sqrt(h_lightmap->GetNbinsY());
  }
}

EXOLightmapCalib::~EXOLightmapCalib()
{
  if (h_lightmap) delete h_lightmap;
  if (h_lightmap_errors) delete h_lightmap_errors;
  if (h_lightmap_counts) delete h_lightmap_counts;
  if (z_axis_lightmap) delete z_axis_lightmap;
  if (z_axis_errors) delete z_axis_errors;
}
