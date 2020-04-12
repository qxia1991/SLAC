#ifndef EXOMonteCarloData_hh
#define EXOMonteCarloData_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOMCAPDHitInfo_hh
#include "EXOUtilities/EXOMCAPDHitInfo.hh"
#endif
#ifndef EXOMCVetoPanelHitInfo_hh
#include "EXOUtilities/EXOMCVetoPanelHitInfo.hh"
#endif
#ifndef EXOMCPixelatedChargeDeposit_hh
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#endif
#ifndef EXOMCParticleInformation_hh
#include "EXOUtilities/EXOMCParticleInformation.hh"
#endif
#ifndef EXOMCInteractionInfo_hh
#include "EXOUtilities/EXOMCInteractionInfo.hh"
#endif
#ifndef EXOCoordinateKey_hh
#include "EXOUtilities/EXOCoordinateKey.hh"
#endif
#ifndef EXOTClonesArray_hh
#include "EXOUtilities/EXOTClonesArray.hh"
#endif
#include <cassert>
#include <map>
#include <cstddef> //for size_t

class EXOMonteCarloData : public TObject 
{
  
  // Class to encapsulate all EXO Monte Carlo Data
  public:
    
    Double_t        fBetaDecayQValue;         //qbeta : Beta decay Q value.
    Double_t        fPrimaryEventX;           //x0    : Primary event x position in millimeters.
    Double_t        fPrimaryEventY;           //y0    : Primary event y position in millimeters.
    Double_t        fPrimaryEventZ;           //z0    : Primary event z position in millimeters.
    Int_t           fTotalPhotons;            //totalph : Total number of scintillation photons generated.
    Int_t           fTotalHitsArrayOne;       //hitsp1  : Total number of photons seen by APD array #1.
    Int_t           fTotalHitsArrayTwo;	      //hitsp2  : Total number of photons seen by APD array #2.    
    Int_t           fTotalHitsVetoPanels;	  //hitsveto: Total number of hits seen by the veto panels.    
    Double_t        fEventGroupingTime;       /* : This is the time used to group charge deposits into 
                                                   EXOEventData event objects within the event action. */

    // Monte Carlo energy deposit info
    
    Double_t        fTotalEnergyInSalt;                          //esalt                 : Total energy deposited in the salt.
    Double_t        fTotalEnergyInShield;                        //eshield               : Total energy deposited in the lead.
    Double_t        fTotalEnergyInOuterCryostat;                 //eoutcry               : Total energy deposited in the outer cryostat.
    Double_t        fTotalEnergyInInnerCryostat;                 //eincry                : Total energy deposited in the inner cryostat.
    Double_t        fTotalEnergyInHFE;                           //ehfe                  : Total energy deposited in the HFE.
    Double_t        fTotalEnergyInVessel;                        //evessel               : Total energy deposited in the TPC vessel.
    Double_t        fTotalEnergyInLiquidXe;                      //elxet                 : Total energy deposited in the liquid xenon.
    Double_t        fTotalIonizationEnergyInLiquidXe;            /*elxei                 : Total ionization energy deposited in the liquid \
                                                                                           xenon, accounting for quenching in alpha events.*/
    Double_t        fTotalEnergyOptPhotonsInCathode;             /*ecathode_photons      : Total energy from optical photons deposited\
                                                                                           in the cathode.*/
    Double_t        fTotalEnergyOptPhotonsInBothAnodes;          /*eanodes_photons       : Total energy from optical photons deposited\
                                                                                           in both anodes summed together.*/
    Double_t        fTotalEnergyOptPhotonsInBothWireSupports;    /*ewire_supports_photons: Total energy from optical photons deposited\
                                                                                           in both wire supports summed together.*/
    Double_t        fTotalEnergyOptPhotonsInCathodeRing;         /*ecathode_ring_photons : Total energy from optical photons deposited\
                                                                                           in the cathode ring.*/
    Double_t        fTotalEnergyOptPhotonsInTeflonReflector;     /*ereflector_photons    : Total energy from optical photons deposited\
                                                                                           in the teflon reflector.*/
    Double_t        fTotalEnergyOptPhotonsInBothAPDFrames;       /*eapd_frames_photons   : Total energy from optical photons deposited\
                                                                                           in both APD frames summed together.*/
    Double_t        fTotalEnergyOptPhotonsInLiquidXe;            /*elxe_photons          : Total energy from optical photons absorbed by the\
                                                                                           LXe (both active and inactive LXe absorptions\
                                                                                           summed together).*/
    Double_t        fTotalEnergyOptPhotonsInLiquidXeVessel;      /*evessel_photons       : Total energy from optical photons deposited\
                                                                                           in the LXe vessel.*/
    Double_t        fTotalEnergyOptPhotonsInRemovedAPDs;         /*eremoved_apds_photons : Total energy from optical photons deposited\
                                                                                           in removed APDs.*/
    Double_t        fTotalEnergyOptPhotonsInFieldShapingRings;   //efield_rings_photons  : Total energy from optical photons deposited 
    Double_t        fTotalEnergyInVetoPanels;                    //evetopanels           : Total energy deposited in the veto panels.
   
