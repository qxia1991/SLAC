#include "EXOUtilities/EXO3DDigitizeWires.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOWaveformData.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOElectronics.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOMCChannelScaling.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOWireCrossing.hh"
#include "TRandom.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1D.h"
#include <fstream>    // for reading real noise files
#include <cstdlib>    // for the int form of abs
using CLHEP::second;
using CLHEP::keV;
using CLHEP::cm;
using CLHEP::microsecond;

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

EXO3DDigitizeWires::EXO3DDigitizeWires() :
    //fEField("/nfs/slac/g/exo_data6/groups/3DFieldMaps/3Dmaxwell/3D_Efield.root"),
    //fWeightFieldU("/nfs/slac/g/exo_data6/groups/3DFieldMaps/3Dmaxwell/3D_UWeight.root", EXOMiscUtil::kUWire),
    //fWeightFieldV("/nfs/slac/g/exo_data6/groups/3DFieldMaps/3Dmaxwell/3D_VWeight.root", EXOMiscUtil::kVWire),
    fEField(""),
    fWeightFieldU("", EXOMiscUtil::kUWire),
    fWeightFieldV("", EXOMiscUtil::kVWire),
    fDdata(NUMBER_READOUT_CHANNELS),
    fSampledData(NUMBER_READOUT_CHANNELS),
    fElectronLifetime(1.0*second),
    fDriftVelocity(DRIFT_VELOCITY),
    fDriftVelocityCollection(0.0),
    fUBaseline(UWIRES_ADC_BASELINE_COUNTS),
    fVBaseline(VWIRES_ADC_BASELINE_COUNTS),
    fWvalue_energy_per_electron(W_VALUE_LXE_EV_PER_ELECTRON),
    fMCExtraScale(1.0),
    fTransverseDiffusionCoeff(0),
    fLongitudinalDiffusionCoeff(0),
    fNumDiffusePCDs(0),
    fDiffusionDuringDrifting(false),
    fDigitizeInduction(true),
    fNSample(0),
    fTriggerTime(TRIGGER_TIME),
    fDigitizeVNeighborSignals(3),
    fDigitizeUNeighborSignals(1),
    fRefCh(19),
    fApplyEmpiricalScaling(true),
    fApplyGainScaling(false),
    fElectronics(NULL),
    fTimingInfo(NULL),
    fACSmearFactor(0.0)
{
    // Make sure the number of channels is even
    assert( (NCHANNEL_PER_WIREPLANE%2) == 0 );
 
    //LogEXOMsg("***********Created 3DDigitize Class****************", EEWarning);
}
 
//Destructor
EXO3DDigitizeWires::~EXO3DDigitizeWires() 
{
    //close all the files before killing class instance
    //LogEXOMsg("***********Killing This 3DDigitize Class****************", EEWarning);
    CloseElectricFieldFile();
    CloseUWeightPotentialFile();
    CloseVWeightPotentialFile();
}

void EXO3DDigitizeWires::set_electron_lifetime( double value )
{
    if ( value <= 1.0*microsecond ) {
        LogEXOMsg("Error: use an electron lifetime greater than one microsecond, substituting default value of one second", EEWarning);
        fElectronLifetime = 1.0*second;
        return;
     }
    std::ostringstream stream;
    stream << "setting the electron lifetime to " << value/microsecond << " microseconds";
     
    LogEXOMsg(stream.str(), EEDebug);
    fElectronLifetime = value;
    return;
}

void EXO3DDigitizeWires::set_drift_velocity( double value )
{
    // Setting the drift velocity in the main part of the detector.
    if ( value <= 0 *cm/microsecond ) {
        LogEXOMsg("use a drift velocity greater than 0, substituting default value of .28 cm/microsecond", EEWarning);
        fDriftVelocity = .28 *cm/microsecond;
        return;
    }

    std::ostringstream stream;
    stream << "setting the drift velocity to " << value/(cm/microsecond) << " cm/microsecond";
    LogEXOMsg(stream.str(),  EEDebug);
    fDriftVelocity = value;
    return;
}

void EXO3DDigitizeWires::set_collection_drift_velocity( double value )
{
    // Setting the drift velocity in the region once the charge has passed the
    // v-wires.
    
    if ( value < 0.0 ) {
        LogEXOMsg("use a collection drift velocity greater than 0,"
                "substituting default value of 0.0 cm/microsecond", EEWarning);
        fDriftVelocityCollection = 0.0;
        return;
    }

    std::ostringstream stream;
    stream << "setting the collection drift velocity to " << value/(cm/microsecond) << " cm/microsecond";
    LogEXOMsg(stream.str(),  EEDebug);
    fDriftVelocityCollection = value;
    return;
}

void EXO3DDigitizeWires::set_nsample( size_t asample )
{
    // Get memory for signals
      if ( asample == 0 ) {
          std::ostringstream stream;
          stream << "nsample = " << asample << " not valid";
          LogEXOMsg(stream.str(), EEError);
          return;
      }
      fNSample = asample;
      for (size_t i = 0 ; i < NUMBER_READOUT_CHANNELS; i++) {
          fDdata[i].SetLength(fNSample*BANDWIDTH_FACTOR);
          fDdata[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH);
          fSampledData[i].SetLength(fNSample);
          fSampledData[i].SetSamplingPeriod(SAMPLE_TIME_HIGH_BANDWIDTH*BANDWIDTH_FACTOR);
      }
} 

void EXO3DDigitizeWires::SetTimingStatisticInfo(EXOTimingStatisticInfo* value)
{
    if(value == NULL) LogEXOMsg("You set fTimingInfo to a null pointer?", EEWarning);
    fTimingInfo = value;
}


void EXO3DDigitizeWires::ResetWires()
{
    //Clear out the Waveform by filling with 0s
    //Clear out the wire channel# list
    for (size_t i=0;i<NWIREPLANE*NCHANNEL_PER_WIREPLANE;i++) fDdata[i].Zero();
    fWireSignalChannels.clear();

}

