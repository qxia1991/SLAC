
//_____________________________________________________________________________
// This module searches for muons based on the ionization tracks
// they leave in the xenon. It can also output histograms of the
// individual muon events. It stores the angular information
// for the muons in EXOEventData.

// Steve Herrin
// 2011-02-11

// -o_o-o_o- Changelog -o_o-o_o-
// 2011-02-17 Major refactoring to clean up code and put stuff into fuctions.
//            Also now uses muons that pass through cathode to calculate drift
//            velocity and records that and muon angles to the event data
// 2011-03-02 Now can look up drift velocity in the database. This is used for
//            determining muon angle and is separate from using muons to
//            determine drift velocity.
// 2011-03-08 Got rid of channel index stuff which isn't needed in the new
//            EXOEventData framework. Added protection in case the module
//            is run against events with no waveform data.
// 2011-06-24 Changed to new TalkTo Interface
// 2011-07-17 Now uses the noise tag flag for muon-like events that cross the
//            cathode, but are too vertical to actually cross the cathode.
//            Previously, the module just didn't tag them as muons.
// 2011-07-21 Fixed a boneheaded mistake in which I was only saving TPC 1
//            angles regardless of which half the muon was in. This resulted
//            in TPC 2 events having an angle of zero when written out.
//            Also changed the math on spherical angle reconstruction to be a
//            little cleaner. This doesn't affect the results significantly.
//            Also takes into account the drift time between u and v wries.
//            This is a tiny effect and also doesn't affect the results by
//            much.
// 2012-02-23 Relaxed criteria that was meant to reject bad reconstructions
//            that was inadvertently tossing out real muons.
// 2013-03-14 Made this into a TreeSaverModule that stores extra data about
//            the muons and possible muons in the event.
// 2013-04-09 Now checks if a channel is good before printing that it can't
//            find a waveform.
// -o_o-o_o- -o_o-o_o-

#include <vector>
#include "EXOAnalysisManager/EXOMuonTrackFinder.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOCalibUtilities/EXODriftVelocityCalib.hh"
#include "TH2I.h"
#include "TGraph.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TCanvas.h"

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOMuonTrackFinder, "muontrack" )

// o-o_o-o_o-o_o-o_o-o_o-o
//
// MuonTrackChannelHelper class
// Handles the details of channel numbering and some baseline calculations
//
// o-o_o-o_o-o_o-o_o-o_o-o

const int baseline_samples = 256;
const double EXOMuonTrackFinder::min_eta = -M_PI/2;
const double EXOMuonTrackFinder::max_eta = M_PI/2;
const int    EXOMuonTrackFinder::eta_bins = 1024;
const int    EXOMuonTrackFinder::r_bins = 256;
const double EXOMuonTrackFinder::t_scale_factor = 37.0/2048.0;



MuonTrackChannelHelper::MuonTrackChannelHelper(EXOEventData *eventdata) :
 ED(eventdata),
 CM(GetChanMapForHeader(ED->fEventHeader))
{
  ED = eventdata;
  Reset();
}



MuonTrackChannelHelper::MuonTrackChannelHelper(EXOEventData *eventdata, int n) : 
 ED(eventdata),
 CM(GetChanMapForHeader(ED->fEventHeader))
{
  SetNum(n);
}



void MuonTrackChannelHelper::Reset() {
  num = -1;
  index = -1;
  
  avg = 0;
  std = 0;
}

void MuonTrackChannelHelper::InitSumWF()
{
  // Initialize SumWF.
  bool MissingWaveforms = false;
  bool wf_init = false;
  int APD_START = NCHANNEL_PER_WIREPLANE*NWIREPLANE;
  for (int i=APD_START; i<APD_START + NUMBER_APD_CHANNELS_PER_PLANE; i++) {
    if (CM.good_channel(i)) {
      const EXOWaveform *wf = ED->GetWaveformData()->GetWaveformWithChannel(i);
      if (wf == NULL) {
	MissingWaveforms = true;
        continue;
      }
      if (not wf_init) { // Only do this once.
        SumWF.MakeSimilarTo(*wf);
        SumWF.Zero();
        wf_init = true;
      }
      SumWF += *wf;
    }
  }
  if(MissingWaveforms) LogEXOMsg("For at least one channel, couldn't get the waveform.", EEWarning);
}


void MuonTrackChannelHelper::ComputeAPDSumAvg() {
  int n;
  if (ED->GetWaveformData()->fNumSamples < baseline_samples) {
    n = ED->GetWaveformData()->fNumSamples;
  } else {
    n = baseline_samples;
  }

  int sum = 0;

  for (int t = 0; t < n; t++) {
    sum += GetAPDSumAtT(t);
  }

  avg = static_cast<double>(sum)/n;
}



void MuonTrackChannelHelper::ComputeAPDSumStd() {
  int n;
  if (ED->GetWaveformData()->fNumSamples < baseline_samples) {
    n = ED->GetWaveformData()->fNumSamples;
  } else {
    n = baseline_samples;
  }

  double sumsq = 0;

  for (int t = 0; t < n; t++) {

    double sum_all_chan = GetAPDSumAtT(t);

    sumsq += (sum_all_chan - avg) * (sum_all_chan - avg);
  }

  std = sqrt(sumsq/(n-1));
}


