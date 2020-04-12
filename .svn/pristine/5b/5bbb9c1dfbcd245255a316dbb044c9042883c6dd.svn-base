#ifndef EXOControlRecord_hh
#define EXOControlRecord_hh

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOControlRecord                                                     //
//                                                                      //
// Encapsulation class for control records from binary files.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TTimeStamp
#include "TTimeStamp.h"
#endif
#include <string>
#include <cstddef> //for size_t
//______________________________________________________________________________
//
// Base class of all control records
//
#define DEF_EXOCONTROL_RECORD(name, num)     \
  friend class EXOBinaryFileInputModule;     \
  ClassDef(name, num)

#define DEFINE_EXOCONTROL_SIMPLE_RECORD(derived,parent) \
class derived : public parent {                         \
  DEF_EXOCONTROL_RECORD(derived,1)                      \
};


class EXOControlRecord : public TObject
{

  public:

   enum RecordType 
   {
     kIgnoredFrame = -2,
     kNotSet = -1,
     kRecordUnknown = 0,
     kBegin,
     kPause,
     kResume,
     kEnd,
     kSegmentBegin,
     kSegmentEnd,
     kNumberOfRecordTypes
   };

   enum FrameType
   {
     kFrameUnknown = 0,
     kTpcData,
     kTpcEmpty,
     kTpcDamaged,
     kVetoData,
     kVetoDamaged,
     kNumberOfFrameTypes
   };

   enum EventArraySize
   {
       kEventArraySize = 7 // This should have been 6 (and equal to
			   // kNumberOfFrameTypes), but I messed up.  We stick
			   // with the extra value, it is protected by the
			   // protected interface anyways.  
   };


   virtual RecordType GetType() const { return kNotSet; } 
   virtual std::string GetTypeString() const
     { return GetStringNameOfType(GetType()); }

   ULong64_t GetTimestamp() const 
   {
     // Return the unix time, in ns.
     return fTimestamp;
   }

   TTimeStamp GetTimestampObj() const;

   Int_t GetRunNumber() const 
     { return fRunNumber; }

   Int_t GetPreviousEventNumber() const 
     { return fPreviousEventNumber; }

   void Print(Option_t* = "") const;
 
   static std::string GetStringNameOfType(RecordType type);
   static std::string GetStringNameOfFrameType(FrameType type);

   EXOControlRecord() : 
     fTimestamp(0),
     fRunNumber(0),
     fPreviousEventNumber(0)
   {}

   EXOControlRecord(ULong64_t time,
                    Int_t     runNo,
                    Int_t     prevEventNo) : TObject(),
     fTimestamp(time),
     fRunNumber(runNo),
     fPreviousEventNumber(prevEventNo)
   {} 

  protected:

   friend class EXOControlRecordList;
   ULong64_t fTimestamp;           // Time in nano-seconds (don't worry, this will work until around 2514).
   Int_t     fRunNumber;           // Run number that this control record is in
   Int_t     fPreviousEventNumber; // The event number that comes immediately before this

  DEF_EXOCONTROL_RECORD(EXOControlRecord,1)  // Base class for control records
};

class EXOUnknownControlRecord : public EXOControlRecord
{
  public:
    RecordType GetType() const { return kRecordUnknown; } 
  DEF_EXOCONTROL_RECORD(EXOUnknownControlRecord,1)  // Base class for control records
};

//______________________________________________________________________________
//
// Base class of all ignored frames.
//
//______________________________________________________________________________
class EXOIgnoredFrameRecord : public EXOControlRecord
{
  public:
    EXOIgnoredFrameRecord() : 
      EXOControlRecord(), 
      fFrameType(kFrameUnknown) 
    {}
    RecordType GetType() const { return kIgnoredFrame; }
    // Note, if the the frame type is kFrameUnknown
    FrameType GetFrameType() const { return fFrameType; }
    void Print(Option_t* = "") const;

  protected:
    FrameType fFrameType; // Type of the ignored frame

  DEF_EXOCONTROL_RECORD(EXOIgnoredFrameRecord,1)  // Base class for control records
};

