/*
This is a module created by Akshay Nanavati, which calculates the drift
velocity. To use it, use dveloc. It works by putting values into a histogram,
analyzing the histogram and finding delta t. For debugging, outputs to the
screen are left, but commented out.

Documentation for version 1.0 can be found at:
https://confluence.slac.stanford.edu/display/exo/EXOAnalysis+Drift+Velocity+Module+Documentation
for a general overview, while more detailed documentation can be found on the
attatched PDF on that page. NOTE: This documentation is out-dated 
(the current version is 2.0). New documentation will hopefully be up soon.

##########################################

Last edits to this module made by Akshay Nanavati (yahska@gmail.com)
in June 2012. Updated to version 2.0 (see line 43).

Edits include:

--Cleaned up the code stylistically
--Improved memory management and efficiency
--Updated the corrected drift velocity parameters
--Added more run-time user defined parameters
--Improved the accuracy of drift velocity calculations especially when there
  are low statistics per bin; significantly improved the split drift 
  velocity calculations

##########################################
*/
#include "EXOAnalysisManager/EXODriftVelocityModule.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "TH1F.h"
#include "TFile.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

//This is to keep track of version history and edits
string VERSION  = "2.0";
string MODIFIEDBY = "Akshay Nanavati (yahska@gmail.com)";
string MODIFIEDON = "June 2012";

IMPLEMENT_EXO_ANALYSIS_MODULE( EXODriftVelocityModule, "dveloc" )

EXODriftVelocityModule::~EXODriftVelocityModule() {}

//The constructor initializes the default values for module parameters
EXODriftVelocityModule::EXODriftVelocityModule() :
    filename_param ("DV_output.root"),
    bins_overall(1000),
    bins_split(500),
    writeFile_param(false),
    fMinDriftTime(-50.0*CLHEP::microsecond),
    fMaxDriftTime(125.0*CLHEP::microsecond),
    xSplits(4),
    ySplits(4),
    lowerRangex(-200),
    upperRangex(200),
    lowerRangey(-200),
    upperRangey(200),
    plotSplitDV(false),
    saveDV(false),
    txtFileName("DV_stats.txt"),
    Hist_DV_Overall_Pos ("Hist_DV_Overall_Pos",
                         "Hist_DV_Overall_Pos",
                         0,
                         fMinDriftTime,
                         fMaxDriftTime),
    Hist_DV_Overall_Neg ("Hist_DV_Overall_Neg", 
                         "Hist_DV_Overall_Neg",
                         0,
                         fMinDriftTime,
                         fMaxDriftTime)
{}

int EXODriftVelocityModule::Initialize()
{

    Hist_DV_Overall_Pos.SetBins(bins_overall, fMinDriftTime,fMaxDriftTime);
    Hist_DV_Overall_Neg.SetBins(bins_overall, fMinDriftTime,fMaxDriftTime);

    stringstream hist;
    stringstream hist_neg;
    Hist_DV_Split_Pos.resize(xSplits * ySplits);
    Hist_DV_Split_Neg.resize(xSplits * ySplits);
 
    for(int histcount = 0; histcount < xSplits * ySplits; histcount++)
    {

        //Results on the positive side of the detector
        hist.str("");
        hist<<"Hist_DV_Split_Pos"<<histcount + 1;
        Hist_DV_Split_Pos[histcount].SetName (hist.str().c_str());
        Hist_DV_Split_Pos[histcount].SetTitle (hist.str().c_str());

        Hist_DV_Split_Pos[histcount].SetBins(bins_split, 
                                             fMinDriftTime, 
                                             fMaxDriftTime);
        
        //Results on the negative side of the detector
        hist_neg.str("");
        hist_neg<<"Hist_DV_Split_Neg"<<histcount + 1;
        Hist_DV_Split_Neg[histcount].SetName (hist_neg.str().c_str());
        Hist_DV_Split_Neg[histcount].SetTitle (hist_neg.str().c_str());
 
        Hist_DV_Split_Neg[histcount].SetBins(bins_split, 
                                             fMinDriftTime, 
                                             fMaxDriftTime);
    }

    return 0;
}

