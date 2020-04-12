//______________________________________________________________________________
// This module corrects for the position dependence of light collection
// Several signals are supported. Most often, the sum signal is used. For
// any signal type, a light correction map must be generated from source data.
//
// The light correction map is loaded from an external ROOT file. This
// can be generated with the generate_light_map program in the alpha
// folder in the the subversion repository.
//
// Steve Herrin
// 2011-09-21
//
// -o_o-o_o- Changelog -o_o-o_o-
// 2011-09-21    v1.0    Initial version with hardcoded correction
// 2011-10-16    v1.1    New corrections for new gains. Was made missing run 2447, though, so can be improved.
// 2011-10-25    v1.2    New corrections. Also normalize the corrections to one and don't do anything with the max light signal.
// 2011-10-25    v1.3    Now corrects multi- (charge) cluster events
// 2011-12-14    v1.4    Improved binning. Now uses sum of individual gangs to avoid plane sum pattern recognition threshold issues. Now works on full detector volume.
// 2012-01-21    v1.5    Now reads in the light map (stored as a histogram), including binning, etc. from a root file. Also some attempts to handle errors better, but might not be complete.
// 2012-01-30    v1.5.1  Fix talktomanager command so that non-standard locations for the light map file work.
// 2012-02-01    v1.6    Bug fix. Correction was being calculated wrong for events very close to the center.
// 2012-02-22    v1.7    Lightmap can now be grabbed from the calibration database 
// 2012-03-24    v1.8    Now correction should be continuous across z axis and lightmap is now default.
// 2013-01-17    v1.9    Ignore zero energy clusters and clusters without a valid position
// -o_o-o_o-           -o_o-o_o-

#include <iostream>
#include <vector>
#include "TH3F.h"
#include "TFile.h"
#include "TMath.h"
#include "TObjString.h"
#include "EXOAnalysisManager/EXOAPDPosCorrModule.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOLightmapCalib.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOAPDPosCorrModule, "apdposcorr")

EXOAPDPosCorrModule::EXOAPDPosCorrModule()
  : lightmap_filename("database"),
    lightmap_database_flavor("vanilla"),
    lightmap_skip_correction(false)
{}

void EXOAPDPosCorrModule::normalize_corrections()
{
  // normalize the corrections to be centered around 1
  Double_t sum = h_lightmap->Integral();
  Double_t mean = sum / (h_lightmap->GetNbinsX() * 
			 h_lightmap->GetNbinsY() *
			 h_lightmap->GetNbinsZ());
  h_lightmap->Scale(1./mean);
}



Int_t EXOAPDPosCorrModule::get_low_r_neighbor(Double_t r)
{
  // returns the bin number that has center immediately lower than r
  // returns 0 if the bin is right at the center and you have to use
  // the lowest bin on the symmetric half of the TPC

  TAxis *axis = h_lightmap->GetXaxis();
  Int_t n_r = axis->FindFixBin(r);
  Int_t n_r_low = 0;

  if (n_r == 0) {
    // if it's underflow, which shouldn't happen
    n_r_low = 0;
  } else if (n_r == axis->GetNbins() + 1) {
    // If it's overflow
    n_r_low = axis->GetNbins();
  } else if (r < axis->GetBinCenter(n_r)) {
    n_r_low = n_r - 1;
  } else {
    n_r_low = n_r;
  }

  return n_r_low;
}

Int_t EXOAPDPosCorrModule::get_low_phi_neighbor(Double_t phi)
{
  // returns the bin number that has center immediately lower than phi

  TAxis *axis = h_lightmap->GetYaxis();
  Int_t n_phi = axis->FindFixBin(phi);
  Int_t n_phi_low = 1;

  if (phi < axis->GetBinCenter(n_phi)) {
    n_phi_low = n_phi - 1;
  } else {
    n_phi_low = n_phi;
  }

  if (n_phi_low == 0) {
    // phi is cyclical
    n_phi_low = axis->GetNbins();
  }

  return n_phi_low;
}

