#ifndef EXOEventHeader_hh
#define EXOEventHeader_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TBits
#include "TBits.h"
#endif
#include <string>
#include <cstddef> //for size_t

class TClonesArray;
class EXOTClonesArray;
class EXOTPCVetoSummary;

class EXOEventHeader : public TObject {

  public:
    EXOEventHeader();
    EXOEventHeader(const EXOEventHeader& copy);
    EXOEventHeader& operator=(const EXOEventHeader& copy);
    virtual void Clear( Option_t* = "" );
    ~EXOEventHeader();

    const EXOTPCVetoSummary* GetLastVetoAtChannel(UChar_t channel) const;
    EXOTPCVetoSummary* AddVetoChannel();
    const EXOTClonesArray*  GetVetoSummaryArray() const;
    size_t GetNumVetoSummaries() const;
    const EXOTPCVetoSummary* GetVetoSummary(size_t i) const;
    Long64_t           GetTimeSinceVeto();

    std::string fSVNRevision;                 // svn_rev       : Revision number of the code repository 
    std::string fBuildID;                     // build_id      : Build ID string, which attempts to uniquely identify the build.
    Int_t       fCompressionID;               // compressionid : Compression ID.
    Int_t       fFrameRevision;               // framerev      : Frame Revision
    UInt_t      fCardCount;                   // cc            : Card count.
    TBits       fCardsEnabled;                // cardsenabled  : Bitfield identifying enabled electronics cards.
    Int_t       fTriggerCount;                // trigcount     : Trigger count.
    UInt_t      fTriggerSeconds;              // trigsec       : Time stamp in seconds.
    UInt_t      fTriggerMicroSeconds;         // trigsub       : Time stamp in microseconds.
    Int_t       fTriggerDrift;                // trigdrift     : Trigger drift.
    UInt_t      fSumTriggerThreshold;         // sn            : Sum trigger threshold.
    UInt_t      fSumTriggerRequest;           // sr            : Sum trigger request.
    UInt_t      fSumTriggerValue;             // trigsum       : Computed trigger sum value.
    UInt_t      fIndividualTriggerThreshold;  // in            : Individual trigger threshold.
    UInt_t      fIndividualTriggerRequest;    // ir            : Individual trigger request.
    UInt_t      fMaxValueChannel;             // indsum        : Channel with max value.
    UInt_t      fTriggerOffset;               // trigoff       : Location of trigger sample within event.
    UInt_t      fTriggerSource;               /* src           : Trigger source.    \
                                                                   1: APD           \
                                                                   2: VG (U Wires)  \
                                                                   3: HV (V Wires) */
    Int_t       fSampleCount;                 // samplecount   : Number of samples in frame, 0 = 1, 2047 = 2048.
    Int_t       fGeant4EventNumber;           /* g4ne          : GEANT4 event number. If the simulated event includes a decay chain or  \
                                                                 metastable states, then g4ne can be different from ne.*/
    Int_t       fGeant4SubEventNumber;        // subne         : Sub-event number, in the case of a decay chain or metastable states.   
    Bool_t      fIsMonteCarloEvent;           /* is_mc         : True if the event originates from a geant simulation. Note that it can \
                                                                 be fooled if you convert to binary format and read it back in, and also\
                                                                 that it will always read false if the event was generated before svn   \
                                                                 revision 2706.*/ 
    Bool_t      fTaggedAsMuon;                // muontag            : True if a module tags the event as a muon
    Bool_t      fTaggedAsMuonCandidate;       //                    : True if a module tags the event as a muon candidate (but may not have passed track cuts)
    Double_t    fMuonDriftVelocity;           // muon_driftvelocity : Drift velocity in mm/us computed from muons crossing cathode
    Double_t    fMuonTheta;                   // muon_theta         : Reconstructed angle of the muon from the zenith
    Double_t    fMuonPhi;                     // muon_phi           : Reconstructed azimuthal angle of the muon (0 means it came from N/+z, +pi/2 means E/-x)
    Int_t    fMuonTime;                       // muon_time         : Time of start of scintillation for the muon
    Bool_t      fTaggedAsNoise;               // noisetag           : True if a module tags the event as noise
                                              // Note:  this variable is deprecated; replace with EXOEventData::fNoiseTags.
                                              //        It will be removed a suitable amount of time after 10/9/2012.

    Bool_t      fSirenActiveInCR;             // : Indicates that the alarm siren in the clean room is sounding for any reason (maybe in the future there should be more variables for specific alarms? SS, 10/20/11)
    Bool_t     fNoiseRemoved;                //   Indicates that U-Wire signals were removed by the Recon Noise tagger
    Bool_t     fOverlapNoise;                //  Marks if the Event was tagged as noise but later untagged as possible coicident Event


    bool operator==(const EXOEventHeader& comp) const;

  protected:

    EXOTClonesArray*  GetVetoSummaryArray();
    TClonesArray* fVetoSummaryArray;          //->list of EXOTPCVetoSummary, holding the most recent time each channel fired.

  ClassDef( EXOEventHeader, 16 )

};

#endif /* EXOEventHeader_hh */
