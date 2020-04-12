#ifndef EXOAddNoise_hh
#define EXOAddNoise_hh

#include "EXOUtilities/EXOVWaveformTransformer.hh"
#include <vector>
#include <cstddef> //for size_t
class EXOTransferFunction;

class EXOAddNoise : public EXOVWaveformTransformer
{

  public:
    EXOAddNoise() : EXOVWaveformTransformer("EXOAddNoise" ), 
      fNoiseMagnitude(1.0), fCacheLength(100) {}
 
    // This transform is optimized for an in-place transformation
    virtual bool IsInPlace() const { return true; }

    // Add an differentiation stage 
    void AddDiffStageWithTime(Double_t time)
      { fDiffStages.push_back(time); Reset(); }

    // Add an integration stage 
    void AddIntegStageWithTime(Double_t time)
      { fIntegStages.push_back(time); Reset(); }

    // Load differentiation and integration stages to match a given EXOTransferFunction object.
    void LoadTransferFunction(const EXOTransferFunction& transfer);
    
    void Clear()
      { ClearDiffStages(); ClearIntegStages();  Reset(); }

    size_t GetNumDiffStages() const { return fDiffStages.size(); }
    size_t GetNumIntegStages() const { return fIntegStages.size(); }

    Double_t GetDiffTime(size_t i)  const { return fDiffStages[i]; }
    Double_t GetIntegTime(size_t i) const { return fIntegStages[i]; }

    void SetNoiseMagnitude(double magn) { fNoiseMagnitude = magn; }
    void SetCacheLength(size_t cache); 
   
  protected: 
    virtual void TransformInPlace(EXODoubleWaveform& anInput) const;
    void ClearDiffStages()  { fDiffStages.clear(); }
    void ClearIntegStages() { fIntegStages.clear(); }
    void Reset() { fTimeWF.SetLength(0); }

    std::vector<Double_t> fDiffStages;
    std::vector<Double_t> fIntegStages;
    double fNoiseMagnitude;
    size_t fCacheLength; 
    mutable EXODoubleWaveform fTimeWF;
};

#endif
