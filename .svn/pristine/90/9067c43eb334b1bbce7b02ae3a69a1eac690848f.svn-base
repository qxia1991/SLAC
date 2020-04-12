#ifndef EXOChargeCluster_hh
#define EXOChargeCluster_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TRef
#include "TRef.h"
#endif
#ifndef EXOUWireSignal_hh
#include "EXOUtilities/EXOUWireSignal.hh"
#endif
#ifndef EXOVWireSignal_hh
#include "EXOUtilities/EXOVWireSignal.hh"
#endif
#ifndef EXOUWireInductionSignal_hh
#include "EXOUtilities/EXOUWireInductionSignal.hh"
#endif
#ifndef EXOTRefArray_hh
#include "EXOUtilities/EXOTRefArray.hh"
#endif
#include <cstddef> //for size_t
class EXOScintillationCluster;

class EXOChargeCluster : public TObject {
  public:
    Double_t       fX;              //xcl  : x position of the cluster, in millimeters.     
    Double_t       fY;              //ycl  : y position of the cluster, in millimeters.
    Double_t       fU;              //ucl  : u position of the cluster, in millimeters.
    Double_t       fURMSFoundSignals; //     weighted RMS of the found u signals in the charge cluster, in millimeters.
    Double_t       fV;              //vcl  : v position of the cluster, in millimeters.
    Double_t       fVRMSFoundSignals; //     weigthed RMS of the found v signals in the charge cluster, in millimeters.
    Double_t       fZ;              //zcl  : z position of the cluster, in millimeters.
    Double_t       fZRMSFoundSignals; //     weighted RMS of the z-position of found u signals in the charge cluster, in millimeters.
    Double_t       fDriftTime;      //dtcl : Drift time of the cluster, in nanoseconds.
    Double_t       fCollectionTime; //tcl  : Collection time of the cluster, in nanoseconds     

    Double_t       fRawEnergy;             //ercl   : Raw cluster energy in keV.
    Double_t       fRawEnergyError;        //erclerr: Statistical error on the energy returned by the fit, in keV.
    Double_t       fCorrectedEnergy;       /*eccl   : Cluster energy corrected for shielding grid\
                                                      inefficiency and U-Wire gains, in keV. (Fixed temporary calibration\
                                                      of 5.601keV/ADC count is used as of 12/16/10 RN.)*/
    Double_t       fAmplitudeInVChannels;             // Sum of the YMatch FOM of all found v-signals, in arbitrary units
    Double_t       fCorrectedAmplitudeInVChannels;    // Gain corrected sum of the YMatch FOM of all found v-signals, in arbitrary units
    Double_t       fPurityCorrectedEnergy; //epcl   : Purity corrected energy
    Double_t       fCorrectedEnergyError;  //eerrcl :  Statistical error on the energy returned by the fit, in keV. (03/29/12: Does not include a contribution from shielding grid correction.)

    Int_t          fDetectorHalf;                 //dhalfcl : Detector half of the cluster; 0 = TPC1, 1 = TPC2, 2 = both, stronger in TPC1, 3 = both, stronger in TPC2.
    Double_t       fEnergyInVChannels;            //evcl    : Energy in the V channels.                 
    Double_t	   fInductionEnergy;		// Sum of magnitudes of U-wire induction signals, keV
    Double_t	   fInductionEnergyError;	// Added in quadratures errors on magnitudes of U-wire induction signals, keV
    Double_t       fSumRisetime;            // Rise time summed over U-wire traces                 
    Double_t       fPurityCorrectedEnergyMix; /*Purity corrected energy with the extra mixed\
                                                signal Energy*/
    Double_t       fCorrectedEnergyMix;     /* Cluster energy corrected for shielding grid\
                                               inefficiency and U-Wire gains, in keV. Including the\
                                               extra mixed Signal Energy*/

    // Get/Set associated scintillation clusters and wire signals
    void SetScintillationCluster( EXOScintillationCluster* aValue );
    EXOScintillationCluster*  GetScintillationCluster(); 
    const EXOScintillationCluster*  GetScintillationCluster() const; 
    bool HaveScintillationCluster() const;

    void InsertUWireSignal( EXOUWireSignal* aValue );
    void InsertVWireSignal( EXOVWireSignal* aValue );
    void InsertUWireInductionSignal( EXOUWireInductionSignal* aValue );
    EXOUWireSignal*  GetUWireSignalAt( size_t index ); //iu#clcs
    EXOVWireSignal*  GetVWireSignalAt( size_t index );
    EXOUWireInductionSignal*  GetUWireInductionSignalAt( size_t index );
    const EXOUWireSignal*  GetUWireSignalAt( size_t index ) const; //iu#clcs
    const EXOVWireSignal*  GetVWireSignalAt( size_t index ) const;
    const EXOUWireInductionSignal*  GetUWireInductionSignalAt( size_t index ) const;
    Int_t GetUWireSignalChannelAt( size_t index ) const;     //iu#cl : channel number of [index] u-wire collection signal.
    Int_t GetVWireSignalChannelAt( size_t index ) const;     //channel number of [index] v-wire collection signal.
    Int_t GetUWireInductionSignalChannelAt( size_t index ) const;     //channel number of [index] u-wire induction signal.
    size_t GetNumUWireSignals() const;                       //ncscl : number of associated u-wire signals
    size_t GetNumVWireSignals() const;                       //number of associated v-wire signals
    size_t GetNumUWireInductionSignals() const;              //number of associated u-wire induction signals

