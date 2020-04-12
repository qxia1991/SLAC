#include "EXOSim/EXOAPDHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<EXOAPDHit> EXOAPDHitsAllocator;

EXOAPDHit::EXOAPDHit() {  
  gangNo = 0;
  time = 0.0;
}

void EXOAPDHit::Draw()  {
}

void EXOAPDHit::Print() {
}



