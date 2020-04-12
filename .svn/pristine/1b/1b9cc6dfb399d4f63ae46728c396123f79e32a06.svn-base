#include "EXOSim/EXODetectorMessenger.hh"
#include "EXOSim/EXODetectorConstruction.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

EXODetectorMessenger::EXODetectorMessenger(EXODetectorConstruction* EXODet) : 
  EXODetector(EXODet),
  fDirectory("/detector/"),
  XenonABSLCmd("/detector/setXenonABSL",this),
  XenonRayleighCmd("/detector/setXenonRayleigh",this),
  XenonScintYieldCmd("/detector/setXenonScintYield",this),
  XenonScintFanoCmd("/detector/setXenonScintFano",this),
  fDumpMaterialsCmd("/detector/dumpMaterials",this),
  fCheckOverlapsCmd("/detector/checkOverlaps",this),
  fDumpGeometryCmd("/detector/dumpGeometry",this),
  fTestPointCmd("/geometry/test/point_test",this)
{

  fDirectory.SetGuidance("Detector commands");

  XenonABSLCmd.SetGuidance("Set the Liquid Xenon absorption length for scintillation");
  XenonABSLCmd.SetParameterName("XenonABSL",false);
  XenonABSLCmd.SetDefaultUnit("cm");
  XenonABSLCmd.SetUnitCategory("Length");
  XenonABSLCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  XenonRayleighCmd.SetGuidance("Set the Liquid Xenon Rayleigh Scattering length");
  XenonRayleighCmd.SetParameterName("XenonRayleigh",false);
  XenonRayleighCmd.SetDefaultUnit("cm");
  XenonRayleighCmd.SetUnitCategory("Length");
  XenonRayleighCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  XenonScintYieldCmd.SetGuidance("Set the Liquid Xenon scintillation yield (photons/MeV)");
  XenonScintYieldCmd.SetParameterName("XenonScintYield",false);
  XenonScintYieldCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  XenonScintFanoCmd.SetGuidance("Set the Liquid Xenon scintillation Fano Factor");
  XenonScintFanoCmd.SetParameterName("XenonScintFano",false);
  XenonScintFanoCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  fDumpMaterialsCmd.SetGuidance("Dump all materials table.");
  fDumpMaterialsCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  fCheckOverlapsCmd.SetGuidance("Execute a test of geometry for volumes overlaps.");
  fCheckOverlapsCmd.SetParameterName("nStat",true);
  fCheckOverlapsCmd.SetDefaultValue(0);
  fCheckOverlapsCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  fDumpGeometryCmd.SetGuidance("Dump all geometry tree with all parameters.");
  fDumpGeometryCmd.SetParameterName("nStat",true);
  fDumpGeometryCmd.SetDefaultValue(0);
  fDumpGeometryCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

  fTestPointCmd.SetGuidance("Print volume and material at specified position.");
  fTestPointCmd.SetParameterName("X", "Y", "Z", false);
  fTestPointCmd.SetDefaultUnit("mm");
  fTestPointCmd.SetUnitCategory("Length");
  fTestPointCmd.AvailableForStates(G4State_PreInit,G4State_Idle);

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXODetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if        ( command == &XenonABSLCmd ) {
    EXODetector->SetXenonABSL(XenonABSLCmd.GetNewDoubleValue(newValue));
  } else if ( command == &XenonRayleighCmd ) {
    EXODetector->SetXenonRayleigh(XenonRayleighCmd.GetNewDoubleValue(newValue));
  } else if ( command == &XenonScintYieldCmd ) {
    EXODetector->SetXenonScintYield(XenonScintYieldCmd.GetNewDoubleValue(newValue));
  } else if ( command == &XenonScintFanoCmd ) {
    EXODetector->SetXenonScintFano(XenonScintFanoCmd.GetNewDoubleValue(newValue));
  } else if ( command == &fDumpMaterialsCmd ) {
    EXODetector->DumpMaterials();
  } else if ( command == &fCheckOverlapsCmd ) {
    EXODetector->CheckOverlaps(fCheckOverlapsCmd.GetNewIntValue(newValue));
  } else if ( command == &fDumpGeometryCmd ) {
    EXODetector->DumpGeometry(fDumpGeometryCmd.GetNewIntValue(newValue));
  } else if ( command == &fTestPointCmd ) {
    EXODetector->PrintPointInformation(fTestPointCmd.GetNew3VectorValue(newValue));
  }

}
