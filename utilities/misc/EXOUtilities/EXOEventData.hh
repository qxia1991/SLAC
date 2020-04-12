#ifndef EXOEventData_hh
#define EXOEventData_hh

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOEventData                                                         //
//                                                                      //
// Encapsulation class for Event Data in EXO.                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOEventHeader_hh
#include "EXOEventHeader.hh"
#endif
#ifndef EXOWaveformData_hh
#include "EXOWaveformData.hh"
#endif
#ifndef EXOScintillationCluster_hh
#include "EXOScintillationCluster.hh"
#endif
#ifndef EXOChargeCluster_hh
#include "EXOChargeCluster.hh"
#endif
#ifndef EXOUWireSignal_hh
#include "EXOUWireSignal.hh"
#endif
#ifndef EXOUWireInductionSignal_hh
#include "EXOUWireInductionSignal.hh"
#endif
#ifndef EXOVWireSignal_hh
#include "EXOVWireSignal.hh"
#endif
#ifndef EXOAPDSignal_hh
#include "EXOAPDSignal.hh"
#endif
#ifndef EXOChargeInjectionSignal_hh
#include "EXOChargeInjectionSignal.hh"
#endif
#ifndef EXOMonteCarloData_hh
#include "EXOMonteCarloData.hh"
#endif
#ifndef EXOTClonesArray_hh
#include "EXOTClonesArray.hh"
#endif
#include "TBits.h"
#ifndef EXODelegates_hh
#include "EXODelegates.hh"
#endif
#include <cstddef> //for size_t

class EXOEventData : public TObject
{

  public:
    EXOEventHeader       fEventHeader;      // Header information
    EXOMonteCarloData    fMonteCarloData;   // encapsulates MC data.
   
    Int_t     fRunNumber;                   //nr    : Run number
    Int_t     fEventNumber;                 //ne    : Unique event number.
    Bool_t    fHasSaturatedChannel;         //sat_chan : true when at least one channel is saturated.  Set by recon.
    Bool_t    fSkippedByClustering;         // true when event was skipped by the clustering module.

    Double_t  GetTotalPurityCorrectedEnergy() const;

  public:

    // Default constructor
    EXOEventData(); 
    EXOEventData( const EXOEventData& );
    EXOEventData& operator=( const EXOEventData& ); 
    virtual ~EXOEventData(); 
    
    //*********************************************
    const EXOTClonesArray* GetUWireSignalArray() const;
    EXOUWireSignal* GetUWireSignal( size_t i ); 
    const EXOUWireSignal* GetUWireSignal( size_t i ) const; 
    EXOUWireSignal* GetNewUWireSignal();
    size_t GetNumUWireSignals() const;                 //nusig : number of u-wire signals 
    //*********************************************

    //*********************************************
    const EXOTClonesArray* GetUWireInductionSignalArray() const;
    EXOUWireInductionSignal* GetUWireInductionSignal( size_t i ); 
    const EXOUWireInductionSignal* GetUWireInductionSignal( size_t i ) const; 
    EXOUWireInductionSignal* GetNewUWireInductionSignal();
    size_t GetNumUWireInductionSignals() const;       //number of u-wire induction signals
    //*********************************************
      
    //*********************************************
    const EXOTClonesArray* GetVWireSignalArray() const;
    EXOVWireSignal* GetVWireSignal( size_t i ); 
    const EXOVWireSignal* GetVWireSignal( size_t i ) const; 
    EXOVWireSignal* GetNewVWireSignal();
    size_t GetNumVWireSignals() const;                 //number of v-wire signals 
    //*********************************************

    //*********************************************
    const EXOTClonesArray* GetAPDSignalArray() const; 
    EXOAPDSignal* GetAPDSignal( size_t i ); 
    const EXOAPDSignal* GetAPDSignal( size_t i ) const; 
    EXOAPDSignal* GetNewAPDSignal();
    size_t GetNumAPDSignals() const;                  // number of registered APD signals
    int FindAPDSignalLocation(EXOAPDSignal::EXOAPDSignal_type type, int pos = 0, int n = -1) const;
    //*********************************************

    //*********************************************
    const EXOTClonesArray* GetChargeInjectionSignalArray() const; 
    EXOChargeInjectionSignal* GetChargeInjectionSignal( size_t i ); 
    const EXOChargeInjectionSignal* GetChargeInjectionSignal( size_t i ) const; 
    EXOChargeInjectionSignal* GetNewChargeInjectionSignal();
    size_t GetNumChargeInjectionSignals() const;                  // number of registered charge injection signals
    //*********************************************

