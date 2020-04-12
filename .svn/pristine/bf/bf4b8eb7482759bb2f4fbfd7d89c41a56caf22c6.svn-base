#ifndef EXOATeamAPDReshaperModule_hh
#define EXOATeamAPDReshaperModule_hh

#include "EXOAnalysisModule.hh"
#include <string>
class TH1F;
class EXOAnalysisManger;
class EXOEventData;
class EXOTalkToManager;

class EXOATeamAPDReshaperModule : public EXOAnalysisModule 
{

private :

  // These pointers will reference data to be read-in from the command line

  std::string filter_param;
  int tau_param;
  double T_param;
  double RC1_param;
  double RC2_param;
  double CR1_param;
  double CR2_param;
  bool overwriteSignals_param;
  bool makeNoiseCorners_param;

  void Settau_param(int aval) { tau_param = aval; }
  void SetT_param(double aval) { T_param = aval; }
  void SetRC1_param(double aval) { RC1_param = aval; }
  void SetRC2_param(double aval) { RC2_param = aval; }
  void SetCR1_param(double aval) { CR1_param = aval; }
  void SetCR2_param(double aval) { CR2_param = aval; }
  void SetoverwriteSignals_param(bool aval) { overwriteSignals_param = aval; }
  void SetmakeNoiseCorners_param(bool aval) { makeNoiseCorners_param = aval; }


  double p; // renormalization factor
  double p_f; // gain of filter

protected:

public :

  EXOATeamAPDReshaperModule();

  int Initialize();
  EventStatus BeginOfRun(EXOEventData *ED);
  EventStatus ProcessEvent(EXOEventData *ED);
  int ShutDown();
  int TalkTo(EXOTalkToManager *tm);

  void SetFilterParam(std::string aval) { filter_param = aval; }
private :

  void FiniteCusp(double signal[], int length);
  void Triangular(double signal[], int length);
  void Gaussian(double signal[], int length);
  //void Median(double signal[], int length);
  void RC_CR(double signal[], int length);

  void CR_shaper(double signal[], int n, double tratio, double baseline );
  void RC_shaper(double signal[], int n, double tratio );
  void CR_unshaper(double signal[], int n, double tratio, double baseline );
  void RC_unshaper(double signal[], int n, double tratio );

  double Baseline(double signal[], int length, double trigger);
  double FindTrigger(int length, EXOEventData *_ED);
  double Max(double signal[], int length);
  double Min(double signal[], int length);
  double Amplitude(double signal[], int length, double trigger);
  double MeasureRMS(double signal[], int length, double trigger);


  class NoiseCorner
  {
  public:
    NoiseCorner();
    NoiseCorner(const std::string name);
    NoiseCorner(const std::string name, int length);
    ~NoiseCorner();
    int* GetShapingTimes();
    int GetShapingTime(int index);
    double* GetrmsValues();
    double GetrmsValue(int index);
    double* GetrmsErrors();
    double GetrmsError(int index);
    int GetArrayLength();
    void Add(int tau, double peakHeight);
    int GetBestTau();
    
  private:
    std::string cornername;
    int numTimes;
    int maxlength;
    int *shapingTimes;
    TH1F *hPeakHeights[25];
    double *rmsValues;
    double *rmsErrors;
    bool NotCalculated;
    void Calculate();
    void Init();
    int ArrayIndex(int tau);
  };


  class Filters : public NoiseCorner
  {
  public:
    Filters();
    Filters(std::string name);
    Filters(std::string name, int length);
    Filters(std::string name, int length, int Tlength);
    ~Filters();
    void Add(std::string filterType, int tau, double peakHeight);
    void Add(std::string filterType, int tau, double T, double peakHeight);
    int GetTArrayLength();
    int GetArrayLength(std::string filterType);
    int GetArrayLength(std::string filterType, double T);
    double GetBestT();
    int GetBestTindex();
    std::string GetBestShaper();
    int GetBestShaperTau();
    double GetBestShaperT();
    int* GetFilterShapingTimes(std::string filterType);
    int* GetFilterShapingTimes(std::string filterType, double T);
    double* GetFilterrmsValues(std::string filterType);
    double* GetFilterrmsValues(std::string filterType, double T);
    double* GetFilterrmsErrors(std::string filterType);
    double* GetFilterrmsErrors(std::string filterType, double T);
    
  private:
    int *tempval;
    double *tempval2;
    std::string filtername;
    int maxlength;
    double *TTimes;
    int numTTimes;
    void Init();
    NoiseCorner *triangular;
    NoiseCorner *gaussian;
    NoiseCorner *finiteCusp[25];
    int ArrayIndex(double T);
    bool BestNotCalculated;
    std::string bestfilter;
    int bestTau;
    double bestT;
    void CalculateBest();

    
  };

  class APDNoiseCorners : public Filters
  {
  public:
    APDNoiseCorners(std::string name, int length, int Tlength);
    ~APDNoiseCorners();
    void Add(int ch, std::string filterType, int tau, double peakHeight);
    void Add(int ch, std::string filterType, int tau, double T, double peakHeight);
    int GetTArrayLength(int ch);
    int GetArrayLength(int ch, std::string filterType);
    int GetArrayLength(int ch, std::string filterType, double T);
    double GetBestT(int ch);
    int GetBestTindex(int ch);
    std::string GetBestShaper(int ch);
    int GetBestShaperTau(int ch);
    double GetBestShaperT(int ch);
    int* GetFilterShapingTimes(int ch, std::string filterType);
    int* GetFilterShapingTimes(int ch, std::string filterType, double T);
    double* GetFilterrmsValues(int ch, std::string filterType);
    double* GetFilterrmsValues(int ch, std::string filterType, double T);
    double* GetFilterrmsErrors(int ch, std::string filterType);
    double* GetFilterrmsErrors(int ch, std::string filterType, double T);

  private:
    std::string noisename;
    Filters *channels[74];

  };

  DEFINE_EXO_ANALYSIS_MODULE( EXOATeamAPDReshaperModule )

};
#endif

  


