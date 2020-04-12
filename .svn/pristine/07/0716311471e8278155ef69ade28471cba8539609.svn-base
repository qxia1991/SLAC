//______________________________________________________________________________
//
// EXOBinaryFileInputModule 
//
// Input module that reads in binary files from TPC Data
//
// Note:  the module is designed to handle veto and TPC data streams that are
// out of sync.  However, the firmware has recently been changed (7/8/2011) so
// that the records should be in sync to within ~1 us (contact Ryan Herbst),
// neglecting the 2,048 us duration of the TPC events.  Generously, I set
// fMaxStreamLagMicroSeconds to 10,000 us; if memory usage is an issue, this
// can be reduced.  Some cutoff is required so the module doesn't hunt through
// an entire run for veto records.
//
// The binary input module also provides access to the control records in the
// binary files.  Each record (deriving from EXOControlRecord) is stored in a
// list EXOControlRecordList.  This object is then shared using the shared
// object mechnanism for analysis modules, allowing analysis modules to have
// access to this object.  An example usage is below:
//
//   EXOControlRecordList* recordList = 
//     dynamic_cast<EXOControlRecordList*>(FindSharedObject("ControlRecords"));
//   if (!recordList) {
//     // handle the case if it's NULL, which means it couldn't be found.
//   } else {
//     // access the list. 
//   }
//
// For more information on how to access the record list, see the
// EXOControlRecordList documentation.

#include "EXOBinaryFileInputModule.hh"

#ifdef HAVE_EXOBIN
// Just disable everything else in case we don't have EXOBinary package
#include "EXOUtilities/EXOGlitchRecord.hh"
#include "EXOUtilities/EXOVetoEventHeader.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTPCVetoSummary.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOControlRecord.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"

#include "EXOBinaryPackage/File/Reader.hh"
#include "EXOBinaryPackage/File/Segment.hh"
#include "EXOBinaryPackage/File/Group/Descriptor.hh"
#include "EXOBinaryPackage/File/Frame/Descriptor.hh"
#include "EXOBinaryPackage/File/Control/Descriptor.hh"

#include "EXOBinaryPackage/Frame/Format/TpcWaveformEvent.hh"
#include "EXOBinaryPackage/Frame/Format/VetoBitlistEvent.hh"
#include "EXOBinaryPackage/Frame/TpcChannel.hh"

#include "EXOBinaryPackage/Control/Control.hh"

#include "TBits.h"
#include "TTree.h"
#include <iostream>
#include <sstream>
using namespace std;

//___________________________________
// Define EXOAnalysisReader -- this class permits notifications as the file is being read.

namespace Exo { namespace File {

class EXOAnalysisReader : public Reader
{
public:
  EXOAnalysisReader(const std::string file, int& status);
  void notify (NotifyReason reason);
  void notify (const Segment& segment, NotifyReason reason);
};

} } // end namespaces
//______________________________________________________________________________
EXOBinaryFileInputModule::EXOBinaryFileInputModule() :
  fLastControlRecord(kNoFile),
  fEventsProcessed(-1),
  fCurrentEventNumber(-1),
  fCurrentRunNumber(-1),
  fSkipEvents(0),
  binaryFile(NULL),
  fCurrentRunType(Exo::Run::Type::DatPhysics),
  fRunTypeKnown(false)
{

  // If this platform defines fundamental data types differently from the one that wrote the binary files,
  // then the binary files cannot be read -- terminate the program.
  // FixME! This should happen during a configuration process!  Also if the
  // size of a variable is critical *use* the defined width types, either ROOT
  // types, or those defined in the C standard. M. Marino  
  assert(sizeof(short) == 2 and sizeof(int) == 4 and sizeof(long long int) ==
8); // there may be others worth checking too.

  // Also FixME!  Should be dealt with at configuration!
  // Also worth checking that the system is little-endian -- terminate if you're on a big-endian platform.
  {unsigned int test = 1; char* bytewise = (char*) &test; assert(bytewise[0]);}

  // Share control records
  RegisterSharedObject("ControlRecords", fRecords); 

}

