//______________________________________________________________________________
//                                                                        
// EXOMonteCarloData                                                           
//                                                                        
// Encapsulates monte carlo data 
//  
// WARNING:  The pointers fParticleInformation and fAPDHits nominally point
// to TClonesArrays; in fact, it is assumed (eg. by the GetNum functions) that they point
// to EXOTClonesArrays, which are guaranteed to always be compressed.
// See the class description for EXOTClonesArray, which explains caveats.

#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"

ClassImp( EXOMonteCarloData )

//______________________________________________________________________________
EXOMonteCarloData::EXOMonteCarloData() : TObject(),
  fParticleInformation(0),
  fAPDHits(0),
  fPixelatedChargeDeposits(0),
  fVetoPanelHits(0),
  fInteractions(0)
{
  EXOMonteCarloData::Clear();
  InitializeArrays();
}

//______________________________________________________________________________
void EXOMonteCarloData::Clear( Option_t* )
{
  
  // Clear data, reset to initial state.
  fBetaDecayQValue = 0;
  fPrimaryEventX   = 0;
  fPrimaryEventY   = 0;
  fPrimaryEventZ   = 0;
  
  fTotalPhotons      = 0;
  fTotalHitsArrayOne = 0;
  fTotalHitsArrayTwo = 0;	
  fTotalHitsVetoPanels = 0;
  fEventGroupingTime = 0.0;	
  
  // Monte Carlo energy deposit info
  
  fTotalEnergyInSalt                        = 0;
  fTotalEnergyInShield                      = 0;
  fTotalEnergyInOuterCryostat               = 0;
  fTotalEnergyInInnerCryostat               = 0;
  fTotalEnergyInHFE                         = 0;
  fTotalEnergyInVessel                      = 0;
  fTotalEnergyInLiquidXe                    = 0;
  fTotalIonizationEnergyInLiquidXe          = 0;
  fTotalEnergyOptPhotonsInCathode           = 0;   
  fTotalEnergyOptPhotonsInBothAnodes        = 0;   
  fTotalEnergyOptPhotonsInBothWireSupports  = 0;   
  fTotalEnergyOptPhotonsInCathodeRing       = 0;   
  fTotalEnergyOptPhotonsInTeflonReflector   = 0;   
  fTotalEnergyOptPhotonsInBothAPDFrames     = 0;   
  fTotalEnergyOptPhotonsInLiquidXe          = 0;   
  fTotalEnergyOptPhotonsInLiquidXeVessel    = 0;   
  fTotalEnergyOptPhotonsInRemovedAPDs       = 0;   
  fTotalEnergyOptPhotonsInFieldShapingRings = 0;   
  fTotalEnergyInVetoPanels                  = 0;
  fDidBrem                                  = false;
  fDidCompton                               = false;

  if ( fParticleInformation ) fParticleInformation->Clear( "C" ); 
  if ( fAPDHits ) fAPDHits->Clear( "C" ); 
  if ( fVetoPanelHits ) fVetoPanelHits->Clear( "C" ); 
  if ( fInteractions ) fInteractions->Clear( "C" );

  fAPDMap.clear();
  fVetoPanelMap.clear();
  EXOMonteCarloData::ClearPixelatedChargeDeposits();
}

//______________________________________________________________________________
void EXOMonteCarloData::ClearPixelatedChargeDeposits()
{
  //Remove all pixelated charge deposits
  if(fPixelatedChargeDeposits){
    fPixelatedChargeDeposits->Clear("C"); 
  }
  fPCDMap.clear();
}

//______________________________________________________________________________
void EXOMonteCarloData::InitializeArrays()
{
  InitializeParticleInformation();
  InitializeAPDHits();
  InitializeVetoPanelHits();
  InitializePixelatedChargeDeposits();
  InitializeInteractions();
}