void MuonTrackChannelHelper::ComputeChanAvg() {
  int n;
  const EXOWaveform *wf = ED->GetWaveformData()->GetWaveformWithChannel(num);
  if (wf == NULL) {
    if (isGoodChan()) {
      LogEXOMsg("Couldn't get the waveform for a channel.", EEWarning);
    }
    avg = 0;
    return;
  }
  
  if ((int)wf->GetLength() < baseline_samples) {
    n = wf->GetLength();
  } else {
    n = baseline_samples;
  }
  
  avg = wf->Sum(0, n)/n;
}



void MuonTrackChannelHelper::ComputeChanStd() {
  int n;
  const EXOWaveform *wf = ED->GetWaveformData()->GetWaveformWithChannel(num);
  if (wf == NULL) {
    if (isGoodChan()) {
      LogEXOMsg("Couldn't get the waveform for a channel.", EEWarning);
    }
    std = 0;
    return;
  }

  if ((int)wf->GetLength() < baseline_samples) {
    n = wf->GetLength();
  } else {
    n = baseline_samples;
  }

  std = sqrt(wf->StdDevSquared<double>(0, n-1)/(n-1));
}



void MuonTrackChannelHelper::SetNum(int n) {
  num = n;
  channel_type = EXOMiscUtil::TypeOfChannel(num);
  if((not EXOMiscUtil::ChannelIsWire(channel_type)) and
     (not EXOMiscUtil::ChannelIsAPDGang(channel_type)) and
     (num == kAPDSumTPC1 or num == kAPDSumTPC2)) channel_type = EXOMiscUtil::kAPDSumOfGangs;
  
  if (isGoodChan()) {
    if ((num != kAPDSumTPC1) && (num != kAPDSumTPC2)) {
      ComputeChanAvg();
      ComputeChanStd();
    } else {
      InitSumWF();
      ComputeAPDSumAvg();
      ComputeAPDSumStd();
    }
  }
}

bool MuonTrackChannelHelper::isGoodChan()
{
  return (num == kAPDSumTPC1) || (num == kAPDSumTPC2) || (CM.good_channel(num));
}

// o-o_o-o_o-o_o-o_o-o_o-o
//
// TrackParams helper class
// Encapsulates information related to the tracks in the wires
//
// o-o_o-o_o-o_o-o_o-o_o-o

void TrackParams::Reset() {
  eta = 0.0;
  r = -1;
  total_spots = 0;
  spots_contributing = 0;
  initial_t = -1;
  final_t = -1;
  final_t_on_track = -1;
}

// o-o_o-o_o-o_o-o_o-o_o-o
//
// EXOMuonTrackFinder class
// Finds the tracks left by muons
//
// o-o_o-o_o-o_o-o_o-o_o-o

EXOMuonTrackFinder::EXOMuonTrackFinder()
{
  fTimingInfo.SetName("MuonTrackFinderStats");
  RegisterSharedObject(fTimingInfo.GetName(), fTimingInfo);
}

EXOMuonTrackFinder::~EXOMuonTrackFinder()
{
  RetractObject(fTimingInfo.GetName());
}


// Returns the threshold for a wire channel, based on
// the options specified by the user
double EXOMuonTrackFinder::GetWireThreshold(MuonTrackChannelHelper *chan){
  double threshold = 0;
  
  if (n_wire_sigma <= 0) {
    if (n_wire_counts <= 0) {
      threshold += 100;
      LogEXOMsg("Using default threshold of 100 counts since neither nwiresigma nor nwirecounts specified", EEWarning);
    } else {
      threshold += n_wire_counts;
    }
  } else {
    threshold += n_wire_sigma * chan->GetStd();
  }
  
  return threshold;
}


// Returns the threshold for an APD channel, based on
// the options specified by the user
double EXOMuonTrackFinder::GetAPDThreshold(MuonTrackChannelHelper *chan){
  double threshold = 0;
  
  if (n_apd_sigma <= 0) {
    if (n_apd_counts <= 0) {
      threshold += 100;
      LogEXOMsg("Using default threshold of 100 counts since neither napdsigma nor napdcounts specified", EEWarning);
    } else {
      threshold += n_apd_counts;
    }
  } else {
    threshold += n_apd_sigma * chan->GetStd();
  }
  
  return threshold;
}


// Decide if the event looks like (ringing) noise
bool EXOMuonTrackFinder::isNoiseEvent() {
  return ED->IsTaggedAsNoise();
}



// Returns the time of the first light signal above threshold on the channel
int EXOMuonTrackFinder::GetFirstLightTime(int tpc) {
  MuonTrackChannelHelper *chan = new MuonTrackChannelHelper(ED, (tpc == 0) ? MuonTrackChannelHelper::kAPDSumTPC1 : MuonTrackChannelHelper::kAPDSumTPC2 );

  double threshold = GetAPDThreshold(chan);
  double max = 0;

  int light_t = ED->GetWaveformData()->fNumSamples + 1;
  for (int t = baseline_samples;  t < ED->GetWaveformData()->fNumSamples; t++) {
    if (chan->GetAPDSumAtT(t) - chan->GetAvg() > threshold) {
      light_t = t;
      break;
    }
    if (chan->GetAPDSumAtT(t) - chan->GetAvg() > max) {
      max = chan->GetAPDSumAtT(t) - chan->GetAvg();
    }
  }

  delete chan;

  return light_t;
}



// Fill histogram h with the waveform data for a given channel
void EXOMuonTrackFinder::AddToRawHist(TH2I *h, MuonTrackChannelHelper *chan) {
  const EXOWaveform *wf = ED->GetWaveformData()->GetWaveformWithChannel(chan->GetNum());
  if (wf == NULL) {
    if (chan->isGoodChan()) {
      LogEXOMsg("Couldn't get the waveform for a channel.", EEWarning);
    }
    return;
  }
  for (size_t t = 0; t < wf->GetLength(); t++) {
    h->Fill(t, chan->GetNum(), wf->At(t) - chan->GetAvg());
  }
}



