#ifndef EXORunAction_h
#define EXORunAction_h 1
#include "G4UserRunAction.hh"
#include "globals.hh"

#include "EXOSim/EXORunActionMessenger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TTree;
class G4Run;
class EXOStackingAction;

class EXORunAction : public G4UserRunAction
{
  public:

    EXORunAction( EXOStackingAction *sa );

  private:

    EXORunActionMessenger runActionMessenger;
    EXOStackingAction *stackAction;
  
    G4bool kill_low_energy_particles;
    G4bool anticorr_alpha;
    G4bool fastscint_alpha;
    G4double e_field_alpha;
    G4double run_id;
    G4double nex_alpha;

    G4double x;
    G4double y;
    G4double z;
    G4double energy;
    TTree* energy_tree;
    G4double fracEnergy;

  public:

    void setFrac(G4double frac){fracEnergy = frac;}
    G4double returnFrac(){return fracEnergy;}
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void set_kill_low_energy_particles( G4bool val );
    void set_anticorr_alpha( G4bool val );
    void set_efield_alpha( G4double val );
    void set_nex_alpha( G4double val);
    void set_fastscint_alpha(G4bool val);
    TTree* ReturnEnergyTree(){return energy_tree;}
    void FillEnergyTree(G4double,G4double,G4double,G4double);
    void DeleteTree();

    //void CountProcesses(G4String);
    G4int increment;
    G4bool returnAlphaAnticorr() const {return anticorr_alpha;}
    void countIncrement(){increment++;}
    G4int returnIncrement() const {return increment;}
    G4double returnAlphaEField() const {return e_field_alpha;}
    G4bool returnFastScintAlpha() const{return fastscint_alpha;}
    G4double returnRunID() const {return run_id;}
    G4double returnNexAlpha() const {return nex_alpha;}

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

