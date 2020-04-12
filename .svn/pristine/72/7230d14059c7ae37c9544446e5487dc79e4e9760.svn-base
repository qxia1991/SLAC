
#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include "G4RotationMatrix.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "G4TwoVector.hh"
#include <vector>
#include <G4Version.hh>

class G4VPhysicalVolume;
class G4Tubs;

// Dummy declarations to get rid of warnings ...
class G4Box;
class G4Trd;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Ellipsoid;
class G4Torus;
class G4Para;
class G4Hype;
class G4Polycone;
class G4Polyhedra;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOAPDParameterisation : public G4VPVParameterisation
{ 
  public:
  
    EXOAPDParameterisation();

    virtual				 
   ~EXOAPDParameterisation();
   
    void ComputeTransformation (const G4int copyNo,
                                G4VPhysicalVolume* physVol) const;
    
    void ComputeDimensions (G4Tubs & APDcopy, const G4int copyNo,
                            const G4VPhysicalVolume* physVol) const;
                            
    std::vector<G4TwoVector> GetAPDPositions();

  private:  // Dummy declarations to get rid of warnings ...

    void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*) const {}
#if 0+G4VERSION_NUMBER >= 1000
    void ComputeDimensions (G4Ellipsoid&,const G4int,const G4VPhysicalVolume*) const {}
#endif
    void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Box&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*) const {}
  private:   
    G4double xAPDcenter;
    G4double yAPDcenter;
    G4double zAPDcenter;  
  G4double fAPDx[NUMBER_APDS_PER_PLANE];
  G4double fAPDy[NUMBER_APDS_PER_PLANE];

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



