//______________________________________________________________________________
//
// EXOComptonInfo
//
// Class encapsulating information for compton scattering, telescope, etc. 
//

#include "EXOUtilities/EXOComptonInfo.hh"

ClassImp( EXOComptonInfo )
//______________________________________________________________________________
void EXOComptonInfo::Clear( Option_t* )
{
  // Clear, reset to initial state
  fNumClustersUsed = 0;     
  fEnergy = 0.0;           
  fFOM = 0.0;              
  fHalfAnglePhi = 0.0;     
  fHalfAnglePhiErr = 0.0;  
  fX1 = 0.0;               
  fY1 = 0.0;               
  fZ1 = 0.0;               
  fX1Err = 0.0;            
  fY1Err = 0.0;            
  fZ1Err = 0.0;            
  fX2 = 0.0;               
  fY2 = 0.0;               
  fZ2 = 0.0;               
  fX2Err = 0.0;            
  fY2Err = 0.0;            
  fZ2Err = 0.0;            
}

//______________________________________________________________________________
bool EXOComptonInfo::operator==(const EXOComptonInfo& other) const
{
  // Do deep equivalence check
  return fNumClustersUsed == other.fNumClustersUsed and     
         fEnergy == other.fEnergy and           
         fFOM == other.fFOM and              
         fHalfAnglePhi == other.fHalfAnglePhi and     
         fHalfAnglePhiErr == other.fHalfAnglePhiErr and  
         fX1 == other.fX1 and               
         fY1 == other.fY1 and               
         fZ1 == other.fZ1 and               
         fX1Err == other.fX1Err and            
         fY1Err == other.fY1Err and            
         fZ1Err == other.fZ1Err and            
         fX2 == other.fX2 and               
         fY2 == other.fY2 and               
         fZ2 == other.fZ2 and               
         fX2Err == other.fX2Err and            
         fY2Err == other.fY2Err and            
         fZ2Err == other.fZ2Err;
}
