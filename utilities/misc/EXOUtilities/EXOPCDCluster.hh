#ifndef EXOPCDCluster_hh
#define EXOPCDCluster_hh

#include "EXOUtilities/EXOCoordinates.hh"
#include "TObject.h"
#include <set>

class EXOMCPixelatedChargeDeposit;

class EXOPCDCluster : public TObject
{
  public:
    EXOPCDCluster();
    EXOPCDCluster(const EXOMCPixelatedChargeDeposit* pcd);
    EXOPCDCluster(EXOPCDCluster &child1, EXOPCDCluster &child2, double dist);
    EXOPCDCluster(const EXOPCDCluster& other);

    EXOPCDCluster& operator=(const EXOPCDCluster& other);
    double GetDistance() const {return fDistance;}
    const EXOCoordinates& GetCenter() const {return fCenter;}
    double GetEnergy() const {return fEnergy;}

    EXOPCDCluster* GetChild1() const; 
    EXOPCDCluster* GetChild2() const;
    EXOPCDCluster* GetMother() const;
    const EXOMCPixelatedChargeDeposit* GetPCD() const;
    std::set<const EXOMCPixelatedChargeDeposit*> GetPCDs() const;

  private:

    void Collect(std::set<const EXOMCPixelatedChargeDeposit*>& collection) const;

    double fDistance;
    double fEnergy;
    EXOCoordinates fCenter;
    bool fIsLeaf;

    const EXOMCPixelatedChargeDeposit* fPCD;
    EXOPCDCluster* fChild1;
    EXOPCDCluster* fChild2;
    EXOPCDCluster* fMother;
    ClassDef(EXOPCDCluster,1)
};

#endif