    //*********************************************
    const EXOTClonesArray* GetChargeClusterArray() const; 
    EXOChargeCluster* GetChargeCluster( size_t i ); 
    const EXOChargeCluster* GetChargeCluster( size_t i ) const; 
    EXOChargeCluster* GetNewChargeCluster();
    size_t GetNumChargeClusters() const;             //ncl : number of charge clusters
    //*********************************************

    //*********************************************
    const EXOTClonesArray* GetScintillationClusterArray() const; 
    EXOScintillationCluster* GetScintillationCluster( size_t i ); 
    const EXOScintillationCluster* GetScintillationCluster( size_t i ) const; 
    EXOScintillationCluster* GetNewScintillationCluster();
    size_t GetNumScintillationClusters() const;      //nsc: number of scintillation clusters
    //*********************************************

    void Remove(EXOAPDSignal* apd);
    void Remove(EXOUWireSignal* uwire);
    void Remove(EXOUWireInductionSignal* uwireind);
    void Remove(EXOVWireSignal* vwire);
    void Remove(EXOChargeInjectionSignal* chargeinjectionsignal);
    void Remove(EXOChargeCluster* charge);
    void Remove(EXOScintillationCluster* scint);

    void ResetForReconstruction(); 

    virtual void Clear( Option_t* option = "");

    EXOWaveformData* GetWaveformData();
    const EXOWaveformData* GetWaveformData() const;   

    //******************************************************
    enum TypeOfNoiseTag {
      // DO NOT CHANGE EXISTING VALUES!
      kSummedWiresWentNegative = 0,
      kManySaturatedWaveforms = 1,
      kAPDRingingNoise = 2,
      kAPDBouncingNoise = 3
    };
    bool IsTaggedAsNoise() const;
    bool IsTaggedAsNoiseOfType(TypeOfNoiseTag type) const;
    bool IsTaggedAsNoise_Excluding(TypeOfNoiseTag type) const;
    void SetNoiseTag(TypeOfNoiseTag type);
    void ClearNoiseTag(TypeOfNoiseTag type);
    void ClearNoiseTags();
    //******************************************************

    bool operator==(const EXOEventData& ed) const;

  protected:

    // InitializeArrays is called to properly initialize the 
    // TClonesArrays 
    void InitializeArrays();
    void InitializeUWires();
    void InitializeUWiresInduction();
    void InitializeVWires();
    void InitializeChargeInjectionSignals();
    void InitializeChargeClusters();
    void InitializeScintillationClusters();
    void InitializeAPDs();
    EXOTClonesArray* GetScintillationClusterArray(); 
    EXOTClonesArray* GetChargeClusterArray(); 
    EXOTClonesArray* GetChargeInjectionSignalArray();
    EXOTClonesArray* GetAPDSignalArray(); 
    EXOTClonesArray* GetUWireSignalArray();
    EXOTClonesArray* GetUWireInductionSignalArray();
    EXOTClonesArray* GetVWireSignalArray();

   
    // Reconstructed u-wire signal info
    TClonesArray*     fUWires;                   //->Pointer to reconstructed u-wires for this event

    // Reconstructed u-wire induction signal info
    TClonesArray*     fUWiresInduction;          //->Pointer to reconstructed u-wire induction for this event 
       
    // Reconstructed v-wire signal info
    TClonesArray*     fVWires;                   //->Pointer to reconstructed v-wires for this event
                                                                 
    // Reconstructed u-wire signal info
    TClonesArray*     fAPDs;                     //->Pointer to reconstructed APDs for this event

    // Reconstructed charge injection signals
    TClonesArray*     fChargeInjectionSignals;   //->Pointer to reconstructed charge injection signals

    // Reconstructed charge cluster info                         
    TClonesArray*     fChargeClusters;           //->Pointer to reconstructed charge clusters
    
    // Reconstructed scintillation signals
    TClonesArray*     fScintClusters;            //->Pointer to reconstructed scintillation clusters
    
    EXOWaveformData     fWaveformData;                        //Waveform Data

    TBits     fNoiseTags;

  EXO_DEFINE_DELEGATED_FUNCTION(EXOEventData, bool, IsVetoed)

  ClassDef(EXOEventData,15)
};

//---- inlines -----------------------------------------------------------------

