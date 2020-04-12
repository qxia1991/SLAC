//______________________________________________________________________________
//
// EXOElectronics
// 
//   Class to encapsulate information on the EXO electronics. Written by C.
//   Davis, pulled over into EXOUtilities by M. Marino 
// 
#include "EXOUtilities/EXOElectronics.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <cassert>
using EXOMiscUtil::TypeOfChannel;
using EXOMiscUtil::ChannelIsAPD;
using EXOMiscUtil::ChannelIsWire;

//______________________________________________________________________________
EXOElectronics::EXOElectronics() :
  fUseAPDNoiseCounts(false),
  fUseWireNoiseCounts(false),
  fAPDNoiseCounts(0.0),
  fWireNoiseCounts(0.0)
{
}
//______________________________________________________________________________
bool EXOElectronics::HasTransferFunctionForChannel(size_t channel) const
{
  // Return true if the TransferFunction for channel has been created.
  if(m_ElectronicsShapersMap.find(channel) != m_ElectronicsShapersMap.end()) return true;
  else return false;
}

//______________________________________________________________________________
bool EXOElectronics::HasNoiseTransformForChannel(size_t channel) const
{
  // Return true if the noise transform for channel has been created.
  if(m_ElectronicsNoiseMap.find(channel) != m_ElectronicsNoiseMap.end()) return true;
  ShaperMap::const_iterator Iter = m_ElectronicsShapersMap.find(channel);
  if(Iter != m_ElectronicsShapersMap.end()) {
    // Temporarily do this
    m_ElectronicsNoiseMap[channel].LoadTransferFunction(Iter->second); 
    return true;
  }
  else return false;
}

//______________________________________________________________________________
const EXOTransferFunction& EXOElectronics::GetTransferFunctionForChannel(size_t channel) const
{
  // Return the EXOTransferFunction object corresponding to channel.  If that
  // object is not in the map, the code will (rightly) crash -- the user should
  // have checked first.  Note that the reference is owned by this
  // EXOElectronicsShapersCalib object.
  ShaperMap::const_iterator Iter = m_ElectronicsShapersMap.find(channel);
  assert(Iter != m_ElectronicsShapersMap.end());
  return Iter->second;
}
//______________________________________________________________________________
const EXOVWaveformTransformer& EXOElectronics::GetNoiseTransformForChannel(size_t channel) const
{
  // Return the Noise transform object corresponding to channel.  For
  // consistency, this function has the same behaviour as
  // GetTransferFunctionForChannel and will crash if channel doesn't exist.  
  //
  // A temporary feature is that one can set globally the noise amplitude for
  // APD and Wires via the access functions SetNoiseAmplitudeForAPDs and
  // SetNoiseAmplitudeForWires.  This should go away as the noise information
  // is also stored in the database.
  NoiseTransformMap::iterator Iter = m_ElectronicsNoiseMap.find(channel);
  assert(Iter != m_ElectronicsNoiseMap.end());
  EXOAddNoise& noise = Iter->second; 
  if (fUseWireNoiseCounts and 
      ChannelIsWire(TypeOfChannel(channel))) {
    noise.SetNoiseMagnitude(fWireNoiseCounts);
  }
  if (fUseAPDNoiseCounts and 
      ChannelIsAPD(TypeOfChannel(channel))) {
    noise.SetNoiseMagnitude(fAPDNoiseCounts);
  }

  return noise;
}

void EXOElectronics::SetTransferFunctionForChannel(size_t channel, EXOTransferFunction &function)
{
  // Set an EXOTransferFunction object for a specific channel. If that channel
  // already has an associated transfer function object, this object is overwritten

  m_ElectronicsShapersMap[channel] = function;
}

void EXOElectronics::SetNoiseTransformForChannel(size_t channel, EXOAddNoise &transform)
{
  // Set an EXOAddNoise object for a specific channel. If that channel
  // already has an associated noise transform object, this object is overwritten

  m_ElectronicsNoiseMap[channel] = transform;
}

//______________________________________________________________________________
void EXOElectronics::SetNoiseAmplitudeForAPDs(double noise) 
{
  // Set and force the noise amplitude to be used for APDs
  fAPDNoiseCounts = noise;
  fUseAPDNoiseCounts = true;
}
//______________________________________________________________________________
void EXOElectronics::SetNoiseAmplitudeForWires(double noise) 
{
  // Set and force the noise amplitude to be used for Wires
  fWireNoiseCounts = noise;
  fUseWireNoiseCounts = true;
}