//______________________________________________________________________________
//
// Base class of all begin run records.
//
//______________________________________________________________________________
class EXOBeginRecord : public EXOControlRecord
{
  public:
   enum RunFlavor 
   {
     kUnknownFlavor = -1,
     kDatPhysics = 0,  /* Normal physics data run     */
     kDatSrcClb  ,     /* Source calibration data run */
     kDatNoise   ,     /* Noise data run              */
     kClbInt     ,     /* Internal calibration run    */
     kClbExt     ,     /* External calibration run    */
     kClbCombined,     /* Combined calibration run    */
     kClbLaser         /* Laser calibration run       */
   };


    RecordType GetType() const { return kBegin; }
    virtual RunFlavor GetRunFlavor() const 
      { return kUnknownFlavor; }

    virtual std::string GetRunFlavorString() const
      { return GetStringOfRunFlavorType(GetRunFlavor()); }

    void Print(Option_t* = "") const;
    static std::string GetStringOfRunFlavorType(RunFlavor);

    UInt_t GetOutputFormat() const { return fOutputFormat; } 
    UInt_t GetMaxRecsize() const { return fMaxRecsize; }   
    UInt_t GetCardSet() const { return fCardSet; }      


  protected:
    UInt_t fOutputFormat; // The output format style 
    UInt_t fMaxRecsize;   // The maximum output record size, bytes
    UInt_t fCardSet;      // The set of enabled front-end cards 

  DEF_EXOCONTROL_RECORD(EXOBeginRecord,1)  // Base class for begin records 
};
//______________________________________________________________________________
//
// Base class for all pause records
//
//______________________________________________________________________________
class EXOPauseRecord : public EXOControlRecord
{
  public:
    RecordType GetType() const { return kPause; }

    Float_t GetIdentifier() const { return fIdentifier; }
    UInt_t GetNEvents(FrameType i) const { return fNEvents[i]; }
    void Print(Option_t* = "") const;

  protected: 
    Float_t fIdentifier; // Pause identifer, identifier in the next resume record must match this 
    UInt_t fNEvents[kEventArraySize]; // Count of event types taken since the last resume 

  DEF_EXOCONTROL_RECORD(EXOPauseRecord,1)  // Pause run record 

};

//______________________________________________________________________________
//
// Base class for all resume records
//
class EXOResumeRecord : public EXOControlRecord
{

  public:
    RecordType GetType() const { return kResume; }

    Float_t GetIdentifier() const { return fIdentifier; }

  protected: 
    Float_t fIdentifier; // Resume identifer, must match the identifier in the previous pause record
  
  DEF_EXOCONTROL_RECORD(EXOResumeRecord,1)  // Resume run record 

};
 
//______________________________________________________________________________
//
// Base class for all end records
//
class EXOEndRecord : public EXOControlRecord
{
  public:
    RecordType GetType() const { return kEnd; }

    UInt_t GetNEvents(FrameType i) const { return fNEvents[i]; }
    void Print(Option_t* = "") const;

  protected:
    UInt_t fNEvents[kEventArraySize]; // Count of event types taken since the last resume 
  
  DEF_EXOCONTROL_RECORD(EXOEndRecord,1)  // End run record 
};

//______________________________________________________________________________
//
// DATA Records
//
//______________________________________________________________________________
//
// Base class of all begin data run records.
//
//______________________________________________________________________________
class EXOBeginDataRunRecord : public EXOBeginRecord
{
  public:
    Float_t GetKeepAlive() const { return fKeepAlive; }
    UShort_t GetSuppress(size_t i) const { return fSuppress[i]; }
  protected:
    Float_t  fKeepAlive;
    UShort_t fSuppress[18]; // The list of front-end channels are not written 

  DEF_EXOCONTROL_RECORD(EXOBeginDataRunRecord, 1); // Base class for all begin data records

};


//______________________________________________________________________________
//
// Physics Records 
//
//______________________________________________________________________________
class EXOBeginPhysicsRunRecord : public EXOBeginDataRunRecord
{
  public:
    RunFlavor GetRunFlavor() const { return kDatPhysics; }

