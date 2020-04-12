/////////////////////////////////////////////////////////////////////
// EXOFullDetectorConstruction.cc
// Coarse EXO-full Geometry
////////////////////////////////////////////////////////////////////
//

#include "EXOSim/EXOFullDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include <sstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOFullDetectorConstruction::EXOFullDetectorConstruction()
:defaultMaterial(0),MineAir(0),MineWall(0),SSTank(0),OuterWater(0),AirGap(0),
 HDPETank(0),IBeam(0),BaTag(0),PbPlate(0),InnerWater(0),OuterVessel(0),
 VacuumSpace(0),InnerVessel(0),HFE(0),TPC(0),LXe(0),TeflonLining(0),
 solidWorld(0),logicWorld(0),physiWorld(0),
 solidMineTB(0),logicMineTB(0),physiMineTop(0),physiMineBottom(0),
 solidMineFB(0),logicMineFB(0),physiMineFront(0),physiMineBack(0),
 solidMineRL(0),logicMineRL(0),physiMineRight(0),physiMineLeft(0),
 solidMineAir(0),logicMineAir(0),physiMineAir(0),
 solidSSTankTB(0),logicSSTankTB(0),physiSSTankTop(0),physiSSTankBottom(0),
 solidSSTank(0),logicSSTank(0),physiSSTank(0),
 solidOuterWaterL(0),logicOuterWaterL(0),physiOuterWaterL(0),
 solidOuterWaterU(0),logicOuterWaterU(0),physiOuterWaterU(0),
 solidHDPETankB(0),logicHDPETankB(0),physiHDPETankB(0),
 solidHDPETank(0),logicHDPETank(0),physiHDPETank(0),
 solidHDPEIBeam(0),logicHDPEIBeam(0),physiHDPEIBeam(0),
 solidBaTag(0),logicBaTag(0),physiBaTag(0),
 solidPbPlate(0),logicPbPlate(0),physiPbPlate(0),
 solidPbIBeam(0),logicPbIBeam(0),physiPbIBeam(0),
 solidInnerWater(0),logicInnerWater(0),physiInnerWater(0),
 solidOuterTi(0),logicOuterTi(0),physiOuterTi(0),
 solidOuterTiTB(0),logicOuterTiTB(0),physiOuterTiTop(0),physiOuterTiBot(0),
 solidVacuumSpace(0),logicVacuumSpace(0),physiVacuumSpace(0),
 solidInnerTi(0),logicInnerTi(0),physiInnerTi(0),
 solidInnerTiTB(0),logicInnerTiTB(0),physiInnerTiTop(0),physiInnerTiBot(0),
 solidHFE(0),logicHFE(0),physiHFE(0),
 solidTPCTB(0),logicTPCTB(0),physiTPCTop(0),physiTPCBottom(0),
 solidTPCRL(0),logicTPCRL(0),physiTPCOuterRight(0),physiTPCOuterLeft(0),
 physiTPCInnerRight(0),physiTPCInnerLeft(0),
 solidTPCFB(0),logicTPCFB(0),physiTPCFront(0),physiTPCBack(0),
 solidLXe1(0),logicLXe1(0),physiLXe1(0),physiLXe3(0),
 solidLXe2(0),logicLXe2(0),physiLXe2(0),
 solidTeflonTB1(0),logicTeflonTB1(0),physiTeflonTop1(0),physiTeflonBot1(0),
 solidTeflonRL1(0),logicTeflonRL1(0),physiTeflonRight1(0),physiTeflonLeft1(0),
 solidTeflonFB1(0),logicTeflonFB1(0),physiTeflonFront1(0),physiTeflonBack1(0),
 solidTeflonTB2(0),logicTeflonTB2(0),physiTeflonTop2(0),physiTeflonBot2(0),
 solidTeflonRL2(0),logicTeflonRL2(0),physiTeflonRight2(0),physiTeflonLeft2(0),
 solidTeflonFB2(0),logicTeflonFB2(0),physiTeflonFront2(0),physiTeflonBack2(0),
 detectorMessenger(this)

