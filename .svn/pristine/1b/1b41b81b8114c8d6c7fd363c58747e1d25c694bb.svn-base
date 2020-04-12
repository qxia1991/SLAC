#ifndef EXOBinaryFileInputModule_hh
#define EXOBinaryFileInputModule_hh
#ifdef HAVE_EXOBIN

#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOBinaryPackage/Run/Type.hh"
#include "EXOUtilities/EXOControlRecordList.hh"
#include "TBits.h"

#include <deque>

class EXOEventData;

namespace Exo { 
namespace File {
  namespace Frame { 
    class Descriptor;
    namespace TpcData { class Descriptor; }
    namespace VetoData { class Descriptor; }
  }
  namespace Control { 
    namespace Begin        { class Descriptor; }
    namespace End          { class Descriptor; }
    namespace Pause        { class Descriptor; }
    namespace Resume       { class Descriptor; }
    namespace SegmentBegin { class Descriptor; }
    namespace SegmentEnd   { class Descriptor; }
  }
  class EXOAnalysisReader;
} 
} // end namespaces

class EXOChannelMap;
class EXOBinaryFileInputModule: public EXOInputModule 
{

  protected :

    struct VetoSummary
    {
      VetoSummary();
      void Clear();
      TBits fMask; // The mask of the veto event.
      Long64_t fMicroSeconds; // Unix time in microseconds of the veto trigger.
    };

    // Memory pool and helper functions for EXOEventData.
    std::vector <EXOEventData*> fEventDataMemoryPool;
    EXOEventData* GetCleanEventData();

    // Memory pool and helper functions for VetoSummary.
    std::vector <VetoSummary*> fVetoSummaryMemoryPool;
    VetoSummary* GetCleanVetoSummary();

    const static Long64_t fCoincidenceCutMicroSeconds = 2; // A tpc with trigger time T can be associated with a veto as late as T + 2us.
    const static Long64_t fMaxStreamLagMicroSeconds = 10000; // How far out of sync may the records be?  See class description.
    Long64_t fVetoStreamMicroSeconds; // What time has the veto stream been read to?

    // Queues
    std::deque<EXOEventData*> fEventDataDeque;
    std::deque<VetoSummary*> fVetoSummaryDequeArray[NUM_VETO_CHANNELS]; // one deque for each veto channel.

    EXOEventData* FlushEventDataDeque(bool force = false);

    enum EControlStatus {kNoFile,
                         kFileOpened,
                         kBeginRun,
                         kBeginSegment,
                         kEndSegment,
                         kPause,
                         kResume,
                         kEndRun,
                         kEndFile};

    EControlStatus fLastControlRecord;
    void UpdateControlStatus(const EControlStatus NewStatus);

    int          fEventsProcessed;
    int          fCurrentEventNumber;
    int          fCurrentRunNumber;
    int          fSkipEvents;

    Exo::File::EXOAnalysisReader *binaryFile;
    Exo::Run::Type::Type          fCurrentRunType;
    bool                          fRunTypeKnown;

    unsigned short int map_data[18];

    EXOControlRecordList fRecords;

    void replace_cleared_data(EXOEventData* event);
    void handle_frame_tpcdata( Exo::File::Frame::TpcData::Descriptor& frameDescriptor );
    void handle_frame_vetodata( Exo::File::Frame::VetoData::Descriptor& vetoDescriptor );
    void handle_frame_ignored( Exo::File::Frame::Descriptor& ignoredDescriptor );
    void handle_control_begin( const Exo::File::Control::Begin::Descriptor& desc );
    void handle_control_end( const Exo::File::Control::End::Descriptor& desc );
    void handle_control_pause( const Exo::File::Control::Pause::Descriptor& desc );
    void handle_control_resume( const Exo::File::Control::Resume::Descriptor& desc );
    void handle_control_segment_begin( const Exo::File::Control::SegmentBegin::Descriptor& desc );
    void handle_control_segment_end( const Exo::File::Control::SegmentEnd::Descriptor& endDescriptor );
    void  OpenFile(const std::string& aFile);
    void  CloseCurrentFile();
    void fill_map_data(const EXOChannelMap& map);

    // Fill Generic record
    template<class TDoc, class TRecord>
    void FillRecord(TRecord& rec, const TDoc& realdoc) const;

    template<class TDoc, class TRecord>
    void FillIgnoredRecord(TRecord& rec, const TDoc& realdoc) const;
    // Fill Begin records
    template<class TDoc, class TRecord>
    void FillBeginRecord(TRecord& rec, const TDoc& realdoc) const;
   
    template<class TDoc, class TRecord>
    void FillBeginDatRecord(TRecord& rec, const TDoc& realdoc) const; 
   
    template<class TDoc, class TRecord>
    void FillBeginCalRecord(TRecord& rec, const TDoc& realdoc) const; 

    template<class TDoc, class TRecord>
    void FillEndRecord(TRecord& rec, const TDoc& realdoc) const; 

    template<class TDoc, class TRecord>
    void FillEndCalRecord(TRecord& rec, const TDoc& realdoc) const; 

    template<class TDoc, class TRecord>
    void FillPauseRecord(TRecord& rec, const TDoc& realdoc) const; 
    
    template<class TDoc, class TRecord>
    void FillResumeRecord(TRecord& rec, const TDoc& realdoc) const; 

    template<class TDoc, class TRecord>
    void FillSegmentBeginRecord(TRecord& rec, const TDoc& realdoc) const; 
    
    template<class TDoc, class TRecord>
    void FillSegmentEndRecord(TRecord& rec, const TDoc& realdoc) const; 
  public :

    EXOBinaryFileInputModule();
    virtual ~EXOBinaryFileInputModule();

    EXOEventData* GetNextEvent();
    int TalkTo(EXOTalkToManager *tm);
    bool reads_from_file() const {return true;}
    bool FileIsOpen() const;
    int ShutDown();
  
    int  get_run_number() { return fCurrentRunNumber; }
    int  get_event_number() { return fCurrentEventNumber; }
  
    void print_map_data();
    void SetSkipEvents(int aval) { fSkipEvents = aval; }
    
  
    DEFINE_EXO_ANALYSIS_MODULE( EXOBinaryFileInputModule )

};
#include "EXOUtilities/EXOPluginUtilities.hh"
EXO_DEFINE_PLUGIN_MODULE(EXOBinaryFileInputModule, "binput")
#endif
#endif