  DEF_EXOCONTROL_RECORD(EXOBeginPhysicsRunRecord,1)  // Begin of a physics run record 
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOPausePhysicsRunRecord,  EXOPauseRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOResumePhysicsRunRecord, EXOResumeRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndPhysicsRunRecord,    EXOEndRecord)
//______________________________________________________________________________

//______________________________________________________________________________
//
// Source Calibration Records 
//
//______________________________________________________________________________
class EXOBeginSourceCalibrationRunRecord : public EXOBeginDataRunRecord
{
  public:
    enum SourceType
    {
      kCoWeak   = 0, // Weak   Cobalt   60 ( 3.0 kBq)
      kCoStrong = 1, // Strong Cobalt   60 (15.0 kBq)
     
      kCsWeak   = 2, // Weak   Cesium  137 ( 0.5 kBq)
      kCsStrong = 3, // Strong Cesium  137 ( 7.2 kBq)
     
      kThWeak   = 4, // Weak   Thorium 228 ( 1.5 kBq)
      kThStrong = 5, // Strong Thorium 228 (38.0 kBq)

      kRaWeak   = 6, // Weak   Radium  226 (strength deliberately blinded)
    };

    enum SourcePosition
    {
                    /* Location in cm(    x,     y,     z)                   */
      kP2_nz = 0, /*!<             (  0.0,   0.0, -30.4)                     */
      kP2_pz = 1, /*!<             (  0.0,   0.0,  30.4)                     */
     
      kP4_ny = 2, /*!<             (  0.0, -25.4,   0.0)                     */
      kP4_py = 3, /*!<             (  0.0,  25.4,   0.0)                     */
     
      kP4_px = 4  /*!<             ( 25.4,   0.0,   0.0)                     */
    };

    RunFlavor GetRunFlavor() const { return kDatSrcClb; }
    SourceType GetSourceType() const { return (SourceType) fSourceType; }
    SourcePosition GetSourcePosition() const { return (SourcePosition) fSourcePosition; }
    std::string GetSourcePositionString() const 
      { return GetStringOfSourcePosition( GetSourcePosition() ); }
    std::string GetSourceTypeString() const 
      { return GetStringOfSourceType( GetSourceType() ); }
    
    UShort_t GetReserved() const { return fReserved; }

    static std::string GetStringOfSourcePosition(SourcePosition pos);
    static std::string GetStringOfSourceType(SourceType type);
   
    void Print(Option_t* = "") const;
  protected:
    Char_t fSourceType;     // The source type
    Char_t fSourcePosition; // Source position
    UShort_t fReserved;     // Anything reserved for future use.

  DEF_EXOCONTROL_RECORD(EXOBeginSourceCalibrationRunRecord,1)  // Source cal begin run record 
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOPauseSourceCalibrationRunRecord,  EXOPauseRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOResumeSourceCalibrationRunRecord, EXOResumeRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndSourceCalibrationRunRecord,    EXOEndRecord)

//______________________________________________________________________________
//
// Noise Calibration Records 
//
//______________________________________________________________________________
class EXOBeginNoiseCalibrationRunRecord : public EXOBeginDataRunRecord
{
  public:
    RunFlavor GetRunFlavor() const { return kDatNoise; }
 
  DEF_EXOCONTROL_RECORD(EXOBeginNoiseCalibrationRunRecord,1) // Noise cal begin run record
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOPauseNoiseCalibrationRunRecord,  EXOPauseRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOResumeNoiseCalibrationRunRecord, EXOResumeRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndNoiseCalibrationRunRecord,    EXOEndRecord)

//______________________________________________________________________________

//______________________________________________________________________________
//
// Calibration base classes
//
//______________________________________________________________________________
class EXOBeginCalibrationRunRecord : public EXOBeginRecord
{
  public:
    Int_t GetNSegments() const { return fNSegments; }

  protected:
    Int_t fNSegments; // Number of segments
  
