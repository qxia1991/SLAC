//********Parallel World Construction for Event Biasing********//

 
#include "EXOSim/EXOParallelDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PVReplica.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4ios.hh"
#include "G4IStore.hh"
#include "G4GeometrySampler.hh"
#include "G4Version.hh"
#include <sstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOParallelDetectorConstruction::EXOParallelDetectorConstruction(G4String worldName)
:G4VUserParallelWorld(worldName), defaultMaterial(0), shadowWorld(0) 
{

  DefineMaterials();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOParallelDetectorConstruction::~EXOParallelDetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EXOParallelDetectorConstruction::DefineMaterials()
{

  // Everything is made of vacuum!

  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;  

  G4int ncomponents;
  G4double fractionmass;

  //--------------------------------------------------//
  //------------- DEFINE ELEMENTS --------------------//
  //--------------------------------------------------//

  G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
  G4Element* Ar = new G4Element("Argon"  ,symbol="Ar" ,z= 18., a= 39.95*g/mole);  

  //--------------------------------------------------//
  //------------- DEFINE MATERIALS -------------------//
  //--------------------------------------------------//

  G4Material* Air = new G4Material("Air",density= 1.25*mg/cm3,ncomponents=3);
  Air->AddElement(N, fractionmass=0.758);
  Air->AddElement(O, fractionmass=0.232);  
  Air->AddElement(Ar, fractionmass=0.010);

  G4Material* Vacuum = new G4Material("Vacuum",density= 1.e-11*g/cm3,ncomponents=1);
  Vacuum->AddMaterial(Air, fractionmass=1.);

  //--------------------------------------------------//
  //------------- MATERIALS OF EVERYTHING ------------//
  //--------------------------------------------------//

  defaultMaterial  = Vacuum;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EXOParallelDetectorConstruction::Construct()
{

  G4String name("none");

  //--------------------------------------------------//
  //---------------------- WORLD ---------------------//
  //--------------------------------------------------//

  G4cout << "****** Constructing Parallel World ****** " << G4endl;

  // Use GetWorld methods to create a clone of the mass world to the
  // parallel world

  shadowWorld = GetWorld();
  G4LogicalVolume *worldLogical = shadowWorld->GetLogicalVolume();
  fPVolumeStore.AddPVolume(G4GeometryCell(*shadowWorld,0));

  //--------------------------------------------------//
  //------------------- OUTER VOLUME -----------------//
  //--------------------------------------------------//

  G4Box* solidOuter = new G4Box("ParaOuterVolume",      //its name
                         WORLDSIZEX-0.01,        //X half-length
                         WORLDSIZEY-0.01,        //Y half-length
                         WORLDSIZEZ-0.01);       //Z half-length
  // (the -0.01 is to avoid overlap with world volume)

  G4LogicalVolume* logicOuter = new G4LogicalVolume(solidOuter,         //its solid
                                   defaultMaterial,    //its material
                                   "ParaOuterVolume"); //its name

  name = GetCellName(1);   // cell_1

  G4VPhysicalVolume* physiOuter = new G4PVPlacement(0,                    //no rotation
                                 G4ThreeVector(0,0,0), 
                                 logicOuter,           //its logical volume
                                 name,                 //its name
                                 worldLogical,         //its mother volume
                                 false,                //no boolean operation
                                 0);                   //copy number

  G4GeometryCell outer(*physiOuter,0);
  fPVolumeStore.AddPVolume(outer);
  fCells.push_back(physiOuter);

  G4cout << "   created ParaOuterVolume, (" << name << ")" << G4endl;


  //--------------------------------------------------//
  //-------------------- LEAD SHIELD -----------------//
  //--------------------------------------------------//

  // Consists in fact in 13 volumes, creating 13 cells boundaries within the mass geometry's lead shield, 
  // each separated by about 2*cm (approximatly the absorbtion length of lead for gamma between 2 and 3 MeV). 

  // The first cell boundary is at the lead shield's outer boundary and the last cells boundary is at the 
  // lead shield's inner boundary.

  cryostatRM.rotateY(90.*deg);
  cryostatRM.rotateZ(15.*deg);

  // should be identical as 'leadShieldXPosition' in EXODetectorConstruction.cc
  const G4double leadShieldXPosition = -(VACUUM_GAP_THICKNESS_FRONT-VACUUM_GAP_THICKNESS_REAR)/2.
                                       -(AIR_GAP_THICKNESS_FRONT-AIR_GAP_THICKNESS_REAR)/2.;

  G4int numberOfShieldVol = 13;

  G4double cellsThicknessR = LEADTHICKNESSSIDES / (numberOfShieldVol-1);         // = 2.058333 cm
  G4double cellsThicknessZ = LEADTHICKNESSFRONTANDREAR / (numberOfShieldVol-1);  // = 2.083333 cm


  std::vector<G4LogicalVolume*>   logicShield(numberOfShieldVol);
  for(G4int i=0; i<numberOfShieldVol; i++){

    std::ostringstream volName;
    volName << "ParaShield" << i;

    const G4double LeadRadOuter[]={LEADOUTERRAD-(i)*cellsThicknessR, LEADOUTERRAD-(i)*cellsThicknessR};
    const G4double LeadRadInner[]={0.0,0.0};
    const G4double LeadZPlane[]={-LEADHEIGHT-LEADTHICKNESSFRONTANDREAR+i*cellsThicknessZ,LEADHEIGHT+LEADTHICKNESSFRONTANDREAR-i*cellsThicknessZ};


    G4Polyhedra* solidShield = new G4Polyhedra(volName.str().c_str(),0.*deg,360.*deg,LEADSIDES,2,LeadZPlane,LeadRadInner,LeadRadOuter);

    logicShield[i] = new G4LogicalVolume(solidShield,	 //its solid
		     		         defaultMaterial,	 //its material
				         volName.str().c_str()); //its name

    G4VisAttributes* RedVisAtt= new G4VisAttributes(G4Colour(1.0,0,0));
    logicShield[i]->SetVisAttributes(RedVisAtt);

    name = GetCellName(2+i);   // cell_2 to cell_14

    G4VPhysicalVolume* physiShield = NULL;
    if(i==0){
      physiShield    = new G4PVPlacement(&cryostatRM,
                                         G4ThreeVector(leadShieldXPosition,0,0),
                                         logicShield[i],   //its logical volume
                                         name,	           //its name
                                         logicOuter,       //its mother volume
                                         false,            //no boolean operation
                                         0);               //copy number 
    }
    else{
      physiShield    = new G4PVPlacement(0,
                                         G4ThreeVector(0,0,0),
                                         logicShield[i],   //its logical volume
                                         name,	           //its name
                                         logicShield[i-1], //its mother volume
                                         false,            //no boolean operation
                                         0);               //copy number 
    }
		 

    G4GeometryCell shield(*physiShield,0);
    fPVolumeStore.AddPVolume(shield);
    fCells.push_back(physiShield);

    G4cout << "   created " << volName.str().c_str() << " volume, (" << name << ")" << G4endl;
    G4cout << "       z = [" << (LeadZPlane[0]+leadShieldXPosition)/cm << "cm," << (LeadZPlane[1]+leadShieldXPosition)/cm << "]cm, r = " << LeadRadOuter[0]/cm << "cm" << G4endl;

  }



  //--------------------------------------------------//
  //-------------------- VACUUM GAP ------------------//
  //--------------------------------------------------//

  //used for defining polygon planes
  const G4double VacuumGapRadOuter[]={0.5*(CUCRY1INNERRAD+CUCRY2OUTERRAD),
                                      0.5*(CUCRY1INNERRAD+CUCRY2OUTERRAD)};
  const G4double VacuumGapRadInner[]={0.*cm,0.*cm};
  const G4double VacuumGapZPlane[]={-CUCRY2HEIGHT-0.5*VACUUM_GAP_THICKNESS_REAR,
  				     CUCRY2HEIGHT+0.5*VACUUM_GAP_THICKNESS_FRONT};

  G4Polyhedra* solidVacuumGap = new G4Polyhedra("ParaVacuumGap",90.*deg,360.*deg,LEADSIDES,2,
				   VacuumGapZPlane,VacuumGapRadInner,VacuumGapRadOuter);

  G4LogicalVolume* logicVacuumGap = new G4LogicalVolume(solidVacuumGap,   //its solid
				       defaultMaterial,  //its material
				       "ParaVacuumGap"); //its name

  logicVacuumGap->SetVisAttributes(G4VisAttributes::Invisible); 

  name = GetCellName(numberOfShieldVol+2);   // cell_15

  G4VPhysicalVolume* physiVacuumGap = new G4PVPlacement(0,
                                     G4ThreeVector(0,0,leadShieldXPosition),
                                     logicVacuumGap,                   //its logical volume
                                     name,                             //its name
                                     logicShield[numberOfShieldVol-1], //its mother volume
                                     false,                            //no boolean operation
                                     0);                               //copy number 	       

  G4GeometryCell vacuumGap(*physiVacuumGap,0);
  fPVolumeStore.AddPVolume(vacuumGap);
  fCells.push_back(physiVacuumGap);

  G4cout << "   created ParaVacuumGap volume, (" << name << ")" << G4endl;



  //--------------------------------------------------//
  //---------------------- HFE -----------------------//
  //--------------------------------------------------//


  const G4double attenuationLength = 16.*cm;
  G4LogicalVolume* lastVol = logicVacuumGap;
  char name_buf[16];
  for (int step=0;step<3;step++ ) { // checkout name_buf size
      G4double stepIn = step*attenuationLength;
      const G4double HFERadOuter[]={CUCRY2INNERRAD-stepIn,CUCRY2INNERRAD-stepIn};
      const G4double HFERadInner[]={0.*cm,0.*cm};
      const G4double HFEZPlane[]={-1.0*(CUCRY2HEIGHT-CUCRY2THICKNESS-stepIn),CUCRY2HEIGHT-CUCRY2THICKNESS-stepIn};
      
      snprintf(name_buf, sizeof(name_buf), "ParaHFE_%i", step);
      G4Polyhedra* solidHFE = new G4Polyhedra(name_buf,90.*deg,360.*deg,LEADSIDES,2,
            		                  HFEZPlane,HFERadInner,HFERadOuter);
      
      G4LogicalVolume* logicHFE = new G4LogicalVolume(solidHFE, 
                                                      defaultMaterial, 
                                                      name_buf);
      
      logicHFE->SetVisAttributes(G4VisAttributes::Invisible);
      
      name = GetCellName(numberOfShieldVol+3+step);   // cell_16
      
      G4VPhysicalVolume* physiHFE = new G4PVPlacement(0,G4ThreeVector(),
                                   logicHFE,
                                   name,
                                   lastVol,
                                   false,0);
      
      G4GeometryCell HFE(*physiHFE,0);
      fPVolumeStore.AddPVolume(HFE);
      fCells.push_back(physiHFE);
      
      G4cout << "   created ParaHFE volume, (" << name << ")" << G4endl;
      lastVol = logicHFE;
    }



}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const G4VPhysicalVolume &EXOParallelDetectorConstruction::GetPhysicalVolumeByName(const G4String& name) const {
  return *fPVolumeStore.GetPVolume(name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String EXOParallelDetectorConstruction::ListPhysNamesAsG4String() {
  G4String names(fPVolumeStore.GetPNames());
  return names;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4String EXOParallelDetectorConstruction::GetCellName(G4int i) 
{
  std::ostringstream os;
  os << "cell_";
  os << i;
  G4String name = os.str();
  return name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4GeometryCell EXOParallelDetectorConstruction::GetGeometryCell(G4int i) 
{

  const G4VPhysicalVolume *p=0;
  if (i < (G4int) fCells.size()) p = fCells[i];
  if (p) return G4GeometryCell(*p,0);
  else {
    G4cout << "EXOParallelDetectorConstruction::GetGeometryCell:  couldn't get G4GeometryCell" << G4endl;
    return G4GeometryCell(*shadowWorld,-2);
  }
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume &EXOParallelDetectorConstruction::GetWorldVolume() const 
{
  return *shadowWorld;
}

void EXOParallelDetectorConstruction::SetupBiasing(double isFactor)
{
  
  // Create importance store
#if 0+G4VERSION_NUMBER < 1000
  G4IStore* aIstore = new G4IStore(*shadowWorld);
#else
  G4IStore* aIstore = G4IStore::GetInstance(shadowWorld->GetName()); // not tested
#endif
  
  // Create geometry cell from the world volume
  G4GeometryCell gWorldVolumeCell(*shadowWorld,0);
  
  // Set the importance for World cell
  aIstore->AddImportanceGeometryCell(1., gWorldVolumeCell);
  // Set the importance for Outer Volume and Shield 1 to Shield 13 cells
  for(size_t i=0; i<GetNumCells(); i++){
    G4double importance = std::pow(isFactor,(int)i);
    G4GeometryCell cell = GetGeometryCell(i);
    aIstore->AddImportanceGeometryCell(importance, cell);  
    G4cout << "    set importance of " << cell.GetPhysicalVolume().GetName() 
           << " to " << importance << G4endl;
  }
  
  // Create the importance sampler from the biasing in the parallel world
  G4GeometrySampler* pgs = new G4GeometrySampler(shadowWorld,"gamma");
  pgs->SetParallel(true);
  pgs->PrepareImportanceSampling(aIstore,0);
  pgs->Configure();

}

