#include "EXOUtilities/EXOClusterPCDs.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include <cmath>
#include <limits>

using namespace std;

EXOClusterPCDs::EXOClusterPCDs(const EXOMonteCarloData &mcd)
: fDistances(mcd.GetNumPixelatedChargeDeposits(),mcd.GetNumPixelatedChargeDeposits(),numeric_limits<double>::max()),
  fRootCluster(NULL)
{
  //The clustering is done in this constructor

  //First initialize everything
  for(size_t i=0; i<mcd.GetNumPixelatedChargeDeposits(); i++){
    fClusters.push_back(EXOPCDCluster(mcd.GetPixelatedChargeDeposit(i)));
    fActiveClusters.push_back(i);
  }

  //Fill upper triangle of distance matrix
  for(size_t i=0; i<fClusters.size(); i++){
    for(size_t j=i+1; j<fClusters.size(); j++){
      fDistances[i][j] = CalculateDistance(fClusters[i],fClusters[j]);
    }
  }
  //Initialization complete

  DoClustering();
}

double EXOClusterPCDs::CalculateDistance(const EXOPCDCluster &c1, const EXOPCDCluster &c2) const
{
  const EXOCoordinates& pos1 = c1.GetCenter();
  const EXOCoordinates& pos2 = c2.GetCenter();
  double dx = pos1.GetX() - pos2.GetX();
  double dy = pos1.GetY() - pos2.GetY();
  double dz = pos1.GetZ() - pos2.GetZ();
  return sqrt(dx*dx + dy*dy + dz*dz);
}

void EXOClusterPCDs::DoClustering()
{
  //cout << "In DoClustering()" << endl;
  while(fActiveClusters.size() > 1){
    size_t x=0,y=0;
    double mindist = fDistances.Min(x,y);
    //fDistances.Print();
    //cout << "minimum at (" << x << "," << y << ")" << endl;
    //cout << "Active clusters: " << endl;
    for(size_t i=0; i<fActiveClusters.size(); i++){
      //cout << "  ac[" << i << "] with energy " << fClusters[fActiveClusters[i]].GetEnergy() << endl;
    }
    fClusters.push_back(EXOPCDCluster(fClusters[fActiveClusters[x]],fClusters[fActiveClusters[y]],mindist));
    fActiveClusters.erase(fActiveClusters.begin() + max(x,y));
    fActiveClusters.erase(fActiveClusters.begin() + min(x,y));
    fActiveClusters.push_back(fClusters.size()-1);
    fDistances.RemoveRowAndColumn(max(x,y));
    fDistances.RemoveRowAndColumn(min(x,y));
    fDistances.AddRowAndColumn(numeric_limits<double>::max());
    size_t posOfNew = fActiveClusters.size()-1; //should always be >= 0
    for(size_t i=0; i<posOfNew; i++){
      fDistances[i][posOfNew] = CalculateDistance(fClusters[fActiveClusters[i]],fClusters[fActiveClusters[posOfNew]]);
    }
  }
  fRootCluster = &fClusters.back();
}

EXOClusterPCDs::SetOfSets EXOClusterPCDs::GetClustersForDistance(double dist) const
{
  SetOfSets returnSet;
  if(fRootCluster){
    Collect(returnSet,dist,fRootCluster);
  }
  return returnSet;
}

vector<double> EXOClusterPCDs::GetEnergiesForDistance(double dist) const
{
  vector<double> returnVector;
  typedef set<const EXOMCPixelatedChargeDeposit*> Set;
  SetOfSets collection = GetClustersForDistance(dist);
  for(SetOfSets::const_iterator iter=collection.begin(); iter != collection.end(); iter++){
    double energy = 0.0;
    for(Set::const_iterator iter2=iter->begin(); iter2 != iter->end(); iter2++){
      energy += (*iter2)->fTotalEnergy;
    }
    returnVector.push_back(energy);
  }
  return returnVector;
}

void EXOClusterPCDs::Collect(SetOfSets& collection,double dist, const EXOPCDCluster* c) const
{
  if(c){
    if(c->GetDistance() < dist){
      collection.insert(c->GetPCDs());
      return;
    }
    Collect(collection,dist,c->GetChild1());
    Collect(collection,dist,c->GetChild2());
  }
}
