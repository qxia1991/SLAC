#ifndef EXOUWireSignal_hh
#define EXOUWireSignal_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOTRefArray_hh
#include "EXOUtilities/EXOTRefArray.hh"
#endif

class EXOChargeCluster;
class EXOUWireSignal : public TObject {
  public:
    Int_t          fChannel;              //usig_ch            : U-wire signal channel number.
    Double_t       fRawEnergy;            //usig_e             : U-wire signal energy in keV. 
    Double_t       fRawEnergyError;       //usig_deltae        : U-wire signal energy uncertainty.  
    Double_t       fCorrectedEnergy;      //usig_ecor          : U-wire signal energy in keV, corrected by analysis modules.
    Double_t       fCorrectedEnergyError; //usig_deltaecor     : U-wire signal corrected energy uncertainty.
    Double_t       fTime;                 //usig_t             : U-wire signal time in nanoseconds.     
    Double_t       fTimeError;            //usig_deltat        : U-wire signal time uncertainty.
    Double_t       fBaseline;             //usig_baseline      : U-wire baseline ADC counts.
    Double_t       fBaselineError;        //usig_deltabaseline : U-wire baseline uncertainty.
    Double_t       fChiSquare;            /*usig_chi2          : U-wire chi^2 from fit (or something approximating it).\
                                                                 Note that the same chi^2 is used for all u-signals on the channel.*/
    Double_t       fChiSquareRestr;       /* U-wire chi^2 from fit, over restricted window (which can differ
					     from the fitting window) */
    Double_t       fChiSquareInd;         // U-wire chi^2 from fit to induction pulse template
    Double_t       fChiSquareIndRestr;    // U-wire chi^2 from fit to induction pulse template (over restricted window)	       
    Double_t       fRisetime;             // Leading edge risetime, as filled by EXORisetimeModule.
    Double_t       fUnshapedInteg;        // Integral of unshaped pulse, filled by EXOUWireInductionModule.
    Double_t       fChiSqRatio;           // Log of the chi2 ratio between deposition and induction fits, 
                                          // filled by EXOUWireInductionModule.
    Double_t       fMaxMinTime;           // Time from pulse maximum to minimum, filled by EXOUWireInductionModule.
    Double_t       fMaxNeighEng;          // Maximum raw energy on neighboring Uwires, filled by 
                                          // EXOUWireInductionModule.
    Bool_t         fIsInduction;          // Flag denoting if U-wire signal is "induction-like," filled by 
                                          // EXOUWireInductionModule
    Double_t       fAPDFilteredWFPeakMagnitude;  // Peak value of the APD filtered waveform in the APD search window preceding the charge cluster.
    Double_t       fAPDFilteredWFPeakTime;   // Time of the above peak value
    Double_t       fAPDFilteredWFThreshold;  // Threshold for the APD filtered waveform in the matched filter
	Double_t fMixSigFitChi2;								// Chi Squared result of the mixed signal fit
	Double_t fMixSigColPar;								//Mixed Signal Collection Parameter
	Double_t fMixSigColErr;									//Mixed Signal Collection Error
	Double_t fMixSigColTime;								//Mixed Signal Collection Time Par
	Double_t fMixSigColErrTime;						//Mixed Signal Collection Time Error
	Double_t fMixSigIndPar;								//Mixed Signal Induction Parameter
	Double_t fMixSigIndErr;									//Mixed Signal Induction Error
	Double_t fMixSigIndTime;								//Mixed Signal Induction Time Par
	Double_t fMixSigIndErrTime;						//Mixed Signal Induction Time Error
	Double_t fMixSigClusterFound;					//Index of Cluster IsInd tag is added to. Set in EXOUWireMixSigAddModule
	Double_t fMixSigUinClustFound;						//Index of the UWire in the Cluster the IsInd tag is closest to.
	Bool_t fMixSigDuplicateTag;						//Set in EXOUWireMisSigAddModule. Says if duplicated IsInd Tag.

    // Get/Set associated charge cluster
    void InsertChargeCluster( EXOChargeCluster* aValue );
    EXOChargeCluster*  GetChargeClusterAt( size_t index );
    const EXOChargeCluster*  GetChargeClusterAt( size_t index ) const;
    size_t GetNumChargeClusters() const;

  public:
    EXOUWireSignal();
    ~EXOUWireSignal();
    virtual void Clear( Option_t* opt = "" ); 
    virtual void RecursiveRemove(TObject* obj);   //Auto remove objects from Wire signals
    virtual void Print( Option_t* opt = "" ) const;
    bool operator==(const EXOUWireSignal& sig) const;

  protected:
    EXOTRefArray      fRefChargeClusters; //usig_ichargecluster : clusters this signal went into.
  ClassDef( EXOUWireSignal, 13 )

};

inline size_t EXOUWireSignal::GetNumChargeClusters() const
{
  // number of charge clusters this u-wire signal went into
  return fRefChargeClusters.GetLast() + 1;
}

#endif /* EXOUWireSignal_hh */