Int_t EXOAPDPosCorrModule::get_low_z_neighbor(Double_t z)
{
  // returns the bin number that has center immediately lower than z

  TAxis *axis = h_lightmap->GetZaxis();
  Int_t n_z = axis->FindFixBin(z);
  Int_t n_z_low = 0;

  if (n_z == 0) {
    // if it's underflow
    n_z_low = 1;
  } else if (n_z == axis->GetNbins() + 1) {
    // If it's overflow
    n_z_low = axis->GetNbins();
  } else if (z < axis->GetBinCenter(n_z)) {
    if (n_z > 1) {
      n_z_low = n_z - 1;
    } else {
      // don't go into underflow bin
      n_z_low = 1;
    }
  } else {
    n_z_low = n_z;
  }

  return n_z_low;
}

Double_t EXOAPDPosCorrModule::get_r_weighting(Double_t r, Int_t n_r_low)
{
  // Return the weighting for interpolation
  // This is the fraction of the distance from the low r bin to the high
  // r bin that the distance from the low r bin to r covers

  Double_t weight = 0;

  TAxis *axis = h_lightmap->GetXaxis();

  if (n_r_low == 0) {
    // if the bin is near the center, low bin is on other TPC half
    // but its center has same magnitude r, so distance is 2x that
    // magnitude
    weight = 1 - (axis->GetBinCenter(1) - r)/(axis->GetBinCenter(1));
  } else if (n_r_low == axis->GetNbins()) {
    // assume everything between center of last r bin and maximum r
    // has the same correction. No outside points to interpolate between
    weight = 0;
  } else {
    weight = (r - axis->GetBinCenter(n_r_low))/(axis->GetBinCenter(n_r_low + 1)-axis->GetBinCenter(n_r_low));
  }
  return weight;
}



Double_t EXOAPDPosCorrModule::get_phi_weighting(Double_t phi, Int_t n_phi_low)
{
  // Return the weighting for interpolation
  // This is the fraction of the distance from the low phi bin to the high
  // phi bin that the distance from the low phi bin to rphicovers

  Double_t weight = 0;

  TAxis *axis = h_lightmap->GetYaxis();

  if (n_phi_low == axis->GetNbins()) {
    // phi is cyclic, so handle the wrap-around correctly
    if (phi >= axis->GetBinCenter(n_phi_low)) {
      weight = (phi - axis->GetBinCenter(n_phi_low))/(2*TMath::Pi()+axis->GetBinCenter(1)-axis->GetBinCenter(n_phi_low));
    } else {
      weight = (2*TMath::Pi() + phi - axis->GetBinCenter(n_phi_low))/(2*TMath::Pi()+axis->GetBinCenter(1)-axis->GetBinCenter(n_phi_low));
    }
  } else {
    // things are simpler here, so just do it the fast way
    weight = (phi - axis->GetBinCenter(n_phi_low))/(axis->GetBinCenter(n_phi_low + 1) - axis->GetBinCenter(n_phi_low));
  }
  return weight;
}



Double_t EXOAPDPosCorrModule::get_z_weighting(Double_t z, Int_t n_z_low)
{
  // Return the weighting for interpolation
  // This is the fraction of the distance from the low z bin to the high
  // z bin that the distance from the low z bin to z covers

  Double_t weight = 0;

  TAxis *axis = h_lightmap->GetZaxis();

  if ((n_z_low == 1) && (z < axis->GetBinCenter(n_z_low))) {
    // No outside points to interpolate between, so assume
    // correction is the same past bin center
    weight = 0;
  } else if ((n_z_low == axis->GetNbins()) && (z >= axis->GetBinCenter(n_z_low))){
    // Same case at the other end of the TPC
    weight = 0;
  } else {
    weight = (z - axis->GetBinCenter(n_z_low))/(axis->GetBinCenter(n_z_low + 1)-axis->GetBinCenter(n_z_low));
  }
  return weight;
}