    Bool_t           fDidBrem;                                    //                      : Did the event include any Bremsstrahlung (yes or no)
    Bool_t           fDidCompton;                                 //                      : Did the event include any Compton Scattering (yes or no)

  public:
    size_t GetNumParticleInformation() const;     // npart : number of particles 
    size_t GetNumPixelatedChargeDeposits() const; // nq : number of pixelated charge deposits 
    size_t GetNumAPDHits() const;                 //napd : number of apd hits
    size_t GetNumVetoPanelHits() const;           //nvetopanel : number of veto panel hits
    size_t GetNumInteractions() const;  // number of interactions

    virtual void InitializeArrays();

    const EXOMCParticleInformation* GetParticleInformation(size_t i) const;
    EXOMCParticleInformation* GetParticleInformation(size_t i);
    const EXOMCAPDHitInfo* GetAPDHit(size_t i) const;
    const EXOMCVetoPanelHitInfo* GetVetoPanelHit(size_t i) const;
    const EXOMCPixelatedChargeDeposit* GetPixelatedChargeDeposit(size_t i) const;
    EXOMCPixelatedChargeDeposit* GetPixelatedChargeDeposit(size_t i);
    const EXOMCInteractionInfo* GetInteraction(size_t i) const;
    EXOMCInteractionInfo* GetInteraction(size_t i);

    EXOMCParticleInformation*    GetNewParticleInformation();
    EXOMCInteractionInfo* GetNewInteraction();

    const EXOMCAPDHitInfo* FindAPDHitInfo(Int_t gangNo, Double_t time) const;
    const EXOMCAPDHitInfo* AddAPDHitInfo(const EXOMCAPDHitInfo& apdHit);
    const EXOMCVetoPanelHitInfo* FindVetoPanelHitInfo(Int_t panelID, Double_t time) const;
    const EXOMCVetoPanelHitInfo* AddVetoPanelHitInfo(const EXOMCVetoPanelHitInfo& vetoPanelHit);

    const EXOTClonesArray* GetAPDHitsArray() const;
    const EXOTClonesArray* GetVetoPanelHitsArray() const;
    const EXOTClonesArray* GetParticleInformationArray() const;
    const EXOTClonesArray* GetPixelatedChargeDepositsArray() const;
    const EXOTClonesArray* GetInteractionArray() const;

    void Remove(EXOMCParticleInformation* partinfo);
    void Remove(EXOMCAPDHitInfo* hitinfo);
    void Remove(EXOMCVetoPanelHitInfo* hitInfo);
    void Remove(EXOMCInteractionInfo* intinfo);

