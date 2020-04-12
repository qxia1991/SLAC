#ifndef EXOScintillationCluster_hh
#define EXOScintillationCluster_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOTClonesArray_hh
#include "EXOUtilities/EXOTClonesArray.hh"
#endif
#ifndef EXOTRefArray_hh
#include "EXOUtilities/EXOTRefArray.hh"
#endif
#ifndef EXOMiscUtil_hh
#include "EXOUtilities/EXOMiscUtil.hh"
#endif
class EXOChargeCluster;
#ifndef EXOAPDSignal_hh
#include "EXOUtilities/EXOAPDSignal.hh"
#endif
#include <cstddef> //for size_t

class EXOScintillationCluster : public TObject {
  public:

    Double_t       fX;                            //xsc        : X Position
    Double_t       fY;                            //ysc        : Y Position
    Double_t       fZ;                            //zsc        : Z Position
    Double_t       fTime;                         //tsc        : Time
    Double_t       fEnergy;                       //esc        : scintillation energy in keV
    Double_t       fEnergyError;                  // scintillation energy error in keV
    Double_t       fRawEnergy;                    //ssc        : Unscaled, position-corrected sum of counts
    Double_t       fWeightedAPDEnergy;            //           : Roughly in keV
    Double_t       fDenoisedEnergy;
    Double_t       fDenoisedEnergyError;
    Int_t          fDenoisingInternalCode;
    Double_t       fDNNVarRaw;                       // DNN-Variable from Tobias  (on Raw   WFs)
    Double_t       fDNNVarRecon;                     // DNN-Variable from Tobias  (on Recon WFs)
    Double_t       fDNNChargeEnergy;                 // DNN Reconstructed Charge Energy from Tobias 

    double GetRadius() const;
    double GetTheta() const;
    double GetEnergy() const;
    double GetEnergyError() const;

    size_t GetNumAPDSignals() const;
    void InsertAPDSignal(EXOAPDSignal *sig);
    EXOAPDSignal* GetAPDSignalAt(size_t index);
    const EXOAPDSignal* GetAPDSignalAt(size_t index) const;
    int FindAPDSignalLocation(EXOAPDSignal::EXOAPDSignal_type type, int pos = 0, int n = -1) const;

    EXOAPDSignal* GetAPDSignal(EXOAPDSignal::EXOAPDSignal_type type, int channel);
    const EXOAPDSignal* GetAPDSignal(EXOAPDSignal::EXOAPDSignal_type type, int channel) const;
    EXOAPDSignal* GetAPDSignal(const char *descr, int channel);
    const EXOAPDSignal* GetAPDSignal(const char *descr, int channel) const;
    EXOAPDSignal* GetGangSignal(int channel);
    const EXOAPDSignal* GetGangSignal(int channel) const;
    EXOAPDSignal* GetPlaneOneSignal();
    const EXOAPDSignal* GetPlaneOneSignal() const;
    EXOAPDSignal* GetPlaneTwoSignal();
    const EXOAPDSignal* GetPlaneTwoSignal() const;

    void AddAPDSignalsOfTypeToList(EXOAPDSignal::EXOAPDSignal_type type, TList& addToList) const;

    void SetRecommendedSignal(EXOAPDSignal *sig);
    EXOAPDSignal* GetRecommendedSignal();
    const EXOAPDSignal* GetRecommendedSignal() const;
    size_t GetNumGangs() const;

    void InsertChargeCluster( EXOChargeCluster* obj );
    EXOChargeCluster* GetChargeClusterAt(size_t index);
    const EXOChargeCluster* GetChargeClusterAt(size_t index) const;
    size_t GetNumChargeClusters() const;

    Int_t  GetAlgorithmUsed() const;   /*algsc : Scintillation cluster type. 0 = dual_apd_signal,\
                                                1 mono_apd_signal (obsolete), 2 = true t0 from simulation, 3 = no apd signal, 4 = TPC 1 mono, 5 = TPC 2 mono */ 

    Double_t GetCountsOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const;
    Double_t GetCountsSumOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const;
    Double_t GetCountsErrorOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const;
    Double_t GetCountsSumErrorOnAPDPlane(EXOMiscUtil::ETPCSide aplane) const;

