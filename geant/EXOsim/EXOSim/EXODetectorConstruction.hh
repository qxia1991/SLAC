#ifndef EXODetectorConstruction_h
#define EXODetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "EXOSim/EXODetectorMessenger.hh"
#include <vector>

class G4Box;
class G4Tubs;
class G4Polyhedra;
class G4Polycone;
class G4Trd;       // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
class G4Torus;     // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
class G4UnionSolid;
class G4SubtractionSolid;
class G4IntersectionSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4PVParameterised;
class EXOAPDParameterisation;
class G4Material;
class G4Region;
class G4UniformMagField;
class EXOLXeSD;
class EXOAPDSD;
class EXOVetoPanelSD;
class EXOPassiveMaterialSD;
class G4ExtrudedSolid;
class G4Navigator;
class G4VTouchable;

class G4Trd;  // added for Mitchell's veto geometry
class G4Trap;  // added for Mitchell's veto geometry

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXODetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    EXODetectorConstruction();

  public:
          
     G4VPhysicalVolume* Construct();
     void SetSourcePosition( std::string );
     void DumpMaterials();
     void CheckOverlaps(int nStat = 0);
     void DumpGeometry(int nStat = 0);
     void UpdateGeometry();
     void DefineMaterials();
     void SetXenonABSL( G4double  );
     void SetXenonRayleigh( G4double  );
     void SetXenonScintYield( G4double  );
     void SetXenonScintFano( G4double  );
     enum xenonComposition_t {
       xenonG4,
       xenonNat,
       xenonEnr,
       xenonDepl
     };
     void SetConstructionVerbose( G4bool v ) { construction_verbose = v; }
     void SetXenonComposition( xenonComposition_t v ) { xenonComposition = v; }
     void SetXenonPressure( G4double v ) { xenonPressure = v; }
     void SetXenonDensity( G4double v ) { xenonDensity = v; }
     G4VPhysicalVolume* ConstructCalorimeter();    

     G4Navigator* GetNavigator() { return fNavigator; }
     void LocateGlobalPoint(const G4ThreeVector &r, G4VTouchable *t);
     void PrintPointInformation(const G4ThreeVector &r);
     
  public: 

     const G4VPhysicalVolume* GetphysiWorld()         {return physiWorld;};
     const G4VPhysicalVolume* GetSalt()               {return physiSalt;};
     const G4VPhysicalVolume* GetAir()                {return physiAir;};
     const G4VPhysicalVolume* GetOuterShield()        {return physiOuterShield;};
     const G4VPhysicalVolume* GetFrontShield()        {return physiFrontShield;};
     const G4VPhysicalVolume* GetRearShield()         {return physiRearShield;};
     const G4VPhysicalVolume* GetSideShield()         {return physiSideShield;};
     const G4VPhysicalVolume* GetAirGap()             {return physiAirGap;};
     const G4VPhysicalVolume* GetOuterCryo()          {return physiOuterCryo;};
     const G4VPhysicalVolume* GetInnerCryo()          {return physiInnerCryo;};
     const G4VPhysicalVolume* GetHFE()                {return physiHFE;};

     const G4VPhysicalVolume* GetSourceTubeS2()       {return physiSourceTubeS2;};
     const G4VPhysicalVolume* GetSourceTubeS5()       {return physiSourceTubeS5;};
     const G4VPhysicalVolume* GetSourceTubeS8()       {return physiSourceTubeS8;};
     const G4VPhysicalVolume* GetSourceTubeS11()      {return physiSourceTubeS11;};
     const G4VPhysicalVolume* GetSourceTubeS14()      {return physiSourceTubeS14;};
     const G4VPhysicalVolume* GetSourceTubeS17()      {return physiSourceTubeS17;};
     const G4VPhysicalVolume* GetSourceTubeS20()      {return physiSourceTubeS20;};
     const G4VPhysicalVolume* GetSourceTubeInS2()     {return physiSourceTubeInS2;};
     const G4VPhysicalVolume* GetSourceTubeInS5()     {return physiSourceTubeInS5;};
     const G4VPhysicalVolume* GetSourceTubeInS8()     {return physiSourceTubeInS8;};
     const G4VPhysicalVolume* GetSourceTubeInS11()    {return physiSourceTubeInS11;};
     const G4VPhysicalVolume* GetSourceTubeInS14()    {return physiSourceTubeInS14;};
     const G4VPhysicalVolume* GetSourceTubeInS17()    {return physiSourceTubeInS17;};
     const G4VPhysicalVolume* GetSourceTubeInS20()    {return physiSourceTubeInS20;};
     const G4VPhysicalVolume* GetSourceCapsule()      {return physiSourceCapsule;};
     const G4VPhysicalVolume* GetSourceInnerCapsule() {return physiSourceInnerCapsule;};
     const G4VPhysicalVolume* GetSourceBead()         {return physiSourceBead;};
     const G4VPhysicalVolume* GetTeflon()             {return physiTeflon;};
     const G4VPhysicalVolume* GetLXeVessel()          {return physiLXeVessel;};
     const G4VPhysicalVolume* GetInactiveLXe()        {return physiInactiveLXe;};
     const G4VPhysicalVolume* GetAPDFrame1()          {return physiAPDFrame1;};
     const G4VPhysicalVolume* GetAPDFrame2()          {return physiAPDFrame2;};
     const G4VPhysicalVolume* GetAPD()                {return physiAPD;};
     const G4VPhysicalVolume* GetActiveLXe()          {return physiActiveLXe;};
     const G4VPhysicalVolume* GetReflector_nz()       {return physiReflector_pz;};
     const G4VPhysicalVolume* GetReflector_pz()       {return physiReflector_nz;};
     const G4VPhysicalVolume* GetResistor_Block1()    {return physiResistor_Block1;};
     const G4VPhysicalVolume* GetResistor_Block2()    {return physiResistor_Block2;};
     const G4VPhysicalVolume* GetWireSupport1()       {return physiWireSupport1;};
     const G4VPhysicalVolume* GetWireSupport2()       {return physiWireSupport2;};
     const G4VPhysicalVolume* GetWireSupportRing1()   {return physiWireSupportRing1;};
     const G4VPhysicalVolume* GetWireSupportRine2()   {return physiWireSupportRing2;};
     const G4VPhysicalVolume* GetCathode()            {return physiCathode;};
     const G4VPhysicalVolume* GetCathodeRing()        {return physiCathodeRing;};
     const G4VPhysicalVolume* GetDummyCathodeRing()   {return physiDummyCathodeRing;};
     const G4VPhysicalVolume* GetAnode1()             {return physiAnode1;};
     const G4VPhysicalVolume* GetAnode2()             {return physiAnode2;};
     const G4VPhysicalVolume* GetFieldRing(int i)     {return physiField_Ring[i];};
 
     // This block added for M. Hughes's veto geometry
     const G4VPhysicalVolume* GetCleanRm1Ext()          {return physiCleanRm1Ext;};
     const G4VPhysicalVolume* GetCleanRm1Int()          {return physiCleanRm1Int;};
     const G4VPhysicalVolume* GetVetoSlabLong()      {return physiVetoSlabLong;};
     const G4VPhysicalVolume* GetVetoScintLong()     {return physiVetoScintLong;};
     const G4VPhysicalVolume* GetVetoSlabShort()     {return physiVetoSlabShort;};
     const G4VPhysicalVolume* GetVetoScintShort()    {return physiVetoScintShort;};
     const G4VPhysicalVolume* GetVetoLightGuideLongL()     {return physiVetoLightGuideLongL;};
     const G4VPhysicalVolume* GetVetoLightGuideLongR()     {return physiVetoLightGuideLongR;};
     const G4VPhysicalVolume* GetVetoLightGuideShortL()     {return physiVetoLightGuideShortL;};
     const G4VPhysicalVolume* GetVetoLightGuideShortR()     {return physiVetoLightGuideShortR;};
     const G4VPhysicalVolume* GetPMTCavity()         {return physiPMTCavity;};

     // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
     const G4VPhysicalVolume* GetLegs()               {return phys_Legs;};
     const G4VPhysicalVolume* GetXeLegs()             {return phys_XeLegs;};
     const G4VPhysicalVolume* GetHV_CuHous()          {return phys_HV_CuHous;};
     const G4VPhysicalVolume* GetHV_XeIn()            {return phys_HV_XeIn;};
     const G4VPhysicalVolume* GetHV_Tef()             {return phys_HV_Tef;};
     const G4VPhysicalVolume* GetHV_Cont()            {return phys_HV_Cont;};
     const G4VPhysicalVolume* GetHV_Iso()             {return phys_HV_Iso;};
     const G4VPhysicalVolume* GetHV_CuRod()           {return phys_HV_CuRod;};

     const G4VPhysicalVolume* FindVolume(const G4String& name, G4int copyNo = 0) const;
     
  private:
     
     G4Material*        defaultMaterial;
     G4Material*        SaltMaterial;
     G4Material*        AirMaterial;
     G4Material*        ShieldMaterial;
     G4Material*        TPCCopperMaterial;
     G4Material*        OuterCryoMaterial;
     G4Material*        VacuumMaterial;
     G4Material*        InnerCryoMaterial;
     G4Material*        HFEMaterial;
     G4Material*        TeflonMaterial;
     G4Material*        HDPEMaterial;  // Geometry-upgrade (TPC Legs & HV Feedthrough)
     G4Material*        AcrylicMaterial;
     G4Material*        LXeMaterial;
     G4Material*        InactiveLXeMaterial;
     G4Material*        APDMaterial;
     G4Material*        WireSupportMaterial;
     G4Material*        StainlessMaterial;
     G4Material*        EpoxyMaterial;
     G4Material*        VespelMaterial;

     // This block added for M. Hughes's veto panels
     G4Material*        Polyethylene;
     G4Material*        VetoSlabMaterial;
     G4Material*        VetoScintMaterial;
     G4Material*        BorGlassMaterial;

     G4VPhysicalVolume  *physiWorld;
     G4VPhysicalVolume  *physiSalt;
     G4VPhysicalVolume  *physiAir;
     G4VPhysicalVolume  *physiOuterShield;
     G4VPhysicalVolume  *physiFrontShield;
     G4VPhysicalVolume  *physiRearShield;
     G4VPhysicalVolume  *physiSideShield;
     G4VPhysicalVolume  *physiAirGap;
     G4VPhysicalVolume  *physiOuterCryo;
     G4VPhysicalVolume  *physiOuterCryoDoor;
     G4VPhysicalVolume  *physiOuterCryoDisk;
     G4VPhysicalVolume  *physiVacuumGap;
     G4VPhysicalVolume  *physiInnerCryo;
     G4VPhysicalVolume  *physiInnerCryoDoor;
     G4VPhysicalVolume  *physiHFE;
     G4VPhysicalVolume  *physiSourceTubeS2;
     G4VPhysicalVolume  *physiSourceTubeS5;
     G4VPhysicalVolume  *physiSourceTubeS8;
     G4VPhysicalVolume  *physiSourceTubeS11;
     G4VPhysicalVolume  *physiSourceTubeS14;
     G4VPhysicalVolume  *physiSourceTubeS17;
     G4VPhysicalVolume  *physiSourceTubeS20;
     G4VPhysicalVolume  *physiSourceTubeInS2;
     G4VPhysicalVolume  *physiSourceTubeInS5;
     G4VPhysicalVolume  *physiSourceTubeInS8;
     G4VPhysicalVolume  *physiSourceTubeInS11;
     G4VPhysicalVolume  *physiSourceTubeInS14;
     G4VPhysicalVolume  *physiSourceTubeInS17;
     G4VPhysicalVolume  *physiSourceTubeInS20;
     G4VPhysicalVolume  *physiSourceCapsule;
     G4VPhysicalVolume  *physiSourceInnerCapsule;
     G4VPhysicalVolume  *physiSourceBead;
     G4VPhysicalVolume  *physiSource;

     G4VPhysicalVolume  *physiTeflon;
     G4VPhysicalVolume  *physiLXeVessel;
     G4VPhysicalVolume  *physiInactiveLXe;
     G4VPhysicalVolume  *physiAPDFrame1;
     G4VPhysicalVolume  *physiAPDFrame2;

     EXOAPDParameterisation  *APDparam;
     G4VPhysicalVolume  *physiAPD;
     G4VPhysicalVolume  *physiAPDs1;
     G4VPhysicalVolume  *physiAPDs2;

     G4VPhysicalVolume  *physiActiveLXe;
     G4VPhysicalVolume  *physiReflector_nz;
     G4VPhysicalVolume  *physiReflector_pz;
     G4VPhysicalVolume  *physiResistor_Block1;
     G4VPhysicalVolume  *physiResistor_Block2;

     G4VPhysicalVolume  *physiWireSupport1; 
     G4VPhysicalVolume  *physiWireSupport2;
     G4VPhysicalVolume  *physiWireSupportRing1;
     G4VPhysicalVolume  *physiWireSupportRing2;
     G4VPhysicalVolume  *physiCathode;        
     G4VPhysicalVolume  *physiCathodeRing;
     G4VPhysicalVolume  *physiDummyCathodeRing;
     G4VPhysicalVolume  *physiAnode1;          
     G4VPhysicalVolume  *physiAnode2;
     G4VPhysicalVolume	*physiField_Ring[20];        
     G4VPhysicalVolume	*physiField_Ring_Lip[40];

     // This block added for M. Hughes's veto geometry
     G4VPhysicalVolume  *physiCleanRm1Ext;
     G4VPhysicalVolume  *physiCleanRm1Int;
     G4VPhysicalVolume  *physiVetoSlabLong;
     G4VPhysicalVolume  *physiVetoScintLong;
     G4VPhysicalVolume  *physiVetoSlabShort;
     G4VPhysicalVolume  *physiVetoScintShort;
     G4VPhysicalVolume  *physiVetoLightGuideLongL;
     G4VPhysicalVolume  *physiVetoLightGuideLongR;
     G4VPhysicalVolume  *physiVetoLightGuideShortL;
     G4VPhysicalVolume  *physiVetoLightGuideShortR;
     G4VPhysicalVolume  *physiPMTBodyLongL[4];
     G4VPhysicalVolume  *physiPMTBodyLongR[4];
     G4VPhysicalVolume  *physiPMTBodyShortL[4];
     G4VPhysicalVolume  *physiPMTBodyShortR[4];
     G4VPhysicalVolume  *physiPMTCavity;

     // This block added for M. Hughes's veto geometry (world volume already existed)
     G4Box              *solidWorld;
     G4Box              *solidSalt;
     G4Box              *solidAir;
     G4Box              *solidCleanRm1Ext;
     G4Box              *solidCleanRm1Int;
     G4Box              *solidVetoSlabLong;
     G4Trd              *solidVetoScintLong;
     G4Box              *solidVetoShellLong;
     G4Box              *solidVetoSlabShort;
     G4Trd              *solidVetoScintShort;
     G4Box              *solidVetoShellShort;
     G4Trap             *solidVetoLightGuide;
     G4Tubs             *solidPMTBody;
     G4Tubs             *solidPMTCavity;

     // This block added for M. Hughes's veto geometry (world volume already existed)
     G4LogicalVolume    *logicWorld;
     G4LogicalVolume    *logicSalt;
     G4LogicalVolume    *logicAir;
     G4LogicalVolume    *logicCleanRm1Ext;
     G4LogicalVolume    *logicCleanRm1Int;
     G4LogicalVolume    *logicVetoSlabLong;
     G4LogicalVolume    *logicVetoScintLong;
     G4LogicalVolume    *logicVetoShellLong;
     G4LogicalVolume    *logicVetoSlabShort;
     G4LogicalVolume    *logicVetoScintShort;
     G4LogicalVolume    *logicVetoShellShort;
     G4LogicalVolume    *logicVetoLightGuide;
     G4LogicalVolume    *logicPMTBody;
     G4LogicalVolume    *logicPMTCavity;

     // Geometry-upgrade (TPC Legs & High Voltage Feedthrough)
     G4VPhysicalVolume  *phys_Legs;
     G4VPhysicalVolume  *phys_XeLegs;
     G4VPhysicalVolume  *phys_HV_CuHous;
     G4VPhysicalVolume  *phys_HV_XeIn;
     G4VPhysicalVolume  *phys_HV_Tef;
     G4VPhysicalVolume  *phys_HV_Cont;
     G4VPhysicalVolume  *phys_HV_Iso;
     G4VPhysicalVolume  *phys_HV_CuRod;

     std::vector<G4VPhysicalVolume*> allphysvolumes; //vector of pointers to physical volumes

	 // This section added for M. Hughes' veto geometry
     G4RotationMatrix vetoLightGuideRML;
     G4RotationMatrix vetoLightGuideRMR;
     G4RotationMatrix PMTBodyRML;
     G4RotationMatrix PMTBodyRMR;
     G4RotationMatrix vetoTopBotRM;
     G4RotationMatrix vetoWestRM;
     G4RotationMatrix vetoSouthRM;

     G4RotationMatrix LeadShieldRM;
     G4RotationMatrix cryostatRM;
     G4RotationMatrix LXeVesselRM;
     G4RotationMatrix SourceTube_verticalRM;
     G4RotationMatrix APDFrame1RM;
     G4RotationMatrix APDFrame2RM;
     G4RotationMatrix WireSupport1RM;     
     G4RotationMatrix WireSupport2RM;     
     G4RotationMatrix WSClone1RM;     
     G4RotationMatrix WSClone2RM;     
     G4RotationMatrix Anode1RM;           
     G4RotationMatrix Anode2RM;          
      
     EXOLXeSD             *SD;
     EXOAPDSD             *APDSD;
     EXOVetoPanelSD       *VetoPanelSD;
     EXOPassiveMaterialSD *PassiveMaterialSD;

     G4bool             SDreg;
     xenonComposition_t xenonComposition;
     G4double           xenonPressure;
     G4double           xenonDensity;
     G4bool             construction_verbose;
     
     G4ThreeVector      S2_pos_TPC;
     G4ThreeVector      S5_pos_TPC;
     G4ThreeVector      S8_pos_TPC;
     G4ThreeVector      S11_pos_TPC;
     G4ThreeVector      S14_pos_TPC;
     G4ThreeVector      S17_pos_TPC;
     G4ThreeVector      S20_pos_TPC;
     G4int              sourcePosNum;
     G4ThreeVector      sourcePosXYZ;

     EXODetectorMessenger  fDetectorMessenger;  //pointer to the Messenger
     
     G4Region*          fInnerRegion;
     G4Navigator       *fNavigator;

};

#endif