    const EXOMCPixelatedChargeDeposit* FindPixelatedChargeDeposit(const EXOCoordinates& coord) const;
    EXOMCPixelatedChargeDeposit* FindOrCreatePixelatedChargeDeposit(const EXOCoordinates& coord);

  protected:

    EXOMCAPDHitInfo* GetNewAPDHit();
    EXOMCAPDHitInfo* FindOrCreateAPDHitInfo(Int_t gangNo, Double_t time);
    EXOMCVetoPanelHitInfo* GetNewVetoPanelHit();
    EXOMCVetoPanelHitInfo* FindOrCreateVetoPanelHitInfo(Int_t panelID, Double_t time);

    EXOTClonesArray* GetAPDHitsArray();
    EXOTClonesArray* GetVetoPanelHitsArray();
    EXOTClonesArray* GetParticleInformationArray();
    EXOTClonesArray* GetPixelatedChargeDepositsArray();
    EXOTClonesArray* GetInteractionArray();
    void InitializeParticleInformation();
    void InitializeAPDHits();
    void InitializePixelatedChargeDeposits();
    void InitializeVetoPanelHits();
    void InitializeInteractions();

    TClonesArray*   fParticleInformation;     //->Array of Primary and secondary generated particles
    TClonesArray*   fAPDHits;                 //->Array of Monte Carlo APD photon counting
    TClonesArray*   fPixelatedChargeDeposits; //->Array of MC pixelated charge deposits
    TClonesArray*   fVetoPanelHits;           //->Array of Monte Carlo veto panel data
    TClonesArray*   fInteractions;         //->Array of Interactions in the LXe 

    typedef std::pair<Long64_t, Int_t> HitPair;
    typedef std::map<HitPair, EXOMCAPDHitInfo*> APDMap;
    mutable APDMap  fAPDMap;                  //! Map of APD hits in the TClonesArray.  This is *not* saved to disk.

    Long64_t ConvertTime( Double_t time ) const;
    void CheckAPDMap() const;
    void FillAPDMap( EXOMCAPDHitInfo* hit ) const;

    typedef std::map<EXOCoordinateKey, EXOMCPixelatedChargeDeposit*> PCDMap;
    mutable PCDMap  fPCDMap;                  //! Map of PCD hits in the TClonesArray, not saved to disk
    void CheckPCDMap() const;
    void FillPCDMap( EXOMCPixelatedChargeDeposit* pcd ) const;

    typedef std::map<HitPair, EXOMCVetoPanelHitInfo*> VetoPanelMap;
    mutable VetoPanelMap fVetoPanelMap;       //! Map of veto panel hits in the TClonesArray. This is not saved to disk.
    void CheckVetoPanelMap() const;
    void FillVetoPanelMap( EXOMCVetoPanelHitInfo* hit ) const;


  public:
    EXOMonteCarloData();
    EXOMonteCarloData(const EXOMonteCarloData&);
    EXOMonteCarloData& operator=(const EXOMonteCarloData&);

    ~EXOMonteCarloData();
    virtual void Clear( Option_t* = "" ); 
    virtual void ClearPixelatedChargeDeposits();

    // Class version number incremented to 5 by M. Hughes for addition of
    // EXOMCVetoPanelHitInfo (Nov 2014).
    ClassDef( EXOMonteCarloData, 7 )

};

//---- inlines -----------------------------------------------------------------


inline const EXOMCParticleInformation* 
  EXOMonteCarloData::GetParticleInformation(size_t i) const
{ 
  // Get Particle information object at index i.  The general mechanism to loop
  // over info objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumParticleInformation(); i++ ) {
  //   EXOMCParticleInformation* part_info = mc_data.GetParticleInformation(i);
  //   Int_t type = part_info->fID.
  //   ...
  // }
  //
  // See documentation of EXOMCParticleInformation for information about access
  // functions and included data members.

  return static_cast< const EXOMCParticleInformation* >(
    GetParticleInformationArray()->At(i)) ; 
}

