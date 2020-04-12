#include "EXOUtilities/EXOControlRecord.hh"
#include <iostream>

ClassImp(EXOControlRecord)
ClassImp(EXOIgnoredFrameRecord)
ClassImp(EXOUnknownControlRecord)
ClassImp(EXOBeginRecord)
ClassImp(EXOPauseRecord)
ClassImp(EXOResumeRecord)
ClassImp(EXOEndRecord) 
ClassImp(EXOBeginDataRunRecord)
ClassImp(EXOBeginPhysicsRunRecord)
ClassImp(EXOPausePhysicsRunRecord)
ClassImp(EXOResumePhysicsRunRecord)
ClassImp(EXOEndPhysicsRunRecord)
ClassImp(EXOBeginSourceCalibrationRunRecord)
ClassImp(EXOPauseSourceCalibrationRunRecord)
ClassImp(EXOResumeSourceCalibrationRunRecord)
ClassImp(EXOEndSourceCalibrationRunRecord)
ClassImp(EXOBeginNoiseCalibrationRunRecord)
ClassImp(EXOPauseNoiseCalibrationRunRecord)
ClassImp(EXOResumeNoiseCalibrationRunRecord)
ClassImp(EXOEndNoiseCalibrationRunRecord)
ClassImp(EXOBeginCalibrationRunRecord)
ClassImp(EXOBeginSegmentCalibrationRecord)
ClassImp(EXOEndSegmentCalibrationRecord)
ClassImp(EXOEndCalibrationRecord)
ClassImp(EXOBeginInternalCalibrationRunRecord)
ClassImp(EXOEndInternalCalibrationRunRecord)
ClassImp(EXOBeginSegmentInternalCalibrationRunRecord)
ClassImp(EXOEndSegmentInternalCalibrationRunRecord)
ClassImp(EXOBeginExternalCalibrationRunRecord)
ClassImp(EXOEndExternalCalibrationRunRecord)
ClassImp(EXOBeginSegmentExternalCalibrationRunRecord)
ClassImp(EXOEndSegmentExternalCalibrationRunRecord)
ClassImp(EXOBeginCombinedCalibrationRunRecord)
ClassImp(EXOEndCombinedCalibrationRunRecord)
ClassImp(EXOBeginSegmentCombinedCalibrationRunRecord)
ClassImp(EXOEndSegmentCombinedCalibrationRunRecord)

//______________________________________________________________________________
std::string EXOControlRecord::GetStringNameOfType(EXOControlRecord::RecordType type)
{
  // Return a string name of the record type
  switch(type) {
     case kIgnoredFrame:   return "Ignored Frame";
     case kRecordUnknown:  return "Unknown";
     case kBegin:        return "Begin";
     case kPause:        return "Pause";
     case kResume:       return "Resume";
     case kEnd:          return "End";
     case kSegmentBegin: return "Segment Begin";
     case kSegmentEnd:   return "Segment End";
     default:            return "Unknown";
  }
}

//______________________________________________________________________________
std::string EXOControlRecord::GetStringNameOfFrameType(EXOControlRecord::FrameType type)
{
  // Return a string name of the record type
  switch(type) {
     case kFrameUnknown: return "Unknown";
     case kTpcData:      return "TPC Data";
     case kTpcEmpty:     return "TPC Empty";
     case kTpcDamaged:   return "TPC Damaged";
     case kVetoData:     return "Veto Data";
     case kVetoDamaged:  return "Veto Damaged";
     default:            return "Unknown";
  }
}
//______________________________________________________________________________
TTimeStamp EXOControlRecord::GetTimestampObj() const
{
  // Return a timestamp object 
  return TTimeStamp((time_t)(fTimestamp*1e-9), 
                    fTimestamp % 1000000000);
                      
}
//______________________________________________________________________________
void EXOControlRecord::Print(Option_t* opt) const
{
  std::cout << "--> Record Type: " << GetTypeString() 
            << std::endl 
            << "    In run: " << GetRunNumber() 
            << ", following event: " << GetPreviousEventNumber() 
            << std::endl
            << "    Time: " << GetTimestampObj().AsString(opt) << std::endl; 
}