// Perform the hough transform on a single point and add the result to histogram h
void EXOMuonTrackFinder::HoughTransformSinglePoint(TH2I *h, MuonTrackChannelHelper *chan, double t) {

  // rescale t so all our angles aren't really small
  double scaled_t = t_scale_factor * t;

  for (int i = 0; i < (eta_bins); i++) {
    double eta = i*(max_eta - min_eta)/(eta_bins) + min_eta;

    h->Fill(eta, scaled_t*cos(eta) + (chan->GetNum() % 38)*sin(eta), 1);
  }
}



// Do the actual Hough transform on channel chan and store the results in h_hough and track
// If *h_spot is not null, add hot spots to this histogram, too
void EXOMuonTrackFinder::DoHoughTransform(TH2I *h_hough, MuonTrackChannelHelper *chan, TrackParams *track, TH2I *h_spot, double max_allowed_t) {
  
  double threshold = GetWireThreshold(chan);

  int spot_t = -1;
  double spot_max = 0;
  bool above_threshold = false;

  // go through channel in time
  // if we're more than threshhold above the baseline, start keeping track of
  // maximum signal. Once we fall below threshhold above the baseline, the hot
  // spot is the point in that peak with the highest signal.

  const EXOWaveform *wf = ED->GetWaveformData()->GetWaveformWithChannel(chan->GetNum());
  if (wf == NULL) {
    if (chan->isGoodChan()) {
      LogEXOMsg("Couldn't get the waveform for a channel.", EEWarning);
    }
    return;
  }
  for (int t = 256; t < (int)wf->GetLength(); t++) {
    
    above_threshold = false;
    
    int data = wf->At(t);
    
    // V wires go negative first
    if (chan->isUWire()) {
      if (data - chan->GetAvg() > threshold ) {
        above_threshold = true;
      }
    } else if (chan->isVWire()) {
      if (chan->GetAvg() - data > threshold ) {
        above_threshold = true;
      }
    } else {
      LogEXOMsg("Track finder doesn't work with APDs", EEError);
    }
 
    // if we're above threshold, look for peak
    if (above_threshold) {
      if (fabs(data - chan->GetAvg()) > spot_max) {
        spot_max = fabs(data - chan->GetAvg());
	spot_t = t;

        // record time of initial and final charge signal
	if ((track->initial_t == -1) || t < track->initial_t) {
          track->initial_t = t;
	}
	if (t > track->final_t) {
	  track->final_t = t;
	}
      }
    } else if (spot_t >= 0) {
      // if we've come off of threshold without recording the point yet
      if (spot_t <= max_allowed_t) {
	HoughTransformSinglePoint(h_hough, chan, spot_t);
	track->total_spots += 1;

	if (h_spot != NULL) {
	  h_spot->Fill(spot_t, chan->GetNum(), 1);
	}
      }

      spot_t = -1;
      spot_max = 0;
    }
  } // end of t for loop
  
  if (above_threshold && spot_t <= max_allowed_t) {
    // if we were in a hot spot when we ran out of samples
    HoughTransformSinglePoint(h_hough, chan, spot_t);
    track->total_spots += 1;
    if (h_spot != NULL) {
      h_spot->Fill(spot_t, chan->GetNum(), 1);
    }
  }
}


