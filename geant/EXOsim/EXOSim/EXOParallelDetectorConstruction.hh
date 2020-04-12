
#ifndef EXOParallelDetectorConstruction_h
#define EXOParallelDetectorConstruction_h 1

#include "EXOPVolumeStore.hh"
#include "G4VUserParallelWorld.hh"
#include "globals.hh"
#include <vector>
#include "G4GeometryCell.hh"
#include "G4RotationMatrix.hh"

class G4Box;
class G4Tubs;
class G4Polyhedra;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class EXOParallelDetectorConstruction : public G4VUserParallelWorld
{
  public:
  
    EXOParallelDetectorConstruction(G4String worldName);
   ~EXOParallelDetectorConstruction();

  public:
     
     const G4VPhysicalVolume &GetPhysicalVolumeByName(const G4String& name) const;
     G4VPhysicalVolume &GetWorldVolumeAddress() const;
     G4String ListPhysNamesAsG4String();
     G4GeometryCell GetGeometryCell(G4int i);
     size_t GetNumCells() const { return fCells.size(); }
     G4VPhysicalVolume& GetWorldVolume() const;

     void SetupBiasing(double isFactor); 
     
  private:
     
     G4String GetCellName(G4int i);

     void Construct();
     EXOPVolumeStore fPVolumeStore;

     std::vector< const G4VPhysicalVolume * > fCells;


     G4Material*        defaultMaterial;


     G4VPhysicalVolume*  shadowWorld;

     G4RotationMatrix cryostatRM;


     void DefineMaterials();
     G4VPhysicalVolume* ConstructDetector();

};

#endif
