#ifndef EXOTransferFunction_hh
#define EXOTransferFunction_hh

#include "EXOUtilities/EXOVWaveformTransformer.hh"
#include "EXOUtilities/EXORCDifferentiator.hh"
#include "EXOUtilities/EXORCIntegrator.hh"
#include <vector>
#include <map>
#include <cstddef> //for size_t

class EXOTransferFunction : public EXOVWaveformTransformer
{

  public:
    EXOTransferFunction();
 
    // This transform is optimized for an in-place transformation
    virtual bool IsInPlace() const { return true; }

    Double_t GetGain(EXODoubleWaveform wf = EXODoubleWaveform() ) const;
    EXODoubleWaveform GetNoiseSpectrum(size_t Length = 2048) const;

    // Add an differentiation stage 
    void AddDiffStageWithTime(Double_t time);

    // Add an integration stage 
    void AddIntegStageWithTime(Double_t time);

    void Clear()
      { ClearDiffStages(); ClearIntegStages();}

    void ClearDiffStages()  { fDiffStages.clear(); fCachedNoiseSpectra.clear(); }
    void ClearIntegStages() { fIntegStages.clear(); fCachedNoiseSpectra.clear(); }

    virtual size_t GetNumDiffStages() const { return fDiffStages.size(); }
    virtual size_t GetNumIntegStages() const { return fIntegStages.size(); }

    virtual Double_t GetDiffTime(size_t i)  const { return fDiffStages[i]; }
    virtual Double_t GetIntegTime(size_t i) const { return fIntegStages[i]; }

    virtual bool operator==(const EXOTransferFunction& other) const;
    virtual bool operator!=(const EXOTransferFunction& other) const;

    Double_t GetEffectiveIntegTime() const;
    Double_t GetEffectiveDiffTime() const;

  protected: 
    typedef std::vector<Double_t> DblVec;
    virtual void TransformInPlace(EXODoubleWaveform& anInput) const;

  private:
    DblVec fDiffStages;
    DblVec fIntegStages;
    mutable EXORCDifferentiator fDiff;
    mutable EXORCIntegrator fInteg;
    mutable EXODoubleWaveform fScratch;
    mutable std::map<size_t, EXODoubleWaveform> fCachedNoiseSpectra; // Temporary fix (cgd, 9/2/2011)

};

#endif
