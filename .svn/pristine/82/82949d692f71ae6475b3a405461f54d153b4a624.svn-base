#ifndef EXOMatchedFilterFinder_hh
#define EXOMatchedFilterFinder_hh

#include "EXOReconstruction/EXOVSignalFinder.hh"
#include "EXOReconstruction/EXOReconUtil.hh"

#include "EXOUtilities/EXOTemplWaveform.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOMatchedFilter.hh"
#include "EXOUtilities/EXOSavitzkyGolaySmoother.hh"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>
class EXOChannelSignals;
class TFile;
class TTree;


class EXOMatchedFilterFinder : public EXOVSignalFinder 
{

  public:
    EXOMatchedFilterFinder();  
    virtual EXOSignalCollection FindSignals(
	  const EXOReconProcessList& processList, 
	  const EXOSignalCollection& inputSignals) const;

    void SetDivideNoise(bool val = true);
    void SetFixedWireThreshold(double val);
    void SetFixedVWireThreshold(double val);
    void SetFixedAPDSumThreshold(double val);
    void SetWireThresholdFactor(double val);
    void SetVWireThresholdFactor(double val);
    void SetAPDSumThresholdFactor(double val);
    void SetWireSmoothWindow(int val);
    void SetAPDSmoothWindow(int val);
    void SetAPDSearchWindowBegin(int val);
    void SetAPDSearchWindowEnd(int val);
    void SetChargeClusterTime(int val); // This method set the cluster time, used as the reference point by APD search window.
    void SetNumThreads(int val);

    // These methods set noise PSD
    void SetWireNoisePowerSpectrum(bool val);
    void SetAPDNoisePowerSpectrum(bool val);
    void SetWireNoiseFilename(std::string val);
    void SetAPDNoiseFilename(std::string val);
    void SetDataTakingPhase(std::string val);
   
  

  protected:
    struct ChannelHelper {
      // Store the pieces of information which we'll need to pass around.
      EXODoubleWaveform fFilteredWF;
      double fThreshold;
      EXOChannelSignals fChannelSignals;
    };

    double GetThresholdFactor(EXOReconUtil::ESignalBehaviorType type) const;
    double CalculateNoiseCounts(const EXODoubleWaveform& wf) const;
    double CalculateNoiseCountsIteratively(const EXODoubleWaveform& wf) const;

    void PrepareMatchFilter(const EXOWaveform& waveform) const;
  
    void ApplyMatchFilter(ChannelHelper& chanHelper,
                          const EXOReconProcessList::WaveformWithType& wfWithType) const;

    void ProcessVerbosity(const EXOChannelSignals& sigs,
                          const EXODoubleWaveform& filtered_signal,
                          double threshold) const;


    void NotifySignalModelHasChanged(int chanOrTag, const EXOSignalModel& mod);

    void SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo);




  private:
    typedef std::map<int, EXOMatchedFilter> FilterMap;
    mutable FilterMap fFilters;
    double fUserWireThreshold; //Wire threshold value set by the user. Negative value means threshold is calculated automatically.
    double fUserVWireThreshold; //V-wire threshold value set by the user. Negative value means threshold is calculated automatically.
    double fUserAPDThreshold; //APD sum threshold value set by the user. Negative value means threshold is calculated automatically.
    double fWireThresholdFactor;
    double fVWireThresholdFactor;
    double fAPDSumThresholdFactor;
    int fWireSmoothWindow;
    int fAPDSmoothWindow;
 
    // Relevant to APD search window
    int fAPDSearchWindowBegin; // Set by talkto
    int fAPDSearchWindowEnd; // Set by talkto
    int fChargeClusterTime; 
    // Search window = [fChargeClusterTime - fAPDSearchWindowBegin, fChargeClusterTime + fAPDSearchWindowEnd]

    EXOSavitzkyGolaySmoother fWireSavGolFilter;
    EXOSavitzkyGolaySmoother fAPDSavGolFilter;
    bool fDivideNoise;
    int fNumThreads;  

    // attributes associated with applying real noise power spectrum to the filter
    bool fUseAPDRealNoise;
    bool fUseWireRealNoise;
    std::string fWireNoiseFilenameParam;
    std::string fAPDNoiseFilenameParam;
    TFile* fWireNoiseFile;
    TFile* fAPDNoiseFile;
    EXOWaveformData* fWireNoiseWfd;
    EXOWaveformData* fAPDNoiseWfd;
    std::string fDataTakingPhase;
    void LoadNoiseFiles();
    void ReplaceAll(std::string &str, std::string target, std::string sub);

    bool SkipChannel(const EXOReconProcessList::WaveformWithType* wfWithType) const {
      // Don't try to find signals on single APD gangs.
      // Channels greater than 0 are HW channels.
      return (wfWithType->fType == EXOReconUtil::kAPD and wfWithType->fWf->fChannel > 0);
    }

    //void SetNoiseWfd();
};