//______________________________________________________________________________
void EXOMonteCarloData::InitializeParticleInformation()
{
  // Initialize particle info array, using MAXPART as initial
  // guess of size
  fParticleInformation = 
    new EXOTClonesArray( EXOMCParticleInformation::Class(), MAXPART );  
}
//______________________________________________________________________________
void EXOMonteCarloData::InitializeInteractions()
{
  // Initialize particle info array, using MAXPART as initial
  // guess of size
  fInteractions =
    new EXOTClonesArray( EXOMCInteractionInfo::Class(), MAXPART );
}
//______________________________________________________________________________
void EXOMonteCarloData::InitializeAPDHits()
{
  // Initialize APD hit array, using MAXNAPD as initial
  // guess of size
  fAPDHits = new EXOTClonesArray( EXOMCAPDHitInfo::Class(), MAXNAPD ); 
}
//______________________________________________________________________________
void EXOMonteCarloData::InitializePixelatedChargeDeposits()
{
  // Initialize pixelated charge dep array, using MAXNAPD as initial guess of size
  fPixelatedChargeDeposits = new EXOTClonesArray( EXOMCPixelatedChargeDeposit::Class(), MAXCLUSTER ); 
}
//______________________________________________________________________________
void EXOMonteCarloData::InitializeVetoPanelHits()
{
  // Initialize veto panel hit array, using MAXVETO as initial
  // guess of size
  fVetoPanelHits = new EXOTClonesArray( EXOMCVetoPanelHitInfo::Class(), MAXVETO ); 
}

//______________________________________________________________________________
EXOMonteCarloData::~EXOMonteCarloData()
{
  delete fParticleInformation;
  delete fAPDHits;
  delete fPixelatedChargeDeposits;
  delete fVetoPanelHits;
  delete fInteractions;
}