    Bool_t Is3DCluster() const;
    Bool_t IsFiducial() const;

  public:
    EXOChargeCluster();
    virtual ~EXOChargeCluster();
    virtual void Clear( Option_t* opt = "" ); 
    virtual void Print( Option_t* opt = "" ) const;
    bool operator==(const EXOChargeCluster& cc) const;

  protected:
    // TODO copy constructor.
    EXOChargeCluster(const EXOChargeCluster&);
    EXOChargeCluster& operator=(const EXOChargeCluster&);

    TRef           fRefScintCluster;              //isccl   : of scint. clusters where t0 originates; negative if no t0.  
    EXOTRefArray      fRefUWireSignals;           //iu#clcs : array of u-wire signals that went into the cluster.
    EXOTRefArray      fRefVWireSignals;           //array of v-wire signals that went into the cluster.
    EXOTRefArray      fRefUWireInductionSignals;  //array of u-wire induction signals that went into the cluster.
  public:
    virtual void RecursiveRemove(TObject* obj);   //Auto remove objects from Wire signals

  ClassDef( EXOChargeCluster, 11 )

}; 

//---- inlines -----------------------------------------------------------------

inline size_t EXOChargeCluster::GetNumUWireSignals() const
{
  // number of u wire signals contributing to this charge cluster
  return fRefUWireSignals.GetLast() + 1;
}

inline bool EXOChargeCluster::HaveScintillationCluster() const
{
  // do we have scintillation cluster assotiated to this charge cluster
  return (fRefScintCluster.GetObject());
}

inline size_t EXOChargeCluster::GetNumVWireSignals() const
{
  // number of v wire signals contributing to this charge cluster
  return fRefVWireSignals.GetLast() + 1;
}
inline size_t EXOChargeCluster::GetNumUWireInductionSignals() const
{
  // number of u wire signals contributing to this charge cluster
  return fRefUWireInductionSignals.GetLast() + 1;
}
inline void EXOChargeCluster::InsertUWireSignal( EXOUWireSignal* aValue )
{
  // Add a u wire signal that this charge cluster is made of
  if(fRefUWireSignals.IsEmpty()) fRefUWireSignals = EXOTRefArray(TProcessID::GetProcessWithUID(aValue));
  fRefUWireSignals.Add(aValue);
}

inline void EXOChargeCluster::InsertVWireSignal( EXOVWireSignal* aValue )
{
  // Add a v wire signal that this charge cluster is made of
  if(fRefVWireSignals.IsEmpty()) fRefVWireSignals = EXOTRefArray(TProcessID::GetProcessWithUID(aValue));
  fRefVWireSignals.Add(aValue);
}

inline void EXOChargeCluster::InsertUWireInductionSignal( EXOUWireInductionSignal* aValue )
{
  // Add a u wire signal that this charge cluster is made of
  if(fRefUWireInductionSignals.IsEmpty()) fRefUWireInductionSignals = EXOTRefArray(TProcessID::GetProcessWithUID(aValue));
  fRefUWireInductionSignals.Add(aValue);
}

inline EXOUWireSignal* EXOChargeCluster::GetUWireSignalAt( size_t index )
{
  // Get an associated u wire signal
  return static_cast<EXOUWireSignal*>(fRefUWireSignals.At(index));
}

inline EXOVWireSignal* EXOChargeCluster::GetVWireSignalAt( size_t index )
{
  // Get an associated v wire signal
  return static_cast<EXOVWireSignal*>(fRefVWireSignals.At(index));
}

inline EXOUWireInductionSignal* EXOChargeCluster::GetUWireInductionSignalAt( size_t index )
{
  // Get an associated v wire signal
  return static_cast<EXOUWireInductionSignal*>(fRefUWireInductionSignals.At(index));
}

inline const EXOUWireSignal* EXOChargeCluster::GetUWireSignalAt( size_t index ) const
{
  // Get an associated u wire signal
  return static_cast<EXOUWireSignal*>(fRefUWireSignals.At(index));
}

inline const EXOVWireSignal* EXOChargeCluster::GetVWireSignalAt( size_t index ) const
{
  // Get an associated v wire signal
  return static_cast<EXOVWireSignal*>(fRefVWireSignals.At(index));
}

inline const EXOUWireInductionSignal* EXOChargeCluster::GetUWireInductionSignalAt( size_t index ) const
{
  // Get an associated v wire signal
  return static_cast<EXOUWireInductionSignal*>(fRefUWireInductionSignals.At(index));
}

inline Int_t EXOChargeCluster::GetUWireSignalChannelAt( size_t index ) const
{
  // Get the (physical) channel number of the associated u wire signal
  return static_cast<EXOUWireSignal*>(fRefUWireSignals.At(index))->fChannel;
}

inline Int_t EXOChargeCluster::GetVWireSignalChannelAt( size_t index ) const
{
  // Get the (physical) channel number of the associated v wire signal
  return static_cast<EXOVWireSignal*>(fRefVWireSignals.At(index))->fChannel;
}

inline Int_t EXOChargeCluster::GetUWireInductionSignalChannelAt( size_t index ) const
{
  // Get the (physical) channel number of the associated v wire signal
  return static_cast<EXOUWireInductionSignal*>(fRefUWireInductionSignals.At(index))->fChannel;
}
#endif /* EXOChargeCluster_hh */