Double_t EXOAPDPosCorrModule::get_signal(EXOScintillationCluster *sc, Double_t z) {
  // get the signal, based on the signal_to_use string

  if (signal_to_use.CompareTo("AllGangSum") == 0) {
    return sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth) + sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth);

  } else if (signal_to_use.CompareTo("AllGangsMinus163And167") == 0) {
    Double_t total = sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth) + sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth);
    EXOAPDSignal *sig = sc->GetAPDSignal(EXOAPDSignal::kGangFit, 163);
    if (sig) {total -= sig->fRawCounts;}
    sig = sc->GetAPDSignal(EXOAPDSignal::kGangFit, 167);
    if (sig) {total -= sig->fRawCounts;}
    return total;

  } else if (signal_to_use.CompareTo("PlaneSum") == 0) {
    return sc->GetCountsOnAPDPlane(EXOMiscUtil::kNorth) + sc->GetCountsOnAPDPlane(EXOMiscUtil::kSouth);

  } else if (signal_to_use.CompareTo("OppositePlane") == 0) {
    if (z > 0) {return sc->GetCountsOnAPDPlane(EXOMiscUtil::kSouth);}
    else {return sc->GetCountsOnAPDPlane(EXOMiscUtil::kNorth);}

  } else if (signal_to_use.CompareTo("OppositePlaneGangSum") == 0) {
    if (z > 0) {return sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth);}
    else {return sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth);}

  } else if (signal_to_use.CompareTo("SamePlane") == 0) {
    if (z > 0) {return sc->GetCountsOnAPDPlane(EXOMiscUtil::kNorth);}
    else {return sc->GetCountsOnAPDPlane(EXOMiscUtil::kSouth);}

  } else if (signal_to_use.CompareTo("SamePlaneGangSum") == 0) {
    if (z > 0) {return sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kNorth);}
    else {return sc->GetCountsSumOnAPDPlane(EXOMiscUtil::kSouth);}

  } else {
    LogEXOMsg("Not a valid signal_to_use!", EEError);
    return 1;
  }
}

Double_t EXOAPDPosCorrModule::get_signal_error(EXOScintillationCluster *sc, Double_t z) {
  // get the error on the signal, based on the signal_to_use string

  if (signal_to_use.CompareTo("AllGangSum") == 0) {
    return TMath::Sqrt(
            TMath::Power(sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth), 2) + 
            TMath::Power(sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth), 2));

  } else if (signal_to_use.CompareTo("AllGangsMinus163And167") == 0) {
    Double_t total = TMath::Power(sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth), 2) +
		     TMath::Power(sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth), 2);
    EXOAPDSignal *sig = sc->GetAPDSignal(EXOAPDSignal::kGangFit, 163);
    if (sig) {total -= sig->fCountsError*sig->fCountsError;}
    sig = sc->GetAPDSignal(EXOAPDSignal::kGangFit, 167);
    if (sig) {total -= sig->fCountsError*sig->fCountsError;}
    return TMath::Sqrt(total);

  } else if (signal_to_use.CompareTo("PlaneSum") == 0) {
    return TMath::Sqrt(TMath::Power(sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth), 2) + 
           TMath::Power(sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth), 2));

  } else if (signal_to_use.CompareTo("OppositePlane") == 0) {
    if (z > 0) {return sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth);}
    else {return sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth);}

  } else if (signal_to_use.CompareTo("OppositePlaneGangSum") == 0) {
    if (z > 0) {return sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth);}
    else {return sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth);}

  } else if (signal_to_use.CompareTo("SamePlane") == 0) {
    if (z > 0) {return sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kNorth);}
    else {return sc->GetCountsErrorOnAPDPlane(EXOMiscUtil::kSouth);}

  } else if (signal_to_use.CompareTo("SamePlaneGangSum") == 0) {
    if (z > 0) {return sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kNorth);}
    else {return sc->GetCountsSumErrorOnAPDPlane(EXOMiscUtil::kSouth);}

  } else {
    LogEXOMsg("Not a valid signal_to_use!", EEError);
    return 1;
  }
}



