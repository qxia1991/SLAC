#ifndef EXOMuonTrackFinder_hh
#define EXOMuonTrackFinder_hh

#include "EXOAnalysisManager/EXOTreeSaverModule.hh"
#include "EXOUtilities/EXOMuonData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOMiscUtil.hh"

class TH2I;
class TGraph;
class TTree;
class EXOEventData;
class TalkToManager;
class EXOChannelMap;

class MuonTrackChannelHelper
{
private:

  void ComputeChanAvg();
  void ComputeChanStd();

  void ComputeAPDSumAvg();
  void ComputeAPDSumStd();
  
  int index;
  int num;
  EXOMiscUtil::EChannelType channel_type;
  
  double avg;
  double std;
  
  EXOEventData *ED;
  const EXOChannelMap &CM;

  EXOIntWaveform SumWF; // Sum of all good APD waveforms (regardless of the value of num).
  void InitSumWF();

public:
  MuonTrackChannelHelper(EXOEventData *eventdata);
  MuonTrackChannelHelper(EXOEventData *eventdata, int n);
  ~MuonTrackChannelHelper() {};

  enum EMuonTrackEnums {
    kAPDSumTPC1 = 550,
    kAPDSumTPC2 = 551
  };
  void Reset();
  void SetNum(int n);
  
  int GetNum() { return num; };
  double GetAvg() { return avg; };
  double GetStd() { return std; };
  int GetAPDSumAtT(int t) {return SumWF.At(t);}

  bool isUWire() {return EXOMiscUtil::ChannelIsUWire(channel_type);}
  bool isVWire() {return EXOMiscUtil::ChannelIsVWire(channel_type);}
  bool isAPD() {return EXOMiscUtil::ChannelIsAPD(channel_type);}
  bool isGoodChan();

};

class TrackParams
{
public:
  TrackParams() { Reset() ;};
  ~TrackParams() {};

  void Reset();
  
  double eta;
  double r;

  int initial_t;
  int final_t;
  int final_t_on_track;

  int total_spots;  
  int spots_contributing;
};

class EXOMuonTrackFinder : public EXOTreeSaverModule
{

private :
  
  double GetWireThreshold(MuonTrackChannelHelper *chan);
  double GetWireThresholdUV(MuonTrackChannelHelper *chan);
  double GetAPDThreshold(MuonTrackChannelHelper *chan);

  bool isNoiseEvent();
  int GetFirstLightTime(int tpc);
  
  void AddToRawHist(TH2I *h, MuonTrackChannelHelper *chan);
  
  void HoughTransformSinglePoint(TH2I *h, MuonTrackChannelHelper *chan, double t);
  void DoHoughTransform(TH2I *h, MuonTrackChannelHelper *chan,
			TrackParams *track, TH2I *h_spot, double max_allowed_t);
  
  void FindTrackFromHough(TH2I *h, TrackParams *track);
  int GetTimeOfLastChargeOnTrack(TH2I *h_spots, TrackParams *track);
  void MakeTrack(TGraph *g, TrackParams *track, int start_chan);

  void GetSphericalAngles(TrackParams *u_track, TrackParams *v_track, int tpc, double &theta, double &phi);

  bool isMuon(TrackParams *track1, TrackParams *track2, int light_t);

  void LookForTracks(int start_chan_num, TrackParams *track, TH2I *h,
		     TH2I *hough, TH2I *spots, double max_allowed_t);

  void ResetHistogramsAndSetChannelNumbers(int tpc);

  void SaveHistogramsToFile(int tpc);

  void SaveMuonEventData(bool muon_in_tpc[2], int light_t);
  void SaveMCTrackData();

  EXOEventData *ED;
  EXOMuonData fMuonData;
  
  double n_wire_sigma;
  double n_wire_counts;
  double n_apd_sigma;
  double n_apd_counts;
  bool make_indv_hists;
  bool verbose;
  int min_light_dt;
  double drift_vel_tpc[2];

  TH2I *h_u;
  TH2I *h_u_spots;
  TH2I *h_u_hough;
  TrackParams *u_tracks[2];
  TGraph *u_track_graph;

  TH2I *h_v;
  TH2I *h_v_spots;
  TH2I *h_v_hough;
  TrackParams *v_tracks[2];
  TGraph *v_track_graph;

  int run_num;
  int run_muon_count;

  static const double min_eta;
  static const double max_eta;
  static const int eta_bins;
  static const int r_bins;
  static const double t_scale_factor;

  EXOTimingStatisticInfo fTimingInfo;

protected:

  std::string GetTreeName() { return "muon"; };
  std::string GetTreeDescription() { return "Tree for Muon Data"; };
  void SetupBranches(TTree *tree);

  void SetNWireSigma(double n) {if (n > -1) {n_wire_sigma = n; n_wire_counts=-1;}};
  void SetNWireCounts(double n) {if (n > -1) {n_wire_counts = n; n_wire_sigma=-1;}};
  void SetNAPDSigma(double n) {if (n > -1) {n_apd_sigma = n; n_apd_counts=-1;}};
  void SetNAPDCounts(double n) {if (n > -1) {n_apd_counts = n; n_apd_sigma=-1;}};
  void SetMinLightDt(int t) {min_light_dt = t;};
  void SetDriftSpeed(double s) {drift_vel_tpc[0] = s; drift_vel_tpc[1] = s;};
  void SetMakeIndivHists(bool b) {make_indv_hists = b;};
  void SetVerbose(bool b) {verbose = b;};

public :

  EXOMuonTrackFinder();
  ~EXOMuonTrackFinder();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *eventdata);
  EventStatus ProcessEvent(EXOEventData *eventdata);
  EventStatus EndOfRun(EXOEventData *eventdata);

  int TalkTo(EXOTalkToManager *tm);

  int ShutDown();

  DEFINE_EXO_ANALYSIS_MODULE( EXOMuonTrackFinder )

};
#endif