// Translates a pair of U and V tracks into a physical angle relative to the zenith
// stores the results in theta and phi
void EXOMuonTrackFinder::GetSphericalAngles(TrackParams *u_track, TrackParams *v_track, int tpc, double &theta, double &phi) {
  double u0;
  double u1;
  double v0;
  double v1;
  double t0;
  double t1;

  bool both_vertical = false;

  // Don't want to use a vertical track to find where the tracks intersect in time
  if (fabs(u_track->eta) > (max_eta - min_eta)/eta_bins ) {
    // arbitrary points in v
    v0 = 0;
    v1 = 37;

    // get the corresponding t values
    t0 = (v_track->r / cos(v_track->eta) - v0 * tan(v_track->eta))/t_scale_factor;
    t1 = (v_track->r / cos(v_track->eta) - v1 * tan(v_track->eta))/t_scale_factor;

    // the electrons have to drift from u to v
    //Double_t t_drift = (CATHODE_ANODE_x_DISTANCE - CATHODE_ANODE_y_DISTANCE)/(drift_vel_tpc[tpc]*1000);

    // But I pick points off of negative V signal, which is due to induction 
    // when the u wire collects, so I think it's slightly better to leave this at 0
    Double_t t_drift = 0;

    // get the corresponding u values for those t values
    // time(u wire signal) = time(v wire signal) + t_drift
    u0 = u_track->r/sin(u_track->eta) - (t0+t_drift)*t_scale_factor/tan(u_track->eta);
    u1 = u_track->r/sin(u_track->eta) - (t1+t_drift)*t_scale_factor/tan(u_track->eta);


  } else if (fabs(v_track->eta) > (max_eta - min_eta)/eta_bins ) {
    // arbitrary points in u
    u0 = 0;
    u1 = 37;

    // get the corresponding t values
    t0 = (u_track->r / cos(u_track->eta) - u0 * tan(u_track->eta))/t_scale_factor;
    t1 = (u_track->r / cos(u_track->eta) - u1 * tan(u_track->eta))/t_scale_factor;

    // the electrons have to drift from u to v
    //Double_t t_drift = (CATHODE_ANODE_x_DISTANCE - CATHODE_ANODE_y_DISTANCE)/(drift_vel_tpc[tpc]*1000);

    Double_t t_drift = 0;
    
    // get the corresponding v values for those t values
    // time(u wire signal) = time(v wire signal) + t_drift
    v0 = v_track->r/sin(v_track->eta) - (t0-t_drift)*t_scale_factor/tan(v_track->eta);
    v1 = v_track->r/sin(v_track->eta) - (t1-t_drift)*t_scale_factor/tan(v_track->eta);
  } else {
    both_vertical = true;
  }
  if (!both_vertical) {
    double x0;
    double x1;
    double y0;
    double y1;
    double z0;
    double z1;

    // formula from reconstruction:
    // v = (v_chan - n_channel_per_plane/2 +0.5)*channel_width
    // u = (u_chan - n_channel_per_plane/2 +0.5)*channel_width

    // x = v - u (in tpc 1)
    // x = u - v (in tpc 2)
    // y = (u + v)/sqrt(3)

    // So v = (v_chan - 19 + 0.5)*9.0
    //    u = (u_chan - 19 + 0.5)*9.0

    // x = (v_chan - u_chan)*9.0
    // y = (v_chan + u_chan - 38 + 1)*9.0/sqrt(3.0)

    // get x, y, and z in mm
    if (tpc == 0) {
      x0 = (v0 - u0)*9.0;
      x1 = (v1 - u1)*9.0;
    
      y0 = (v0 + u0 - 37)*9.0/sqrt(3.0);
      y1 = (v1 + u1 - 37)*9.0/sqrt(3.0);
    
      // since we only care about difference, this is okay
      // factor of 1000 since we're doing everything in microseconds
      z0 = (drift_vel_tpc[tpc]*1000) * -t0;
      z1 = (drift_vel_tpc[tpc]*1000) * -t1;
    } else {
      x0 = (u0 - v0)*9.0;
      x1 = (u1 - v1)*9.0;
    
      y0 = (v0 + u0 - 37)*9.0/sqrt(3.0);
      y1 = (v1 + u1 - 37)*9.0/sqrt(3.0);
    
      z0 = (drift_vel_tpc[tpc]*1000) * t0;
      z1 = (drift_vel_tpc[tpc]*1000) * t1;
    }


    // get the spherical polar angles for the vector from x0 to x1
    // (or vice versa)
    // We know y should be positive
    double x = x1 - x0;
    double y = y1 - y0;
    double z = z1 - z0;
    if (y0 > y1) {
      x = -x;
      y = -y;
      z = -z;
    }
    phi = -atan2(x, z); // this is so north is 0, east is +pi/2, etc.
    theta = acos(y / sqrt(x*x + y*y + z*z));

  } else {
    // if the tracks were both vertical in time, then the event was vertical
    theta = 0;
    phi = 0;
  }
}



// Get the track parameters from a hough transform histogram and store them in track
void EXOMuonTrackFinder::FindTrackFromHough(TH2I *h, TrackParams *track) {

  std::vector<double> etas(0);
  std::vector<double> rs(0);

  int max = static_cast<int>(floor(h->GetMaximum()));
  int n = 0;

  // scan over eta and r and look for bins that have hits equal to the maximum
  for (int binx = 1; binx <= h->GetNbinsX(); ++binx) {
    for (int biny = 1; biny <= h->GetNbinsY(); ++biny) {

      if (h->GetBinContent(binx, biny) == max) {
	etas.push_back(h->GetXaxis()->GetBinLowEdge(binx));
	rs.push_back(h->GetYaxis()->GetBinCenter(biny));
	++n;
      }
    }
  }

  // take the median
  // the arrays should already be sorted by eta, and most tracks should be a line in
  // the eta-r plane, so this isn't as nonsensical as it looks
  if (n % 2 == 1) {
    track->eta = etas[n/2];
    track->r = rs[n/2];
    track->spots_contributing = max;
  } else {
    track->eta = 0.5*(etas[n/2-1] + etas[n/2]);
    track->r = 0.5*(rs[n/2-1] + rs[n/2]);
    track->spots_contributing = 2*max;
  }
}


// Get the track parameters from a hough transform histogram and store them in track
int EXOMuonTrackFinder::GetTimeOfLastChargeOnTrack(TH2I *h_spots, TrackParams *track) {

  const double max_d = 1;

  double slope = -tan(track->eta)/t_scale_factor;
  double intercept = track->r/(cos(track->eta)*t_scale_factor);

  for (int binx = h_spots->GetNbinsX(); binx > 0; --binx) {
    for (int biny = 1; biny <= h_spots->GetNbinsY(); ++biny) {
      if (h_spots->GetBinContent(binx, biny) > 0) {
	int x = static_cast<int>(floor(h_spots->GetXaxis()->GetBinLowEdge(binx)));
	int y = static_cast<int>(floor(h_spots->GetYaxis()->GetBinLowEdge(biny)));

	if (fabs((y%38) - (x-intercept)/slope) <= max_d) {
	  track->final_t_on_track = x;
	  return x;
	}
      }
    }
  }
  return track->initial_t;
}

