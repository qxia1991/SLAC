//______________________________________________________________________________
//                                                                        
// EXOEventData                                                           
//                                                                        
// Encapsulates the data of an EXO event.  This class is the top of the tree of
// a set of data.  The basic tree format is shown:
//
//   EXOEventHeader                    // header info, trigger info, etc. 
//   EXOMonteCarloData                 // monte carlo data, only populated for MC
//     EXOMCAPDHitInfo                 // MC Hit info in APDS
//     EXOMCVetoPanelHitInfo           // MC Hit info in veto panels
//     EXOMCParticleInformation        // MC Particle info
//     EXOMCPixelatedChargeDeposit     // MC pixelated charge deposit
//   EXOComptonInfo                    // Compton data
//
// And sets of arrays of the following:
//
//   EXOScintillationCluster           // scintillation cluster
//     EXOGangInfo                     // gang information per scint cluster
//   EXOChargeCluster                  // charge cluster
//   EXOUWireSignal                    // u-wire signal
//   EXOUWireInductionSignal           // u-wire induction signal
//   EXOVWireSignal                    // v-wire signal
//   EXOChargeInjectionSignal          // charge injection signals
//   EXOAPDSignal                      // APD signal
//
//   EXOWaveformData                   // raw data, accessed through 
//                                     // GetWaveformData()
//
// GetWaveformData() which returns an EXOWaveformData encapsulating a set of
// EXOWaveform objects.
//
//
// This new format encapsulates each subset of data in its class, meaning all
// the data related to scintillation clusters are in EXOScintillationCluster,
// all the data for charge clusters live in EXOChargeCluster, etc.  
//
// ==> Referencing, or looping over an array: 
// 
// Previously, this was done by indexing a set of arrays.  For example, the
// following code is taken from EXODataQualityModule: 
//
// EXOAnalysisModule::EventStatus 
//   EXODataQualityModule::BeginOfEvent(EXOEventData *ED)
// {
//   // Loop over all reconstructed clusters and enter the info
//   // into the histogram
//   
//   for ( unsigned int i = 0; i < ED->ncl; i++ ) {
//     ...  
//     // Cluster energy
//   
//     h201->Fill( ED->eccl[i] );
//     h803->Fill( ED->evcl[i] );
//   
//     // Position
//   
//     h301->Fill( ED->xcl[i] );
//     h302->Fill( ED->ycl[i] );
//     h303->Fill( ED->zcl[i] );
//     h304->Fill( ED->ucl[i] );
//     h305->Fill( ED->vcl[i] );
//     h306->Fill( ED->tcl[i]/microsecond );
//     ...
//
// In the new EXOEventData format, this code would look like the following:
//
// for ( unsigned int i = 0; i < ED->GetNumChargeClusters(); i++ ) {
//   EXOChargeCluster* cluster = ED->GetChargeCluster(i);
//   ... 
//   // Cluster energy
// 
//   h201->Fill( cluster->fCorrectedEnergy );
//   h803->Fill( cluster->fEnergyInVChannels );
// 
//   // Position
// 
//   h301->Fill( cluster->fX );
//   h302->Fill( cluster->fY );
//   h303->Fill( cluster->fZ );
//   h304->Fill( cluster->fU );
//   h305->Fill( cluster->fV );
//   h306->Fill( cluster->fCollectionTime/microsecond );
//
// ==> Filling a cluster or signal in an event 
// 
// Similarly, to fill an event in the older setup required referencing 
// the index of a set of arrays directly.  One would also have to be
// careful about overrunning the predetermined size limits of the arrays.
// This is no longer the case, instead now the user requests from the 
// EXOEventData object a new object (cluster, wire or apd signal, etc.)
// and then fills the requisite data members of this object. 
// A good example is to consider the code in the EXOReconstructionModule:
// 
//   In the old format this looked like:
//
//  // Get reconstructed charge clusters
//  EXOChargeCluster *chclu = NULL;
//  std::vector <EXOChargeCluster> charge_clusters = rec->get_chargeclusters();
//  ED->ncl = charge_clusters.size();
//  for ( unsigned int i = 0; i < ED->ncl; i++ ) {
//    chclu = &charge_clusters[i];
//    if ( chclu == NULL ) continue;
//
//    double test = chclu->get_er();
//
//    if ( test < 1.0 ) continue; // get rid of clusters with no energy
//
//    ED->ercl[i] = test;
//    ED->eerrcl[i] = chclu->get_esig();
//    ED->xcl[i] = chclu->get_x();
//    ED->ycl[i] = chclu->get_y();
//    ED->ucl[i] = chclu->get_u();
//    ED->tcl[i] = chclu->get_t();
//    if ( *fix_shielding_correction == true ) {
//      double zcl_in_mm = ED->zcl[i]*mm;
//      double shielding_correction_factor =  get_shielding_correction(zcl_in_mm);
//      ED->eccl[i] = ED->ercl[i]*shielding_correction_factor;
//      cout << zcl_in_mm << " " << shielding_correction_factor << endl;
//    }
//    ...
//    }
//
//
//  ==> In the new format, this looks like the following:
// 
//
//  // Get reconstructed charge clusters
//  EXOChargeCluster *chclu = NULL;
//  std::vector <Recon::EXOChargeCluster*> charge_clusters = rec->get_chargeclusters();
//  for ( size_t i = 0; i < charge_clusters.size(); i++ ) {
//    chclu = &charge_clusters[i];
//    if ( chclu == NULL ) continue;
//
//    double test = chclu->get_er();
//
//    if ( test < 1.0 ) continue; // get rid of clusters with no energy
//
//    EXOChargeCluster* cluster = ED->GetNewChargeCluster();
//    cluster->fRawEnergy = test;
//    cluster->fCorrectedEnergyError = chclu->get_esig();
//    cluster->fX = chclu->get_x();
//    cluster->fY = chclu->get_y();
//    cluster->fU = chclu->get_u();
//    cluster->fCollectionTime = chclu->get_t();
//    if ( *fix_shielding_correction == true ) {
//      double zcl_in_mm = cluster->fZ*mm;
//      double shielding_correction_factor =  get_shielding_correction(zcl_in_mm);
//      cluster->fCorrectedEnergy = cluster->fRawEnergy*shielding_correction_factor;
//      cout << zcl_in_mm << " " << shielding_correction_factor << endl;
//    }
//    ...
//    }
//
//  WARNING:  The pointers fUWires, fAPDs, fChargeClusters, fScintClusters, fChargeInjectionSignals nominally point
//  to TClonesArrays; in fact, it is assumed (eg. by the GetNum functions) that they point
//  to EXOTClonesArrays, which are guaranteed to always be compressed.
//  See the class description for EXOTClonesArray, which explains caveats.