{

//**************************************************//
//****************DEFAULT VALUES********************//
//**************************************************//
//
//  Values are from conversations with Matt Swift & John Ku
//  on 1/26/12.
//  Setup assumes experiment will be at DUSEL
//    --AS 1/26/12
//////////////////////////////////////////////////////////

//THE WORLD

  WorldSizeX = 4000.*cm;
  WorldSizeYZ = 4000.*cm;

//THE MINE

  MineThickness = 35.*cm;      // 70 cm rock thickness per G. Gratta
  MineX         = 2500.*cm;    // 5 m Worst-case scenario from J. Ku
  MineY         = 1000.*cm;    // 2 m
  MineZ         = 1000.*cm;    // 2 m

//STAINLESS STEEL TANK

  SSTankR    = 851.27*cm;     // OD = 1702.54 cm
  SSTankTBh  = 0.635*cm;      // wall thickness = 1.27 cm
  SSTankh    = 774.365*cm;    // height = total height - thickness 
                              // (Total height = 1550 cm)
  
//OUTER WATER SHIELD

  OuterWaterR  = 850.*cm;      // ID of SS Tank = 1700 cm
  OuterWaterh  = 525.*cm;      // Height from HDPE bottom to top of water
                               // Currently set to height of HDPE tank = 1050 cm
  OuterWaterLh = 249.365*cm;   // Distance from bottom of HDPE tank to SS tank bottom
                               // SS Tank height - HDPE height = 498.73 cm

//HDPE TANK

  HDPETankOR  = 510.*cm;       // OD = 1020 cm
  HDPETankIR  = 485.*cm;       // ID = 970 cm
  HDPETankh   = 512.5*cm;      // height = total height - thickness 
                               // Total height = 1050 cm
  HDPETankBh  = 12.5*cm;       // wall thickness = 25 cm
  HDPEIBeamh  = 7.62*cm;       // Support structure for HDPE Tank (SS)
                               // Thickness = 15.24 cm

//BA TAGGING AREA AND PB PLATE

  BaTagh    = 204.88*cm;       // Height = 409.76 cm 
  PbPlateh  = 25.*cm;          // Thickness of Pb Plate = 50 cm
  PbIBeamh  = 7.62*cm;         // Thickness of SS support structure = 15.24 cm

//INNER WATER SHIELD

  InnerWaterh  = 275.*cm;    // Height = 550 cm

//OUTER TI VESSEL

  OuterTiOR   = 220.5*cm;      // OD = 441 cm
  OuterTiIR   = 218.*cm;       // ID = 436 cm
  OuterTih    = 236.25*cm;     // height = Total height - 2*thickness
                               // Total height = 473 cm
  OuterTiTBh  = 1.25*cm;       // wall thickness = 2.5 cm

//INNER TI VESSEL

  InnerTiOR   = 207.74*cm;     // OD = 415.48 cm
  InnerTiIR   = 205.74*cm;     // ID = 411.48 cm
  InnerTih    = 198.*cm;       // height = Total height - 2*thickness
                               // Total height = 400 cm
  InnerTiTBh  = 1.*cm;         // wall thickness = 2 cm

//THE TPC VESSEL

  TPCThick   = 0.15*cm;        // wall thickness = 0.3 cm (3 mm) per G. Gratta
  TPCOuterX  = 75.6*cm;        // 151.2 cm
  TPCInnerX  = 25.*cm;         // size of Xe cells = 50 cm
  TPCY       = 80.3*cm;        // 160.6
  TPCZ       = 75.3*cm;        // 150.6

//TEFLON LINING

  TeflonThick   = 0.75*mm;     // Value from EXO-200, per M. Dolinski's paper


  // materials
  DefineMaterials();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOFullDetectorConstruction::~EXOFullDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EXOFullDetectorConstruction::Construct()
{
  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOFullDetectorConstruction::DefineMaterials()
{ 

G4String symbol;             //a=mass of a mole;
G4double a, z, density;      //z=mean number of protons;  
//G4int iz, n;                 //iz=number of protons  in an isotope; 
                             // n=number of nucleons in an isotope;

G4int ncomponents, natoms;
G4double fractionmass;

//**************************************************//
//****************DEFINE ELEMENTS*******************//
//**************************************************//

G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
G4Element* F  = new G4Element("Fluorine",symbol="F" , z= 9., a= 19.00*g/mole);
G4Element* Mg = new G4Element("Magnesium",symbol="Mg",z= 12.,a= 24.30*g/mole);
G4Element* Si = new G4Element("Silicon",symbol="Si" ,z= 14., a= 28.09*g/mole);
G4Element* Ar = new G4Element("Argon"  ,symbol="Ar" ,z= 18., a= 39.95*g/mole);
G4Element* Ca = new G4Element("Calcium",symbol="Ca" ,z= 20., a= 40.08*g/mole);
G4Element* Ti = new G4Element("Titanium",symbol="Ti",z= 22., a= 42.867*g/mole);
G4Element* Cr = new G4Element("Chromium",symbol="Cr",z= 24., a= 52.00*g/mole);
G4Element* Fe = new G4Element("Iron"    ,symbol="Fe",z= 26., a= 55.84*g/mole);
G4Element* Ni = new G4Element("Nickel"  ,symbol="Ni",z= 28., a= 58.69*g/mole);
G4Element* Cu = new G4Element("Copper"  ,symbol="Cu",z= 29., a= 63.55*g/mole);
G4Element* Xe = new G4Element("Xenon"   ,symbol="Xe",z= 54., a= 131.29*g/mole);
G4Element* Pb = new G4Element("Lead"    ,symbol="Pb",z= 82., a= 207.2*g/mole);

//**************************************************//
//****************DEFINE MATERIALS******************//
//**************************************************//

G4Material* H2O = 
new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
H2O->AddElement(H, natoms=2);
H2O->AddElement(O, natoms=1);

G4Material* C4H3OF7 = 
new G4Material("HFE", density= 1.77*g/cm3, ncomponents=4);
C4H3OF7->AddElement(H, natoms=3);
C4H3OF7->AddElement(C, natoms=4);
C4H3OF7->AddElement(O, natoms=1);
C4H3OF7->AddElement(F, natoms=7);

G4Material* Air = new G4Material("Air",density= 1.25*mg/cm3,ncomponents=3);
Air->AddElement(N, fractionmass=0.758);
Air->AddElement(O, fractionmass=0.232);
Air->AddElement(Ar, fractionmass=0.010);

G4Material* Concrete = 
new G4Material("concrete",density= 2.34*g/cm3, ncomponents=6);
Concrete->AddElement(C, natoms=23);
Concrete->AddElement(O, natoms=40);
Concrete->AddElement(H, natoms=10);
Concrete->AddElement(Si, natoms=12);
Concrete->AddElement(Mg, natoms=2);
Concrete->AddElement(Ca, natoms=12);

G4Material* Copper = new G4Material("Copper",density=8.96*g/cm3,ncomponents=1);
Copper->AddElement(Cu, fractionmass=1.0);

G4Material* Lead = new G4Material("Lead",density= 11.34*g/cm3,ncomponents=1);
Lead->AddElement(Pb, fractionmass=1.0);

G4Material* SStl = 
new G4Material("StainlessSteel",density= 8.*g/cm3, ncomponents=3);
SStl->AddElement(Cr, fractionmass=0.08);
SStl->AddElement(Fe, fractionmass=0.74);
SStl->AddElement(Ni, fractionmass=0.18);

G4Material* RedSStl = 
new G4Material("ReducedStainlessSteel",density= 0.462*g/cm3, ncomponents=1);
RedSStl->AddMaterial(SStl, fractionmass=1.);

G4Material* Poly = 
new G4Material("HDPE",density= 0.95*g/cm3, ncomponents=2);
Poly->AddElement(H, fractionmass=0.144);
Poly->AddElement(C, fractionmass=0.856);

G4Material* Titanium = new G4Material("Titanium",density= 4.506*g/cm3,
                       ncomponents=1);
Titanium->AddElement(Ti, fractionmass=1.0);

G4Material* Vacuum =
new G4Material("Vacuum",density= 1.e-11*g/cm3,ncomponents=1);
Vacuum->AddMaterial(Air, fractionmass=1.);

G4Material* LiquidXe = new G4Material("LiquidXe",density= 3.057*g/cm3,
                       ncomponents=1);
LiquidXe->AddElement(Xe, fractionmass=1.0);

G4Material* Teflon = new G4Material("Teflon",density= 2.2*g/cm3,
                       ncomponents=2);
Teflon->AddElement(C, fractionmass=0.240183);
Teflon->AddElement(F, fractionmass=0.759817);

//**************************************************//
//****************MATERIALS OF EVERYTHING***********//
//**************************************************//

defaultMaterial  = Vacuum;
MineWall         = Concrete;
MineAir          = Air;
SSTank           = SStl;
OuterWater       = H2O;
AirGap           = Air;
HDPETank         = Poly;
IBeam            = RedSStl;
BaTag            = Air;
PbPlate          = Lead;
InnerWater       = H2O;
OuterVessel      = Titanium;
VacuumSpace      = Vacuum;
InnerVessel      = Titanium;
HFE              = C4H3OF7;
TPC              = Copper;
TeflonLining     = Teflon;
LXe              = LiquidXe;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VPhysicalVolume* EXOFullDetectorConstruction::ConstructDetector()
{

  // Clean old geometry, if any
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
   
//**************************************************//
//*********************** WORLD ********************//
//**************************************************//

  solidWorld = new G4Box("World",WorldSizeX,WorldSizeYZ,WorldSizeYZ);

  logicWorld = new G4LogicalVolume(solidWorld,      //its solid
                                   defaultMaterial, //its material
                                   "World");        //its name

  physiWorld = new G4PVPlacement(0,                 //no rotation
                                 G4ThreeVector(),   //at (0,0,0)
                                 logicWorld,        //its logical volume
                                 "World",           //its name
                                 0,                 //its mother volume
                                 false,             //no boolean operation
                                 0);                //copy number
  
//**************************************************//
//******************** MINE WALLS ******************//
//**************************************************//

//Top & bottom planes
  solidMineTB = new G4Box("MineTB",                 //its name
                          MineX+2*MineThickness,    //X half-length
                          MineY+2*MineThickness,    //Y half-length
                          MineThickness);           //Z half-length

  logicMineTB = new G4LogicalVolume(solidMineTB,    //its solid
                                    MineWall,       //its material
                                    "MineTB");      //its name

  physiMineTop = new G4PVPlacement(0,              //no rotation
                                   G4ThreeVector(0,                       //X 
                                                 0,                       //Y
                                                 2*MineZ+MineThickness),  //Z
                                   logicMineTB,   //its logical volume
                                   "MineTop",     //its name
                                   logicWorld,    //its mother volume
                                   false,         //no boolean operation
                                   0);            //copy number

  physiMineBottom = new G4PVPlacement(0,            //no rotation
                                      G4ThreeVector(0,                 //X
                                                    0,                 //Y
                                                    -MineThickness),   //Z
                                      logicMineTB,  //its logical volume
                                      "MineBottom", //its name
                                      logicWorld,   //its mother volume
                                      false,        //no boolean operation
                                      0);           //copy number

//Front & Back planes
  solidMineFB = new G4Box("MineFB",               //its name
                          MineX,                  //X half-length
                          MineThickness,          //Y half-length
                          MineZ);                 //Z half-length

  logicMineFB = new G4LogicalVolume(solidMineFB,  //its solid
                                    MineWall,     //its material
                                    "MineFB");    //its name

  physiMineFront = new G4PVPlacement(0,           //no rotation
                                     G4ThreeVector(0,                    //X
                                                   MineY+MineThickness,  //Y
                                                   MineZ),               //Z
                                     logicMineFB,  //its logical volume
                                     "MineFront",  //its name
                                     logicWorld,   //its mother volume
                                     false,        //no boolean operation
                                     0);           //copy number

  physiMineBack = new G4PVPlacement(0,             //no rotation
                                    G4ThreeVector(0,                    //X
                                                  -MineY-MineThickness, //Y
                                                  MineZ),               //Z
                                    logicMineFB,  //its logical volume
                                    "MineBack",   //its name
                                    logicWorld,   //its mother volume
                                    false,        //no boolean operation
                                    0);           //copy number

//Right & Left planes
  solidMineRL = new G4Box("MineRL",              //its name
                          MineThickness,         //X half-length
                          MineY+2*MineThickness, //Y half-length
                          MineZ);                //Z half-length

  logicMineRL = new G4LogicalVolume(solidMineRL, //its solid
                                    MineWall,    //its material
                                    "MineRL");   //its name

  physiMineLeft = new G4PVPlacement(0,           //no rotation
                                    G4ThreeVector(-MineX-MineThickness,  //X
                                                  0,                     //Y
                                                  MineZ),                //Z
                                    logicMineRL, //its logical volume
                                    "MineLeft",  //its name
                                    logicWorld,  //its mother volume
                                    false,       //no boolean operation
                                    0);         //copy number

  physiMineRight = new G4PVPlacement(0,          //no rotation
                                     G4ThreeVector(MineX+MineThickness,  //X
                                                   0,                    //Y
                                                   MineZ),               //Z
                                     logicMineRL, //its logical volume
                                     "MineRight", //its name
                                     logicWorld,  //its mother volume
                                     false,       //no boolean operation
                                     0);          //copy number

//**************************************************//
//******************** MINE AIR ********************//
//**************************************************//

  solidMineAir = new G4Box("MineAir",            //its name
                           MineX,                //X half-length
                           MineY,                //Y half-length
                           MineZ);               //Z half-length

  logicMineAir = new G4LogicalVolume(solidMineAir,  //its solid
                                     MineAir,       //its material
                                     "MineAir");    //its name

  physiMineAir = new G4PVPlacement(0,              //no rotation
                                   G4ThreeVector(0,              //X
                                                 0,              //Y
                                                 MineZ),         //Z
                                   logicMineAir,  //its logical volume
                                   "MineAir",     //its name
                                   logicWorld,    //its mother volume
                                   false,         //no boolean operation
                                   0);            //copy number

//**************************************************//
//******************** SS Tank *********************//
//**************************************************//

// SS Tank Floor & Ceiling
  solidSSTankTB = new G4Tubs("SSTankTB",          //its name
                             0,                   //inner radius
                             SSTankR,             //outer radius
                             SSTankTBh,           //Z half-length
                             0,                   //starting phi angle
                             360*deg);            //segment angle

  logicSSTankTB = new G4LogicalVolume(solidSSTankTB, //its solid
                                      SSTank,        //its material
                                      "SSTankTB");   //its name

  physiSSTankBottom = new G4PVPlacement(0,            //no rotation
                                        G4ThreeVector(0,                //X
                                                      0,                //Y
                                                      SSTankTBh-MineZ), //Z
                                        logicSSTankTB,  //its logical volume
                                        "SSTankBottom", //its name
                                        logicMineAir,   //its mother volume
                                        false,          //no boolean operation
                                        0);             //copy number

// SS Tank 
  solidSSTank = new G4Tubs("SSTankTB",          //its name
                           OuterWaterR,         //inner radius
                           SSTankR,             //outer radius
                           SSTankh,             //Z half-length
                           0,                   //starting phi angle
                           360*deg);            //segment angle

  logicSSTank = new G4LogicalVolume(solidSSTank,  //its solid
                                    SSTank,       //its material
                                    "SSTank");    //its name

  physiSSTank = new G4PVPlacement(0,             //no rotation
                                  G4ThreeVector(0,                            //X
                                                0,                            //Y
                                                SSTankh+2*SSTankTBh-MineZ),   //Z
                                  logicSSTank,  //its logical volume
                                  "SSTank",     //its name
                                  logicMineAir, //its mother volume
                                  false,        //no boolean operation
                                  0);           //copy number

//**************************************************//
//*************** OUTER WATER SHIELD ***************//
//**************************************************//

  solidOuterWaterL = new G4Tubs("OuterWaterLower", //its name
                                0,                 //inner radius
                                OuterWaterR,       //outer radius
                                OuterWaterLh,      //Z half-length
                                0,                 //starting phi angle
                                360*deg);          //segment angle

  logicOuterWaterL = new G4LogicalVolume(solidOuterWaterL,   //its solid
                                         OuterWater,         //its material
                                         "OuterWaterLower"); //its name

  physiOuterWaterL = new G4PVPlacement(0,                   //no rotation
                                       G4ThreeVector(0,                               //X
                                                     0,                               //Y
                                                     OuterWaterLh+2*SSTankTBh-MineZ), //Z
                                       logicOuterWaterL,   //its logical volume
                                       "OuterWaterLower",  //its name
                                       logicMineAir,       //its mother volume
                                       false,              //no boolean operation
                                       0);                 //copy number

  solidOuterWaterU = new G4Tubs("OuterWaterUpper",         //its name
                                HDPETankOR,                //inner radius
                                OuterWaterR,               //outer radius
                                OuterWaterh-OuterWaterLh,  //Z half-length
                                0,                         //starting phi angle
                                360*deg);                  //segment angle

  logicOuterWaterU = new G4LogicalVolume(solidOuterWaterU,   //its solid
                                         OuterWater,         //its material
                                         "OuterWaterUpper"); //its name

  physiOuterWaterU = new G4PVPlacement(0,                    //no rotation
                                       G4ThreeVector(0,                                          //X
                                                     0,                                          //Y
                                                     OuterWaterh+OuterWaterLh+2*SSTankTBh-MineZ),//Z
                                       logicOuterWaterU,    //its logical volume
                                       "OuterWaterUpper",   //its name
                                       logicMineAir,        //its mother volume
                                       false,               //no boolean operation
                                       0);                  //copy number

//**************************************************//
//******************* HDPE TANK ********************//
//**************************************************//

// HDPE Tank Bottom
  solidHDPETankB = new G4Tubs("HDPEBottom",     //its name
                              0,                //inner radius
                              HDPETankOR,       //outer radius
                              HDPETankBh,       //Z half-length
                              0,                //starting phi angle
                              360*deg);         //segment angle

  logicHDPETankB = new G4LogicalVolume(solidHDPETankB,  //its solid
                                       HDPETank,        //its material
                                       "HDPEBottom");   //its name

  physiHDPETankB = new G4PVPlacement(0,                 //no rotation
                                     G4ThreeVector(0,                                            //X
                                                   0,                                            //Y
                                                   HDPETankBh+2*SSTankTBh+2*OuterWaterLh-MineZ), //Z
                                     logicHDPETankB,   //its logical volume
                                     "HDPEBottom",     //its name
                                     logicMineAir,     //its mother volume
                                     false,            //no boolean operation
                                     0);               //copy number

// HDPE Tank 
  solidHDPETank = new G4Tubs("HDPETank",       //its name
                             HDPETankIR,       //inner radius
                             HDPETankOR,       //outer radius
                             HDPETankh,        //Z half-length
                             0,                //starting phi angle
                             360*deg);         //segment angle

  logicHDPETank = new G4LogicalVolume(solidHDPETank,  //its solid
                                      HDPETank,       //its material
                                      "HDPETank");    //its name

  physiHDPETank = new G4PVPlacement(0,               //no rotation
                                    G4ThreeVector(0,                                     //X
                                                  0,                                     //Y
                                                  HDPETankh+2*HDPETankBh+                //Z
                                                   2*OuterWaterLh+2*SSTankTBh-MineZ),
                                    logicHDPETank,  //its logical volume
                                    "HDPETank",     //its name
                                    logicMineAir,   //its mother volume
                                    false,          //no boolean operation
                                    0);             //copy number

// HDPE Tank Reduced-density I-Beam Support
  solidHDPEIBeam = new G4Tubs("HDPEIBeam",    //its name
                              0,              //inner radius
                              HDPETankOR,     //outer radius
                              HDPEIBeamh,     //Z half-length
                              0,              //starting phi angle
                              360*deg);       //segment angle

  logicHDPEIBeam = new G4LogicalVolume(solidHDPEIBeam,  //its solid
                                       IBeam,           //its material
                                       "HDPEIBeam");    //its name

  physiHDPEIBeam = new G4PVPlacement(0,                 //no rotation
                                     G4ThreeVector(0,                           //X
                                                   0,                           //Y
                                                   OuterWaterLh-HDPEIBeamh),    //Z
                                     logicHDPEIBeam,   //its logical volume
                                     "HDPEIBeam",      //its name
                                     logicOuterWaterL, //its mother volume
                                     false,            //no boolean operation
                                     0);               //copy number

//**************************************************//
//**************** BA TAGGING AREA *****************//
//**************************************************//

  solidBaTag = new G4Tubs("BaTag",           //its name
                          0,                 //inner radius
                          HDPETankIR,        //outer radius
                          BaTagh,            //Z half-length
                          0,                 //starting phi angle
                          360*deg);          //segment angle

  logicBaTag = new G4LogicalVolume(solidBaTag,  //its solid
                                   BaTag,       //its material
                                   "BaTag");    //its name

  physiBaTag = new G4PVPlacement(0,              //no rotation
                                 G4ThreeVector(0,                                       //X
                                               0,                                       //Y
                                               2*SSTankTBh+2*OuterWaterLh+2*HDPETankBh+ //Z
                                                2*InnerWaterh+2*PbIBeamh+2*PbPlateh+
                                                BaTagh-MineZ), 
                                 logicBaTag,    //its logical volume
                                 "BaTag",       //its name
                                 logicMineAir,  //its mother volume
                                 false,         //no boolean operation
                                 0);            //copy number

//**************************************************//
//******************** PB PLATE ********************//
//**************************************************//

// Pb Plate
  solidPbPlate = new G4Tubs("PbPlate",    //its name
                            0,            //inner radius
                            HDPETankIR,   //outer radius
                            PbPlateh,     //Z half-length
                            0,            //starting phi angle
                            360*deg);     //segment angle

  logicPbPlate = new G4LogicalVolume(solidPbPlate,  //its solid
                                     PbPlate,       //its material
                                     "PbPlate");    //its name

  physiPbPlate = new G4PVPlacement(0,               //no rotation
                                   G4ThreeVector(0,                                             //X
                                                 0,                                             //Y
                                                 2*SSTankTBh+2*OuterWaterLh+2*HDPETankBh+       //Z
                                                  2*InnerWaterh+2*PbIBeamh+PbPlateh-MineZ),
                                   logicPbPlate,   //its logical volume
                                   "PbPlate",      //its name
                                   logicMineAir,   //its mother volume
                                   false,          //no boolean operation
                                   0);             //copy number

// Pb Plate I-Beam support
  solidPbIBeam = new G4Tubs("PbIBeam",     //its name
                            0,             //inner radius
                            HDPETankIR,    //outer radius
                            PbIBeamh,      //Z half-length
                            0,             //starting phi angle
                            360*deg);      //segment angle

  logicPbIBeam = new G4LogicalVolume(solidPbIBeam,    //its solid
                                     IBeam,           //its material
                                     "PbIBeam");      //its name

  physiPbIBeam = new G4PVPlacement(0,               //no rotation
                                   G4ThreeVector(0,                                        //X
                                                 0,                                        //Y
                                                 2*SSTankTBh+2*OuterWaterLh+2*HDPETankBh+  //Z
                                                  2*InnerWaterh+PbIBeamh-MineZ),
                                   logicPbIBeam,   //its logical volume
                                   "PbIBeam",      //its name
                                   logicMineAir,   //its mother volume
                                   false,          //no boolean operation
                                   0);             //copy number

//**************************************************//
//******************* INNER WATER ******************//
//**************************************************//

  solidInnerWater = new G4Tubs("InnerWater",  //its name
                               0,             //inner radius
                               HDPETankIR,    //outer radius
                               InnerWaterh,   //Z half-length
                               0,             //starting phi angle
                               360*deg);      //segment angle

  logicInnerWater = new G4LogicalVolume(solidInnerWater,  //its solid
                                        InnerWater,       //its material
                                        "InnerWater");    //its name

  physiInnerWater = new G4PVPlacement(0,                 //no rotation
                                      G4ThreeVector(0,                                  //X
                                                    0,                                  //Y
                                                    2*SSTankTBh+2*OuterWaterLh+         //Z
                                                     2*HDPETankBh+InnerWaterh-MineZ),
                                      logicInnerWater,  //its logical volume
                                      "InnerWater",     //its name
                                      logicMineAir,     //its mother volume
                                      false,            //no boolean operation
                                      0);               //copy number

//**************************************************//
//***************** OUTER TI VOLUME ****************//
//**************************************************//

// Outer Vessel
  solidOuterTi = new G4Tubs("OuterTi",   //its name
                            OuterTiIR,   //inner radius
                            OuterTiOR,   //outer radius
                            OuterTih,    //Z half-length
                            0,           //starting phi angle
                            360*deg);    //segment angle

  logicOuterTi = new G4LogicalVolume(solidOuterTi,   //its solid
                                     OuterVessel,    //its material
                                     "OuterTi");     //its name

  physiOuterTi = new G4PVPlacement(0,                //no rotation
                                   G4ThreeVector(0,                                   //X
                                                 0,                                   //Y
                                                 -InnerWaterh+2*OuterTiTBh+OuterTih), //Z
                                   logicOuterTi,    //its logical volume
                                   "OuterTi",       //its name
                                   logicInnerWater, //its mother volume
                                   false,           //no boolean operation
                                   0);              //copy number

// Outer Vessel Top & Bottom
  solidOuterTiTB = new G4Tubs("OuterTiTB",  //its name
                              0,            //inner radius
                              OuterTiOR,    //outer radius
                              OuterTiTBh,   //Z half-length
                              0,            //starting phi angle
                              360*deg);     //segment angle

  logicOuterTiTB = new G4LogicalVolume(solidOuterTiTB,   //its solid
                                       OuterVessel,      //its material
                                       "OuterTiTB");     //its name

  physiOuterTiTop = new G4PVPlacement(0,               //no rotation
                                      G4ThreeVector(0,                                     //X
                                                    0,                                     //Y
                                                    -InnerWaterh+3*OuterTiTBh+2*OuterTih), //Z
                                      logicOuterTiTB, //its logical volume
                                      "OuterTiTop",   //its name
                                      logicInnerWater,//its mother volume 
                                      false,          //no boolean operation
                                      0);             //copy number

  physiOuterTiBot = new G4PVPlacement(0,               //no rotation
                                      G4ThreeVector(0,                        //X
                                                    0,                        //Y
                                                    -InnerWaterh+OuterTiTBh), //Z
                                      logicOuterTiTB,  //its logical volume
                                      "OuterTiBot",    //its name
                                      logicInnerWater, //its mother volume
                                      false,           //no boolean operation
                                      0);              //copy number

//**************************************************//
//****************** VACUUM SPACE ******************//
//**************************************************//

  solidVacuumSpace = new G4Tubs("VacuumSpace",  //its name
                                0,              //inner radius
                                OuterTiIR,      //outer radius
                                OuterTih,       //Z half-length
                                0,              //starting phi angle
                                360*deg);       //segment angle

  logicVacuumSpace = new G4LogicalVolume(solidVacuumSpace,   //its solid
                                         VacuumSpace,        //its material
                                         "VacuumSpace");     //its name

  physiVacuumSpace = new G4PVPlacement(0,                //no rotation
                                       G4ThreeVector(0,                                  //X
                                                     0,                                  //Y
                                                     -InnerWaterh+2*OuterTiTBh+OuterTih),//Z
                                       logicVacuumSpace, //its logical volume
                                       "VacuumSpace",    //its name
                                       logicInnerWater,  //its mother volume
                                       false,            //no boolean operation
                                       0);               //copy number

//**************************************************//
//***************** INNER TI VESSEL ****************//
//**************************************************//

// Inner Vessel
  solidInnerTi = new G4Tubs("InnerTi",  //its name
                            InnerTiIR,  //inner radius
                            InnerTiOR,  //outer radius
                            InnerTih,   //Z half-length
                            0,          //starting phi angle
                            360*deg);   //segment angle

  logicInnerTi = new G4LogicalVolume(solidInnerTi,   //its solid
                                     InnerVessel,    //its material
                                     "InnerTi");     //its name

  physiInnerTi = new G4PVPlacement(0,                 //no rotation
                                   G4ThreeVector(0,   //X
                                                 0,   //Y
                                                 0),  //Z
                                   logicInnerTi,      //its logical volume
                                   "InnerTi",         //its name
                                   logicVacuumSpace,  //its mother volume
                                   false,             //no boolean operation
                                   0);                //copy number

// Inner Vessel Top & Bottom
  solidInnerTiTB = new G4Tubs("InnerTiTB",  //its name
                              0,            //inner volume
                              InnerTiOR,    //outer volume
                              InnerTiTBh,   //Z half-length
                              0,            //starting phi angle
                              360*deg);     //segment angle

  logicInnerTiTB = new G4LogicalVolume(solidInnerTiTB, //its solid
                                       InnerVessel,    //its material
                                       "InnerTiTB");   //its name

  physiInnerTiTop = new G4PVPlacement(0,                //no rotation
                                      G4ThreeVector(0,                    //X
                                                    0,                    //Y
                                                    InnerTih+InnerTiTBh), //Z
                                      logicInnerTiTB,  //its logical volume
                                      "InnerTiTop",    //its name
                                      logicVacuumSpace,//its mother volume
                                      false,           //no boolean operation
                                      0);              //copy number

  physiInnerTiBot = new G4PVPlacement(0,                 //no rotation
                                      G4ThreeVector(0,                      //X
                                                    0,                      //Y
                                                    -InnerTih-InnerTiTBh),  //Z
                                      logicInnerTiTB,    //its logial volume
                                      "InnerTiBot",      //its name
                                      logicVacuumSpace,  //its mother volume
                                      false,             //no boolean operation
                                      0);                //copy number

//**************************************************//
//********************** HFE ***********************//
//**************************************************//

  solidHFE = new G4Tubs("HFE",      //its name
                        0,          //inner radius
                        InnerTiIR,  //outer radius
                        InnerTih,   //Z half-length
                        0,          //starting phi angle
                        360*deg);   //segment angle

  logicHFE = new G4LogicalVolume(solidHFE,  //its solid
                                 HFE,       //its material
                                 "HFE");    //its name

  physiHFE = new G4PVPlacement(0,                //no rotation
                               G4ThreeVector(0,  //X
                                             0,  //Y
                                             0), //Z
                               logicHFE,         //its logical volume
                               "HFE",            //its name
                               logicVacuumSpace, //its mother volume
                               false,            //no boolean operation
                               0);               //copy number

//**************************************************//
//********************** TPC ***********************//
//**************************************************//

//Top & bottom planes
  solidTPCTB = new G4Box("TPCTB",              //its name
                         TPCOuterX,            //X half-length
                         TPCY,                 //Y half-length
                         TPCThick);            //Z half-length

  logicTPCTB = new G4LogicalVolume(solidTPCTB, //its solid
                                   TPC,        //its material
                                   "TPCTB");   //its name

  physiTPCTop = new G4PVPlacement(0,           //no rotation
                                  G4ThreeVector(0,              //X
                                                0,              //Y
                                                TPCZ-TPCThick), //Z
                                  logicTPCTB,  //its logical volume
                                  "TPCTop",    //its name
                                  logicHFE,    //its mother volume
                                  false,       //no boolean operation
                                  0);          //copy number

  physiTPCBottom = new G4PVPlacement(0,             //no rotation
                                     G4ThreeVector(0,               //X
                                                   0,               //Y
                                                   -TPCZ+TPCThick), //Z
                                     logicTPCTB,    //its logical volume
                                     "TPCBottom",   //its name
                                     logicHFE,      //its mother volume
                                     false,         //no boolean operation
                                     0);            //copy number

//Front & Back planes
  solidTPCFB = new G4Box("TPCFB",            //its name
                         TPCOuterX,          //X half-length
                         TPCThick,           //Y half-length
                         TPCZ-2*TPCThick);     //Z half-length

  logicTPCFB = new G4LogicalVolume(solidTPCFB, //its solid
                                   TPC,        //its material
                                   "TPCFB");   //its name

  physiTPCFront = new G4PVPlacement(0,           //no rotation
                                    G4ThreeVector(0,              //X
                                                  TPCY-TPCThick,  //Y
                                                  0),             //Z
                                    logicTPCFB,  //its logical volume
                                    "TPCFront",  //its name
                                    logicHFE,    //its mother volume
                                    false,       //no boolean operation
                                    0);          //copy number

  physiTPCBack = new G4PVPlacement(0,           //no rotation
                                   G4ThreeVector(0,               //X
                                                 -TPCY+TPCThick,  //Y
                                                 0),              //Z
                                   logicTPCFB,  //its logical volume
                                   "TPCBack",   //its name
                                   logicHFE,    //its mother volume
                                   false,       //no boolean operation
                                   0);          //copy number

//Right & Left planes
  solidTPCRL = new G4Box("TPCRL",          //its name
                         TPCThick,         //X half-length
                         TPCY-2*TPCThick,             //Y half-length
                         TPCZ-2*TPCThick);            //Z half-length

  logicTPCRL = new G4LogicalVolume(solidTPCRL, //its solid
                                   TPC,        //its material
                                   "TPCRL");   //its name

  physiTPCOuterLeft = new G4PVPlacement(0,               //no rotation
                                        G4ThreeVector(-TPCOuterX+TPCThick, //X
                                                      0,                   //Y
                                                      0),                  //Z
                                        logicTPCRL,     //its logical volume
                                        "TPCOuterLeft", //its name
                                        logicHFE,       //its mother volume
                                        false,          //no boolean operation
                                        0);             //copy number

  physiTPCOuterRight = new G4PVPlacement(0,                //no rotation
                                         G4ThreeVector(TPCOuterX-TPCThick,  //X
                                                       0,                   //Y
                                                       0),                  //Z
                                         logicTPCRL,      //its logical volume
                                         "TPCOuterRight", //its name
                                         logicHFE,        //its mother volume
                                         false,           //no boolean operation
                                         0);              //copy number

  physiTPCInnerLeft = new G4PVPlacement(0,               //its logical volume
                                        G4ThreeVector(-TPCInnerX-TPCThick, //X
                                                      0,                   //Y
                                                      0),                  //Z
                                        logicTPCRL,     //its logical volume
                                        "TPCInnerLeft", //its name
                                        logicHFE,       //its mother volume
                                        false,          //no boolean operation
                                        0);             //copy number

  physiTPCInnerRight = new G4PVPlacement(0,               //no rotation
                                         G4ThreeVector(TPCInnerX+TPCThick,  //X
                                                       0,                   //Y
                                                       0),                  //Z
                                         logicTPCRL,     //its logical volume
                                         "TPCInnerRight",//its name
                                         logicHFE,       //its mother volume
                                         false,          //no boolean operation
                                         0);             //copy number

//**************************************************//
//********************** LXe ***********************//
//**************************************************//

  solidLXe1 = new G4Box("LXe1",                                //its name
                        TPCInnerX,                             //X half-length
                        TPCY-2*TPCThick,                       //Y half-length
                        TPCZ-2*TPCThick);                      //Z half-length

  logicLXe1 = new G4LogicalVolume(solidLXe1,  //its solid
                                  LXe,        //its material
                                  "LXe1");    //its name

  physiLXe1 = new G4PVPlacement(0,            //no rotation
                                G4ThreeVector(-2*TPCInnerX-2*TPCThick,            //X
                                              0,                                //Y
                                              0),                               //Z
                                logicLXe1,    //its logical volume
                                "LXe1",       //its name
                                logicHFE,     //its mother volume
                                false,        //no boolean operation
                                0);           //copy number

  physiLXe3 = new G4PVPlacement(0,            //no rotation
                                G4ThreeVector(2*TPCInnerX+2*TPCThick,               //X
                                              0,                                  //Y
                                              0),                                 //Z
                                logicLXe1,    //its logical volume
                                "LXe3",       //its name
                                logicHFE,     //its mother volume
                                false,        //no boolean operation
                                0);           //copy number

  solidLXe2 = new G4Box("LXe2",    //its name
                        TPCInnerX, //X half-length
                        TPCY,      //Y half-length
                        TPCZ);     //Z half-length

  logicLXe2 = new G4LogicalVolume(solidLXe2,  //its solid
                                  LXe,        //its material
                                  "LXe2");    //its name

  physiLXe2 = new G4PVPlacement(0,            //no rotation
                                G4ThreeVector(0,  //X
                                              0,  //Y
                                              0), //Z
                                logicLXe1,    //its logical volume
                                "LXe2",       //its name
                                logicHFE,     //its mother volume
                                false,        //no boolean operation
                                0);           //copy number

//**************************************************//
//***************** TEFLON LINING ******************//
//**************************************************//

// Cells 1 & 3 Lining

  solidTeflonTB1 = new G4Box("TeflonTB1",                          //its name
                             TPCInnerX,                            //X half-length
                             TPCY-2*TPCThick,                      //Y half-length
                             TeflonThick);                         //Z half-length

  logicTeflonTB1 = new G4LogicalVolume(solidTeflonTB1,  //its solid
                                       TeflonLining,    //its material
                                       "TeflonTB1");    //its name

  physiTeflonTop1 = new G4PVPlacement(0,                //no rotation
                                      G4ThreeVector(0,                            //X
                                                    0,                            //Y
                                                    TPCZ-2*TPCThick-TeflonThick), //Z
                                      logicTeflonTB1,   //its logical volume
                                      "TeflonTop1",     //its name
                                      logicLXe1,        //its mother volume
                                      false,            //no boolean operation
                                      0);               //copy number

  physiTeflonBot1 = new G4PVPlacement(0,               //no rotation
                                      G4ThreeVector(0,                               //X
                                                    0,                               //Y
                                                    -(TPCZ-2*TPCThick-TeflonThick)), //Z
                                      logicTeflonTB1,  //its logical volume
                                      "TeflonBot1",    //its name
                                      logicLXe1,       //its mother volume
                                      false,           //no boolean operation
                                      0);              //copy number

  solidTeflonRL1 = new G4Box("TeflonRL1",                        //its name
                             TeflonThick,                        //X half-length
                             TPCY-2*TPCThick,                    //Y half-length
                             TPCZ-2*TeflonThick-4*TeflonThick);  //Z half-length

  logicTeflonRL1 = new G4LogicalVolume(solidTeflonRL1,  //its solid
                                       TeflonLining,    //its material
                                       "TeflonRL1");    //its name

  physiTeflonRight1 = new G4PVPlacement(0,              //no rotation
                                        G4ThreeVector(TPCInnerX-TeflonThick,     //X
                                                      0,                         //Y
                                                      0),                        //Z
                                        logicTeflonRL1, //its logical volume
                                        "TeflonRight1", //its name
                                        logicLXe1,      //its mother volume
                                        false,          //no boolean operation
                                        0);             //copy number

  physiTeflonLeft1 = new G4PVPlacement(0,              //no rotation
                                       G4ThreeVector(-(TPCInnerX-TeflonThick),    //X
                                                     0,                           //Y
                                                     0),                          //Z
                                       logicTeflonRL1, //its logical volume
                                       "TeflonLeft1",  //its name
                                       logicLXe1,      //its mother volume
                                       false,          //no boolean operation
                                       0);             //copy number

  solidTeflonFB1 = new G4Box("TeflonFB1",                           //its name
                             TPCInnerX-2*TeflonThick,               //X half-length
                             TeflonThick,                           //Y half-length
                             TPCZ-2*TPCThick-2*TeflonThick);        //Z half-length

  logicTeflonFB1 = new G4LogicalVolume(solidTeflonFB1,  //its solid
                                       TeflonLining,    //its material
                                       "TeflonFB1");    //its name

  physiTeflonFront1 = new G4PVPlacement(0,               //no rotation
                                        G4ThreeVector(0,                            //X
                                                      TPCY-2*TPCThick-TeflonThick,  //Y
                                                      0),                           //Z
                                        logicTeflonFB1,  //its logical volume
                                        "TeflonFront1",  //its name
                                        logicLXe1,       //its mother volume
                                        false,           //no boolean operation
                                        0);              //copy number

  physiTeflonBack1 = new G4PVPlacement(0,               //no rotation
                                       G4ThreeVector(0,                               //X
                                                     -(TPCY-2*TPCThick-TeflonThick),  //Y
                                                     0),                              //Z
                                       logicTeflonFB1,  //its logical volume
                                       "TeflonBack1",   //its name
                                       logicLXe1,       //its mother volume
                                       false,           //no boolean operation
                                       0);              //copy number

//**************************************************//
//************ VISUALIZATION ATTRIBUTES ************//
//**************************************************//

// ATTRIBUTES OF THE WORLD
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

//ATTRIBUTES OF THE CONCRETE
  G4VisAttributes* ConcreteAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  ConcreteAtt->SetVisibility(true);
  logicMineTB->SetVisAttributes(ConcreteAtt);
  logicMineFB->SetVisAttributes(ConcreteAtt);
  logicMineRL->SetVisAttributes(ConcreteAtt);

//ATTRIBUTES OF THE STEEL
  G4VisAttributes* SteelAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  SteelAtt->SetVisibility(true);
  logicSSTankTB->SetVisAttributes(SteelAtt);
  logicSSTank->SetVisAttributes(SteelAtt);
  logicPbIBeam->SetVisAttributes(SteelAtt);
  logicHDPEIBeam->SetVisAttributes(SteelAtt);

//ATTRIBUTES OF THE WATER
  G4VisAttributes* WaterAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WaterAtt->SetVisibility(true);
  logicOuterWaterL->SetVisAttributes(WaterAtt);
  logicOuterWaterU->SetVisAttributes(WaterAtt);
  logicInnerWater->SetVisAttributes(WaterAtt);

//ATTRIBUTES OF THE POLY
  G4VisAttributes* PolyAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  PolyAtt->SetVisibility(true);
  logicHDPETank->SetVisAttributes(PolyAtt);
  logicHDPETankB->SetVisAttributes(PolyAtt);

//ATTRIBUTES OF THE LEAD
  G4VisAttributes* PbAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  PbAtt->SetVisibility(true);
  logicPbPlate->SetVisAttributes(PbAtt);

//ATTRIBUTES OF THE VACUUM
  G4VisAttributes* VacuumAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  VacuumAtt->SetVisibility(true);
  logicVacuumSpace->SetVisAttributes(VacuumAtt);

//ATTRIBUTES OF THE TITANIUM
  G4VisAttributes* TiAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
  TiAtt->SetVisibility(true);
  logicOuterTi->SetVisAttributes(TiAtt);
  logicOuterTiTB->SetVisAttributes(TiAtt);
  logicInnerTi->SetVisAttributes(TiAtt);
  logicInnerTiTB->SetVisAttributes(TiAtt);

//ATTRIBUTES OF THE LXE
  G4VisAttributes* LXeAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.5));
  LXeAtt->SetVisibility(true);
  logicLXe1->SetVisAttributes(LXeAtt);
  logicLXe2->SetVisAttributes(LXeAtt);

  //
  //always return the physical World
  //
  return physiWorld;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"

void EXOFullDetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