inline void EXOMatchedFilterFinder::SetDivideNoise(bool val/* = true*/)
{
  //Set whether measured noise spectrum should be divided out if possible.
  fDivideNoise = val;
}

inline void EXOMatchedFilterFinder::SetFixedWireThreshold(double val)
{
  //Set a fixed value for the wire threshold.
  //If val is negative, the threshold is being calculated automatically
  //via GetThresholdFactor()*CalculateNoiseCounts().
  fUserWireThreshold = val;
}

inline void EXOMatchedFilterFinder::SetFixedVWireThreshold(double val)
{
  //Set a fixed value for the V-wire threshold.
  //If val is negative, the threshold is being calculated automatically
  //via GetThresholdFactor()*CalculateNoiseCounts().
  fUserVWireThreshold = val;
}

inline void EXOMatchedFilterFinder::SetFixedAPDSumThreshold(double val)
{
  //Set a fixed value for the apd sum threshold.
  //If val is negative, the threshold is being calculated automatically
  //via GetThresholdFactor()*CalculateNoiseCounts().
  fUserAPDThreshold = val;
}

inline void EXOMatchedFilterFinder::SetWireThresholdFactor(double val)
{
  //Set a factor times which the MAD is multiplied to get a threshold.
  //(i.e. the "number of sigmas")
  //This is only used if there is no fixed use threshold set.
  fWireThresholdFactor = val;
}

inline void EXOMatchedFilterFinder::SetVWireThresholdFactor(double val)
{
  //Set a factor times which the MAD is multiplied to get a threshold.
  //(i.e. the "number of sigmas")
  //This is only used if there is no fixed use threshold set.
  fVWireThresholdFactor = val;
}

inline void EXOMatchedFilterFinder::SetAPDSumThresholdFactor(double val)
{
  //Set a factor times which the MAD is multiplied to get a threshold.
  //(i.e. the "number of sigmas")
  //This is only used if there is no fixed use threshold set.
  fAPDSumThresholdFactor = val;
}

inline void EXOMatchedFilterFinder::SetWireSmoothWindow(int val)
{
  //Set the size of the smoothing window used in the filter for the
  //match-filtered waveform. Setting zero or a negative value
  //diables smoothing for wires.
  fWireSmoothWindow = val;
  if(val > 0){
    fWireSavGolFilter.ResetSmootherAttributes(val,0,2);
  }
}

inline void EXOMatchedFilterFinder::SetAPDSmoothWindow(int val)
{
  //Set the size of the smoothing window used in the filter for the
  //match-filtered waveform. Setting zero or a negative value
  //diables smoothing for APDs.
  fAPDSmoothWindow = val;
  if(val > 0){
    fAPDSavGolFilter.ResetSmootherAttributes(val,0,2);
  }
}

inline void EXOMatchedFilterFinder::SetAPDSearchWindowBegin(int val)
{
  //Set the size of the APD signal search window 
  fAPDSearchWindowBegin = val;
}

inline void EXOMatchedFilterFinder::SetAPDSearchWindowEnd(int val)
{
  //Set the size of the APD signal search window 
  fAPDSearchWindowEnd = val;
}

