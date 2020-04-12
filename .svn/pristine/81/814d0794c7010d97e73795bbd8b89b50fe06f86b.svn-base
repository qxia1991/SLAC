#ifndef EXOPhysicsList_hh
#define EXOPhysicsList_hh 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
//#include "CompileTimeConstraints.hh"
#include <G4String.hh>
#include <vector>

//namespace EXO {

  class EXOPhysicsListMessenger;

  // imitate G4GenericPhysicsList
  class EXOPhysicsList: public G4VModularPhysicsList
  {
  public:
    EXOPhysicsList(G4int ver = 1);
    //EXOPhysicsList(std::vector<G4String>* physConstr, G4int ver = 1);
    ~EXOPhysicsList();

  public:
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();
    
    void ListPhysics();
    void ClearPhysics();
    void RegisterPhysics(G4VPhysicsConstructor*);
    void ReplacePhysics(G4VPhysicsConstructor*);
    void RemovePhysics(G4VPhysicsConstructor*);
    void RegisterPhysicsConstructor(const G4String&);

    static G4VModularPhysicsList* GetGenericPhysicsList(const G4String&);
    static G4VModularPhysicsList* GetReferencePhysicsList(const G4String&);

  void SetEnableFineTPC( G4bool val ) { cutInsideTPC = val ? 0.0005*CLHEP::mm : 0.; }
  void SetEnableInnerCut( G4bool val ) { cutInnerRegion = val ? 0.1*CLHEP::mm : 0.; }

    void AddParallelWorldName(G4String& pname) { fParallelWorldNames.push_back(pname); }
//    const G4String& GetParallelWorldName(G4int index) { return ; }

  private:

    EXOPhysicsListMessenger *fMessenger;
    std::vector<G4String>    fParallelWorldNames;

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4double cutForMuon;
  G4double cutForProton;
  G4double cutForNeutron;
  G4double cutForAlpha;
  G4double cutForGenericIon;
  G4double cutInsideTPC;
  G4double cutInnerRegion;

  };

//} // namespace

#endif /* EXOPhysicsList_hh */