//______________________________________________________________________________
std::string EXOBeginSourceCalibrationRunRecord::GetStringOfSourceType(
  EXOBeginSourceCalibrationRunRecord::SourceType type)
{
  switch(type) {
     case kCoWeak:   return "Weak Co"; 
     case kCoStrong: return "Strong Co";
     case kCsWeak:   return "Weak Cs";
     case kCsStrong: return "Strong Cs";
     case kThWeak:   return "Weak Th";
     case kThStrong: return "Strong Th";
     case kRaWeak:   return "Weak Ra";
     default:        return "Unknown";
  }

}

//______________________________________________________________________________
std::string EXOBeginSourceCalibrationRunRecord::GetStringOfSourcePosition(
  EXOBeginSourceCalibrationRunRecord::SourcePosition pos)
{
  switch(pos) {
     case kP2_nz:  return "P2_nz"; 
     case kP2_pz:  return "P2_pz";
     case kP4_ny:  return "P4_ny"; 
     case kP4_py:  return "P4_py";
     case kP4_px:  return "P4_px";
     default:      return "Unknown";
  }

}

//______________________________________________________________________________
std::string EXOBeginRecord::GetStringOfRunFlavorType(
  EXOBeginRecord::RunFlavor type)
{
  switch(type) {
     case kDatPhysics:  return "Data Physics"; 
     case kDatSrcClb:   return "Data Source Cal";
     case kDatNoise:    return "Data Noise";
     case kClbInt:      return "Calibration Internal";
     case kClbExt:      return "Calibration External";
     case kClbCombined: return "Calibration Combined";
     case kClbLaser:    return "Calibration Laser";
     default:           return "Unknown";
  }

}




//______________________________________________________________________________
void EXOBeginRecord::Print(Option_t* opt) const
{
  EXOControlRecord::Print(opt);
  std::cout << "----> Run Flavor: " << GetRunFlavorString() << std::endl; 
}

//______________________________________________________________________________
void EXOBeginSourceCalibrationRunRecord::Print(Option_t* opt) const
{
  EXOBeginRecord::Print(opt);
  std::cout << "----> Source Type: " << GetSourceTypeString() << std::endl; 
  std::cout << "----> Source Position: " << GetSourcePositionString() << std::endl; 
}

//______________________________________________________________________________
void EXOPauseRecord::Print(Option_t* opt) const
{
  EXOControlRecord::Print(opt);
  std::string prefix = "----> ";
  std::cout << prefix << "Summary info for Pause" << std::endl; 
  std::cout << prefix << "Frame types seen since last resume: " << std::endl; 
  for (int i=0;i<kNumberOfFrameTypes;i++) {
    std::cout << prefix << "  Number of " << GetStringNameOfFrameType((FrameType)i) << ": " << fNEvents[i] << std::endl;
  }
}

//______________________________________________________________________________
void EXOEndRecord::Print(Option_t* opt) const
{
  EXOControlRecord::Print(opt);
  std::string prefix = "----> ";
  std::cout << prefix << "Summary info for End" << std::endl; 
  std::cout << prefix << "Frame types seen since last resume: " << std::endl; 
  for (int i=0;i<kNumberOfFrameTypes;i++) {
    std::cout << prefix << "  Number of " << GetStringNameOfFrameType((FrameType)i) << ": " << fNEvents[i] << std::endl;
  }
}

//______________________________________________________________________________
void EXOEndSegmentCalibrationRecord::Print(Option_t* opt) const
{
  EXOControlRecord::Print(opt);
  std::string prefix = "----> ";
  std::cout << prefix << "Summary info for End segment calibration" << std::endl; 
  std::cout << prefix << "Frame types seen since in this segment: " << std::endl; 
  for (int i=0;i<kNumberOfFrameTypes;i++) {
    std::cout << prefix << "  Number of " << GetStringNameOfFrameType((FrameType)i) << ": " << fNEvents[i] << std::endl;
  }
}

//______________________________________________________________________________
void EXOIgnoredFrameRecord::Print(Option_t* opt) const
{
  EXOControlRecord::Print(opt);
  std::string prefix = "----> ";
  std::cout << prefix << "Ignored frame record, type: " 
            << GetStringNameOfFrameType(GetFrameType()) << std::endl; 
  if (GetFrameType() == kFrameUnknown) {
    std::cout << prefix << "Unknown frame type, time is NOT valid." 
              << std::endl; 
  }
}
