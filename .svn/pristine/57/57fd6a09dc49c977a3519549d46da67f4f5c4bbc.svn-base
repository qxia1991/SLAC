#ifndef EXOMCPixelatedChargeDeposit_hh
#define EXOMCPixelatedChargeDeposit_hh
#include <set>
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#include "EXOUtilities/EXOCoordinateKey.hh"

class EXOCoordinates;

class EXOMCPixelatedChargeDeposit : public TObject 
{
  // Class to encapsulate all EXO Monte Carlo Pixelated Charge Deposit Info  
  public:
    Double_t       fTotalEnergy;           //etq : Charge pixel total energy in CLHEP units (MeV = 1).
    Double_t       fTotalIonizationEnergy; //eiq : Charge pixel ionization energy in CLHEP units (MeV = 1), accounting for quenching in alpha events and smearing due to LXe resolution.
    Double_t       fWireHitTime;  // Time at which the charge was deposited on the U wire
    Double_t       fWeight;       // Weight of a hit.  In principal, all of these must be equal for an event. 
    
    std::vector<Int_t>       fAncestorParticleType; //Array of ancestor particle types
    std::vector<Double_t>     fAncestorParticleEnergy; //Array of ancestor particle energies
    
    Int_t fDepositChannel;  // Software U-channel on which the ionized charge deposited. (If it deposited on a V-channel, then we set this value to +999 since there is no direct correspondence between drifting and real geometry.  If no wire is hit, the value is -999.)
    std::set<Int_t> fWireChannelsAffected;   // Set containing all (software) channel numbers of the wires (U and V) whose waveforms were influenced by this charge deposit.
    
    Int_t fNumPhot;   // Number of PE Absportion contained (for rough scatter tracking)
    Int_t fNumCompt;  // Number of Compton Scatters contained (for rough scatter tracking)

  public:
    EXOMCPixelatedChargeDeposit() : TObject() { EXOMCPixelatedChargeDeposit::Clear(); }
    EXOMCPixelatedChargeDeposit(const EXOCoordinates& Coord);
    virtual void Clear( Option_t* = "" );

    void SetCoordinates(const EXOCoordinates& Coord);
    EXOCoordinates GetPixelCenter() const;
    const EXOCoordinateKey& GetPixelCoordinateKey() const;
    void AddPCDUHit(int ch, double energy);
    std::vector<Int_t> GetPCDHitChArray(void) {return fPCDHitChannels;};
    std::vector<Double_t> GetPCDHitEnergyArray(void) {return fPCDHitEnergy;}; 

  protected:
    EXOCoordinateKey fCoordinateKey;
    //With Diffusion a single PCD can hit more than one channel so need to track this better
    std::vector<Int_t>    fPCDHitChannels;
    std::vector<Double_t> fPCDHitEnergy;

  ClassDef( EXOMCPixelatedChargeDeposit, 10)
};

#endif /* EXOMCPixelatedChargeDeposit_hh */
