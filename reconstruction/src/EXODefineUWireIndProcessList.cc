//______________________________________________________________________________
// 
// EXODefineUWireIndProcessList add u-wire induction signals as negative
// channels in order to fit these with induction signal templates.  For more
// information, see  GetProcessList().
//______________________________________________________________________________

#include "EXOReconstruction/EXODefineUWireIndProcessList.hh"

using namespace std;

Int_t EXODefineUWireIndProcessList::fChannelToAdd=0;
EXODefineUWireIndProcessList::signalList_t  EXODefineUWireIndProcessList::fCandidateSignalList;
EXODefineUWireIndProcessList::waveformMap_t EXODefineUWireIndProcessList::fWaveformMap;

void EXODefineUWireIndProcessList::ResetCandidateSignalList(){
  EXODefineUWireIndProcessList::fCandidateSignalList.resize(0);
}
void EXODefineUWireIndProcessList::AddCandidateSignal(const Int_t channel,const double time){
  EXODefineUWireIndProcessList::fCandidateSignalList.push_back(make_pair(channel,time));
}

//______________________________________________________________________________
EXOReconProcessList EXODefineUWireIndProcessList::StaticGetProcessList(const EXOReconProcessList& data)
{
  // Add all U wire waveforms as an additional entry in the process list, but
  // changing their channel to a unique negative number.  This allows processing
  // the same waveform with a different signal model with minimal changes to the
  // reconstruction framework.
  //
  // Ensures const'ness because this is called by a const function that
  // satisfies EXOVDefineProcessList.

  EXOReconProcessList ret;
  
  if (data.GetSize() == 0) return ret;

  data.ResetIterator();
  const EXOWaveform *wfptr = data.GetNextWaveform();

  data.ResetIterator();
  const EXOReconProcessList::WaveformWithType *wfWithType = NULL;
  while((wfWithType = data.GetNextWaveformAndType()) != NULL){
    const EXOWaveform *wf = wfWithType->fWf;

    //The following preserves const'ness.
    if(wf->fChannel != EXODefineUWireIndProcessList::GetChannelToAdd()) continue;

    //Only look at U wires.
    if(wf->GetChannelType() != EXOMiscUtil::kUWire)  continue;
    if(wfWithType->fType    != EXOReconUtil::kUWire) continue;

    Int_t channel=static_cast<Int_t>(EXOReconUtil::kUWireIndOffset) - wf->fChannel;

    //fWaveformMap (returned by GetWaveformMap) should be mutable such that it
    //can be modified by the following code anyway...so don't worry about
    //violating EXODefineUWireIndProcessList::GetProcessList const'ness.
    EXOWaveform &indWF = EXODefineUWireIndProcessList::GetWaveformMap()[channel];
    indWF.MakeSimilarTo(*wf);
    indWF.Zero();
    indWF.SetData(wf->GetData(),wf->GetLength());
    indWF.fChannel = channel;
    ret.Add(indWF,EXOReconUtil::kUWireInd);
    break;
  }// end of loop over waveforms

  return ret;
}

//______________________________________________________________________________
EXOReconProcessList EXODefineUWireIndProcessList::GetProcessList(const EXOReconProcessList &data) const
{
  // Only to appease EXOVDefineProcessList const requirement. Calls non-const
  // static member function GetProcessListStatic...which preserves const'ness.

  return EXODefineUWireIndProcessList::StaticGetProcessList(data);
}
