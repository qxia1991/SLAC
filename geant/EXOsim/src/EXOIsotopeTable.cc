#include "G4SystemOfUnits.hh"
#include "G4IsotopeProperty.hh"
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "EXOSim/EXOIsotopeTable.hh"

#include "globals.hh"

// for Nuclear level lifetime fix
#include "G4NuclearLevelStore.hh"
#include "G4NuclearLevelManager.hh"
#include "G4NuclearLevel.hh"
//#include "G4RIsotopeTable.hh"
static const G4double levelTolerance = 2.0 * keV; // taken from G4RIsotopeTable

EXOIsotopeTable::EXOIsotopeTable()
{

  G4cout << "*********************************************************************" << G4endl;
  G4cout << "Using the EXO modified version of EXOIsotopeTable" << G4endl;
  G4cout << "Isotope lifetimes greater than limit will be set to a limit value" << G4endl;
  G4cout << "*********************************************************************" << G4endl;
  delegate = NULL;
  limit = 60*second;
}

///////////////////////////////////////////////////////////////////////////////
//
EXOIsotopeTable::~EXOIsotopeTable()
{
}

G4bool EXOIsotopeTable::FindIsotope(G4IsotopeProperty* )
{
  // do nothing, it is here just for the compiler
  // it is required by the base class
  return true;
}

G4IsotopeProperty* EXOIsotopeTable::GetIsotope(G4int Z, G4int A, G4double E)
{
  G4IsotopeProperty* result = 0;
  G4VIsotopeTable *table = FindDelegate();
  if (table!=0) {
    result = table->GetIsotope(Z,A,E);
    // If isotope doesn't exist table usually create it

    // this is to fix bug in GEANT4 when nuclear excitation levels are created stable
    G4NuclearLevelManager * levelManager = G4NuclearLevelStore::GetInstance()->GetManager (Z, A);
    if (result && levelManager->NumberOfLevels() ) { // FIXME
      const G4NuclearLevel* level = levelManager->NearestLevel (E);
      if (std::abs(E - level->Energy()) < levelTolerance) {
        if (level->Energy()>0. && result->GetLifeTime()==-1.) { // we need to fix
          if (level->HalfLife()!=0.) result->SetLifeTime(level->HalfLife());
          else result->SetLifeTime(1.e-11*second); // some code treat 0. as Inf, substitute EM times
        }
      }
    }

    if (result != 0) ApplyEXOLifetimeFix(result);
  }
  return result;
}

G4VIsotopeTable* EXOIsotopeTable::FindDelegate()
{
  if (delegate==0) {
    // Let's find a real isotope table which should be somewhere, registered.
    // Please notice, that it may be unavailable if G4RadioactiveDecay isn't created.
    G4IonTable *ionTable = G4ParticleTable::GetParticleTable()->GetIonTable();
    for (int i=0;i<10;i++) {
      // there should be only 2 entries, but let's try 10
      G4VIsotopeTable* t = ionTable->GetIsotopeTable(i);
      if (t==this) continue; // skip ourself
      if (t==0) break;       // table finished
      delegate = t; break;   // let's hope this is what we want
    }
  }
  return delegate;
}

// SystemOfUnits extention
//namespace CLHEP {
  static const double minute = 60*second;
  static const double hour   = 3600*second;
  static const double day    = 24*hour;
  static const double year   = 365.25*day;
//}
std::string PrettyTime(G4double t)
{
  const double units_val[] = { 
    year, day, hour, minute, second, millisecond, microsecond, nanosecond
  };
  const char* units_name[] = {
    "year", "day", "hour", "minute", "second", "millisecond", "microsecond", "nanosecond"
  };
  std::stringstream str;
  for (unsigned i=0;i<sizeof(units_val)/sizeof(*units_val);i++)
    if ( t >= units_val[i]) {
      double tt = t/units_val[i];
      if (tt<0.001 || tt>1000.) str << std::scientific;
      str << tt <<" "<< units_name[i] <<"s";
      return str.str();
    }
  str << std::scientific << t/second << " seconds";
  return str.str();
}

void EXOIsotopeTable::ApplyEXOLifetimeFix(G4IsotopeProperty* properties)
{
  // Reduce the lifetime from large numbers to small numbers (~1 minute).  This
  // is to avoid precision issues from the limited bit depth of doubles.

  G4double halflife = properties->GetLifeTime()*M_LN2;

  if ( halflife > 0.0 ) {
    G4cout << "EXOIsotopeTable: Z, A = " << properties->GetAtomicNumber() << " " << properties->GetAtomicMass();
    if (halflife > limit) {
      properties->SetLifeTime(limit/M_LN2);
      G4cout << ":\tchanging halflife from " << PrettyTime(halflife) << " to " << PrettyTime(limit) << G4endl;
    } else 
      G4cout << ":\tusing true halflife of " << PrettyTime(halflife) << G4endl;
  }

}