  public:
    EXOScintillationCluster();
    EXOScintillationCluster(const EXOScintillationCluster&);
    EXOScintillationCluster& operator=(const EXOScintillationCluster&);
    virtual ~EXOScintillationCluster();
    virtual void Clear( Option_t* opt = "" ); 
    virtual void Print( Option_t* opt = "" ) const;

    bool operator==(const EXOScintillationCluster& sc) const;
    
  protected:

    EXOTRefArray  fRefChargeClusters;    //scint_ichargecluster
//
    EXOTRefArray  fRefAPDSignals;        // array of APD signals that are related to the cluster.
    TRef          fRefRecommendedSignal; // reference to 'recommended' signal.

    Double_t GetCountsOnAPDPlane(EXOMiscUtil::ETPCSide aplane, bool error) const;
    Double_t GetCountsSumOnAPDPlane(EXOMiscUtil::ETPCSide aplane, bool error) const;
  public:
    virtual void RecursiveRemove(TObject* obj); // Auto remove object from TRefArray

    ClassDef( EXOScintillationCluster, 12 )
}; 

//---- inlines -----------------------------------------------------------------

inline size_t EXOScintillationCluster::GetNumChargeClusters() const 
{ 
  // Get number of associated charge clusters.
  return fRefChargeClusters.GetLast() + 1; 
}

//______________________________________________________________________________
inline void EXOScintillationCluster::InsertAPDSignal(EXOAPDSignal *sig)
{
  if(fRefAPDSignals.IsEmpty()) fRefAPDSignals = EXOTRefArray(TProcessID::GetProcessWithUID(sig));
  fRefAPDSignals.Add(sig);
}

//______________________________________________________________________________
inline EXOAPDSignal* EXOScintillationCluster::GetAPDSignalAt(size_t index)
{
  // Get APD signal associated to this cluster at index i. 
  return static_cast<EXOAPDSignal*>(fRefAPDSignals.At(index));
}
//______________________________________________________________________________
inline const EXOAPDSignal* EXOScintillationCluster::GetAPDSignalAt(size_t index) const
{
  // Get APD signal associated to this cluster at index i. 
  return static_cast<const EXOAPDSignal*>(fRefAPDSignals.At(index));
}
//
inline size_t EXOScintillationCluster::GetNumAPDSignals() const
{
  // Get total number of associated signals
  return fRefAPDSignals.GetLast()+1;
}
inline EXOAPDSignal* EXOScintillationCluster::GetGangSignal(int channel) 
{
  // Get individual gang fit signal
  return GetAPDSignal(EXOAPDSignal::kGangFit,channel); 
}
inline const EXOAPDSignal* EXOScintillationCluster::GetGangSignal(int channel) const
{
  // Get individual gang fit signal
  return GetAPDSignal(EXOAPDSignal::kGangFit,channel); 
}
inline EXOAPDSignal* EXOScintillationCluster::GetPlaneOneSignal() 
{
  // Get plane one fit signal
  return GetAPDSignal(EXOAPDSignal::kPlaneFit,1);
}
inline const EXOAPDSignal* EXOScintillationCluster::GetPlaneOneSignal() const
{
  // Get plane one fit signal
  return GetAPDSignal(EXOAPDSignal::kPlaneFit,1);
}
inline EXOAPDSignal* EXOScintillationCluster::GetPlaneTwoSignal() 
{
  // Get plane two fit signal
  return GetAPDSignal(EXOAPDSignal::kPlaneFit,2);
}
inline const EXOAPDSignal* EXOScintillationCluster::GetPlaneTwoSignal() const
{
  // Get plane two fit signal
  return GetAPDSignal(EXOAPDSignal::kPlaneFit,2);
}

inline Int_t EXOScintillationCluster::GetAlgorithmUsed() const
{
  // Scintillation cluster type. 
  //    0 = dual_apd_signal, 
  //    1 = mono_apd_signal (obsolete), 
  //    2 = true t0 from simulation, 
  //    3 = no apd signal, 
  //    4 = TPC 1 mono,
  //    5 = TPC 2 mono 

  return (GetPlaneOneSignal()) ?
           ((GetPlaneTwoSignal()) ? 0:4 ) :
           ((GetPlaneTwoSignal()) ? 5:-1);
}
#endif /* EXOScintillationCluster_hh */
