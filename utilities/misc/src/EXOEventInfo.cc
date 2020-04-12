#include "EXOUtilities/EXOEventInfo.hh"
#include "EXOUtilities/EXODimensions.hh"

ClassImp( EXOEventInfo )

EXOEventInfo::EXOEventInfo()
: fChannelInfos(NULL)
{
  fChannelInfos = new EXOTClonesArray( EXOChannelInfo::Class(), NUMBER_READOUT_CHANNELS );
}

EXOEventInfo::~EXOEventInfo()
{
  if(fChannelInfos){
    delete fChannelInfos;
  }
}

void EXOEventInfo::Clear(Option_t*)
{
  fRunNumber = -999;
  fEventNumber = -999;
  if(fChannelInfos){
    fChannelInfos->Clear("C");
  }
}

EXOChannelInfo* EXOEventInfo::GetNewChannelInfo()
{
  return static_cast<EXOChannelInfo*> (GetChannelInfoArray()->GetNewOrCleanedObject( GetNumChannelInfos() ));
}

