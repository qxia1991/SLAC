#ifndef EXOChannelInfo_hh
#define EXOChannelInfo_hh

#include "TObject.h"

struct EXOChannelInfo: public TObject
{
  int fChannel;
  int fLength;
  double fBaseline;
  double fNoiseCounts;
  void Clear(Option_t*){
    fChannel = -999;
    fLength = -999;
    fBaseline = -999;
    fNoiseCounts = -999;
  }
  ClassDef( EXOChannelInfo, 1)
};

#endif