//______________________________________________________________________________
EXOMonteCarloData::EXOMonteCarloData(const EXOMonteCarloData& other) : TObject(other)
{
  fBetaDecayQValue = other.fBetaDecayQValue;
  fPrimaryEventX = other.fPrimaryEventX;
  fPrimaryEventY = other.fPrimaryEventY;
  fPrimaryEventZ = other.fPrimaryEventZ;
  
  fTotalPhotons = other.fTotalPhotons;
  fTotalHitsArrayOne = other.fTotalHitsArrayOne;
  fTotalHitsArrayTwo = other.fTotalHitsArrayTwo;	
  fTotalHitsVetoPanels = other.fTotalHitsVetoPanels;	
  fEventGroupingTime = other.fEventGroupingTime;	
  
  // Monte Carlo energy deposit info
  
  fTotalEnergyInSalt = other.fTotalEnergyInSalt;
  fTotalEnergyInShield = other.fTotalEnergyInShield;
  fTotalEnergyInOuterCryostat = other.fTotalEnergyInOuterCryostat;
  fTotalEnergyInInnerCryostat = other.fTotalEnergyInInnerCryostat;
  fTotalEnergyInHFE = other.fTotalEnergyInHFE;
  fTotalEnergyInVessel = other.fTotalEnergyInVessel;
  fTotalEnergyInLiquidXe = other.fTotalEnergyInLiquidXe;
  fTotalIonizationEnergyInLiquidXe = other.fTotalIonizationEnergyInLiquidXe;
  fTotalEnergyOptPhotonsInCathode = other.fTotalEnergyOptPhotonsInCathode;   
  fTotalEnergyOptPhotonsInBothAnodes = other.fTotalEnergyOptPhotonsInBothAnodes;   
  fTotalEnergyOptPhotonsInBothWireSupports = other.fTotalEnergyOptPhotonsInBothWireSupports;   
  fTotalEnergyOptPhotonsInCathodeRing = other.fTotalEnergyOptPhotonsInCathodeRing;   
  fTotalEnergyOptPhotonsInTeflonReflector = other.fTotalEnergyOptPhotonsInTeflonReflector;   
  fTotalEnergyOptPhotonsInBothAPDFrames = other.fTotalEnergyOptPhotonsInBothAPDFrames;   
  fTotalEnergyOptPhotonsInLiquidXe = other.fTotalEnergyOptPhotonsInLiquidXe;   
  fTotalEnergyOptPhotonsInLiquidXeVessel = other.fTotalEnergyOptPhotonsInLiquidXeVessel;   
  fTotalEnergyOptPhotonsInRemovedAPDs = other.fTotalEnergyOptPhotonsInRemovedAPDs;   
  fTotalEnergyOptPhotonsInFieldShapingRings = other.fTotalEnergyOptPhotonsInFieldShapingRings;   
  fTotalEnergyInVetoPanels = other.fTotalEnergyInVetoPanels;
  fDidBrem = other.fDidBrem;
  fDidCompton = other.fDidCompton;

  fParticleInformation = new EXOTClonesArray(*other.fParticleInformation);
  fAPDHits = new EXOTClonesArray(*other.fAPDHits);
  fPixelatedChargeDeposits = new EXOTClonesArray(*other.fPixelatedChargeDeposits);
  fVetoPanelHits = new EXOTClonesArray(*other.fVetoPanelHits);
  fInteractions = new EXOTClonesArray(*other.fInteractions);
}
//______________________________________________________________________________
EXOMonteCarloData& EXOMonteCarloData::operator=(const EXOMonteCarloData& other)
{
  if (this == &other) return *this;
  fBetaDecayQValue = other.fBetaDecayQValue;
  fPrimaryEventX = other.fPrimaryEventX;
  fPrimaryEventY = other.fPrimaryEventY;
  fPrimaryEventZ = other.fPrimaryEventZ;
  
  fTotalPhotons = other.fTotalPhotons;
  fTotalHitsArrayOne = other.fTotalHitsArrayOne;
  fTotalHitsArrayTwo = other.fTotalHitsArrayTwo;	
  fTotalHitsVetoPanels = other.fTotalHitsVetoPanels;	
  
  // Monte Carlo energy deposit info
  
  fTotalEnergyInSalt = other.fTotalEnergyInSalt;
  fTotalEnergyInShield = other.fTotalEnergyInShield;
  fTotalEnergyInOuterCryostat = other.fTotalEnergyInOuterCryostat;
  fTotalEnergyInInnerCryostat = other.fTotalEnergyInInnerCryostat;
  fTotalEnergyInHFE = other.fTotalEnergyInHFE;
  fTotalEnergyInVessel = other.fTotalEnergyInVessel;
  fTotalEnergyInLiquidXe = other.fTotalEnergyInLiquidXe;
  fTotalIonizationEnergyInLiquidXe = other.fTotalIonizationEnergyInLiquidXe;
  fTotalEnergyOptPhotonsInCathode = other.fTotalEnergyOptPhotonsInCathode;   
  fTotalEnergyOptPhotonsInBothAnodes = other.fTotalEnergyOptPhotonsInBothAnodes;   
  fTotalEnergyOptPhotonsInBothWireSupports = other.fTotalEnergyOptPhotonsInBothWireSupports;   
  fTotalEnergyOptPhotonsInCathodeRing = other.fTotalEnergyOptPhotonsInCathodeRing;   
  fTotalEnergyOptPhotonsInTeflonReflector = other.fTotalEnergyOptPhotonsInTeflonReflector;   
  fTotalEnergyOptPhotonsInBothAPDFrames = other.fTotalEnergyOptPhotonsInBothAPDFrames;   
  fTotalEnergyOptPhotonsInLiquidXe = other.fTotalEnergyOptPhotonsInLiquidXe;   
  fTotalEnergyOptPhotonsInLiquidXeVessel = other.fTotalEnergyOptPhotonsInLiquidXeVessel;   
  fTotalEnergyOptPhotonsInRemovedAPDs = other.fTotalEnergyOptPhotonsInRemovedAPDs;   
  fTotalEnergyOptPhotonsInFieldShapingRings = other.fTotalEnergyOptPhotonsInFieldShapingRings;   
  fTotalEnergyInVetoPanels = other.fTotalEnergyInVetoPanels;
  fDidBrem = other.fDidBrem;
  fDidCompton = other.fDidCompton;

  *static_cast<EXOTClonesArray*>(fParticleInformation) = *other.fParticleInformation;
  *static_cast<EXOTClonesArray*>(fAPDHits) = *other.fAPDHits;
  *static_cast<EXOTClonesArray*>(fPixelatedChargeDeposits) = *other.fPixelatedChargeDeposits;
  *static_cast<EXOTClonesArray*>(fVetoPanelHits) = *other.fVetoPanelHits;
  *static_cast<EXOTClonesArray*>(fInteractions) = *other.fInteractions;

  return *this;
}

