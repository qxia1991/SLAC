#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4Trd.hh"       // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
#include "G4Torus.hh"     // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
#include "G4ExtrudedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"
#include "G4VisExtent.hh"
#include "G4UniformMagField.hh"
#include "G4SDManager.hh"
#include "G4Region.hh" 
#include "G4RegionStore.hh"
#include "G4EmCalculator.hh"

#include "EXOSim/EXOLXeSD.hh"
#include "EXOSim/EXOAPDSD.hh"
#include "EXOSim/EXOVetoPanelSD.hh"
#include "EXOSim/EXOPassiveMaterialSD.hh"
#include "EXOSim/EXOAPDParameterisation.hh"
#include "EXOSim/EXODetectorConstruction.hh"
#include "EXOUtilities/EXODimensions.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VTouchable.hh"

#include "G4MaterialTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4TwoVector.hh"
#include <vector>

// Added for M. Hughes' geometry
#include "G4NistManager.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....


EXODetectorConstruction::EXODetectorConstruction() 
 : fDetectorMessenger(this)
{

  defaultMaterial = 0;
  SaltMaterial = 0;
  AirMaterial = 0;
  ShieldMaterial = 0;
  OuterCryoMaterial = 0;
  VacuumMaterial = 0;
  InnerCryoMaterial = 0;
  HFEMaterial = 0;
  TPCCopperMaterial = 0;
  LXeMaterial = 0;
  APDMaterial = 0;
  TeflonMaterial = 0;
  HDPEMaterial = 0; // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
  StainlessMaterial = 0;
  EpoxyMaterial = 0;
  AcrylicMaterial = 0;
  InactiveLXeMaterial = 0;
  WireSupportMaterial = 0;
  VespelMaterial = 0;
  Polyethylene = 0;
  VetoSlabMaterial = 0;
  VetoScintMaterial = 0;
  BorGlassMaterial = 0;

  // Salt now included. 
  physiSalt = 0;

  SDreg = true;
  
  fInnerRegion = 0;

  // -------------------- TALK TO INITIAL CONDITIONS ---------------------------
  
  sourcePosNum = 0; // 0=not deployed
  // sourcePosXYZ = G4ThreeVector(); // does not matter unless sourcePosNum >0
  // Default source tube positions in TPC coordinates
  // These can be changed by EXODetectorConstruction::SetSourcePosition()
  S2_pos_TPC = G4ThreeVector(0,0,-29.48*cm);   // -z horizontal
  S5_pos_TPC = G4ThreeVector(25.4*cm,0,0);    // +x horizontal
  S8_pos_TPC = G4ThreeVector(0,0,29.48*cm);    // +z horizontal
  S11_pos_TPC = G4ThreeVector(0,25.5*cm,0);   // +y horizontal
  S14_pos_TPC = G4ThreeVector(0,0,-30.12*cm); // -z vertical
  S17_pos_TPC = G4ThreeVector(0,-25.5*cm,0);  // -y horizontal
  S20_pos_TPC = G4ThreeVector(0,0,30.12*cm); // +z vertical
  // NOTE: The tube at S8 really bends up through 108*deg and S20 bends to horizontal


  xenonComposition = xenonEnr;
  xenonPressure = 1.45*bar; // 1100 torr
  xenonDensity = 3.023*g/cm3;

  construction_verbose = false;
  fNavigator = new G4Navigator();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4VPhysicalVolume* EXODetectorConstruction::Construct()
{

  // materials
  if (!defaultMaterial) DefineMaterials();

  return ConstructCalorimeter();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void EXODetectorConstruction::DefineMaterials()
{ 
  //***********************************************************************//
  //****************************DEFINE ELEMENTS****************************//
  //***********************************************************************//

  G4Element* H  = new G4Element("Hydrogen","H" , 1.,  1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,"C" , 6.,  12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen","N" , 7.,  14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,"O" , 8.,  16.00*g/mole);
  G4Element* F  = new G4Element("Florine", "F" , 9.,  19.*g/mole);
  G4Element* Na = new G4Element("Sodium",  "Na", 11., 23.*g/mole);
  //G4Element* Al = new G4Element("Aluminum","Al", 13., 26.98*g/mole);
  G4Element* Si = new G4Element("Silicon", "Si", 14., 28.09*g/mole);
  G4Element* Cl = new G4Element("Chlorine","Cl", 17., 35.453*g/mole);
  G4Element* Cr = new G4Element("Chromium","Cr", 24., 51.996*g/mole);
  G4Element* Mn = new G4Element("Manganese","Mn", 25., 54.938*g/mole);
  G4Element* Ni = new G4Element("Nickel","Ni", 28., 58.693*g/mole);
  G4Element* Fe = new G4Element("Iron","Fe", 26., 55.845*g/mole);
  G4Element* Cu = new G4Element("Copper",  "Cu", 29., 63.546*g/mole);

  G4Isotope* iXe128 = new G4Isotope("Xe128", 54, 128, 128*g/mole);
  G4Isotope* iXe129 = new G4Isotope("Xe129", 54, 129, 129*g/mole);
  G4Isotope* iXe130 = new G4Isotope("Xe130", 54, 130, 130*g/mole);
  G4Isotope* iXe131 = new G4Isotope("Xe131", 54, 131, 131*g/mole);
  G4Isotope* iXe132 = new G4Isotope("Xe132", 54, 132, 132*g/mole);
  G4Isotope* iXe134 = new G4Isotope("Xe134", 54, 134, 134*g/mole);
  G4Isotope* iXe136 = new G4Isotope("Xe136", 54, 136, 136*g/mole);
                            
  G4Element* Xe = 0;
  switch (xenonComposition)
  {
  case xenonNat: // Natural Xe
    Xe = new G4Element("Xenon",   "Xe", 7);
    Xe->AddIsotope(iXe128,  1.93*perCent);
    Xe->AddIsotope(iXe129, 25.92*perCent);
    Xe->AddIsotope(iXe130,  4.04*perCent);
    Xe->AddIsotope(iXe131, 21.17*perCent);
    Xe->AddIsotope(iXe132, 27.10*perCent);
    Xe->AddIsotope(iXe134, 10.60*perCent);
    Xe->AddIsotope(iXe136,  9.24*perCent);
    break;

  case xenonEnr: // enriched Xe 136
    Xe = new G4Element("Xenon",   "Xe", 2);
    Xe->AddIsotope(iXe134, 19.4*perCent);
    Xe->AddIsotope(iXe136, 80.6*perCent);
    break;    

  case xenonDepl: // depleted Xe 136
    Xe = new G4Element("Xenon",   "Xe", 7);
    Xe->AddIsotope(iXe128,  2.0*perCent);
    Xe->AddIsotope(iXe129, 28.1*perCent);
    Xe->AddIsotope(iXe130,  4.4*perCent);
    Xe->AddIsotope(iXe131, 22.9*perCent);
    Xe->AddIsotope(iXe132, 29.5*perCent);
    Xe->AddIsotope(iXe134, 10.6*perCent);
    Xe->AddIsotope(iXe136,  2.5*perCent);
    break;

  default: 
    Xe = new G4Element("Xenon",   "Xe", 54., 131.29*g/mole);
  }

  ShieldMaterial = new G4Material("Lead", 82., 207.19*g/mole, 11.35*g/cm3);

  TPCCopperMaterial =
  new G4Material("TPCCopperMaterial", 8.96*g/cm3, 1, kStateSolid, 173.15*kelvin, 1*atmosphere);
  TPCCopperMaterial->AddElement(Cu, 1);

  OuterCryoMaterial =
  new G4Material("OuterCryoMaterial", 8.96*g/cm3, 1, kStateSolid, 293.15*kelvin, 1*atmosphere);
  OuterCryoMaterial->AddElement(Cu, 1);

  InnerCryoMaterial =
  new G4Material("InnerCryoMaterial", 8.96*g/cm3, 1, kStateSolid, 173.15*kelvin, 1*atmosphere);
  InnerCryoMaterial->AddElement(Cu, 1);

  LXeMaterial =
  new G4Material("LXeMaterial", xenonDensity, 1, kStateLiquid, 173.15*kelvin, xenonPressure);
  LXeMaterial->AddElement(Xe, 1);

  InactiveLXeMaterial =
  new G4Material("InactiveLXeMaterial", xenonDensity, 1, kStateLiquid, 173.15*kelvin, xenonPressure);
  InactiveLXeMaterial->AddElement(Xe, 1);

  // the following is HFE-7000 at -105C
  HFEMaterial =
  new G4Material("HFEMaterial", 1.77*g/cm3, 4, kStateLiquid, 173.15*kelvin, 1*atmosphere);
  HFEMaterial->AddElement(H, 3);
  HFEMaterial->AddElement(O, 1);
  HFEMaterial->AddElement(C, 4);
  HFEMaterial->AddElement(F, 7);

  TeflonMaterial =
  new G4Material("TeflonMaterial", 2.15*g/cm3, 3,kStateSolid, 173.15*kelvin);
  TeflonMaterial->AddElement(F, 10);
  TeflonMaterial->AddElement(O, 1);
  TeflonMaterial->AddElement(C, 5);

  // High density Polyethylene, geometry-upgrade (TPC Legs & High Voltage Feedthrough)
  HDPEMaterial =
  new G4Material("HDPEMaterial", 0.96*g/cm3, 2,kStateSolid, 173.15*kelvin);
  HDPEMaterial->AddElement(H, 2);
  HDPEMaterial->AddElement(C, 1);
    
  AcrylicMaterial =
  new G4Material("AcrylicMaterial", 1.185*g/cm3, 3,kStateSolid, 173.15*kelvin);
  AcrylicMaterial->AddElement(H, 8);
  AcrylicMaterial->AddElement(O, 2);
  AcrylicMaterial->AddElement(C, 5);

  APDMaterial =
  new G4Material("APDMaterial", 2.33*g/cm3, 1,kStateSolid, 173.15*kelvin);
  APDMaterial->AddElement(Si, 1);

  // Using 1% water content in the salt, per data from WIPP.
  SaltMaterial =
  new G4Material("SaltMaterial",2.1*g/cm3, 4, kStateSolid);
  SaltMaterial->AddElement(Na, 0.495);
  SaltMaterial->AddElement(Cl, 0.495);
  SaltMaterial->AddElement(O, 0.00333);  // added 1% water content in the salt
  SaltMaterial->AddElement(H, 0.00667);  // added 1% water content in the salt

  AirMaterial =
  new G4Material("AirMaterial",1.290*mg/cm3, 2, kStateGas);
    AirMaterial->AddElement(N, 0.7);
    AirMaterial->AddElement(O, 0.3);

  VacuumMaterial =
  new G4Material("VacuumMaterial", 1., 1.01*g/mole, universe_mean_density,
                kStateGas, 3.e-18*pascal, 2.73*kelvin);
                           
  StainlessMaterial = new G4Material("StainlessSteel", 8.03*g/cm3, 6, kStateSolid, 173.15*kelvin);
    StainlessMaterial->AddElement(C,  0.0003);
	StainlessMaterial->AddElement(Si, 0.0075);
	StainlessMaterial->AddElement(Cr, 0.19);
	StainlessMaterial->AddElement(Mn, 0.02);
	StainlessMaterial->AddElement(Ni, 0.10);
	StainlessMaterial->AddElement(Fe, 0.6822);
  
  //epoxy-resin , C11H12O3
  EpoxyMaterial = new G4Material("EpoxyResin", 1.268*g/cm3, 3, kStateSolid, 173.15*kelvin);
	EpoxyMaterial->AddElement(H, 12);
	EpoxyMaterial->AddElement(C, 11);
	EpoxyMaterial->AddElement(O, 3);
  
  VespelMaterial = new G4Material("Vespel", 1.45*g/cm3, 4, kStateSolid, 173.15*kelvin);
    VespelMaterial->AddElement(C, 22);
    VespelMaterial->AddElement(H, 10);
    VespelMaterial->AddElement(O, 5);
    VespelMaterial->AddElement(N, 2);

  //default materials of the World
  defaultMaterial  = VacuumMaterial;

  // Suppress output of materials table for M. Hughes' branch
  //G4cout << "Materials " << *G4Material::GetMaterialTable() << G4endl;


  //------------------------------------------------------------------------
  //-----------XENON SCINTILLATION AND OPTICAL PROPERTIES-------------------
  //------------------------------------------------------------------------

  const G4int nEntries = 3;  // number of photon energies

  // Scintillation spectrum: a delta function at 7 eV (177 nm).
  G4double PhotonEnergy[nEntries] = {6.5*eV, 7.*eV, 7.5*eV};
  G4double ScintilFast[nEntries] = {0.00, 1.00, 0.00};

  // Optical properties
  G4double RefractiveIndexXenon[nEntries] = {1.6, 1.6, 1.6};    // Refractive index
  G4double RayleighXenon[nEntries] = {30.*cm, 30.*cm, 30.*cm};  // Rayleigh scattering length
  G4double AbsorptionXenon[nEntries] = {300.*cm, 300.*cm, 300.*cm};      // Absorption length

  G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();
  myMPT1->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndexXenon,nEntries);
  myMPT1->AddProperty("ABSLENGTH",    PhotonEnergy, AbsorptionXenon,     nEntries);
  myMPT1->AddProperty("RAYLEIGH",     PhotonEnergy, RayleighXenon,     nEntries);
  myMPT1->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast,     nEntries);

  // A conservative estimate for the scintillation yeild
  // is 14,000 photons per MeV, although some measurements
  // suggest much higher numbers (40 - 70,000 photons/MeV).
  // A low number can be used for this to save time in
  // some simulations.
  
  myMPT1->AddConstProperty("SCINTILLATIONYIELD",0./MeV);     // scintillation yield
  myMPT1->AddConstProperty("RESOLUTIONSCALE",1.0);           // Fano factor
  myMPT1->AddConstProperty("FASTTIMECONSTANT", 2.*ns);       // fast time constant
  myMPT1->AddConstProperty("YIELDRATIO",1.);                 // ratio of fast scintillation
  
  LXeMaterial->SetMaterialPropertiesTable(myMPT1);

  //-------------------------------------------------------------------------
  //------------------------TEFLON OPTICAL PROPERTIES------------------------
  //-------------------------------------------------------------------------

  G4double RefractiveIndex3[nEntries] = { 1.35, 1.35, 1.35};
  G4double RaleighTeflon[nEntries] = { 0.1*mm, 0.1*mm, 0.1*mm};
  G4double AbsTeflon[nEntries] = {10.*cm, 10.*cm, 10.*cm};

  G4MaterialPropertiesTable* myMPT3 = new G4MaterialPropertiesTable();
  myMPT3->AddProperty("RAYLEIGH", PhotonEnergy, RaleighTeflon, nEntries);
  myMPT3->AddProperty("ABSLENGTH", PhotonEnergy, AbsTeflon, nEntries);
  myMPT3->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex3, nEntries);
  
  TeflonMaterial->SetMaterialPropertiesTable(myMPT3);

//==============================================================================
//  DEFINE VETO MATERIALS
//==============================================================================

  // Elements commented out are already defined above for calorimeter construction.
  G4Element* B  = new G4Element("Boron"   ,  "B",  5., 10.811*g/mole);
  G4Element* Al = new G4Element("Aluminum", "Al", 13., 26.981*g/mole);
  G4Element* K  = new G4Element("Potassium", "K", 19., 39.098*g/mole);
  // G4Element* Na = new G4Element("Sodium",  "Na", 11., 23.*g/mole);
  // G4Element* Si = new G4Element("Silicon", "Si", 14., 28.09*g/mole);
  // G4Element* O  = new G4Element("Oxygen"  ,"O" , 8.,  16.00*g/mole);
  // G4Element* H  = new G4Element("Hydrogen","H" , 1.,  1.01*g/mole);
  // G4Element* C  = new G4Element("Carbon"  ,"C" , 6.,  12.01*g/mole);

  // Borosilicate glass for 2" PMTs (based on Schott BK7 specs)
  BorGlassMaterial = new G4Material("BorosilicateGlass", 2.51*g/cm3, 6, kStateSolid);
      BorGlassMaterial->AddElement(B, 0.040064);
      BorGlassMaterial->AddElement(Al,0.011644);
      BorGlassMaterial->AddElement(K, 0.003321);
      BorGlassMaterial->AddElement(Na,0.028191);
      BorGlassMaterial->AddElement(Si,0.377220);
      BorGlassMaterial->AddElement(O, 0.539562);

  G4NistManager* NISTman = G4NistManager::Instance();
  //NISTman->SetVerbose(1);
	
  // Plastic scintillator BC-412 inside veto panels (Polyvinyl toluene = C_10H_11; "organic fluors")
  VetoScintMaterial = NISTman->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  

  // Borated polyethylene slab beneath scintillator layer of veto panels.
  Polyethylene = NISTman->FindOrBuildMaterial("G4_POLYETHYLENE");
  VetoSlabMaterial = new G4Material("VetoSlabMaterial", .951*g/cm3, 2, kStateSolid);
      VetoSlabMaterial->AddElement(B, .05);
      VetoSlabMaterial->AddMaterial(Polyethylene, .95);

  // Need metal materials for clean room?

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4VPhysicalVolume* EXODetectorConstruction::ConstructCalorimeter()
{

  //------------------------------------------------------------------------
  //----------------------------- ROTATIONS --------------------------------
  //------------------------------------------------------------------------
  cryostatRM.rotateY(90.*deg);
  cryostatRM.rotateZ(15.*deg);

  LeadShieldRM.rotateY(90.*deg);

  LXeVesselRM.rotateZ(-15.*deg);
  LXeVesselRM.rotateY(-90.*deg);

  // From HFE Refrence, Z points up
  SourceTube_verticalRM.rotateZ(-15.*deg);
  SourceTube_verticalRM.rotateX(90.*deg);


  //------------------------------------------------------------------------
  //------------------------- COMMON COLORS --------------------------------
  //------------------------------------------------------------------------
  G4Colour *CopperColor = new G4Colour(184./255.,115./255.,51./255,0.3);
  G4VisAttributes* CopperVisAtt= new G4VisAttributes(CopperColor);

  G4Colour *StainlessColor = new G4Colour(.69,.77,.87,0.5);
  G4VisAttributes* StainlessVisAtt= new G4VisAttributes(StainlessColor);

  G4Colour *AirColor = new G4Colour(255./255.,110./255.,199./255.,0.25); //Neon Pink
  G4VisAttributes* AirVisAtt= new G4VisAttributes(AirColor);

  G4Colour *HFEColor = new G4Colour(77./255,77./255.,255./255,0.25); //SlateBlue
  G4VisAttributes* HFEVisAtt= new G4VisAttributes(HFEColor);

  G4Colour *LeadColor = new G4Colour(.36,.36,.36,0.4); //Grey
  G4VisAttributes* LeadVisAtt= new G4VisAttributes(LeadColor);

  G4Colour *TeflonColor = new G4Colour(1,1,1,0.5); //White
  G4VisAttributes* TeflonVisAtt= new G4VisAttributes(TeflonColor);

  G4Colour *AcrylicColor = new G4Colour(.5,1.,.2,.6); //Grey
  G4VisAttributes* AcrylicVisAtt= new G4VisAttributes(AcrylicColor);

  G4Colour *SourceBeadColor = new G4Colour(1,0,0); //very red
  G4VisAttributes* SourceBeadVisAtt = new G4VisAttributes(SourceBeadColor);


  //-------------------------------------------------------------------------
  //---------------------------------WORLD-----------------------------------
  //-------------------------------------------------------------------------

  solidWorld = new G4Box("World",WORLDSIZEX,WORLDSIZEY,WORLDSIZEZ);
                         
  logicWorld = new G4LogicalVolume(solidWorld, defaultMaterial, "World");
  //logicWorld->SetVisAttributes(G4VisAttributes::Invisible); 
                                   
  physiWorld = new G4PVPlacement(0,			//no rotation
				G4ThreeVector(),	//at (0,0,0)
                                 logicWorld,		//its logical volume   
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number

  allphysvolumes.push_back(physiWorld);


  //-------------------------------------------------------------------------
  //------------------------------ SALT LAYER -------------------------------
  //-------------------------------------------------------------------------
  // Implemented by J. Wood.  Salt material contains 1% water content, per WIPP stratigraphy.

  // Reminder of salt dimensions, which are defined in EXODimensions:
  // SALTSIZEX = (DRIFTSIZEX + 3.*m);
  // SALTSIZEY = (DRIFTSIZEY + 4.*m);
  // SALTSIZEZ = (DRIFTSIZEZ + 3.*m);

  // Put these up here so that we can back-shift the salt in terms of variables,
  // in order to keep the TPC centered. Dimensions updated June 2014. (M. Hughes)
  const G4double CleanRm1Z = (2804./2)*mm;  // clean room N-S length
  const G4double CleanRm1X = (4597./2)*mm;  // clean room W-E length
  const G4double CleanRm1Y = (3013./2)*mm;  // clean room "height"; old value = (2798./2)*mm

  // Offsets below represent distances between TPC center and interior faces of clean
  // room walls.  Needed up here for salt shift.  From slide furnished by A. Pocar.
  const G4double TPCOffsetFromWest = 1319.97*mm;
  const G4double TPCOffsetFromSouth = 1263.18*mm;


  // Now start building the salt cavern.
  solidSalt = new G4Box("Salt", SALTSIZEX, SALTSIZEY, SALTSIZEZ);

  logicSalt = new G4LogicalVolume(solidSalt, SaltMaterial, "Salt");
  //logicSalt->SetVisAttributes(G4VisAttributes::Invisible);

  // Salt volume must be shifted, so as to restore TPC center to world center. Output of
  // 'point_test 0 0 0' determines offset here. Due to rotation of side lead shield and
  // daughter solids, output of point test in local coordinates should be translated:
  // Local(x,y,z)-->Global(z,y,-x) for applying shifts to salt. (M. Hughes)
  physiSalt = new G4PVPlacement(0,
 				  G4ThreeVector(-(DRIFTSIZEX - CleanRm1X - 1397.*mm) - (CleanRm1X - 101.6*mm - TPCOffsetFromWest),
								//851*mm - (-CleanRm1Y + LEADOUTERRAD), // 851*mm = -1*m salt shift + 139*mm clean room elevation
								-(-DRIFTSIZEY + CleanRm1Y + 1117.6*mm) - (-CleanRm1Y + (101.6*mm + 203.2*mm)/2. + LEADOUTERRAD) + 949.2*mm,
                                0*mm - (-CleanRm1Z + 101.6*mm + TPCOffsetFromSouth)),
								logicSalt,  //its logical volume
								"Salt",     //its name
								logicWorld, //its mother volume
								false,      //no boolean operation
								0);         //copy number

  allphysvolumes.push_back(physiSalt);  // added salt volume to vector


  //-------------------------------------------------------------------------
  //------ AIR (THIS HAS DRIFT INTERIOR DIMENSIONS) -------------------------
  //-------------------------------------------------------------------------

  solidAir = new G4Box("Air", DRIFTSIZEX, DRIFTSIZEY, DRIFTSIZEZ);
                         
  logicAir = new G4LogicalVolume(solidAir,	AirMaterial, "Air");
  //logicAir->SetVisAttributes(G4VisAttributes::Invisible);

  physiAir = new G4PVPlacement(0,			//no rotation
                 G4ThreeVector(0*m,
				               -1.0*m,      // translated 1m to make top salt 5m thick and bottom 3m thick
							   0*m),
                               logicAir,	//its logical volume
                               "Air",		//its name
                               logicSalt,	//its mother  volume
                               false,		//no boolean operation
                               0);			//copy number

  allphysvolumes.push_back(physiAir);


  //-------------------------------------------------------------------------
  //------------------------ CLEAN ROOM MODULE 1 ----------------------------
  //-------------------------------------------------------------------------
  // ADDED FROM MITCHELL'S CODES.

  // June 2014: Converting from single clean room volume to interior/exterior
  // shell pair, in order to fix offsets for TPC relative to veto panels. (M. Hughes)

  // From CRW clean room drawings, Mod 1 external dimensions should be (converted from in):
  //     N-S length ~ Z = 2803.525*mm
  //     W-E length ~ X = 4597.400*mm
  //     Height     ~ Y = 2765.425*mm --> 3013.075*mm (from V. Strickland; see below)
  // Specified height of 108.875" known to be wrong!  See http://exo-elog.stanford.edu/Veto/55
  // Using 118.625" height quoted by V. Strickland in a later email. (July 2014)

  // Shift clean room within drift for accurate spacing to West wall (55") and floor.
  // See measurements made in 02/2012 and 01/2014 by D. Auty:
  //     https://confluence.slac.stanford.edu/display/exo/Position+of+veto+panels
  //     https://confluence.slac.stanford.edu/display/exo/2014_01_17+veto+panels

  // July 2014: distance from ground now includes 20" for the red box-beam module support
  // structure (per L. Bartoszek drawing), which was implicitly included in D. Auty's
  // measurements from the ground.
  const G4ThreeVector CleanRm1Position(DRIFTSIZEX - CleanRm1X - 1397.*mm,   // Added 55" offset to West wall.
                                       //-DRIFTSIZEY + CleanRm1Y + 1.5*m,   // Equals 149*mm global offset. Sits 1.5m above the drift floor.
                                       -DRIFTSIZEY + CleanRm1Y + 1117.6*mm, // Now sits 24" (D. Auty 2014) + 20" (box beam) above the drift floor.
									   0.*mm); // Mod 1 isn't really centered in this dimension, but 10*m of drift tunnel in each direction should be overkill anyway.

  // Finally, start building Mod 1.
  solidCleanRm1Ext = new G4Box("CleanRm1", CleanRm1X, CleanRm1Y, CleanRm1Z);

  logicCleanRm1Ext = new G4LogicalVolume(solidCleanRm1Ext, AirMaterial, "CleanRm1Ext");

  physiCleanRm1Ext = new G4PVPlacement(0, CleanRm1Position, logicCleanRm1Ext,
                                       "CleanRm1Ext", logicAir, false, 0);

  // Per CRW drawings, let clean room wall and ceiling thickness be 4" (101.6*mm), floor thickness 8" (203.2*mm).
  solidCleanRm1Int = new G4Box("CleanRm1Int", CleanRm1X-101.6*mm,
                                              CleanRm1Y-(101.6*mm + 203.2*mm)/2.,
											  CleanRm1Z-101.6*mm);

  logicCleanRm1Int = new G4LogicalVolume(solidCleanRm1Int, AirMaterial, "CleanRm1Int");

  physiCleanRm1Int = new G4PVPlacement(0, G4ThreeVector(0.*mm,(101.6/2.)*mm,0.*mm), logicCleanRm1Int,
                                       "CleanRm1Int", logicCleanRm1Ext, false, 0);

  allphysvolumes.push_back(physiCleanRm1Ext);
  allphysvolumes.push_back(physiCleanRm1Int);  // added all volumes including Mitchell's to vector


  //-------------------------------------------------------------------------
  //----------------------------- VETO PANELS -------------------------------
  //-------------------------------------------------------------------------
  // ADDED FROM MITCHELL'S CODES.  THANKS, YOU'RE THE BESSST... (J. Wood)

  // Currently implemented without Al foil layer or black plastic wrapping;
  // bare slabs, scintillator panels, light guides, and PMTs only (M. Hughes).
  const G4double VetoPanelHalfXLong  = (3750./2)*mm;
  const G4double VetoPanelHalfXShort = (3150./2)*mm;
  const G4double VetoPanelHalfY  = (650./2)*mm;
  const G4double VetoPanelHalfZ  = (50./2)*mm;
  const G4double LightGuideHalfX = (100./2)*mm;
  const G4double LightGuideHalfZ = (60./2)*mm;

  // Define air-filled shells to contain the veto panel components, for both long
  // and short panels.  Add borated polyethylene base ("slab"), scintillator
  // slab "scint", light guides, PMTS w/ vacuum cavities.

  // Does not include extra layers: black plastic -> air gap -> Al foil on scint
  // -> air gap -> scint.  Do we need the Al foil at a minimum?

  // May 2014: shell geometry modified to contain only light guides and PMTs;
  // slabs and scintillators are now placed separately from the shells, so that
  // we can set unique copy numbers for the scintillators, which are now sensitive
  // volumes.
  solidVetoShellLong = new G4Box("VetoShellLong", VetoPanelHalfXLong,
                                 VetoPanelHalfY, LightGuideHalfZ);

  logicVetoShellLong = new G4LogicalVolume(solidVetoShellLong,
                                                  AirMaterial, "VetoShellLong");
  logicVetoShellLong->SetVisAttributes(G4VisAttributes::Invisible);  

  solidVetoShellShort = new G4Box("VetoShellShort", VetoPanelHalfXShort,
                                  VetoPanelHalfY, LightGuideHalfZ);

  logicVetoShellShort = new G4LogicalVolume(solidVetoShellShort,
                                            AirMaterial, "VetoShellShort");
  logicVetoShellShort->SetVisAttributes(G4VisAttributes::Invisible);  
  // Physical placements to be executed later across clean room faces (M. Hughes).


  // Define and place long and short base slabs:
  solidVetoSlabLong = new G4Box("VetoSlabLong", VetoPanelHalfXLong,
                                VetoPanelHalfY, VetoPanelHalfZ);

  logicVetoSlabLong = new G4LogicalVolume(solidVetoSlabLong,
                                          VetoSlabMaterial, "VetoSlabLong");

  solidVetoSlabShort = new G4Box("VetoSlabShort", VetoPanelHalfXShort,
                                 VetoPanelHalfY, VetoPanelHalfZ);

  logicVetoSlabShort = new G4LogicalVolume(solidVetoSlabShort,
                                           VetoSlabMaterial, "VetoSlabShort");
  // Physical placements to be executed later across clean room faces (M. Hughes).


  // Define and long and short scintillator volumes, including end bevels:
  solidVetoScintLong = new G4Trd("VetoScintLong",
            (VetoPanelHalfXLong - tan(12.*deg)*2*VetoPanelHalfZ),
            VetoPanelHalfXLong, VetoPanelHalfY, VetoPanelHalfY, VetoPanelHalfZ);

  logicVetoScintLong = new G4LogicalVolume(solidVetoScintLong,
                                           VetoScintMaterial, "VetoScintLong");

  solidVetoScintShort = new G4Trd("VetoScintShort",
           (VetoPanelHalfXShort - tan(12.*deg)*2*VetoPanelHalfZ),
           VetoPanelHalfXShort, VetoPanelHalfY, VetoPanelHalfY, VetoPanelHalfZ);

  logicVetoScintShort = new G4LogicalVolume(solidVetoScintShort,
                                            VetoScintMaterial, "VetoScintShort");

  // Sanity check on bevel implementations:
  /*
  G4cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ = " << G4endl;
  G4cout << "!!!!!!!! VetoPanelHalfXLong - tan(12.*deg)*2*VetoPanelHalfZ) = " << (VetoPanelHalfXLong - tan(12.*deg)*2*VetoPanelHalfZ) << G4endl;
  G4cout << "!!!!!!!! VetoPanelHalfXShort - tan(12.*deg)*2*VetoPanelHalfZ) = " << (VetoPanelHalfXShort - tan(12.*deg)*2*VetoPanelHalfZ) << G4endl;
  G4cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ = " << G4endl;
  */

  // Define and place light guides, including end bevels:
  vetoLightGuideRML.rotateX(90.*deg);
  vetoLightGuideRML.rotateZ(180.*deg);
  vetoLightGuideRMR.rotateX(-90.*deg);

  // Correct light guide placement for the odd way that the center is calculated for a trap
  // object.  By trial and error, add an offset equal to half of half the length difference
  // of the "top" and "bottom" faces, plus some tiny additional correction.  Now it's *perfect*.
  // vetoLightGuideOffset = (tan(7*deg)*LightGuideHalfZ)/2 + correction.
  //const G4double vetoLightGuideOffset = (3.06/2 + .31)*mm;
  const G4double vetoLightGuideOffset = (3.06/2 + .3115)*mm;

  solidVetoLightGuide = new G4Trap("VetoLightGuide", 2*(VetoPanelHalfY-1.*um), 2*(LightGuideHalfZ-1.*um), 
         2*LightGuideHalfX, (2*LightGuideHalfX - tan(7*deg)*2*LightGuideHalfZ));

  logicVetoLightGuide = new G4LogicalVolume(solidVetoLightGuide,
                                            VetoScintMaterial, "VetoLightGuide");

  physiVetoLightGuideLongL = new G4PVPlacement(&vetoLightGuideRML,
      G4ThreeVector((-VetoPanelHalfXLong + LightGuideHalfX + vetoLightGuideOffset),
	                0.,
					0.),  //2*VetoPanelHalfZ),
      logicVetoLightGuide, "VetoLightGuideL", logicVetoShellLong, false, 0);

  physiVetoLightGuideLongR = new G4PVPlacement(&vetoLightGuideRMR,
      G4ThreeVector((VetoPanelHalfXLong - LightGuideHalfX - vetoLightGuideOffset),
	                0.,
					0.),  //2*VetoPanelHalfZ),
      logicVetoLightGuide, "VetoLightGuideR", logicVetoShellLong, false, 0);

  physiVetoLightGuideShortL = new G4PVPlacement(&vetoLightGuideRML,
      G4ThreeVector((-VetoPanelHalfXShort + LightGuideHalfX + vetoLightGuideOffset),
	                0.,
					0.),  //2*VetoPanelHalfZ),
      logicVetoLightGuide, "VetoLightGuideL", logicVetoShellShort, false, 0);

  physiVetoLightGuideShortR = new G4PVPlacement(&vetoLightGuideRMR,
      G4ThreeVector((VetoPanelHalfXShort - LightGuideHalfX - vetoLightGuideOffset),
	                0.,
					0.),  //2*VetoPanelHalfZ),
      logicVetoLightGuide, "VetoLightGuideR", logicVetoShellShort, false, 0);

  allphysvolumes.push_back(physiVetoLightGuideLongL);
  allphysvolumes.push_back(physiVetoLightGuideLongR);
  allphysvolumes.push_back(physiVetoLightGuideShortL);
  allphysvolumes.push_back(physiVetoLightGuideShortR);  // added all volumes including Mitchell's to vector


  // Define and place 2" PMTs.  For now, use a simplified model comprising glass
  // cylinder as the PMT body, plus offset, cylindrical vacuum cavity inside.
  const G4double PMTBodyRad     = (51./2)*mm;
  const G4double PMTBodyHalfLen = (110./2)*mm;
  const G4double PMTCavityRad     = (46./2)*mm;
  const G4double PMTCavityHalfLen = (50./2)*mm;
  const G4double PMTWindowThickness = 2.*mm;

  solidPMTBody = new G4Tubs("PMTBody", 0., PMTBodyRad, PMTBodyHalfLen, 0.*deg, 360*deg);

  logicPMTBody = new G4LogicalVolume(solidPMTBody, BorGlassMaterial, "PMTBody");

  solidPMTCavity = new G4Tubs("PMTCavity", 0., PMTCavityRad, PMTCavityHalfLen, 0.*deg, 360*deg);

  logicPMTCavity = new G4LogicalVolume(solidPMTCavity, AirMaterial, "PMTCavity");

  physiPMTCavity = new G4PVPlacement(0,
      G4ThreeVector(0.,0.,(PMTBodyHalfLen - PMTCavityHalfLen - PMTWindowThickness)),
      logicPMTCavity, "PMTCavity", logicPMTBody, false, 0);

  allphysvolumes.push_back(physiPMTCavity);

  // Position 4 PMTs on each end of both the long and short panel shells.  PMTS shall
  // be numbered 1-4 from the lefthand side, as viewed from the top center of the
  // panel, looking down and towards either end.
  const G4double PMToffsets[] = {-214.*mm, -76.*mm, 76.*mm, 214.*mm};
  PMTBodyRML.rotateY(90.*deg);
  PMTBodyRMR.rotateY(-90.*deg);

  for (int i=0; i<4; i++) {
 	  char num[2];
	  sprintf(num, "%d", i + 1);

      physiPMTBodyLongL[i] = new G4PVPlacement(&PMTBodyRML,
          G4ThreeVector(-(VetoPanelHalfXLong - 2*LightGuideHalfX - PMTBodyHalfLen),
                        -(PMToffsets[i]),
						0.),  //2*VetoPanelHalfZ),
          logicPMTBody, ("PMT_L_" + G4String(num)), logicVetoShellLong, false, 0);

      physiPMTBodyLongR[i] = new G4PVPlacement(&PMTBodyRMR,
          G4ThreeVector((VetoPanelHalfXLong - 2*LightGuideHalfX - PMTBodyHalfLen),
                        (PMToffsets[i]),
						0.),  //2*VetoPanelHalfZ),
          logicPMTBody, ("PMT_R_" + G4String(num)), logicVetoShellLong, false, 0);

      physiPMTBodyShortL[i] = new G4PVPlacement(&PMTBodyRML,
          G4ThreeVector(-(VetoPanelHalfXShort - 2*LightGuideHalfX - PMTBodyHalfLen),
                        -(PMToffsets[i]),
						0.),  //2*VetoPanelHalfZ),
          logicPMTBody, ("PMT_L_" + G4String(num)), logicVetoShellShort, false, 0);

      physiPMTBodyShortR[i] = new G4PVPlacement(&PMTBodyRMR,
          G4ThreeVector((VetoPanelHalfXShort - 2*LightGuideHalfX - PMTBodyHalfLen),
                        (PMToffsets[i]),
						0.),  //2*VetoPanelHalfZ),
          logicPMTBody, ("PMT_R_" + G4String(num)), logicVetoShellShort, false, 0);

      allphysvolumes.push_back(physiPMTBodyLongL[i]);
      allphysvolumes.push_back(physiPMTBodyLongR[i]);
      allphysvolumes.push_back(physiPMTBodyShortL[i]);
      allphysvolumes.push_back(physiPMTBodyShortR[i]);  // added all volumes including Mitchell's to vector

  }


  // The long and short shells, which represent the tops of model panels, should
  // now be completely populated by constituent parts.  It remains to place the
  // long and short panel "shell" assemblies on the clean room faces.

  // June 2014: Now using bottom gap of 20" + 9", to account for red box-beam support
  // structure w/ D. Auty 2014 measurements.
  const G4double TopPanelGap= 671.*mm;
  const G4double BottomPanelGap = 736.6*mm; // Old value: 1008.*mm
  const G4double WestWallGap = 99.*mm;
  const G4double WestVerticalGap = 39.*mm;
  const G4double SouthWallGapTriple = 300.*mm;
  const G4double SouthWallGapDoubleHigh = 136.*mm;
  const G4double SouthWallGapDoubleLow = 34.*mm;

  // Reusable object for placing each bank of panel components, relative to the clean room exterior walls.
  G4ThreeVector PanelGroupShift;

  // Reusable object for placing shells with unique copy numbers.
  G4VPhysicalVolume *physiVetoShell;

  // May 2014: Rotate all panels about the simulation y-axis by 180 degree, in order to align them
  // properly with respect to the TPC legs.  Do this by modifying the rotation matrices (and add one
  // for the South wall).  For the net rotation of panel groups about the clean room, perform the
  // parity transformations of panel placement coordinates {x -> -x, z -> -z}. (M. Hughes)
  vetoTopBotRM.rotateZ(90.*deg);  // Fine as is.
  vetoTopBotRM.rotateY(90.*deg);  // Fine as is.
  vetoWestRM.rotateY(270.*deg);   // Formerly rotateY(90.*deg).
  vetoSouthRM.rotateX(180.*deg);  // This is new.
  

  // TO DO: Add real panel names (e.g. S14) as physical placement names. (M. Hughes)
  // Add 7 top panels first:
  for (int i=0; i<7; i++) {
 	  char num[2];
	  sprintf(num, "%d", i + 1);

	  PanelGroupShift = G4ThreeVector(-(-CleanRm1X + (2*i+1)*VetoPanelHalfY - 22.*mm),  // x -> -x
		                              (CleanRm1Y + (2*VetoPanelHalfZ + LightGuideHalfZ) + TopPanelGap),
	                                  -(CleanRm1Z - VetoPanelHalfXLong + 31.*mm));  // z -> -z

      physiVetoSlabLong = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
		                -(VetoPanelHalfZ + LightGuideHalfZ),
						0.),
          logicVetoSlabLong, ("VetoSlab_Top_" + G4String(num)), logicAir, false, i+1);

      physiVetoScintLong = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
		                (-LightGuideHalfZ + VetoPanelHalfZ),
						0.),
          logicVetoScintLong, ("VetoScint_Top_" + G4String(num)), logicAir, false, i+1);

      physiVetoShell = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
	      G4ThreeVector(0.,
		                2*VetoPanelHalfZ,  //(LightGuideHalfZ + VetoPanelHalfZ)),
                        0.),
		  logicVetoShellLong, ("VetoShell_Top_" + G4String(num)), logicAir, false, i+1);

      allphysvolumes.push_back(physiVetoSlabLong);
      allphysvolumes.push_back(physiVetoScintLong);
      allphysvolumes.push_back(physiVetoShell);  // added all volumes including Mitchell's to vector

  }

  // Now add 4 bottom panels:
  for (int i=0; i<4; i++) {
 	  char num[2];
	  sprintf(num, "%d", i + 1);

      // June 2014: bottom panels now flush with South wall plane, per "South-West"
      // measurement by D. Auty.
	  PanelGroupShift = G4ThreeVector(-(-CleanRm1X + (2*i+1)*VetoPanelHalfY + 820.*mm),  // x -> -x
		                              -(CleanRm1Y + (2*VetoPanelHalfZ + LightGuideHalfZ) + BottomPanelGap),
	                                  -(CleanRm1Z - VetoPanelHalfXLong));  // z -> -z
	                                  //-(CleanRm1Z - VetoPanelHalfXLong + 77.*mm));  // z -> -z

      physiVetoSlabLong = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
		                -(VetoPanelHalfZ + LightGuideHalfZ),
						0.),
          logicVetoSlabLong, ("VetoSlab_Bottom_" + G4String(num)), logicAir, false, i+8);

      physiVetoScintLong = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
		                (-LightGuideHalfZ + VetoPanelHalfZ),
						0.),
          logicVetoScintLong, ("VetoScint_Bottom_" + G4String(num)), logicAir, false, i+8);

      physiVetoShell = new G4PVPlacement(&vetoTopBotRM, CleanRm1Position + PanelGroupShift +
	      G4ThreeVector(0., 
                        2*VetoPanelHalfZ,
                        0.), 
		  logicVetoShellLong, ("VetoShell_Bottom_" + G4String(num)), logicAir, false, i+8);

      allphysvolumes.push_back(physiVetoSlabLong);
      allphysvolumes.push_back(physiVetoScintLong);
      allphysvolumes.push_back(physiVetoShell);  // added all volumes including Mitchell's to vector

  }

  // Now add 6 total panels on the west wall:
  for (int i=0; i<6; i++) {
 	  char num[2];
	  sprintf(num, "%d", i + 1);

      if (i<3) {
	    PanelGroupShift = G4ThreeVector((CleanRm1X + (2*VetoPanelHalfZ + LightGuideHalfZ) + WestWallGap),  // x -> -x
		                                (.5*WestVerticalGap + (5-2*i)*VetoPanelHalfY),
	                                    -(CleanRm1Z - VetoPanelHalfXShort + 20.*mm));  // z -> -z
      }
      else {
	    PanelGroupShift = G4ThreeVector((CleanRm1X + (2*VetoPanelHalfZ + LightGuideHalfZ) + WestWallGap),  // x -> -x
		                                -(.5*WestVerticalGap + (2*i-5)*VetoPanelHalfY),
	                                    -(CleanRm1Z - VetoPanelHalfXShort + 20.*mm));  // z -> -z
      }

      physiVetoSlabShort = new G4PVPlacement(&vetoWestRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(-(VetoPanelHalfZ + LightGuideHalfZ),  // x -> -x
			            0.,
	                    0.),
          logicVetoSlabShort, ("VetoSlab_West_" + G4String(num)), logicAir, false, i+12);

      physiVetoScintShort = new G4PVPlacement(&vetoWestRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(-(LightGuideHalfZ - VetoPanelHalfZ),  // x -> -x
			            0.,
						0.),
          logicVetoScintShort, ("VetoScint_West_" + G4String(num)), logicAir, false, i+12);

      physiVetoShell = new G4PVPlacement(&vetoWestRM, CleanRm1Position + PanelGroupShift +
		  G4ThreeVector(2*VetoPanelHalfZ,  // x -> -x
			            0.,
						0.),
          logicVetoShellShort, ("VetoShell_West_" + G4String(num)), logicAir, false, i+12);

      allphysvolumes.push_back(physiVetoSlabShort);
      allphysvolumes.push_back(physiVetoScintShort);
      allphysvolumes.push_back(physiVetoShell);  //  added all volumes including Mitchell's to vector

  }  // Done populating the west wall veto components.


  // Finally add 12 total panels on the south wall:
  for (int i=0; i<12; i++) {
      char num[3];
      sprintf(num, "%2.2d", i + 1);

      if (i<2) {
	    PanelGroupShift = G4ThreeVector(-(-CleanRm1X + VetoPanelHalfXShort - 206.*mm),  // x -> -x
		                                (-CleanRm1Y + (2*i+1)*VetoPanelHalfY - 407.*mm),
	                                    -(CleanRm1Z + (2*VetoPanelHalfZ + LightGuideHalfZ) + SouthWallGapDoubleLow));  // z -> -z
      }
      else if (i>=2 && i<4) {
 	    PanelGroupShift = G4ThreeVector(-(-CleanRm1X + VetoPanelHalfXShort + 52.*mm),  // x -> -x
		                                (-CleanRm1Y + (2*i+1)*VetoPanelHalfY - 407.*mm + 41.*mm),
	                                    -(CleanRm1Z + (2*VetoPanelHalfZ + LightGuideHalfZ) + SouthWallGapDoubleHigh));  // z -> -z
      }
      else if (i>=4 && i<6) {
	    PanelGroupShift = G4ThreeVector(-(-CleanRm1X + VetoPanelHalfXShort + 52.*mm),  // x -> -x
		                                (-CleanRm1Y + (2*i+1)*VetoPanelHalfY - 407.*mm + 41.*mm + 52.*mm),
	                                    -(CleanRm1Z + (2*VetoPanelHalfZ + LightGuideHalfZ) + SouthWallGapDoubleHigh));  // z -> -z
      }
      else if (i>=6 && i<9) {
	    PanelGroupShift = G4ThreeVector(-(CleanRm1X - VetoPanelHalfXShort - 36.*mm),  // x -> -x
		                                (-CleanRm1Y + (2*(i-6)+1)*VetoPanelHalfY - 362.*mm),
	                                    -(CleanRm1Z + (2*VetoPanelHalfZ + LightGuideHalfZ) + SouthWallGapTriple));  // z -> -z
      }
      else {
	    PanelGroupShift = G4ThreeVector(-(CleanRm1X - VetoPanelHalfXShort - 36.*mm), // (CleanRm1X - VetoPanelHalfXShort + 169.*mm),  // x -> -x
		                                (-CleanRm1Y + (2*(i-6)+1)*VetoPanelHalfY - 362.*mm + 39.*mm), 
	                                    -(CleanRm1Z + (2*VetoPanelHalfZ + LightGuideHalfZ) + SouthWallGapTriple));  // z -> -z
      }


      physiVetoSlabShort = new G4PVPlacement(&vetoSouthRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
			            0.,
	                    (VetoPanelHalfZ + LightGuideHalfZ)),  // z -> -z
          logicVetoSlabShort, ("VetoSlab_South_" + G4String(num)), logicAir, false, i+18);

      physiVetoScintShort = new G4PVPlacement(&vetoSouthRM, CleanRm1Position + PanelGroupShift +
          G4ThreeVector(0.,
			            0.,
						-(-LightGuideHalfZ + VetoPanelHalfZ)),  // z -> -z
          logicVetoScintShort, ("VetoScint_South_" + G4String(num)), logicAir, false, i+18);

      physiVetoShell = new G4PVPlacement(&vetoSouthRM, CleanRm1Position + PanelGroupShift +
		  G4ThreeVector(0.,
			            0.,
						-(2*VetoPanelHalfZ)),  // z -> -z
          logicVetoShellShort, ("VetoShell_South_" + G4String(num)), logicAir, false, i+18);

      allphysvolumes.push_back(physiVetoSlabShort);
      allphysvolumes.push_back(physiVetoScintShort);
      allphysvolumes.push_back(physiVetoShell);  // added all volumes including Mitchell's to vector

  }  // Done populating the south wall veto components.


  //-------------------------------------------------------------------------
  //----------------------- OUTER LEAD SHIELD WALL --------------------------
  //-------------------------------------------------------------------------

  // used to shift all the lead shield volumes for the TPC to be at the origin in the global system of coordinates
  const G4double leadShieldXPosition = -(VACUUM_GAP_THICKNESS_FRONT-VACUUM_GAP_THICKNESS_REAR)/2.
                                 -(AIR_GAP_THICKNESS_FRONT-AIR_GAP_THICKNESS_REAR)/2.;

  std::vector<G4TwoVector> outerShieldVertices;
  outerShieldVertices.push_back( G4TwoVector( OUTERLEADWALLTHICKNESS/2.,-OUTERLEADWALLHEIGHT1) ); 
  outerShieldVertices.push_back( G4TwoVector(-OUTERLEADWALLTHICKNESS/2.,-OUTERLEADWALLHEIGHT1) );
  outerShieldVertices.push_back( G4TwoVector(-OUTERLEADWALLTHICKNESS/2., OUTERLEADWALLHEIGHT1) );
  outerShieldVertices.push_back( G4TwoVector(                     0.*cm, OUTERLEADWALLHEIGHT2) );
  outerShieldVertices.push_back( G4TwoVector( OUTERLEADWALLTHICKNESS/2., OUTERLEADWALLHEIGHT1) );

  std::vector<G4ExtrudedSolid::ZSection> outerShieldZSection;
  outerShieldZSection.push_back( G4ExtrudedSolid::ZSection(-OUTERLEADWALLLENGTH, G4TwoVector(0.,0.), 1.0) );
  outerShieldZSection.push_back( G4ExtrudedSolid::ZSection( OUTERLEADWALLLENGTH, G4TwoVector(0.,0.), 1.0) );

  G4ExtrudedSolid* solidOuterShield = new G4ExtrudedSolid("OuterShield", outerShieldVertices, outerShieldZSection );

  G4LogicalVolume* logicOuterShield = new G4LogicalVolume( solidOuterShield, ShieldMaterial, "OuterShield");
  //logicShield->SetVisAttributes (G4VisAttributes::Invisible);
  logicOuterShield->SetVisAttributes(LeadVisAtt); 

  physiOuterShield = new G4PVPlacement(0,
                         G4ThreeVector(leadShieldXPosition-(LEADHEIGHT+LEADTHICKNESSFRONTANDREAR+INNER_OUTER_WALL_DISTANCE+OUTERLEADWALLTHICKNESS/2.) + (CleanRm1X - 101.6*mm - TPCOffsetFromWest),
                                       -(LEADOUTERRAD-OUTERLEADWALLHEIGHT1) + (-CleanRm1Y  + (101.6*mm + 203.2*mm)/2. + LEADOUTERRAD),
							           (-CleanRm1Z + 101.6*mm + TPCOffsetFromSouth)),
		                               logicOuterShield,	   //its logical volume
		                               "OuterShield",	   //its name
                                       logicCleanRm1Int,  // mother volume changed since CleanRm1 added
		                               false,	           //no boolean operation
		                               0);		   //copy number 
  allphysvolumes.push_back(physiOuterShield);

  //-------------------------------------------------------------------------
  //------------------------------LEAD SHIELD--------------------------------
  //-------------------------------------------------------------------------
  // Global coordinate system: Lead shield should run along the y

  // Front lead wall
  std::vector<G4TwoVector> frontShieldVertices;
  frontShieldVertices.push_back( G4TwoVector(-105.0*cm, -LEADOUTERRAD ) );
  frontShieldVertices.push_back( G4TwoVector(-105.0*cm, -LEADOUTERRAD+180.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( -84.0*cm, -LEADOUTERRAD+180.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( -84.0*cm, -LEADOUTERRAD+200.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( -63.0*cm, -LEADOUTERRAD+200.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( -63.0*cm, -LEADOUTERRAD+220.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector(  63.0*cm, -LEADOUTERRAD+220.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector(  63.0*cm, -LEADOUTERRAD+200.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector(  84.0*cm, -LEADOUTERRAD+200.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector(  84.0*cm, -LEADOUTERRAD+180.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( 105.0*cm, -LEADOUTERRAD+180.0*cm ) );
  frontShieldVertices.push_back( G4TwoVector( 105.0*cm, -LEADOUTERRAD ) ); 

  std::vector<G4ExtrudedSolid::ZSection> frontShieldZSection;
  frontShieldZSection.push_back( G4ExtrudedSolid::ZSection(-LEADTHICKNESSFRONTANDREAR/2., G4TwoVector(0.,0.), 1.0) );
  frontShieldZSection.push_back( G4ExtrudedSolid::ZSection( LEADTHICKNESSFRONTANDREAR/2. - LEADFRONTSHAVEOFF, G4TwoVector(0.,0.), 1.0) );

  G4ExtrudedSolid* solidFrontShield = new G4ExtrudedSolid("FrontLeadShield", frontShieldVertices, frontShieldZSection );

  G4LogicalVolume* logicFrontShield = new G4LogicalVolume(solidFrontShield,	ShieldMaterial,	"FrontLeadShield");
  logicFrontShield->SetVisAttributes(LeadVisAtt);

  physiFrontShield = new G4PVPlacement(&LeadShieldRM,
                         G4ThreeVector(leadShieldXPosition-LEADHEIGHT-LEADTHICKNESSFRONTANDREAR/2.-1.0*um + (CleanRm1X - 101.6*mm - TPCOffsetFromWest),
						               (-CleanRm1Y + (101.6*mm + 203.2*mm)/2. + LEADOUTERRAD),
									   (-CleanRm1Z + 101.6*mm + TPCOffsetFromSouth)),
                                       logicFrontShield,
				                       "FrontLeadShield",
                                       logicCleanRm1Int,  // mother volume changed since CleanRm1 added
				                       false,
									   0);
  allphysvolumes.push_back(physiFrontShield);
   

  // Rear lead wall
  std::vector<G4TwoVector> rearShieldVertices;
  rearShieldVertices.push_back( G4TwoVector(-108.55*cm, -LEADOUTERRAD ) );
  rearShieldVertices.push_back( G4TwoVector(-108.55*cm, -LEADOUTERRAD+172.8*cm ) );
  rearShieldVertices.push_back( G4TwoVector( -86.0*cm,  -LEADOUTERRAD+172.8*cm ) );
  rearShieldVertices.push_back( G4TwoVector( -86.0*cm,  -LEADOUTERRAD+216.0*cm ) );
  rearShieldVertices.push_back( G4TwoVector(  86.0*cm,  -LEADOUTERRAD+216.0*cm ) );
  rearShieldVertices.push_back( G4TwoVector(  86.0*cm,  -LEADOUTERRAD+172.8*cm ) );
  rearShieldVertices.push_back( G4TwoVector( 108.55*cm, -LEADOUTERRAD+172.8*cm ) );
  rearShieldVertices.push_back( G4TwoVector( 108.55*cm, -LEADOUTERRAD ) ); 

  std::vector<G4ExtrudedSolid::ZSection> rearShieldZSection;
  rearShieldZSection.push_back( G4ExtrudedSolid::ZSection(-LEADTHICKNESSFRONTANDREAR/2., G4TwoVector(0.,0.), 1.0) );
  rearShieldZSection.push_back( G4ExtrudedSolid::ZSection( LEADTHICKNESSFRONTANDREAR/2., G4TwoVector(0.,0.), 1.0) );

  G4ExtrudedSolid* solidRearShield = new G4ExtrudedSolid("RearLeadShield", rearShieldVertices, rearShieldZSection );     

  G4LogicalVolume* logicRearShield = new G4LogicalVolume(solidRearShield, ShieldMaterial, "RearLeadShield");
  logicRearShield->SetVisAttributes(LeadVisAtt);

  physiRearShield = new G4PVPlacement(&LeadShieldRM,
                        G4ThreeVector(leadShieldXPosition+LEADHEIGHT+LEADTHICKNESSFRONTANDREAR/2.+1.0*um + (CleanRm1X - 101.6*mm - TPCOffsetFromWest),
						              (-CleanRm1Y + (101.6*mm + 203.2*mm)/2. + LEADOUTERRAD),
									  (-CleanRm1Z + 101.6*mm + TPCOffsetFromSouth)),
                                      logicRearShield,
				                      "RearLeadShield",
                                      logicCleanRm1Int,  // mother volume changed since CleanRm1 added
				                      false,
									  0);
  allphysvolumes.push_back(physiRearShield);                                                                                          


  // Side walls
  //used for defining polygon planes
  const G4double LeadRadOuter[]={LEADOUTERRAD,LEADOUTERRAD};
  const G4double LeadRadInner[]={0.0,0.0};
  const G4double LeadZPlane[]={-LEADHEIGHT,LEADHEIGHT};
  G4Polyhedra* solidSideShield = new G4Polyhedra("SideLeadShield",0.*deg,360.*deg,LEADSIDES,2,LeadZPlane,LeadRadInner,LeadRadOuter);

  /*
  // adding the side and front shields together
  G4UnionSolid* solidShield_temp = new G4UnionSolid("LeadShield_temp", solidSideShield, solidFrontShield, 0, G4ThreeVector(0.,0.,LEADHEIGHT+LEADTHICKNESSFRONTANDREAR/2.));
  // adding the rear shield
  solidShield = new G4UnionSolid("LeadShield", solidShield_temp, solidRearShield, 0, G4ThreeVector(0.,0.,-LEADHEIGHT-LEADTHICKNESSFRONTANDREAR/2.));
  */

  G4LogicalVolume* logicSideShield = new G4LogicalVolume(solidSideShield,	//its solid
                                                         ShieldMaterial,	//its material
                                                         "SideLeadShield");	//its name
  logicSideShield->SetVisAttributes(LeadVisAtt);

  physiSideShield = new G4PVPlacement(&cryostatRM,
                        G4ThreeVector(leadShieldXPosition + (CleanRm1X - 101.6*mm - TPCOffsetFromWest),
						              (-CleanRm1Y + (101.6*mm + 203.2*mm)/2. + LEADOUTERRAD),
									  (-CleanRm1Z + 101.6*mm + TPCOffsetFromSouth)),
                                      logicSideShield,
				                      "SideLeadShield",
                                      logicCleanRm1Int,  // mother volume changed since CleanRm1 added
				                      false,
									  0);
  allphysvolumes.push_back(physiSideShield);
  
  //-------------------------------------------------------------------------
  //--------------------------------AIR GAP----------------------------------
  //-------------------------------------------------------------------------

  // need to figure out where the outer cryostat lip starts in the lead coordinates
  const G4double LeadLipHeight = (CUCRY1HEIGHT-CUCRY1LIPDEPTH - 0.3*cm) - (AIR_GAP_THICKNESS_FRONT-AIR_GAP_THICKNESS_REAR)/2.;

  //used for defining polygon planes
  const G4double AirGapRadOuter[]={LEADINNERRAD,LEADINNERRAD,LEADLIPINNERRAD,LEADLIPINNERRAD};
  const G4double AirGapRadInner[]={0.0,0.0,0.0,0.0};
  const G4double AirGapZPlane[]={-LEADHEIGHT,LeadLipHeight,LeadLipHeight,LEADHEIGHT};
  G4Polyhedra* solidAirGap = new G4Polyhedra("AirGap",90.*deg,360.*deg,LEADSIDES,4,AirGapZPlane,AirGapRadInner,AirGapRadOuter);

  G4LogicalVolume* logicAirGap = new G4LogicalVolume(solidAirGap, AirMaterial, "AirGap");
  logicAirGap->SetVisAttributes(AirColor);

  physiAirGap = new G4PVPlacement(0,
                    G4ThreeVector(0,0,0),
				                  logicAirGap,	   //its logical volume
				                  "AirGap",	   //its name
				                  logicSideShield,	   //its mother volume
				                  false,	   //no boolean operation
				                  0);		   //copy number 	       
  allphysvolumes.push_back(physiAirGap);
  
  //-------------------------------------------------------------------------
  //---------------------------OUTER CRYOSTAT--------------------------------
  //-------------------------------------------------------------------------

  //used for defining polygon planes
  const G4double OuterCryoRadOuter[]={CUCRY1OUTERRAD,CUCRY1OUTERRAD,CUCRY1LIPOUTERRAD,CUCRY1LIPOUTERRAD};
  const G4double OuterCryoRadInner[]={0.*cm,0.*cm,0*cm,0*cm};
  const G4double OuterCryoZPlane[]={-CUCRY1HEIGHT,CUCRY1HEIGHT-CUCRY1LIPDEPTH,CUCRY1HEIGHT-CUCRY1LIPDEPTH,CUCRY1HEIGHT};

  // the main solid
  G4Polyhedra* solidOuterCryoMain = new G4Polyhedra("OuterCryoMain",90.*deg,360.*deg,LEADSIDES,4,
          OuterCryoZPlane,OuterCryoRadInner,OuterCryoRadOuter);
  // the outer cryostat door
  G4Tubs* solidOuterCryoDoor = new G4Tubs("OuterCryoDoor",0,CUCRY1DOORRAD,CUCRY1DOORTHICKNESS,0.*deg,360.*deg);

  // union of the main solid with the door
  G4UnionSolid* solidOuterCryo = new G4UnionSolid("OuterCryo", solidOuterCryoMain, solidOuterCryoDoor, 0, G4ThreeVector(0.,0.,CUCRY1HEIGHT));


  G4LogicalVolume* logicOuterCryo = new G4LogicalVolume(solidOuterCryo,     //its solid
          OuterCryoMaterial,  //its material
				  "OuterCryo");       //its name
  //G4VisAttributes* CopperVisAtt= new G4VisAttributes(CopperColor);
  logicOuterCryo->SetVisAttributes (CopperVisAtt);

  physiOuterCryo = new G4PVPlacement(0,
                       G4ThreeVector(0.,
					                 0.,
					                 -(AIR_GAP_THICKNESS_FRONT-AIR_GAP_THICKNESS_REAR)/2.),
				                     logicOuterCryo,	   //its logical volume
				                     "OuterCryo",	   //its name
				                     logicAirGap,	   //its mother volume
				                     false,	   //no boolean operation
				                     0);		   //copy number 	       

  allphysvolumes.push_back(physiOuterCryo);
  
  //-------------------------------------------------------------------------
  //---------------------------INSULATING VACUUM-----------------------------
  //-------------------------------------------------------------------------
     
  //used for defining polygon planes
  const G4double VacuumGapRadOuter[]={CUCRY1INNERRAD,CUCRY1INNERRAD};
  const G4double VacuumGapRadInner[]={0.*cm,0.*cm};
  const G4double VacuumGapZPlane[]={-1.0*(CUCRY1HEIGHT-CUCRY1THICKNESS),CUCRY1HEIGHT-CUCRY1THICKNESS};

  // the main solid
  G4Polyhedra* solidVacuumGapMain = new G4Polyhedra("VacuumGapMain",90.*deg,360.*deg,LEADSIDES,2,
				   VacuumGapZPlane,VacuumGapRadInner,VacuumGapRadOuter);

  // the outer cryostat reinforcing disk
  G4Tubs* solidOuterCryoDisk = new G4Tubs("OuterCryoDisk",0,CUCRY1DISKRAD,CUCRY1DISKTHICKNESS,0.*deg,360.*deg);

  // the outer cryostat hole behind the door
  G4Tubs* solidOuterCryoHole = new G4Tubs("OuterCryoHole",0,CUCRY1HOLERAD,CUCRY1THICKNESS,0.*deg,360.*deg);
  
  // subtracting the reinforcing disk from the main solid
  G4SubtractionSolid* solidVacuumGap_temp = new G4SubtractionSolid("VacuumGap-disk", solidVacuumGapMain, solidOuterCryoDisk, 0,
          G4ThreeVector(0.,0.,-CUCRY1HEIGHT+CUCRY1THICKNESS));

  // union of the main solid with the hole behind the cryostat door
  G4UnionSolid* solidVacuumGap = new G4UnionSolid("VacuumGap", solidVacuumGap_temp, solidOuterCryoHole, 0,
          G4ThreeVector(0.,0.,CUCRY1HEIGHT-CUCRY1THICKNESS));


  G4LogicalVolume* logicVacuumGap = new G4LogicalVolume(solidVacuumGap,   //its solid
				  VacuumMaterial,   //its material
				  "VacuumGap");     //its name
  //  logicVacuumGap->SetVisAttributes (CopperVisAtt);
  logicVacuumGap->SetVisAttributes (G4VisAttributes::Invisible); 

  physiVacuumGap = new G4PVPlacement(0,G4ThreeVector(0,0,0),
				  logicVacuumGap,	   //its logical volume
				  "VacuumGap",	   //its name
				  logicOuterCryo,  //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number 	       

  allphysvolumes.push_back(physiVacuumGap);
  
  //-------------------------------------------------------------------------
  //---------------------------INNER CRYOSTAT--------------------------------
  //-------------------------------------------------------------------------

  //used for defining polygon planes
  const G4double InnerCryoRadOuter[]={CUCRY2OUTERRAD,CUCRY2OUTERRAD};
  const G4double InnerCryoRadInner[]={0.*cm,0.*cm};
  const G4double InnerCryoZPlane[]={-CUCRY2HEIGHT,CUCRY2HEIGHT};

  // the main solid
  G4Polyhedra* solidInnerCryoMain = new G4Polyhedra("InnerCryo",90.*deg,360.*deg,LEADSIDES,2,
          InnerCryoZPlane,InnerCryoRadInner,InnerCryoRadOuter);

  // the inner cryostat door
  G4Tubs* solidInnerCryoDoor = new G4Tubs("InnerCryoDoor",0,CUCRY2DOORRAD,CUCRY2DOORTHICKNESS,0.*deg,360.*deg);

  // union of the main solid with the door
  G4UnionSolid* solidInnerCryo1 = new G4UnionSolid("InnerCryo", solidInnerCryoMain, solidInnerCryoDoor, 0, G4ThreeVector(0.,0.,CUCRY2HEIGHT));

  // Create heat exchanger
  G4Box * heatExTemplate1 = new G4Box("HeatExchanger1", HEATEXWIDTH, HEATEXTHICKNESS, HEATEXHEIGHT);
  G4Box * heatExTemplate2 = new G4Box("HeatExchanger2", HEATEXWIDTH, HEATEXTHICKNESS, HEATEXHEIGHT);
  G4Box * heatExTemplate3 = new G4Box("HeatExchanger3", HEATEXWIDTH, HEATEXTHICKNESS, HEATEXHEIGHT);

  // merge 3 heat exchangers into the inner cryo!
  G4RotationMatrix rm_heatex1;
  rm_heatex1.rotateZ(45*deg);
  G4RotationMatrix rm_heatex2;
  rm_heatex2.rotateZ(-75*deg);
  G4RotationMatrix rm_heatex3;
  rm_heatex3.rotateZ(-15*deg);
  G4UnionSolid * solidInnerCryo2 = new G4UnionSolid("InnerCryo", solidInnerCryo1, heatExTemplate1, &rm_heatex1, 
                                                    G4ThreeVector(HEATEXOFFSET*sin(45*deg), HEATEXOFFSET*cos(45*deg), 0));
  G4UnionSolid * solidInnerCryo3 = new G4UnionSolid("InnerCryo", solidInnerCryo2, heatExTemplate2, &rm_heatex2, 
                                                    G4ThreeVector(HEATEXOFFSET*sin(-75*deg), HEATEXOFFSET*cos(-75*deg), 0));
  G4UnionSolid * solidInnerCryo = new G4UnionSolid("InnerCryo", solidInnerCryo3, heatExTemplate3, &rm_heatex3, 
                                                    G4ThreeVector(HEATEXOFFSET*sin(-15*deg), HEATEXOFFSET*cos(-15*deg), 0));

    			     
  G4LogicalVolume* logicInnerCryo = new G4LogicalVolume(solidInnerCryo,     //its solid
          InnerCryoMaterial,  //its material
				  "InnerCryo");	      //its name
  logicInnerCryo->SetVisAttributes(CopperVisAtt);

  physiInnerCryo = new G4PVPlacement(0,G4ThreeVector(0,0,-(VACUUM_GAP_THICKNESS_FRONT-VACUUM_GAP_THICKNESS_REAR)/2.),
				  logicInnerCryo,	   //its logical volume
				  "InnerCryo",	     //its name
				  logicVacuumGap,    //its mother volume
				  false,	           //no boolean operation
				  0);		             //copy number 	       

  allphysvolumes.push_back(physiInnerCryo);
  
  //-------------------------------------------------------------------------
  //----------------------------------HFE------------------------------------
  //-------------------------------------------------------------------------

  const G4double HFERadOuter[]={CUCRY2INNERRAD,CUCRY2INNERRAD};
  const G4double HFERadInner[]={0.*cm,0.*cm};
  const G4double HFEZPlane[]={-1.0*(CUCRY2HEIGHT-CUCRY2THICKNESS),CUCRY2HEIGHT-CUCRY2THICKNESS};

  // the main solid
  G4Polyhedra* solidHFEMain = new G4Polyhedra("HFEMain",90.*deg,360.*deg,LEADSIDES,2,
			     HFEZPlane,HFERadInner,HFERadOuter);

  // the inner cryostat hole behind the door
  G4Tubs* solidInnerCryoHole = new G4Tubs("InnerCryoHole",0,CUCRY2HOLERAD,CUCRY2THICKNESS,0.*deg,360.*deg);
  
  // union of the main solid with the hole behind the inner cryostat door
  G4UnionSolid* solidHFE = new G4UnionSolid("HFE", solidHFEMain, solidInnerCryoHole, 0, G4ThreeVector(0.,0.,CUCRY2HEIGHT-CUCRY2THICKNESS));

  G4LogicalVolume* logicHFE = new G4LogicalVolume(solidHFE, HFEMaterial, "HFE");

  logicHFE->SetVisAttributes(HFEVisAtt);

  physiHFE = new G4PVPlacement(0,
                               G4ThreeVector(),
                               logicHFE,
                               "HFE",
                               logicInnerCryo,
                               false,0);

  allphysvolumes.push_back(physiHFE);
  
  //-------------------------------------------------------------------------
  //---------------------------------SOURCE TUBE-----------------------------
  //-------------------------------------------------------------------------

  G4Tubs* solidSourceTubeS2 = new G4Tubs("SourceTubeS2",0,SOURCE_TUBE_OUTER_RADIUS, //tube at -z runs horizontal
				23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS5 = new G4Tubs("SourceTubeS5",0,SOURCE_TUBE_OUTER_RADIUS, //tube at +x runs horizontal
				27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS8 = new G4Tubs("SourceTubeS8",0,SOURCE_TUBE_OUTER_RADIUS, //tube at +z runs horizontal
				23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS11 = new G4Tubs("SourceTubeS11",0,SOURCE_TUBE_OUTER_RADIUS, //Tube at +y horizontal
        27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS14 = new G4Tubs("SourceTubeS14",0,SOURCE_TUBE_OUTER_RADIUS, //Tube at +z runs vertical
        23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS17 = new G4Tubs("SourceTubeS17",0,SOURCE_TUBE_OUTER_RADIUS, //Tube at -y runs horizontal
        27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeS20 = new G4Tubs("SourceTubeS20",0,SOURCE_TUBE_OUTER_RADIUS, //Tube at +z runs vertical
        23.*cm, 0.*deg,360.*deg);
  
  G4LogicalVolume* logicSourceTubeS2 = new G4LogicalVolume(solidSourceTubeS2,
                  TPCCopperMaterial,          //material
				  "SourceTubeS2");	        //its name
  G4LogicalVolume* logicSourceTubeS5 = new G4LogicalVolume(solidSourceTubeS5,
				  TPCCopperMaterial,          //material
				  "SourceTubeS5");	        //its name
  G4LogicalVolume* logicSourceTubeS8 = new G4LogicalVolume(solidSourceTubeS8,
				  TPCCopperMaterial,          //material
				  "SourceTubeS8");	        //its name
  G4LogicalVolume* logicSourceTubeS11 = new G4LogicalVolume(solidSourceTubeS11,
				  TPCCopperMaterial,          //material
				  "SourceTubeS11");	        //its name
  G4LogicalVolume* logicSourceTubeS14 = new G4LogicalVolume(solidSourceTubeS14,
				  TPCCopperMaterial,          //material
				  "SourceTubeS14");	        //its name
  G4LogicalVolume* logicSourceTubeS17 = new G4LogicalVolume(solidSourceTubeS17,
				  TPCCopperMaterial,          //material
				  "SourceTubeS17");	        //its name
  G4LogicalVolume* logicSourceTubeS20 = new G4LogicalVolume(solidSourceTubeS20,
				  TPCCopperMaterial,          //material
				  "SourceTubeS20");	        //its name

  logicSourceTubeS2->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS5->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS8->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS11->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS14->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS17->SetVisAttributes(CopperVisAtt);
  logicSourceTubeS20->SetVisAttributes(CopperVisAtt);
  
  // By now the default tube positions are manipulated by /exosim/SourcePosition if necessary
  // Now we need to translate to HFE coordinate system
  // HFE Z points away from WarmDoor, and Y is 15 degrees from up
  double z_rot = -15*deg;
  double y_rot = -90*deg;

  // basically X=Z, Y = cos(15)*Y-sin(15)*X, Z=cos(15)*X+sin(15)*Y
  G4ThreeVector S2_pos_HFE = G4ThreeVector(S2_pos_TPC.getZ()*cos(z_rot) + S2_pos_TPC.getY()*sin(z_rot), //X
          S2_pos_TPC.getY()*cos(z_rot) - S2_pos_TPC.getZ()*sin(z_rot), //Y
          -1.*S2_pos_TPC.getX()); //Z
          
  G4ThreeVector S5_pos_HFE = G4ThreeVector(S5_pos_TPC.getZ()*cos(z_rot) + S5_pos_TPC.getY()*sin(z_rot),
          S5_pos_TPC.getY()*cos(z_rot) - S5_pos_TPC.getZ()*sin(z_rot),
          -1.*S5_pos_TPC.getX());
  
  G4ThreeVector S8_pos_HFE = G4ThreeVector(S8_pos_TPC.getZ()*cos(z_rot) + S8_pos_TPC.getY()*sin(z_rot),
          S8_pos_TPC.getY()*cos(z_rot) - S8_pos_TPC.getZ()*sin(z_rot),
          -1.*S8_pos_TPC.getX());
  
  G4ThreeVector S11_pos_HFE = G4ThreeVector(S11_pos_TPC.getZ()*cos(z_rot) + S11_pos_TPC.getY()*sin(z_rot),
          S11_pos_TPC.getY()*cos(z_rot) - S11_pos_TPC.getZ()*sin(z_rot),
          -1.*S11_pos_TPC.getX());
          
  G4ThreeVector S14_pos_HFE = G4ThreeVector(S14_pos_TPC.getZ()*cos(z_rot) + S14_pos_TPC.getY()*sin(z_rot),
          S14_pos_TPC.getY()*cos(z_rot) - S14_pos_TPC.getZ()*sin(z_rot),
          -1.*S14_pos_TPC.getX());
  
  G4ThreeVector S17_pos_HFE = G4ThreeVector(S17_pos_TPC.getZ()*cos(z_rot) + S17_pos_TPC.getY()*sin(z_rot),
          S17_pos_TPC.getY()*cos(z_rot) - S17_pos_TPC.getZ()*sin(z_rot),
          -1.*S17_pos_TPC.getX());
  
  G4ThreeVector S20_pos_HFE = G4ThreeVector(S20_pos_TPC.getZ()*cos(z_rot) + S20_pos_TPC.getY()*sin(z_rot),
          S20_pos_TPC.getY()*cos(z_rot) - S20_pos_TPC.getZ()*sin(z_rot),
          -1.*S20_pos_TPC.getX());
  
  // NOTE: The z-direction of these isn't important conceptually, just get the tube along the correct axis
  physiSourceTubeS2 = new G4PVPlacement(0,S2_pos_HFE,
				  logicSourceTubeS2,	//its logical volume
				  "SourceTubeS2",	      //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
          
  physiSourceTubeS5 = new G4PVPlacement(&LXeVesselRM,S5_pos_HFE,
				  logicSourceTubeS5, 	//its logical volume
				  "SourceTubeS5",       //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
  
  physiSourceTubeS8 = new G4PVPlacement(0,S8_pos_HFE,
				  logicSourceTubeS8,	//its logical volume
				  "SourceTubeS8",	      //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number

  physiSourceTubeS11 = new G4PVPlacement(&LXeVesselRM,S11_pos_HFE,
				  logicSourceTubeS11,	//its logical volume
				  "SourceTubeS11",	    //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
          
  physiSourceTubeS14 = new G4PVPlacement(&SourceTube_verticalRM,S14_pos_HFE,
				  logicSourceTubeS14,	//its logical volume
				  "SourceTubeS14",	    //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
  
  physiSourceTubeS17 = new G4PVPlacement(&LXeVesselRM,S17_pos_HFE,
				  logicSourceTubeS17,	//its logical volume
				  "SourceTubeS17",	    //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
  
  physiSourceTubeS20 = new G4PVPlacement(&SourceTube_verticalRM,S20_pos_HFE,
				  logicSourceTubeS20,	//its logical volume
				  "SourceTubeS20",	    //its name
				  logicHFE,             //its mother volume
				  false,	              //no boolean operation
				  0);		                //copy number
  
  allphysvolumes.push_back(physiSourceTubeS2);
  allphysvolumes.push_back(physiSourceTubeS5);
  allphysvolumes.push_back(physiSourceTubeS8);
  allphysvolumes.push_back(physiSourceTubeS11);
  allphysvolumes.push_back(physiSourceTubeS14);
  allphysvolumes.push_back(physiSourceTubeS17);
  allphysvolumes.push_back(physiSourceTubeS20); 
  
  //------------------------------INSIDE SOURCE TUBE-------------------------

  G4Tubs* solidSourceTubeInS2 = new G4Tubs("SourceTubeInS2",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at +x
        23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS5 = new G4Tubs("SourceTubeInS5",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at +y
				27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS8 = new G4Tubs("SourceTubeInS8",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at -y
        23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS11 = new G4Tubs("SourceTubeInS11",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at -y
				27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS14 = new G4Tubs("SourceTubeInS14",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at -y
				23.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS17 = new G4Tubs("SourceTubeInS17",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at -y
				27.*cm, 0.*deg,360.*deg);
  G4Tubs* solidSourceTubeInS20 = new G4Tubs("SourceTubeInS20",0.*cm,SOURCE_TUBE_INNER_RADIUS, //tube at -y
				23.*cm, 0.*deg,360.*deg);
          
  G4LogicalVolume* logicSourceTubeInS2 = new G4LogicalVolume(solidSourceTubeInS2,    //its solid
				  AirMaterial,            //material
          "SourceTubeInS2");	  //its name
  G4LogicalVolume* logicSourceTubeInS5 = new G4LogicalVolume(solidSourceTubeInS5,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS5");	  //its name
  G4LogicalVolume* logicSourceTubeInS8 = new G4LogicalVolume(solidSourceTubeInS8,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS8");	  //its name
  G4LogicalVolume* logicSourceTubeInS11 = new G4LogicalVolume(solidSourceTubeInS11,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS11");	  //its name
  G4LogicalVolume* logicSourceTubeInS14 = new G4LogicalVolume(solidSourceTubeInS14,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS14");	  //its name
  G4LogicalVolume* logicSourceTubeInS17 = new G4LogicalVolume(solidSourceTubeInS17,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS17");	  //its name
  G4LogicalVolume* logicSourceTubeInS20 = new G4LogicalVolume(solidSourceTubeInS20,    //its solid
				  AirMaterial,            //material
				  "SourceTubeInS20");	  //its name
          
  logicSourceTubeInS2->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS5->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS8->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS11->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS14->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS17->SetVisAttributes(AirVisAtt);
  logicSourceTubeInS20->SetVisAttributes(AirVisAtt);

  physiSourceTubeInS2 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS2, //its logial volume            
				  "SourceTubeInS2",	  //its name
				  logicSourceTubeS2,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS5 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS5, //its logical volume
				  "SourceTubeInS5",	  //its name
				  logicSourceTubeS5,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS8 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS8, //its logical volume
				  "SourceTubeInS8",	  //its name
				  logicSourceTubeS8,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS11 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS11, //its logical volume
				  "SourceTubeInS11",	  //its name
				  logicSourceTubeS11,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS14 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS14, //its logical volume
				  "SourceTubeInS14",	  //its name
				  logicSourceTubeS14,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS17 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS17, //its logical volume
				  "SourceTubeInS17",	  //its name
				  logicSourceTubeS17,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
  physiSourceTubeInS20 = new G4PVPlacement(0,G4ThreeVector(),
				  logicSourceTubeInS20, //its logical volume
				  "SourceTubeInS20",	  //its name
				  logicSourceTubeS20,    //its mother volume
				  false,                  //no boolean operation
				  0);                     //copy number
          
  allphysvolumes.push_back(physiSourceTubeInS2);
  allphysvolumes.push_back(physiSourceTubeInS5);
  allphysvolumes.push_back(physiSourceTubeInS8);
  allphysvolumes.push_back(physiSourceTubeInS11);
  allphysvolumes.push_back(physiSourceTubeInS14);
  allphysvolumes.push_back(physiSourceTubeInS17);
  allphysvolumes.push_back(physiSourceTubeInS20);
  
  //------------------------- Source Capsule -------------------------------
  // This is only used if the source is deployed, you deploy the source w/ talkto /exosim/SourcePosition
  if (sourcePosNum > 0)
  {
    G4Tubs* solidSourceCapsule = new G4Tubs("SourceCapsule",0.*cm,SOURCE_CAPSULE_RADIUS,SOURCE_CAPSULE_LENGTH,0,360.*deg);
    G4LogicalVolume* logicSourceCapsule = new G4LogicalVolume(solidSourceCapsule, StainlessMaterial, "SourceCapsule");
    logicSourceCapsule->SetVisAttributes(StainlessVisAtt);
    
    // The source capsule is 0.581" long welded
    //     source is at =0.236+(0.055/2)/tan(118/2)+0.02+0.086-R inches from the cable end
    //     sourceID = 0.055"
    //     it's in a vespel container
    if ( sourcePosNum == 2 )
    {
      physiSourceCapsule = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceCapsule,"SourceCapsule",logicSourceTubeInS2,false,0);
    }
    else if ( sourcePosNum == 5 )
    {
      physiSourceCapsule = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceCapsule,"SourceCapsule",logicSourceTubeInS5,false,0);
    }
    else if ( sourcePosNum == 8 )
    {
      physiSourceCapsule = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceCapsule,"SourceCapsule",logicSourceTubeInS8,false,0);
    }
    else if ( sourcePosNum == 11 )
    {
      physiSourceCapsule = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceCapsule,"SourceCapsule",logicSourceTubeInS11,false,0);
    }
    
    // --------------------------- SourceInnerCapsule ---------------------------------
    // Fill in inner diameter of the source capsule and plug w/ vespel, 0.02" + 0.110" + 0.078" total length
    G4Tubs* solidSourceInnerCapsule = new G4Tubs("SourceInnerCapsule",0.*cm,.070*cm,0.264*cm,0,360.*deg);
    G4LogicalVolume* logicSourceInnerCapsule = new G4LogicalVolume(solidSourceInnerCapsule, VespelMaterial,"SourceInnerCapsule");
    logicSourceInnerCapsule->SetVisAttributes(SourceBeadVisAtt);

    // The Vespel capsule starts from -1*0.581"/2 + .236" + 0.055"/2/tan(60*deg) = -0.096cm from center of capsule + it's half length
    physiSourceInnerCapsule = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceInnerCapsule,"SourceInnerCapsule",
        logicSourceCapsule,false,0);
    
  
    // ------------------------------- SourceBead -------------------------------------
    G4Sphere* solidSourceBead = new G4Sphere("SourceBead",0,SOURCE_BEAD_RADIUS,0,360*deg,0,180*deg);
    G4LogicalVolume* logicSourceBead = new G4LogicalVolume(solidSourceBead, EpoxyMaterial,"SourceBead");
    logicSourceBead->SetVisAttributes(SourceBeadVisAtt);
    
    // Position is -1*.3937 + 0.02" + 0.086 - R
    physiSourceBead = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicSourceBead,"SourceBead",logicSourceInnerCapsule,false,0);
     
    allphysvolumes.push_back(physiSourceCapsule);
    allphysvolumes.push_back(physiSourceBead);
  }   
    
  //------------------------------------------------------------------------
  //---------------------------------LXE VESSEL-----------------------------
  //------------------------------------------------------------------------

  //Definition of the can with weld rings
  const G4int numRZ = 34;
  G4double rmaxVessel[numRZ];
  G4double rminVessel[numRZ];
  G4double zplanesVessel[numRZ];
  G4int iz = 0;
  zplanesVessel[iz]=-53.96446/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.7330*cm; iz+=1;
  zplanesVessel[iz]=-53.30406/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.7330*cm; iz+=1;
  zplanesVessel[iz]=-53.30406/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.5679*cm; iz+=1;
  zplanesVessel[iz]=-48.88446/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.5679*cm; iz+=1;
  zplanesVessel[iz]=-48.88446/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.4790*cm; iz+=1;
  zplanesVessel[iz]=-44.84078/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=22.4790*cm; iz+=1;
  zplanesVessel[iz]=-44.84078/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-40.10406/2.*cm;  rmaxVessel[iz]=22.86000*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-40.10406/2.*cm;  rmaxVessel[iz]=23.01240*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-37.36086/2.*cm;  rmaxVessel[iz]=23.01240*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-33.46499/2.*cm;  rmaxVessel[iz]=20.09140*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-30.92958/2.*cm;  rmaxVessel[iz]=20.09140*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-30.92958/2.*cm;  rmaxVessel[iz]=19.94916*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-11.37920/2.*cm;  rmaxVessel[iz]=19.94916*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-11.37920/2.*cm;  rmaxVessel[iz]=20.09140*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-10.10920/2.*cm;  rmaxVessel[iz]=20.09140*cm;    rminVessel[iz]=0*cm;       iz+=1;
  zplanesVessel[iz]=-10.10920/2.*cm;  rmaxVessel[iz]=19.94916*cm;    rminVessel[iz]=0*cm;       iz+=1;
  //Assume symmetric (mirror across xy plane)
  for (int i=0;i<numRZ/2;i++)
  {
    zplanesVessel[numRZ/2+i] = -1*zplanesVessel[(numRZ/2)-1-i];
    rmaxVessel[numRZ/2+i] = rmaxVessel[(numRZ/2)-1-i];
    rminVessel[numRZ/2+i] = rminVessel[(numRZ/2)-1-i];
  }

  G4Polycone* solidLXeVesselShell = new G4Polycone("LXeVesselShell",0.*deg,360.*deg,numRZ,
				  zplanesVessel,rminVessel,rmaxVessel);

  // LETS MAKE CAPS AND ADD THEM TO THE VESSEL SHELL
  // INNER PART BETWEEN CAP FACE AND FIRST ISO SUPPORT I call a "PIE"
  const G4double CATH2CAPDIST = 44.51566/2.0*cm; //To the inside of the cap (LXe side)
  const G4double LXEVESSELENDTHICKNESS = 0.16256*cm; //Thickness of can holding in LXe
  const G4double CAPPIETHICKNESS = 2.02184*cm; //How thick the PIE spokes are
  const G4double CAPPIEZMID = CATH2CAPDIST + LXEVESSELENDTHICKNESS + CAPPIETHICKNESS/2.; //This is Z position to place stuff
  const G4double CAPLEGX = 22.4*cm; //A little over length to merge fully w/ shell.
  const G4double CAPLEGY = 0.1524*cm; //How wide the spokes are
  
  //LINE 1
  G4Box* solidCapLegs = new G4Box("CapLegThings",CAPLEGX, CAPLEGY/2., CAPPIETHICKNESS/2.);
  //LINE 2
  G4RotationMatrix CapRotZ1; CapRotZ1.rotateZ(1.*180.0/6.*deg);
  G4UnionSolid* solidCapLegs2 = new G4UnionSolid("CapLeg 1", solidCapLegs, solidCapLegs, &CapRotZ1, G4ThreeVector());
  //LINE 3
  G4RotationMatrix CapRotZ2; CapRotZ2.rotateZ(2.*180.0/6.*deg);
  G4UnionSolid* solidCapLegs3 = new G4UnionSolid("CapLeg 2", solidCapLegs2, solidCapLegs, &CapRotZ2, G4ThreeVector());
  //LINE 4
  G4RotationMatrix CapRotZ3; CapRotZ3.rotateZ(3.*180.0/6.*deg);
  G4UnionSolid* solidCapLegs4 = new G4UnionSolid("CapLeg 3", solidCapLegs3, solidCapLegs, &CapRotZ3, G4ThreeVector());
  //LINE 5
  G4RotationMatrix CapRotZ4; CapRotZ4.rotateZ(4.*180.0/6.*deg);
  G4UnionSolid* solidCapLegs5 = new G4UnionSolid("CapLeg 4", solidCapLegs4, solidCapLegs, &CapRotZ4, G4ThreeVector());
  //LINE 6
  G4RotationMatrix CapRotZ5; CapRotZ5.rotateZ(5.*180.0/6.*deg);
  G4UnionSolid* solidCapLegs6 = new G4UnionSolid("CapLeg 5", solidCapLegs5, solidCapLegs, &CapRotZ5, G4ThreeVector());
  
  // Add plug to center
  G4Tubs* solidCapCenterPlug = new G4Tubs("LXeVesselCapPlug",0*cm,4.0761*cm,CAPPIETHICKNESS/2.,0.*deg,360.*deg);
  G4UnionSolid* solidCapplusPlug = new G4UnionSolid("Cap+CenterCyl", solidCapLegs6, solidCapCenterPlug, 0, G4ThreeVector());
  
  //Then subtract smaller plug from center to make ring and remove legs in center
  G4Tubs* solidCapCenterHole = new G4Tubs("LXeVesselCapHole",0*cm,6.096/2.0*cm,CAPPIETHICKNESS/2.,0.*deg,360.*deg);
  G4SubtractionSolid* solidCapRing = new G4SubtractionSolid("Cap with ring", solidCapplusPlug, solidCapCenterHole, 0, G4ThreeVector());
  // Add Fillits
  const G4double CAPFILLITX = 4.076858*cm;
  const G4double CAPFILLITR = (tan(180./12.*deg)*4.076858)*cm-CAPLEGY/2.;
  G4Tubs* solidCapFillit = new G4Tubs("Fillits between spokes",0, CAPFILLITR, CAPPIETHICKNESS/2., 0, 360.*deg);
  G4SubtractionSolid* solidCapminusFillit1 = new G4SubtractionSolid("CapRing-Fillit1",solidCapRing         , solidCapFillit, 0, G4ThreeVector(cos(0.5*180./6.*deg)*CAPFILLITX, sin(0.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit2 = new G4SubtractionSolid("CapRing-Fillit2", solidCapminusFillit1, solidCapFillit, 0, G4ThreeVector(cos(1.5*180./6.*deg)*CAPFILLITX, sin(1.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit3 = new G4SubtractionSolid("CapRing-Fillit3", solidCapminusFillit2, solidCapFillit, 0, G4ThreeVector(cos(2.5*180./6.*deg)*CAPFILLITX, sin(2.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit4 = new G4SubtractionSolid("CapRing-Fillit4", solidCapminusFillit3, solidCapFillit, 0, G4ThreeVector(cos(3.5*180./6.*deg)*CAPFILLITX, sin(3.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit5 = new G4SubtractionSolid("CapRing-Fillit5", solidCapminusFillit4, solidCapFillit, 0, G4ThreeVector(cos(4.5*180./6.*deg)*CAPFILLITX, sin(4.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit6 = new G4SubtractionSolid("CapRing-Fillit6", solidCapminusFillit5, solidCapFillit, 0, G4ThreeVector(cos(5.5*180./6.*deg)*CAPFILLITX, sin(5.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit7 = new G4SubtractionSolid("CapRing-Fillit7", solidCapminusFillit6, solidCapFillit, 0, G4ThreeVector(cos(6.5*180./6.*deg)*CAPFILLITX, sin(6.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit8 = new G4SubtractionSolid("CapRing-Fillit8", solidCapminusFillit7, solidCapFillit, 0, G4ThreeVector(cos(7.5*180./6.*deg)*CAPFILLITX, sin(7.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit9 = new G4SubtractionSolid("CapRing-Fillit9", solidCapminusFillit8, solidCapFillit, 0, G4ThreeVector(cos(8.5*180./6.*deg)*CAPFILLITX, sin(8.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit10 = new G4SubtractionSolid("CapRing-Fillit10", solidCapminusFillit9, solidCapFillit, 0, G4ThreeVector(cos(9.5*180./6.*deg)*CAPFILLITX, sin(9.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit11 = new G4SubtractionSolid("CapRing-Fillit11", solidCapminusFillit10, solidCapFillit, 0, G4ThreeVector(cos(10.5*180./6.*deg)*CAPFILLITX, sin(10.5*180./6.*deg)*CAPFILLITX, 0));
  G4SubtractionSolid* solidCapminusFillit12 = new G4SubtractionSolid("CapRing-Fillit12", solidCapminusFillit11, solidCapFillit, 0, G4ThreeVector(cos(11.5*180./6.*deg)*CAPFILLITX, sin(11.5*180./6.*deg)*CAPFILLITX, 0));

  //Add Caps to LXeVesselShell
  G4UnionSolid* solidLXeVesselCap1of2 = new G4UnionSolid("LXeV with PZ cap", solidLXeVesselShell, solidCapminusFillit12, 0, G4ThreeVector(0,0,CAPPIEZMID));
  G4UnionSolid* solidLXeVesselCap2of2 = new G4UnionSolid("LXeV with PZ cap", solidLXeVesselCap1of2, solidCapminusFillit12, 0, G4ThreeVector(0,0,-1*CAPPIEZMID));
  
  //ISOGRID LAYER is between outermost star and CAP structure
  const G4double CAPISOTHICKNESS = 0.254*cm;
  const G4double CAPISOMIDZ = CATH2CAPDIST + LXEVESSELENDTHICKNESS + CAPPIETHICKNESS + CAPISOTHICKNESS/2;
  const G4double CAPISOX = 22.4*cm; //A little over length (shoudln't matter?)
  const G4double CAPISOY = 0.635*cm;
  const G4double CAPISOIR = 5.6134/2.*cm;
  const G4double CAPISOOR = 4.25458*cm;

  //The Legs
  G4Box* solidIsoLeg = new G4Box("IsoLegThings",CAPISOX, CAPISOY/2., CAPISOTHICKNESS/2.);
  G4UnionSolid* solidIsoLeg2 = new G4UnionSolid("IsoLeg 2", solidIsoLeg, solidIsoLeg, &CapRotZ1, G4ThreeVector());
  G4UnionSolid* solidIsoLeg3 = new G4UnionSolid("IsoLeg 3", solidIsoLeg2, solidIsoLeg, &CapRotZ2, G4ThreeVector());
  G4UnionSolid* solidIsoLeg4 = new G4UnionSolid("IsoLeg 4", solidIsoLeg3, solidIsoLeg, &CapRotZ3, G4ThreeVector());
  G4UnionSolid* solidIsoLeg5 = new G4UnionSolid("IsoLeg 5", solidIsoLeg4, solidIsoLeg, &CapRotZ4, G4ThreeVector()); // stops working here
  G4UnionSolid* solidIsoLeg6 = new G4UnionSolid("IsoLeg 6", solidIsoLeg5, solidIsoLeg, &CapRotZ5, G4ThreeVector());
  
  // Add plug to center
  G4Tubs* solidIsoCenterPlug = new G4Tubs("LXeVesselIsoPlug",0*cm,CAPISOOR,CAPISOTHICKNESS/2.,0.*deg,360.*deg);
  G4UnionSolid* solidIsoplusPlug = new G4UnionSolid("Iso+CenterCyl", solidIsoLeg6, solidIsoCenterPlug, 0, G4ThreeVector());
  //Then subtract smaller plug from center to make ring and remove legs in center
  G4Tubs* solidIsoCenterHole = new G4Tubs("LXeVesselIsoHole",0*cm,CAPISOIR,CAPISOTHICKNESS/2.,0.*deg,360.*deg);
  G4SubtractionSolid* solidIsoRing = new G4SubtractionSolid("Iso with ring", solidIsoplusPlug, solidIsoCenterHole, 0, G4ThreeVector());
  // And Subtract fillits again
  const G4double ISOFILLITR = (tan(180./12.*deg)*4.076858)*cm-CAPISOY/2.;
  G4Tubs* solidIsoFillit = new G4Tubs("Fillits between IsoSpokes",0, ISOFILLITR, CAPISOTHICKNESS/2., 0, 360.*deg);
  G4SubtractionSolid* solidIsominusFillit1 = new G4SubtractionSolid("IsoRing-Fillit1",solidIsoRing        , solidIsoFillit, 0, G4ThreeVector(cos(0.5*180./6.*deg)*CAPISOOR, sin(0.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit2 = new G4SubtractionSolid("IsoRing-Fillit2",solidIsominusFillit1, solidIsoFillit, 0, G4ThreeVector(cos(1.5*180./6.*deg)*CAPISOOR, sin(1.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit3 = new G4SubtractionSolid("IsoRing-Fillit3",solidIsominusFillit2, solidIsoFillit, 0, G4ThreeVector(cos(2.5*180./6.*deg)*CAPISOOR, sin(2.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit4 = new G4SubtractionSolid("IsoRing-Fillit4",solidIsominusFillit3, solidIsoFillit, 0, G4ThreeVector(cos(3.5*180./6.*deg)*CAPISOOR, sin(3.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit5 = new G4SubtractionSolid("IsoRing-Fillit5",solidIsominusFillit4, solidIsoFillit, 0, G4ThreeVector(cos(4.5*180./6.*deg)*CAPISOOR, sin(4.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit6 = new G4SubtractionSolid("IsoRing-Fillit6",solidIsominusFillit5, solidIsoFillit, 0, G4ThreeVector(cos(5.5*180./6.*deg)*CAPISOOR, sin(5.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit7 = new G4SubtractionSolid("IsoRing-Fillit7",solidIsominusFillit6, solidIsoFillit, 0, G4ThreeVector(cos(6.5*180./6.*deg)*CAPISOOR, sin(6.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit8 = new G4SubtractionSolid("IsoRing-Fillit8",solidIsominusFillit7, solidIsoFillit, 0, G4ThreeVector(cos(7.5*180./6.*deg)*CAPISOOR, sin(7.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit9 = new G4SubtractionSolid("IsoRing-Fillit9",solidIsominusFillit8, solidIsoFillit, 0, G4ThreeVector(cos(8.5*180./6.*deg)*CAPISOOR, sin(8.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit10 = new G4SubtractionSolid("IsoRing-Fillit10",solidIsominusFillit9, solidIsoFillit, 0, G4ThreeVector(cos(9.5*180./6.*deg)*CAPISOOR, sin(9.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit11 = new G4SubtractionSolid("IsoRing-Fillit11",solidIsominusFillit10, solidIsoFillit, 0, G4ThreeVector(cos(10.5*180./6.*deg)*CAPISOOR, sin(10.5*180./6.*deg)*CAPISOOR, 0));
  G4SubtractionSolid* solidIsominusFillit12 = new G4SubtractionSolid("IsoRing-Fillit12",solidIsominusFillit11, solidIsoFillit, 0, G4ThreeVector(cos(11.5*180./6.*deg)*CAPISOOR, sin(11.5*180./6.*deg)*CAPISOOR, 0));
  G4Tubs* solidIsoOuterRing = new G4Tubs("IsoRing outerRing",22.1869*cm,22.4282*cm,CAPISOTHICKNESS/2.,0,360.*deg);
  G4UnionSolid* solidIsoComplete = new G4UnionSolid("CompleteIsoSupport",solidIsominusFillit12,solidIsoOuterRing,0,G4ThreeVector());
  //Add ISOGRID to LXeVessel
  G4UnionSolid* solidLXeVesselCapIso1of2 = new G4UnionSolid("LXeV with PZ cap", solidLXeVesselCap2of2, solidIsoComplete, 0, G4ThreeVector(0,0,CAPISOMIDZ));
  G4UnionSolid* solidLXeVesselCapIso2of2 = new G4UnionSolid("LXeV with NZ cap", solidLXeVesselCapIso1of2, solidIsoComplete, 0, G4ThreeVector(0,0,-1*CAPISOMIDZ));  
  
  //Now need to add the STAR but f-all of that, I'm goint to simplify it a bit
  //In realiaty the star has tapered arms, and reliefs between arms
  //As a compromise the arms are agerage width over taper, and the reliefs are
  // mostly just ignored (small amount of extra mass I guess)
  const G4double CAPSTARTHICKNESS = 0.2794*cm;
  const G4double CAPSTARMIDZ = CATH2CAPDIST + LXEVESSELENDTHICKNESS + CAPPIETHICKNESS + CAPISOTHICKNESS + CAPSTARTHICKNESS/2.;
  const G4double CAPSTARIR = 1.270*cm; //TODO: NEED TO GET REAL NUMBER
  const G4double CAPSTAROR = 5.9182*cm;
  const G4double CAPSTARX = .75625*cm;
  const G4double CAPSTARY = 17.9*cm;
  G4Box* solidStarLeg = new G4Box("StarLegThings",CAPSTARX/2., CAPSTARY, CAPSTARTHICKNESS/2.);
  G4UnionSolid* solidStarLeg2 = new G4UnionSolid("StarLeg 2", solidStarLeg, solidStarLeg, &CapRotZ1, G4ThreeVector());
  G4UnionSolid* solidStarLeg3 = new G4UnionSolid("StarLeg 3", solidStarLeg2, solidStarLeg, &CapRotZ2, G4ThreeVector());
  G4UnionSolid* solidStarLeg4 = new G4UnionSolid("StarLeg 4", solidStarLeg3, solidStarLeg, &CapRotZ3, G4ThreeVector());
  G4UnionSolid* solidStarLeg5 = new G4UnionSolid("StarLeg 5", solidStarLeg4, solidStarLeg, &CapRotZ4, G4ThreeVector());
  G4UnionSolid* solidStarLeg6 = new G4UnionSolid("StarLeg 6", solidStarLeg5, solidStarLeg, &CapRotZ5, G4ThreeVector());
  
  // Add plug to center
  G4Tubs* solidStarCenterPlug = new G4Tubs("LXeVesselStarPlug",0*cm,CAPSTAROR,CAPSTARTHICKNESS/2.,0.*deg,360.*deg);
  G4UnionSolid* solidStarplusPlug = new G4UnionSolid("Star+CenterCyl", solidStarLeg6, solidStarCenterPlug, 0, G4ThreeVector());
  //Then subtract smaller plug from center to make ring and remove legs in center
  G4Tubs* solidStarCenterHole = new G4Tubs("LXeVesselStarHole",0*cm,CAPSTARIR,CAPSTARTHICKNESS/2.,0.*deg,360.*deg);
  G4SubtractionSolid* solidStarRing = new G4SubtractionSolid("Star with ring", solidStarplusPlug, solidStarCenterHole, 0, G4ThreeVector());

  G4UnionSolid* LXeVesselwithStar1 = new G4UnionSolid("Star on VesselPZ", solidLXeVesselCapIso2of2, solidStarRing, 0, G4ThreeVector(0,0,CAPSTARMIDZ));
  G4UnionSolid* LXeVesselwithStar2 = new G4UnionSolid("Star on VesselPZ", LXeVesselwithStar1, solidStarRing, 0, G4ThreeVector(0,0,-1*CAPSTARMIDZ));
  
  // CREATE LOGICAL VOLUME
  G4LogicalVolume* logicLXeVessel = new G4LogicalVolume(LXeVesselwithStar2,	//its solid
                                                        TPCCopperMaterial,	//its material
                                                        "LXeVessel");	//its name
  //G4VisAttributes* LXeVesselVisAtt= new G4VisAttributes(G4Colour(1,1,1));
  logicLXeVessel->SetVisAttributes(CopperVisAtt);

  physiLXeVessel = new G4PVPlacement(&LXeVesselRM,
                                     G4ThreeVector(),
                                     logicLXeVessel,	//its logical volume
                                     "LXeVessel",	//its name
                                     logicHFE,	//its mother  volume
                                     false,		//no boolean operation
                                     0);		//copy number

  allphysvolumes.push_back(physiLXeVessel);
    
    
  //**********************************************************************************************
  // GEOMETRY UPGRADE: TPC Legs and High Voltage Feedthrough added , M.Tarka **********************
  //************************************************************************************************
    
  //------------------------------------------------------------------------------------------------
  //---------------------------------LXE VESSEL LEGS -----------------------------------------------
  //------------------------------------------------------------------------------------------------
   
  // The margin is mainly used as a transition gap between {Legs & HV-feedthrough} and
  // {TPC vessel & TPC door}. The diameters of the dummy TPC e.g. are bigger by the margin.
  G4double margin = 0.01*mm; 
    
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Reference Solid for Legs & HV feedthrough. All legs and the HV feedthrough are unified with the
  // solidRef using correct, relative orientations and positions. The orientation of the real TPC is
  // different from the dummy TPC. Final orientation matrixes are applied at the end. Those bring
  // the volumes into the right orientations relative to the real TPC.
  //////////////////////////////////////////////////////////////////////////////////////////////////
  G4Box* solidRef = new G4Box("solidRef",1*mm,1*mm,1*mm);
	
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Build a dummy TPC vessel (only as a solid). It will be subtracted at the end from the Legs and
  // the upper parts of the High Voltage-feedthrough. All inner radii = 0
  //////////////////////////////////////////////////////////////////////////////////////////////////
  G4double VesselDmy_posX = 0*cm;
  G4double VesselDmy_posY = 0*cm;
  G4double VesselDmy_posZ = 0*cm;
    
  G4int numRZDmy = 22;
  G4double rmaxVesselDmy[numRZDmy];
  G4double rminVesselDmy[numRZDmy];
  G4double zplanesVesselDmy[numRZDmy];
  G4int izDmy = 0;
    
  zplanesVesselDmy[izDmy]=-53.96446/2.*cm-1*margin;  rmaxVesselDmy[izDmy]=22.86000*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-40.10406/2.*cm-1*margin;  rmaxVesselDmy[izDmy]=22.86000*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-40.10406/2.*cm-1*margin;  rmaxVesselDmy[izDmy]=23.01240*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-37.36086/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=23.01240*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-33.46499/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=20.09140*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-30.92958/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=20.09140*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-30.92958/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=19.94916*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-11.37920/2.*cm-1*margin;  rmaxVesselDmy[izDmy]=19.94916*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-11.37920/2.*cm-1*margin;  rmaxVesselDmy[izDmy]=20.09140*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-10.10920/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=20.09140*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
  zplanesVesselDmy[izDmy]=-10.10920/2.*cm+1*margin;  rmaxVesselDmy[izDmy]=19.94916*cm+1*margin;    rminVesselDmy[izDmy]=0*cm; izDmy+=1;
    
  for (int i=0;i<numRZDmy/2;i++)
  {
      zplanesVesselDmy[numRZDmy/2+i] = -1*zplanesVesselDmy[(numRZDmy/2)-1-i];
      rmaxVesselDmy[numRZDmy/2+i] = rmaxVesselDmy[(numRZDmy/2)-1-i];
      rminVesselDmy[numRZDmy/2+i] = rminVesselDmy[(numRZDmy/2)-1-i];
  }
    
  G4Polycone* solid_LXeVesselDmy = new G4Polycone("LXeVesselDmy",0.*deg,360.*deg,numRZDmy,
                                                    zplanesVesselDmy,rminVesselDmy,rmaxVesselDmy);
    
  G4ThreeVector positionLXeVesselDmy = G4ThreeVector(VesselDmy_posX,VesselDmy_posY,VesselDmy_posZ);
    
    
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Construct a cylinder volume to be cutted from the bottom part of the legs. Creates a
  // transition to the TPC door. Used also for the bottom end of the High Voltage feedthorugh.
  //////////////////////////////////////////////////////////////////////////////////////////////////
    
  // Rotation matrix defining DoorCut orientation relative to the
  // dummy TPC (not same orientation as the real TPC)
  G4RotationMatrix* RotDoorCut = new G4RotationMatrix;
  RotDoorCut  -> rotateX(0.*deg);
  RotDoorCut  -> rotateY(90.*deg);
  RotDoorCut  -> rotateZ(0.*deg);
    
  // Position of the DoorCut relative to dummy TPC
  G4ThreeVector TransDoorCut = G4ThreeVector( -(74.564+2)*cm+(1*margin),0*cm,0*cm );
    
  // Define the DoorCut solid
  G4Tubs* solid_DoorCut = new G4Tubs("DoorCut",0*cm,50*cm,2*cm,0.*deg,360.*deg);
  //
  // Construct first leg pair P1 ///////////////////////////////////////////////////////////////////
  //
  G4double WT = 0.159; // Wallthickness of the Cu legs [cm]
  //
  // P1 dimensions
  //
  // PART A outer volume
  G4double P1_A_dimX = 2.858/2*cm;
  G4double P1_A_dimY = 5.398/2*cm;
  G4double P1_A_dimZ = 18.386/2*cm;
  // PART B_a outer volume
  G4double P1_B_a_dimX = P1_A_dimX;
  G4double P1_B_a_dimY = P1_A_dimY;
  G4double P1_B_a_dimZ = 2.497/2*cm+margin;
  // PART B_b outer volume
  G4double P1_B_b_dimX1 = WT*cm;
  G4double P1_B_b_dimX2 = 2*(P1_B_a_dimX-1.588/2*cm)+WT*cm;
  G4double P1_B_b_dimY1 = P1_B_a_dimY+margin;
  G4double P1_B_b_dimY2 = P1_B_a_dimY+margin;
  G4double P1_B_b_dimZ  = P1_B_a_dimZ+margin;
  // PART C outer volume
  G4double P1_C_dimX = 1.588/2*cm;
  G4double P1_C_dimY = P1_A_dimY;
  G4double P1_C_dimZ = (57/2)*cm;
  //
  // P1 relative positions
  //
  // PART A
  G4double P1_A_posX = VesselDmy_posX;
  G4double P1_A_posY = VesselDmy_posY;
  G4double P1_A_posZ = VesselDmy_posZ;
  //
  // PART B_a
  G4double P1_B_a_posX = P1_A_posX;
  G4double P1_B_a_posY = P1_A_posY;
  G4double P1_B_a_posZ = P1_A_posZ+P1_A_dimZ+P1_B_a_dimZ;
  G4ThreeVector P1_B_a_pos = G4ThreeVector(P1_B_a_posX,P1_B_a_posY,P1_B_a_posZ);
  // PART B_b
  G4double P1_B_b_posX = P1_B_a_posX - P1_B_a_dimX;
  G4double P1_B_b_posY = P1_B_a_posY;
  G4double P1_B_b_posZ = 0*cm ;
  //
  G4ThreeVector P1_B_b_pos = G4ThreeVector(P1_B_b_posX-WT*cm,P1_B_b_posY,P1_B_b_posZ);
  G4ThreeVector P1_B_b_pos_in = G4ThreeVector(P1_B_b_posX,P1_B_b_posY,P1_B_b_posZ);
  //
  // PART C
  G4double P1_C_posX = P1_B_a_posX+P1_B_a_dimX-P1_C_dimX;
  G4double P1_C_posY = P1_B_a_posY;
  G4double P1_C_posZ = P1_B_a_posZ+P1_B_a_dimZ+P1_C_dimZ;
  G4ThreeVector P1_C_pos = G4ThreeVector(P1_C_posX,P1_C_posY,P1_C_posZ);
  //
  // Construct P1 pair components: A, B, C
  //
  // PART A
  // A outer volume
  G4Box* solid_P1_A = new G4Box("P1_A_out",P1_A_dimX,P1_A_dimY,P1_A_dimZ);
  // A inner volume
  G4Box* solid_P1_A_in = new G4Box("P1_A_in",P1_A_dimX-WT*cm,P1_A_dimY-WT*cm,P1_A_dimZ);
  // PART B
  // B_a outer volume
  G4Box* solid_P1_B_a_out = new G4Box("P1_B_a_out",P1_B_a_dimX,P1_B_a_dimY,P1_B_a_dimZ);
  // B_b outer volume
  G4Trd* solid_P1_B_b_out = new G4Trd("P1_B_b_out",P1_B_b_dimX1,P1_B_b_dimX2,P1_B_b_dimY1,P1_B_b_dimY2,P1_B_b_dimZ);
  // Subtract the Trd face / construct the outer volume
  // B outer volume
  G4SubtractionSolid* solid_P1_B = new G4SubtractionSolid("P1_B_a_out-P1_B_b_out", solid_P1_B_a_out, solid_P1_B_b_out,0,P1_B_b_pos);
  // PART B inner volume
  // B_a inner volume
  G4Box* solid_P1_B_a_in = new G4Box("P1_B_a_in",P1_B_a_dimX-WT*cm,P1_B_a_dimY-WT*cm,P1_B_a_dimZ);
  // B_b inner volume
  G4Trd* solid_P1_B_b_in = solid_P1_B_b_out;
  // Subtract the Trd face / construct the inner volume
  // B inner volume
  G4SubtractionSolid* solid_P1_B_in = new G4SubtractionSolid("P1_B_a_in-P1_B_b_in", solid_P1_B_a_in, solid_P1_B_b_in,0,P1_B_b_pos_in);
  // PART C
  // C outer volume
  G4Box* solid_P1_C = new G4Box("P1_C_out",P1_C_dimX,P1_C_dimY,P1_C_dimZ);
  // C inner volume
  G4Box* solid_P1_C_in = new G4Box("P1_C_in",P1_C_dimX-WT*cm,P1_C_dimY-WT*cm,P1_C_dimZ);
  //
  // Construct the final P1 leg from components A, B, C  ===========================================
  //
  G4UnionSolid* solid_P1_a = new G4UnionSolid("P1_A+P1_B", solid_P1_A, solid_P1_B, 0, P1_B_a_pos);
  // Complete P1 leg outer volume
  G4UnionSolid* solid_P1 = new G4UnionSolid("P1_U1+P1_C", solid_P1_a, solid_P1_C, 0, P1_C_pos);
  G4UnionSolid* solid_P1XeLeg_U1 = new G4UnionSolid("P1_A_in+P1_B_in", solid_P1_A_in, solid_P1_B_in, 0,P1_B_a_pos);
  // Complete P1 leg inner volume
  G4UnionSolid* solid_P1XeLeg = new G4UnionSolid("P1XeLeg_U1+P1_C_in", solid_P1XeLeg_U1, solid_P1_C_in, 0,P1_C_pos);
  //
  // Translation and rotation vectors relative to the LXeVessel for the P1 leg & its diagonal clone
  //
  G4RotationMatrix* Rot_P1 = new G4RotationMatrix;
  Rot_P1  -> rotateY(90.*deg);
  Rot_P1  -> rotateX(-10.*deg);
  Rot_P1  -> rotateZ(0.*deg);
  //
  G4RotationMatrix* Rot_P1_clone = new G4RotationMatrix;
  Rot_P1_clone  -> rotateZ(180.*deg);
  Rot_P1_clone  -> rotateY(-90.*deg);
  Rot_P1_clone  -> rotateX(-10.*deg);
  //
  G4ThreeVector Trans_P1 = G4ThreeVector(-9.077*cm,-18.541*cm,18.127*cm); // ++
  G4ThreeVector Trans_P1_clone = G4ThreeVector(-9.077*cm,18.541*cm,-18.127*cm); // --
  //
  // Construct second leg pair P2 //////////////////////////////////////////////////////////////////
  //
  // P2 dimensions
  //
  G4double P2_L_tot = (18.386+2.497+57);
  G4double P2_dimX = (2.858/2)*cm;
  G4double P2_dimY = (5.398/2)*cm;
  G4double P2_dimZ = (P2_L_tot/2)*cm ;
  G4double P2_d_Leg = (P2_L_tot/2)*cm - (18.386/2)*cm;
  //
  // Construct the final P2 leg   ==================================================================
  //
  // Complete P2 leg outer volume
  G4Box* solid_P2 = new G4Box("P2_out",P2_dimX,P2_dimY,P2_dimZ);
  // Complete P2 leg inner volume
  G4Box* solid_P2XeLeg = new G4Box("P2_in",P2_dimX-WT*cm,P2_dimY-WT*cm,P2_dimZ);
  //
  // Translation and rotation vectors relative to the LXeVessel for the P2 leg & its diagonal clone
  //
  G4RotationMatrix* Rot_P2 = new G4RotationMatrix;
  Rot_P2  -> rotateZ(0.*deg);
  Rot_P2  -> rotateY(-90.*deg);
  Rot_P2  -> rotateX(10.*deg);
  //
  G4RotationMatrix* Rot_P2_clone = new G4RotationMatrix;
  Rot_P2_clone  -> rotateZ(0.*deg);
  Rot_P2_clone  -> rotateY(-90.*deg);
  Rot_P2_clone  -> rotateX(-10.*deg);
  //
  G4ThreeVector Trans_P2 = G4ThreeVector( -9.077*cm-(cos(10*deg)*P2_d_Leg), -18.541*cm-(sin(10*deg)*P2_d_Leg) ,-18.127*cm);
  G4ThreeVector Trans_P2_clone = G4ThreeVector( -9.077*cm-(cos(10*deg)*P2_d_Leg), 18.541*cm+(sin(10*deg)*P2_d_Leg) ,18.127*cm);
  //
  // Construct third leg pair P3 ///////////////////////////////////////////////////////////////////
  //
  //
  // P3 dimensions
  //
  // PART A
  G4double P3_A_dimX = (2.858/2)*cm;
  G4double P3_A_dimY = (5.398/2)*cm;
  G4double P3_A_dimZ = (24.917/2)*cm;
  //
  // PART B
  G4double P3_B_dimX = (2.381/2)*cm;
  G4double P3_B_dimY = (5.398/2)*cm;
  G4double P3_B_dimZ = (54/2)*cm;
  G4double P3_B_delta = 1*cm; // the part which overlapps with part A
  G4double P3_B_hypo = sqrt( ((P3_B_dimZ-P3_B_delta)*(P3_B_dimZ-P3_B_delta)) + ((P3_B_dimX/2)*(P3_B_dimX/2)) );
  //
  // PART C outer volume
  G4double P3_C_dimX = (2.0/2)*cm;
  G4double P3_C_dimY = (5.398/2)*cm;
  G4double P3_C_dimZ = (4.609/2)*cm;
  //
  // P3 relative positions
  //
  // PART A
  G4double P3_A_posX = 0*cm;
  G4double P3_A_posY = 0*cm;
  G4double P3_A_posZ = 0*cm;
  //
  // PART B
  G4double P3_B_posX = P3_A_posX - P3_A_dimX + P3_B_dimX - sin(15*deg)*P3_B_hypo;
  G4double P3_B_posY = P3_A_posY;
  G4double P3_B_posZ = P3_A_posZ + P3_A_dimZ + cos(15*deg)*P3_B_hypo;
  G4ThreeVector P3_B_pos = G4ThreeVector( P3_B_posX, P3_B_posY, P3_B_posZ );
  G4RotationMatrix* Rot_P3_B = new G4RotationMatrix;
  Rot_P3_B  -> rotateX(0.*deg);
  Rot_P3_B  -> rotateY(15.*deg);
  Rot_P3_B  -> rotateZ(0.*deg);
  //
  // PART C
  G4double P3_C_Alpha0 = atan(P3_C_dimZ/P3_C_dimX)*360/(2*3.1416); // rad -> deg
  G4double P3_C_R0 = sqrt((P3_C_dimX*P3_C_dimX)+(P3_C_dimZ*P3_C_dimZ));
  G4double P3_C_Phi0 = 20;
  G4double P3_C_Phi = P3_C_Alpha0 - P3_C_Phi0;
  G4double P3_C_posX = P3_A_posX + P3_A_dimX - P3_C_dimX - P3_C_R0*(cos(P3_C_Phi*deg)-cos(P3_C_Alpha0*deg));
  G4double P3_C_posY = P3_A_posY;
  G4double P3_C_posZ = P3_A_posZ + P3_A_dimZ + P3_C_dimZ - (P3_C_dimZ - P3_C_R0*sin(P3_C_Phi*deg));
  G4ThreeVector P3_C_pos = G4ThreeVector( P3_C_posX, P3_C_posY, P3_C_posZ );
  G4RotationMatrix* Rot_P3_C = new G4RotationMatrix;
  Rot_P3_C  -> rotateX(0*deg);
  Rot_P3_C  -> rotateY(P3_C_Phi0*deg);
  Rot_P3_C  -> rotateZ(0*deg);
  //
  // Construct P3 pair components, A, B, C
  //
  // PART A
  // A outer volume
  G4Box* solid_P3_A_out = new G4Box("P3_A_out",P3_A_dimX,P3_A_dimY,P3_A_dimZ);
  // A inner volume
  G4Box* solid_P3_A_in = new G4Box("P3_A_in",P3_A_dimX-WT*cm,P3_A_dimY-WT*cm,P3_A_dimZ);
  // PART B
  // B outer volume
  G4Box* solid_P3_B_out = new G4Box("P3_B_out",P3_B_dimX,P3_B_dimY,P3_B_dimZ);
  // B inner volume
  G4Box* solid_P3_B_in = new G4Box("P3_B_in",P3_B_dimX-WT*cm,P3_B_dimY-WT*cm,P3_B_dimZ);
  // PART C
  // C outer volume
  G4Box* solid_P3_C_out = new G4Box("P3_C_out",P3_C_dimX,P3_C_dimY,P3_C_dimZ);
  // C inner volume
  G4Box* solid_P3_C_in = new G4Box("P3_C_in",P3_C_dimX-WT*cm,P3_C_dimY-WT*cm,P3_C_dimZ);
  //
  // Construct the final P3 leg from components A, B, C  =========================================
  //
  G4UnionSolid* solid_P3_U1_out = new G4UnionSolid("P3_A_out + P3_B_out", solid_P3_A_out, solid_P3_B_out, Rot_P3_B, P3_B_pos);
  // Complete P3 leg outer volume
  G4UnionSolid* solid_P3 = new G4UnionSolid("P3_U1_out + P3_C_out", solid_P3_U1_out, solid_P3_C_out, Rot_P3_C, P3_C_pos);
  G4UnionSolid* solid_P3XeLeg_U1 = new G4UnionSolid("P3_A_in + P3_B_in", solid_P3_A_in, solid_P3_B_in, Rot_P3_B, P3_B_pos);
  // Complete P3 leg inner volume
  G4UnionSolid* solid_P3XeLeg = new G4UnionSolid("P3XeLeg_U1 + P3_C_in", solid_P3XeLeg_U1, solid_P3_C_in, Rot_P3_C, P3_C_pos);
  //
  // Translation and rotation vectors relative to the LXeVessel for the P3 leg & its diagonal clone
  //
  G4RotationMatrix* Rot_P3 = new G4RotationMatrix;
  Rot_P3  -> rotateZ(0.*deg);
  Rot_P3  -> rotateY(90.*deg);
  Rot_P3  -> rotateX(0.*deg);
  //
  G4RotationMatrix* Rot_P3_clone = new G4RotationMatrix;
  Rot_P3_clone  -> rotateY(90.*deg);
  Rot_P3_clone  -> rotateZ(180.*deg);
  Rot_P3_clone  -> rotateX(0.*deg);
    
  G4ThreeVector Trans_P3 = G4ThreeVector(-24.917/2*cm,0*cm,-18.127*cm);
  G4ThreeVector Trans_P3_clone = G4ThreeVector(-24.917/2*cm,0*cm,18.127*cm);
  //
  // Construct the final inner 6-Leg volume ======================================================
  //
  G4UnionSolid* solid_P1_U1 = new G4UnionSolid("solidRef+P1", solidRef, solid_P1, Rot_P1, Trans_P1);
  G4UnionSolid* solid_P1_U2 = new G4UnionSolid("P1_U1+P1", solid_P1_U1, solid_P1, Rot_P1_clone, Trans_P1_clone);
  G4UnionSolid* solid_P1P2_U1 = new G4UnionSolid("P1_U2+P2", solid_P1_U2, solid_P2, Rot_P2, Trans_P2);
  G4UnionSolid* solid_P1P2_U2 = new G4UnionSolid("P1P2_U2+P2", solid_P1P2_U1, solid_P2, Rot_P2_clone, Trans_P2_clone);
  G4UnionSolid* solid_P1P2P3_U1 = new G4UnionSolid("P1P2_U2+P3", solid_P1P2_U2, solid_P3, Rot_P3, Trans_P3);
  G4UnionSolid* solid_P1P2P3_U2 = new G4UnionSolid("P1P2P3_U1+P2", solid_P1P2P3_U1, solid_P3, Rot_P3_clone, Trans_P3_clone);
    
  // Make Leg transitions to the vessel
  G4SubtractionSolid* solid_P1P2P3_S1 = new G4SubtractionSolid("P1P2P3_U2-LXeVesselDmy", solid_P1P2P3_U2,solid_LXeVesselDmy);
  // Make Leg transitions to the TPC door
  G4SubtractionSolid* solid_Legs = new G4SubtractionSolid("P1P2P3_S1-CutLegs", solid_P1P2P3_S1, solid_DoorCut,RotDoorCut,TransDoorCut);
  //
  // Construct the final inner 6-Leg innner volume made of Xe ====================================
  //
  G4UnionSolid* solid_XeLegP1_U1 = new G4UnionSolid("solidRef+P1AirLeg", solidRef, solid_P1XeLeg, Rot_P1, Trans_P1);
  G4UnionSolid* solid_XeLegP1_U2 = new G4UnionSolid("AirLegP1_U1+P1AirLeg", solid_XeLegP1_U1, solid_P1XeLeg, Rot_P1_clone, Trans_P1_clone);
  G4UnionSolid* solid_XeLegP1P2_U1 = new G4UnionSolid("solid_XeLegP1_U2+P2XeLeg", solid_XeLegP1_U2, solid_P2XeLeg, Rot_P2, Trans_P2);
  G4UnionSolid* solid_XeLegP1P2_U2 = new G4UnionSolid("solid_XeLegP1P2_U1+P2XeLeg", solid_XeLegP1P2_U1, solid_P2XeLeg, Rot_P2_clone, Trans_P2_clone);
  G4UnionSolid* solid_XeLegP1P2P3_U1 = new G4UnionSolid("solid_XeLegP1P2_U2+P3XeLeg", solid_XeLegP1P2_U2, solid_P3XeLeg, Rot_P3,Trans_P3);
  G4UnionSolid* solid_XeLegP1P2P3_U2 = new G4UnionSolid("solid_XeLegP1P2P3_U1+P3XeLeg", solid_XeLegP1P2P3_U1, solid_P3XeLeg, Rot_P3_clone,Trans_P3_clone);
	
  // Make Xe-Leg transitions to the TPC
  G4SubtractionSolid* solid_XeLegP1P2P3_S1 = new G4SubtractionSolid("XeLegP1P2P3_U2-LXeVesselDmy", solid_XeLegP1P2P3_U2,solid_LXeVesselDmy);
  // Make Xe-Leg transitions to the TPC door
  G4SubtractionSolid* solid_XeLegs = new G4SubtractionSolid("XeLegP1P2P3_S1-DoorCut", solid_XeLegP1P2P3_S1, solid_DoorCut,RotDoorCut,TransDoorCut);
    
  // Rotation matrix for the final legs orientation. Same for the leg interior filled with Xe.
  G4RotationMatrix* LegRotFinal = new G4RotationMatrix;
  LegRotFinal  -> rotateY(-90.*deg);
  LegRotFinal  -> rotateX(15.*deg);
  LegRotFinal  -> rotateZ(0.*deg);
    
  G4LogicalVolume* logic_Legs = new G4LogicalVolume(solid_Legs,         	 // its solid
                                                    TPCCopperMaterial,	     // its material
                                                    "Legs_log");	         // its name
    
  G4VisAttributes* Legs_VisAtt= new G4VisAttributes(G4Colour(0.2,0,1));
  logic_Legs -> SetVisAttributes(Legs_VisAtt);
  phys_Legs = new G4PVPlacement(LegRotFinal,positionLXeVesselDmy,
                                                   logic_Legs,	             // its logical volume
                                                   "Legs_Cu",	             // its name
                                                   logicHFE,                 // mother volume
                                                   false,		             // no boolean operation
                                                   0);		                 // copy number
	
  // check volume for overlapps
  allphysvolumes.push_back(phys_Legs);
    
    
  G4LogicalVolume* logic_XeLegs = new G4LogicalVolume(solid_XeLegs,	         // its solid
                                                      LXeMaterial,	         // its material
                                                      "XeLegs_log");	     // its name
    
  G4VisAttributes* XeLegs_VisAtt= new G4VisAttributes(G4Colour(0,1,0.2));
  logic_XeLegs -> SetVisAttributes(XeLegs_VisAtt);
  phys_XeLegs = new G4PVPlacement(0,positionLXeVesselDmy,
                                                     logic_XeLegs,	         // its logical volume
                                                     "Legs_Xe",	             // its name
                                                     logic_Legs,	         // its mother  volume
                                                     false,		             // no boolean operation
                                                     0);		             // copy number
  // check volume for overlapps
  allphysvolumes.push_back(phys_XeLegs);
	
    
  //------------------------------------------------------------------------------------------------
  //------------------------ High Voltage Feedthrough ----------------------------------------------
  //------------------------------------------------------------------------------------------------
    
  // Rotation matrix for all HV components, orientation relative to the dummy TPC
  G4RotationMatrix* Rot_HV = new G4RotationMatrix;
  Rot_HV  -> rotateY(90.*deg);
  Rot_HV  -> rotateX(30.*deg);
  Rot_HV  -> rotateZ(0.*deg);
  //
  // Translation vector for the final position of the HV feedthrough
  G4ThreeVector HV_TransFinal = G4ThreeVector(0*mm,0*mm,0*mm);
  //
  // Rotation matrix for the final orientation of the HV feedthrough components relative to the real TPC
  G4RotationMatrix* HV_RotFinal = new G4RotationMatrix;
  HV_RotFinal  -> rotateY(-90.*deg);
  HV_RotFinal  -> rotateX(15.*deg);
  HV_RotFinal  -> rotateZ(0.*deg);
  //
  // HV: Outer Copper housing //////////////////////////////////////////////////////////////////////
  //
  //
  // PART A - Copper housing, main part
  //
  // Translation vector for HV_CuHous_A relative to the center of the dummy TPC
  G4ThreeVector Trans_HV_CuHous_A = G4ThreeVector(-746.57*mm*cos(30*deg),746.57*mm*sin(30*deg),0*mm);
  G4int NumPlanes_HV_CuHous_A = 18;
  G4double ra_HV_CuHous_A[NumPlanes_HV_CuHous_A];
  G4double ri_HV_CuHous_A[NumPlanes_HV_CuHous_A];
  G4double PlanePos_HV_CuHous_A[NumPlanes_HV_CuHous_A];
  G4int i_HV_CuHous_A = 0;
  //
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]= 0.*mm;      ra_HV_CuHous_A[i_HV_CuHous_A]=15.88/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-363.89*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=15.88/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-363.89*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=22.86/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-370.24*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=22.86/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-370.24*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=22.86/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-375.25*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=22.86/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-375.25*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=42.16/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-376.77*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=42.16/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-376.77*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=42.16/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-411.57*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=42.16/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-411.57*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=53.34/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-432.65*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=53.34/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-432.65*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=53.34/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-542.02*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=53.34/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-542.73*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=53.61/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-543.92*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=54.43/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-545.15*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=59.95/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  PlanePos_HV_CuHous_A[i_HV_CuHous_A]=-555.00*mm;  ra_HV_CuHous_A[i_HV_CuHous_A]=59.95/2*mm;  ri_HV_CuHous_A[i_HV_CuHous_A]=0*mm+margin;   i_HV_CuHous_A+=1;
  //
  // The solid, Cu housing main part
  G4Polycone* solid_HV_CuHous_A = new G4Polycone("HV_CuHous_A",0.*deg,360.*deg,NumPlanes_HV_CuHous_A,PlanePos_HV_CuHous_A,ri_HV_CuHous_A,ra_HV_CuHous_A);
  //
  // PART B - Copper housing, the Torus part
  //
  G4double HV_CuHous_B_Rmin  =  0*mm;         // Inside radius
  G4double HV_CuHous_B_Rmax  =  15.88/2*mm;   // Outside radius
  G4double HV_CuHous_B_R     =  152.5*mm;     // Swept radius of torus
  G4double HV_CuHous_B_RSPhi =  0;            // Starting angle in radians
  G4double HV_CuHous_B_RDPhi =  0.5236;       // Delta angle of the segment in radians, 0.5236 = 30 deg
  //
  // Torus part, HV_CuHous_B rotation relative to HV_CuHous_A
  G4RotationMatrix* Rot_HV_CuHous_B = new G4RotationMatrix;
  Rot_HV_CuHous_B  -> rotateZ(-90*deg);
  Rot_HV_CuHous_B  -> rotateX(-90.*deg);
  Rot_HV_CuHous_B  -> rotateY(0.*deg);
  //
  // Translation vector for HV_CuHous_B relative to the center of the dummy TPC
  G4ThreeVector Trans_HV_CuHous_B = G4ThreeVector( 0*mm,-HV_CuHous_B_R,0*mm);
  //
  // The solid, Cu housing torus part
  G4Torus* solid_HV_CuHous_B = new G4Torus("HV_CuHous_B",HV_CuHous_B_Rmin,HV_CuHous_B_Rmax,HV_CuHous_B_R,HV_CuHous_B_RSPhi,HV_CuHous_B_RDPhi);
  //
  // PART C - Copper housing, the end-tube
  //
  G4double HV_CuHous_C_dimL = 15*mm; // Length of end_tube, longer then in real, will be cutted with solid_DoorCut later
  //
  G4RotationMatrix* Rot_HV_CuHous_C = new G4RotationMatrix; // End-tube part, HV_CuHous_C rotation relative to HV_CuHous_B
  Rot_HV_CuHous_C  -> rotateX(90*deg);
  Rot_HV_CuHous_C  -> rotateZ(0.*deg);
  Rot_HV_CuHous_C  -> rotateY(30.*deg);
  //
  // Translation vector for HV_CuHous_B relative to the center of the dummy TPC
  G4ThreeVector Trans_HV_CuHous_C = G4ThreeVector( (HV_CuHous_B_R*cos(30*deg)) - (HV_CuHous_C_dimL*sin(30*deg)),(HV_CuHous_B_R/2) + (HV_CuHous_C_dimL*cos(30*deg)),0*mm);
  //
  // The solid, Cu housing end-tube part
  G4Tubs* solid_HV_CuHous_C = new G4Tubs("HV_CuHous_C",0*mm,15.88/2*mm,HV_CuHous_C_dimL,0.*deg,360.*deg);
  //
  // Construct the the final HV Cu-housing volume ==================================================
  //
  // Unify Torus and end-tube part
  G4UnionSolid* solid_HV_CuHous1 = new G4UnionSolid("HV_CuHous_B+HV_CuHous_C", solid_HV_CuHous_B, solid_HV_CuHous_C, Rot_HV_CuHous_C, Trans_HV_CuHous_C);
  // Unify Torus+end-tube with main Cu housing part
  G4UnionSolid* solid_HV_CuHous2 = new G4UnionSolid("CuHous_A+HV_CuHous1", solid_HV_CuHous_A, solid_HV_CuHous1, Rot_HV_CuHous_B, Trans_HV_CuHous_B);
  //
  // Unify complete HV Cu-housing with reference solid, brings the part into the right relativ orientation and position
  G4UnionSolid* solid_HV_CuHous3 = new G4UnionSolid("solidRef+HV_CuHous2", solidRef, solid_HV_CuHous2, Rot_HV,Trans_HV_CuHous_A);
  // Make the transitions to the TPC
  G4SubtractionSolid* solid_HV_CuHous4  = new G4SubtractionSolid("HV_CuHous3-LXeVesselDmy", solid_HV_CuHous3,solid_LXeVesselDmy);
  // Make the transitions to the door
  G4SubtractionSolid* solid_HV_CuHous= new G4SubtractionSolid("HV_CuHous4-DoorCut", solid_HV_CuHous4, solid_DoorCut, RotDoorCut,TransDoorCut);
  //
  G4LogicalVolume* logic_HV_CuHous = new G4LogicalVolume(solid_HV_CuHous,    // its solid
                                                         TPCCopperMaterial,	 // its material
                                                         "HV_CuHous_log");	 // its name
  //
  G4VisAttributes* HV_CuHous_VisAtt= new G4VisAttributes(G4Colour(0.2,0.8,1));
  logic_HV_CuHous -> SetVisAttributes(HV_CuHous_VisAtt);
  //
  phys_HV_CuHous = new G4PVPlacement(HV_RotFinal, HV_TransFinal,
                                                        logic_HV_CuHous,	 // its logical volume
                                                        "HV_CuHous",	     // its name
                                                        logicHFE,            // mother  volume
                                                        false,		         // no boolean operation
                                                        0);		             // copy number
    
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_CuHous);
  //
  // HV: Xe in Cu housing //////////////////////////////////////////////////////////////////////////
  //
  // PART A - Xe in copper housing, main part
  //
  G4int NumPlanes_HV_XeIn_A = 18;
  G4double ra_HV_XeIn_A[NumPlanes_HV_XeIn_A];
  G4double ri_HV_XeIn_A[NumPlanes_HV_XeIn_A];
  G4double PlanePos_HV_XeIn_A[NumPlanes_HV_XeIn_A];
  G4int i_HV_XeIn_A = 0;
  //
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]= 0.*mm;      ra_HV_XeIn_A[i_HV_XeIn_A]=12.57/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-363.89*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=12.57/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-363.89*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=12.57/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-370.24*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=12.57/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-370.24*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=20.32/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-375.25*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=20.32/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-375.25*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=20.32/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-376.77*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=20.32/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-376.77*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=39.62/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-411.57*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=39.62/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-411.57*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=39.62/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-432.65*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=39.62/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-432.65*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=50.80/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-542.02*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=50.80/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-542.73*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=50.80/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-543.92*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=51.22/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-545.15*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=52.04/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  PlanePos_HV_XeIn_A[i_HV_XeIn_A]=-555.00*mm;  ra_HV_XeIn_A[i_HV_XeIn_A]=52.04/2*mm;  ri_HV_XeIn_A[i_HV_XeIn_A]=0*mm;   i_HV_XeIn_A+=1;
  //
  // The solid, Xe in Cu housing main part
  G4Polycone* solid_HV_XeIn_A = new G4Polycone("HV_XeIn_A",0.*deg,360.*deg,NumPlanes_HV_XeIn_A,PlanePos_HV_XeIn_A,ri_HV_XeIn_A,ra_HV_XeIn_A);
  //
  // PART B - Xe in copper housing, the Torus part
  //
  G4double HV_XeIn_B_Rmax  =  12.57/2*mm; // Torus outside radius
  //
  // Xe in Cu housing torus part
  G4Torus* solid_HV_XeIn_B = new G4Torus("HV_XeIn_B",HV_CuHous_B_Rmin,HV_XeIn_B_Rmax,HV_CuHous_B_R,HV_CuHous_B_RSPhi,HV_CuHous_B_RDPhi);
  //
  // PART C - Xe in copper housing, the End-tube part
  //
  // Xe in Cu housing end-tube part
  G4Tubs* solid_HV_XeIn_C = new G4Tubs("HV_XeIn_C",0*mm,HV_XeIn_B_Rmax,HV_CuHous_C_dimL,0.*deg,360.*deg);
  //
  // Construct the the final HV Xe volume in the Cu-housing  =======================================
  //
  // Unify Xe Torus and Xe end-tube part
  G4UnionSolid* solid_HV_XeIn1 = new G4UnionSolid("HV_XeIn_B+HV_XeIn_C", solid_HV_XeIn_B, solid_HV_XeIn_C, Rot_HV_CuHous_C, Trans_HV_CuHous_C);
  // Unify Xe Torus+Xe end-tube with Xe main part in Cu housing
  G4UnionSolid* solid_HV_XeIn2 = new G4UnionSolid("HV_XeIn_A+HV_XeIn1", solid_HV_XeIn_A, solid_HV_XeIn1, Rot_HV_CuHous_B, Trans_HV_CuHous_B);
  //
  // Unify complete HV Cu-housing with reference solid, brings the part into the right relative
  // orientation and position
  G4UnionSolid* solid_HV_XeIn3 = new G4UnionSolid("solidRef+HV_XeIn2", solidRef, solid_HV_XeIn2, Rot_HV,Trans_HV_CuHous_A);
  // Make the transitions to the TPC
  G4SubtractionSolid* solid_HV_XeIn4  = new G4SubtractionSolid("HV_XeIn3-LXeVesselDmy", solid_HV_XeIn3,solid_LXeVesselDmy);
  // Make the transitions to the TPC door
  G4SubtractionSolid* solid_HV_XeIn= new G4SubtractionSolid("HV_XeIn4-DoorCut", solid_HV_XeIn4, solid_DoorCut, RotDoorCut,TransDoorCut);
  //
  G4LogicalVolume* logic_HV_XeIn = new G4LogicalVolume(solid_HV_XeIn,        // its solid
                                                       LXeMaterial,	         // its material
                                                       "HV_XeIn_log");	     // its name
    
  G4VisAttributes* HV_XeIn_VisAtt= new G4VisAttributes(G4Colour(0.2,0.8,1));
  logic_HV_XeIn -> SetVisAttributes(HV_XeIn_VisAtt);
  phys_HV_XeIn = new G4PVPlacement(0,G4ThreeVector(),
                                                      logic_HV_XeIn,	     // its logical volume
                                                      "HV_XeIn",	         // its name
                                                      logic_HV_CuHous,       // mother volume
                                                      false,		         // no boolean operation
                                                      0);		             // copy number
    
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_XeIn);
  //
  // HV: Teflon isolation inside Cu housing ////////////////////////////////////////////////////////
  //
  // PART A (the only one) - Teflon isolation in copper housing, close to TPC, contains the contact
  //
  // Translation vector for Teflon isolation relative to the center of the dummy TPC
  G4ThreeVector Trans_HV_Tef_A = G4ThreeVector(-369.33*mm*cos(30*deg),369.33*mm*sin(30*deg),0*mm);
  G4int NumPlanes_HV_Tef_A = 9;
  G4double ra_HV_Tef_A[NumPlanes_HV_Tef_A];
  G4double ri_HV_Tef_A[NumPlanes_HV_Tef_A];
  G4double PlanePos_HV_Tef_A[NumPlanes_HV_Tef_A];
  G4int i_HV_Tef_A = 0;
  //
  PlanePos_HV_Tef_A[i_HV_Tef_A]=0.*mm;       ra_HV_Tef_A[i_HV_Tef_A]=19.05/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=13.01/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-46.65*mm;   ra_HV_Tef_A[i_HV_Tef_A]=19.05/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=10.32/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-57.78*mm;   ra_HV_Tef_A[i_HV_Tef_A]=19.05/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=10.32/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-57.78*mm;   ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=10.32/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-116.63*mm;  ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=10.32/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-116.63*mm;  ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=25.40/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-166.11*mm;  ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=25.40/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-166.11*mm;  ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=19.05/2*mm; i_HV_Tef_A+=1;
  PlanePos_HV_Tef_A[i_HV_Tef_A]=-176.39*mm;  ra_HV_Tef_A[i_HV_Tef_A]=49.78/2*mm;  ri_HV_Tef_A[i_HV_Tef_A]=19.05/2*mm; i_HV_Tef_A+=1;
  //
  G4Polycone* solid_HV_Tef_A = new G4Polycone("HV_Tef_A",0.*deg,360.*deg,NumPlanes_HV_Tef_A,PlanePos_HV_Tef_A,ri_HV_Tef_A,ra_HV_Tef_A);
  //
  // Construct the final Teflon part ===============================================================
  //
  // Unify Teflon isolation with reference solid
  G4UnionSolid* solid_HV_Tef1 = new G4UnionSolid("solidLRef+HV_Tef1", solidRef, solid_HV_Tef_A, Rot_HV,Trans_HV_Tef_A);
  // Make the transitions to the TPC, DoorCut not needed
  G4SubtractionSolid* solid_HV_Tef  = new G4SubtractionSolid("HV_Tef1-LXeVesselDmy", solid_HV_Tef1,solid_LXeVesselDmy);
  //
  G4LogicalVolume* logic_HV_Tef = new G4LogicalVolume(solid_HV_Tef,          // its solid
                                                      TeflonMaterial,	     // its material
                                                      "HV_Tef_log");	     // its name
    
  G4VisAttributes* HV_Tef_VisAtt= new G4VisAttributes(G4Colour(0.2,0.8,1));
  logic_HV_Tef -> SetVisAttributes(HV_Tef_VisAtt);
  phys_HV_Tef = new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm),
                                                     logic_HV_Tef,	         // its logical volume
                                                     "HV_Tef",	             // its name
                                                     logic_HV_XeIn,          // mother volume
                                                     false,		             // no boolean operation
                                                     0);		             // copy number
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_Tef);
  //
  // HV: Cu contact in Teflon isolation ////////////////////////////////////////////////////////////
  //
  //
  // PART A (the only one) - close to TPC
  //
  // Translation vector for contact relative to the center of the dummy TPC
  G4ThreeVector Trans_HV_Cont_A = G4ThreeVector(-250.48*mm*cos(30*deg),250.48*mm*sin(30*deg),0*mm);
  G4int NumPlanes_HV_Cont_A = 10;
  G4double ra_HV_Cont_A[NumPlanes_HV_Cont_A];
  G4double ri_HV_Cont_A[NumPlanes_HV_Cont_A];
  G4double PlanePos_HV_Cont_A[NumPlanes_HV_Cont_A];
  G4int i_HV_Cont_A = 0;
  //
  PlanePos_HV_Cont_A[i_HV_Cont_A]= 0.*mm-margin;     ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=12.70/2*mm+margin; i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-8.67*mm-margin;   ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=12.70/2*mm+margin; i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-8.67*mm-margin;   ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=3.18/2*mm+margin;  i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-9.17*mm;          ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=3.18/2*mm+margin;  i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-9.17*mm;          ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=11.88/2*mm;        i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-35.73*mm;         ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=11.88/2*mm;        i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-35.73*mm;         ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=0*mm;              i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-47.26*mm+margin;  ra_HV_Cont_A[i_HV_Cont_A]=24.69/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=0*mm;              i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-47.26*mm+margin;  ra_HV_Cont_A[i_HV_Cont_A]=17.78/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=0*mm;              i_HV_Cont_A+=1;
  PlanePos_HV_Cont_A[i_HV_Cont_A]=-57.60*mm;         ra_HV_Cont_A[i_HV_Cont_A]=17.78/2*mm-margin;  ri_HV_Cont_A[i_HV_Cont_A]=0*mm;              i_HV_Cont_A+=1;
  //
  G4Polycone* solid_HV_Cont_A = new G4Polycone("HV_Cont_A",0.*deg,360.*deg,NumPlanes_HV_Cont_A,PlanePos_HV_Cont_A,ri_HV_Cont_A,ra_HV_Cont_A);
  //
  // Construct the final contact part ==============================================================
  //
  // Unify Contact with reference solid
  G4UnionSolid* solid_HV_Cont1 = new G4UnionSolid("solidRef+HV_Cont_A", solidRef, solid_HV_Cont_A, Rot_HV,Trans_HV_Cont_A);
  // Make the transitions to the TPC, DoorCut not needed
  G4SubtractionSolid* solid_HV_Cont  = new G4SubtractionSolid("HV_Cont1-LXeVesselDmy", solid_HV_Cont1,solid_LXeVesselDmy);
  //
  G4LogicalVolume* logic_HV_Cont = new G4LogicalVolume(solid_HV_Cont,        // its solid
                                                       TPCCopperMaterial,	 // its material
                                                       "HV_Cont_log");	     // its name
    
  G4VisAttributes* HV_Cont_VisAtt= new G4VisAttributes(G4Colour(0.2,0.8,1));
  logic_HV_Cont -> SetVisAttributes(HV_Cont_VisAtt);
  phys_HV_Cont = new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm),
                                                      logic_HV_Cont,	     // its logical volume
                                                      "HV_Cont",	         // its name
                                                      logic_HV_XeIn,         // mother volume
                                                      false,		         // no boolean operation
                                                      0);		             // copy number
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_Cont);
  //
  // HV: Polyethylene isolation around Cu rod/cable ////////////////////////////////////////////////
  //
  // Position of final part relative center of the dummy TPC
  G4ThreeVector Trans_HV_Iso = G4ThreeVector( (-746.57*mm+(498.91/2)*mm)*cos(30*deg),(746.57*mm-(498.91/2)*mm)*sin(30*deg),0*mm);
  // Translation vector for HV_Iso_C relative to HV_Iso_A
  G4ThreeVector Trans_HV_Iso_C = G4ThreeVector( 0*mm,-HV_CuHous_B_R,(498.91/2)*mm);
  G4double HV_Ri_Iso = (2.69/2)*mm + margin;
  G4double HV_Ra_Iso = (9.4/2)*mm;
  G4double HV_H_Iso  = (498.91/2)*mm+margin;
  //
  // PART A - PE main part
  //
  G4Tubs* solid_HV_Iso_A = new G4Tubs("HV_Iso_A",HV_Ri_Iso,HV_Ra_Iso,HV_H_Iso,0.*deg,360.*deg);
  //
  // PART B - the PE torus section
  //
  G4Torus* solid_HV_Iso_B = new G4Torus("HV_Iso_B",HV_Ri_Iso,HV_Ra_Iso,HV_CuHous_B_R,HV_CuHous_B_RSPhi,HV_CuHous_B_RDPhi);
  //
  // PART C - the PE end-tube
  //
  G4Tubs* solid_HV_Iso_C = new G4Tubs("HV_Iso_C",HV_Ri_Iso,HV_Ra_Iso,HV_CuHous_C_dimL,0.*deg,360.*deg);
  //
  // Construct the final PE isolation ==============================================================
  //
  // Unify PE Torus and PE end-tube part
  G4UnionSolid* solid_HV_Iso1 = new G4UnionSolid("HV_Iso_B + HV_Iso_C", solid_HV_Iso_B, solid_HV_Iso_C, Rot_HV_CuHous_C, Trans_HV_CuHous_C);
  // Unify PE Torus+ PE end-tube with PE main part
  G4UnionSolid* solid_HV_Iso2 = new G4UnionSolid("HV_Iso_A + HV_Iso1", solid_HV_Iso_A, solid_HV_Iso1, Rot_HV_CuHous_B, Trans_HV_Iso_C);
  // Unify PE part with reference solid with right coordinates and orientation
  G4UnionSolid* solid_HV_Iso3 = new G4UnionSolid("solidRef+HV_Iso2", solidRef, solid_HV_Iso2, Rot_HV,Trans_HV_Iso);
  // Make the transition to the TPC
  G4SubtractionSolid* solid_HV_Iso4  = new G4SubtractionSolid("HV_Iso3-LXeVesselDmy", solid_HV_Iso3,solid_LXeVesselDmy);
  // Make the transition to the door
  G4SubtractionSolid* solid_HV_Iso = new G4SubtractionSolid("HV_Iso4", solid_HV_Iso4, solid_DoorCut, RotDoorCut,TransDoorCut);
  //
  G4LogicalVolume* logic_HV_Iso = new G4LogicalVolume(solid_HV_Iso,          // its solid
                                                      HDPEMaterial,	         // its material
                                                      "HV_Iso_log");	     // its name
    
  G4VisAttributes* HV_Iso_VisAtt= new G4VisAttributes(G4Colour(0.2,0.8,1));
  logic_HV_Iso -> SetVisAttributes(HV_Iso_VisAtt);
  phys_HV_Iso = new G4PVPlacement(0,G4ThreeVector( 0*mm,0*mm,0*mm),
                                                     logic_HV_Iso,	         // its logical volume
                                                     "HV_Iso",	             // its name
                                                     logic_HV_XeIn,          // mother volume
                                                     false,		             // no boolean operation
                                                     0);
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_Iso);
  //
  // HV: Cu rod/cable, inner most part /////////////////////////////////////////////////////////////
  //
  // Translation vector for HV_CuRod_C relative to HV_CuRod_A
  G4ThreeVector Trans_HV_CuRod_C = G4ThreeVector( 0*mm, -HV_CuHous_B_R, (523.69/2)*mm);
  // Position of final part relative center of the dummy TPC
  G4ThreeVector Trans_HV_CuRod = G4ThreeVector( (-746.57*mm+(523.69/2)*mm)*cos(30*deg),(+746.57*mm-(523.69/2)*mm)*sin(30*deg),0*mm);
  G4double HV_Ra_CuRod = (2.69/2)*mm;            // outer radius
  G4double HV_H_CuRod_A  = (523.69/2)*mm+margin; // length of the straight, main part
  //
  // PART A - Cu rod main part
  //
  G4Tubs* solid_HV_CuRod_A = new G4Tubs("HV_CuRod_A",0*mm,HV_Ra_CuRod,HV_H_CuRod_A,0.*deg,360.*deg);
  //
  // PART B - the Cu rod torus section
  //
  G4Torus* solid_HV_CuRod_B= new G4Torus("HV_CuRod_B",0*mm,HV_Ra_CuRod,HV_CuHous_B_R,HV_CuHous_B_RSPhi,HV_CuHous_B_RDPhi);
  //
  // PART C - the Cu rod end-tube
  //
  G4Tubs* solid_HV_CuRod_C = new G4Tubs("HV_CuRod_C",0*mm,HV_Ra_CuRod,HV_CuHous_C_dimL,0.*deg,360.*deg);
  //
  // Construct the final Cu rod part ===============================================================
  //
  // Unify Cu rod Torus and Cu rod end-tube part
  G4UnionSolid* solid_HV_CuRod1 = new G4UnionSolid("HV_CuRod_B + HV_CuRod_C", solid_HV_CuRod_B, solid_HV_CuRod_C, Rot_HV_CuHous_C, Trans_HV_CuHous_C);
  // Unify Cu rod Torus+ Cu rod end-tube with Cu rod main part
  G4UnionSolid* solid_HV_CuRod2 = new G4UnionSolid("HV_CuRod_A + HV_CuRod1", solid_HV_CuRod_A, solid_HV_CuRod1, Rot_HV_CuHous_B, Trans_HV_CuRod_C);
  //
  // Unify Cu rod part with reference solid with right coordinates and orientation
  G4UnionSolid* solid_HV_CuRod3 = new G4UnionSolid("solidRef+HV_CuRod2", solidRef, solid_HV_CuRod2, Rot_HV,Trans_HV_CuRod);
  // Make the transition to the TPC
  G4SubtractionSolid* solid_HV_CuRod4  = new G4SubtractionSolid("HV_CuRod3-LXeVesselDmy", solid_HV_CuRod3,solid_LXeVesselDmy);
  // Make the transition to the TPC door
  G4SubtractionSolid* solid_HV_CuRod = new G4SubtractionSolid("HV_CuRod4-DoorCut", solid_HV_CuRod4, solid_DoorCut, RotDoorCut,TransDoorCut);
  //
  G4LogicalVolume* logic_HV_CuRod = new G4LogicalVolume(solid_HV_CuRod,      // its solid
                                                        TPCCopperMaterial,	 // its material
                                                        "HV_CuRod_log");	 // its name
    
  G4VisAttributes* HV_CuRod_VisAtt= new G4VisAttributes(G4Colour(0.2,0,1));
  logic_HV_CuRod -> SetVisAttributes(HV_CuRod_VisAtt);
  phys_HV_CuRod = new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm),
                                                       logic_HV_CuRod,	     // its logical volume
                                                       "HV_CuRod",	         // its name
                                                       logic_HV_XeIn,        // mother volume
                                                       false,		         // no boolean operation
                                                       0);		             // copy number
  // check volume for overlapps
  allphysvolumes.push_back(phys_HV_CuRod);
    
  //**********************************************************************************************
  //*************************** END OF GEOMETRY UPGRADE *******************************************
  //************************************************************************************************
  
  
  //------------------------------------------------------------------------------------------------
  //---------------------------------INACTIVE LXE---------------------------------------------------
  //------------------------------------------------------------------------------------------------

  G4int numRZLXe = 12;
  G4double rmaxLXe[numRZ];
  G4double rminLXe[numRZ];
  G4double zplanesLXe[numRZ];
  G4int ix = 0;

  zplanesLXe[ix]=-49.13846/2.*cm;  rmaxLXe[ix]=22.7330*cm;  rminLXe[ix]=22.7330*cm; ix+=1; //In gap beside cap and LXe
  zplanesLXe[ix]=-1*CATH2CAPDIST;  rmaxLXe[ix]=22.7330*cm;  rminLXe[ix]=22.7330*cm; ix+=1; //To the inside of cap
  zplanesLXe[ix]=-1*CATH2CAPDIST;  rmaxLXe[ix]=22.7330*cm;  rminLXe[ix]=0; ix+=1; //To the inside of cap
  zplanesLXe[ix]=-37.86886/2.*cm;  rmaxLXe[ix]=22.7330*cm;  rminLXe[ix]=0; ix+=1; //The begenning of bend
  zplanesLXe[ix]=-37.86886/2.*cm;  rmaxLXe[ix]=22.4362*cm;  rminLXe[ix]=0; ix+=1; //IR of beginning
  zplanesLXe[ix]=-33.46500/2.*cm;  rmaxLXe[ix]=19.8120*cm;  rminLXe[ix]=0; ix+=1;

  for (int i=0;i<numRZLXe/2;i++)
  {
    zplanesLXe[numRZLXe/2+i] = -1*zplanesLXe[(numRZLXe/2)-1-i];
    rmaxLXe[numRZLXe/2+i] = rmaxLXe[(numRZLXe/2)-1-i];
    rminLXe[numRZLXe/2+i] = rminLXe[(numRZLXe/2)-1-i];
  }

  G4Polycone* solidInactiveLXe = new G4Polycone("InactiveLXe",0.*deg,360.*deg,numRZLXe,
				    zplanesLXe,rminLXe,rmaxLXe);

  G4LogicalVolume* logicInactiveLXe = new G4LogicalVolume(solidInactiveLXe,   //its solid
                                                          LXeMaterial,  //its material
                                                          "InactiveLXe");	  //its name
  G4VisAttributes* InactiveLXeVisAtt= new G4VisAttributes(G4Colour(.7,.4,1,0.3));
  logicInactiveLXe->SetVisAttributes(InactiveLXeVisAtt);
  //logicInactiveLXe->SetVisAttributes(LXeVisAtt);

  physiInactiveLXe = new G4PVPlacement(0,G4ThreeVector(),
                                       logicInactiveLXe,	//its logical volume
                                       "InactiveLXe",	//its name
                                       logicLXeVessel,	//its mother  volume
                                       false,		//no boolean operation
                                       0);		//copy number

  allphysvolumes.push_back(physiInactiveLXe);
  
  //------------------------------------------------------------------------
  //-------------------------APD FRAMES AND APDs----------------------------
  //------------------------------------------------------------------------  

  APDFrame1RM.rotateZ(30.*deg);
  APDFrame2RM.rotateZ(-30.*deg);
  APDFrame2RM.rotateX(180.*deg);

  //used for defining polygon planes
  const G4double APDFrameRadOuter[]={APDFRAMERADIUS,APDFRAMERADIUS};
  const G4double APDFrameRadInner[]={0.*cm,0.*cm};
  const G4double APDFrameZPlane[]={-1.0*APDFRAMETHICKNESS,APDFRAMETHICKNESS};
  const G4double APDTHICKNESS = 1.34*mm/2;

  //define individual pieces
  G4Polyhedra* solidAPDFrame = new G4Polyhedra("APDFrame",0.*deg,360.*deg,6,2,
				   APDFrameZPlane,APDFrameRadInner,APDFrameRadOuter);
    			     
  // The APD frames cannot be defined without the holes for the APD's
  // so....
  
  std::vector<G4UnionSolid*> solidAPDs;
  std::stringstream APDname;
  int apd_i = 0;
  
  // The APDs are actually more complicated (with beveled surfaces)
  // but the detail does not seem worth the CPU costs right now
  // The APD dimentions from R. Neilson et al., Nucl. Instrum. Meth. A608, 68 (2009)
  // 19.6 to 21.1mm diameter, 200mm^2 active area (16mm R), and 1.32 to 1.35mm thick
  G4Tubs* solidAPDHole = new G4Tubs("APDHole",0,APDRADIUS,APDFRAMETHICKNESS,0.*deg,360.*deg);
  G4Tubs* solidAPD = new G4Tubs("APD",0,APDRADIUS,APDTHICKNESS,0.*deg,360.*deg);


  //APDParamerisation takes care of the positioning of the APD on the APDFrame
  APDparam = new EXOAPDParameterisation();
  std::vector<G4TwoVector> APDpos = APDparam->GetAPDPositions();
  G4ThreeVector APDpos0 = G4ThreeVector(APDpos[0][0], APDpos[0][1],0);

  // Add APD's together in union solid so they can be subtracted from the APDFrame
  solidAPDs.push_back( new G4UnionSolid("UnionAPD1", solidAPDHole, solidAPDHole, 0,
              G4ThreeVector(APDpos[1][0] - APDpos0[0], APDpos[1][1] - APDpos0[1], 0)));
  //G4cout << "Making union holes for APDFrame" << G4endl;
  for( unsigned int i = 2; i < APDpos.size(); i++){
    APDname.str("");
    APDname << "UnionAPD" << i;
    solidAPDs.push_back( new G4UnionSolid(APDname.str(), solidAPDs.at(i-2), solidAPDHole, 0,
                G4ThreeVector(APDpos[i][0]-APDpos0[0],APDpos[i][1]-APDpos0[1],0)) );
  }

  //G4cout << " Subtracting holes from APDFrame" << G4endl;
  G4SubtractionSolid* solidHolyAPDFrame = new G4SubtractionSolid("solidHolyAPDFrame", solidAPDFrame, solidAPDs.back(),0,
          G4ThreeVector(APDpos0[0],APDpos0[1],0));

  G4LogicalVolume* logicAPDFrame = new G4LogicalVolume(solidHolyAPDFrame, TPCCopperMaterial,"APDFrame");
  logicAPDFrame->SetVisAttributes(CopperVisAtt);

  // These are the physical APDFrames
  physiAPDFrame1 = new G4PVPlacement(&APDFrame1RM,
				  G4ThreeVector(0,0,CATHODE_APDFACE_DISTANCE + APDFRAMETHICKNESS),
				  logicAPDFrame,	   //its logical volume
				  "APDFrame",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number
 
  physiAPDFrame2 = new G4PVPlacement(&APDFrame2RM,
				  G4ThreeVector(0,0,-CATHODE_APDFACE_DISTANCE - APDFRAMETHICKNESS),
				  logicAPDFrame,	   //its logical volume
				  "APDFrame",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  1);		   //copy number
    
  allphysvolumes.push_back(physiAPDFrame1);
  allphysvolumes.push_back(physiAPDFrame2);
  G4cout << " Done w/ APD code " << G4endl;

 
  //---------------------------------APDs---------------------------------
  G4LogicalVolume* logicAPD = new G4LogicalVolume(solidAPD,	  //its solid
				 APDMaterial, //its material
				 "APD");	  //its name

  //These are the APD physical volumes
  for( unsigned int i = 0; i < APDpos.size(); i++) {

    // skip APDs that are not within APDFrame
    if (hypot(APDpos[i][0],APDpos[i][1])>183*mm) continue;

    physiAPDs1 = new G4PVPlacement(0,
			  APDFrame1RM*G4ThreeVector(APDpos[i][0], APDpos[i][1], (CATHODE_APDFACE_DISTANCE + APDTHICKNESS)),
			  logicAPD,	   //its logical volume
			  "APD",	   //its name
			  logicInactiveLXe,      //its mother volume
			  false,	   //no boolean operation
			  i);		   //copy number

    physiAPDs2 = new G4PVPlacement(0,
			  APDFrame2RM*G4ThreeVector(APDpos[i][0], APDpos[i][1], (CATHODE_APDFACE_DISTANCE + APDTHICKNESS)),
				logicAPD,	   //its logical volume
				"APD",	   //its name
				logicInactiveLXe,      //its mother volume
				false,	   //no boolean operation
				1000+i);   //copy number
  }

  G4VisAttributes* APDVisAtt= new G4VisAttributes(G4Colour(1,0,0));
  APDVisAtt->SetForceSolid(true);
  logicAPD->SetVisAttributes(APDVisAtt);

  allphysvolumes.push_back(physiAPDs1);
  allphysvolumes.push_back(physiAPDs2);

  //------------------------------------------------------------------------
  //---------------------------------REFLECTOR------------------------------
  //------------------------------------------------------------------------
  
  G4Tubs* solidReflector = new G4Tubs("Reflector",REFLECTORINNERRAD,REFLECTOROUTERRAD,
           16.5608/2.*cm,0.*deg, 360.*deg);
  			     
  G4LogicalVolume* logicReflector = new G4LogicalVolume(solidReflector,    //its solid
				   TeflonMaterial,  //its material
				   "Reflector");	    //its name
  //G4VisAttributes* ReflectorVisAtt= new G4VisAttributes(G4Colour(1,1,1));
  logicReflector->SetVisAttributes(TeflonVisAtt);
  //logicReflector->SetVisAttributes(G4VisAttributes::Invisible);

  physiReflector_pz = new G4PVPlacement(0,G4ThreeVector(0,0,3.6805/2.*cm + 16.5608/2.*cm),
				  logicReflector,  //its logical volume
				  "Reflector",	   //its name
				  logicInactiveLXe,  //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number
  physiReflector_nz = new G4PVPlacement(0,G4ThreeVector(0,0,-3.6805/2.*cm - 16.5608/2.*cm),
				  logicReflector,  //its logical volume
				  "Reflector",	   //its name
				  logicInactiveLXe,  //its mother volume
				  false,	   //no boolean operation
				  1);		   //copy number
 
  allphysvolumes.push_back(physiReflector_pz);
  allphysvolumes.push_back(physiReflector_nz);

  //----------------------------------------------------------------------
  //-------------------------------Resistor Block-------------------------
  //----------------------------------------------------------------------

  G4Box* solidResistor_Block=new G4Box("Resistor_Block",
                                RESISTOR_BLOCK_THICKNESS/2.,       //half x dimension
                                RESISTOR_BLOCK_WIDTH/2.,           //half y dimension
                                RESISTOR_BLOCK_LENGTH/2.);          //half z dimension
  G4LogicalVolume* logicResistor_Block = new G4LogicalVolume(solidResistor_Block,   //its solid
				   AcrylicMaterial,                //its material 
				   "Resistor_Block");
	           //its name

  G4VisAttributes* ResistorBlockVisAtt= new G4VisAttributes(G4Colour(0.,.75,1.,0.8));
  logicResistor_Block->SetVisAttributes(ResistorBlockVisAtt);

  physiResistor_Block1 = new G4PVPlacement(0,G4ThreeVector(RESISTOR_BLOCK_X_CENTER,0,RESISTOR_BLOCK_Z_CENTER),
				  logicResistor_Block,     //its logical volume
				  "ResistorBlock",	   //its name
				  logicInactiveLXe,          //its mother volume
				  false,	           //no boolean operation
				  0);		           //copy number 	       
  physiResistor_Block2 = new G4PVPlacement(0,G4ThreeVector(RESISTOR_BLOCK_X_CENTER,0,-1.*RESISTOR_BLOCK_Z_CENTER),
				  logicResistor_Block,   //its logical volume
				  "ResistorBlock",	 //its name
				  logicInactiveLXe,        //its mother volume
				  false,	         //no boolean operation
				  1);		         //copy number


  allphysvolumes.push_back(physiResistor_Block1);
  allphysvolumes.push_back(physiResistor_Block2);

  //-----------------------------------------------------------------------
  //-------------------------------WireSupport-----------------------------
  //-----------------------------------------------------------------------

  WireSupport1RM.rotateZ(30.*deg);
  WireSupport2RM.rotateZ(-30.*deg);
  WireSupport2RM.rotateX(180.*deg);

  const G4double WireSupportRadOuter[]={WIRESUPPORT_OUTER_RADIUS,WIRESUPPORT_OUTER_RADIUS};
  const G4double WireSupportRadInner[]={WIRESUPPORT_INNER_RADIUS,WIRESUPPORT_INNER_RADIUS};
  const G4double WireSupportZPlane[]={-1.0*WIRESUPPORT_THICKNESS,WIRESUPPORT_THICKNESS};

  //define individual pieces
  G4Polyhedra* solidWireSupportPre = new G4Polyhedra("solidWireSupportPre",0.*deg,360.*deg,6,2,
                                   WireSupportZPlane,WireSupportRadInner,WireSupportRadOuter);

  G4Tubs* solidWireSupportSub = new G4Tubs("solidWireSupportSub",WIRESUPPORT_RING_IR,
          WIRESUPPORT_OUTER_RADIUS/0.867 + 0.1*cm, // OUTER_RADIUS/cos(30deg)
          WIRESUPPORT_THICKNESS,0.*deg,360.*deg);
                                   
  G4SubtractionSolid* solidWireSupport = new G4SubtractionSolid("solidWireSupport",solidWireSupportPre,solidWireSupportSub,
          0,G4ThreeVector());

  G4LogicalVolume* logicWireSupport = new G4LogicalVolume(solidWireSupport,      //its solid
          AcrylicMaterial,    //its material
          "WireSupport");    //its name

  //G4VisAttributes* WireSupportVisAtt = new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.5));
  logicWireSupport->SetVisAttributes(AcrylicVisAtt);

  physiWireSupport1 = new G4PVPlacement(&WireSupport1RM,
         G4ThreeVector(0,0,CATHODE_WIRESUPPORT_DISTANCE + WIRESUPPORT_THICKNESS),
         logicWireSupport,       //its logical volume
         "WireSupport",         //its name
         logicInactiveLXe,       //its mother volume
         false,                  //no boolean operation
         0);                     //copy number
         
  physiWireSupport2 = new G4PVPlacement(&WireSupport2RM,
         G4ThreeVector(0,0,-1*CATHODE_WIRESUPPORT_DISTANCE - WIRESUPPORT_THICKNESS),
         logicWireSupport,       //its logical volume
         "WireSupport",         //its name
         logicInactiveLXe,       //its mother volume
         false,                  //no boolean operation
         1);                     //copy number

  allphysvolumes.push_back(physiWireSupport1);
  allphysvolumes.push_back(physiWireSupport2);
  
  //------------------------------------------------------------------------
  //---------------------------Wire Support Ring ---------------------------
  //------------------------------------------------------------------------  
  
  G4Tubs* solidWireSupportRing = new G4Tubs("WireSupportRing",WIRESUPPORT_RING_IR,WIRESUPPORT_RING_OR,
          WIRESUPPORT_RING_DEPTH/2.,
          0.*deg,360.*deg);

  G4LogicalVolume* logicWireSupportRing = new G4LogicalVolume(solidWireSupportRing,
          TPCCopperMaterial, "WireSupportRing");
          
  logicWireSupportRing->SetVisAttributes(CopperVisAtt);
  
  physiWireSupportRing1 = new G4PVPlacement(&WireSupport1RM,G4ThreeVector(0,0,WIRESUPPORT_RING_Z_PLACEMENT),
          logicWireSupportRing,
          "WireSupportRing",
          logicInactiveLXe,
          false,0);
  
  physiWireSupportRing2 = new G4PVPlacement(&WireSupport2RM,G4ThreeVector(0,0,-1.*WIRESUPPORT_RING_Z_PLACEMENT),
          logicWireSupportRing,
          "WireSupportRing",
          logicInactiveLXe,
          false,1);
          
  allphysvolumes.push_back(physiWireSupportRing1);
  allphysvolumes.push_back(physiWireSupportRing2);
  
  //------------------------------------------------------------------------
  //---------------------------------Field Rings ---------------------------  
  //------------------------------------------------------------------------  

  //define individual pieces
  G4Tubs* solidField_Ring_main = new G4Tubs("Field_Ring_Bulk",FIELDRING_INNER_RADIUS,FIELDRING_OUTER_RADIUS, //bulk of ring
          FIELDRING_WIDTH/2.,  //thickness in z
          0.*deg,360.*deg);

  G4Tubs* solidField_Ring_Lip = new G4Tubs("Field_Ring_Lip",FIELDRING_LIP_INNER_RADIUS,FIELDRING_INNER_RADIUS, //lip of ring
          FIELDRING_LIP_WIDTH/2.,  //thickness in z
          0.*deg,360.*deg);
  
  G4UnionSolid* solidField_Ring_tmp = new G4UnionSolid("Field_Ring_tmp",solidField_Ring_main,solidField_Ring_Lip,0,
          G4ThreeVector(0,0,FIELDRING_WIDTH/2.-FIELDRING_LIP_WIDTH/2.));
  G4UnionSolid* solidField_Ring = new G4UnionSolid("Field_Ring",solidField_Ring_tmp,solidField_Ring_Lip,0,
          G4ThreeVector(0,0,-1*(FIELDRING_WIDTH/2.-FIELDRING_LIP_WIDTH/2.)));
    			     
  G4LogicalVolume* logicField_Ring = new G4LogicalVolume(solidField_Ring,    //its solid
				  TPCCopperMaterial,   //copper
          "Field_Ring");	    //its name
          
  //G4VisAttributes* Field_RingVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  logicField_Ring->SetVisAttributes(CopperVisAtt);

  
  //FieldRings are placed 1.82753cm + n*1.68656cm where n=0:9
  //G4VPhysicalVolume	*physiField_Ring[20];
  for (int z=0;z<10;z++)
  {
    //Positive side
    physiField_Ring[10+z] = new G4PVPlacement(0,G4ThreeVector(0,0,1.83753*cm + z*1.68656*cm+FIELDRING_WIDTH/2.),
				  logicField_Ring,	   //its logical volume
				  "Field_Ring",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  10+z);		   //copy number
    //Negitive Side
    physiField_Ring[z] = new G4PVPlacement(0,G4ThreeVector(0,0,-1.83753*cm - z*1.68656*cm-FIELDRING_WIDTH/2.),
				  logicField_Ring,	   //its logical volume
				  "Field_Ring",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  z);		   //copy number
  }
                      
  for (int z=0; z<20; z++)
  {
    allphysvolumes.push_back(physiField_Ring[z]);
  }

  //------------------------------------------------------------------------
  //---------------------------------Cathode Ring --------------------------  
  //------------------------------------------------------------------------  

  G4double CathRingIR1 = 16.7640*cm;
  G4double CathRingIR2 = 17.3761*cm;
  G4double CathRingIR3 = 18.5166*cm;
  G4double CathRingOR1 = 17.7038*cm;
  //G4double CathRingOR2 = 18.7198*cm;
  G4double CathRingOR2 = FIELDRING_OUTER_RADIUS;

  const G4int numCathRing = 8;
  G4double zplanesCathRing[] = {0.,      0.23495*cm, 0.23495*cm, 0.3302*cm,  0.3302*cm,  0.48895*cm,  0.48895*cm,  1.04013*cm};
  G4double rminCathRing[] = {CathRingIR1,CathRingIR1,CathRingIR1,CathRingIR1,CathRingIR2,CathRingIR2,CathRingIR3,CathRingIR3};
  G4double rmaxCathRing[] = {CathRingOR1,CathRingOR1,CathRingOR2,CathRingOR2,CathRingOR2,CathRingOR2,CathRingOR2,CathRingOR2};
  
  G4Polycone* solidCathodeRing = new G4Polycone("CathRingPolyCone",0.*deg,360.*deg,numCathRing,
				  zplanesCathRing,rminCathRing,rmaxCathRing);
    			     
  G4LogicalVolume* logicCathodeRing = new G4LogicalVolume(solidCathodeRing,    //its solid
				   TPCCopperMaterial,  //its material
				   "CathodeRing");	    //its name

  //G4VisAttributes* CathodeRingVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  logicCathodeRing->SetVisAttributes(CopperVisAtt);

  physiCathodeRing = new G4PVPlacement(0,G4ThreeVector(0,0,0),
				  logicCathodeRing,	   //its logical volume
				  "CathodeRing",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number     
				  
  allphysvolumes.push_back(physiCathodeRing);
  
  //
  // Dummy Vessel
  //
  
  G4double DummyCathRingIR1 = 17.8944*cm;
  G4double DummyCathRingIR2 = 18.5166*cm;
  //G4double DummyCathRingOR  = 18.7198*cm;
  G4double DummyCathRingOR  = FIELDRING_OUTER_RADIUS;
  
  const G4int numDummyCathRing = 4;
  G4double zplanesDummyCathRing[] = {0.,            -0.2540*cm,      -0.2540*cm,       -.80518*cm};
  G4double rminDummyCathRing[] = {DummyCathRingIR1, DummyCathRingIR1, DummyCathRingIR2, DummyCathRingIR2};
  G4double rmaxDummyCathRing[] = {DummyCathRingOR,  DummyCathRingOR,  DummyCathRingOR,  DummyCathRingOR};
  
  G4Polycone* solidDummyCathodeRing = new G4Polycone("DummyCathRingPolyCone",0.*deg,360.*deg,numDummyCathRing,
				  zplanesDummyCathRing,rminDummyCathRing,rmaxDummyCathRing);
          
  G4LogicalVolume* logicDummyCathodeRing = new G4LogicalVolume(solidDummyCathodeRing,    //its solid
				   TPCCopperMaterial,    //its material
				   "DummyCathodeRing");	//its name

  G4VisAttributes* DummyCathodeRingVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  logicDummyCathodeRing->SetVisAttributes(DummyCathodeRingVisAtt);

  physiDummyCathodeRing = new G4PVPlacement(0,G4ThreeVector(0,0,-0.3302*cm),
				  logicDummyCathodeRing,	   //its logical volume
				  "DummyCathodeRing",	   //its name
				  logicInactiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number     
				  
  allphysvolumes.push_back(physiDummyCathodeRing);
  
  
  //----------------------------------------------------------------------
  //---------------------------------ACTIVE LXE---------------------------
  //----------------------------------------------------------------------

  // The active liquid xenon must extend all the way to the APD plane....
  // otherwise photons do not get counted by the APDs. 
  // Why you ask? It is not our place to ask why.

  // We will subtract the volume of the wiresupport here, to avoid an overlap, and
  // because there isn't actually xenon in that region anyway.
  G4Tubs* solidPreActiveLXe1 = new G4Tubs("PreActiveLXe1",0,REFLECTORINNERRAD,CATHODE_APDFACE_DISTANCE,0.*deg,360.*deg);
  G4SubtractionSolid* solidPreActiveLXe2 = new G4SubtractionSolid("PreActiveLXe2", solidPreActiveLXe1, solidWireSupport, &WireSupport1RM, G4ThreeVector(0.,0.,CATHODE_WIRESUPPORT_DISTANCE + WIRESUPPORT_THICKNESS));
  G4SubtractionSolid* solidPreActiveLXe3 = new G4SubtractionSolid("PreActiveLXe3", solidPreActiveLXe2, solidWireSupport, &WireSupport2RM, G4ThreeVector(0.,0.,-1.0*(CATHODE_WIRESUPPORT_DISTANCE + WIRESUPPORT_THICKNESS)));

  //We will subtract out the cathode rings here
  G4SubtractionSolid* solidPreActiveLXe4 = new G4SubtractionSolid("PreActiveLXe4", solidPreActiveLXe3, solidCathodeRing, 0, G4ThreeVector(0,0,0));
  G4SubtractionSolid* solidActiveLXe = new G4SubtractionSolid("ActiveLXe", solidPreActiveLXe4, solidDummyCathodeRing, 0, G4ThreeVector(0,0,-0.3302*cm));
    
  G4LogicalVolume* logicActiveLXe = new G4LogicalVolume(solidActiveLXe,    //its solid
				   LXeMaterial,      //its material 
				   "ActiveLXe");	    //its name
  //  logicActiveLXe->SetVisAttributes(LXeVisAtt);
  logicActiveLXe->SetVisAttributes(G4VisAttributes::Invisible);

  physiActiveLXe = new G4PVPlacement(0,G4ThreeVector(0,0,0),
				  logicActiveLXe,  //its logical volume
				  "ActiveLXe",	   //its name
				  logicInactiveLXe,//its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number

  allphysvolumes.push_back(physiActiveLXe);

  //------------------------------------------------------------------------
  //---------------------------------Cathode -------------------------------
  //------------------------------------------------------------------------  

  //define individual pieces
  G4Tubs* solidCathode = new G4Tubs("Cathode",0.*cm,CATHODE_RADIUS,
				0.0125/2*cm,  //thickness in z 
				0.*deg,360.*deg);
  
    			     
  G4LogicalVolume* logicCathode = new G4LogicalVolume(solidCathode,    //its solid
				   LXeMaterial,  //its material
				   "Cathode");	    //its name

  logicCathode->SetVisAttributes(G4VisAttributes::Invisible);

  physiCathode = new G4PVPlacement(0,G4ThreeVector(0,0,0),
				  logicCathode,	   //its logical volume
				  "Cathode",	   //its name
				  logicActiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number    

				  

  allphysvolumes.push_back(physiCathode);

  //------------------------------------------------------------------------
  //---------------------------------Anode ---------------------------------
  //------------------------------------------------------------------------

  Anode1RM.rotateZ(30.*deg);
  Anode2RM.rotateZ(-30.*deg);
  Anode2RM.rotateX(180.*deg);

  //used for defining polygon planes
  const G4double AnodeRadOuter[]={WIRESUPPORT_INNER_RADIUS,WIRESUPPORT_INNER_RADIUS};
  const G4double AnodeRadInner[]={0.*cm,0.*cm};
  const G4double AnodeZPlane[]={-0.0125/2*cm,0.0125/2*cm};

  //define individual pieces
  G4Polyhedra* solidPreAnode = new G4Polyhedra("PreAnode",0.*deg,360.*deg,6,2,
                                   AnodeZPlane,AnodeRadInner,AnodeRadOuter);
  // Trim the anode to fit inside the ActiveLXe (prevent overlap
  G4IntersectionSolid* solidAnode = new G4IntersectionSolid("Anode", solidPreAnode, solidActiveLXe);
  
  G4LogicalVolume* logicAnode = new G4LogicalVolume(solidAnode,    //its solid
          LXeMaterial,  //its material
          "logicAnode");	    //its name
  logicAnode->SetVisAttributes(G4VisAttributes::Invisible);

  physiAnode1 = new G4PVPlacement(&Anode1RM,G4ThreeVector(0,0,-1.0*CATHODE_ANODE_DISTANCE),
				  logicAnode,	   //its logical volume
				  "Anode",	   //its name
				  logicActiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  0);		   //copy number 

  physiAnode2 = new G4PVPlacement(&Anode2RM,G4ThreeVector(0,0,CATHODE_ANODE_DISTANCE),
				  logicAnode,	   //its logical volume
				  "Anode",	   //its name
				  logicActiveLXe,      //its mother volume
				  false,	   //no boolean operation
				  1);		   //copy number 

  allphysvolumes.push_back(physiAnode1);
  allphysvolumes.push_back(physiAnode2);
  
  //-------------------------------------------------------------------------
  //----------------------LXE-TEFLON REFLECTOR SURFACE----------------------------
  //-------------------------------------------------------------------------

  // Define Optical surface

  G4OpticalSurface* OpReflectorSurface = new G4OpticalSurface("ReflectorSurface");
  OpReflectorSurface->SetType(dielectric_dielectric);   // surface type
  OpReflectorSurface->SetFinish(groundbackpainted);     // surface finish
  OpReflectorSurface->SetModel(unified);                // reflection model
  G4double sigma_alpha = 0.08;                       // surface roughness
  OpReflectorSurface->SetSigmaAlpha(sigma_alpha);

  // Define objects creating surface

  G4LogicalBorderSurface* ReflectorSurface_pz =
          new G4LogicalBorderSurface("ReflectorSurface",
          physiActiveLXe,physiReflector_pz,OpReflectorSurface);
  G4LogicalBorderSurface* ReflectorSurface_nz =
          new G4LogicalBorderSurface("ReflectorSurface",
          physiActiveLXe,physiReflector_nz,OpReflectorSurface);

  //
  // Generate & Add Material Properties Table attached to the optical surfaces
  //

  // The model initially checks if the photons are fresnel reflected from the rough
  // teflon surface. If not they are then either absorbed or reflected with a cos(theta)
  // distribution.

  const G4int nEntries = 3;  // number of photon energies
  G4double PhotonEnergy[nEntries] = {6.5*eV, 7.*eV, 7.5*eV};

  //  if(TeflonSurface->GetVolume1() == physiXenon) G4cout << "Equal" << G4endl;
  //  if(TeflonSurface->GetVolume2() == physiTeflon) G4cout << "Equal" << G4endl;


  //OpticalReflectorSurface 
  G4double RefractiveIndex[nEntries] = {1.4, 1.4, 1.4}; // surface refractive index
  G4double SpecularLobe[nEntries]    = {1., 1., 1.};    // specular lobe component
  G4double SpecularSpike[nEntries]   = {0., 0., 0.};    // specular spike component
  G4double Backscatter[nEntries]     = {0., 0., 0.};    // backscatter component
  G4double Reflectivity[nEntries]    = {0.74, 0.74, 0.74};  // reflectivity of "transmitted" photons

  G4MaterialPropertiesTable* myST1 = new G4MaterialPropertiesTable();
  
  myST1->AddProperty("RINDEX",                PhotonEnergy, RefractiveIndex, nEntries);
  myST1->AddProperty("SPECULARLOBECONSTANT",  PhotonEnergy, SpecularLobe,    nEntries);
  myST1->AddProperty("SPECULARSPIKECONSTANT", PhotonEnergy, SpecularSpike,   nEntries);
  myST1->AddProperty("REFLECTIVITY",          PhotonEnergy, Reflectivity,    nEntries);
  myST1->AddProperty("BACKSCATTERCONSTANT",   PhotonEnergy, Backscatter,     nEntries);

  OpReflectorSurface->SetMaterialPropertiesTable(myST1);


  //--------------------------------------------------------------------------
  //----------------------XENON-APD FRAME SURFACE-----------------------------
  //--------------------------------------------------------------------------

  // Define Optical surface

  G4OpticalSurface* OpAPDFrameSurface = new G4OpticalSurface("APDFrameSurface");
  OpAPDFrameSurface->SetType(dielectric_metal);      // surface type
  OpAPDFrameSurface->SetFinish(polished);                // surface finish
  OpAPDFrameSurface->SetModel(glisur);                // reflection model

  // Define objects creating surface

  G4LogicalSkinSurface* APDFrameSurface = 
                                 new G4LogicalSkinSurface("APDFrameSurface",
                                 logicAPDFrame,OpAPDFrameSurface);

  //
  // Generate & Add Material Properties Table attached to the optical surfaces
  //

  G4MaterialPropertiesTable* myST2 = new G4MaterialPropertiesTable();
  G4double CuReflectivity[nEntries]    = {0.85, 0.85, 0.85};  // reflectivity of APD case
  myST2->AddProperty("REFLECTIVITY",          PhotonEnergy, CuReflectivity,    nEntries);


  OpAPDFrameSurface->SetMaterialPropertiesTable(myST2); 

  //--------------------------------------------------------------------------
  //----------------------XENON-APD SURFACE-----------------------------------
  //--------------------------------------------------------------------------

  G4OpticalSurface* OpAPDSurface = new G4OpticalSurface("APDSurface");
  OpAPDSurface->SetType(dielectric_metal);      // need to set type as dielectric_metal to be able to use imaginary index of refraction
  OpAPDSurface->SetFinish(polished);            // surface finish
  OpAPDSurface->SetModel(glisur);               // reflection model
	
  G4LogicalSkinSurface* APDSurface = 
  new G4LogicalSkinSurface("APDSurface",
							 logicAPD,OpAPDSurface);
							 
  G4double ReR[nEntries] = {0.8497803194885711,0.6836017976651515,0.564222275285965};
  //numbers from Brigham Young Photonics' Website multiplied by ReR:
  G4double ImR[nEntries] = {2.32302445149589,1.676748038458553,1.2483104982656};

  G4MaterialPropertiesTable* myST4 = new G4MaterialPropertiesTable();
  myST4->AddProperty("REALRINDEX",PhotonEnergy,ReR,nEntries);  
  myST4->AddProperty("IMAGINARYRINDEX",PhotonEnergy,ImR,nEntries);
  OpAPDSurface->SetMaterialPropertiesTable(myST4);  


  //-------------------------------------------------------------------------
  //----------------------SENSITIVE DETECTORS--------------------------------
  //-------------------------------------------------------------------------

  if(SDreg) {

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String name;

    name="/EXODet/Xenon";
    SD = new EXOLXeSD(name, this);
	//SD_Cath = new EXOLXeSD(name, this);
    SDman->AddNewDetector(SD);
    logicActiveLXe->SetSensitiveDetector(SD);
	logicCathode->SetSensitiveDetector(SD); //to enable events inside cathode

    name = "/EXODet/APD";
    APDSD = new EXOAPDSD(name, this);
    SDman->AddNewDetector(APDSD);
    logicAPD->SetSensitiveDetector(APDSD);

    name = "/EXODet/VetoPanel";
    VetoPanelSD = new EXOVetoPanelSD(name, this);
    SDman->AddNewDetector(VetoPanelSD);
	// Consider a panel hit, only if the particle passes through scintillator (not shell volume!)
    logicVetoScintLong->SetSensitiveDetector(VetoPanelSD);
    logicVetoScintShort->SetSensitiveDetector(VetoPanelSD);

    name = "/EXODet/PassiveMaterial";
    PassiveMaterialSD = new EXOPassiveMaterialSD(name, this);
    SDman->AddNewDetector(PassiveMaterialSD);
	// Should we add the salt volume to PassiveMaterialSD? (M. Hughes)
    //logicSalt->SetSensitiveDetector(PassiveMaterialSD);
    logicAir->SetSensitiveDetector(PassiveMaterialSD);
	//logicCleanRm1Ext->SetSensitiveDetector(PassiveMaterialSD);
	//logicCleanRm1Int->SetSensitiveDetector(PassiveMaterialSD);
    logicSideShield->SetSensitiveDetector(PassiveMaterialSD);
    logicFrontShield->SetSensitiveDetector(PassiveMaterialSD);
    logicRearShield->SetSensitiveDetector(PassiveMaterialSD);
    logicOuterCryo->SetSensitiveDetector(PassiveMaterialSD);
    logicInnerCryo->SetSensitiveDetector(PassiveMaterialSD);
    logicHFE->SetSensitiveDetector(PassiveMaterialSD);
    logicLXeVessel->SetSensitiveDetector(PassiveMaterialSD);
    logicInactiveLXe->SetSensitiveDetector(PassiveMaterialSD);
    logicAPDFrame->SetSensitiveDetector(PassiveMaterialSD);
    logicAPDFrame->SetSensitiveDetector(PassiveMaterialSD);
    logicReflector->SetSensitiveDetector(PassiveMaterialSD);
    logicCathodeRing->SetSensitiveDetector(PassiveMaterialSD); 
    logicAnode->SetSensitiveDetector(PassiveMaterialSD);    
    logicAnode->SetSensitiveDetector(PassiveMaterialSD);  
    logicWireSupport->SetSensitiveDetector(PassiveMaterialSD); 
    logicWireSupport->SetSensitiveDetector(PassiveMaterialSD); 
    logicField_Ring->SetSensitiveDetector(PassiveMaterialSD);  

    SDreg = false;

  }

  //-------------------------------------------------------------------------
  //----------------------DETECTOR REGIONS-----------------------------------
  //-------------------------------------------------------------------------

  // DEFINE INNER REGION FOR PHYSICS CUTS TO BE FINER 
  if( fInnerRegion != 0 ) delete fInnerRegion;
  if( fInnerRegion == 0 ) fInnerRegion = new G4Region("InnerRegion");
  fInnerRegion->AddRootLogicalVolume(logicInnerCryo);

  // Define the TPC as a separate region so production cuts can be set
  // independently.

  G4Region* regTPC = new G4Region("TPC");
  regTPC->AddRootLogicalVolume(logicInactiveLXe);
  // This is never used ???? Perhaps it should be B.Mong 2012/11/6

  if ( construction_verbose == true )
    DumpGeometry();

  fNavigator->SetWorldVolume(physiWorld);
  return physiWorld;
}

void EXODetectorConstruction::DumpMaterials()
{
  G4cout << *G4Material::GetMaterialTable() << G4endl;
}

void EXODetectorConstruction::CheckOverlaps(int nStat)
{
  if (nStat==0) nStat = 1000;
  // Check all phys.volumes for overlaps
  G4int size = (G4int)allphysvolumes.size();
  for(G4int i=0;i<size;++i)
  {
    allphysvolumes.at(i)->CheckOverlaps(nStat);
  }
}

void EXODetectorConstruction::DumpGeometry(int nStat)
{
  if (nStat==0) nStat = 100000000;
  // here we would like to use nStat to control precision of mass calculation
  // but actually that is available for BoolenSolids only
  // all the rest goes with default value 1000000

  bool force_calc = true;
  // this value shoud invalidate all cached results and force a new calculation of mass
  // but somehot it doesn't work, i.e. masses doesn't change what they should from 
  // random number generator changed

    // Print mass and other useful information
    std::vector<std::string> VolumeNames;
    G4double x0, y0, z0;
    G4double x,y,z;
    G4int size = (G4int)allphysvolumes.size();
    for(G4int i=0;i<size;++i)
    {
      G4LogicalVolume* tmplogicalvolume = allphysvolumes.at(i)->GetLogicalVolume();
      if (tmplogicalvolume->GetName() == "World") continue;
      if (VolumeNames.size()==0){
        VolumeNames.push_back(tmplogicalvolume->GetName());
      }
      else if(std::find(VolumeNames.begin(), VolumeNames.end(), tmplogicalvolume->GetName()) != VolumeNames.end())
      {
        continue;
      }
      else {
        VolumeNames.push_back(tmplogicalvolume->GetName());
      }
      
      G4cout << tmplogicalvolume->GetName() << G4endl;
      G4cout << "\tMaterial = " << tmplogicalvolume->GetMaterial()->GetName() <<
      " ; Density = " << (tmplogicalvolume->GetMaterial()->GetDensity()/g)*cm3 << "g/cm^3" << G4endl;
      G4cout << "\tMass (with daughters) = " << tmplogicalvolume->GetMass(force_calc)/kg << " kg" << G4endl;
      
      //
      //G4EmCalculator calc;
      //G4cout << "\tCutRange Gamma 1mm = " << calc.ComputeEnergyCutFromRangeCut(1.0/mm, "e-", tmplogicalvolume->GetMaterial()->GetName())/eV << " eV" << G4endl;
      
      
      //Lets see if we can't subtract out daughters
      
      G4int numDau  = tmplogicalvolume->GetNoDaughters();
      G4cout << "\tNumber of daugheters = " << numDau << G4endl;
      G4double tmpvolmass = 0;
      
      for(G4int j=0;j<numDau;j++)
      {
        G4VPhysicalVolume* tmpphysvol = tmplogicalvolume->GetDaughter(j);
        G4int numCopies = tmpphysvol->GetCopyNo();
        //G4cout << "Number of copies (changing) = " << numCopies << G4endl;
        G4LogicalVolume* tmplogicdaughterofphys = tmpphysvol->GetLogicalVolume();
        //G4cout << "This daughter is " << tmplogicdaughterofphys->GetName() << G4endl;
        tmpvolmass = tmpvolmass + (tmplogicdaughterofphys->GetMass()/kg)*(1);
      }
      G4cout << "\tMass of daughters = " << tmpvolmass << " kg" << G4endl;
      G4cout << "\tMass (- daughters) = " << tmplogicalvolume->GetMass()/kg - tmpvolmass << "kg" << G4endl;
      
      //Compute radial extent determine volume center location
      G4VSolid* tmpsolid = tmplogicalvolume->GetSolid();
      G4double radius = (tmpsolid->GetExtent().GetExtentRadius());
      G4cout << "\tSpherical Containig Radius = " << radius/cm << " cm" << G4endl;
      //G4cout << "\tCenter (before rotation) = " << allphysvolumes.at(i)->GetTranslation() << G4endl;
      G4RotationMatrix* tmprot = allphysvolumes.at(i)->GetRotation();
      x0 = allphysvolumes.at(i)->GetTranslation().x();
      y0 = allphysvolumes.at(i)->GetTranslation().y();
      z0 = allphysvolumes.at(i)->GetTranslation().z();
      if (tmprot != 0)
      {
        G4double x0tmp=x0, y0tmp=y0, z0tmp=z0;
        x0 = x0tmp*tmprot->xx() + y0tmp*tmprot->xy() + z0tmp*tmprot->xz();
        y0 = x0tmp*tmprot->yx() + y0tmp*tmprot->yy() + z0tmp*tmprot->yz();
        z0 = x0tmp*tmprot->zx() + y0tmp*tmprot->zy() + z0tmp*tmprot->zz();
      }
      G4cout << "\tCenter = <" << x0/cm <<","<< y0/cm <<","<< z0/cm <<">"<< " cm" << G4endl;
    }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
//                                  MISC SHIT
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

#include "G4RunManager.hh"

void EXODetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}

void EXODetectorConstruction::SetSourcePosition( std::string SourcePosString )
{
  // Places source capsule in source tube at the specified location from TalkTo command.
  // Wants a string in the form of {#, X, Y, Z} dimentions in cm. S# must be one of the
  // source positions [2,5,8]. Hope to support the other source positions another time.
  std::istringstream os(SourcePosString);
  int source_snum;
  double source_x, source_y, source_z;
  G4ThreeVector tmp_pos_TPC = G4ThreeVector();
  os >> source_snum >> source_x >> source_y >> source_z;
  G4cout << "\n*********************************************************************" << G4endl;
  if ( !os.fail() )
  {
    switch (source_snum) {
      case 0:
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source is not deployed" << G4endl;
        G4cout << "  using default source tube positions" << G4endl;
        break;
      case 2:
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source deployed to S2" << G4endl;
        G4cout << "  Source@ <X,Y,Z> = <" << source_x << ", " << source_y << ", " << source_z << "> cm" << G4endl;
        S2_pos_TPC = G4ThreeVector(source_x*cm,source_y*cm,source_z*cm);
        S14_pos_TPC = G4ThreeVector(S14_pos_TPC.X, S14_pos_TPC.Y, source_z*cm - 0.25*2.54*cm);
        break;
      case 5:
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source deployed to S5" << G4endl;
        G4cout << "  Source@ <X,Y,Z> = <" << source_x << ", " << source_y << ", " << source_z << "> cm" << G4endl;
        S5_pos_TPC = G4ThreeVector(source_x*cm,source_y*cm,source_z*cm);
        break;
      case 8:
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source deployed to S8" << G4endl;
        G4cout << "  Source@ <X,Y,Z> = <" << source_x << ", " << source_y << ", " << source_z << "> cm" << G4endl;
        S8_pos_TPC = G4ThreeVector(source_x*cm,source_y*cm,source_z*cm);
        S20_pos_TPC = G4ThreeVector(S14_pos_TPC.X, S14_pos_TPC.Y, source_z*cm + 0.25*2.54*cm);
        break;
      case 11:
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source deployed to S11" << G4endl;
        G4cout << "  Source@ <X,Y,Z> = <" << source_x << ", " << source_y << ", " << source_z << "> cm" << G4endl;
        S11_pos_TPC = G4ThreeVector(source_x*cm,source_y*cm,source_z*cm);
        break;
      default:
        G4cout << "CRITICAL: EXODetectorConstructor::SetSourcePosition: You tried deploying to bad place" << G4endl;
        G4cout << "EXODetectorConstructor::SetSourcePosition: Source is not deployed" << G4endl;
        G4cout << "  using default source tube positions" << G4endl;
        source_snum = 0;
        // TODO throw error and stop here, if somone expects the source to be there, there is no point
        // in continuing
        break;
    }
  }
  
  // TODO: I would also like to put basic limits/checks on the source positions
  sourcePosNum = source_snum;
  G4cout << "*********************************************************************\n" << G4endl;
}

void EXODetectorConstruction::SetXenonABSL( G4double length )
{

  G4Material *xenon = G4Material::GetMaterial("LXeMaterial");
  if ( !xenon ) {
    G4cout << "can't find liquid xenon material" << G4endl;
    return;
  }

  G4cout << "setting Liquid Xenon absorption length to " << length/cm << " cm" << G4endl;

  G4MaterialPropertiesTable* xenon_mt = xenon->GetMaterialPropertiesTable();
  xenon_mt->RemoveProperty("ABSLENGTH");

  G4double xenon_PP[3]   = {6.5*eV, 7.0*eV , 7.5*eV}; // lambda range
  G4double xenon_ABSL[3] = {length, length,length}; // atten length
  xenon_mt->AddProperty("ABSLENGTH",xenon_PP, xenon_ABSL,  3);

}


void EXODetectorConstruction::SetXenonRayleigh( G4double length )

{

  G4Material *xenon = G4Material::GetMaterial("LXeMaterial");
  if ( !xenon ) {
    G4cout << "can't find liquid xenon material" << G4endl;
    return;
  }

  G4cout << "setting Liquid Xenon Rayleigh scattering length to " << length/cm
         << " cm" << G4endl;

  G4MaterialPropertiesTable* xenon_mt = xenon->GetMaterialPropertiesTable();
  xenon_mt->RemoveProperty("RAYLEIGH");

  G4double xenon_PP[3]   = { 6.5*eV, 7.*eV, 7.5*eV}; // lambda range
  G4double xenon_ABSL[3] = {length,length ,length}; // atten length
  xenon_mt->AddProperty("RAYLEIGH",xenon_PP, xenon_ABSL,  3);

}



void EXODetectorConstruction::SetXenonScintYield( G4double yield )

{

  G4Material *xenon = G4Material::GetMaterial("LXeMaterial");
  if ( !xenon ) {
    G4cout << "can't find liquid xenon material" << G4endl;
    return;
  }

  G4cout << "********************************************************" << G4endl;
  G4cout << "Doing complete photon tracking simulation" << G4endl;
  G4cout << "setting Liquid Xenon Scintillation yeild to " << yield
         << " /MeV" << G4endl;
  G4cout << "********************************************************" << G4endl;


  G4MaterialPropertiesTable* xenon_mt = xenon->GetMaterialPropertiesTable();
  xenon_mt->RemoveProperty("SCINTILLATIONYIELD");
  xenon_mt->AddConstProperty("SCINTILLATIONYIELD", yield/MeV);

}


void EXODetectorConstruction::SetXenonScintFano( G4double value )

{

  G4Material *xenon = G4Material::GetMaterial("LXeMaterial");
  if ( !xenon ) {
    G4cout << "can't find liquid xenon material" << G4endl;
    return;
  }

  G4cout << "setting Liquid Xenon Scintillation Fano factor to " << value << G4endl;

  G4MaterialPropertiesTable* xenon_mt = xenon->GetMaterialPropertiesTable();
  xenon_mt->RemoveProperty("RESOLUTIONSCALE");
  xenon_mt->AddConstProperty("RESOLUTIONSCALE",value);           // Fano factor 

}

void EXODetectorConstruction::LocateGlobalPoint(const G4ThreeVector &r, G4VTouchable *t)
{
  fNavigator->LocateGlobalPointAndUpdateTouchable(r, t, false);
}

void EXODetectorConstruction::PrintPointInformation(const G4ThreeVector &r)
{
  G4TouchableHistory hist;
  LocateGlobalPoint(r,&hist);

  if (hist.GetHistoryDepth()) {
    G4VPhysicalVolume *vol = hist.GetVolume(0);
    G4Material *mat = (vol) ? vol->GetLogicalVolume()->GetMaterial() : 0;

    // print location in global coordinate system:
    G4cout << "Point " << r/mm << " mm" << " is:\n" << " in volume ";

    if (vol) {
      G4cout << '"' << vol->GetName() <<"\":" << vol->GetCopyNo() <<'\n';
      // print location in local coordinate system of vol:
      G4ThreeVector localPosition = hist.GetHistory()->GetTopTransform().TransformPoint(r);
      G4cout << " at point " << localPosition/mm << " mm within the local coordinate system" << '\n';
    }
	else {
      G4cout << "(null)\n";
    }

    G4cout << " with material \"" << ( (mat) ? mat->GetName() : "(null)" ) << "\"\n"
           << " volume nesting:";

    for (int i=hist.GetHistoryDepth()-1;i>=0;i--) {
	  G4cout << "\n" << std::setw(hist.GetHistoryDepth()-i+2) << " \""
	         << hist.GetVolume(i)->GetName() << "\":"
	         << hist.GetVolume(i)->GetCopyNo();
    }
    G4cout << G4endl;
  }
  else {
    G4cout << "Point " << r/mm << " mm" << " is I have no idea where, sorry." << G4endl;
  }

}

const G4VPhysicalVolume* EXODetectorConstruction::FindVolume(const G4String& name, G4int copyNo) const
{
  for (unsigned i=0;i<allphysvolumes.size();i++)
    if ( allphysvolumes[i] && 
         allphysvolumes[i]->GetName()==name && 
         allphysvolumes[i]->GetCopyNo()==copyNo)
      return allphysvolumes[i];
  return 0;
}

