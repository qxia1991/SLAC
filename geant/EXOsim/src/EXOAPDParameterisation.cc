#include "EXOSim/EXOAPDParameterisation.hh"
#include "EXOUtilities/EXODimensions.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOAPDParameterisation::EXOAPDParameterisation()
{
   xAPDcenter = 0; 
   yAPDcenter = 0;
   //   zAPDcenter = -CATHODE_APDPLANE_DISTANCE - APDHEIGHT + 1.5*mm;
   zAPDcenter = APDFRAMETHICKNESS - APDHEIGHT;

  // create array of APD positions

  const G4int nAPDrows = 21; // number of rows of APDs
  G4int APDrowsLeft[nAPDrows] = {2,3,5,5,7,7,9,9,9,8,9,8,8,8,8,7,8,7,5,2,0}; // APDs in each row
  G4int APDrowsRight[nAPDrows] = {0,2,4,7,7,7,7,8,7,8,8,8,8,9,8,7,6,5,4,3,2}; // APDs in each row    
  G4int APDnumber = 0;
  for (G4int i=0; i < nAPDrows; i++) {
    for (G4int j=0; j < (APDrowsLeft[i]+1); j++) {
      if(APDrowsRight[i] != 0) {
      for (G4int k=0; k < APDrowsRight[i]; k++) {
	if (j != APDrowsLeft[i] && k == 0) {        
          fAPDy[APDnumber] = ((nAPDrows + 1.)/2. - (i + 1.)) * APD_SPACING*sqrt(3.)/2.;
	    if ( APDrowsLeft[i] == 2 && APDrowsRight[i] == 0)
          fAPDx[APDnumber] = (-APDrowsLeft[i] + j) * APD_SPACING;
            else {
	      if ( fmod(i,2.) > 0.5 )
          fAPDx[APDnumber] = (-(APDrowsLeft[i])+ 0.5 + j) * APD_SPACING;
              else {
          fAPDx[APDnumber] = (-(APDrowsLeft[i]) + j + 1.) * APD_SPACING;
              }
            } 
          APDnumber++;
	  }
	else if (j == APDrowsLeft[i]) {
          fAPDy[APDnumber] = ((nAPDrows + 1.)/2. - (i + 1.)) * APD_SPACING*sqrt(3.)/2.;
	      if ( fmod(i,2.) > 0.5 )
          fAPDx[APDnumber] = (0.5 + k) * APD_SPACING;
              else {
          fAPDx[APDnumber] = (1. + k) * APD_SPACING;
              }
          APDnumber++;	 
	}
      }
      }
      else 
      if (j != APDrowsLeft[i]) {        
          fAPDy[APDnumber] = ((nAPDrows + 1.)/2. - (i + 1.)) * APD_SPACING*sqrt(3.)/2.;
	    if ( APDrowsLeft[i] == 2 && APDrowsRight[i] == 0)
          fAPDx[APDnumber] = (-APDrowsLeft[i] + j) * APD_SPACING;
            else {
	      if ( fmod(i,2.) > 0.5 )
          fAPDx[APDnumber] = (-(APDrowsLeft[i])+ 0.5 + j) * APD_SPACING;
              else {
          fAPDx[APDnumber] = (-(APDrowsLeft[i]) + j + 1.) * APD_SPACING;
              }
            } 
          APDnumber++;
      }
    }
  }

   G4cout << "############## APD total for one plane is " << APDnumber << G4endl;

  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOAPDParameterisation::~EXOAPDParameterisation()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOAPDParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
 
  G4double theta = (12 - 30)*3.1415926/180.0;
  G4double costheta = cos(theta);
  G4double sintheta = sin(theta);

  // create APD plane 1 (position z)

  if (copyNo < NUMBER_APDS_PER_PLANE) {

    G4double Zposition= -1.0*zAPDcenter;
    G4double Xposition0 = fAPDx[copyNo] + xAPDcenter;
    G4double Yposition0 = fAPDy[copyNo] + yAPDcenter;
    G4double Xposition = costheta*Xposition0 + sintheta*Yposition0; 
    G4double Yposition = -1.0*sintheta*Xposition0 + costheta*Yposition0; 

    G4ThreeVector origin(Xposition,Yposition,Zposition);
    physVol->SetTranslation(origin);
    physVol->SetRotation(0);
  }

  else {

    G4cout << "EXOAPDParameterisation: copyNo should be less than number of APDs per plane" << G4endl;

    G4ThreeVector origin(0.0,0.0,0.0);
    physVol->SetTranslation(origin);
    physVol->SetRotation(0);
   
  }  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOAPDParameterisation::ComputeDimensions
//(G4Tubs& APDcopy, const G4int copyNo, const G4VPhysicalVolume*) const
(G4Tubs& /*APDcopy*/, const G4int /*copyNo*/, const G4VPhysicalVolume*) const
{

  // These statements get rid of the stupid compiler warnings.

//  G4Tubs dummyTube = APDcopy;
//  G4int dummyint = copyNo;
//  dummyint++;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4TwoVector> EXOAPDParameterisation::GetAPDPositions(){
  std::vector<G4TwoVector> apd_pos;
    for(int i=0;i<NUMBER_APDS_PER_PLANE;i++){
      apd_pos.push_back(G4TwoVector(fAPDx[i],fAPDy[i]));
      //G4cout << fAPDx[i] << "," << fAPDy[i] << G4endl;
    }
  return apd_pos;
}

