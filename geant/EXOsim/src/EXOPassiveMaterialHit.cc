#include "EXOSim/EXOPassiveMaterialHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<EXOPassiveMaterialHit> EXOPassiveMaterialHitsAllocator;

EXOPassiveMaterialHit::EXOPassiveMaterialHit() : 
    materialID(0),
    etotal(0.0)
{  
}

void EXOPassiveMaterialHit::Draw()  {
}

void EXOPassiveMaterialHit::Print() {
}