//______________________________________________________________________________
EXOBinaryFileInputModule::~EXOBinaryFileInputModule()
{
  // Delete memory in memory pools
  for(size_t i = 0; i < fEventDataMemoryPool.size(); i++) {
    delete fEventDataMemoryPool[i];
  }
  for(size_t i = 0; i < fVetoSummaryMemoryPool.size(); i++) {
    delete fVetoSummaryMemoryPool[i];
  }

  // If there are still queued objects, delete them too.
  // This can happen, for instance, when maxevents is set.
  for(size_t i = 0; i < fEventDataDeque.size(); i++) {
    delete fEventDataDeque[i];
  }
  for(size_t i = 0; i < NUM_VETO_CHANNELS; i++) {
    for(size_t j = 0; j < fVetoSummaryDequeArray[i].size(); j++) {
      delete fVetoSummaryDequeArray[i][j];
    }
  }

  // Delete the file reader.
  delete binaryFile;

  // Retract the control records
  RetractObject("ControlRecords");
}
//______________________________________________________________________________
void EXOBinaryFileInputModule::fill_map_data(const EXOChannelMap& channelMap)
{

  // For the moment we assume that all 18 DAQ cards are being used. 
  // EXOBinaryPackage hard-codes this in some places too, but we should still look into removing this hard-coded aspect
  // from EXOAnalysis.
  
  for ( int i = 0; i < 18; i++ ) {
    unsigned short word = 0;

    for ( int j = 15; j >= 0 ; j-- ) {
      word <<= 1; // shift everything left (higher-order) -- we're pushing the bits onto a stack.
      if(channelMap.get_physical_channel(i,j) > -1) word |= 1; // set this bit
    }

    map_data[i] = word;
  }
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::print_map_data()
{

  printf("*****************************************************************\n");
  printf("*****************************************************************\n");
  printf("Channel map being used to retrieve waveforms from binary package\n");
  printf("*****************************************************************\n");
  for ( int i = 0; i < 18; i++ ) printf("%d \t %x\n", i, map_data[i]);
  printf("*****************************************************************\n");
  printf("*****************************************************************\n");

}

//______________________________________________________________________________
void EXOBinaryFileInputModule::CloseCurrentFile()
{
  // Close Binary file if open; no-op if there is no file open.
  if(not binaryFile->is_open()) return;
  cout << "Closing binary file with name " << binaryFile->file_name() << "." << endl;
  UpdateControlStatus(kNoFile);
  if(binaryFile->close() != 0) {
    LogEXOMsg("Failed to close a binary file.", EEAlert);
  }
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::OpenFile(const std::string& filename)
{
  // Open binary file specified by filename 

  // Save the current information in case opening fails.
  Exo::File::EXOAnalysisReader* oldFile = binaryFile;

  int status;
  binaryFile = new Exo::File::EXOAnalysisReader(filename.c_str(), status);
  if(status != 0 or binaryFile == NULL or not binaryFile->is_open()) {
    // Opening failed -- restore the original file and throw.
    if(binaryFile) delete binaryFile;
    binaryFile = oldFile;
    std::ostringstream ostream;
    ostream << "Failed to open binary file " << filename << " with status " << status << ".";
    throw EXOMiscUtil::EXOBadCommand(ostream.str());
  }
  if(oldFile) delete oldFile; // We've successfully opened the new file, so delete the old one.

  fCurrentEventNumber = -1;
  fEventsProcessed = 0;
  UpdateControlStatus(kFileOpened);
  cout << "Successfully opened binary file with name " << binaryFile->file_name() << endl;
}

//______________________________________________________________________________
bool EXOBinaryFileInputModule::FileIsOpen() const
{
  return (binaryFile and binaryFile->is_open());
}

//______________________________________________________________________________
int EXOBinaryFileInputModule::TalkTo(EXOTalkToManager* tm)
{
  // Set up talk to commands
  tm->CreateCommand("/binput/skip","number of events to skip",
                    this, fSkipEvents, &EXOBinaryFileInputModule::SetSkipEvents);
  return 0;
}
//______________________________________________________________________________
EXOEventData* EXOBinaryFileInputModule::GetNextEvent()
{

  if (!binaryFile->is_open()) { 
      LogEXOMsg("No file opened", EEAlert);
      return NULL;
  }

  while(true) { // Keep skipping until we are able to return a complete TPC record.

    // First, attempt to flush out queued tpc objects.
    // If fLastControlRecord == kEndFile, force the flush.
    // (So, after the file ends, we just start flushing events.)
    if(EXOEventData* NextEvent = FlushEventDataDeque(fLastControlRecord == kEndFile)) return NextEvent;

    // If fLastControlRecord == kEndFile and all events have been flushed,
    // we should return NULL to signal that we're done.
    if(fLastControlRecord == kEndFile) return NULL;

    Exo::File::Group::Descriptor recordDescriptor;
    Exo::File::Group::Reader::Type recordType;
    recordType = binaryFile->next(recordDescriptor);
    if ( Exo::File::Group::Reader::is_eof(recordType) ) {
      // my understanding is that if you reach the end of a file segment, then the next function above will
      // automatically search for the next file segment based on filename.
      // Only upon failing this search and reaching the true end of file will this point be reached.
      cout << "Reached the end of the file after " << fEventsProcessed << " events." << endl;
      cout << "The queue will now be flushed." << endl;
      UpdateControlStatus(kEndFile);
      continue; // This will force the flush to be completed.
    }
    else if ( Exo::File::Group::Reader::is_invalid(recordType) ) {
      // All invalid types that are not EOF are unrecoverable.
      cout << "Encountered invalid record with Group::Reader::Type = " << int(recordType) << endl;
      LogEXOMsg("An invalid record (other than EOF) was encountered.", EEAlert);
    }
    else if ( Exo::File::Group::Reader::is_bad(recordType) ) {
      // Retry the process -- it could just be an overloaded disk.
      // Failing that, Tony J or JJ may be able to patch the binary file -- 
      // errors happen in the DAQ, but they often follow patterns, which helps in fixing them.
      if(fCurrentRunNumber == 2743 or fCurrentRunNumber == 2761 or
         fCurrentRunNumber == 2766 or fCurrentRunNumber == 3537 or
         fCurrentRunNumber == 5340) {
        // These runs just have a few bad records, and we need them for a paper.  So, don't abort processing.
        LogEXOMsg("A bad record was encountered -- either the file is corrupted, or I/O failed and should be repeated, or this run was ended in a very unclean fashion", EECritical);
      } else {
        LogEXOMsg("A bad record was encountered -- either the file is corrupted, or I/O failed and should be repeated, or this run was ended in a very unclean fashion", EEAlert);
      }
      continue;
    }
    else if ( recordType == Exo::File::Group::Reader::Unknown ) {
      // There is an unclassified problem with the record, but DAQ may rectify the data stream afterward -- recover.
      // Note that in most all situations, this occurs because an extra word has been inserted.
      // This causes the events to be out of alignment, so we don't receive them in a unified way.
      // It can probably be patched, but this sort of error won't recover.  Fine.
      LogEXOMsg("An unknown record was encountered -- a DAQ-level error, but might recover", EEError);
      continue;
    }
    else if ( recordType == Exo::File::Group::Reader::Management or recordType == Exo::File::Group::Reader::Meta ) {
      // These should only contain information of interest for online monitoring and EXOBinaryPackage.
      LogEXOMsg("Encountered Management or Meta record (used only by DAQ) -- skipping", EENotice);
      continue;
    }
    else if ( recordType != Exo::File::Group::Reader::Control and recordType != Exo::File::Group::Reader::Frame ) {
      // Encountered a record type which I'm not familiar with
      LogEXOMsg("Encountered a \"good\" record of a type not recognized in binput", EEWarning);
      continue;
    }

    // We now have either a Control or Frame record.
    if ( recordType == Exo::File::Group::Reader::Control) {
      // what kind of Control record?
      Exo::File::Control::Descriptor &controlDescriptor = recordDescriptor;
      Exo::File::Control::Type::Type controlType = controlDescriptor.type();
      switch(controlType) {
        case  Exo::File::Control::Type::Begin:
          handle_control_begin( (Exo::File::Control::Begin::Descriptor &) controlDescriptor );
          break;
        case  Exo::File::Control::Type::Pause:
          handle_control_pause( (Exo::File::Control::Pause::Descriptor &) controlDescriptor );
          break;
        case  Exo::File::Control::Type::Resume:
          handle_control_resume( (Exo::File::Control::Resume::Descriptor &) controlDescriptor );
          break;
        case  Exo::File::Control::Type::SegmentBegin:
          handle_control_segment_begin( (Exo::File::Control::SegmentBegin::Descriptor &) controlDescriptor );
          break;
        case  Exo::File::Control::Type::SegmentEnd:
          handle_control_segment_end( (Exo::File::Control::SegmentEnd::Descriptor &) controlDescriptor );
          break;
        case  Exo::File::Control::Type::End:
          handle_control_end ( (Exo::File::Control::End::Descriptor &) controlDescriptor );
          break; // We should soon encounter the EOF record; then remaining TPC events will be flushed from the queue.
        case  Exo::File::Control::Type::Unknown: {
          LogEXOMsg("Encountered unknown Control Record", EEWarning);
          EXOUnknownControlRecord rec;  
          rec.fPreviousEventNumber = fCurrentEventNumber;
          rec.fRunNumber           = fCurrentRunNumber;
          fRecords.Add(rec);
          break;
        }
        default:
          LogEXOMsg("Unrecognized control record, what should I do?", EEAlert);
          break;
      }
      continue;
    }

    // So we have a Frame record.
    if ( recordType != Exo::File::Group::Reader::Frame) {
      LogEXOMsg("A retrieved record is neither control record nor frame record", EEAlert);
    }

    // Check that we're in a state where a frame record is allowed.
    if(not fRunTypeKnown) {
      LogEXOMsg("Encountered a frame record outside of a run.", EEAlert);
    }
    else if(fCurrentRunType == Exo::Run::Type::DatPhysics or 
            fCurrentRunType == Exo::Run::Type::DatSrcClb  or 
            fCurrentRunType == Exo::Run::Type::DatNoise) {
      if(fLastControlRecord != kBeginRun and fLastControlRecord != kResume) {
        LogEXOMsg("Encountered a frame when the control records do not permit it", EEAlert);
      }
    }
    else if(fCurrentRunType == Exo::Run::Type::ClbInt or 
            fCurrentRunType == Exo::Run::Type::ClbExt or 
            fCurrentRunType == Exo::Run::Type::ClbCombined or
            fCurrentRunType == Exo::Run::Type::ClbLaser) {
      if(fLastControlRecord != kBeginSegment) {
        LogEXOMsg("Encountered a frame when the control records do not permit it", EEAlert);
      }
    }
    else {
      LogEXOMsg("We are in a run type I don't recognize -- need to update code", EEAlert);
    }

    Exo::File::Frame::Descriptor& frameDescriptor = recordDescriptor;
    if ( frameDescriptor.type() == Exo::File::Frame::Type::TpcData ) {
      if (fSkipEvents>0) {
          fSkipEvents--;
          continue; // we're skipping this TPC event
      }
      handle_frame_tpcdata(frameDescriptor);
      fEventsProcessed++;
      continue;
    }
    else if ( frameDescriptor.type() == Exo::File::Frame::Type::VetoData ) {
      handle_frame_vetodata(frameDescriptor);
      continue;
    }
    else {
      // the frame was damaged, etc.
      handle_frame_ignored(frameDescriptor);
      LogEXOMsg("Frame record returned by binary package was Damaged, Empty, or of Unknown type", EECritical);
      continue;
    }

    // How did we get to this point?
    LogEXOMsg("Reached the end of the while loop; this should never happen.", EEAlert);
  }

  // How did we get to this point?
  LogEXOMsg("Exited the while loop; this should never happen.  Quitting.", EEAlert);
  return NULL;
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::replace_cleared_data(EXOEventData* event)
{
  event->fRunNumber = fCurrentRunNumber;
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_frame_ignored( 
  Exo::File::Frame::Descriptor& ignoredDataDesc )
{
  // Handle ignored frames, we just take the time stamp (which is only valid
  // for non Unknown frame types), and position in the data stream.
  
  EXOIgnoredFrameRecord rec;
  rec.fFrameType = (EXOControlRecord::FrameType)ignoredDataDesc.type();
  rec.fPreviousEventNumber = fCurrentEventNumber;
  rec.fRunNumber           = fCurrentRunNumber;
  switch(ignoredDataDesc.type()) {
    case Exo::File::Frame::Type::Unknown: break; 
    case Exo::File::Frame::Type::TpcEmpty: 
      FillIgnoredRecord(rec, ignoredDataDesc.tpcEmpty().header().time());
      break;
    case Exo::File::Frame::Type::TpcDamaged: 
      FillIgnoredRecord(rec, ignoredDataDesc.tpcDamaged().header().time());
      break;
    case Exo::File::Frame::Type::VetoDamaged: 
      FillIgnoredRecord(rec, ignoredDataDesc.vetoDamaged().event());
      break;
    case Exo::File::Frame::Type::VetoData: 
      // It is possible to get veto data when something is wrong with it.
      FillIgnoredRecord(rec, ignoredDataDesc.vetoData().event());
      break;
    default:
      LogEXOMsg("Unknown ignored type!", EEAlert);
  };
  fRecords.Add(rec);
  
}
//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_frame_tpcdata( Exo::File::Frame::TpcData::Descriptor& TpcDataDescriptor )
{
  // Function to read in all information from an Exo::File::Frame::TpcData record.
  // As of 4/7/2011, all information is being read and saved EXCEPT:
  //       The number of electronics channels is not saved; we assume that the number of waveforms is more relevant.
  //       Only the software (physical) channel numbers of waveforms are saved, as converted by the channel map.
  //       We assume that waveforms have not been truncated for data compression purposes, and discard variables
  //         relevant to this situation.  If we begin doing this, documentation is embedded in this function.

  // Get EXOEventData object from memory pool.
  // Note:  we own this now!  Do not exit this function without putting it somewhere.
  EXOEventData* NewEventData = GetCleanEventData();

  // Replace the information that's only contained in control records (currently we only deal with run number).
  replace_cleared_data(NewEventData);

  // Retrieve the header of this record.
  Exo::Frame::Tpc::Base::Header& tpcHeader = TpcDataDescriptor.header();

  // Retrieve the EXOEventHeader object -- we'll write into this.
  EXOEventHeader& eventHeader = NewEventData->fEventHeader;

  // Event number
  eventHeader.fTriggerCount = tpcHeader.serial_number();
  NewEventData->fEventNumber = eventHeader.fTriggerCount;

  // Trigger Time
  eventHeader.fTriggerSeconds = tpcHeader.time().seconds();
  eventHeader.fTriggerMicroSeconds = tpcHeader.time().subseconds();
  eventHeader.fTriggerDrift = tpcHeader.time().drift();

  // Trigger information
  eventHeader.fSumTriggerThreshold = tpcHeader.trigger().sn();
  eventHeader.fSumTriggerRequest = tpcHeader.trigger().sr();
  eventHeader.fSumTriggerValue = tpcHeader.trigger().sv(); 
  eventHeader.fIndividualTriggerThreshold = tpcHeader.trigger().in();
  eventHeader.fIndividualTriggerRequest = tpcHeader.trigger().ir();
  eventHeader.fMaxValueChannel = tpcHeader.trigger().iv(); 
  eventHeader.fTriggerOffset = tpcHeader.trigger().offset();
  eventHeader.fTriggerSource = tpcHeader.trigger().source();

  // Number of samples
  NewEventData->GetWaveformData()->fNumSamples = tpcHeader.nsamples();
  eventHeader.fSampleCount = tpcHeader.nsamples() - 1; // fSampleCount is the index of the last sample.

  // Frame revision
  eventHeader.fFrameRevision = tpcHeader.version();

  // Card count and number of cards enabled
  eventHeader.fCardCount = tpcHeader.ncards();
  unsigned int CardsEnabled = tpcHeader.enabled();
  eventHeader.fCardsEnabled.Set( eventHeader.fCardCount, &CardsEnabled );

  // Get the binary package channel map (specifying which hardware channels to fetch).
  const EXOChannelMap& channelMap = GetChanMapForHeader(eventHeader);
  fill_map_data(channelMap);
  const Exo::Frame::Tpc::Channel::Map18 MyMap(map_data);

  // Create an Event object, allocating (on stack) just enough space to hold the requested hardware channel waveforms.
  size_t size32 = Exo::Frame::Tpc::Waveform::Event::size32(TpcDataDescriptor, MyMap);
  uint32_t buffer[size32];
  const Exo::Frame::Tpc::Waveform::Event *event = new (buffer) Exo::Frame::Tpc::Waveform::Event(TpcDataDescriptor, MyMap);

  // How many waveforms have been fetched in this event?
  int nwaveforms = event->nwaveforms();

  for ( int i = 0; i < nwaveforms; i++ ) {

    // Get the descriptor corresponding to waveform i (not ordered in any way).
    // Note that this is a copy; could probably be changed to a return by reference in binary package, but the
    // performance hit should be negligible.
    Exo::Frame::Tpc::Waveform::Descriptor waveformDescriptor = event->descriptor( i );

    /*_____________________________________________________________________________________

    In principle, if waveforms were compressed, then eg. the following could be an example situation:
               nsamples could be 2048, indicating that data was taken for 2048 samples.
               We might decide we only wish to store samples 500-1500 in memory.
               Thus, waveformDescriptor.first() would return 500.
               waveformDescriptor.last() would return 1500.
               waveformDescriptor.nadcs() would return last-first+1 = 1001.  (This is always redundant.)
               waveformDescriptors.adcs()[500] through waveformDescriptors.adcs()[1500] would contain the portion
                 of the waveform which was stored.  I'm unsure whether the rest would store junk or zeroes.

    For now, we assume that nadcs != nsample, first != 0, last != nadcs-1 are all errors.
    _________________________________________________________________________________________*/

    if ( (unsigned int)waveformDescriptor.nadcs() !=  tpcHeader.nsamples() or 
         waveformDescriptor.first() != 0 or waveformDescriptor.last() != waveformDescriptor.nadcs()-1 ) {
      LogEXOMsg("A waveform has been inappropriately truncated.", EEAlert);
      fEventDataMemoryPool.push_back(NewEventData); // The event is no good; recycle the memory.
      return;
    }

    // Create new event data waveform to hold data.
    EXOWaveform* dataWaveform = NewEventData->GetWaveformData()->GetNewWaveform(); 

    // Load waveform trace into dataWaveform.
    dataWaveform->SetData( waveformDescriptor.adcs(), waveformDescriptor.nadcs() ); 

    // Convert electronics channel number to physical channel number; only store physical channel number.
    int DAQcard = Exo::Frame::Tpc::Channel::card( waveformDescriptor.channel_number() );
    int card_channel = Exo::Frame::Tpc::Channel::card_channel( waveformDescriptor.channel_number() );
    dataWaveform->fChannel = channelMap.get_physical_channel( DAQcard, card_channel) ;

  } // end loop over waveforms

  // increment event counters
  fCurrentEventNumber = NewEventData->fEventNumber;

  // Update fVetoStreamMicroSeconds, if the TPC stream has clearly passed beyond the veto stream.
  Long64_t TPCtime = eventHeader.fTriggerSeconds;
  TPCtime *= 1000000; // Careful not to store 8-byte value in 4-byte intermediate result.
  TPCtime += eventHeader.fTriggerMicroSeconds;
  if(TPCtime - fMaxStreamLagMicroSeconds > fVetoStreamMicroSeconds) {
    fVetoStreamMicroSeconds = TPCtime - fMaxStreamLagMicroSeconds;
  }

  // Check that we're receiving the tpc events in a weakly time-ordered fashion.  (Probably something much stronger should be necessary, but oh well.)
  static Long64_t LastTPCStreamRecord = 0;
  if(TPCtime < LastTPCStreamRecord) {
    LogEXOMsg("The tpc records are being received from the binary file out-of-order.", EEAlert);
  }
  LastTPCStreamRecord = TPCtime;

  // Push NewEventData to the back of the queue.
  fEventDataDeque.push_back(NewEventData);
}
//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_frame_vetodata( Exo::File::Frame::VetoData::Descriptor& vetoDescriptor )
{
  // As of this writing (5/6/2011) the output is complete.
  //
  // Note that both glitch and muon veto records are handled here.

  const Exo::Frame::Veto::Bitlist::Event& vetoEvent = vetoDescriptor.event();
  const ULong64_t mask = vetoEvent.mask();

  // If mask is 0, then something happened.
  if(not mask) {
    LogEXOMsg("A veto record contains no active bits", EEError);
    handle_frame_ignored((Exo::File::Frame::Descriptor&)vetoDescriptor);
    return;
  }

  // We don't currently use bits 58, 62-63; nor do we use the odd-numbered bits 0-58.  (We do use 59, 60, 61 for glitches.)
  // If any of them fire, that's concerning.
  if(mask & 0xC6AAAAAAAAAAAAAAULL) {
    LogEXOMsg("A veto record had an unused bit filled.", EEError);
    handle_frame_ignored((Exo::File::Frame::Descriptor&)vetoDescriptor);
    // JJ suggests we shouldn't ignore these entirely, since a veto channel can get noisy.  Fine.
    // We'll write them as normal veto records, ignoring the bits that should not have been active.
    // return;
  }

  // Veto records can hold glitch bits, too; if it's both, then that's concerning.
  if(mask >> 59 and mask << 5) {
    // Obtain more information; Ralph has commented that knowing which channels fired may be useful.
    std::ostringstream aStream;
    aStream << "Both veto and glitch channels fired: Channels ";
    bool SeekingFirstChannel = true; // for formatting purposes.
    for(size_t i = 0; i < 64; i++) {
      if(mask & (1ULL << i) ) {
        if(SeekingFirstChannel) aStream << i;
        else aStream << ", " << i;
        SeekingFirstChannel = false;
      }
    }
    aStream << " fired.";
    LogEXOMsg(aStream.str(), EEError);

    // Ryan has requested that in this case, we write both glitch and veto events separately. (11/9/2011)
    // return;
  }

  // If it was the glitch detector firing, store that to the "glitch" branch
  if(mask >> 59) {
    EXOGlitchRecord ThisGlitch;
    ThisGlitch.fVersion = vetoEvent.version();
    ThisGlitch.fRunNumber = fCurrentRunNumber;
    ThisGlitch.fSerialNumber = vetoEvent.serial_number();
    ThisGlitch.fSeconds = vetoEvent.seconds();
    ThisGlitch.fMicroSeconds = vetoEvent.subseconds();
    ThisGlitch.fTimingError = vetoEvent.timing_error();

    // Parse mask which should occupy bits 59-61.
    if(mask & (1ULL << 59)) { ThisGlitch.fGlitchLevelOne = true; }
    if(mask & (1ULL << 60)) { ThisGlitch.fGlitchLevelTwo = true; }
    if(mask & (1ULL << 61)) { ThisGlitch.fGlitchLevelThree = true; }
    if( (ThisGlitch.fGlitchLevelTwo and not ThisGlitch.fGlitchLevelOne) or
        (ThisGlitch.fGlitchLevelThree and not ThisGlitch.fGlitchLevelTwo) or
        (ThisGlitch.fGlitchLevelThree and not ThisGlitch.fGlitchLevelOne) ) {
      // If a high-level glitch fires, all glitches lower than that should also fire.
      // However, it is a known hardware issue that this can occur.  Issue a warning and move on.
      LogEXOMsg("A higher-level glitch fired without lower-level glitches; this is a known hardware problem that should be studied soon (Clayton 10-27-2011).", EEWarning);
    }

    // Add the object to the glitch tree.
    AddGlitchRecord(&ThisGlitch);
  } // done with glitch record

  if(mask << 5) {
    // If we reach this point, it must be a muon veto event.
    EXOVetoEventHeader vetoEvents;

    // Fill a veto event, and add it to the tree.  
    vetoEvents.fVetoVersion = vetoEvent.version();
    vetoEvents.fVetoRunNumber = fCurrentRunNumber;
    vetoEvents.fVetoEventNumber = vetoEvent.serial_number();
    vetoEvents.fVetoUTime = vetoEvent.seconds();
    vetoEvents.fVetoMuTime = vetoEvent.subseconds();
    vetoEvents.fVetoTime = vetoEvent.seconds()+(Double_t)vetoEvent.subseconds()/1.0e6;
    vetoEvents.fVetoTimeError = vetoEvent.timing_error();
    vetoEvents.fIntegrityError = vetoEvent.integrity_error();
    vetoEvents.fVetoPanelHit.Set(64, &mask);
    AddVetoRecord(&vetoEvents);

    // Also, need to add VetoSummary objects to the fVetoSummaryDequeArray, so that TPC events can know something about vetos.
    for(size_t i = 0; i < NUM_VETO_CHANNELS; i++) {
      if(mask & (1ULL << (2*i)) ) { // If bit 2i of mask is active
        VetoSummary* NewVetoSummary = GetCleanVetoSummary();
        NewVetoSummary->fMicroSeconds = vetoEvent.seconds();
        NewVetoSummary->fMicroSeconds *= 1000000; // Being careful to avoid too-large results in intermediate values.
        NewVetoSummary->fMicroSeconds += vetoEvent.subseconds();
        NewVetoSummary->fMask.Set(64, &mask);
        fVetoSummaryDequeArray[i].push_back(NewVetoSummary);
      }
    }
  } // done with veto record

  // Update fVetoStreamMicroSeconds.
  fVetoStreamMicroSeconds = vetoEvent.seconds();
  fVetoStreamMicroSeconds *= 1000000; // Being careful to avoid too-large results in intermediate values.
  fVetoStreamMicroSeconds += vetoEvent.subseconds();

  // Check that we're receiving the veto records in a (weakly) time-ordered fashion.  This bug has actually happened!
  static Long64_t TimeOfLastVetoStreamRecord = 0;
  if(TimeOfLastVetoStreamRecord > fVetoStreamMicroSeconds) {
    LogEXOMsg("Veto records were received out-of-order from the binary file", EEAlert);
  }
  TimeOfLastVetoStreamRecord = fVetoStreamMicroSeconds;
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillRecord(TRecord& rec, const TDoc& realdoc) const
{
  rec.fPreviousEventNumber = fCurrentEventNumber;
  rec.fTimestamp           = realdoc.timestamp();
  rec.fRunNumber           = fCurrentRunNumber;
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillIgnoredRecord(TRecord& rec, const TDoc& realdoc) const
{
  // Converts to nano-seconds
  rec.fTimestamp           = realdoc.useconds()*1000;
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillBeginRecord(TRecord& rec, const TDoc& realdoc) const
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  rec.fOutputFormat = realdoc.output_format();
  rec.fMaxRecsize   = realdoc.max_recsize();
  rec.fCardSet      = realdoc.card_set();
} 

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillBeginDatRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillBeginRecord<TDoc, TRecord>(rec, realdoc);
  rec.fKeepAlive = realdoc.keepalive(); 
  memcpy(&rec.fSuppress, &realdoc.suppress_set(), sizeof(rec.fSuppress));
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillBeginCalRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillBeginRecord<TDoc, TRecord>(rec, realdoc);
  rec.fNSegments    = realdoc.nsegments(); 
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillEndRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  memcpy(&rec.fNEvents, &realdoc.nevents(), 
    sizeof(rec.fNEvents[0])*Exo::Frame::Type::Count);
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillEndCalRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillEndRecord<TDoc, TRecord>(rec, realdoc);
  rec.fNSegments    = realdoc.nsegments(); 
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillPauseRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  memcpy(&rec.fNEvents, &realdoc.nevents(), 
    sizeof(rec.fNEvents[0])*Exo::Frame::Type::Count);
  rec.fIdentifier    = realdoc.identifier(); 
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillResumeRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  rec.fIdentifier    = realdoc.identifier(); 
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillSegmentBeginRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  rec.fIdentifier = realdoc.identifier(); 
  rec.fNEvents    = realdoc.nevents(); 
  rec.fInterval   = realdoc.interval(); 
  rec.fDac        = realdoc.dac(); 
  rec.fDelay      = realdoc.delay(); 
  memcpy(&rec.fSuppress, &realdoc.suppress_set(), sizeof(rec.fSuppress));
  memcpy(&rec.fEnabled, &realdoc.enabled_set(), sizeof(rec.fEnabled));
}

//______________________________________________________________________________
template<class TDoc, class TRecord>
void EXOBinaryFileInputModule::FillSegmentEndRecord(TRecord& rec, const TDoc& realdoc) const  
{
  FillRecord<TDoc, TRecord>(rec, realdoc);
  rec.fIdentifier    = realdoc.identifier(); 
  memcpy(&rec.fNEvents, &realdoc.nevents(), 
    sizeof(rec.fNEvents[0])*Exo::Frame::Type::Count);
}

#define CASE_CONTROL_INSIDE(atype, subtype, RecordType, ClassType) \
      Exo::Control::atype::subtype::RecordType realdoc =    \
        (const Exo::File::Control::atype::subtype::RecordType::Descriptor &) descriptor; \
      ClassType rec; 

#define CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, ClassType, RecordType, RTExtra) \
    case atype ## subtype: {                                      \
      CASE_CONTROL_INSIDE(atype, subtype, RecordType, ClassType)  \
      Fill ## RecordType ## RTExtra ## Record(rec, realdoc);      \
      fRecords.Add(rec);                                          \
      break;                                                      \
    }
 
#define CASE_CONTROL_BEGIN_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, Begin, )


// Pause/Resume
#define CASE_CONTROL_PAUSE_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, Pause, )
#define CASE_CONTROL_RESUME_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, Resume, )

// Begin/End Segment
#define CASE_CONTROL_BEGIN_SEG_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, SegmentBegin, )
#define CASE_CONTROL_END_SEG_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, SegmentEnd, )

// End records
#define CASE_CONTROL_END_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, End, )
#define CASE_CONTROL_END_CAL_RECORD(atype, subtype, RecordType) \
  CASE_CONTROL_HANDLE_SIMPLE_RECORD(atype, subtype, RecordType, End, Cal)


//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_begin( 
  const Exo::File::Control::Begin::Descriptor& descriptor )
{
  // Handle Begin control records, saves and adds to the fRecords records list.
  using namespace Exo::Run::Type;
  Exo::Control::Begin begin = descriptor;
  fCurrentRunType   = descriptor.run_type();
  fRunTypeKnown = true;
  fCurrentRunNumber = begin.run_number();

  switch (fCurrentRunType) {
    case DatPhysics: {
      CASE_CONTROL_INSIDE(Dat, Physics, Begin, EXOBeginPhysicsRunRecord)
      FillBeginDatRecord(rec, realdoc);
      fRecords.Add(rec);
      break; 
    }
    case DatSrcClb: {
      CASE_CONTROL_INSIDE(Dat, SrcClb, Begin, EXOBeginSourceCalibrationRunRecord)
      FillBeginDatRecord(rec, realdoc);
      rec.fSourceType     = realdoc.source_type();
      rec.fSourcePosition = realdoc.source_position();
      rec.fReserved       = realdoc.reserved();
      fRecords.Add(rec);
      break; 
    }
    case DatNoise: {
      CASE_CONTROL_INSIDE(Dat, Noise, Begin, EXOBeginNoiseCalibrationRunRecord)
      FillBeginDatRecord(rec, realdoc);
      fRecords.Add(rec);
      break; 
    }
    case ClbInt: {
      CASE_CONTROL_INSIDE(Clb, Int, Begin, EXOBeginInternalCalibrationRunRecord)
      FillBeginCalRecord(rec, realdoc);
      fRecords.Add(rec);
      break; 
    }
    case ClbExt: {
      CASE_CONTROL_INSIDE(Clb, Ext, Begin, EXOBeginExternalCalibrationRunRecord)
      FillBeginCalRecord(rec, realdoc);
      rec.fSerialNumber = realdoc.serial_number();
      rec.fCardNumber   = realdoc.card_number();
      fRecords.Add(rec);
      break; 
    }
    case ClbCombined: {
      CASE_CONTROL_INSIDE(Clb, Combined, Begin, EXOBeginCombinedCalibrationRunRecord)
      FillBeginCalRecord(rec, realdoc);
      rec.fSerialNumber = realdoc.serial_number();
      rec.fCardNumber   = realdoc.card_number();
      fRecords.Add(rec);
      break; 
    }
    case ClbLaser: {
      CASE_CONTROL_INSIDE(Clb, Laser, Begin, EXOBeginLaserCalibrationRunRecord)
      FillBeginCalRecord(rec, realdoc);
      fRecords.Add(rec);

      break;
    }
  }
  UpdateControlStatus(kBeginRun);
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_pause( 
  const Exo::File::Control::Pause::Descriptor& descriptor )
{
  using namespace Exo::Run::Type;

  switch (fCurrentRunType) {
    CASE_CONTROL_PAUSE_RECORD(Dat, Physics,  EXOPausePhysicsRunRecord)
    CASE_CONTROL_PAUSE_RECORD(Dat, SrcClb,   EXOPauseSourceCalibrationRunRecord)
    CASE_CONTROL_PAUSE_RECORD(Dat, Noise,    EXOPauseNoiseCalibrationRunRecord)
    case ClbInt:     
    case ClbExt: 
    case ClbCombined: 
    case ClbLaser: 
      LogEXOMsg("Pause records not allowed during a calibration run.", EEAlert);
      break;

  }

  UpdateControlStatus(kPause);
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_resume( 
  const Exo::File::Control::Resume::Descriptor& descriptor )
{
  using namespace Exo::Run::Type;

  switch (fCurrentRunType) {
    CASE_CONTROL_RESUME_RECORD(Dat, Physics,  EXOResumePhysicsRunRecord)
    CASE_CONTROL_RESUME_RECORD(Dat, SrcClb,   EXOResumeSourceCalibrationRunRecord)
    CASE_CONTROL_RESUME_RECORD(Dat, Noise,    EXOResumeNoiseCalibrationRunRecord)

    case ClbInt:     
    case ClbExt: 
    case ClbCombined: 
    case ClbLaser: 
      LogEXOMsg("Resume records not allowed during a calibration run.", EEAlert);
      break;

  }

  UpdateControlStatus(kResume);
}


//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_end( 
  const Exo::File::Control::End::Descriptor& descriptor )
{
  using namespace Exo::Run::Type;

  fRunTypeKnown = false;

  switch (fCurrentRunType) {
    CASE_CONTROL_END_RECORD(Dat, Physics,  EXOEndPhysicsRunRecord)
    CASE_CONTROL_END_RECORD(Dat, SrcClb,   EXOEndSourceCalibrationRunRecord)
    CASE_CONTROL_END_RECORD(Dat, Noise,    EXOEndNoiseCalibrationRunRecord)
    CASE_CONTROL_END_CAL_RECORD(Clb, Int,      EXOEndInternalCalibrationRunRecord)
    CASE_CONTROL_END_CAL_RECORD(Clb, Ext,      EXOEndExternalCalibrationRunRecord)
    CASE_CONTROL_END_CAL_RECORD(Clb, Combined, EXOEndCombinedCalibrationRunRecord)
    CASE_CONTROL_END_CAL_RECORD(Clb, Laser,    EXOEndLaserCalibrationRunRecord)
  }

  UpdateControlStatus(kEndRun);
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_segment_begin( 
  const Exo::File::Control::SegmentBegin::Descriptor& descriptor )
{
  using namespace Exo::Run::Type;

  switch (fCurrentRunType) {
    case DatPhysics:      
    case DatSrcClb: 
    case DatNoise:     
      LogEXOMsg("Segment begin records not allowed during a data run.", EEAlert);
      break;
    CASE_CONTROL_BEGIN_SEG_RECORD(Clb, Int,      EXOBeginSegmentInternalCalibrationRunRecord)
    CASE_CONTROL_BEGIN_SEG_RECORD(Clb, Ext,      EXOBeginSegmentExternalCalibrationRunRecord)
    CASE_CONTROL_BEGIN_SEG_RECORD(Clb, Combined, EXOBeginSegmentCombinedCalibrationRunRecord)
    case ClbLaser: {
      CASE_CONTROL_INSIDE(Clb, Laser, SegmentBegin, EXOBeginSegmentLaserCalibrationRunRecord)
      FillRecord(rec, realdoc);
      rec.fIdentifier = realdoc.identifier(); 
      rec.fNEvents    = realdoc.nevents(); 
      rec.fRate    = realdoc.rate(); 
      memcpy(&rec.fSuppress, &realdoc.suppress_set(), sizeof(rec.fSuppress));
      rec.fPattern = realdoc.pattern();
      memcpy(&rec.fConditionsNorth.fPulse, &realdoc.conditions_north().pulse(),
             sizeof(rec.fConditionsNorth.fPulse));
      memcpy(&rec.fConditionsNorth.fHV, &realdoc.conditions_north().hv(),
             sizeof(rec.fConditionsNorth.fHV));
      memcpy(&rec.fConditionsSouth.fPulse, &realdoc.conditions_south().pulse(),
             sizeof(rec.fConditionsSouth.fPulse));
      memcpy(&rec.fConditionsSouth.fHV, &realdoc.conditions_south().hv(),
             sizeof(rec.fConditionsSouth.fHV));
      fRecords.Add(rec);
    }
  }
  UpdateControlStatus(kBeginSegment);
}

//______________________________________________________________________________
void EXOBinaryFileInputModule::handle_control_segment_end( 
  const Exo::File::Control::SegmentEnd::Descriptor& descriptor )
{
  using namespace Exo::Run::Type;

  switch (fCurrentRunType) {
    case DatPhysics:      
    case DatSrcClb: 
    case DatNoise:     
      LogEXOMsg("Segment end records not allowed during a data run.", EEAlert);
      break;
    CASE_CONTROL_END_SEG_RECORD(Clb, Int,      EXOEndSegmentInternalCalibrationRunRecord)
    CASE_CONTROL_END_SEG_RECORD(Clb, Ext,      EXOEndSegmentExternalCalibrationRunRecord)
    CASE_CONTROL_END_SEG_RECORD(Clb, Combined, EXOEndSegmentCombinedCalibrationRunRecord)
    CASE_CONTROL_END_SEG_RECORD(Clb, Laser,    EXOEndSegmentLaserCalibrationRunRecord)

  }

  UpdateControlStatus(kEndSegment);
}

//______________________________________________________________________________
EXOEventData* EXOBinaryFileInputModule::GetCleanEventData()
{
  // Retrieve an EXOEventData object from the memory pool -- it will be cleaned up here.
  // If there are no more, create one.
  // Note that this transfers ownership of the object to whoever is receiving the object.

  if(fEventDataMemoryPool.empty()) {
    // Need to create a new one.
    return new EXOEventData;
  }
  else {
    // Clean up an old EXOEventData.
    EXOEventData* CleanedEvent = fEventDataMemoryPool.back();
    fEventDataMemoryPool.pop_back();
    CleanedEvent->Clear();
    return CleanedEvent;
  }
}
//______________________________________________________________________________
EXOBinaryFileInputModule::VetoSummary* EXOBinaryFileInputModule::GetCleanVetoSummary()
{
  // Retrieve a VetoSummary object from the memory pool -- it will be cleaned up here.
  // If there are no more, create one.
  // Note that this transfers ownership of the object to whoever is receiving the object.

  if(fVetoSummaryMemoryPool.empty()) {
    return new VetoSummary;
  }
  else {
    VetoSummary* CleanedVetoSummary = fVetoSummaryMemoryPool.back();
    fVetoSummaryMemoryPool.pop_back();
    CleanedVetoSummary->Clear();
    return CleanedVetoSummary;
  }
}
//______________________________________________________________________________
EXOEventData* EXOBinaryFileInputModule::FlushEventDataDeque(bool force /* = false */)
{
  // If we have read far enough into the veto stream to associate TPC events with the correct veto events,
  // do the linking, return the pointer to the next EXOEventData object, and return the pointer to the memory pool
  // (so that when we return to this module, we can assume that everything in the memory pool has been used).
  // If we cannot safely link the next TPC event, return NULL.
  // When force is true, we assume that regardless of veto read-ahead, the remaining tpc events should be linked
  // (ie. at the end of the run).
  //
  // Regardless, clean up the stored VetoSummary objects and return them to the VetoSummary memory pool.

  // If we don't have any tpc events yet, then there's nothing we can do.
  if(fEventDataDeque.empty()) return NULL;

  // Retrieve the unix time of the tpc event (trigger time), in microseconds.
  Long64_t tpcTriggerTime = fEventDataDeque.front()->fEventHeader.fTriggerSeconds;
  tpcTriggerTime *= 1000000; // Careful not to let an intermediate result overload a 4-byte value.
  tpcTriggerTime += fEventDataDeque.front()->fEventHeader.fTriggerMicroSeconds;
  Long64_t CoincidenceCutoff = tpcTriggerTime + fCoincidenceCutMicroSeconds; // veto events up to and including this time can be associated.

  // Spring cleaning: drop all veto events except one, for each veto channel, that occur prior to CoincidenceCutoff.
  // Return dropped veto objects to the memory pool.
  for(size_t i = 0; i < NUM_VETO_CHANNELS; i++) {
    while(fVetoSummaryDequeArray[i].size() > 1 and fVetoSummaryDequeArray[i][1]->fMicroSeconds <= CoincidenceCutoff) {
      fVetoSummaryMemoryPool.push_back(fVetoSummaryDequeArray[i].front());
      fVetoSummaryDequeArray[i].pop_front();
    }
  }

  // If the veto stream hasn't been read far enough, and we're not forcing it to finish, return NULL.
  if(fVetoStreamMicroSeconds < CoincidenceCutoff and not force) return NULL;

  // OK, so we are able to write in the veto stuff, and return a complete tpc event.  Do so.
  EXOEventHeader& eventHeader = fEventDataDeque.front()->fEventHeader;
  for(size_t i = 0; i < NUM_VETO_CHANNELS; i++) {
    if((not fVetoSummaryDequeArray[i].empty()) and fVetoSummaryDequeArray[i].front()->fMicroSeconds <= CoincidenceCutoff) {
      // This channel is OK to write into the TPC event.
      EXOTPCVetoSummary* NewVeto = eventHeader.AddVetoChannel();
      NewVeto->fChannel = 2*i;
      NewVeto->fMask = fVetoSummaryDequeArray[i].front()->fMask;
      NewVeto->fMicroSecondsBeforeTPCTrigger = tpcTriggerTime - fVetoSummaryDequeArray[i].front()->fMicroSeconds;
    }
  }

  // Put the tpc event in the memory pool without clearing it -- that way, when we return, it'll be ready to go.
  fEventDataMemoryPool.push_back(fEventDataDeque.front());
  fEventDataDeque.pop_front();

  // The tpc event is ready to go.
  return fEventDataMemoryPool.back();

}
//______________________________________________________________________________
void EXOBinaryFileInputModule::UpdateControlStatus(const EControlStatus NewStatus)
{
  // Update the current status of the reading; this lets the program detect errors associated with the flow of data.
  // Note that these restrictions may be overly strict.  (Can we pause during a calibration run? Can we end a run just anywhere?)
  // But if one of those corner cases occurs, it should be checked; then a change to this condition can be introduced.
  // It is only here as a safety check, after all.
  //
  // We require the following:
  // FileOpened must follow NoFile.
  // BeginRun must follow either a FileOpened or an EndRun.
  // BeginSegment/EndSegment must come as a consecutive pair, following a begin run or another segment.
  // Pause/Resume must come as a consecutive pair, following a BeginRun or another pause.
  // EndRun can only occur from within a run.
  // EndFile can occur only after a run has been ended.
  // NoFile can occur at any time (since the file can be closed at any time).

  if((NewStatus == kFileOpened and not (fLastControlRecord == kNoFile))                                       or
     (NewStatus == kBeginRun   and not (fLastControlRecord == kFileOpened or fLastControlRecord == kEndRun)) or
     (NewStatus == kBeginSegment and not (fLastControlRecord == kBeginRun or fLastControlRecord == kEndSegment)) or
     (NewStatus == kEndSegment and not (fLastControlRecord == kBeginSegment))                                or
     (NewStatus == kPause      and not  (fLastControlRecord == kBeginRun or fLastControlRecord == kResume))  or
     (NewStatus == kResume     and not  (fLastControlRecord == kPause))                                        or
     (NewStatus == kEndRun     and (fLastControlRecord == kNoFile  or fLastControlRecord == kEndRun))  or
     (NewStatus == kEndFile    and not (fLastControlRecord == kEndRun))) {
    std::cout << "Run status is " << int(fLastControlRecord) << "; "
              << "trying to switch to status " << int(NewStatus) << ", which is invalid." << std::endl;
    LogEXOMsg("Control records were ordered in an unrecognized way", EEAlert);
  }

  fLastControlRecord = NewStatus;
}
//______________________________________________________________________________
int EXOBinaryFileInputModule::ShutDown()
{
  // Build a veto and glitch tree index (by time), before the file gets written to disk.
  TTree* vetoTree = dynamic_cast<TTree*>(FindSharedObject(EXOMiscUtil::GetVetoTreeName()));
  if(vetoTree and vetoTree->GetEntries()) {
    Int_t ret = vetoTree->BuildIndex("fVetoUTime", "fVetoMuTime");
    if(ret < 0) LogEXOMsg("Veto tree index could not be built -- why?", EEAlert);
  }
  TTree* glitchTree = dynamic_cast<TTree*>(FindSharedObject(EXOMiscUtil::GetGlitchTreeName()));
  if(glitchTree and glitchTree->GetEntries()) {
    Int_t ret = glitchTree->BuildIndex("fSeconds", "fMicroSeconds");
    if(ret < 0) LogEXOMsg("Glitch tree index could not be built -- why?", EEAlert);
  }
  return 0;
}
//______________________________________________________________________________
// Implement EXOBinaryFileInputModule::VetoSummary.
EXOBinaryFileInputModule::VetoSummary::VetoSummary()
: fMask(64)
{
  fMicroSeconds = 0;
}

void EXOBinaryFileInputModule::VetoSummary::Clear()
{
  fMask.ResetAllBits();
  fMicroSeconds = 0;
}
//______________________________________________________________________________
namespace Exo {
namespace File {

// Necessary because of rules for inheritance of constructors.
EXOAnalysisReader::EXOAnalysisReader(const std::string file, int& status)
: Reader(file.c_str(), status)
{}

// Replace the virtual function in the binary package with one that outputs notifications to the screen.
void EXOAnalysisReader::notify(NotifyReason reason)
{
  if (reason == Open) {
    cout<<"EXOBinaryFileInputModule: Opening first file:  "<<file_name()<<endl;
  }
  if (reason == Close) {
    cout<<"EXOBinaryFileInputModule: Closing file "<<file_name()<<endl;
  }
  if (reason == ReadForward) {
    cout<<"EXOBinaryFileInputModule: Continuing to file "<<file_name()<<endl;
  }
  if (reason == ReadReverse) {
    cout<<"EXOBinaryFileInputModule: Reading back to file "<<file_name()<<endl;
  }
}

// Replace the virtual function in the binary package with one that outputs notifications to the screen.
void EXOAnalysisReader::notify(const Segment& segment, NotifyReason reason)
{
  notify(reason);
  // Yes, the segment contains information too, but it is mostly redundant.
  // This function is provided in response to an icc warning, so that both forms of notify are explicitly redefined.
}

} // namespace File
} // namespace Exo
#endif /* HAVE_EXOBIN */
