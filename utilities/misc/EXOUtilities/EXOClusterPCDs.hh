#ifndef EXOClusterPCDs_hh
#define EXOClusterPCDs_hh

#include "EXOUtilities/EXOPCDCluster.hh"
#include "EXOMatrix.hh"
#include "TObject.h"
#include <vector>
#include <set>

class EXOMonteCarloData;

class EXOClusterPCDs : public TObject
{
  public:
    typedef std::set< std::set<const EXOMCPixelatedChargeDeposit*> > SetOfSets;

    EXOClusterPCDs() : fRootCluster(NULL){;}
    EXOClusterPCDs(const EXOMonteCarloData &mcd);
    SetOfSets GetClustersForDistance(double dist) const;
    std::vector<double> GetEnergiesForDistance(double dist) const;

  private:

    double CalculateDistance(const EXOPCDCluster &c1, const EXOPCDCluster &c2) const;
    void DoClustering();
    void Collect(SetOfSets& collection, double dist, const EXOPCDCluster* c) const;

    EXOMatrix<double> fDistances;         //! do not write to file
    std::vector<EXOPCDCluster> fClusters; //! do not write to file
    std::vector<int> fActiveClusters;     //! do not write to file
    EXOPCDCluster* fRootCluster;
    ClassDef(EXOClusterPCDs,1)
};

#endif