inline EXOMCParticleInformation* 
  EXOMonteCarloData::GetParticleInformation(size_t i)
{ 
  return static_cast< EXOMCParticleInformation* >(
    GetParticleInformationArray()->At(i)) ; 
}


inline const EXOMCInteractionInfo*
  EXOMonteCarloData::GetInteraction(size_t i) const
{
  // Get Interaction Information object at index i.  The general mechanism to loop
  // over info objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumInteractions(); i++ ) {
  //   EXOMCInteractionInfo* int_info = mc_data.GetInteraction(i);
  //   Int_t type = part_info->fID.
  //   ...
  // }
  //
  // See documentation of EXOMCInteractionInformation for information about access
  // functions and included data members.

  return static_cast< const EXOMCInteractionInfo* >(
    GetInteractionArray()->At(i)) ;
}

inline EXOMCInteractionInfo*
  EXOMonteCarloData::GetInteraction(size_t i)
{
  return static_cast< EXOMCInteractionInfo* >(
    GetInteractionArray()->At(i)) ;
}


inline const EXOMCPixelatedChargeDeposit* EXOMonteCarloData::GetPixelatedChargeDeposit(size_t i) const
{
  // Get Particle information object at index i.
  // The general mechanism to loop over info objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumPixelatedChargeDeposits(); i++ ) {
  //   EXOMCPixelatedChargeDeposit* pix_deposit = mc_data.GetPixelatedChargeDeposit(i);
  //   ...
  // }
  //
  // See documentation of EXOMCPixelatedChargeDeposit for information about access
  // functions and included data members.
  return static_cast< const EXOMCPixelatedChargeDeposit* >( GetPixelatedChargeDepositsArray()->At(i) );
}

inline EXOMCPixelatedChargeDeposit* EXOMonteCarloData::GetPixelatedChargeDeposit(size_t i) 
{
  // Get Particle information object at index i.
  // The general mechanism to loop over info objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumPixelatedChargeDeposits(); i++ ) {
  //   EXOMCPixelatedChargeDeposit* pix_deposit = mc_data.GetPixelatedChargeDeposit(i);
  //   ...
  // }
  //
  // See documentation of EXOMCPixelatedChargeDeposit for information about access
  // functions and included data members.
  return static_cast< EXOMCPixelatedChargeDeposit* >( GetPixelatedChargeDepositsArray()->At(i) );
}


inline const EXOMCPixelatedChargeDeposit* 
  EXOMonteCarloData::FindPixelatedChargeDeposit(const EXOCoordinates& coord) const
{ 
  // Get Pixelated charge object at coordinate pixel key.
  // Returns NULL if no key is found.
  if (GetNumPixelatedChargeDeposits() == 0) return NULL;
  // Makes sure the APD Map is set.
  CheckPCDMap();
  const EXOCoordinateKey& MyKey = coord.GetCoordinateKey(EXOMiscUtil::kUVCoordinates);
  PCDMap::iterator iter = fPCDMap.find( MyKey );
  if ( iter == fPCDMap.end() ) return NULL; 
  return static_cast<const EXOMCPixelatedChargeDeposit*>(iter->second);
}

inline const EXOMCAPDHitInfo* EXOMonteCarloData::FindAPDHitInfo(Int_t gangNo, Double_t time) const
{ 
  // Get APD Hit info with a particular gangNo.  Returns NULL if gangNo not
  // found.
  if (GetNumAPDHits() == 0) return NULL;
  // Makes sure the APD Map is set.
  CheckAPDMap();
  APDMap::iterator iter = fAPDMap.find( std::make_pair(ConvertTime(time), gangNo) );
  if ( iter == fAPDMap.end() ) return NULL; 
  return static_cast<const EXOMCAPDHitInfo*>(iter->second);
}

