#ifndef EXOAPDSignal_hh
#define EXOAPDSignal_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TRef
#include "TRef.h"
#endif
#include <string>

class EXOScintillationCluster;

class EXOAPDSignal : public TObject {
  public:
    enum EXOAPDSignal_type {
      kBad = -1,
      kGeneric,
      kGenericFit,
      kGenericSum,
      kGangFit,
      kPlaneFit,
      kFullFit,
      kGangSum,
      kPlaneSum,
      kFullSum,
      kCentroid
    };

    //EXOAPDSignal() : TObject() { EXOAPDSignal::Clear(); }
    //EXOAPDSignal(const EXOAPDSignal &other);
    //~EXOAPDSignal();
    EXOAPDSignal& operator= (const EXOAPDSignal &other);
    bool operator==(const EXOAPDSignal& other) const;

    void Print(Option_t *opt = "") const;
    void Clear(Option_t *opt = "");

    EXOAPDSignal_type GetType() const;
    const char* GetTypeName() const;
    double GetCounts() const;
    double GetEnergy() const;
    double GetError() const;

    const EXOScintillationCluster* GetScintCluster() const;
    EXOScintillationCluster* GetScintCluster();
    void SetScintCluster(EXOScintillationCluster *sc);
    
  public:
    Int_t        fType;                 // coded signal identification (EXOAPDSignal_type)
    Int_t        fChannel;              // sequence number of signal (gang number, plane number, ...)
    std::string  fDescr;                // signal identification ('gang_fit', 'plane_sum' or other)
    Double_t     fTime;                 // time of start
    Double_t     fRawCounts;            // not corrected counts
    Double_t     fCounts;               // gain corrected counts
    Double_t     fCorrectedCounts;      // counts corrected for everything
    Double_t     fCountsError;          // error on counts corrected for everything
    Double_t     fChiSquared;           // chi^2 for this fit (or something similar)

  protected:
    TRef         fRefScintCluster;      // cluster this signal belongs to.
  ClassDef( EXOAPDSignal, 2 )

};

//---- inlines -----------------------------------------------------------------

inline EXOAPDSignal::EXOAPDSignal_type EXOAPDSignal::GetType() const { 
  return (kGeneric<=fType && fType<=kCentroid)?(EXOAPDSignal_type)fType:kBad; }
inline double EXOAPDSignal::GetCounts() const {
  return (fCorrectedCounts!=0.)?fCorrectedCounts:((fCounts!=0.)?fCounts:fRawCounts); }

#endif /* EXOAPDSignal_hh */
