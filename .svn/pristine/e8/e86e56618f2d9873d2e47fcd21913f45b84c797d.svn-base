//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/////////////////////////////////////////////////////////////////////////////
#ifndef EXOElectronConversion_h
#define EXOElectronConversion_h 1
#include "G4VDiscreteProcess.hh"

class G4Step;

class EXOElectronConversion : public G4VDiscreteProcess {
public:

  EXOElectronConversion(const G4String&);
  virtual ~EXOElectronConversion();
  
  G4bool IsApplicable(const G4ParticleDefinition&  );
  // is applicable for optical photon only

  G4double GetMeanFreePath(const G4Track& ,
				 G4double ,
				 G4ForceCondition* condition);
  //returns infinity (max integer possible) . This means the process does
  // not limit the step, but sets the Forced condition for the DoIt to be
  // invoked at every step. But only at the boundary between Hpd quartz 
  // window and the Hpd photocathode any action be taken.

  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
				       const G4Step&  aStep);

  //  G4double getHpdQEff(G4int, G4double);
  // To interpolate the QE from QE data.
  //G4double getHpdPhElectronKE() 
  //{return HpdPhElectronKE; }
  //G4double getPhCathodeToSilDetDist()
  //{return PhCathodeToSilDetDist; }
  //G4double getHpdDemag(G4int hpdnum) {return DemagnificationFactor[hpdnum]; }
 private:
  //  RichTbAnalysisManager* rAnalysisPhy;
  //G4String PrePhotoElectricVolName;
  //G4String PostPhotoElectricVolName;
  //G4double HpdPhElectronKE;
  //G4double PhCathodeToSilDetDist;
  //G4double PSFsigma;
  G4double electron_kinetic;
 
  G4double photon_direction_phi;
  G4double photon_direction_theta;
  G4double p_mean;
  G4double p_mean_photons;

  G4double rand_direction_phi;
  G4double rand_direction_theta;
  G4double polar_direction_phi;
  G4double polar_direction_theta;  
  G4double pol_direction_x;
  G4double pol_direction_y;
  G4double pol_direction_z;
  G4double phot_direction_x;
  G4double phot_direction_y;
  G4double phot_direction_z;
  G4double scale_phot;
  G4double scale; 
  G4double recomb_random; 

};

#endif