EXOAnalysisModule::EventStatus EXODriftVelocityModule::ProcessEvent(EXOEventData *ED) {
    //Fill the histogram with tcl values. 
    //Also, subtract out the apd time(GetScintillationCluster(0)->fTime)
    size_t numclusters = ED->GetNumChargeClusters();
    for(size_t i = 0; i < numclusters; i++) 
    {
        EXOChargeCluster* charge_cluster = ED->GetChargeCluster(i);
        if( charge_cluster->GetNumUWireSignals() > 0 ) 
        {

            // Ensure the entire charge cluster is in the positive half of the
            // TPC. The range for positive is 0-37
            bool all_positive_channels = true;
            for( size_t j = 0; j < charge_cluster->GetNumUWireSignals(); j++) 
            {
                if(charge_cluster->GetUWireSignalChannelAt(j) < 0 || 
                   charge_cluster->GetUWireSignalChannelAt(j) > 37 ) 
                {
                    all_positive_channels = false;
                    break;
                }       
            }

            if(all_positive_channels) 
            {
                //Set the apd time
                Double_t scint_time = 0;
                if (ED->GetNumScintillationClusters() > 0 ) 
                {
                    scint_time = ED->GetScintillationCluster(0)->fTime;
                }
                //This excludes events from events from between the APD 
                //plane and the u-wires which was causing a peak to 
                //appear in front of the histograms. This should not
                //effect the drift velocity calculation
                if (-200 < charge_cluster -> fX &&
                    charge_cluster -> fX < 200 &&
                    -200 < charge_cluster -> fY &&
                    charge_cluster -> fY < 200)
                {
                    Hist_DV_Overall_Pos.Fill(charge_cluster->fCollectionTime 
                                              - scint_time);
                }

                //Check to see in which slice the charge cluster lies in and
                //put it in the appropriate histogram
                
                int index = 0; //Keeps track of which histogram to place 
                               //the charge cluster in
                
                //I have broken the detector up into 16 100 x 100 square 
                //regions between -200 and 200 along both the x- and y-axis.
                //Please refer to the documentation for more information
                //on how the detector is split
                
                int incrementy = (upperRangey - lowerRangey) / ySplits;
                int incrementx = (upperRangex - lowerRangex) / xSplits;
                
                for (int yRange = lowerRangey; 
                     yRange <= upperRangey - incrementy; 
                     yRange += incrementy)
                {
                    for (int xRange = lowerRangex; 
                         xRange <= upperRangex - incrementx; 
                         xRange += incrementx)
                    {
                        if (xRange < charge_cluster->fX && 
                            charge_cluster->fX < xRange + incrementx && 
                            yRange < charge_cluster->fY && 
                            charge_cluster->fY < yRange + incrementy)
                        {
                            Hist_DV_Split_Pos[index].Fill(charge_cluster->
                                                          fCollectionTime - 
                                                          scint_time);
                            break;
                        }
                        index += 1;
                    }
                }
            }

            // Ensure the entire cluster is in the negative half of the 
            // TPC. The range for negative is 76-113
            bool all_negative_channels = true;
            for( size_t j = 0; j < charge_cluster->GetNumUWireSignals(); j++) 
            {
                if(charge_cluster->GetUWireSignalChannelAt(j) < 76 || 
                   charge_cluster->GetUWireSignalChannelAt(j) > 113 ) 
                {
                        all_negative_channels = false;
                        break;
                }
            }   

            if(all_negative_channels) 
            {
                Double_t scint_time = 0;
                if (ED->GetNumScintillationClusters() > 0 ) 
                {
                    scint_time = ED->GetScintillationCluster(0)->fTime;
                }
                //This excludes events from events from between the APD 
                //plane and the u-wires which was causing a peak to 
                //appear in front of the histograms. This should not
                //effect the drift velocity calculation
                if (-200 < charge_cluster -> fX &&
                    charge_cluster -> fX < 200 &&
                    -200 < charge_cluster -> fY &&
                    charge_cluster -> fY < 200)
                {
                    Hist_DV_Overall_Neg.Fill(charge_cluster->fCollectionTime 
                                              - scint_time);
                }

                //Check to see in which slice the charge cluster lies in and
                //put it in the appropriate histogram
                
                int index = 0; //Keeps track of which histogram to place 
                               //the charge cluster in
                
                //I have broken the detector up into 16 100 x 100 square 
                //regions between -200 and 200 along both the x- and y-axis.
                //Please refer to the documentation for more information
                //on how the detector is split
                
                int incrementy = (upperRangey - lowerRangey) / ySplits;
                int incrementx = (upperRangex - lowerRangex) / xSplits;
                
                for (int yRange = lowerRangey; 
                     yRange <= upperRangey - incrementy; 
                     yRange += incrementy)
                {
                    for (int xRange = lowerRangex; 
                         xRange <= upperRangex - incrementx; 
                         xRange += incrementx)
                    {
                        if (xRange < charge_cluster->fX && 
                            charge_cluster->fX < xRange + incrementx && 
                            yRange < charge_cluster->fY && 
                            charge_cluster->fY < yRange + incrementy)
                        {
                            Hist_DV_Split_Neg[index].Fill(charge_cluster->
                                                           fCollectionTime - 
                                                           scint_time);
                            break;
                        }
                        index += 1;
                    }
                }
            }
        }
    }
  return kOk;
}