inline void EXOMatchedFilterFinder::SetChargeClusterTime(int val)
{
  //Set the size of the APD signal search window 
  fChargeClusterTime = val;
}

inline void EXOMatchedFilterFinder::SetNumThreads(int val)
{
  // When compiled with threads enabled, specify how many to use.
  if(val <= 0) val = 1;
  fNumThreads = val;
}


inline void EXOMatchedFilterFinder::SetWireNoisePowerSpectrum(bool val)
{
  // Set whether to use the averaged wire noise power spectrum
  fUseWireRealNoise = val;
}

inline void EXOMatchedFilterFinder::SetAPDNoisePowerSpectrum(bool val)
{
  // Set whether to use the averaged wire noise power spectrum
  fUseAPDRealNoise = val;
}

inline void EXOMatchedFilterFinder::SetWireNoiseFilename(std::string val)
{
  // Sets the name of the wire noise power spectrum file
  fWireNoiseFilenameParam = val;
}

inline void EXOMatchedFilterFinder::SetAPDNoiseFilename(std::string val)
{
  // Sets the name of the APD noise power spectrum file
  fAPDNoiseFilenameParam = val;
}

inline void EXOMatchedFilterFinder::SetDataTakingPhase(std::string val)
{
  fDataTakingPhase = val;
  std::cout << "This run was taken during phase " << fDataTakingPhase << ".\n";
  LoadNoiseFiles();
}

inline void EXOMatchedFilterFinder::ReplaceAll(std::string &str, std::string target, std::string sub)
{
    // -- substitute all occurrences of "target" in "str" with "sub"
    size_t index = 0;
    while((index = str.find(target,index)) != std::string::npos){
      str.replace(index, target.length(), sub);
      index += sub.length();
      //std::cout << "index=" << index << ", " << sub << "," << str << std::endl;
    }
}

inline void EXOMatchedFilterFinder::LoadNoiseFiles()
{
  if (fAPDNoiseFilenameParam != "") {
    ReplaceAll(fAPDNoiseFilenameParam,"*",fDataTakingPhase);
    /*
    size_t index = 0;
    while((index = fAPDNoiseFilenameParam.find("*",index)) != std::string::npos){
      fAPDNoiseFilenameParam.replace(index, fDataTakingPhase.length(), fDataTakingPhase);
      index += fDataTakingPhase.length();
      //std::cout << "index=" << index << ", " << fDataTakingPhase << "," << fAPDNoiseFilenameParam << std::endl;
    }*/

    std::cout << "Using APD noise file " << fAPDNoiseFilenameParam << std::endl;
    fAPDNoiseFile = TFile::Open(fAPDNoiseFilenameParam.c_str());

    if (fAPDNoiseFile == NULL) {
      LogEXOMsg("The apd noise file " + fAPDNoiseFilenameParam + " does not exist.", EEError);
    }
   
    fAPDNoiseWfd = (EXOWaveformData *) fAPDNoiseFile->Get("EXOWaveformData");
    fAPDNoiseWfd->Decompress();    
  }

  if (fWireNoiseFilenameParam != "") {
    ReplaceAll(fWireNoiseFilenameParam,"*",fDataTakingPhase);

    /*/ -- substitute all "*" in the file name with the appropriate phase number
    size_t index = 0;
    while((index = fWireNoiseFilenameParam.find("*",index)) != std::string::npos){
      fWireNoiseFilenameParam.replace(index, fDataTakingPhase.length(), fDataTakingPhase);
      index += fDataTakingPhase.length();
    }*/

    std::cout << "Using wire noise file " << fWireNoiseFilenameParam << std::endl;
    fWireNoiseFile = TFile::Open(fWireNoiseFilenameParam.c_str());
    
    if (fWireNoiseFile == NULL) {
      LogEXOMsg("The wire noise file " + fWireNoiseFilenameParam + " does not exist.", EEError);
    }
   
    fWireNoiseWfd = (EXOWaveformData *) fWireNoiseFile->Get("EXOWaveformData");
    fWireNoiseWfd->Decompress();
  }
}

#endif /* EXOMatchedFilterFinder_hh */
