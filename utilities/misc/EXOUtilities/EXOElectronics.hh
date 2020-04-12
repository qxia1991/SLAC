#ifndef EXOElectronics_hh
#define EXOElectronics_hh

#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOAddNoise.hh"
#include <map>
#include <cstddef> //for size_t

class EXOElectronics {

  protected:
  
    typedef std::map<size_t, EXOTransferFunction> ShaperMap;
    typedef std::map<size_t, EXOAddNoise> NoiseTransformMap;

    ShaperMap         m_ElectronicsShapersMap;
    mutable NoiseTransformMap m_ElectronicsNoiseMap;

    bool fUseAPDNoiseCounts; 
    bool fUseWireNoiseCounts; 
    double fAPDNoiseCounts;
    double fWireNoiseCounts;
  
  public:
    const EXOTransferFunction& GetTransferFunctionForChannel(size_t channel) const;
    void SetTransferFunctionForChannel(size_t channel, EXOTransferFunction &function);
    bool HasTransferFunctionForChannel(size_t channel) const;
   
    const EXOVWaveformTransformer& GetNoiseTransformForChannel(size_t channel) const; 
    void SetNoiseTransformForChannel(size_t channel, EXOAddNoise &transform); 
    bool HasNoiseTransformForChannel(size_t channel) const; 

    void SetNoiseAmplitudeForAPDs(double noiseampl);
    void SetNoiseAmplitudeForWires(double noiseampl);

    EXOElectronics();
};
#endif
