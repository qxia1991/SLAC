//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: EXOFullDetectorConstruction.hh,v 1.7 2006/06/29 17:48:32 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EXOFullDetectorConstruction_h
#define EXOFullDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "EXOSim/EXOFullDetectorMessenger.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UniformMagField;
class G4Region;
class EXOLXeSD;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOFullDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    EXOFullDetectorConstruction();
   ~EXOFullDetectorConstruction();

  public:
     
     G4VPhysicalVolume* Construct();

     void UpdateGeometry();
     
  public:
  
     G4double GetWorldSizeX()           {return WorldSizeX;}; 
     G4double GetWorldSizeYZ()          {return WorldSizeYZ;};
     
     const G4VPhysicalVolume* GetphysiWorld() {return physiWorld;};           
                 
  private:
     
     G4Material*        defaultMaterial;
     G4Material*        MineAir;
     G4Material*        MineWall;
     G4Material*        SSTank;
     G4Material*        OuterWater;
     G4Material*        AirGap;
     G4Material*        HDPETank;
     G4Material*        IBeam;
     G4Material*        BaTag;
     G4Material*        PbPlate;
     G4Material*        InnerWater;
     G4Material*        OuterVessel;
     G4Material*        VacuumSpace;
     G4Material*        InnerVessel;
     G4Material*        HFE;
     G4Material*        TPC;
     G4Material*        LXe;
     G4Material*        TeflonLining;

     G4double           WorldSizeYZ;
     G4double           WorldSizeX;
     G4double           MineThickness;
     G4double           MineX;
     G4double           MineY;
     G4double           MineZ;
     G4double           SSTankR;
     G4double           SSTankh;
     G4double           SSTankTBh;
     G4double           OuterWaterR;
     G4double           OuterWaterh;
     G4double           OuterWaterLh;
     G4double           HDPETankOR;
     G4double           HDPETankIR;
     G4double           HDPETankh;
     G4double           HDPETankBh;
     G4double           HDPEIBeamh;
     G4double           BaTagh;
     G4double           PbPlateh;
     G4double           PbIBeamh;
     G4double           InnerWaterh;
     G4double           OuterTiOR;
     G4double           OuterTiIR;
     G4double           OuterTih;
     G4double           OuterTiTBh;
     G4double           InnerTiOR;
     G4double           InnerTiIR;
     G4double           InnerTih;
     G4double           InnerTiTBh;
     G4double           TPCThick;
     G4double           TPCOuterX;
     G4double           TPCInnerX;
     G4double           TPCY;
     G4double           TPCZ;
     G4double           TeflonThick;
            
     G4Box*             solidWorld;    //pointer to the solid World 
     G4LogicalVolume*   logicWorld;    //pointer to the logical World
     G4VPhysicalVolume* physiWorld;    //pointer to the physical World

     G4Box*             solidMineTB;    //pointer to the solid Mine TopBottom 
     G4LogicalVolume*   logicMineTB;    //pointer to the logical Mine TopBottom
     G4VPhysicalVolume* physiMineTop;    //pointer to the physical Mine Top
     G4VPhysicalVolume* physiMineBottom;    //pointer to the physical MineBottom 

     G4Box*             solidMineFB;    //pointer to the solid Mine FrontBack 
     G4LogicalVolume*   logicMineFB;    //pointer to the logical Mine FrontBack
     G4VPhysicalVolume* physiMineFront;    //pointer to the physical Mine Front
     G4VPhysicalVolume* physiMineBack;    //pointer to the physical Mine Back 

     G4Box*             solidMineRL;    //pointer to the solid Mine FrontBack 
     G4LogicalVolume*   logicMineRL;    //pointer to the logical Mine FrontBack
     G4VPhysicalVolume* physiMineRight;    //pointer to the physical Mine Right
     G4VPhysicalVolume* physiMineLeft;    //pointer to the physical Mine Left 

     G4Box*             solidMineAir;    //pointer to the solid Mine FrontBack 
     G4LogicalVolume*   logicMineAir;    //pointer to the logical Mine FrontBack
     G4VPhysicalVolume* physiMineAir;    //pointer to the physical Mine Right

     G4Tubs*            solidSSTankTB;    //pointer to the solid SSTankTB 
     G4LogicalVolume*   logicSSTankTB;    //pointer to the logical SSTankTB
     G4VPhysicalVolume* physiSSTankTop;    //pointer to the physical SSTank Top
     G4VPhysicalVolume* physiSSTankBottom; //pointer to the physical SSTank Bot 

     G4Tubs*            solidSSTank;    //pointer to the solid SSTank 
     G4LogicalVolume*   logicSSTank;    //pointer to the logical SSTank
     G4VPhysicalVolume* physiSSTank;    //pointer to the physical SSTank

     G4Tubs*            solidOuterWaterL;   //pointer to the solid H2O Shield 
     G4LogicalVolume*   logicOuterWaterL;  //pointer to the logical H2O Shield
     G4VPhysicalVolume* physiOuterWaterL;  //pointer to the physical H2O Shield
     G4Tubs*            solidOuterWaterU;   //pointer to the solid H2O Shield 
     G4LogicalVolume*   logicOuterWaterU;  //pointer to the logical H2O Shield
     G4VPhysicalVolume* physiOuterWaterU;  //pointer to the physical H2O Shield

     G4Tubs*            solidHDPETankB;  //pointer to the solid HDPE TankBot
     G4LogicalVolume*   logicHDPETankB;  //pointer to the logical HDPE TankBot
     G4VPhysicalVolume* physiHDPETankB;  //pointer to the physical HDPE Tank Bot

     G4Tubs*            solidHDPETank;  //pointer to the solid HDPE Tank
     G4LogicalVolume*   logicHDPETank;  //pointer to the logical HDPE Tank
     G4VPhysicalVolume* physiHDPETank;  //pointer to the physical HDPE Tank

     G4Tubs*            solidHDPEIBeam;  //pointer to the solid IBeam support
     G4LogicalVolume*   logicHDPEIBeam;  //pointer to the logical IBeam support
     G4VPhysicalVolume* physiHDPEIBeam;  //pointer to the physical IBeam support

     G4Tubs*            solidBaTag;  //pointer to the solid Ba Tag Area
     G4LogicalVolume*   logicBaTag;  //pointer to the logical Ba Tag Area
     G4VPhysicalVolume* physiBaTag;  //pointer to the physical Ba Tag Area

     G4Tubs*            solidPbPlate;  //pointer to the solid PbPlate
     G4LogicalVolume*   logicPbPlate;  //pointer to the logical PbPlate
     G4VPhysicalVolume* physiPbPlate;  //pointer to the physical PbPlate

     G4Tubs*            solidPbIBeam;  //pointer to the solid PbIBeam
     G4LogicalVolume*   logicPbIBeam;  //pointer to the logical PbIBeam
     G4VPhysicalVolume* physiPbIBeam;  //pointer to the physical PbIBeam

     G4Tubs*            solidInnerWater;  //pointer to the solid InnerWater
     G4LogicalVolume*   logicInnerWater;  //pointer to the logical InnerWater
     G4VPhysicalVolume* physiInnerWater;  //pointer to the physical InnerWater

     G4Tubs*            solidOuterTi;  //pointer to the solid Outer Vessel
     G4LogicalVolume*   logicOuterTi;  //pointer to the logical Outer Vessel
     G4VPhysicalVolume* physiOuterTi;  //pointer to the physical Outer Vessel

     G4Tubs*            solidOuterTiTB;  //pointer to the solid Outer Vessel
     G4LogicalVolume*   logicOuterTiTB;  //pointer to the logical Outer Vessel
     G4VPhysicalVolume* physiOuterTiTop;  //pointer to the physical Outer Vessel
     G4VPhysicalVolume* physiOuterTiBot;  //pointer to the physical Outer Vessel

     G4Tubs*            solidVacuumSpace;  //pointer to the solid Vacuum Space
     G4LogicalVolume*   logicVacuumSpace;  //pointer to the logical Vacuum Space
     G4VPhysicalVolume* physiVacuumSpace;  //pointer to the physical VacuumSpace

     G4Tubs*            solidInnerTi;  //pointer to the solid Inner Vessel
     G4LogicalVolume*   logicInnerTi;  //pointer to the logical Inner Vessel
     G4VPhysicalVolume* physiInnerTi;  //pointer to the physical Inner Vessel

     G4Tubs*            solidInnerTiTB;  //pointer to the solid Outer Vessel
     G4LogicalVolume*   logicInnerTiTB;  //pointer to the logical Outer Vessel
     G4VPhysicalVolume* physiInnerTiTop;  //pointer to the physical Outer Vessel
     G4VPhysicalVolume* physiInnerTiBot;  //pointer to the physical Outer Vessel

     G4Tubs*            solidHFE;  //pointer to the solid HFE
     G4LogicalVolume*   logicHFE;  //pointer to the logical HFE
     G4VPhysicalVolume* physiHFE;  //pointer to the physical HFE

     G4Box*             solidTPCTB;    //pointer to the solid TPC TopBottom
     G4LogicalVolume*   logicTPCTB;    //pointer to the logical TPC TopBottom
     G4VPhysicalVolume* physiTPCTop;    //pointer to the physical TPC Top
     G4VPhysicalVolume* physiTPCBottom;    //pointer to the physical TPC Bottom

     G4Box*             solidTPCRL;    //pointer to the solid TPC RL 
     G4LogicalVolume*   logicTPCRL;    //pointer to the logical TPC RL
     G4VPhysicalVolume* physiTPCOuterRight;  //pointer to the physical TPC Out R
     G4VPhysicalVolume* physiTPCOuterLeft;  //pointer to the physical TPC Out L
     G4VPhysicalVolume* physiTPCInnerRight;  //pointer to the physical TPC In R
     G4VPhysicalVolume* physiTPCInnerLeft;  //pointer to the physical TPC In L

     G4Box*             solidTPCFB;    //pointer to the solid TPC FrontBack
     G4LogicalVolume*   logicTPCFB;    //pointer to the logical TPC FrontBack
     G4VPhysicalVolume* physiTPCFront;    //pointer to the physical TPC Front
     G4VPhysicalVolume* physiTPCBack;    //pointer to the physical TPC Back

     G4Box*             solidLXe1;    //pointer to the solid LXe
     G4LogicalVolume*   logicLXe1;    //pointer to the logical LXe
     G4VPhysicalVolume* physiLXe1;    //pointer to the physical LXe 1
     G4VPhysicalVolume* physiLXe3;    //pointer to the physical LXe 3
     G4Box*             solidLXe2;    //pointer to the solid LXe
     G4LogicalVolume*   logicLXe2;    //pointer to the logical LXe
     G4VPhysicalVolume* physiLXe2;    //pointer to the physical LXe 2

     G4Box*             solidTeflonTB1;    //pointer to the solid Teflon T/B 
     G4LogicalVolume*   logicTeflonTB1;    //pointer to the logical Teflon TB 
     G4VPhysicalVolume* physiTeflonTop1;   //pointer to the physical Teflon Top 
     G4VPhysicalVolume* physiTeflonBot1;   //pointer to the physical Teflon Bot 
     G4Box*             solidTeflonRL1;    //pointer to the solid Teflon RL
     G4LogicalVolume*   logicTeflonRL1;    //pointer to the logical Teflon RL 
     G4VPhysicalVolume* physiTeflonRight1;   //pointer to the physical Teflon R 
     G4VPhysicalVolume* physiTeflonLeft1;   //pointer to the physical Teflon L 
     G4Box*             solidTeflonFB1;    //pointer to the solid Teflon RL
     G4LogicalVolume*   logicTeflonFB1;    //pointer to the logical Teflon RL 
     G4VPhysicalVolume* physiTeflonFront1;   //pointer to the physical Teflon R 
     G4VPhysicalVolume* physiTeflonBack1;   //pointer to the physical Teflon L 

     G4Box*             solidTeflonTB2;    //pointer to the solid Teflon T/B 
     G4LogicalVolume*   logicTeflonTB2;    //pointer to the logical Teflon TB 
     G4VPhysicalVolume* physiTeflonTop2;   //pointer to the physical Teflon Top 
     G4VPhysicalVolume* physiTeflonBot2;   //pointer to the physical Teflon Bot 
     G4Box*             solidTeflonRL2;    //pointer to the solid Teflon RL
     G4LogicalVolume*   logicTeflonRL2;    //pointer to the logical Teflon RL 
     G4VPhysicalVolume* physiTeflonRight2;   //pointer to the physical Teflon R 
     G4VPhysicalVolume* physiTeflonLeft2;   //pointer to the physical Teflon L 
     G4Box*             solidTeflonFB2;    //pointer to the solid Teflon RL
     G4LogicalVolume*   logicTeflonFB2;    //pointer to the logical Teflon RL 
     G4VPhysicalVolume* physiTeflonFront2;   //pointer to the physical Teflon R 
     G4VPhysicalVolume* physiTeflonBack2;   //pointer to the physical Teflon L 

     G4Region*  targetRegion;
     G4Region*  detectorRegion;

     EXOLXeSD   *SD;
 
     G4bool     SDreg;

     EXOFullDetectorMessenger  detectorMessenger;  //pointer to the Messenger
      
  private:
    
     void DefineMaterials();
     G4VPhysicalVolume* ConstructDetector();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