Double_t EXOAPDPosCorrModule::get_correction(Double_t r, Double_t z, Double_t phi, Double_t &error)
{
  // Do a linear interpolation to get correction for light collection efficiency
  // Also return the error on that interpolation
  Double_t correction = 0;
  Double_t sq_error = 0;

  // Get the neighboring bins
  Int_t n_r_lo = get_low_r_neighbor(r);
  Int_t n_z_lo = get_low_z_neighbor(z);
  Int_t n_phi_lo = get_low_phi_neighbor(phi);

  // Get the weights for how much of the interpolation
  // the high neighbor contributes
  Double_t r_weight = get_r_weighting(r, n_r_lo);
  Double_t z_weight = get_z_weighting(z, n_z_lo);
  Double_t phi_weight = get_phi_weighting(phi, n_phi_lo);

  // To save some writing, define the high bin numbers
  // and deal with the edge cases
  Int_t n_r_hi = n_r_lo + 1;
  if (n_r_hi > h_lightmap->GetNbinsX()) {
    n_r_hi = h_lightmap->GetNbinsX();
  }
  Int_t n_z_hi = n_z_lo + 1;
  if (n_z_hi > h_lightmap->GetNbinsZ()) {
    n_z_hi = h_lightmap->GetNbinsZ();
  }
  // cyclic coordinate
  Int_t n_bins_phi = h_lightmap->GetNbinsY();
  Int_t n_phi_hi = (n_phi_lo % n_bins_phi) + 1;

  if (n_r_lo == 0) {
    // handle the case close to the axis
    correction += (1-r_weight)*(1-z_weight)
      *z_axis_lightmap->at(n_z_lo);
    correction += (1-r_weight)*(  z_weight)
      *z_axis_lightmap->at(n_z_hi);

  } else { 
    correction += (1-r_weight)*(1-z_weight)*(1-phi_weight)
      *h_lightmap->GetBinContent(n_r_lo, n_phi_lo, n_z_lo);
    correction += (1-r_weight)*(  z_weight)*(1-phi_weight)
      *h_lightmap->GetBinContent(n_r_lo, n_phi_lo, n_z_hi);
    correction += (1-r_weight)*(1-z_weight)*(  phi_weight)
      *h_lightmap->GetBinContent(n_r_lo, n_phi_hi, n_z_lo);
    correction += (1-r_weight)*(  z_weight)*(  phi_weight)
      *h_lightmap->GetBinContent(n_r_lo, n_phi_hi, n_z_hi);
  }
  correction += (  r_weight)*(1-z_weight)*(1-phi_weight)
    *h_lightmap->GetBinContent(n_r_hi, n_phi_lo, n_z_lo);
  correction += (  r_weight)*(  z_weight)*(1-phi_weight)
    *h_lightmap->GetBinContent(n_r_hi, n_phi_lo, n_z_hi);
  correction += (  r_weight)*(1-z_weight)*(  phi_weight)
    *h_lightmap->GetBinContent(n_r_hi, n_phi_hi, n_z_lo);
  correction += (  r_weight)*(  z_weight)*(  phi_weight)
    *h_lightmap->GetBinContent(n_r_hi, n_phi_hi, n_z_hi);
  
  Double_t err_term;
  if (n_r_lo == 0) {
    err_term = (1-r_weight)*(1-z_weight)
      *z_axis_errors->at(n_z_lo);
    sq_error += err_term * err_term;
    err_term = (1-r_weight)*(  z_weight)
      *z_axis_errors->at(n_z_hi);
    sq_error += err_term * err_term;
  } else {
    err_term = (1-r_weight)*(1-z_weight)*(1-phi_weight)
      *h_errors->GetBinContent(n_r_lo, n_phi_lo, n_z_lo);
    sq_error += err_term * err_term;
    err_term = (1-r_weight)*(  z_weight)*(1-phi_weight)
      *h_errors->GetBinContent(n_r_lo, n_phi_lo, n_z_hi);
    sq_error += err_term * err_term;
    err_term = (1-r_weight)*(1-z_weight)*(  phi_weight)
      *h_errors->GetBinContent(n_r_lo, n_phi_hi, n_z_lo);
    sq_error += err_term * err_term;
    err_term = (1-r_weight)*(  z_weight)*(  phi_weight)
      *h_errors->GetBinContent(n_r_lo, n_phi_hi, n_z_hi);
    sq_error += err_term * err_term;
  }
  err_term = (  r_weight)*(1-z_weight)*(1-phi_weight)
    *h_errors->GetBinContent(n_r_hi, n_phi_lo, n_z_lo);
  sq_error += err_term * err_term;
  err_term = (  r_weight)*(  z_weight)*(1-phi_weight)
    *h_errors->GetBinContent(n_r_hi, n_phi_lo, n_z_hi);
  sq_error += err_term * err_term;
  err_term = (  r_weight)*(1-z_weight)*(  phi_weight)
    *h_errors->GetBinContent(n_r_hi, n_phi_hi, n_z_lo);
  sq_error += err_term * err_term;
  err_term = (  r_weight)*(  z_weight)*(  phi_weight)
    *h_errors->GetBinContent(n_r_hi, n_phi_hi, n_z_hi);
  sq_error += err_term * err_term;

  error = TMath::Sqrt(sq_error);
  return correction;
}