  DEF_EXOCONTROL_RECORD(EXOBeginCalibrationRunRecord,1)  // Generic cal begin run record 
};

//______________________________________________________________________________
class EXOBeginSegmentCalibrationRecord : public EXOControlRecord
{
  public:
    RecordType GetType() const { return kSegmentBegin; }

    Int_t GetIdentifier() const { return fIdentifier; }
    Int_t GetNEvents() const { return fNEvents; }
    Float_t GetRate() const { return fRate; }
    Int_t GetInterval() const { return fInterval; }
    UShort_t GetDac() const { return fDac; }
    UShort_t GetDelay() const { return fDelay; }
    UShort_t GetEnabled(size_t i) const { return fEnabled[i]; }
    UShort_t GetSuppress(size_t i) const { return fSuppress[i]; }
 
  protected:
    Int_t fIdentifier; /* The segment identifier. The identifier \
                          found in the next SegmentEnd record    \
                          must match this value */
    Int_t fNEvents;  /* The number of events in this segment */
    Float_t fRate;   /* The trigger rate, in Hz */
    Int_t fInterval; /* The minimum trigger interval, in clock ticks */
    UShort_t fDac;   /* The DAC value */
    UShort_t fDelay; /* The delay time, in clock ticks */
    UShort_t fEnabled[18];  /* The list of front-end channels that \
                               are enabled to receive the DAC pulse */
    UShort_t fSuppress[18]; /* The list of front-end channels that \
 	                       will not be written out */
  
  DEF_EXOCONTROL_RECORD(EXOBeginSegmentCalibrationRecord,1)  // Generic cal begin segment record 
};

//______________________________________________________________________________
class EXOEndSegmentCalibrationRecord : public EXOControlRecord
{
  public:
    RecordType GetType() const { return kSegmentEnd; }
    Int_t GetIdentifier() const { return fIdentifier; }
    UInt_t GetNEvents(FrameType i) const { return fNEvents[i]; }
    void Print(Option_t* = "") const;

  protected:
    Int_t fIdentifier; /* The segment identifier. The identifier   \
                          must match the one found in the previous \
                          SegmentBegin record. */
    UInt_t fNEvents[kEventArraySize]; // Count of event types taken in this segment 

  DEF_EXOCONTROL_RECORD(EXOEndSegmentCalibrationRecord,1)  // Generic cal end segment record 
};

//______________________________________________________________________________
class EXOEndCalibrationRecord : public EXOEndRecord
{
  public:
    Int_t GetNSegments() const { return fNSegments; }
  protected:
    Int_t fNSegments; // Number of segments
  
  DEF_EXOCONTROL_RECORD(EXOEndCalibrationRecord,1)  // Generic cal end run record 
};

//______________________________________________________________________________
//
// Internal Calibration 
//
//______________________________________________________________________________
class EXOBeginInternalCalibrationRunRecord : public EXOBeginCalibrationRunRecord 
{
  public:
    RunFlavor GetRunFlavor() const { return kClbInt; }

  DEF_EXOCONTROL_RECORD(EXOBeginInternalCalibrationRunRecord,1)  // Internal calibration begin record 
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndInternalCalibrationRunRecord,    EXOEndCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOBeginSegmentInternalCalibrationRunRecord,    
  EXOBeginSegmentCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndSegmentInternalCalibrationRunRecord,    
  EXOEndSegmentCalibrationRecord)
//______________________________________________________________________________

//______________________________________________________________________________
//
// External Calibration 
//
//______________________________________________________________________________
class EXOBeginExternalCalibrationRunRecord : public EXOBeginCalibrationRunRecord
{
  public:
    RunFlavor GetRunFlavor() const { return kClbExt; }

    UInt_t GetSerialNumber() const { return fSerialNumber; }
    UInt_t GetCardNumber() const { return fCardNumber; }

  protected:
    UInt_t fSerialNumber; // Serial number
    UInt_t fCardNumber;   // Card number
  