// if we want to draw a track on a histogram, puts it in g
void EXOMuonTrackFinder::MakeTrack(TGraph *g, TrackParams *track, int start_chan) {  
  int y[2] = {start_chan, start_chan + 37};
  
  for (int i = 0; i < 2; i++) {
    g->SetPoint(i, (-tan(track->eta)*(y[i] % 38)+track->r/cos(track->eta))/t_scale_factor, static_cast<double>(y[i]));
  }
}


// Decide if the tracks satisfy criteria to flag as a muon
bool EXOMuonTrackFinder::isMuon(TrackParams *track1, TrackParams *track2, int light_t) {

  if ((track1->total_spots < 3) || (track2->total_spots < 3)) {
    return false;
  }

  // if the track found doesn't really go well with the points
  if (track1->spots_contributing < 5
      && track1->spots_contributing <= ceil(static_cast<double>(track1->total_spots)/2)) {
    return false;
  }
  if (track2->spots_contributing < 5
      && track2->spots_contributing <= ceil(static_cast<double>(track2->total_spots)/2)) {
    return false;
  }

  // if the drift time is way too long to go with the light signal
  if ((min_light_dt > -1)
      && (((track1->initial_t - light_t) > 150)
	  || ((track2->initial_t - light_t) > 150))) {
    return false;
  }

  // if there was no light signal
  if ((min_light_dt > -1)
      && ((track1->initial_t - light_t <min_light_dt)
	  && (track2->initial_t - light_t < min_light_dt))) {
    return false;
  }

  return true;
}


// For one set of wires on half of the chamber, look for muon tracks
void EXOMuonTrackFinder::LookForTracks(int start_chan_num, TrackParams *track, TH2I *h, TH2I *hough, TH2I *spots, double max_allowed_t) {

    for (int chan_num = start_chan_num; chan_num < NCHANNEL_PER_WIREPLANE+start_chan_num; chan_num++) {

      MuonTrackChannelHelper *chan = new MuonTrackChannelHelper(ED, chan_num);
      if (chan->isGoodChan()) {      
	if (make_indv_hists) {
	  AddToRawHist(h, chan);
	  DoHoughTransform(hough, chan, track, spots, max_allowed_t);
	} else {
	  DoHoughTransform(hough, chan, track, spots, max_allowed_t);
	}
      }

      delete chan;
    }
}


// Reset the histograms for the next event
// Also set the channel numbers depending on the half of the tpc
void EXOMuonTrackFinder::ResetHistogramsAndSetChannelNumbers(int tpc) {
  u_tracks[tpc]->Reset();
  v_tracks[tpc]->Reset();

  // reset the u wire histograms
  h_u->Reset();
  h_u_spots->Reset();
  h_u_hough->Reset();

  u_track_graph->SetPoint(0, 0, 0);
  u_track_graph->SetPoint(1, 0, 0);
  v_track_graph->SetPoint(0, 0, 0);
  v_track_graph->SetPoint(1, 0, 0);

  // set the channel numbers properly
  int nsamples = ED->GetWaveformData()->fNumSamples;
  if (tpc == 1) {
    h_u->SetBins(nsamples, 0, nsamples, 38, 76, 114);
    h_u_spots->SetBins(nsamples, 0, nsamples, 38, 76, 114);
  } else {
    h_u->SetBins(nsamples, 0, nsamples, 38, 0, 38);
    h_u_spots->SetBins(nsamples, 0, nsamples, 38, 0, 38);
  }

  // reset the v wire histograms
  h_v->Reset();
  h_v_spots->Reset();
  h_v_hough->Reset();

  // set bins to the right channel numbers
  if (tpc == 1) {
    h_v->SetBins(nsamples, 0, nsamples, 38, 114, 152);
    h_v_spots->SetBins(nsamples, 0, nsamples, 38, 114, 152);
  } else {
    h_v->SetBins(nsamples, 0, nsamples, 38, 38, 76);
    h_v_spots->SetBins(nsamples, 0, nsamples, 38, 38, 76);
  }
}


// Save the histograms for a given event to a root and pdf file
void EXOMuonTrackFinder::SaveHistogramsToFile(int tpc) {
  char root_filename[64];
  char image_filename[64];
  sprintf(root_filename, "muon_run_%05d_ev_%06d_tpc_%d.root", ED->fRunNumber, ED->fEventNumber, tpc+1);
  sprintf(image_filename, "muon_run_%05d_ev_%06d_tpc_%d.pdf", ED->fRunNumber, ED->fEventNumber, tpc+1);
  if (verbose) {
    std::cout << "  Making plots." << std::endl;
  }
  
  gROOT->SetStyle("Plain");
  gROOT->SetBatch(kTRUE);
  TDirectory *old_dir = gDirectory;
  TFile *f = new TFile(root_filename, "RECREATE");
	
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c_tracks");
  c->Divide(2,3);
  c->cd(1);
  h_u->Draw("COLZ");
  c->cd(3);
  h_u_spots->Draw("BOX");
  u_track_graph->Draw("L");
  c->cd(5);
  h_u_hough->Draw("COLZ");
	
  c->cd(2);
  h_v->Draw("COLZ");
  c->cd(4);
  h_v_spots->Draw("BOX");
  v_track_graph->Draw("L");
  c->cd(6);
  h_v_hough->Draw("COLZ");
	
  c->cd(0);

  c->Print(image_filename);

  c->Write();

  h_u->Write();
  h_u_spots->Write();
  h_u_hough->Write();
  u_track_graph->Write("u_track_graph");

  h_v->Write();
  h_v_spots->Write();
  h_v_hough->Write();
  v_track_graph->Write("v_track_graph");

  f->Close();

  old_dir->cd();

  delete c;
  delete f;
}

