#ifndef EXONeutronPhysics_hh
#define EXONeutronPhysics_hh 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXONeutronPhysics : public G4VPhysicsConstructor
{
public:

  EXONeutronPhysics(G4int ver = 1);

  // obsolete
  EXONeutronPhysics(G4int ver, const G4String& name);

  ~EXONeutronPhysics();

  void ConstructParticle();
  void ConstructProcess();

private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