inline const EXOMCAPDHitInfo* EXOMonteCarloData::AddAPDHitInfo(const EXOMCAPDHitInfo& apdHit)
{
  // Add the apd hit info to the monte carlo data.  This will search for an
  // equivalent EXOMCAPDHitInfo in the data, and add it, returning the final result.
  EXOMCAPDHitInfo* found = FindOrCreateAPDHitInfo(apdHit.fGangNo, apdHit.fTime); 
  *found += apdHit;
  return found;
}

inline const EXOMCAPDHitInfo* EXOMonteCarloData::GetAPDHit(size_t i) const 
{ 
  // Get APD hit object at index i.  The general mechanism to loop
  // over hit objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumAPDHits(); i++ ) {
  //   EXOMCAPDHitInfo* apd = mc_data.GetAPDHit(i);
  //   Int_t numhits = apd->fNumHits.
  //   ...
  // }
  //
  // See documentation of EXOMCAPDHitInfo for information about access
  // functions and included data members.

  return static_cast< const EXOMCAPDHitInfo* >(GetAPDHitsArray()->At(i)) ; 
}

inline const EXOMCVetoPanelHitInfo* EXOMonteCarloData::AddVetoPanelHitInfo(const EXOMCVetoPanelHitInfo& vetoPanelHit)
{
  // Add the veto panel hit info to the Monte Carlo data.  This will search for an
  // equivalent EXOMCVetoPanelHitInfo in the data, and add it, returning the final result.

  EXOMCVetoPanelHitInfo* found = FindOrCreateVetoPanelHitInfo(vetoPanelHit.fPanelID, vetoPanelHit.fTime); 
  *found += vetoPanelHit;
  return found;
}

inline const EXOMCVetoPanelHitInfo* EXOMonteCarloData::FindVetoPanelHitInfo(Int_t panelID, Double_t time) const
{ 
  // Get veto panel hit info with a particular panel ID.  Returns NULL if panel ID not found.
  if (GetNumVetoPanelHits() == 0) return NULL;

  // Makes sure the veto panel map is set.
  CheckVetoPanelMap();

  VetoPanelMap::iterator iter = fVetoPanelMap.find( std::make_pair(ConvertTime(time), panelID) );
  if ( iter == fVetoPanelMap.end() ) return NULL;

  return static_cast<const EXOMCVetoPanelHitInfo*>(iter->second);
}

inline const EXOMCVetoPanelHitInfo* EXOMonteCarloData::GetVetoPanelHit(size_t i) const 
{ 
  // Get veto panel hit object at index i.  The general mechanism to loop
  // over hit objects is to do the following:
  //
  // EXOEventData* ED;  // Assuming we have a valid pointer to EXOEventData
  //
  // EXOMonteCarloData& mc_data = ED->fMonteCarloData;
  // for (size_t i=0; i<mc_data.GetNumVetoPanelHits(); i++ ) {
  //   EXOMCVetoPanelHitInfo* veto = mc_data.GetVetoPanelHit(i);
  //   Int_t numhits = veto->fNumHits.
  //   ...
  // }
  //
  // See documentation of EXOMCVetoPanelHitInfo for information about access
  // functions and included data members.

  return static_cast< const EXOMCVetoPanelHitInfo* >(GetVetoPanelHitsArray()->At(i)) ; 
}

inline EXOMCParticleInformation* EXOMonteCarloData::GetNewParticleInformation()
{ 
  // Get a new part info object for loading into the event.  See e.g.
  // EXOEventData::GetNewScintillationCluster() documentation for how this
  // works.
  return static_cast<EXOMCParticleInformation*>(
    GetParticleInformationArray()->GetNewOrCleanedObject(
    GetNumParticleInformation()) ); 
}

inline EXOMCInteractionInfo* EXOMonteCarloData::GetNewInteraction()
{
  // Get a new part info object for loading into the event.  See e.g.
  // EXOEventData::GetNewScintillationCluster() documentation for how this
  // works.
  return static_cast<EXOMCInteractionInfo*>(
    GetInteractionArray()->GetNewOrCleanedObject(
    GetNumInteractions()) );
}