void EXOMuonTrackFinder::SaveMCTrackData() {
  // We don't have the direction variables on the trunk EXOEventData
  // so just put dummy values for now. 
  //if (ED->fEventHeader.fIsMonteCarloEvent) {
  //  double x = -ED->fMonteCarloData.fPrimaryEventDirectionX;
  //  double y = -ED->fMonteCarloData.fPrimaryEventDirectionY;
  //  double z = -ED->fMonteCarloData.fPrimaryEventDirectionZ;
  //  fMuonData.fMonteCarloPhi = -atan2(x, z); // this is so north is 0, east is +pi/2, etc.
  //  fMuonData.fMonteCarloTheta = acos(y / sqrt(x*x + y*y + z*z));
  //}
  fMuonData.fMonteCarloPhi = -999;
  fMuonData.fMonteCarloTheta = -999;
}


void EXOMuonTrackFinder::SaveMuonEventData(bool muon_in_tpc[2], int light_t) {

  double theta[2] = {0., 0.};
  double phi[2] = {0., 0.};

  for (int tpc=0; tpc<2; tpc++) {
    if (muon_in_tpc[tpc] || (u_tracks[tpc]->spots_contributing > 3 || 
			     v_tracks[tpc]->spots_contributing > 3)) {
      EXOMuonTrack *track = fMuonData.GetNewMuonTrack();

      track->fMaximumDriftTime = u_tracks[tpc]->final_t_on_track - light_t;
      if (track->fMaximumDriftTime < 0) {
	track->fMaximumDriftTime = 0;
      }
      track->fNumUHits = u_tracks[tpc]->total_spots;
      track->fNumUHitsOnTrack = u_tracks[tpc]->spots_contributing;
      track->fNumVHits = v_tracks[tpc]->total_spots;
      track->fNumVHitsOnTrack = v_tracks[tpc]->spots_contributing;
      if (tpc == 0) {
	track->fTPCHalf = EXOMuonTrack::kTPC1;
      } else if (tpc == 1) {
	track->fTPCHalf = EXOMuonTrack::kTPC2;
      } else {
	track->fTPCHalf = EXOMuonTrack::kUndefined;
      }

      if (muon_in_tpc[tpc]) {
	GetSphericalAngles(u_tracks[tpc], v_tracks[tpc], tpc, theta[tpc], phi[tpc]);

	track->fTheta = theta[tpc];
	track->fPhi = phi[tpc];

	if (verbose) {
	  std::cout << "Found possible muon in run " << ED->fRunNumber 
		    << ", event " << ED->fEventNumber
		    << ", tpc half " << tpc+1 << std::endl;
	  if (min_light_dt > -1) {
	    std::cout << "  Light time: " << light_t 
		      << " Initial charge time: u-wire: " << u_tracks[tpc]->initial_t
		      << " v-wire: " << v_tracks[tpc]->initial_t
		      << " Final charge time: u-wire: " << u_tracks[tpc]->final_t_on_track
		      << " v-wire: " << v_tracks[tpc]->final_t_on_track << std::endl;
	  }
	  std::cout << "  Incident spherical angles are theta = "
		    << theta[tpc] << ", phi = " << phi[tpc] << std::endl;
	}
      } else {
	track->fTheta = -999;
	track->fPhi = -999;
      }

    }
  }


  EXOEventHeader& header = ED->fEventHeader;
  if (muon_in_tpc[0] && muon_in_tpc[1]) {
   
    // veto events in which a vertical track somehow registers in both halves of the tpc (i.e. it has to be noise)
    double cut_eta = 3*(max_eta - min_eta)/eta_bins;
    if ((fabs(u_tracks[0]->eta) <= cut_eta) && (fabs(v_tracks[0]->eta) <= cut_eta) 
	&& (fabs(u_tracks[1]->eta) <= cut_eta) && (fabs(v_tracks[1]->eta) <= cut_eta)) {

      header.fTaggedAsNoise = true;
      if (verbose) {
	std::cout << "The muons seen in event " << ED->fEventNumber
		  << " are probably actually electronics noise. Not tagging this event as a muon." 
		  << std::endl;
      }
    } else {

      header.fTaggedAsMuon = true;
      run_muon_count += 1;

      // average the drift velocity in the two tpcs
      header.fMuonDriftVelocity = CATHODE_ANODE_x_DISTANCE/2 * 
	(1./(u_tracks[0]->final_t_on_track - light_t) + 1./(u_tracks[1]->final_t_on_track - light_t));
      
      if (verbose) {
	std::cout << "  Drift velocity: " << ED->fEventHeader.fMuonDriftVelocity
		  << " mm/microsecond" << std::endl;
      }

      // try to use the better track to determine angle
      // if they're tied, go with what tpc 0 says
      if ((u_tracks[0]->spots_contributing + v_tracks[0]->spots_contributing) >= 
	  (u_tracks[1]->spots_contributing + v_tracks[1]->spots_contributing)) {
	header.fMuonTheta = theta[0];
	header.fMuonPhi = phi[0];
	header.fMuonTime = light_t;
      } else {
	header.fMuonTheta = theta[1];
	header.fMuonPhi = phi[1];
	header.fMuonTime = light_t;
      }

    }
  } else if (muon_in_tpc[0]) {
    header.fTaggedAsMuon = true;
    run_muon_count += 1;
    header.fMuonTheta = theta[0];
    header.fMuonPhi = phi[0];
    header.fMuonTime = light_t;
  } else if (muon_in_tpc[1]) {
    header.fTaggedAsMuon = true;
    run_muon_count += 1;
    header.fMuonTheta = theta[1];
    header.fMuonPhi = phi[1];
    header.fMuonTime = light_t;
  }
}

