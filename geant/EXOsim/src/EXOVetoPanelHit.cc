#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "EXOSim/EXOVetoPanelHit.hh"

G4Allocator<EXOVetoPanelHit> EXOVetoPanelHitsAllocator;


EXOVetoPanelHit::EXOVetoPanelHit() : EXOVHit() // This sets time, weight, and mirror number to defaults.
{
  position[0] = 0.0;
  position[1] = 0.0;
  position[2] = 0.0;
  hitEnergy = 0.0;
  particleName = "";
  panelID = -1;
}

