#ifndef EXOClusterCull_hh
#define EXOClusterCull_hh

#include "TVector3.h"
#include <list>
#include <set>
#include <string>

#include "EXOScintEventSummary.hh"

class EXOEventData;
class EXOMCPixelatedChargeDeposit;
class EXOChargeCluster;
class EXOScintillationCluster;
class TTree;
class TFile;
class TRandom3;

class EXOClusterCull 
{
public:

  EXOClusterCull();
  virtual ~EXOClusterCull();
  EXOScintEventSummary ClusterCull(const EXOScintillationCluster *sc, const EXOEventData* ED, bool oldDef3d=false) const;
  void SetChargeClusterThreshold(double thresh);
  bool HexagonalCulling(double x, double y, double z, double RHex) const;
  Bool_t RadialCulling(Double_t x, Double_t y, Double_t Rmax) const;
  void SetRadialCullingCut(double val) {fRadialCull = val;}
  void SetRadialCullingCutMC(double val) {fRadialCullMC = val;}
  void FillDebugTree(bool val = true);
  void SetDebugFilename(const std::string val) {fDebugFilename = val;}
  void WriteDebugTree();

  struct PCDVal {
    TVector3 position;
    double   energy;
  };
  typedef std::list<PCDVal> VecList;
  typedef std::set<const EXOMCPixelatedChargeDeposit*> PCDSet;
  typedef std::set<const EXOChargeCluster*> ChargeClusterSet;

  VecList GetListForEventData(const EXOEventData& ed) const;
  void PerformClusteringOnList(VecList& listToCluster,
                               double clusterDiameter) const;
  //void SetClusterAcceptanceRadius(double val) {fClusterAcceptanceRadius = val;}
  void SetExtraUChannelAcceptance(double val) {fExtraUChannelAcceptance = val;}
  void SetExtraVChannelAcceptance(double val) {fExtraVChannelAcceptance = val;}
  void SetMaxDriftSamples(UInt_t val) {fMaxDriftSamples = val;}  
  void SetMaxPCDTime(double val) {fMaxPCDTime = val;}
  double GetMaxPCDTime() {return fMaxPCDTime;}

protected:
  int SelectPCDs(const EXOEventData* ED, const std::set<const EXOChargeCluster*>& ccs, std::map<const EXOChargeCluster*, std::set<const EXOMCPixelatedChargeDeposit*> >& pcds, std::set<const EXOMCPixelatedChargeDeposit*> &nonAssociatedPcds) const;
  bool ChannelCompatible(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const;
  bool TimeCompatible(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const;
  double CalculateDistance(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd, int metric) const;
  double CalculateDistance(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd, TString dim) const;
  double CalculateUZDistance(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const;
  bool IsUdistanceAcceptable(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const;
  bool IsVdistanceAcceptable(const EXOChargeCluster& cc, const EXOMCPixelatedChargeDeposit& pcd) const;

private:

  double fChargeClusterThreshold;        // charge cluster threshold for PCD association
  double fRadialCull;                    // Radial culling cut
  double fRadialCullMC;                  // Radial culling cut for MC
  //double fClusterAcceptanceRadius;       // Charge cluster <--> PCD maximum allowed distance
  double fExtraUChannelAcceptance;        // Charge cluster <--> PCD extra num U channel widths acceptance
  double fExtraVChannelAcceptance;        // Charge cluster <--> PCD extra num V channel widths acceptance
  UInt_t fMaxDriftSamples; // Maximum allowed samples to consider CC belonging to the same "virtual" scintillation event
  double fMaxPCDTime; //Maximum allowed time between PCDs when attempting to assosiate PCDs to clusters

  std::string fDebugFilename;
  bool fFillDebugTree;
  TFile* fDebugFile;
  TTree* fDebugTree;
  mutable const EXOMCPixelatedChargeDeposit* fDebugPCD;
  mutable Double_t fDebugDistance;
  mutable Double_t fDebugMissingCCEnergy;
  mutable Int_t fDebugEventNumber;
  mutable Int_t fDebugRunNumber;
  mutable Bool_t fDebugSkippedByClustering;

  ClassDef(EXOClusterCull,1)             // Culls charge clusters associated to a given scintillation cluster
};

inline bool operator==(const EXOClusterCull::PCDVal& one, const EXOClusterCull::PCDVal& two)
{
  return one.position == two.position && one.energy == two.energy;
}

inline bool operator<(const EXOClusterCull::PCDVal& one, const EXOClusterCull::PCDVal& two)
{
  if (one.position != two.position) {
    if (one.position.x() != two.position.x()) {
      return one.position.x() < two.position.x();
    }
    if (one.position.y() != two.position.y()) {
      return one.position.y() < two.position.y();
    }
    if (one.position.z() != two.position.z()) {
      return one.position.z() < two.position.z();
    }
  }
  return one.energy < two.energy;
}
#endif