#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTreeArrayLengths.hh"

ClassImp( EXOEventData )

EXO_IMPLEMENT_DELEGATED_FUNCTION(EXOEventData, IsVetoed)

//______________________________________________________________________________
EXOEventData::EXOEventData() : 
  TObject(),
  fUWires(0),
  fUWiresInduction(0),
  fVWires(0),
  fAPDs(0),
  fChargeInjectionSignals(0),
  fChargeClusters(0),
  fScintClusters(0)
{
  // Default constructor, clear and setup arrays
  EXOEventData::Clear();
  InitializeArrays();
}

//______________________________________________________________________________
void EXOEventData::InitializeArrays()
{
  InitializeUWires(); 
  InitializeUWiresInduction(); 
  InitializeVWires(); 
  InitializeChargeInjectionSignals();
  InitializeChargeClusters();
  InitializeScintillationClusters();
  InitializeAPDs();
}

//______________________________________________________________________________
void EXOEventData::InitializeUWires()
{
  // Initialize U Wire array, gives an initial *guess* of MAXCOLL_TOT size
  fUWires = 
    new EXOTClonesArray( EXOUWireSignal::Class(), MAXCOLL_TOT ); 
}
//______________________________________________________________________________
void EXOEventData::InitializeUWiresInduction()
{
  // Initialize the u-wire induction array with an initial guess of MAXCOLL_TOT
  // size.
  fUWiresInduction = 
    new EXOTClonesArray( EXOUWireInductionSignal::Class(), MAXCOLL_TOT ); 
}
//______________________________________________________________________________
void EXOEventData::InitializeChargeInjectionSignals()
{
  // Initialize charge injection signal array, gives an initial *guess* of MAXCOLL_TOT size
  fChargeInjectionSignals = 
    new EXOTClonesArray( EXOChargeInjectionSignal::Class(), MAXCOLL_TOT ); 
}

//______________________________________________________________________________
void EXOEventData::InitializeVWires()
{
  // Initialize V Wire array, gives an initial *guess* of MAXCOLL_TOT size
  fVWires = 
    new EXOTClonesArray( EXOVWireSignal::Class(), MAXCOLL_TOT ); 
}

//______________________________________________________________________________
void EXOEventData::InitializeChargeClusters()
{
  // Initialize Charge cluster array, gives an initial *guess* of MAXCLUSTER
  // size
  fChargeClusters = 
    new EXOTClonesArray( EXOChargeCluster::Class(), MAXCLUSTER ); 
}
//______________________________________________________________________________
void EXOEventData::InitializeScintillationClusters()
{
  // Initialize Scintillation cluster array, gives an initial *guess* of MAXSC
  // size
  fScintClusters = 
    new EXOTClonesArray( EXOScintillationCluster::Class(), MAXSC ); 
}