void EXO3DDigitizeWires::ScaleAndDigitizeWireSignals(EXOWaveformData& wfData)
{
    // Scale the signal for the ADC and turn it into integers
    // Also add in the offset to the x and y channels
    // Put waveforms into WaveformData.
    for ( int i = 0; i < NWIREPLANE; i++ ) {
        for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
            EXODoubleWaveform& tempD = fSampledData[i*NCHANNEL_PER_WIREPLANE + j];
            EXOWaveform& NewWaveform = *wfData.GetNewWaveform();

            NewWaveform.SetLength(fNSample);
            Int_t channel_number = i*NCHANNEL_PER_WIREPLANE + j;
            NewWaveform.fChannel = channel_number;

            // Find Gains for the Charge Channels
            // scale the MC WFs by the inverse of this so that the correct gain 
            // variations are present in MC.  These need to be the same as the gains in the
            // u-gain module btw
            
            //we are dividing out the gains now.  If you don't use the database gains than roughly guess
            //constant gain of 350.0 which is roughly the average that is in the database.
            Double_t gain = fApplyGainScaling ? GetGainOnChannel(channel_number):1.0;

            //Apply empirical scaling to match Run 2 data amplitudes.
            //now this should not include the gains only the average value for U/V/APD
            Double_t scale_fac = fApplyEmpiricalScaling ? GetScalingOnChannel(channel_number) : 1.0;
            scale_fac = fMCExtraScale*scale_fac/gain;
            scale_fac *= (1 + fACSmearFactor);
            
            //std::cout << "Digi Scale for ch " << channel_number << "  " << scale_fac << std::endl;
            
            //if (EXOMiscUtil::TypeOfChannel(channel_number) == EXOMiscUtil::kVWire){
            //    std::cout << "MJ Scale Check " << GetScalingOnChannel(channel_number) << " " << GetGainOnChannel(channel_number) << std::endl;
            //}

            for ( size_t k = 0; k < fNSample; k++ ) {
                //std::cout << tempD[k] << std::endl;
                double tempValue = tempD[k]*ADC_BITS*scale_fac/(ADC_FULL_SCALE_ELECTRONS_WIRE*fWvalue_energy_per_electron);
                
                //if (EXOMiscUtil::TypeOfChannel(channel_number) == EXOMiscUtil::kVWire){
                //    
                //    tempValue = tempValue*1.03*(1/0.9);
                //}
                
                tempValue += ((i/NCHANNEL_PER_WIREPLANE)%2 == 0 ? fUBaseline : fVBaseline); // Use the appropriate baseline.
                
                NewWaveform[k] = (int)(tempValue + (tempValue > 0.0 ? 0.5 : -0.5) ); // Implement correct rounding.
            }
        }
    }

}


void EXO3DDigitizeWires::AddNoiseToWireSignals()
{
    // Add in random noise

    // If this is NULL, get out.
    if (not fElectronics) return;
    for ( int i = 0; i < NWIREPLANE; i++ ) {
        for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
            size_t chan = i*NCHANNEL_PER_WIREPLANE + j;
            if (not fElectronics->HasNoiseTransformForChannel(chan)) continue;
            fElectronics->GetNoiseTransformForChannel(chan).Transform(&fSampledData[chan]);    
        }
    }
}
    
void EXO3DDigitizeWires::DoADCSamplingWires()
{
    // Do the ADC sampling of the shaped signal
    for ( int i = 0; i < NWIREPLANE; i++ ) {
        for ( int j = 0; j < NCHANNEL_PER_WIREPLANE; j++ ) {
            EXODoubleWaveform& temp = fDdata[i*NCHANNEL_PER_WIREPLANE + j];
            EXODoubleWaveform& dest = fSampledData[i*NCHANNEL_PER_WIREPLANE + j];
            for ( size_t m = 0; m < fNSample; m++ ) {
                dest[m] = temp[m*BANDWIDTH_FACTOR];
            }
        }
    }

}

void EXO3DDigitizeWires::ShapeWireSignals()
{
    // Shape the high bandwidth signals

    if (not fElectronics) return;
    // We shape *only* the signals which were filled
    std::set<int>::const_iterator iter;
    Double_t gain = 0.0;
    const EXOTransferFunction* lastTransfer = 0;
    for (iter = fWireSignalChannels.begin();iter != fWireSignalChannels.end();iter++) {
        EXODoubleWaveform& wf = fDdata[*iter];
        if (not fElectronics->HasTransferFunctionForChannel(*iter)) continue;
        const EXOTransferFunction& transFunc = fElectronics->GetTransferFunctionForChannel(*iter);
        if ( not lastTransfer || *lastTransfer != transFunc ) {
            // means we need to recalculate the gain.  FixMe, if this is going to be
            // called with many different shaping times, this will be a slowdown.
            gain = transFunc.GetGain();
            lastTransfer = &transFunc;
        }
        transFunc.Transform(&wf);
        wf /= gain;
    }
    
    
}

void EXO3DDigitizeWires::PlotWireSignals()
{
    std::set<int>::const_iterator iter;
    for (iter = fWireSignalChannels.begin();iter != fWireSignalChannels.end();iter++) {
        EXODoubleWaveform& wf = fDdata[*iter];
        TCanvas *c2 = new TCanvas("c2");
        std::cout << "  Channel =   " << *iter  << std::endl;
        wf.GimmeHist()->Draw();
        c2->Update();
        string n;
        std::cin >> n;

    }

}

double EXO3DDigitizeWires::GetScalingOnChannel(const size_t channel) const
{
    std::map<size_t, double>::const_iterator Iter = fScaling.find(channel);
    if(Iter == fScaling.end()) {
        std::ostringstream stream;
        stream<<"The scaling on channel "<<channel<<" was not found.";
        LogEXOMsg(stream.str(), EEAlert);
        return 0.0;
    }
    return Iter->second;
}