inline EXOMCPixelatedChargeDeposit* 
EXOMonteCarloData::FindOrCreatePixelatedChargeDeposit(const EXOCoordinates& coord) 
{
  // Return a pointer to the deposit information corresponding to coordinate Coord.
  // If the entry does not exist yet, create it.

  // Try to find the pixel, in case it already exists.
  EXOMCPixelatedChargeDeposit* ChargeDeposit = 
    const_cast<EXOMCPixelatedChargeDeposit*>(FindPixelatedChargeDeposit(coord));
  if(ChargeDeposit) return ChargeDeposit;

  // I guess it doesn't exist.  Create it, and initialize it to the appropriate pixel.
  ChargeDeposit = static_cast<EXOMCPixelatedChargeDeposit*>(
      GetPixelatedChargeDepositsArray()->GetNewOrCleanedObject(GetNumPixelatedChargeDeposits()) );
  ChargeDeposit->SetCoordinates(coord);
  FillPCDMap(ChargeDeposit);
  return ChargeDeposit;
}

inline EXOMCAPDHitInfo* EXOMonteCarloData::FindOrCreateAPDHitInfo(Int_t gangNo, Double_t time) 
{
  // Return a pointer to the APD Hit info corresponding to the gangNo.  If the
  // entry does not exist yet, create it.

  EXOMCAPDHitInfo* apdInfo = 
    const_cast<EXOMCAPDHitInfo*>(FindAPDHitInfo(gangNo, time));
  if(apdInfo) return apdInfo;

  // I guess it doesn't exist.  Create it, and initialize it to the appropriate pixel.
  apdInfo = GetNewAPDHit();
  apdInfo->fGangNo = gangNo;
  apdInfo->fTime = time;
  FillAPDMap(apdInfo);
  return apdInfo;
}

inline EXOMCAPDHitInfo* EXOMonteCarloData::GetNewAPDHit()
{ 
  // Get a new apd hit info object for loading into the event.  See e.g.
  // EXOEventData::GetNewScintillationCluster() documentation for how this
  // works.
  return static_cast<EXOMCAPDHitInfo*>(
    GetAPDHitsArray()->GetNewOrCleanedObject(
    GetNumAPDHits()) ); 
}

inline EXOMCVetoPanelHitInfo* EXOMonteCarloData::FindOrCreateVetoPanelHitInfo(Int_t panelID, Double_t time) 
{
  // Return a pointer to the veto panel hit info corresponding to the panelID.  If the
  // entry does not exist yet, create it.

  EXOMCVetoPanelHitInfo* vetoPanelInfo = const_cast< EXOMCVetoPanelHitInfo* >(FindVetoPanelHitInfo(panelID, time));
  if(vetoPanelInfo) return vetoPanelInfo;

  // I guess it doesn't exist. Create it, and initialize it to the appropriate panelID.
  vetoPanelInfo = GetNewVetoPanelHit();
  vetoPanelInfo->fPanelID = panelID;
  vetoPanelInfo->fTime = time;
  FillVetoPanelMap(vetoPanelInfo);
  return vetoPanelInfo;
}

inline EXOMCVetoPanelHitInfo* EXOMonteCarloData::GetNewVetoPanelHit()
{ 
  // Get a new veto panel hit info object for loading into the event.  See e.g.
  // EXOEventData::GetNewScintillationCluster() documentation for how this works.

  return static_cast< EXOMCVetoPanelHitInfo* >(
      GetVetoPanelHitsArray()->GetNewOrCleanedObject(GetNumVetoPanelHits())); 
}

inline EXOTClonesArray* EXOMonteCarloData::GetAPDHitsArray()
{
  // Get array of APD hits.  Normal access to the array should be through the
  // access functions GetAPDHit and GetNumAPDHits.
  assert(fAPDHits != NULL);
  return static_cast< EXOTClonesArray* >( fAPDHits ); 
}