//______________________________________________________________________________
void EXOEventData::InitializeAPDs()
{
  // Initialize APD array, gives an initial *guess* of MAXNAPD size
  fAPDs = 
    new EXOTClonesArray( EXOAPDSignal::Class(), MAXNAPD ); 
}

//______________________________________________________________________________
EXOEventData::~EXOEventData()
{
  delete fUWires;
  delete fUWiresInduction;
  delete fVWires;
  delete fChargeInjectionSignals;
  delete fChargeClusters;
  delete fScintClusters;
  delete fAPDs;
}

//______________________________________________________________________________
void EXOEventData::Clear( Option_t* /*option*/ )
{
  // Clear Data, reset to initial state.  Clear resets all the arrays.  It is
  // important to note that no destructors are called, instead all the TObject
  // components of the TClonesArrays are Cleared.  This is to avoid excessive
  // new/deletes during Fill.  See the documentation for TClonesArrays for more
  // information.

  fRunNumber                  = 0;
  fEventNumber                = 0;

  fMonteCarloData.Clear( "C" );
  fEventHeader.Clear( "C" );

  if ( fUWires ) fUWires->Clear( "C" );
  if ( fUWiresInduction ) fUWiresInduction->Clear( "C" );
  if ( fVWires ) fVWires->Clear( "C" );
  if ( fChargeInjectionSignals ) fChargeInjectionSignals->Clear( "C" );
  if ( fChargeClusters ) {
    fChargeClusters->Clear( "C" );
  }
  if ( fScintClusters ) {
    fScintClusters->Clear( "C" );
  }
  if ( fAPDs ) fAPDs->Clear( "C" );
  fWaveformData.Clear( "C" );

  fHasSaturatedChannel = false;
  fSkippedByClustering = false;
  fNoiseTags.ResetAllBits();
}

//______________________________________________________________________________

EXOEventData::EXOEventData(const EXOEventData& other) : TObject(other) 
{
  // Copy constructor

  fRunNumber = other.fRunNumber;
  fEventNumber = other.fEventNumber;

  fMonteCarloData = other.fMonteCarloData;
  fEventHeader = other.fEventHeader;
  fUWires = new EXOTClonesArray(*other.fUWires);
  fUWiresInduction = new EXOTClonesArray(*other.fUWiresInduction);
  fVWires = new EXOTClonesArray(*other.fVWires);
  fChargeInjectionSignals = new EXOTClonesArray(*other.fChargeInjectionSignals);
  fChargeClusters = new EXOTClonesArray(*other.fChargeClusters);
  fScintClusters = new EXOTClonesArray(*other.fScintClusters);
  fAPDs = new EXOTClonesArray(*other.fAPDs);
  fWaveformData = other.fWaveformData;
  fHasSaturatedChannel = other.fHasSaturatedChannel;
  fSkippedByClustering = other.fSkippedByClustering;
  fNoiseTags = other.fNoiseTags;
}
//______________________________________________________________________________
EXOEventData& EXOEventData::operator=(const EXOEventData& other)
{
  // Copy constructor
  if (this == &other) return *this;

  fRunNumber = other.fRunNumber;
  fEventNumber = other.fEventNumber;

  fMonteCarloData = other.fMonteCarloData;
  fEventHeader = other.fEventHeader;
  *static_cast<EXOTClonesArray*>(fUWires) = *other.fUWires;
  *static_cast<EXOTClonesArray*>(fUWiresInduction) = *other.fUWiresInduction;
  *static_cast<EXOTClonesArray*>(fVWires) = *other.fVWires;
  *static_cast<EXOTClonesArray*>(fChargeInjectionSignals) = *other.fChargeInjectionSignals;
  *static_cast<EXOTClonesArray*>(fChargeClusters) = *other.fChargeClusters;
  *static_cast<EXOTClonesArray*>(fScintClusters) = *other.fScintClusters;
  *static_cast<EXOTClonesArray*>(fAPDs) = *other.fAPDs;
  fWaveformData = other.fWaveformData;
  fHasSaturatedChannel = other.fHasSaturatedChannel;
  fSkippedByClustering = other.fSkippedByClustering;
  fNoiseTags = other.fNoiseTags;

  return *this;
}
//______________________________________________________________________________
bool EXOEventData::operator==(const EXOEventData& other) const
{
  // Do deep check of equivalence.
  // *WARNING* Does *NOT* check monte carlo data.
  bool retVal = 
    fRunNumber == other.fRunNumber and
    fEventNumber == other.fEventNumber and
    //fMonteCarloData == other.fMonteCarloData and
    fEventHeader == other.fEventHeader and
    fHasSaturatedChannel == other.fHasSaturatedChannel and
    fSkippedByClustering == other.fSkippedByClustering and
    fNoiseTags == other.fNoiseTags and
    GetNumUWireSignals() == other.GetNumUWireSignals() and
    GetNumUWireInductionSignals() == other.GetNumUWireInductionSignals() and
    GetNumVWireSignals() == other.GetNumVWireSignals() and
    GetNumAPDSignals() == other.GetNumAPDSignals() and 
    GetNumChargeClusters() == other.GetNumChargeClusters() and
    GetNumScintillationClusters() == other.GetNumScintillationClusters();
  if (not retVal) return false;
  if (not (*GetWaveformData() == *other.GetWaveformData())) return false; 
  for (size_t i=0;i<GetNumUWireSignals();i++) {
    if (not (*GetUWireSignal(i) == *other.GetUWireSignal(i))) return false;
  }
  for (size_t i=0;i<GetNumUWireInductionSignals();i++) {
    if (not (*GetUWireInductionSignal(i) == *other.GetUWireInductionSignal(i))) return false;
  }
  for (size_t i=0;i<GetNumChargeInjectionSignals();i++) {
    if (not (*GetChargeInjectionSignal(i) == *other.GetChargeInjectionSignal(i))) return false;
  }
  for (size_t i=0;i<GetNumVWireSignals();i++) {
    if (not (*GetVWireSignal(i) == *other.GetVWireSignal(i))) return false;
  }
  for (size_t i=0;i<GetNumAPDSignals();i++) {
    if (not (*GetAPDSignal(i) == *other.GetAPDSignal(i))) return false;
  }
  for (size_t i=0;i<GetNumChargeClusters();i++) {
    if (not (*GetChargeCluster(i) == *other.GetChargeCluster(i))) return false;
  }
  for (size_t i=0;i<GetNumScintillationClusters();i++) {
    if (not (*GetScintillationCluster(i) == *other.GetScintillationCluster(i))) return false;
  }

  return true;
}