int EXODriftVelocityModule::TalkTo(EXOTalkToManager *talktoManager) {
    
    talktoManager->CreateCommand("/driftvelocity/binsOverall", 
                                 "set the number of bins for DV calculation",
                                 this, 
                                 bins_overall, 
                                 &EXODriftVelocityModule::Setbins_overall);

    talktoManager->CreateCommand("/driftvelocity/binsSplit",
                                 "set the number of bins for DV calculation",
                                 this, 
                                 bins_split, 
                                 &EXODriftVelocityModule::Setbins_split );

    talktoManager->CreateCommand("/driftvelocity/writeRootFile", 
                                 "outputs root file of histograms", 
                                 this, 
                                 writeFile_param, 
                                 &EXODriftVelocityModule::SetwriteFile_param );

    talktoManager->CreateCommand("/driftvelocity/rootFileName", 
                                 "choose output filename", 
                                 this, 
                                 filename_param, 
                                 &EXODriftVelocityModule::SetFilename);

    talktoManager->CreateCommand("/driftvelocity/MinDriftTime",
                                 "Set the minimum drift time to be stored" 
                                 "in histograms in ns (CLHEP units)",
                                 this,
                                 fMinDriftTime,
                                 &EXODriftVelocityModule::SetMinDriftTime);

    talktoManager->CreateCommand("/driftvelocity/MaxDriftTime",
                                 "Set the maximum drift time to be stored" 
                                 "in histograms in ns (CLHEP units)",
                                 this,
                                 fMaxDriftTime,
                                 &EXODriftVelocityModule::SetMaxDriftTime);

    talktoManager->CreateCommand("/driftvelocity/xSplits",
                                 "Set the number of x-axis splits on"
                                 "the detector",
                                 this,
                                 xSplits,
                                 &EXODriftVelocityModule::SetXSplits);

    talktoManager->CreateCommand("/driftvelocity/ySplits",
                                 "Set the number of y-axis splits on" 
                                 "the detector",
                                 this,
                                 ySplits,
                                 &EXODriftVelocityModule::SetYSplits);

    talktoManager->CreateCommand("/driftvelocity/upperRangeX",
                                 "Set the upper range to be split" 
                                 "on the x-axis",
                                 this,
                                 upperRangex,
                                 &EXODriftVelocityModule::SetUpperRangeX);

    talktoManager->CreateCommand("/driftvelocity/lowerRangeX",
                                 "Set the lower range to be split" 
                                 "on the x-axis",
                                 this,
                                 lowerRangex,
                                 &EXODriftVelocityModule::SetLowerRangeX);

    talktoManager->CreateCommand("/driftvelocity/upperRangeY",
                                 "Set the upper range to be split" 
                                 "on the y-axis",
                                 this,
                                 upperRangey,
                                 &EXODriftVelocityModule::SetUpperRangeY);
    
    talktoManager->CreateCommand("/driftvelocity/lowerRangeY",
                                 "Set the lower range to be split" 
                                 "on the y-axis",
                                 this,
                                 lowerRangey,
                                 &EXODriftVelocityModule::SetLowerRangeY);
    
    talktoManager->CreateCommand("/driftvelocity/plotSplitDV",
                                 "Create a histogram of the split" 
                                 "drift velocities",
                                 this,
                                 plotSplitDV,
                                 &EXODriftVelocityModule::SetPlotSplitDV);

    talktoManager->CreateCommand("/driftvelocity/saveDV",
                                 "Save the drift velocities to a txt file", 
                                 this,
                                 saveDV,
                                 &EXODriftVelocityModule::SetSaveDV);

    talktoManager->CreateCommand("/driftvelocity/txtFileName",
                                 "Give the file name for the DV text file", 
                                 this,
                                 txtFileName,
                                 &EXODriftVelocityModule::SetTxtFName);

    return 0;
}

