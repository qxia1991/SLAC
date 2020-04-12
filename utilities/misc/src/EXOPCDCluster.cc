#include "EXOUtilities/EXOPCDCluster.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <set>
#include <iostream>

using EXOMiscUtil::kXYCoordinates;
using namespace std;

EXOPCDCluster::EXOPCDCluster()
: fDistance(0),
  fEnergy(0),
  fIsLeaf(true),
  fPCD(NULL),
  fChild1(NULL),
  fChild2(NULL),
  fMother(NULL)
{

}

EXOPCDCluster::EXOPCDCluster(const EXOMCPixelatedChargeDeposit* pcd)
: fDistance(0),
  fEnergy(pcd->fTotalEnergy),
  fCenter(pcd->GetPixelCenter()),
  fIsLeaf(true),
  fPCD(pcd),
  fChild1(NULL),
  fChild2(NULL),
  fMother(NULL)
{
  //cout << "In leaf constructor. Energy = " << fEnergy << endl;
}

EXOPCDCluster::EXOPCDCluster(EXOPCDCluster &child1, EXOPCDCluster &child2, double dist)
: fDistance(dist),
  fEnergy(child1.fEnergy + child2.fEnergy),
  fIsLeaf(false),
  fPCD(NULL),
  fChild1(&child1),
  fChild2(&child2),
  fMother(NULL)
{
  //cout << "In mother constructor. Energy = " << fEnergy << endl;
  child1.fMother = this;
  child2.fMother = this;

  double x = (child1.fEnergy*child1.fCenter.GetX() + child2.fEnergy*child2.fCenter.GetX()) / fEnergy;
  double y = (child1.fEnergy*child1.fCenter.GetY() + child2.fEnergy*child2.fCenter.GetY()) / fEnergy;
  double z = (child1.fEnergy*child1.fCenter.GetZ() + child2.fEnergy*child2.fCenter.GetZ()) / fEnergy;
  double t = (child1.fEnergy*child1.fCenter.GetT() + child2.fEnergy*child2.fCenter.GetT()) / fEnergy;

  fCenter.SetCoordinates(kXYCoordinates,x,y,z,t);
}

EXOPCDCluster::EXOPCDCluster(const EXOPCDCluster& other)
: fDistance(other.fDistance),
  fEnergy(other.fEnergy),
  fCenter(other.fCenter),
  fIsLeaf(other.fIsLeaf),
  fPCD(other.fPCD),
  fChild1(other.fChild1),
  fChild2(other.fChild2),
  fMother(other.fMother)
{
  //cout << "In Copy Constructor" << endl;
  EXOPCDCluster* child1 = other.GetChild1();
  EXOPCDCluster* child2 = other.GetChild2();
  EXOPCDCluster* mother = other.GetMother();
  if(child1){
    child1->fMother = this;
  }
  if(child2){
    child2->fMother = this;
  }
  if(mother){
    if(mother->GetChild1() == &other){
      mother->fChild1 = this;
    }
    else if(mother->GetChild2() == &other){
      mother->fChild2 = this;
    }
    else{
      cout << "  Error!!" << endl;
    }
  }
}

EXOPCDCluster& EXOPCDCluster::operator=(const EXOPCDCluster& other)
{
  //cout << "In Assignment Operator" << endl;
  fChild1 = other.fChild1;
  fChild2 = other.fChild2;
  fMother = other.fMother;
  fDistance = other.fDistance;
  fEnergy = other.fEnergy;
  fCenter = other.fCenter;
  fIsLeaf = other.fIsLeaf;
  fPCD = other.fPCD;

  EXOPCDCluster* child1 = other.GetChild1();
  EXOPCDCluster* child2 = other.GetChild2();
  EXOPCDCluster* mother = other.GetMother();
  if(child1){
    child1->fMother = this;
  }
  if(child2){
    child2->fMother = this;
  }
  if(mother){
    if(mother->GetChild1() == &other){
      mother->fChild1 = this;
    }
    else if(mother->GetChild2() == &other){
      mother->fChild2 = this;
    }
    else{
      cout << "  Error!!" << endl;
    }
  }

  return *this;
}

EXOPCDCluster* EXOPCDCluster::GetChild1() const
{
  if(fIsLeaf){
    return NULL;
  }
  return fChild1;
}

EXOPCDCluster* EXOPCDCluster::GetChild2() const
{
  if(fIsLeaf){
    return NULL;
  }
  return fChild2;
}

EXOPCDCluster* EXOPCDCluster::GetMother() const
{
  return fMother;
}

const EXOMCPixelatedChargeDeposit* EXOPCDCluster::GetPCD() const
{
  if(not fIsLeaf){
    return NULL;
  }
  return fPCD;
}


set<const EXOMCPixelatedChargeDeposit*> EXOPCDCluster::GetPCDs() const
{
  set<const EXOMCPixelatedChargeDeposit*> returnSet;
  Collect(returnSet);
  return returnSet;
}

void EXOPCDCluster::Collect(set<const EXOMCPixelatedChargeDeposit*>& collection) const
{
  EXOPCDCluster* child1 = GetChild1();
  EXOPCDCluster* child2 = GetChild2();

  if(child1){
    child1->Collect(collection);
  }
  if(child2){
    child2->Collect(collection);
  }
  if(fIsLeaf){
    collection.insert(GetPCD());
  }
}