double EXO3DDigitizeWires::GetGainOnChannel(const size_t channel) const
{
    std::map<size_t, double>::const_iterator Iter;
    if (EXOMiscUtil::TypeOfChannel(channel) == EXOMiscUtil::kUWire) {
        Iter = fUGains.find(channel);
    }
    else if (EXOMiscUtil::TypeOfChannel(channel) == EXOMiscUtil::kVWire){
        Iter = fVGains.find(channel);
    }
    else{
        LogEXOMsg("3D Digitizer tried to scale a channel which is not a Wire Channel (can't do that)", EEError);
    }
    if(Iter == fScaling.end()) {
        std::ostringstream stream;
        stream<<"The gain on channel "<<channel<<" was not found in 3D Digi.";
        LogEXOMsg(stream.str(), EEAlert);
        return 0.0;
    }
    return Iter->second;
}


void EXO3DDigitizeWires::GenerateUnshapedSignals(EXOMCPixelatedChargeDeposit* Pixel)
{
    //Reset Pixel information that we're about to fill (in case we're redigitizing).
    Pixel->fWireHitTime = 0;
    Pixel->fDepositChannel = -999;
    Pixel->fWireChannelsAffected.clear();

    // Get Pixel information
    const double Xpos = Pixel->GetPixelCenter().GetX();
    const double Ypos = Pixel->GetPixelCenter().GetY();
    const double Zpos = Pixel->GetPixelCenter().GetZ();
    const double Time = Pixel->GetPixelCenter().GetT();
    const double Energy = Pixel->fTotalIonizationEnergy;

    //Create data structures to store channel information -- these are the channels we'll trace.
    // We'll fill it separately for u-wires and v-wires

    // Which channel is closest 
    // UChannels  z>0: [ch0...ch37] -> [-166.5, +166.5] steps of 9
    std::vector<WireToDigitize> VChannelsToUse;
    std::vector<WireToDigitize> UChannelsToUse;
    int VCh; //V-Channel number 
    int UCh; //U-Channel number
    double RelXpos; //Relative X pos inside Efield cut
    double RelYpos; //Relative Y pos inside Efield cut
    WireToDigitize aWireV;
    WireToDigitize aWireU;


    //Get the Relative x-y position with respect to the reference wire
    GetClosestChannel(Xpos, Ypos, Zpos, UCh, VCh, RelXpos, RelYpos);

    //Set the Reference wire positions (this is always the same)
    aWireV.fPosition = 0.5*CHANNEL_WIDTH;
    aWireV.fWaveform = &fDdata[VCh];
    VChannelsToUse.push_back(aWireV);
    fWireSignalChannels.insert(VCh);
    Pixel->fWireChannelsAffected.insert(VCh);

    //std::cout << "VNeighbors " << (int)fDigitizeVNeighborSignals << std::endl;

    for(int chanGap = -(int)fDigitizeVNeighborSignals;
            chanGap <= (int)fDigitizeVNeighborSignals; chanGap++) {
        if (chanGap == 0) continue;
        int chosen_channel = VCh + chanGap;
        if((chosen_channel >= 1*NCHANNEL_PER_WIREPLANE and chosen_channel < 2*NCHANNEL_PER_WIREPLANE) or 
          (chosen_channel >= 3*NCHANNEL_PER_WIREPLANE and chosen_channel < 4*NCHANNEL_PER_WIREPLANE))
        {
            aWireV.fPosition = (0.5+chanGap)*CHANNEL_WIDTH ;
            aWireV.fWaveform = &fDdata[chosen_channel];
            VChannelsToUse.push_back(aWireV);
            fWireSignalChannels.insert(chosen_channel);
            Pixel->fWireChannelsAffected.insert(chosen_channel);

            //std::cout << "MJ OUT: V-To-Use-Pos " << chosen_channel << " " << aWireV.fPosition << std::endl;

        }
    }
    //std::cout << std::endl;


    //aWire contains Reference wire postions not absolute positons 
    aWireU.fPosition = 0.5*CHANNEL_WIDTH;
    aWireU.fWaveform = &fDdata[UCh];
    UChannelsToUse.push_back(aWireU);
    fWireSignalChannels.insert(UCh);
    Pixel->fWireChannelsAffected.insert(UCh);

    for(int chanGap = -(int)fDigitizeUNeighborSignals;
            chanGap <= (int)fDigitizeUNeighborSignals; chanGap++) {
        if (chanGap == 0) continue;
        int chosen_channel = UCh + chanGap;
        if((chosen_channel >= 0*NCHANNEL_PER_WIREPLANE and chosen_channel < 1*NCHANNEL_PER_WIREPLANE) or
           (chosen_channel >= 2*NCHANNEL_PER_WIREPLANE and chosen_channel < 3*NCHANNEL_PER_WIREPLANE))
        {
            aWireU.fPosition = (0.5+chanGap)*CHANNEL_WIDTH ;
            aWireU.fWaveform = &fDdata[chosen_channel];
            UChannelsToUse.push_back(aWireU);
            fWireSignalChannels.insert(chosen_channel);
            Pixel->fWireChannelsAffected.insert(chosen_channel);
        }
    }


    //double Utest;  //Hold the final Uposition of the track
    //double Vtest;  //Hold the final Vposition of the track
    //EXOMiscUtil::XYToUVCoords(Utest, Vtest, Xpos, Ypos, Zpos);
    //std::cout << " Start Uch = " << UCh << " Ustart = " << Utest ;
    //EXOMiscUtil::XYToUVCoords(Utest, Vtest, RelXpos, RelYpos, Zpos);
    //std::cout << " UstartRel = " << Utest << " dU = " << Utest-4.5  << std::endl;

    GenerateSignals(UChannelsToUse,  VChannelsToUse,  UCh, VCh, 
                    RelXpos, RelYpos, Zpos, Time, Energy, Pixel);
    

}



