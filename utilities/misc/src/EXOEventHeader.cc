//______________________________________________________________________________
//
// EXOEventHeader
//
// Encapsulates information belonging to the event header.
//
// Note:  There should always be an fVetoSummaryArray array, even if it's empty.
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOUtilities/EXOTPCVetoSummary.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOTClonesArray.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

#include <limits>

ClassImp( EXOEventHeader )

//______________________________________________________________________________
EXOEventHeader::EXOEventHeader()
: TObject()
{
  fVetoSummaryArray = new EXOTClonesArray(EXOTPCVetoSummary::Class(), NUM_VETO_CHANNELS);
  Clear();
}
//______________________________________________________________________________
EXOEventHeader::EXOEventHeader(const EXOEventHeader& copy)
: TObject(copy)
{
  fSVNRevision = copy.fSVNRevision;
  fBuildID = copy.fBuildID;
  fCompressionID = copy.fCompressionID;
  fFrameRevision = copy.fFrameRevision;
  fCardCount = copy.fCardCount;
  fCardsEnabled = copy.fCardsEnabled;
  fTriggerCount = copy.fTriggerCount;
  fTriggerSeconds = copy.fTriggerSeconds;
  fTriggerMicroSeconds = copy.fTriggerMicroSeconds;
  fTriggerDrift = copy.fTriggerDrift;
  fSumTriggerThreshold = copy.fSumTriggerThreshold;
  fSumTriggerRequest = copy.fSumTriggerRequest;
  fSumTriggerValue = copy.fSumTriggerValue;
  fIndividualTriggerThreshold = copy.fIndividualTriggerThreshold;
  fIndividualTriggerRequest = copy.fIndividualTriggerRequest;
  fMaxValueChannel = copy.fMaxValueChannel;
  fTriggerOffset = copy.fTriggerOffset;
  fTriggerSource = copy.fTriggerSource;
  fSampleCount = copy.fSampleCount;
  fGeant4EventNumber = copy.fGeant4EventNumber;
  fGeant4SubEventNumber = copy.fGeant4SubEventNumber;
  fIsMonteCarloEvent = copy.fIsMonteCarloEvent;
  fTaggedAsMuon = copy.fTaggedAsMuon;
  fTaggedAsMuonCandidate = copy.fTaggedAsMuonCandidate;
  fMuonDriftVelocity = copy.fMuonDriftVelocity;
  fMuonTheta = copy.fMuonTheta;
  fMuonPhi = copy.fMuonPhi;
  fMuonTime = copy.fMuonTime;
  fTaggedAsNoise = copy.fTaggedAsNoise;
  fSirenActiveInCR = copy.fSirenActiveInCR;
  fNoiseRemoved    = copy.fNoiseRemoved;
  fOverlapNoise    = copy.fOverlapNoise;
  fVetoSummaryArray = new EXOTClonesArray(*copy.fVetoSummaryArray);
}
//______________________________________________________________________________
EXOEventHeader& EXOEventHeader::operator=(const EXOEventHeader& copy)
{
  if(this == &copy) return *this;

  this->TObject::operator=(copy);

  fSVNRevision = copy.fSVNRevision;
  fBuildID = copy.fBuildID;
  fCompressionID = copy.fCompressionID;
  fFrameRevision = copy.fFrameRevision;
  fCardCount = copy.fCardCount;
  fCardsEnabled = copy.fCardsEnabled;
  fTriggerCount = copy.fTriggerCount;
  fTriggerSeconds = copy.fTriggerSeconds;
  fTriggerMicroSeconds = copy.fTriggerMicroSeconds;
  fTriggerDrift = copy.fTriggerDrift;
  fSumTriggerThreshold = copy.fSumTriggerThreshold;
  fSumTriggerRequest = copy.fSumTriggerRequest;
  fSumTriggerValue = copy.fSumTriggerValue;
  fIndividualTriggerThreshold = copy.fIndividualTriggerThreshold;
  fIndividualTriggerRequest = copy.fIndividualTriggerRequest;
  fMaxValueChannel = copy.fMaxValueChannel;
  fTriggerOffset = copy.fTriggerOffset;
  fTriggerSource = copy.fTriggerSource;
  fSampleCount = copy.fSampleCount;
  fGeant4EventNumber = copy.fGeant4EventNumber;
  fGeant4SubEventNumber = copy.fGeant4SubEventNumber;
  fIsMonteCarloEvent = copy.fIsMonteCarloEvent;
  fTaggedAsMuon = copy.fTaggedAsMuon;
  fTaggedAsMuonCandidate = copy.fTaggedAsMuonCandidate;
  fMuonDriftVelocity = copy.fMuonDriftVelocity;
  fMuonTheta = copy.fMuonTheta;
  fMuonPhi = copy.fMuonPhi;
  fMuonTime = copy.fMuonTime;
  fTaggedAsNoise = copy.fTaggedAsNoise;
  fSirenActiveInCR = copy.fSirenActiveInCR;
  fNoiseRemoved    = copy.fNoiseRemoved; 
  fOverlapNoise    = copy.fOverlapNoise;

  *static_cast<EXOTClonesArray*>(fVetoSummaryArray) = *copy.fVetoSummaryArray;

  return *this;
}
//______________________________________________________________________________
EXOEventHeader::~EXOEventHeader()
{
  delete fVetoSummaryArray; // I believe that a TClonesArray always owns its contents, so they needn't be deleted separately.
}
//______________________________________________________________________________
void EXOEventHeader::Clear( Option_t* )
{
  // Reset header to initial state. 
  fSVNRevision                = "";
  fBuildID                    = "";
  fCompressionID              = 0;
  fFrameRevision              = 0;
  fCardCount                  = 0;
  fCardsEnabled.Clear();
  fTriggerCount               = 0;
  fTriggerSeconds             = 0;
  fTriggerMicroSeconds        = 0;
  fTriggerDrift               = 0;
  fSumTriggerThreshold        = 0;
  fSumTriggerRequest          = 0;
  fSumTriggerValue            = 0;
  fIndividualTriggerThreshold = 0;
  fIndividualTriggerRequest   = 0;
  fMaxValueChannel            = 0;
  fTriggerOffset              = 0;
  fTriggerSource              = 0;
  fSampleCount                = 0;
  fGeant4EventNumber          = 0;
  fGeant4SubEventNumber       = 0;
  fIsMonteCarloEvent          = false;
                              
  fTaggedAsMuonCandidate       = false;
  fTaggedAsMuon               = false;
  fMuonDriftVelocity          = 0;
  fMuonTheta                  = 0.0;
  fMuonPhi                    = 0.0;
  fMuonTime                   = 0;
  fTaggedAsNoise              = false;
  fSirenActiveInCR            = false;
  fNoiseRemoved               = false;
  fOverlapNoise               = false;

  fVetoSummaryArray->Clear("C");
}
//___________________________________________________________________________
const EXOTPCVetoSummary* EXOEventHeader::GetLastVetoAtChannel(UChar_t channel) const
{
  // Return the most recent veto hit to channel.
  // Note that we allow the veto to have occurred during or prior to the TPC trace.
  // If the channel has never fired, return NULL.

  for(size_t i = 0; i < GetNumVetoSummaries() ; i++) {
    const EXOTPCVetoSummary* aVeto = GetVetoSummary(i); 
    if(aVeto->fChannel == channel) return aVeto;
  }
  return NULL;
}
//___________________________________________________________________________
const EXOTClonesArray* EXOEventHeader::GetVetoSummaryArray() const
{
  // Get array of veto objects
  return static_cast<const EXOTClonesArray*>(fVetoSummaryArray); 
}
//___________________________________________________________________________
EXOTClonesArray* EXOEventHeader::GetVetoSummaryArray() 
{
  // Get array of veto objects
  return static_cast<EXOTClonesArray*>(fVetoSummaryArray); 
}
//___________________________________________________________________________
size_t EXOEventHeader::GetNumVetoSummaries() const
{
  // Get the number of veto summaries.
  return (size_t) GetVetoSummaryArray()->GetEntriesFast();
}
//___________________________________________________________________________
EXOTPCVetoSummary* EXOEventHeader::AddVetoChannel()
{
  // Get a new veto object.  Note that the user is responsible for filling the
  // object with sensible information: a valid channel that has not already
  // been filled, and the correct corresponding mask.  No checking is done.
  return static_cast<EXOTPCVetoSummary*> (GetVetoSummaryArray()->GetNewOrCleanedObject(GetNumVetoSummaries()));
}
//__________________________________________________________________________
Long64_t EXOEventHeader::GetTimeSinceVeto()
{
  // Gets the time (in us) since the most recent veto by scanning through the
  // list fVetoSummaryArray for the minimum time.  Negative times are OK --
  // they indicate time after the TPC trigger, but should still be within the
  // TPC trace.  When there are no vetoes, the default is as large as possible,
  // ie. std::numeric_limits<Long64_t>::max().

  Long64_t MinTime = std::numeric_limits<Long64_t>::max();
  for(size_t i = 0; i < GetNumVetoSummaries(); i++) {
    const EXOTPCVetoSummary* aVeto = GetVetoSummary(i); 
    if(aVeto->fMicroSecondsBeforeTPCTrigger < MinTime) MinTime = aVeto->fMicroSecondsBeforeTPCTrigger;
  }
  return MinTime;
}
//__________________________________________________________________________
const EXOTPCVetoSummary* EXOEventHeader::GetVetoSummary(size_t i) const 
{
  // Get Veto summary at array position i
  return static_cast<const EXOTPCVetoSummary*> (GetVetoSummaryArray()->At(i));
}
//__________________________________________________________________________
bool EXOEventHeader::operator==(const EXOEventHeader& comp) const
{
  bool check_data = 
 //      fSVNRevision == comp.fSVNRevision and
 //      fBuildID == comp.fBuildID and
        fCompressionID == comp.fCompressionID and
        fFrameRevision == comp.fFrameRevision and
        fCardCount == comp.fCardCount and
        fCardsEnabled == comp.fCardsEnabled and
        fTriggerCount == comp.fTriggerCount and
        fTriggerSeconds == comp.fTriggerSeconds and
        fTriggerMicroSeconds == comp.fTriggerMicroSeconds and
        fTriggerDrift == comp.fTriggerDrift and
        fSumTriggerThreshold == comp.fSumTriggerThreshold and
        fSumTriggerRequest == comp.fSumTriggerRequest and
        fSumTriggerValue == comp.fSumTriggerValue and
        fIndividualTriggerThreshold == comp.fIndividualTriggerThreshold and
        fIndividualTriggerRequest == comp.fIndividualTriggerRequest and
        fMaxValueChannel == comp.fMaxValueChannel and
        fTriggerOffset == comp.fTriggerOffset and
        fTriggerSource == comp.fTriggerSource and
        fSampleCount == comp.fSampleCount and
        fGeant4EventNumber == comp.fGeant4EventNumber and
        fGeant4SubEventNumber == comp.fGeant4SubEventNumber and
        fIsMonteCarloEvent == comp.fIsMonteCarloEvent and
        fTaggedAsMuon == comp.fTaggedAsMuon and
        fTaggedAsMuonCandidate == comp.fTaggedAsMuonCandidate and
        fMuonDriftVelocity == comp.fMuonDriftVelocity and
        fMuonTheta == comp.fMuonTheta and
        fMuonPhi == comp.fMuonPhi and
        fMuonTime == comp.fMuonTime and
        fTaggedAsNoise == comp.fTaggedAsNoise and
        fSirenActiveInCR == comp.fSirenActiveInCR and 
        fNoiseRemoved    == comp.fNoiseRemoved    and
        fOverlapNoise    == comp.fOverlapNoise    and
        GetNumVetoSummaries() == comp.GetNumVetoSummaries();
  if (not check_data) {
    LogEXOMsg("Header information is not the same", EEDebug);
    return check_data;
  }
  
  // Now check the Veto data 
  size_t numVetoes = GetNumVetoSummaries();
  for (size_t i=0;i<numVetoes;i++) {
    if ( not (*comp.GetVetoSummary(i) == *GetVetoSummary(i)) ) {
      LogEXOMsg(Form("TPC Veto Summaries at %i are not equivalent", (int)i), EEDebug);
      return false; 
    }
  }
  return true;
}
