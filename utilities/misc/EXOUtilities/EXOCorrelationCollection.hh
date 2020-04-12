#ifndef EXOCorrelationCollection_hh
#define EXOCorrelationCollection_hh

#include "EXOUtilities/EXOWaveformFT.hh"
#include "TObject.h"

#include <map>
#include <cstddef>

class EXOCorrelationCollection: public TObject
{
  typedef std::map < std::pair<int,int>, EXOWaveformFT > ChannelMap;
  public:
    EXOCorrelationCollection();
    int GetMaxChannel() const;
    int GetMinChannel() const;
    EXOWaveformFT& GetCorrelation(int channel1, int channel2);
    EXOWaveformFT& operator()(int channel1, int channel2) {return GetCorrelation(channel1,channel2);}
    void Normalize(double divisor);
    
  protected:
    ChannelMap fMap;

  ClassDef( EXOCorrelationCollection, 1 )
};

#endif