//______________________________________________________________________________
int EXOEventData::FindAPDSignalLocation(EXOAPDSignal::EXOAPDSignal_type type, int pos, int n) const
{ 
  // Get index of first APD signal of this type, in range [pos, pos+n).
  if (pos<0) pos = 0;
  if (n!=-1) n += pos;
  if (n>=(int)GetNumAPDSignals()) n = GetNumAPDSignals();
  for (int i=pos;i<n;i++) {
    const EXOAPDSignal* sig = GetAPDSignal(i);
    if (sig && sig->fType==type)
      return i;
  }
  return -1;
}

//______________________________________________________________________________
Double_t EXOEventData::GetTotalPurityCorrectedEnergy() const
{
  Double_t ret = 0.0;
  TIter next(GetChargeClusterArray());
  const EXOChargeCluster* cc;
  while ((cc = (const EXOChargeCluster*) next())) ret += cc->fPurityCorrectedEnergy;
  return ret;
}

//______________________________________________________________________________
bool EXOEventData::IsTaggedAsNoise() const
{
  // Returns true if any noise tag was set.
  return (fNoiseTags.CountBits() > 0);
}

//______________________________________________________________________________
bool EXOEventData::IsTaggedAsNoiseOfType(EXOEventData::TypeOfNoiseTag type) const
{
  // Returns true if this particular noise tag was set.
  return fNoiseTags[int(type)];
}

//______________________________________________________________________________
bool EXOEventData::IsTaggedAsNoise_Excluding(EXOEventData::TypeOfNoiseTag type) const
{
  // Returns true if some noise tag besides type was set.
  // This is provided to ease encapsulation -- sometimes we want to ignore specific tags,
  // and don't wish to accomplish this by enumerating all other types of tags.
  TBits TempNoiseTags = fNoiseTags;
  TempNoiseTags.ResetBitNumber(int(type));
  return (TempNoiseTags.CountBits() > 0);
}

//______________________________________________________________________________
void EXOEventData::SetNoiseTag(EXOEventData::TypeOfNoiseTag type)
{
  // Activate this noise tag.  (No-op if it's already active.)
  fNoiseTags.SetBitNumber(int(type));
}

//______________________________________________________________________________
void EXOEventData::ClearNoiseTag(EXOEventData::TypeOfNoiseTag type)
{
  // Unset this noise tag.  (No-op if it wasn't set anyway.)
  fNoiseTags.ResetBitNumber(int(type));
}

//______________________________________________________________________________
void EXOEventData::ClearNoiseTags()
{
  // Clear all noise tags.
  fNoiseTags.ResetAllBits();
}