inline EXOWaveformData* EXOEventData::GetWaveformData() 
{ 
  // Returns waveform data.  
  return &fWaveformData;
}

inline const EXOWaveformData* EXOEventData::GetWaveformData() const 
{ 
  // Returns waveform data.  
  return &fWaveformData;
}

inline EXOTClonesArray* EXOEventData::GetUWireSignalArray() 
{ 
  // Get array of U Wires.  Normally access to the array should be
  // through the access functions  GetUWireSignal and  
  // GetNumUWireSignals.
  return static_cast< EXOTClonesArray* >( fUWires ); 
}

inline EXOTClonesArray* EXOEventData::GetUWireInductionSignalArray() 
{ 
  // Get array of u-wire induction signals.  Normally access to the array should
  // be through the access functions GetUWireInductionSignal and  
  // GetNumUWireInductionSignals.
  return static_cast< EXOTClonesArray* >( fUWiresInduction ); 
}

inline EXOTClonesArray* EXOEventData::GetChargeInjectionSignalArray() 
{ 
  // Get array of charge injection.  Normally access to the array should be
  // through the access functions  GetChargeInjectionSignal and  
  // GetNumChargeInjectionSignals.
  return static_cast< EXOTClonesArray* >( fChargeInjectionSignals ); 
}

inline EXOTClonesArray* EXOEventData::GetVWireSignalArray() 
{ 
  // Get array of V Wires.  Normally access to the array should be
  // through the access functions  GetVWireSignal and  
  // GetNumVWireSignals.
  return static_cast< EXOTClonesArray* >( fVWires ); 
}

inline const EXOTClonesArray* EXOEventData::GetUWireSignalArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fUWires ); 
}

inline const EXOTClonesArray* EXOEventData::GetUWireInductionSignalArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fUWiresInduction ); 
}

inline const EXOTClonesArray* EXOEventData::GetChargeInjectionSignalArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fChargeInjectionSignals ); 
}

inline const EXOTClonesArray* EXOEventData::GetVWireSignalArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fVWires ); 
}

inline EXOUWireSignal* EXOEventData::GetUWireSignal( size_t i ) 
{ 
  // Get U Wire object at index i.  The general mechanism to loop over signals
  // in a wire is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumUWireSignals(); i++ ) {
  //   EXOUWireSignal* sig = ED->GetUWireSignal(i);
  //   Double_t baseline = sig->fBaseline.
  //   ...
  // }
  //
  // See documentation of EXOUWireSignal for information about
  // access functions and included data members.
  return static_cast< EXOUWireSignal* >( GetUWireSignalArray()->At(i) ); 
}

inline EXOUWireInductionSignal* EXOEventData::GetUWireInductionSignal( size_t i ) 
{ 
  // Get u-wire induction object at index i.  The general mechanism to loop over
  // signals in a wire is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumUWireInductionSignals(); i++ ) {
  //   EXOUWireInductionSignal* sig = ED->GetUWireInductionSignal(i);
  //   Double_t baseline = sig->fBaseline.
  //   ...
  // }
  //
  // See documentation of EXOUWireInductionSignal for information about
  // access functions and included data members.
  return static_cast< EXOUWireInductionSignal* >( GetUWireInductionSignalArray()->At(i) ); 
}

inline EXOChargeInjectionSignal* EXOEventData::GetChargeInjectionSignal( size_t i ) 
{ 
  // Get charge injection signal object at index i.  The general mechanism to loop over charge injection signals
  // is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumChargeInjectionSignals(); i++ ) {
  //   EXOChargeInjectionSignal* sig = ED->GetChargeInjectionSignal(i);
  //   Double_t baseline = sig->fBaseline.
  //   ...
  // }
  //
  // See documentation of EXOChargeInjectionSignal for information about
  // access functions and included data members.
  return static_cast< EXOChargeInjectionSignal* >( GetChargeInjectionSignalArray()->At(i) ); 
}

inline EXOVWireSignal* EXOEventData::GetVWireSignal( size_t i ) 
{ 
  // Get V Wire object at index i. For the general mechanism to loop over signals
  // see at GetUWireSignal():
  return static_cast< EXOVWireSignal* >( GetVWireSignalArray()->At(i) ); 
}

inline const EXOUWireSignal* EXOEventData::GetUWireSignal( size_t i ) const 
{ 
  return static_cast< const EXOUWireSignal* >( GetUWireSignalArray()->At(i) ); 
}