int EXOMuonTrackFinder::Initialize()
{

  std::cout << "Initializing EXO Alpha Team Muon Track Finder as " << GetName() << std::endl;

  for (int tpc=0; tpc<2; tpc++) {
    u_tracks[tpc] = new TrackParams();
    v_tracks[tpc] = new TrackParams();
  }

  h_u = new TH2I("h_u","u wire signals", 2048, 0, 2048, 38, 0, 38);
  h_u_spots = new TH2I("h_u_spots","u wire hot spots", 2048, 0, 2048, 38, 0, 38);
  h_u_hough = new TH2I("h_u_hough","u wire hough transform", eta_bins + 1, -M_PI/2, M_PI/2, r_bins + 1, 0, 37);
  
  u_track_graph = new TGraph(2);
  u_track_graph->SetLineWidth(2);
  u_track_graph->SetLineColor(632);

  h_v = new TH2I("h_v","v wire signals", 2048, 0, 2048, 38, 0, 38);
  h_v_spots = new TH2I("h_v_spots","v wire hot spots", 2048, 0, 2048, 38, 0, 38);
  h_v_hough = new TH2I("h_v_hough","v wire hough transform", eta_bins + 1, -M_PI_2, M_PI_2, r_bins + 1, 0, 37);
  
  v_track_graph = new TGraph(2);
  v_track_graph->SetLineWidth(2);
  v_track_graph->SetLineColor(632);

  return 0;
}



EXOAnalysisModule::EventStatus EXOMuonTrackFinder::BeginOfRun(EXOEventData *eventdata) {
  run_num = 0;
  run_muon_count = 0;

  if (drift_vel_tpc[0] == -1) {
    // Shouldn't have to delete current_driftvelocity.  
    // calibManager should also have a pointer to it, and should recycle it.
    std::cout << "EXOMuonTrackFinder: querying calibManager for drift velocity." << std::endl;
    EXODriftVelocityCalib *current_driftvelocity = GetCalibrationFor(EXODriftVelocityCalib, 
                                                                     EXODriftVelocityHandler, 
                                                                     "vanilla", 
                                                                     eventdata->fEventHeader);
    if(current_driftvelocity) {
      drift_vel_tpc[0] = current_driftvelocity->get_drift_velocity_TPC1();
      drift_vel_tpc[1] = current_driftvelocity->get_drift_velocity_TPC2();

      std::cout << "EXOMuonTrackFinder: successfully retrieved drift velocity from database." << std::endl;
      std::cout << "Drift Velocity TPC1 = " << drift_vel_tpc[0] << std::endl;
      std::cout << "Drift Velocity TPC2 = " << drift_vel_tpc[1] << std::endl;
    }
    else { // the database read failed
      drift_vel_tpc[0] = DRIFT_VELOCITY;
      drift_vel_tpc[1] = DRIFT_VELOCITY;

      std::cout << "EXOMuonTrackFinder: database query was unsuccessful; using default drift velocity = " << DRIFT_VELOCITY << " mm/ns from EXODimensions.hh." << std::endl;
    }
  }
  return kOk;
}