inline const EXOTClonesArray* EXOMonteCarloData::GetAPDHitsArray() const    
{
  assert(fAPDHits != NULL);
  return static_cast< const EXOTClonesArray* >( fAPDHits ); 
}

inline EXOTClonesArray* EXOMonteCarloData::GetVetoPanelHitsArray()
{
  // Get array of veto panel hits.  Normal access to the array should be through the
  // access functions GetVetoPanelHit and GetNumVetoPanelHits.
  assert(fVetoPanelHits != NULL);
  return static_cast< EXOTClonesArray* >( fVetoPanelHits ); 
}

inline const EXOTClonesArray* EXOMonteCarloData::GetVetoPanelHitsArray() const    
{
  assert(fVetoPanelHits != NULL);
  return static_cast< const EXOTClonesArray* >( fVetoPanelHits ); 
}

inline EXOTClonesArray* EXOMonteCarloData::GetPixelatedChargeDepositsArray()
{ 
  // Get array of pixelated charge deposits.  Normal access to the array should be through the
  // access functions provided.
  assert(fPixelatedChargeDeposits != NULL);
  return static_cast< EXOTClonesArray* >( fPixelatedChargeDeposits ); 
}

inline const EXOTClonesArray* EXOMonteCarloData::GetPixelatedChargeDepositsArray() const    
{
  assert(fPixelatedChargeDeposits != NULL);
  return static_cast< const EXOTClonesArray* >( fPixelatedChargeDeposits ); 
}

inline EXOTClonesArray* EXOMonteCarloData::GetParticleInformationArray()
{ 
  // Get array of particle info.  Normal access to the array should be through
  // the access functions GetParticleInformation and  
  // GetNumParticleInformation.
  assert(fParticleInformation != NULL);
  return static_cast< EXOTClonesArray* >( fParticleInformation ); 
}

inline const EXOTClonesArray* EXOMonteCarloData::GetParticleInformationArray() const   
{
  assert(fParticleInformation != NULL);
  return static_cast< const EXOTClonesArray* >( fParticleInformation ); 
}

inline size_t EXOMonteCarloData::GetNumParticleInformation() const 
{ 
  // Get Number of particle information objects in event
  return ( fParticleInformation ) ? 
    fParticleInformation->GetEntriesFast() : 0; 
}

inline EXOTClonesArray* EXOMonteCarloData::GetInteractionArray()
{
  // Get array of particle info.  Normal access to the array should be through
  // the access functions GetInteraction and  
  // GetNumInteractions.
  assert(fInteractions != NULL);
  return static_cast< EXOTClonesArray* >( fInteractions );
}

inline const EXOTClonesArray* EXOMonteCarloData::GetInteractionArray() const
{
  assert(fInteractions != NULL);
  return static_cast< const EXOTClonesArray* >( fInteractions );
}

inline size_t EXOMonteCarloData::GetNumInteractions() const
{
  // Get Number of particle information objects in event
  return ( fInteractions ) ?
    fInteractions->GetEntriesFast() : 0;
}


inline size_t EXOMonteCarloData::GetNumPixelatedChargeDeposits() const 
{ 
  // Get Number of pixelated charge deposit objects in event
  return ( fPixelatedChargeDeposits ) ? fPixelatedChargeDeposits->GetEntriesFast() : 0; 
}

inline size_t EXOMonteCarloData::GetNumAPDHits() const 
{ 
  // Get Number of APD hit objects in event
  return ( fAPDHits ) ? fAPDHits->GetEntriesFast() : 0; 
}

inline size_t EXOMonteCarloData::GetNumVetoPanelHits() const 
{ 
  // Get Number of veto panel hit objects in event
  return ( fVetoPanelHits ) ? fVetoPanelHits->GetEntriesFast() : 0; 
}

inline void EXOMonteCarloData::Remove(EXOMCParticleInformation* partinfo)
{
  // Remove particle information from MC event
  GetParticleInformationArray()->RemoveAndCompress(partinfo);
}

