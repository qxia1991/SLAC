#ifndef EXOEventInfo_hh
#define EXOEventInfo_hh

#include "EXOUtilities/EXOTClonesArray.hh"
#include "EXOUtilities/EXOChannelInfo.hh"
#include <cstddef> //for size_t

class EXOEventInfo: public TObject
{
  public:
    int fRunNumber;
    int fEventNumber;

  private:
    TClonesArray *fChannelInfos; //-> Channel Info array

  public:
    EXOEventInfo();
    ~EXOEventInfo();

    void Clear(Option_t* /*opt*/ = "");
    const EXOTClonesArray* GetChannelInfoArray() const;
    EXOTClonesArray* GetChannelInfoArray();
    EXOChannelInfo* GetNewChannelInfo();
    size_t GetNumChannelInfos() const;

  ClassDef( EXOEventInfo, 1)
};

inline const EXOTClonesArray* EXOEventInfo::GetChannelInfoArray() const
{
  return static_cast<const EXOTClonesArray* > (fChannelInfos);
}

inline EXOTClonesArray* EXOEventInfo::GetChannelInfoArray()
{
  return static_cast< EXOTClonesArray* > (fChannelInfos);
}

inline size_t EXOEventInfo::GetNumChannelInfos() const
{
  return (fChannelInfos) ? fChannelInfos->GetEntriesFast() : 0;
}

#endif
