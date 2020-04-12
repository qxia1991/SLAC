#ifndef EXOFortGen_hh
#define EXOFortGen_hh
#include "G4ThreeVector.hh"
#include <vector>
#include <map>
class G4ParticleDefinition;

namespace EXOFortGen 
{

  // Particle class for return function
  class Particle {
    public:
      G4ParticleDefinition* fParticle; 
      G4ThreeVector         fMomentum;
      G4double              fEnergy; 
      G4double              fDeltaT; 
  };

  // Typedef for vector of delivered particles
  typedef std::vector<Particle> ParVec;

  // Typedef for map of available Fortran Generators
  typedef std::map< std::string, int > GenMap;

  // Get a map (string, generator id) of the available fortran generators.
  const GenMap& GetFortranGenerators();

  // The following grabs a vector of primary particles given a generator.
  // Some generators take a request for energy (e.g. kgMonoLine, kCosmics)
  // which can be passed through this interface.  Other generators ignore
  // this. 
  //
  // Takes the generator id, see GetFortranGenerators().
  const ParVec& GetParticlesWithGenerator(int generator, G4double energy = 0.0); 

  // The following grabs a vector of primary particles given a generator.
  // Some generators take a request for energy (e.g. kgMonoLine, kCosmics)
  // which can be passed through this interface.  Other generators ignore
  // this. 
  //
  // Takes the name of the generator, see GetFortranGenerators(). 
  const ParVec& GetParticlesWithGenerator(const std::string& genname, G4double energy = 0.0); 

}

#endif
