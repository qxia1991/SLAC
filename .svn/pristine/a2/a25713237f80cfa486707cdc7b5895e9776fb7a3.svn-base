#ifndef EXOScintEventSummary_hh
#define EXOScintEventSummary_hh

#include <set>
#include <map>
#include <string>

class EXOScintillationCluster;
class EXOChargeCluster;
class EXOMCPixelatedChargeDeposit;
class EXOEventHeader;

class EXOScintEventSummary
{
  friend class EXOClusterCull;
  
  public:
    typedef std::set<const EXOChargeCluster*> CCSet;
    typedef std::set<const EXOMCPixelatedChargeDeposit*> PCDSet;
    typedef std::map<const EXOChargeCluster*, PCDSet> PCDMap;
    typedef std::pair<const EXOChargeCluster*, PCDSet> PCDPair;
    EXOScintEventSummary();

    void AssertInitialization(bool requireSC = true) const;

    double GetReconstructedChargeEnergy() const;
    double GetReconstructedMixChargeEnergy() const;
    double GetChargeInductionEnergy() const;
    double GetEnergyOfAssociatedPCDs() const;
    double GetEnergyOfNonAssociatedPCDs() const;
    double GetStandoff() const;
    double GetMaxRadius() const;
    double GetMinZ() const;
    double GetMaxZ() const;
    int GetNumSite() const;
    int GetNumCollWires() const;
    int GetNumIndWires() const;
    int GetNumVWires() const;
    double GetMaxInd() const;
    double GetFrac3DRecon() const;
    bool IsFiducial() const;
    bool HasMissingPositions(bool oldCCDef3d=false) const;
    bool WasSomethingCulled() const;
    bool PassesDiagonalCut() const;
    double GetSimulatedEnergyForCluster(const EXOChargeCluster* cc) const;

    const CCSet& GetGoodChargeClusters() const;
    const CCSet& GetCulledChargeClusters() const;
    const PCDMap& GetAssociatedPCDs() const;
    const PCDSet& GetNonAssociatedPCDs() const;
    static void SetDiagonalCutDBFlavor(const std::string flavor);
    static void SetFiducialVolumeDBFlavor(const std::string flavor);

  void SetPCDMetric(int metric){fPCDMetric = metric;};
  int GetPCDMetric(){return fPCDMetric;};
  void SetMultipleChargeOnlyEvents(){fMultipleChargeOnlyEvents = true;};
  bool HasMultipleChargeOnlyEvents() const {return fMultipleChargeOnlyEvents;};

  private:
    static std::string fDiagonalCutDBFlavor; //!   database flavor for diagonal cut. Don't serialize
    static std::string fFiducialCutDBFlavor; //!   database flavor for diagonal cut. Don't serialize
    const EXOScintillationCluster* fScintCluster;
    const EXOEventHeader* fEventHeader;
    CCSet fChargeClusters;
    CCSet fCulledChargeClusters;
    PCDMap fChargeDeposits;
    PCDSet fNonAssociatedChargeDeposits;
  int fPCDMetric;
  bool fMultipleChargeOnlyEvents;
};

inline const EXOScintEventSummary::CCSet& EXOScintEventSummary::GetGoodChargeClusters() const
{
  return fChargeClusters;
}

inline const EXOScintEventSummary::CCSet& EXOScintEventSummary::GetCulledChargeClusters() const
{
  return fCulledChargeClusters;
}

inline const EXOScintEventSummary::PCDMap& EXOScintEventSummary::GetAssociatedPCDs() const
{
  return fChargeDeposits;
}

inline const EXOScintEventSummary::PCDSet& EXOScintEventSummary::GetNonAssociatedPCDs() const
{
  return fNonAssociatedChargeDeposits;
}

#endif