Double_t EXOAPDPosCorrModule::get_multicluster_correction(EXOScintillationCluster *sc, Double_t &error) 
{
  // For a multi charge cluster event, the light didn't originate
  // from one place.
  // So assume that the correction to apply is the average of the
  // corrections at the cluster locations, weighted by the energy
  // of each of those clusters
  // When ncl == 1, this reduces to the correction at its location

  Int_t ncl = sc->GetNumChargeClusters();

  std::vector<Double_t> cc_corrections(ncl, 0);
  std::vector<Double_t> cc_corr_errs(ncl, 0);
  std::vector<Double_t> cc_energies(ncl, 0);
  std::vector<Double_t> cc_ener_errs(ncl, 0);
 
  Double_t sum_cc_energy = 0;

  for (Int_t j = 0; j < ncl; j++) {
    // Loop through all their clusters and get their correction
    // and their energy (based on charge signal) for weighting
    EXOChargeCluster *cc = sc->GetChargeClusterAt(j);
    Double_t x = cc->fX;
    Double_t y = cc->fY;

    // skip the cluster if it doesn't have a valid position
    if (x <= -999 || y <= -999 || (TMath::Abs(cc->fZ) > CATHODE_APDFACE_DISTANCE)) {
      continue;
    }

    Double_t r = TMath::Sqrt(x*x+y*y);
    Double_t phi = TMath::ATan2(y, x);

    Double_t error = 0;
    Double_t correction = get_correction(r, cc->fZ, phi, error);

    cc_corrections.at(j) = correction;
    cc_corr_errs.at(j) = error;

    Double_t cc_energy;
    Double_t cc_energy_err;
    // Use the best available measure of charge energy
    if (cc->fPurityCorrectedEnergy > 0) {
      cc_energy = cc->fPurityCorrectedEnergy;
    } else if (cc->fCorrectedEnergy > 0) {
      cc_energy = cc->fCorrectedEnergy;
    } else {
      cc_energy = cc->fRawEnergy;
    }
    // No fPurityCorrectedEnergyError !
    if (cc->fCorrectedEnergyError > 0) {
      cc_energy_err = cc->fCorrectedEnergyError;
    } else {
      cc_energy_err = cc->fRawEnergyError;
    }

    if (cc_energy <= 0) {
      continue;
    }

    cc_energies.at(j) = cc_energy;
    sum_cc_energy += cc_energy;

    cc_ener_errs.at(j) = cc_energy_err;
  }

  Int_t n = cc_energies.size();

  Double_t sum_correction = 0;
  for (Int_t j = 0; j < n; j++) {
    // get the numerator of the weighted average correction

    sum_correction += cc_energies.at(j)*cc_corrections.at(j);
  }

  Double_t sum_sq_error = 0;
  for (Int_t j = 0; j < n; j++) {
    // add up all the errors in quadrature
    sum_sq_error += cc_ener_errs.at(j) / (sum_cc_energy * sum_cc_energy);
    sum_sq_error += cc_ener_errs.at(j)*cc_corrections.at(j) / (sum_correction * sum_correction);
    sum_sq_error += cc_energies.at(j)*cc_corr_errs.at(j) / (sum_correction * sum_correction);
  }

  Double_t correction = 0;
  if (sum_cc_energy > 0) {
    correction = sum_correction/sum_cc_energy;
  }
  error = correction*TMath::Sqrt(sum_sq_error);

  return correction;
}

void EXOAPDPosCorrModule::compute_z_axis_lightmap()
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



void EXOAPDPosCorrModule::load_lightmap_from_file(std::string filename)
{
  // load the light map from the specified root file

  lm_file = new TFile(filename.c_str(), "READ");

  if (lm_file->IsOpen()) {

    h_lightmap = (TH3F*)lm_file->Get("h_lightmap");
    h_errors = (TH3F*)lm_file->Get("h_errors");
    h_counts = (TH3F*)lm_file->Get("h_counts");
    signal_to_use = ((TObjString*)lm_file->Get("signal_to_use"))->String();
    normalize_corrections();
    compute_z_axis_lightmap();
    std::cout << "Using lightmap with title: " << h_lightmap->GetTitle() << std::endl;
    std::cout << "Using lightmap errors with title: " << h_errors->GetTitle() << std::endl;
    std::cout << "Using lightmap counts with title: " << h_counts->GetTitle() << std::endl;
  } else {
    LogEXOMsg("Couldn't find a lightmap file. Not correcting.", EEError);
  }
}


