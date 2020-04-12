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
#include "EXOSim/EXOElectronConversion.hh"
#include "EXOSim/EXORunAction.hh"
#include "G4PhysicalConstants.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "G4Poisson.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh" 
#include "G4DynamicParticle.hh"
//#include "G4PhotoElectricEffect.hh"
#include "TTree.h"
#include "G4SystemOfUnits.hh" // confuses TString

//Constructor
EXOElectronConversion::EXOElectronConversion(const G4String& processName):G4VDiscreteProcess(processName){;}

//Deconstructor
EXOElectronConversion::~EXOElectronConversion(){;}

G4bool EXOElectronConversion::IsApplicable(const G4ParticleDefinition& 
					               aParticleType)
{
  //return ( &aParticleType == G4Electron::Electron() );
 

    return true;
}

G4double EXOElectronConversion::GetMeanFreePath(const G4Track& ,
                                              G4double ,
                                              G4ForceCondition* condition)
{
  //The PostStepDoIt Below needs to be run after each step, regardless of which process limited the step (hence, the strongly forced condition
	*condition = StronglyForced;

	return DBL_MAX;
}


//Physics to do POST STEP
G4VParticleChange* EXOElectronConversion::PostStepDoIt(const G4Track& aTrack,
                                                           const G4Step& aStep)
{
 

  //Creating the pointer to EXORunAction
  EXORunAction* runAction = (EXORunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
  aParticleChange.Initialize(aTrack);


  //Only run this code if the Alpha Team's Anticorrelation Algorithm is being used
  if(runAction->returnAlphaAnticorr()){


  //Allow the creation of 100000 Secondaries (photons)
  aParticleChange.SetNumberOfSecondaries(100000);

  //Get Pointers to pre and post step points
  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
  
  //Get x,y,z positions of the step end point
      G4double x_point = pPostStepPoint->GetPosition()[0] /mm;
      G4double y_point = pPostStepPoint->GetPosition()[1] /mm;
      G4double z_point = pPostStepPoint->GetPosition()[2] /mm;
      char x_composite[80];
      char x_composite_2[80];
            
      // How many electrons produced from this energy deposit?
	  // 15.6 eV per ionization electron from Aprile's LXe Detector Review Paper arXiv:0910.4956v1
    
      p_mean = (aStep.GetTotalEnergyDeposit() / eV)/15.6;
      
      //N_ex/N_el set in Macro - usually = 0.13
      p_mean_photons = p_mean*runAction->returnNexAlpha();     
      
      //Get number of electrons produced with this energy deposit using a Poisson Distribution
	  G4long psn = G4Poisson(p_mean);
 
      //Get number of exitons produced again using a Poisson Distribution
          G4long psn_photons = G4Poisson(p_mean_photons);
	 
      //Have the photon be an optical photon
      electron_kinetic = 7.07 * eV;

      Double_t energy = 0;

      //Find Energy deposited locally

      //Only do this step for the second of the two replicated events
  	if(runAction->returnIncrement()%2!=0){       

	  //Get Energy Distribution Tree that was produced in the first of the replicated events

	  TTree* info = runAction->ReturnEnergyTree();

	  //Get info of the local energy depositions
	  sprintf(x_composite,"sqrt((x-%f)^2+(y-%f)^2+(z-%f)^2)<0.01",x_point,y_point,z_point);
	  sprintf(x_composite_2,"energy:sqrt((x-%f)^2+(y-%f)^2+(z-%f)^2)",x_point,y_point,z_point);
	  Int_t temp = info->Draw(x_composite_2,x_composite,"goff");

	  //Add this local energy
      	  for(G4int i=0;i<temp;i++){ 
	    energy+=(info->GetV1()[i])*(1-(info->GetV2()[i]/0.01));
	  }
  	}

	//Creating Photons From Excited Xe Atoms
	for(G4long i=0;i<psn_photons;i++){

	  //Get Random Numbers needed below (need to get random numbers for both replicated events, otherwise the will not be identical events)

	  //photon direction
	  photon_direction_theta = 2.*pi*G4UniformRand();
	  photon_direction_phi = 2.*asin(sqrt(G4UniformRand()));
	  
	  //polarization direction
	  rand_direction_theta = 2.*pi*G4UniformRand();
	  rand_direction_phi = 2.*asin(sqrt(G4UniformRand()));

	  if(runAction->returnIncrement()%2!=0){   
      //Find Photon Direction

	    phot_direction_x = sin(photon_direction_phi)*cos(photon_direction_theta);
	    phot_direction_y = sin(photon_direction_phi)*sin(photon_direction_theta);
	    phot_direction_z = cos(photon_direction_phi);
	    scale_phot = sqrt(pow(phot_direction_x,2)+pow(phot_direction_y,2)+pow(phot_direction_z,2));
	      G4ThreeVector photon_direction = G4ThreeVector(phot_direction_x/scale_phot,phot_direction_y/scale_phot,phot_direction_z/scale_phot);


	  //Find Random Vector to use to set Polarization
	      G4ThreeVector rand_direction = G4ThreeVector(sin(rand_direction_phi)*cos(rand_direction_theta),sin(rand_direction_phi)*sin(rand_direction_theta),cos(rand_direction_phi));	 
	  
	  //Take Cross product of the photon direction and the rand direction to get polarization direction
	  
	      pol_direction_x = photon_direction[1]*rand_direction[2]-photon_direction[2]*rand_direction[1];
	      pol_direction_y = photon_direction[2]*rand_direction[0]-photon_direction[0]*rand_direction[2];
	      pol_direction_z = photon_direction[0]*rand_direction[1]-photon_direction[1]*rand_direction[0];
	      scale = sqrt(pow(pol_direction_x,2)+pow(pol_direction_y,2)+pow(pol_direction_z,2));

      	  //Define Photon
	      if(!runAction->returnFastScintAlpha()){
	      G4DynamicParticle* aPhoton = new G4DynamicParticle (G4OpticalPhoton::OpticalPhoton(),photon_direction,electron_kinetic);
	      aPhoton->SetPolarization(pol_direction_x/scale,pol_direction_y/scale,pol_direction_z/scale);
	      //Photon Added HERE
	      aParticleChange.AddSecondary(aPhoton);
	      //delete aPhoton;
	      }

	  } 
	  //end of Creating Excited Xe Atoms
	}


	//Creating Photons from Ionization (or collecting Electrons)

        G4int electronCount = 0;
        G4int photonCount = 0;

	for(G4long i=0;i<psn;i++){

	  //Again, get random numbers
	  photon_direction_theta = 2.*pi*G4UniformRand();
	  photon_direction_phi = 2.*asin(sqrt(G4UniformRand()));
	  
	  rand_direction_theta = 2.*pi*G4UniformRand();
	  rand_direction_phi = 2.*asin(sqrt(G4UniformRand()));

	  recomb_random = G4UniformRand();
  	 	
     //Here is where the decision is made to either have the electron be a drift electron or to create a photon

	  if(runAction->returnIncrement()%2!=0){   

	    //Get recombination parameter that was found using the Stanford TPC data

	    G4double recombination_parameter = 77.43/runAction->returnAlphaEField()+10.86;
	    recombination_parameter = recombination_parameter/1000000.;

	    if(recomb_random > 1./(energy*recombination_parameter)*log(1+recombination_parameter*energy)){

	      photonCount++;


	  //Have the photon's direction be random
	  //This is from http://en.wikibooks.org/wiki/Mathematica/Uniform_Spherical_Distribution
	  //Find Photon Direction

	    phot_direction_x = sin(photon_direction_phi)*cos(photon_direction_theta);
	    phot_direction_y = sin(photon_direction_phi)*sin(photon_direction_theta);
	    phot_direction_z = cos(photon_direction_phi);
	    scale_phot = sqrt(pow(phot_direction_x,2)+pow(phot_direction_y,2)+pow(phot_direction_z,2));
	      G4ThreeVector photon_direction = G4ThreeVector(phot_direction_x/scale_phot,phot_direction_y/scale_phot,phot_direction_z/scale_phot);	

	  //Find Random Vector to use to set Polarization
	      G4ThreeVector rand_direction = G4ThreeVector(sin(rand_direction_phi)*cos(rand_direction_theta),sin(rand_direction_phi)*sin(rand_direction_theta),cos(rand_direction_phi));	 
	  
	  //Take Cross product of the photon direction and the rand direction to get polarization direction
	  
	      pol_direction_x = photon_direction[1]*rand_direction[2]-photon_direction[2]*rand_direction[1];
	      pol_direction_y = photon_direction[2]*rand_direction[0]-photon_direction[0]*rand_direction[2];
	      pol_direction_z = photon_direction[0]*rand_direction[1]-photon_direction[1]*rand_direction[0];
	      scale = sqrt(pow(pol_direction_x,2)+pow(pol_direction_y,2)+pow(pol_direction_z,2));

      	  //Define Photon
	      if(!runAction->returnFastScintAlpha()){
	      G4DynamicParticle* aPhoton = new G4DynamicParticle (G4OpticalPhoton::OpticalPhoton(),photon_direction,electron_kinetic);
	      aPhoton->SetPolarization(pol_direction_x/scale,pol_direction_y/scale,pol_direction_z/scale);	      
	      //Photon Added HERE
	      aParticleChange.AddSecondary(aPhoton);
	      //delete aPhoton;
} 
	    }else{
      	//Only Record Electron if it's whithin the collection region
              electronCount++;
	    }
	    //end of Iteration Loop
	  }
       //End of For Loop
	}



	//Find the Fraction of energy that went into ionization - to be used in EXOLXeSD
        G4double frac;
	if(psn!=0){ 
        	frac = (double)electronCount/psn;
	}else{
	        frac = 1.0;
	}
        runAction->setFrac(frac);
  }
 return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}
