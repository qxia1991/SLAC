//______________________________________________________________________________
//                                                                        
// EXOMCParticleInformation 
//
// encapsulates monte carlo particle information
//                                                                        
#include "EXOUtilities/EXOMCParticleInformation.hh"
ClassImp( EXOMCParticleInformation )
//______________________________________________________________________________
void EXOMCParticleInformation::Clear( Option_t* )
{
  // Clear info, reset to initial state.
  fID               = 0; 
  fAtomicNumber     = 0; 
  fKineticEnergykeV = 0;
  fCharge           = 0;
  fExcitedStatekeV  = 0;
}