void EXOAPDPosCorrModule::load_lightmap_from_database(const EXOEventHeader &header)
{
  // load the light map from the database

  const EXOLightmapCalib* lightmap_from_database = GetCalibrationFor(EXOLightmapCalib,
								     EXOLightmapCalibHandler,
								     lightmap_database_flavor,
								     header);

  h_lightmap = (TH3F *)lightmap_from_database->GetLightmap();
  h_errors = (TH3F *)lightmap_from_database->GetLightmapErrors();
  h_counts = (TH3F *)lightmap_from_database->GetLightmapCounts();
  signal_to_use = lightmap_from_database->GetSignalToUse();
  z_axis_lightmap = lightmap_from_database->GetZAxisLightmap();
  z_axis_errors = lightmap_from_database->GetZAxisErrors();

}


int EXOAPDPosCorrModule::Initialize()
{
  std::cout << "Starting position-based APD light correction module." << std::endl;

  if (lightmap_filename.compare("database") != 0) {
    load_lightmap_from_file(lightmap_filename);
  }
  // unfortunately, need an event header to grab the lightmap from the database,
  // so we'll do that in ProcessEvent

  return 0;
}


EXOAnalysisModule::EventStatus EXOAPDPosCorrModule::ProcessEvent(EXOEventData *ED)
{

  if (lightmap_filename.compare("database") == 0) {
    // grab the lightmap from the database
    // it should already be normalized
    load_lightmap_from_database(ED->fEventHeader);
  }

  for (size_t i = 0; i < ED->GetNumScintillationClusters(); i++) {

    EXOScintillationCluster *sc = ED->GetScintillationCluster(i);

    // zero these out
    sc->fRawEnergy = 0;
    sc->fEnergyError = 0;

    Int_t ncl = sc->GetNumChargeClusters();

    if (ncl >= 1) {

      Double_t signal = get_signal(sc, sc->GetChargeClusterAt(0)->fZ);
      Double_t signal_error = get_signal_error(sc, sc->GetChargeClusterAt(0)->fZ);

      Double_t corr_error = 0;
      Double_t correction = get_multicluster_correction(sc, corr_error);

      if (lightmap_skip_correction){
          //Apply no correction (only for MC or for testing stuff)
          correction = 1.0;
          corr_error = 0.0;
      }

      if (correction > 0) {
	sc->fRawEnergy = signal/correction;
      }

      // no fRawEnergyError !?
      sc->fEnergyError = sc->fRawEnergy * TMath::Sqrt(corr_error*corr_error/(correction*correction) + signal_error*signal_error/(signal*signal));
    }
  }

  return kOk;
}


int EXOAPDPosCorrModule::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/apdposcorr/file",
 	          "The root file containing the lightmap (or database to grab from the database).",
 	          this,
 	          lightmap_filename,
 	          &EXOAPDPosCorrModule::set_lightmap_filename);

  talktoManager->CreateCommand("/apdposcorr/flavor",
 	          "The flavor to request from the database.",
 	          this,
 	          lightmap_database_flavor,
 	          &EXOAPDPosCorrModule::set_lightmap_database_flavor);

  talktoManager->CreateCommand("/apdposcorr/skipCorr",
                               "For MC you may want to skip correction if not put in to begin with.",
                               this,
                               lightmap_skip_correction,
                               &EXOAPDPosCorrModule::set_lightmap_skip_correction);
  return 0;
}

int EXOAPDPosCorrModule::ShutDown()
{

  if (lightmap_filename.compare("database") != 0 && lm_file) {
    lm_file->Close();
    delete lm_file;
  }

  std::cout << "Done with position-based APD light correction module" << std::endl;

  return 0;
}

void EXOAPDPosCorrModule::set_lightmap_filename(std::string filename)
{
  lightmap_filename = filename;
}

void EXOAPDPosCorrModule::set_lightmap_database_flavor(std::string flavor)
{
  lightmap_database_flavor = flavor;
}

void EXOAPDPosCorrModule::set_lightmap_skip_correction(bool val){
    lightmap_skip_correction = val;
}