inline void EXOMonteCarloData::Remove(EXOMCInteractionInfo* intinfo)
{
  // Remove particle information from MC event
  GetInteractionArray()->RemoveAndCompress(intinfo);
}

inline void EXOMonteCarloData::Remove(EXOMCAPDHitInfo* hitinfo)
{
  // Remove hit info from MC event
  GetAPDHitsArray()->RemoveAndCompress(hitinfo);
}

inline void EXOMonteCarloData::Remove(EXOMCVetoPanelHitInfo* hitInfo)
{
  // Remove hit info from MC event
  GetVetoPanelHitsArray()->RemoveAndCompress(hitInfo);
}

inline void EXOMonteCarloData::CheckAPDMap() const
{
  // Protected internal function to check the APD map and sort if necessary.
  // Since we own this data, we can just look to see if the numbers match,
  // otherwise, the map has the correct internal data.
  if ( fAPDMap.size() == GetNumAPDHits() ) return;
  // Otherwise, fill the map
  fAPDMap.clear();
  size_t numHits = GetNumAPDHits();
  for (size_t i=0;i<numHits;i++) {
    FillAPDMap(static_cast<EXOMCAPDHitInfo*>(GetAPDHitsArray()->At(i)));
  }
}

inline void EXOMonteCarloData::FillAPDMap( EXOMCAPDHitInfo* hit) const
{
  // Internal function to fill the APD map, which maps pixelated time/gang to
  // objects.  It is const because this is a cache function.
  fAPDMap[std::make_pair(ConvertTime(hit->fTime), hit->fGangNo)] = hit;
}

inline Long64_t EXOMonteCarloData::ConvertTime( Double_t time ) const
{
  return static_cast<Long64_t>(time/SAMPLE_TIME_HIGH_BANDWIDTH);
}

inline void EXOMonteCarloData::CheckVetoPanelMap() const
{
  // Protected internal function to check the veto panel map and sort if necessary.
  // Since we own this data, we can just look to see if the numbers match;
  // otherwise, the map has the correct internal data.

  if ( fVetoPanelMap.size() == GetNumVetoPanelHits() ) return;

  // Otherwise, fill the map
  fVetoPanelMap.clear();
  size_t numHits = GetNumVetoPanelHits();

  for (size_t i=0; i<numHits; i++) {
    FillVetoPanelMap(static_cast<EXOMCVetoPanelHitInfo*>(GetVetoPanelHitsArray()->At(i)));
  }
}

inline void EXOMonteCarloData::FillVetoPanelMap( EXOMCVetoPanelHitInfo* hit) const
{
  // Internal function to fill the veto panel map, which maps time/panelID to
  // objects.  It is const because this is a cache function.
  fVetoPanelMap[std::make_pair(ConvertTime(hit->fTime), hit->fPanelID)] = hit;
}

inline void EXOMonteCarloData::CheckPCDMap() const
{
  // Protected internal function to check the PCD map and sort if necessary.
  // Since we own this data, we can just look to see if the numbers match,
  // otherwise, the map has the correct internal data.
  if ( fPCDMap.size() == GetNumPixelatedChargeDeposits() ) return;
  // Otherwise, fill the map
  fPCDMap.clear();
  size_t numHits = GetNumPixelatedChargeDeposits();
  for (size_t i=0;i<numHits;i++) {
    FillPCDMap(static_cast<EXOMCPixelatedChargeDeposit*>(GetPixelatedChargeDepositsArray()->At(i)));
  }
}

inline void EXOMonteCarloData::FillPCDMap( EXOMCPixelatedChargeDeposit* pcd) const
{
  // Internal function to fill the pcd map, which maps the coordinate keys to
  // the object.  objects.  It is const because this is a cache function.
  fPCDMap[pcd->GetPixelCoordinateKey()] = pcd;
}
#endif /* EXOMonteCarloData_hh */