int EXODriftVelocityModule::ShutDown()
{
    //The positive correction constants
    const double mPositive = 1.04903;
    const double bPositive = -0.0133822;
    const double mNegative = 1.01653;
    const double bNegative = -0.00968731;
 
    cout << endl << "****************************************************" 
         << endl;
    cout << endl << "Drift Velocity Module Version " << VERSION 
         << endl;
    cout << "Last Modified: " << MODIFIEDON << " by " << MODIFIEDBY << endl;
    cout << endl << "****************************************************" 
         << endl;
    cout << endl << "--------Calculated Drift Velocity (DV) Stats--------" 
         << endl;
    cout << "Corrected drift velocity is found by plugging the calculated" 
         << endl;
    cout << "drift velocity into the linear equation y = " << mPositive 
         << " * x + " << bPositive << endl;
    cout << "for the positive half and y = " << mNegative << " * x + " << bPositive
         << endl;
    cout << "for the negative half." << endl;
    cout << "y is the corrected drift velocity and x is the calculated" 
         << endl;
    cout << "drift velocity. There are 2 sets of data, one for the +z plane" 
         << endl;
    cout << "and the other for a -z plane. " 
         << "For more detailed documentation on" << endl;
    cout << "this module, please visit:" << endl;
    cout << "https://confluence.slac.stanford.edu/display/exo/EXOAnalysis"
         << "+Drift+Velocity+Module+Documentation" << endl;
    cout << "as well as the attatched PDF on that page" << endl;
    cout << "----------------------------------------------------" << endl;
    
    ofstream txtFile; //The text file where data will be written to
    TH1F posSplitDV;  //The histograms where the split data will be written to
    TH1F negSplitDV;  

    if (saveDV)
    {
        txtFile.open (txtFileName.c_str(), ios::trunc);
    }

    if (plotSplitDV)
    {
        posSplitDV.SetName ("PosSplitDV");
        posSplitDV.SetTitle ("PosSplitDV");
        posSplitDV.SetBins ((xSplits * ySplits) / 2, 0, 0.3);
        
        negSplitDV.SetName ("NegSplitDV");
        negSplitDV.SetTitle ("NegSplitDV");
        negSplitDV.SetBins ((xSplits * ySplits) / 2, 0, 0.3);
    }

    //From the histogram, find the time.  Technically, we have to find delta t, 
    //but it is safe to assume that the first time parameter is at 0
    
    Double_t max = Hist_DV_Overall_Pos.GetMaximum();
    Double_t maxbin = Hist_DV_Overall_Pos.GetMaximumBin();
    Double_t limit = max/3;
    Double_t drifttime = 0.0;
    int delta = 10; //If the program thinks it has found the upperbound, it will
                   //check to see if somewhere in the next delta bins the 
                   //histogram has another peak, in which case the upper bound 
                   //has not been found.
    bool found;
    for(Int_t v = static_cast<Int_t>(maxbin); 
        v < (Hist_DV_Overall_Pos.GetNbinsX()); 
        v++)
    {
        found = true;
        Double_t q = Hist_DV_Overall_Pos.GetBinContent(v);
        if(q < limit)
        {
            //Check to see if the upper bound has actually been found
            for (int i = 1; i <= delta; i++)
            {
                if (i < Hist_DV_Overall_Pos.GetNbinsX())
                {
                    if (Hist_DV_Overall_Pos.GetBinContent(v + i) > limit)
                    {
                        found = false;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            if (found)
            {
                drifttime = Hist_DV_Overall_Pos.GetBinCenter(v);
                break;
            }
        }
    }
    Double_t DV_Overall;
    DV_Overall = 19.86/(drifttime/1000);
    if (saveDV)
    {
        txtFile << "DV_Overall_Pos: " << DV_Overall << endl;
        txtFile << "DV_Overall_Pos_Corrected: " 
                << mPositive * DV_Overall + bPositive << endl;
    } 

    cout << "       Overall Calculated DV Stats (Postive):       " << endl;
    cout << endl << "Drift Velocity..." << DV_Overall 
         << "...Corrected DV..." << mPositive * DV_Overall + bPositive << endl;
    cout << "----------------------------------------------------" << endl;

    cout << "   Detector Split Calculated DV Stats (Positive):   " << endl;
    cout << endl << "          Calculated DV..............." << "Corrected DV" 
         << endl;
    for(int hist_count = 0; hist_count < ySplits * xSplits; hist_count++)
    {
        Double_t max_split = Hist_DV_Split_Pos[hist_count].GetMaximum();
        Double_t maxbin_split = Hist_DV_Split_Pos[hist_count].GetMaximumBin();
        Double_t limit_split = max_split/3;
        Double_t drifttime_split = 0.0;
        for(Int_t bin_num = static_cast<Int_t>(maxbin_split); 
            bin_num < (Hist_DV_Split_Pos[hist_count].GetNbinsX()); 
            bin_num++)
        {
            found = true;
            Double_t bin_cont;
            bin_cont = Hist_DV_Split_Pos[hist_count].GetBinContent(bin_num);
            if(bin_cont < limit_split)
            {
                //Check to see if the upper bound has actually been found
                for (int i = 1; i <= delta; i++)
                {
                    if (i < Hist_DV_Split_Pos[hist_count].GetNbinsX())
                    {
                        if (Hist_DV_Split_Pos[hist_count].
                            GetBinContent(bin_num + i) > limit_split)
                        {
                            found = false;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (found)
                {
 
                    drifttime_split = (Hist_DV_Split_Pos[hist_count].
                                       GetBinCenter(bin_num));
                    break;
                }
            }
        }
        Double_t DV_split;
        DV_split = 19.86 / (drifttime_split / 1000);
        if (plotSplitDV)
        {
            posSplitDV.Fill(DV_split);
        } 
        if (saveDV)
        {
            txtFile << "Pos_Split_" << hist_count + 1 << ": " << DV_split 
                    << endl;
            txtFile << "Pos_Split_" << hist_count + 1 << "_Corrected: "
                    << mPositive * DV_split + bPositive << endl;
        }

        cout << "Split# " << hist_count + 1 << "..." << DV_split 
             << ".................." << mPositive * DV_split + bPositive << endl;
    }
    
    cout<<"----------------------------------------------------"<<endl;

    Double_t max_neg = Hist_DV_Overall_Neg.GetMaximum();
    Double_t maxbin_neg = Hist_DV_Overall_Neg.GetMaximumBin();
    Double_t limit_neg = max_neg/3;
    Double_t drifttime_neg = 0.0;
    for(Int_t v_neg=static_cast<Int_t>(maxbin_neg); 
        v_neg<(Hist_DV_Overall_Neg.GetNbinsX()); 
        v_neg++)
    {
        found = true;
        Double_t q_neg = Hist_DV_Overall_Neg.GetBinContent(v_neg);
        if(q_neg < limit_neg)
        {
            //Check to see if the upper bound has actually been found
            for (int i = 1; i <= delta; i++)
            {
                if (i < Hist_DV_Overall_Neg.GetNbinsX())
                {
                    if (Hist_DV_Overall_Neg.GetBinContent(v_neg + i) > 
                        limit_neg)
                    {
                        found = false;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            if (found)
            {
                drifttime_neg = Hist_DV_Overall_Neg.GetBinCenter(v_neg);
                break;
            }
        }
    }

    Double_t DV_Overall_neg;
    DV_Overall_neg = 19.86 / (drifttime_neg / 1000);
    if (saveDV)
    {
        txtFile << "DV_Overall_Neg: " << DV_Overall_neg << endl;
        txtFile << "DV_Overall_Neg_Corrected: " 
                << mNegative * DV_Overall_neg + bNegative << endl;
    }

    cout << "       Overall Calculated DV Stats (Negative):      " << endl;
    cout << endl << "Drift Velocity..." << DV_Overall_neg 
         << "...Corrected DV..." << mNegative * DV_Overall_neg + bNegative
         << endl;
    cout << "----------------------------------------------------" << endl;

    cout << "    Detector Split Calculated DV Stats (Negative):    " << endl;
    cout << endl << "          Calculated DV..............." 
         << "Corrected DV" << endl;

    for(int hist_count_neg = 0; 
            hist_count_neg < ySplits * xSplits; 
            hist_count_neg++)
    {
        Double_t max_split_neg = (Hist_DV_Split_Neg[hist_count_neg].
                                  GetMaximum());
        Double_t maxbin_split_neg = (Hist_DV_Split_Neg[hist_count_neg].
                                     GetMaximumBin());
        Double_t limit_split_neg = max_split_neg/3;
        Double_t drifttime_split_neg = 0.0;
        for(Int_t bin_num_neg = static_cast<Int_t>(maxbin_split_neg); 
            bin_num_neg < (Hist_DV_Split_Neg[hist_count_neg].GetNbinsX()); 
            bin_num_neg++)
        {
            found = true;
            Double_t bin_cont_neg = (Hist_DV_Split_Neg[hist_count_neg].
                                     GetBinContent(bin_num_neg));
            if(bin_cont_neg < limit_split_neg)
            {
                //Check to see if the upper bound has actually been found
                for (int i = 1; i <= delta; i++)
                {
                    if (i < Hist_DV_Split_Neg[hist_count_neg].GetNbinsX())
                    {
                        if (Hist_DV_Split_Neg[hist_count_neg].
                            GetBinContent(bin_num_neg + i) > limit_split_neg)
                        {
                            found = false;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (found)
                {
 
                    drifttime_split_neg = (Hist_DV_Split_Neg[hist_count_neg].
                                           GetBinCenter(bin_num_neg));
                    break;
                }
            }
        }
        Double_t DV_split_neg;
        DV_split_neg = 19.86/(drifttime_split_neg/1000);
        if (plotSplitDV)
        {
            negSplitDV.Fill(DV_split_neg);
        }
        if (saveDV)
        {
            txtFile << "Neg_Split_" << hist_count_neg + 1 << ": "
                    << DV_split_neg << endl;
            txtFile << "Neg_Split_" << hist_count_neg + 1 
                    << "_Corrected: " << mNegative * DV_split_neg + bNegative
                    << endl;
        }

        cout << "Split# " << hist_count_neg + 1 << "..." << DV_split_neg 
             << ".................." << mNegative* DV_split_neg + bNegative
             << endl;
    }
    cout << "----------------------------------------------------" << endl;

    cout << endl << "****************************************************" 
         << endl;

    cout<<endl;

    if (saveDV)
    {
        txtFile.close();
    }

    if (writeFile_param) 
    {
        TFile *f = new TFile(filename_param.c_str(), "RECREATE");

        Hist_DV_Overall_Pos.Write();
        Hist_DV_Overall_Neg.Write();

        //Write one histogram for each of the splits
        std::stringstream hist;
        std::stringstream hist_neg;
        for(int count=0; count < xSplits * ySplits; count++)
        {
            //Results on the positive side of the detector
            Hist_DV_Split_Pos[count].Write();

            //Results on the negative side of the detector
            Hist_DV_Split_Neg[count].Write();
        }
        if (plotSplitDV)
        {   
            posSplitDV.Write();
            negSplitDV.Write();
        }
        f->Close();
        delete f;

    }

    return 0;
}