void EXO3DDigitizeWires::GenerateSignals(const std::vector<WireToDigitize>& UChannelsToUse, 
                                         const std::vector<WireToDigitize>& VChannelsToUse, 
                                         int URefCh, int VRefCh, double Xpos, double Ypos,double Zpos, 
                                         double Time, double Energy,
                                         EXOMCPixelatedChargeDeposit* Pixel)
{

    if(EXOMiscUtil::TypeOfChannel(URefCh) != EXOMiscUtil::kUWire or EXOMiscUtil::TypeOfChannel(VRefCh) != EXOMiscUtil::kVWire) {
        std::ostringstream stream;
        stream << "UCh = " <<URefCh << " VCh =  " << VRefCh;
        LogEXOMsg("Invalid channel type for Reference Channel " + stream.str(), EEAlert);
        return;
    }
    if(Time + fTriggerTime < 0.0) {
        LogEXOMsg("Event occurs before the traces start; skipping", EEWarning);
        return;
    }
    size_t TimeIndex = static_cast<size_t>((Time + fTriggerTime)/SAMPLE_TIME_HIGH_BANDWIDTH);
    if(TimeIndex > fNSample*BANDWIDTH_FACTOR) {
        LogEXOMsg("Event occurs after the traces end; skipping", EEWarning);
        return;
    }


    if(fDiffusionDuringDrifting and not(fTransverseDiffusionCoeff > 0) and not( fLongitudinalDiffusionCoeff> 0))
    {
        LogEXOMsg("Diffusion is on but coefficents are 0.0", EEWarning);
    }


    std::vector<double> UnshapedUSignal(UChannelsToUse.size(), 0);
    std::vector<double> UnshapedVSignal(VChannelsToUse.size(), 0);

    Double_t uplot[fNSample*BANDWIDTH_FACTOR];
    Double_t vplot[fNSample*BANDWIDTH_FACTOR];
    Double_t zplot[fNSample*BANDWIDTH_FACTOR];
    Double_t time[fNSample*BANDWIDTH_FACTOR];
    int plot_j = 0;

    double Ustart;
    double Vstart;
        
    EXOMiscUtil::XYToUVCoords(Ustart, Vstart, Xpos, Ypos, Zpos);

    //Double_t trueRadial = std::sqrt(Pixel->GetPixelCenter().GetX()*Pixel->GetPixelCenter().GetX() + Pixel->GetPixelCenter().GetY()*Pixel->GetPixelCenter().GetY());
    //Double_t radialCorr = 1./(1. + 0.54*std::exp((trueRadial-180.28)/18.08));

    double Zstart;
    Zstart = Zpos;
    //If you start too high just kill the PCD so you don't wast time since this 
    //can never get to the UPLande anyways if it starts above the UPlane
    if(abs(Zstart) > CATHODE_ANODE_x_DISTANCE){
        return;
    }

    for(size_t i = TimeIndex; i < fNSample*BANDWIDTH_FACTOR; i++) {
        double ex, ey, ez;
        fEField.GetEField(Xpos, Ypos, Zpos, ex, ey,ez);
        double emag = std::sqrt(ex*ex + ey*ey + ez*ez);
        if(emag <= 1.0e-4*keV/cm) {
            //Actually probably hit a wire but fine graining causes you to be alittle short
            std::ostringstream smallE;
            double Utest;  //Hold the final Uposition of the track
            double Vtest;  //Hold the final Vposition of the track
            EXOMiscUtil::XYToUVCoords(Utest, Vtest, Xpos, Ypos, Zpos);
            smallE << endl;
            smallE << "emag = " << emag  << " U = " << Utest << " V = " << Vtest << " z =" << Zpos;
            LogEXOMsg("In a region of very small electric field -- stopping trace " + smallE.str() , EEWarning);
            double Umod = std::fmod(UChannelsToUse[0].fPosition - Utest, WIRE_PITCH);
            if(Umod < 0) Umod+=WIRE_PITCH; //because of how fmod handles negative numerators
            if(Umod > WIRE_PITCH/2.0) Umod = WIRE_PITCH - Umod; //Reverse the direction            
            return;
        }
        ex /= emag;
        ey /= emag;
        ez /= emag;

        double drift_velocity = fDriftVelocity;
        if(fDriftVelocityCollection > 0.0 and abs(Zpos) >= CATHODE_ANODE_y_DISTANCE) {
            drift_velocity = fDriftVelocityCollection;
        }
        
        //Diffusion Random Walk in Transverse and Longitidual Planes
        //Diffusion results in spread of Sqrt(2*D*t) in each direction 
        //Different Coefficents for transverse/longitudal directions
        double randomWalkX = 0.;
        double randomWalkY = 0.;
        double randomWalkZ = 0.;
        if(fDiffusionDuringDrifting){
            if(fTransverseDiffusionCoeff > 0.){
                double sigmaXY = sqrt(fTransverseDiffusionCoeff*SAMPLE_TIME_HIGH_BANDWIDTH);
                randomWalkX = gRandom->Gaus(0.,sigmaXY);
                randomWalkY = gRandom->Gaus(0.,sigmaXY);
            }
            if(fLongitudinalDiffusionCoeff > 0.){
                double sigmaZ = sqrt(2 * fLongitudinalDiffusionCoeff * SAMPLE_TIME_HIGH_BANDWIDTH);
                randomWalkZ = gRandom->Gaus(0.,sigmaZ);
            }

        }

        double dX = -1.0*ex * drift_velocity * SAMPLE_TIME_HIGH_BANDWIDTH + randomWalkX;
        double dY = -1.0*ey * drift_velocity * SAMPLE_TIME_HIGH_BANDWIDTH + randomWalkY;
        double dZ = -1.0*ez * drift_velocity * SAMPLE_TIME_HIGH_BANDWIDTH + randomWalkZ;

        // How much free charge is still drifting at this point?
        double Q_free = Energy * std::exp(-1.0 * (i-TimeIndex) * SAMPLE_TIME_HIGH_BANDWIDTH / fElectronLifetime);


        //if we've broken the plane, interpolate back to the plane
        //CATHODE_ANODE_x_DISTANCE --> zpos of the Uwire plane
        if(abs(Zpos + dZ) > CATHODE_ANODE_x_DISTANCE  && abs(Zpos) < CATHODE_ANODE_x_DISTANCE) {
            double delt_frac = (CATHODE_ANODE_x_DISTANCE - abs(Zpos))/abs(dZ);
            delt_frac = TMath::Min(1.0, delt_frac);
            dZ *= delt_frac;
            dX *= delt_frac;
            dY *= delt_frac;
        }

        //avoid getting stuck in V-wires
        //if stuck push it out
        //Might not need this
        /*
        if( (TMath::Abs(Zpos+dZ - CATHODE_ANODE_y_DISTANCE) < WIRE_DIAMETER) &&
                (TMath::Abs(Zpos - CATHODE_ANODE_y_DISTANCE) < WIRE_DIAMETER) ) {
            dZ+= WIRE_DIAMETER;
        }
        */

        /*
        if(true) {
            std::ostringstream stream;
            stream << "Zpos +dz = " << abs(Zpos + dZ) << " cath =  " << CATHODE_ANODE_x_DISTANCE - WIRE_DIAMETER << std::endl;
            stream << "ex =" << ex << " ey=" << ey << " ez=" << ez << std::endl;
            LogEXOMsg("Check " + stream.str(), EEWarning);
        }
        */
        //double Utest;
        //double Vtest;
        //EXOMiscUtil::XYToUVCoords(Utest, Vtest, Xpos+dX, Ypos+dY, Zpos+dZ);
        //std::cout << Utest << " " << Vtest << " " << Zpos << std::endl;

        double Utest;  //Hold the final Uposition of the track
        double Vtest;  //Hold the final Vposition of the track
        EXOMiscUtil::XYToUVCoords(Utest, Vtest, Xpos+dX, Ypos+dY, Zpos+dZ);
        uplot[plot_j] = Utest;
        vplot[plot_j] = Vtest;
        zplot[plot_j] = Zpos+dZ;
        time[plot_j] = i;
        
        //if (plot_j%10==0){
        //    astd::cout << "Update U/V/Z plots: " << plot_j << "  " 
        //               << uplot[plot_j] << " " << vplot[plot_j] << " "  << zplot[plot_j] << std::endl;
        //}    
        
        plot_j++;



        if(abs(Zpos + dZ) > CATHODE_APDFACE_DISTANCE)
        {
            //Once you cross the APD plane you can never get back so stop wasting time and kill it
            //I think this Should only happen for events with starting Zpos > Uplane
            //These events are trash though so they can be ignored 
            //So check to make sure started higher than z-plane if not there might be a problem
            if(abs(Zstart) > CATHODE_ANODE_x_DISTANCE){
                LogEXOMsg("Past the APD plane without hitting anything because started to high. Killing PCD." , EEWarning);
                return;
            }
            else{
                LogEXOMsg("Past the APD plane without hitting anything ??????????????" , EEWarning);
            }
            return;
        }


        //Did it pass into the UPlane close enough to hit a wire
        if(abs(Zpos + dZ) > (CATHODE_ANODE_x_DISTANCE - WIRE_DIAMETER)){
            //See if it hit a wire
            //Convert to u-v plane to check distance to wires
            double Uend;  //Hold the final Uposition of the track
            double Vend;  //Hold the final Vposition of the track
            EXOMiscUtil::XYToUVCoords(Uend, Vend, Xpos+dX, Ypos+dY, Zpos+dZ);
            
            //Get the u-distance away from the closest UWire
            //Wires are seperated by WIRE_PITCH
            //Should be between 0 and WIRE_PITCH/2
            double Umod = std::fmod(UChannelsToUse[0].fPosition - Uend, WIRE_PITCH);
            if(Umod < 0) Umod+=WIRE_PITCH; //because of how fmod handles negative numerators 
            if(Umod > WIRE_PITCH/2.0) Umod = WIRE_PITCH - Umod; //Reverse the direction
            if(Umod >= WIRE_PITCH)
            {
                LogEXOMsg("Our remainder prescription seems to have failed -- how?", EEAlert);
                return;
            }

            //Is it within one wire radius away from a Uwire
            if(std::sqrt(std::pow(Umod, 2) +
               std::pow(abs(Zpos + dZ)  - CATHODE_ANODE_x_DISTANCE, 2)) < WIRE_DIAMETER)
            {
                //Close enought to a wire to HIT A U-WIRE!!!
                if(Pixel) {
                    Pixel->fWireHitTime = i*SAMPLE_TIME_HIGH_BANDWIDTH;
                    //Get the Reference Uchannel that just got hit 
                    //Shift back into the real Uchannel that got hit
                    int hitUCh, holdV;
                    double holdX, holdY;
                    
                    //double Uhere;
                    //double Vhere;
                    //EXOMiscUtil::XYToUVCoords(Uhere, Vhere, Xpos+dX, Ypos+dY, abs(Zpos+dZ));
                    //std::cout << "------------------------------" << std::endl;
                    //std::cout << " Try1 Uhere = " << Uhere << " Uend - Uhere = " << Uend - Uhere << std::endl;
                    //EXOMiscUtil::XYToUVCoords(Uhere, Vhere, Xpos+dX, Ypos+dY, Zpos+dZ);
                    //std::cout << " Try2 Uhere = " << Uhere << " Uend - Uhere = " << Uend - Uhere << std::endl;
                    //std::cout << "------------------------------" << std::endl;

                    GetClosestChannel(Xpos+dX, Ypos+dY, Zpos+dZ, hitUCh, holdV, holdX, holdY);
                    hitUCh = URefCh + (hitUCh%NCHANNEL_PER_WIREPLANE  - fRefCh);

                    //int gap;
                    //if(Zpos > 0)
                    //{
                    //    gap = hitUCh - 19;
                    //}
                    //else if(Zpos < 0)
                    //{
                    //    gap = hitUCh - 95;
                    //}
                    //double ch_pos_print = (0.5+gap)*CHANNEL_WIDTH ;
                    //std::cout << " End U Ch = " << hitUCh << " Ch Upos = " << ch_pos_print << " UendRel = " << Uend << " dU = " << Uend-4.5 << std::endl;
                    //std::cout << "Z = " << Zpos+dZ << std::endl; 
                    //std::cout << std::endl;
                    
                    Pixel->fDepositChannel = hitUCh;
                    if(EXOMiscUtil::TypeOfChannel(Pixel->fDepositChannel) != EXOMiscUtil::kUWire or
                       not EXOMiscUtil::OnSameDetectorHalf(Pixel->fDepositChannel, URefCh))  
                    {
                        //std::cout << "Dep 999 hitU = " << hitUCh << " RefU = " << fRefCh << " Actual refU " << URefCh  << std::endl;
                        Pixel->fDepositChannel = 999;
                    }
                }
                // Now fill in the Uwire signals with appropriate values.
                bool HasFoundHitChannel = false; // Make sure we only hit one channel.
                for(size_t isig = 0; isig < UnshapedUSignal.size(); isig++) {
                    double NextWeightPotential = 0.0; // Unless the charge just deposited in this channel...
                    //Is this the channel we hit
                    if((not HasFoundHitChannel) and EXOMiscUtil::TypeOfChannel(Pixel->fDepositChannel) == EXOMiscUtil::kUWire and
                             std::fabs(Uend - UChannelsToUse[isig].fPosition) <= CHANNEL_WIDTH/2.0) {
                        //std::cout << "Found hit Ufinal = " << Uend << " Ch = " << UChannelsToUse[isig].fPosition << " diff is " << std::fabs(Uend - UChannelsToUse[isig].fPosition) << std::endl;
                        NextWeightPotential = 1.0;
                        HasFoundHitChannel = true;
                    }

                    //std::cout << "Dep Chan = " << Pixel->fDepositChannel << std::endl;
                    //std::cout << "Ch pos = " << UChannelsToUse[isig].fPosition << "Upos " << Uend << " W = " << UnshapedUSignal[isig] << std::endl;

                    if(NextWeightPotential < .5)
                    {
                        double test = UnshapedUSignal[isig] + Q_free*(fWeightFieldU.GetWeightPotential(EXOMiscUtil::kUWire, UChannelsToUse[isig].fPosition, Xpos+dX, Ypos+dY, Zpos + dZ) - fWeightFieldU.GetWeightPotential(EXOMiscUtil::kUWire, UChannelsToUse[isig].fPosition, Xpos, Ypos, Zpos));
                        //std::cout << "UCh pos = " << UChannelsToUse[isig].fPosition << " Uend =" << Uend << " W final ="  << test << std::endl;
                    }

                    UnshapedUSignal[isig] += Q_free * (NextWeightPotential - fWeightFieldU.GetWeightPotential(EXOMiscUtil::kUWire, UChannelsToUse[isig].fPosition, Xpos, Ypos, Zpos));

                    for(size_t j = i; j < fNSample*BANDWIDTH_FACTOR; j++) {
                        (*(UChannelsToUse[isig].fWaveform))[j] += UnshapedUSignal[isig];
                    }
                
                    //TCanvas *c1 = new TCanvas("c1");
                    //std::cout << " Uend =  " << Uend    << " this channel =  " << UChannelsToUse[isig].fPosition << std::endl;    
                    //((*(UChannelsToUse[isig].fWaveform)).GimmeHist())->Draw();
                    //c1->Update();
                    //string n;
                    //std::cin >> n;



                }
                //Generate for Vwires too
                for(size_t isig = 0; isig < UnshapedVSignal.size(); isig++) {
                    double NextWeightPotential = 0.0; //Hit UWire so all VWires are 0 next
                    UnshapedVSignal[isig] += Q_free * (NextWeightPotential-
                         fWeightFieldV.GetWeightPotential(EXOMiscUtil::kVWire, VChannelsToUse[isig].fPosition, Xpos, Ypos, Zpos));
                    for(size_t j = i; j < fNSample*BANDWIDTH_FACTOR; j++) {
                        (*(VChannelsToUse[isig].fWaveform))[j] += UnshapedVSignal[isig];
                    }

                    //std::cout << "MJ WP " << VChannelsToUse[isig].fPosition << " " << Xpos << " "  <<  Ypos << " " << Zpos << "  "
                    //          << fWeightFieldV.GetWeightPotential(EXOMiscUtil::kVWire, VChannelsToUse[isig].fPosition, Xpos, Ypos, Zpos) << std::endl;

                }

                //std::ostringstream good;
                //good << endl;
                //good << " U = " << Uend << " Vend = " << Vend << " z =" << Zpos;
                //good << " x = " << Xpos << " y = " << Ypos << " z =" << Zpos;
                //LogEXOMsg(" made it " + good.str() , EEWarning);
                //std::cout << "----------------------------------------------------- " << std::endl;
                //std::cout << "Final_Good_Hit " << Uend << " " << Vend << " " << Zpos << std::endl;
                //std::cout << "----------------------------------------------------- " << std::endl;
                
                if (false and abs(zplot[0]) > 150){
                    std::cout << "Do plot and Pause" << std::endl;
                    TCanvas *c1 = new TCanvas("c1");
                    //TGraph *gr = new TGraph(plot_j,vplot,zplot);
                    //TGraph *gr = new TGraph(plot_j,uplot,zplot);  
                    //TGraph *gr = new TGraph(plot_j, time, (*(VChannelsToUse[0].fWaveform)).GetData());
                    //gr->Draw("AC*");
                    //((*(UChannelsToUse[1].fWaveform)).GimmeHist())->Draw();
                    ((*(VChannelsToUse[2].fWaveform)).GimmeHist())->Draw();
                    //((*(VChannelsToUse[1].fWaveform)).GimmeHist())->Draw("SAME");
                    c1->Update();
                
                    //TCanvas *c2 = new TCanvas("c2");
                    //TGraph *gr2 = new TGraph(plot_j,uplot,zplot);
                    //gr2->Draw("AC*");
                    //c2->Update();
                    string n;
                    std::cin >> n;
                }
                
                return;
            
                //end uwire hit manager
            }
            /*
            std::ostringstream bad1;
            bad1 << endl;
            bad1 << " U = " << Uend << " Vend = " << Vend << " z =" << Zpos+dZ;
            LogEXOMsg(" plane but no hit " + bad1.str() , EEWarning);
            */
            //end uplane hit manager
        }

        //No Wires Hit generate signal and update postion to drift the charge
        //Generate unshaped signals for this time.
        for(size_t isig = 0; isig < UnshapedUSignal.size(); isig++) {
            UnshapedUSignal[isig] += Q_free *
                (fWeightFieldU.GetWeightPotential(EXOMiscUtil::kUWire, UChannelsToUse[isig].fPosition, Xpos+dX, Ypos+dY, Zpos + dZ) -
                 fWeightFieldU.GetWeightPotential(EXOMiscUtil::kUWire, UChannelsToUse[isig].fPosition, Xpos, Ypos, Zpos));
            (*(UChannelsToUse[isig].fWaveform))[i] += UnshapedUSignal[isig];
            //std::cout << "Tot Signal at z = " << Zpos << " u = " << Utest << " v= " << Vtest  << " Ch and pos = " << std::endl;
            //std::cout << URefCh << " pos =   " << UChannelsToUse[isig].fPosition << " W =  " <<  UnshapedUSignal[isig]  << std::endl;
        }
        for(size_t isig = 0; isig < UnshapedVSignal.size(); isig++) {
            UnshapedVSignal[isig] += Q_free *
                (fWeightFieldV.GetWeightPotential(EXOMiscUtil::kVWire, VChannelsToUse[isig].fPosition, Xpos+dX, Ypos+dY, Zpos + dZ) -
                 fWeightFieldV.GetWeightPotential(EXOMiscUtil::kVWire, VChannelsToUse[isig].fPosition, Xpos,    Ypos,    Zpos ));
            (*(VChannelsToUse[isig].fWaveform))[i] += UnshapedVSignal[isig];
        }         
        
        /*
        std::ostringstream stream1;
        stream1 << std::endl;
        stream1 << " dx2 = " << dX << " dy2 = " << dY << " dz2 = " << dZ;
        stream1 << std::endl;
        LogEXOMsg("Pos Change2 " + stream1.str(), EEWarning);
        */
        Xpos+= dX;
        Ypos+= dY;
        Zpos+= dZ;
        
        //end time loop
    }
    
    
    //Nothing Hit and Loop ended something went wrong 
    std::ostringstream endpos;
    endpos << std::endl;
    endpos << "final x = " << Xpos << " final y = " << Ypos << " final z =" << Zpos;
    endpos << std::endl;
    //LogEXOMsg("It looks like a waveform ended before charge ever deposited -- bad Efields or too short waveforms?" + spos.str() + endpos.str(), EEWarning);
    LogEXOMsg("It looks like a waveform ended before charge ever deposited -- bad Efields or too short waveforms?", EEWarning); 
}

