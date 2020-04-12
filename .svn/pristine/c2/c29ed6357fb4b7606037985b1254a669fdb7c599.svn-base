//______________________________________________________________________________
// EXOTimingStatisticInfo
//
// An object to hold timing and generic statistics information.  
//______________________________________________________________________________

#include "EXOUtilities/EXOTimingStatisticInfo.hh"
#include "EXOUtilities/EXOTClonesArray.hh"

ClassImp( EXOStopwatch )
ClassImp( EXOTimingStatisticInfo )

//______________________________________________________________________________
void EXOStopwatch::Print(Option_t* opt) const
{
  // Print the object
  Printf("Tag Name: %s ",fWatchName.Data());
  TStopwatch::Print(opt);
}

//______________________________________________________________________________
EXOTimingStatisticInfo::EXOTimingStatisticInfo() :
  TNamed()
{
  // Constructor of timing info
  fTimingInfo = new EXOTClonesArray( EXOStopwatch::Class(), 10 ); 
  fStatInfo = new TClonesArray( TParameter<Double_t>::Class(), 10 ); 
}

//______________________________________________________________________________
EXOTimingStatisticInfo::EXOTimingStatisticInfo(const EXOTimingStatisticInfo& obj) : TNamed(obj)
{
  // Copy constructor of timing info
  fTimingInfo = new EXOTClonesArray( *obj.fTimingInfo );  
  fStatInfo = new TClonesArray( *obj.fStatInfo );  
}

//______________________________________________________________________________
EXOTimingStatisticInfo& EXOTimingStatisticInfo::operator=(const EXOTimingStatisticInfo& obj)
{
  // Copy operator
  if (this == &obj) return *this;
  TNamed::operator=(obj);
  *static_cast<EXOTClonesArray*>(fTimingInfo) = *obj.fTimingInfo;  
  *fStatInfo = *obj.fStatInfo;  
  return *this;
}

//______________________________________________________________________________
EXOTimingStatisticInfo::~EXOTimingStatisticInfo()
{
  delete fTimingInfo;
  delete fStatInfo;
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::Clear( Option_t* )
{
  // Clear all timing information, deletes current stop watches.
  TObject::Clear();
  fTimingInfo->Clear("C");

  // We have to delete stat info fully 
  fStatInfo->Delete();
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::Reset()
{
  // Reset timing info, does not delete current stopwatches.
  TIter iter(fTimingInfo);
  EXOStopwatch* watch;
  while ((watch = (EXOStopwatch*)iter.Next())) watch->Reset();
  // We have to delete stat info fully 
  fStatInfo->Delete();
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::StartTimerForTag(const std::string & tag, bool reset /* = true */)
{
  // Start timer for a tag.
  // If you use reset = false, be sure you've reset the timer where appropriate.
  GetOrCreateTimerForTag(tag).Start(reset);
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::StopTimerForTag(const std::string & tag)
{
  // Stop timer for a tag.
  GetOrCreateTimerForTag(tag).Stop();
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::ResetTimerForTag(const std::string & tag)
{
  // Reset timer for a tag.
  GetOrCreateTimerForTag(tag).Reset();
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::Print(Option_t* opt) const
{
  // Print the timing and statistics information
  TIter iter(fTimingInfo);
  EXOStopwatch* watch;
  while ((watch = (EXOStopwatch*)iter.Next())) watch->Print(opt);

  TIter iterStat(fStatInfo);
  TParameter<Double_t>* par;
  while ((par = (TParameter<Double_t>*)iterStat.Next())) par->Print(opt);
}

//______________________________________________________________________________
EXOStopwatch* EXOTimingStatisticInfo::FindTimerForTag(const std::string& obj) const
{
  // Find a time or return NULL when it can't be found 
  TIter iter(fTimingInfo);
  EXOStopwatch* watch;
  while ((watch = (EXOStopwatch*)iter.Next())) {
    if ( watch->GetName() == obj ) return watch;
  } 
  return NULL;
}

//______________________________________________________________________________
EXOStopwatch& EXOTimingStatisticInfo::GetOrCreateTimerForTag(const std::string& obj)
{
  // Either get an existing stopwatch or create a new one with a tag.
  // Gauranteed to return an existing object.
  EXOStopwatch* watch = FindTimerForTag(obj);
  if (watch) return *watch;
  watch = static_cast<EXOStopwatch*>(
    static_cast<EXOTClonesArray*>(fTimingInfo)->
      GetNewOrCleanedObject( fTimingInfo->GetEntriesFast() ) );
  watch->SetName(obj);
  return *watch;
}

//______________________________________________________________________________
TParameter<Double_t>* EXOTimingStatisticInfo::FindStatisticForTag(const std::string& obj) const
{
  // Find a statistic or return NULL when it can't be found 
  TIter iter(fStatInfo);
  TParameter<Double_t>* par;
  while ((par = (TParameter<Double_t>*)iter.Next())) {
    if ( par->GetName() == obj ) return par;
  } 
  return NULL;
}

//______________________________________________________________________________
TParameter<Double_t>& EXOTimingStatisticInfo::GetOrCreateStatisticForTag(const std::string& obj)
{
  // Either get an existing statistic or create a new one with a tag.
  // Gauranteed to return an existing object.
  TParameter<Double_t>* par = FindStatisticForTag(obj);
  if (par) return *par;
  par = new((*fStatInfo)[fStatInfo->GetEntriesFast()]) TParameter<Double_t>(obj.c_str(), 0.0); 
  return *par;
}

//______________________________________________________________________________
void EXOTimingStatisticInfo::SetStatisticForTag(const std::string& tag, double aVal)
{
  // Set statistic for a particular tag
  GetOrCreateStatisticForTag(tag).SetVal(aVal);
}

//______________________________________________________________________________
double EXOTimingStatisticInfo::GetStatisticForTag(const std::string& tag) const
{
  // Set statistic for a particular tag
  TParameter<Double_t>* par = FindStatisticForTag(tag);
  if (par) return par->GetVal();
  return 0.0; 
}