inline const EXOUWireInductionSignal* EXOEventData::GetUWireInductionSignal( size_t i ) const 
{ 
  return static_cast< const EXOUWireInductionSignal* >( GetUWireInductionSignalArray()->At(i) ); 
}

inline const EXOChargeInjectionSignal* EXOEventData::GetChargeInjectionSignal( size_t i ) const 
{ 
  return static_cast< const EXOChargeInjectionSignal* >( GetChargeInjectionSignalArray()->At(i) ); 
}

inline const EXOVWireSignal* EXOEventData::GetVWireSignal( size_t i ) const 
{ 
  return static_cast< const EXOVWireSignal* >( GetVWireSignalArray()->At(i) ); 
}

inline EXOUWireSignal* EXOEventData::GetNewUWireSignal()
{ 
  // Get a new reconstructed u-wire signal.  This is called when a new wire
  // signal is to be loaded into EXOEventData.  For example, a user would do for
  // a new wire signal to be loaded:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOUWireSignal* wireSignal =
  //   ED->GetNewUWireSignal(); 
  //
  // EXOChargeCluster* chargeCluster = clusterIFound;
  // wireSignal->fChannel = channelIFound;
  // wireSignal->SetChargeCluster( chargeCluster );
  //
  // ...
  //
  // See documentation of EXOUWireSignal for information about
  // access functions and included data members.
  return static_cast<EXOUWireSignal*>(
    GetUWireSignalArray()->GetNewOrCleanedObject( GetNumUWireSignals() ) );
}

inline EXOUWireInductionSignal* EXOEventData::GetNewUWireInductionSignal()
{ 
  // Get a new reconstructed u-wire induction signal.  This is called when a new
  // wire signal is to be loaded into EXOEventData.  For example, a user would
  // do for a new wire signal to be loaded:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOUWireInductionSignal* wireSignal =
  //   ED->GetNewUWireInductionSignal(); 
  //
  // ...
  //
  // See documentation of EXOUWireInductionSignal for information about
  // access functions and included data members.
  return static_cast<EXOUWireInductionSignal*>
    ( GetUWireInductionSignalArray()->GetNewOrCleanedObject( GetNumUWireInductionSignals() ) );
}

inline EXOChargeInjectionSignal* EXOEventData::GetNewChargeInjectionSignal()
{ 
  // Get a new reconstructed charge injection signal.  This is called when a new charge injection
  // signal is to be loaded into EXOEventData.  For example, a user would do for
  // a new charge injection signal to be loaded:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOChargeInjectionSignal* chargeInjectionSignal =
  //   ED->GetNewChargeInjectionSignal(); 
  //
  // chargeInjectionSignal->fChannel = channelIFound;
  //
  // ...
  //
  // See documentation of EXOChargeInjectionSignal for information about
  // access functions and included data members.
  return static_cast<EXOChargeInjectionSignal*>(
    GetChargeInjectionSignalArray()->GetNewOrCleanedObject( GetNumChargeInjectionSignals() ) );
}

inline EXOVWireSignal* EXOEventData::GetNewVWireSignal()
{ 
  // Get a new reconstructed v-wire signal. See GetNewUWireSignal() for more info.
  return static_cast<EXOVWireSignal*>(
    GetVWireSignalArray()->GetNewOrCleanedObject( GetNumVWireSignals() ) );
}

inline size_t EXOEventData::GetNumUWireSignals() const 
{ 
  // Get the number of reconstructed U Wires in an event.
  return ( fUWires ) ? fUWires->GetEntriesFast() : 0; 
}

inline size_t EXOEventData::GetNumUWireInductionSignals() const 
{ 
  // Get the number of reconstructed u-wire induction signals in an event.
  return ( fUWiresInduction ) ? fUWiresInduction->GetEntriesFast() : 0; 
}

inline size_t EXOEventData::GetNumChargeInjectionSignals() const 
{ 
  // Get the number of reconstructed charge injection signals in an event.
  return ( fChargeInjectionSignals ) ? fChargeInjectionSignals->GetEntriesFast() : 0; 
}

inline size_t EXOEventData::GetNumVWireSignals() const 
{ 
  // Get the number of reconstructed V Wires in an event.
  return ( fVWires ) ? fVWires->GetEntriesFast() : 0; 
}

inline EXOTClonesArray* EXOEventData::GetChargeClusterArray() 
{ 
  // Get array of Charge clusters.  Normally access to the array should be
  // through the access functions GetChargeCluster and  
  // GetNumChargeClusters.
  return static_cast< EXOTClonesArray* >( fChargeClusters ); 
}