void EXO3DDigitizeWires::Digitize(EXOWaveformData& WaveformData, EXOMonteCarloData& MonteCarloData)
{

#define PERFORM_TIMING_AND_FUNCTION(func, temp)         \
    if (fTimingInfo) fTimingInfo->StartTimerForTag(#func);\
    func(temp);                                           \
    if (fTimingInfo) fTimingInfo->StopTimerForTag(#func);
  
    // Intialize the signals, zeroing them out
    PERFORM_TIMING_AND_FUNCTION(ResetWires, );

    // Following is a value that will create *under 1* ADC count.  We will ignore these.
    double adc_limit = ADC_FULL_SCALE_ENERGY_WIRE/ADC_BITS;

    if(fTimingInfo) fTimingInfo->StartTimerForTag("GenerateWireSignalAt");

    if(fDiffusionDuringDrifting and (fNumDiffusePCDs < 1 or fTransverseDiffusionCoeff < 0. or fLongitudinalDiffusionCoeff < 0.))
    {
        LogEXOMsg("Check Diffusion Parameters", EEWarning);
    }

    //Need at least 1 PCD to drift
    if(fNumDiffusePCDs < 1){
        fNumDiffusePCDs = 1;
    }
    
    vector<EXOMCPixelatedChargeDeposit> OriginalPCDs;

    // Loop over PCDs (breaking each up into diffuse PCDs)
    // or store them in a vector so they can be split and digitized later (with diffusion)
    for(size_t i = 0; i < MonteCarloData.GetNumPixelatedChargeDeposits(); i++) {
        EXOMCPixelatedChargeDeposit* PixelDeposit = MonteCarloData.GetPixelatedChargeDeposit(i);
        // If the pixel deposit deposits less energy than will even register one
        // ADC count, ignore it.
        if (PixelDeposit->fTotalIonizationEnergy <= adc_limit) continue;
        // If the pixel deposit is outside the wire hexagon, ignore it.
        EXOWireCrossing& crossing = EXOWireCrossing::GetInstance();
        if(crossing.GetDistanceFromHexagon(PixelDeposit->GetPixelCenter()) > 0){
            continue;
        }
        double OriginalEnergy = PixelDeposit->fTotalEnergy;
        double OriginalIonizationEnergy = PixelDeposit->fTotalIonizationEnergy;
        unsigned int NumSmallPCDs = 1; //If no Diffusion just use the one true PCD
        if(fDiffusionDuringDrifting){ 
            //Divide MC PCD into smaller PCDs
            //fNumDiffusePCDs = keV per PCD
            NumSmallPCDs = TMath::CeilNint((OriginalEnergy/CLHEP::keV)/fNumDiffusePCDs);
        }
        //std::cout << "Num now = " << NumSmallPCDs << " E " << OriginalEnergy/CLHEP::keV << " keV = " << CLHEP::keV   << std::endl;

        //Changed so now fNumDiffusePCDs is actually energy per 1 diffuse pcd 
        //fNumDiffusePCDs = 10 would be 1 PCD per 10keV
        
        //PixelDeposit->fTotalEnergy = OriginalEnergy / double(fNumDiffusePCDs);
        //PixelDeposit->fTotalIonizationEnergy = OriginalIonizationEnergy / double(fNumDiffusePCDs);
        
        PixelDeposit->fTotalEnergy = OriginalEnergy / double(NumSmallPCDs);
        PixelDeposit->fTotalIonizationEnergy = OriginalIonizationEnergy / double(NumSmallPCDs);
        //Digitize Each of these Diffuse PCDs
        //std::cout << NumSmallPCDs << std::endl;
        for(size_t j=0; j<NumSmallPCDs; j++){
            //Drift charge to generate signals (fills fDdata)
            GenerateUnshapedSignals(PixelDeposit);
            PixelDeposit->AddPCDUHit(PixelDeposit->fDepositChannel, PixelDeposit->fTotalIonizationEnergy);
        }
        PixelDeposit->fTotalEnergy = OriginalEnergy;
        PixelDeposit->fTotalIonizationEnergy = OriginalIonizationEnergy;
    }
    
    if(fTimingInfo) fTimingInfo->StopTimerForTag("GenerateWireSignalAt");
    
    
    //PlotWireSignals();
    
    //Do shaping, sampling, add noise, digitization.
    PERFORM_TIMING_AND_FUNCTION(ShapeWireSignals, );
    PERFORM_TIMING_AND_FUNCTION(DoADCSamplingWires, );
    PERFORM_TIMING_AND_FUNCTION(AddNoiseToWireSignals, );
    PERFORM_TIMING_AND_FUNCTION(ScaleAndDigitizeWireSignals, WaveformData);

}


void EXO3DDigitizeWires::GetClosestChannel(double Xpos, double Ypos, double Zpos, int &Uch, int &Vch, double& xrel, double& yrel)
{
    //Given the x,y,z position of a Pixel Find the closest U and V channel
    //Return the relative x and y position of the pixel shifted into the reference channels grid
    //Reference channels are located at U = 4.5mm, V = 4.5mm
    //So returns a relative U/V postion beetween 0mm and 9mm 
    
    double Upos;
    double Vpos;
    EXOMiscUtil::XYToUVCoords(Upos, Vpos, Xpos, Ypos, Zpos);
 
    // Which channel is closest?
    Vch = (int)(Vpos/CHANNEL_WIDTH) + NCHANNEL_PER_WIREPLANE/2;
    if(Vpos < 0) Vch--; // round down, not towards zero.
    if(Vch < 0) Vch = 0;
    if(Vch >= NCHANNEL_PER_WIREPLANE) Vch = NCHANNEL_PER_WIREPLANE-1;
    Vch += (Zpos >= 0 ? NCHANNEL_PER_WIREPLANE : 3*NCHANNEL_PER_WIREPLANE); // North vs. South plane.

    //Relative positon with respect to closest wire channel
    double RelativeVpos = Vpos - (Vch%NCHANNEL_PER_WIREPLANE - NCHANNEL_PER_WIREPLANE/2)*CHANNEL_WIDTH;

    //Which channel is closest?
    Uch = (int)(Upos/CHANNEL_WIDTH) + NCHANNEL_PER_WIREPLANE/2;
    if(Upos < 0) Uch--; // round down, not towards zero.
    if(Uch < 0) Uch = 0;
    if(Uch >= NCHANNEL_PER_WIREPLANE) Uch = NCHANNEL_PER_WIREPLANE-1;
    Uch += (Zpos >= 0 ? 0 : 2*NCHANNEL_PER_WIREPLANE); // North vs. South plane.


    //Realtive position with respect to closest wire channel
    double RelativeUpos = Upos - (Uch%NCHANNEL_PER_WIREPLANE - NCHANNEL_PER_WIREPLANE/2)*CHANNEL_WIDTH;

    if(RelativeUpos - 4.5 > 4.5)
    {

        std::cout << std::endl;
        std::cout << "------------------REL U POST NEVER MORE THAN 4.5 or went wrong-----------------------" << std::endl;
        //std::cout << "Predicted Channel at " << (Uch%NCHANNEL_PER_WIREPLANE - NCHANNEL_PER_WIREPLANE/2)*CHANNEL_WIDTH << std::endl;
        std::cout << "Tested U = " << Upos << " Rel = "  << RelativeUpos - 4.5 << std::endl;
        //Shift into the x-y position with respect to the reference channael 
        
    }
    

    EXOMiscUtil::UVToXYCoords(RelativeUpos, RelativeVpos, xrel, yrel, Zpos);
    return;
}








