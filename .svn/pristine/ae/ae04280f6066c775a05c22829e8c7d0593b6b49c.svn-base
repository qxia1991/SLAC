#ifndef EXOMCParticleInformation_hh
#define EXOMCParticleInformation_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class EXOMCParticleInformation : public TObject 
{

  // Class to encapsulate all EXO Monte Carlo Particle Information
  public:
    Int_t          fID;                /*idpart : Particle species. 0 = nucleus, 1 = e-,\
                                                : 2 = e+, 3 = gamma, 4 = alpha, 5 = neutrino, 6 = muon, 7 = other. */
    Int_t          fAtomicNumber;      //apart  : Particle atomic number.
    Double_t       fKineticEnergykeV;  //epart  : Particle kinetic energy in keV.
    Double_t       fCharge;            //qpart  : Particle charge.
    Double_t       fExcitedStatekeV;   //expart : Particle excited state energy in keV.     

  public:
    EXOMCParticleInformation() : TObject() { EXOMCParticleInformation::Clear(); }
    virtual void Clear( Option_t* = "" ); 

  ClassDef( EXOMCParticleInformation, 1 )

};

#endif /* EXOMCParticleInformation_hh*/