inline const EXOTClonesArray* EXOEventData::GetChargeClusterArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fChargeClusters ); 
}

inline EXOChargeCluster* EXOEventData::GetChargeCluster( size_t i ) 
{ 
  // Get charge cluster object at index i.  The general mechanism to loop over
  // charge clusters is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumChargeClusters(); i++ ) {
  //   EXOChargeCluster* sig = ED->GetChargeCluster(i);
  //   Double_t xpos = sig->fX;
  //   ...
  // }
  //
  // See documentation of EXOChargeCluster for information about
  // access functions and included data members.

  return static_cast< EXOChargeCluster* >( GetChargeClusterArray()->At(i) ); 
}

inline const EXOChargeCluster* EXOEventData::GetChargeCluster( size_t i ) const 
{ 
  return static_cast< const EXOChargeCluster* >( GetChargeClusterArray()->At(i) ); 
}

inline EXOChargeCluster* EXOEventData::GetNewChargeCluster()
{ 
  // Get a new reconstructed charge cluster.  This is called when a new charge
  // cluster is to be loaded into EXOEventData.  For example, a user would do:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOChargeCluster* cluster =
  //   ED->GetNewChargeCluster(); 
  //
  // EXOUWireSignal* uwire_signal; // Assuming we have a filled
  //                                            // wire signal
  // cluster->fX = xIfound;
  // cluster->fDriftTime = timeIFound;
  // cluster->SetUSignalOfFirstSignal( uwire_signal );
  // ...
  //
  // See documentation of EXOChargeCluster for information about
  // access functions and included data members.

  return static_cast<EXOChargeCluster*>(
    GetChargeClusterArray()->GetNewOrCleanedObject( GetNumChargeClusters() ) );
}

inline size_t EXOEventData::GetNumChargeClusters() const
{ 
  // Get Number of charge clusters in the event.
  return ( fChargeClusters ) ? fChargeClusters->GetEntriesFast() : 0; 
}

inline EXOTClonesArray* EXOEventData::GetScintillationClusterArray() 
{ 
  // Get array of Scintillation clusters.  Normally access to the array should
  // be through the access functions GetScintillationCluster and
  // GetNumScintillationClusters.
  return static_cast< EXOTClonesArray* >( fScintClusters ); 
}

inline const EXOTClonesArray* EXOEventData::GetScintillationClusterArray() const 
{ 
  // Get array of Scintillation clusters.  Normally access to the array should
  // be through the access functions GetScintillationCluster and
  // GetNumScintillationClusters.
  return static_cast< const EXOTClonesArray* >( fScintClusters ); 
}

inline EXOScintillationCluster* 
  EXOEventData::GetScintillationCluster( size_t i ) 
{ 
  // Get scintillation cluster object at index i.  The general mechanism to
  // loop over scintillation clusters is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumScintillationClusters(); i++ ) {
  //   EXOScintillationCluster* sig = 
  //     ED->GetScintillationCluster(i);
  //   Double_t xpos = sig->fX;
  //   ...
  // }
  //
  // See documentation of EXOScintillationCluster for information
  // about access functions and included data members.
  return static_cast< EXOScintillationCluster* >( 
    GetScintillationClusterArray()->At(i) ); 
}

inline const EXOScintillationCluster* 
  EXOEventData::GetScintillationCluster( size_t i ) const 
{ 
  return static_cast< const EXOScintillationCluster* >( 
    GetScintillationClusterArray()->At(i) ); 
}

inline EXOScintillationCluster* 
  EXOEventData::GetNewScintillationCluster()
{ 
  // Get a new reconstructed scintillation cluster.  This is called when a new
  // scintillation cluster is to be loaded into EXOEventData.  For example, a
  // user would do:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOScintillationCluster* cluster =
  //   ED->GetNewScintillationCluster(); 
  //
  // cluster->fCountsSumOnAPDPlaneTwo = countsIFound;
  // ...
  //
  // See documentation of EXOScintillationCluster for information
  // about access functions and included data members.
  return static_cast<EXOScintillationCluster*>(
    GetScintillationClusterArray()->GetNewOrCleanedObject( 
      GetNumScintillationClusters() ) );
}

inline size_t EXOEventData::GetNumScintillationClusters() const 
{ 
  // Get Number of charge clusters in the event.
  return ( fScintClusters ) ? fScintClusters->GetEntriesFast() : 0; 
}

