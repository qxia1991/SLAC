#ifndef EXOChannelMapManager_hh
#define EXOChannelMapManager_hh

#include "EXOUtilities/EXOChannelMap.hh"

class EXOEventHeader;
class EXOTimestamp;
class EXOChannelMapCalib;
class EXOChannelMapManager {

  protected:

    EXOChannelMap* fChannelMap;
    bool fChanMapIsOwned;

  public:
    static EXOChannelMapManager& GetChannelMapMgr()
    {
      static EXOChannelMapManager gfChanMgr;
      return gfChanMgr;
    }

    const EXOChannelMap& GetChannelMapWithHeader(const EXOEventHeader& eh);
    //const EXOChannelMap& GetChannelAtTime(const EXOTimestamp& eh);

  protected:
    EXOChannelMapManager() : fChanMapIsOwned(true) {fChannelMap = new EXOChannelMap;}
    EXOChannelMapManager(const EXOChannelMapManager&);
    EXOChannelMapManager& operator=(const EXOChannelMapManager&);
    ~EXOChannelMapManager() {if(fChanMapIsOwned) delete fChannelMap;}
    void  create_map_for_version_0(EXOChannelMap& channelMap);
    void  create_map_for_version_1();

    void MakeMeTheOwner()
    {
      // If we don't already own the channel map, make a new one (so we aren't changing a channel map we don't own).
      if(not fChanMapIsOwned) {
        fChannelMap = new EXOChannelMap;
        fChanMapIsOwned = true;
      }
    }
  
};

#define GetChanMapForHeader(header) \
EXOChannelMapManager::GetChannelMapMgr().GetChannelMapWithHeader(header)

#endif