EXOAnalysisModule::EventStatus EXOMuonTrackFinder::ProcessEvent(EXOEventData *eventdata)
{
  fMuonData.Clear("C");

  // I hate passing this around everywhere.
  ED = eventdata;

  if (ED->fEventHeader.fIsMonteCarloEvent) {
    SaveMCTrackData();
  }

  if (ED->GetWaveformData()->fNumSamples <= 0) {
    LogEXOMsg("No waveform data.", EEError);
    return kDrop;
  }

  if (isNoiseEvent()) {
    if (ED->fEventHeader.fIsMonteCarloEvent) {
      // So we don't miss out on the MC data
      FillTree(ED);
    }
    return kOk;
  }

  // default values for these
  EXOEventHeader& header = ED->fEventHeader;
  header.fTaggedAsMuon = false;
  header.fMuonDriftVelocity = -999.99;
  header.fMuonTheta = -999.99;
  header.fMuonPhi = -999.99;
  header.fMuonTime = -1;

  //fMuonData->ResetForReconstruction();

  // since we can't get run number from ED for the EndOfRun method
  if (run_num == 0) {
    run_num = ED->fRunNumber;
  }

  // keep track of what happens in both halves
  bool muon_in_tpc[2] = {false, false};

  int light_t = ED->GetWaveformData()->fNumSamples + 1;

  fTimingInfo.ResetTimerForTag("look_for_u_tracks");
  fTimingInfo.ResetTimerForTag("hough_u_tracks");
  fTimingInfo.ResetTimerForTag("last_charge_u_tracks");
  fTimingInfo.ResetTimerForTag("look_for_v_tracks");
  fTimingInfo.ResetTimerForTag("hough_v_tracks");
  fTimingInfo.ResetTimerForTag("last_charge_v_tracks");

  for (int tpc = 0; tpc < 2; tpc++) {

    ResetHistogramsAndSetChannelNumbers(tpc);
    u_tracks[tpc]->Reset();
    v_tracks[tpc]->Reset();


    int new_lt = GetFirstLightTime(tpc);
    if (new_lt < light_t) {
      light_t = new_lt;
    }
    double max_allowed_t = light_t + 1.25*CATHODE_ANODE_x_DISTANCE/(1000*drift_vel_tpc[tpc]);

    // first look for tracks in the u wires
    fTimingInfo.StartTimerForTag("look_for_u_tracks", false);
    LookForTracks(76*tpc, u_tracks[tpc], h_u, h_u_hough, h_u_spots, max_allowed_t);
    fTimingInfo.StopTimerForTag("look_for_u_tracks");
    
    // two points will give you a line no matter what
    // so only look for tracks if there are enough points
    // also dont look at events that are obviously noise
    if (u_tracks[tpc]->total_spots >= 3) {
      fTimingInfo.StartTimerForTag("hough_u_tracks", false);
      FindTrackFromHough(h_u_hough, u_tracks[tpc]);
      fTimingInfo.StopTimerForTag("hough_u_tracks");
      fTimingInfo.StartTimerForTag("last_charge_u_tracks", false);
      GetTimeOfLastChargeOnTrack(h_u_spots, u_tracks[tpc]);
      fTimingInfo.StopTimerForTag("last_charge_u_tracks");
      if (make_indv_hists) {
	MakeTrack(u_track_graph, u_tracks[tpc], 76*tpc);
      }

    }
      
    fTimingInfo.StartTimerForTag("look_for_v_tracks", false);
    LookForTracks(76*tpc+38, v_tracks[tpc], h_v, h_v_hough, h_v_spots, max_allowed_t); 
    fTimingInfo.StopTimerForTag("look_for_v_tracks");
      
    // two points will give you a line no matter what
    // so only look for a track if we have more than that
    if (v_tracks[tpc]->total_spots >= 3) {
      fTimingInfo.StartTimerForTag("hough_v_tracks", false);
      FindTrackFromHough(h_v_hough, v_tracks[tpc]);
      fTimingInfo.StopTimerForTag("hough_v_tracks");
      fTimingInfo.StartTimerForTag("last_charge_v_tracks", false);
      GetTimeOfLastChargeOnTrack(h_v_spots, v_tracks[tpc]);
      fTimingInfo.StopTimerForTag("last_charge_v_tracks");
      if (make_indv_hists) {
	MakeTrack(v_track_graph, v_tracks[tpc], 38 + 76*tpc);
      }
    }
    if (make_indv_hists && u_tracks[tpc]->total_spots + v_tracks[tpc]->total_spots > 0) {
	SaveHistogramsToFile(tpc);
    }
    if (isMuon(u_tracks[tpc], v_tracks[tpc], light_t)) {
      muon_in_tpc[tpc] = true;
    }
  }

  SaveMuonEventData(muon_in_tpc, light_t);

  FillTree(ED);
  
  return kOk;
}



EXOAnalysisModule::EventStatus EXOMuonTrackFinder::EndOfRun(EXOEventData *eventdata) {
  if (verbose) {
    std::cout << "Found " << run_muon_count << " muons in run " << run_num << std::endl;
  }

  return kOk;
}



int EXOMuonTrackFinder::TalkTo(EXOTalkToManager *talktoManager)
{
  talktoManager->CreateCommand("/muontrack/nwiresigma",
			       "number of sigma above baseline on wire to consider a hot spot",
			       this,
			       -1.0,
			       &EXOMuonTrackFinder::SetNWireSigma);

  talktoManager->CreateCommand("/muontrack/nwirecounts",
			       "number of counts above baseline on wire to consider a hot spot",
			       this,
			       80.0,
			       &EXOMuonTrackFinder::SetNWireCounts);

  talktoManager->CreateCommand("/muontrack/napdsigma",
			       "number of sigma above baseline on apd to consider a hot spot",
			       this,
			       -1.0,
			       &EXOMuonTrackFinder::SetNAPDSigma);


  talktoManager->CreateCommand("/muontrack/napdcounts",
			       "number of counts above baseline on apd to consider a hot spot",
			       this,
			       10000.0,
			       &EXOMuonTrackFinder::SetNAPDCounts);

  talktoManager->CreateCommand("/muontrack/indvhists",
			       "save histograms of individual muon candidates",
			       this,
			       false,
			       &EXOMuonTrackFinder::SetMakeIndivHists);


  talktoManager->CreateCommand("/muontrack/driftspeed",
			       "Drift speed in mm/nanosecond or -1 to use database",
			       this,
			       -1.0,
			       &EXOMuonTrackFinder::SetDriftSpeed);

  talktoManager->CreateCommand("/muontrack/verbose",
			       "print out information about the muons",
			       this,
			       false,
			       &EXOMuonTrackFinder::SetVerbose);

  talktoManager->CreateCommand("/muontrack/minlightdt",
			       "only tag events with a corresponding light signal dt before the beginning of the muon track (set to -1 to disable)",
			       this,
			       0,
			       &EXOMuonTrackFinder::SetMinLightDt);

  return 0;
}



int EXOMuonTrackFinder::ShutDown()
{
  for (int tpc=0; tpc < 2; tpc++) {
    delete u_tracks[tpc];
    delete v_tracks[tpc];
  }

  delete h_u;
  delete h_u_spots;
  delete h_u_hough;
  delete u_track_graph;
  
  delete h_v;
  delete h_v_spots;
  delete h_v_hough;
  delete v_track_graph;
  
  std::cout << "EXO Alpha Team Muon Track Finder finished running." << std::endl;
  return 0;
}


void EXOMuonTrackFinder::SetupBranches(TTree *tree) {

  tree->Branch("MuonBranch", &fMuonData);
}
