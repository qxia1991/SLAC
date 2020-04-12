#ifndef EXOSpecialPhysics_hh
#define EXOSpecialPhysics_hh 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EXOSpecialPhysics : public G4VPhysicsConstructor
{
public:

  EXOSpecialPhysics(G4int ver = 1);

  // obsolete
  EXOSpecialPhysics(G4int ver, const G4String& name);

  ~EXOSpecialPhysics();

  void ConstructParticle();
  void ConstructProcess();

private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
