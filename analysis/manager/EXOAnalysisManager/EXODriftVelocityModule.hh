#ifndef EXODriftVelocityModule_hh
#define EXODriftVelocityModule_hh

#include "EXOAnalysisModule.hh"
#include "Rtypes.h"
#include <string>
#include <vector>
#include "TH1F.h"

class EXOEventData;
class EXOTalkToManager;

class EXODriftVelocityModule : public EXOAnalysisModule 
{

private :

    std::string   filename_param;
    Int_t  bins_overall;
    Int_t  bins_split;
    bool   writeFile_param;
    double fMinDriftTime;
    double fMaxDriftTime;
    int xSplits;
    int ySplits;
    int lowerRangex;
    int upperRangex;
    int lowerRangey;
    int upperRangey;
    bool plotSplitDV;
    bool saveDV;
    std::string txtFileName;
    TH1F   Hist_DV_Overall_Pos;
    std::vector<TH1F>   Hist_DV_Split_Pos;
    TH1F   Hist_DV_Overall_Neg;
    std::vector<TH1F>   Hist_DV_Split_Neg;

public :

    EXODriftVelocityModule();
    ~EXODriftVelocityModule();

    int Initialize();
    EventStatus ProcessEvent(EXOEventData *ED);
    int ShutDown();
    int TalkTo(EXOTalkToManager *tm);
    void SetFilename(std::string aval) { filename_param = aval; }
    void SetwriteFile_param(bool aval) { writeFile_param = aval; }
    void Setbins_overall(Int_t aval) { bins_overall = aval; }
    void Setbins_split(Int_t aval) { bins_split = aval; }
    void SetMinDriftTime(double time) { fMinDriftTime = time; }
    void SetMaxDriftTime(double time) { fMaxDriftTime = time; }
    void SetXSplits (int splits) { xSplits = splits; }
    void SetYSplits (int splits) { ySplits = splits; }
    void SetUpperRangeX (int upper) { upperRangex = upper; }
    void SetLowerRangeX (int lower) { lowerRangex = lower; }
    void SetUpperRangeY (int upper) { upperRangey = upper; }
    void SetLowerRangeY (int lower) { lowerRangey = lower; }
    void SetPlotSplitDV (bool set) { plotSplitDV = set; }
    void SetSaveDV (bool set) { saveDV = set; }
    void SetTxtFName (std::string fname) { txtFileName = fname; }
    DEFINE_EXO_ANALYSIS_MODULE( EXODriftVelocityModule )

};
#endif