inline EXOTClonesArray* EXOEventData::GetAPDSignalArray() 
{ 
  // Get array of APD events.  Normally access to the array should be
  // through the access functions GetAPDArray and  
  // GetNumAPDs.
  return static_cast< EXOTClonesArray* >( fAPDs ); 
}

inline const EXOTClonesArray* EXOEventData::GetAPDSignalArray() const
{ 
  return static_cast< const EXOTClonesArray* >( fAPDs ); 
}

inline EXOAPDSignal* EXOEventData::GetAPDSignal( size_t i ) 
{ 
  // Get APD signal object at index i.  The general mechanism to loop over APD
  // signals is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // for (size_t i=0; i<ED->GetNumAPDSignals(); i++ ) {
  //   EXOAPDSignal* apd = ED->GetAPDSignal(i);
  //   Double_t chi_square_gang = apd->fChiSquareGang;
  //   ...
  // }
  //
  // See documentation of EXOAPDSignal for information about access functions
  // and included data members.

  return static_cast< EXOAPDSignal* >( GetAPDSignalArray()->At(i) ); 
}

inline const EXOAPDSignal* EXOEventData::GetAPDSignal( size_t i ) const 
{ 
  return static_cast< const EXOAPDSignal* >( GetAPDSignalArray()->At(i) ); 
}

inline EXOAPDSignal* EXOEventData::GetNewAPDSignal()
{ 
  // Get a new reconstructed APD signal.  This is called when a new APD signal
  // is to be loaded into EXOEventData.  For example, a user would do:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOAPDSignal* apd =
  //   ED->GetNewAPDSignal(); 
  //
  // apd->fCountsSumOnAPDPlaneTwo = countsIFound;
  // ...
  //
  // See documentation of EXOAPDSignal for information about access functions
  // and included data members.

  return static_cast<EXOAPDSignal*>(
    GetAPDSignalArray()->GetNewOrCleanedObject( GetNumAPDSignals() ) );
}

inline size_t EXOEventData::GetNumAPDSignals() const 
{ 
  // Get number of APD Signals in the event
  return ( fAPDs ) ? fAPDs->GetEntriesFast() : 0; 
}

inline void EXOEventData::Remove(EXOAPDSignal* apd)
{
  // Remove APD Signal from the event
  GetAPDSignalArray()->RemoveAndCompress(apd);
}

inline void EXOEventData::Remove(EXOUWireSignal* uwire)
{
  // Remove UWire Signal from the event
  GetUWireSignalArray()->RemoveAndCompress(uwire);
}

inline void EXOEventData::Remove(EXOUWireInductionSignal* uwireind)
{
  // Remove a u-wire induction signal from the event.
  GetUWireInductionSignalArray()->RemoveAndCompress(uwireind);
}

inline void EXOEventData::Remove(EXOChargeInjectionSignal* chargeInjectionSignal)
{
  // Remove Charge Injection Signal from the event
  GetChargeInjectionSignalArray()->RemoveAndCompress(chargeInjectionSignal);
}

inline void EXOEventData::Remove(EXOVWireSignal* vwire)
{
  // Remove VWire Signal from the event
  GetVWireSignalArray()->RemoveAndCompress(vwire);
}

inline void EXOEventData::Remove(EXOChargeCluster* charge)
{
  // Remove Charge cluster from the event
  GetChargeClusterArray()->RemoveAndCompress(charge);
}

inline void EXOEventData::Remove(EXOScintillationCluster* scint)
{
  // Remove Scintillation cluster from the event
  GetScintillationClusterArray()->RemoveAndCompress(scint);
}

inline void EXOEventData::ResetForReconstruction()
{
  // Clears the event data for reconstruction, removing signals and derived
  // clusters.  
  //
  // We must call Delete because we need to make sure destructors are called if
  // any objects exist in these arrays because they were likely created in
  // another process and this messes up TRef(Array)s.  This is a no-op in 
  // data that has empty arrays which is the case when EXOEventData is new
  // or has had Clear("C") called on it. 
  GetUWireSignalArray()->Delete();
  GetUWireInductionSignalArray()->Delete();
  GetVWireSignalArray()->Delete();
  GetChargeInjectionSignalArray()->Delete();
  GetChargeClusterArray()->Delete();
  GetScintillationClusterArray()->Delete();
  GetAPDSignalArray()->Delete();
}
#endif