  DEF_EXOCONTROL_RECORD(EXOBeginExternalCalibrationRunRecord,1)  // External cal begin run record 
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndExternalCalibrationRunRecord,    EXOEndCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOBeginSegmentExternalCalibrationRunRecord,    
  EXOBeginSegmentCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndSegmentExternalCalibrationRunRecord,    
  EXOEndSegmentCalibrationRecord)

//______________________________________________________________________________

//______________________________________________________________________________
//
// Combined Calibration 
//
//______________________________________________________________________________
class EXOBeginCombinedCalibrationRunRecord : public EXOBeginCalibrationRunRecord
{
  public:
    RunFlavor GetRunFlavor() const { return kClbCombined; }

    UInt_t GetSerialNumber() const { return fSerialNumber; }
    UInt_t GetCardNumber() const { return fCardNumber; }

  protected:
    UInt_t fSerialNumber; // Serial number
    UInt_t fCardNumber;   // Card number
  
  DEF_EXOCONTROL_RECORD(EXOBeginCombinedCalibrationRunRecord,1)  // Combined cal begin run record 
};
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndCombinedCalibrationRunRecord,    EXOEndCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOBeginSegmentCombinedCalibrationRunRecord,    
  EXOBeginSegmentCalibrationRecord)
DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndSegmentCombinedCalibrationRunRecord,    
  EXOEndSegmentCalibrationRecord)

//______________________________________________________________________________
//______________________________________________________________________________

//______________________________________________________________________________
//
// Laser Calibration 
//
//______________________________________________________________________________
class EXOBeginLaserCalibrationRunRecord : public EXOBeginCalibrationRunRecord
{
  public:
    RunFlavor GetRunFlavor() const { return kClbLaser; }

  DEF_EXOCONTROL_RECORD(EXOBeginLaserCalibrationRunRecord,1)  // Laser cal begin run record 
};

DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndLaserCalibrationRunRecord,    EXOEndCalibrationRecord)

class EXOBeginSegmentLaserCalibrationRunRecord : public EXOBeginSegmentCalibrationRecord
{
  public:
    enum PatternType {
      kReserved = 0,   // reserved for future use
      kNorthOnly,      // North only
      kSouthOnly,      // South only
      kAlternate,      // Alternating, north, then south 
      kNumPatternTypes
    };
    struct Pulse {
      UShort_t fDac;      // DAC setting for laser amplitude
      UShort_t fDuration; // The duration of the laser pulse 
    };

    enum HvIndex {
      kInduction = 0, // HV on the induction plane
      kBias,          // HV on the biase     plane
      kTrim0,         // HV on the trim 0    pie  
      kTrim1,         // HV on the trim 1    pie  
      kTrim2,         // HV on the trim 2    pie  
      kTrim3,         // HV on the trim 3    pie  
      kTrim4,         // HV on the trim 4    pie  
      kTrim5,         // HV on the trim 5    pie  
      kNumHvIndex
    };

    enum APDEnd {
      kNorth = 0,
      kSouth
    };

    class Conditions {
      public:
        const Pulse& GetPulse() const { return fPulse; } 
        Float_t GetHV(size_t i) const { return fHV[i]; } 

      protected:
        friend class EXOBinaryFileInputModule;
        Pulse   fPulse;           /* pulse pattern for this end */
        Float_t fHV[kNumHvIndex]; /* HV for this end.           */
    };

    PatternType GetPattern() const { return (PatternType) fPattern; }
    const Conditions& GetNorthConditions() const
      { return fConditionsNorth; }
    const Conditions& GetSouthConditions() const
      { return fConditionsSouth; }

  protected:
    UChar_t    fPattern;         /* Pulsing pattern */
    Conditions fConditionsNorth; /* North calibration conditions */
    Conditions fConditionsSouth; /* South calibration conditions */

  DEF_EXOCONTROL_RECORD(EXOBeginSegmentLaserCalibrationRunRecord, 1);

};

DEFINE_EXOCONTROL_SIMPLE_RECORD(EXOEndSegmentLaserCalibrationRunRecord,    
  EXOEndSegmentCalibrationRecord)


#endif /* EXOControlRecord_hh */

