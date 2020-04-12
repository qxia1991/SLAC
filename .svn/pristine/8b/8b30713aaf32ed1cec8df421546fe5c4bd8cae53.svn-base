#include "EXOUtilities/EXOCorrelationCollection.hh"
#include "EXOUtilities/EXOErrorLogger.hh" 
#include <algorithm>

using namespace std;

ClassImp( EXOCorrelationCollection )

EXOCorrelationCollection::EXOCorrelationCollection()
{

}

EXOWaveformFT& EXOCorrelationCollection::GetCorrelation(int channel1, int channel2)
{
  int lower = min(channel1,channel2);
  int upper = max(channel1,channel2);
  pair<int,int> key = make_pair(lower,upper);
  return fMap[key];
}

int EXOCorrelationCollection::GetMaxChannel() const
{
  ChannelMap::const_iterator iter = fMap.begin();
  if(iter == fMap.end()){
    return 0;
  }
  int chan = max(iter->first.first,iter->first.second);
  for(iter = ++(fMap.begin()); iter!=fMap.end(); iter++){
    int temp = max(iter->first.first,iter->first.second);
    chan = max(temp,chan);
  }
  return chan;
}

int EXOCorrelationCollection::GetMinChannel() const
{
  ChannelMap::const_iterator iter = fMap.begin();
  if(iter == fMap.end()){
    return 0;
  }
  int chan = min(iter->first.first,iter->first.second);
  for(iter = ++(fMap.begin()); iter!=fMap.end(); iter++){
    int temp = min(iter->first.first,iter->first.second);
    chan = min(temp,chan);
  }
  return chan;
}

void EXOCorrelationCollection::Normalize(double divisor)
{
  for(ChannelMap::iterator iter = fMap.begin(); iter!= fMap.end(); iter++){
    iter->second /= divisor;
  }
}
