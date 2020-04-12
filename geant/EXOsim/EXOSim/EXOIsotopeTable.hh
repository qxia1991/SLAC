#ifndef EXOIsotopeTable_h
#define EXOIsotopeTable_h 1

#include "globals.hh"
#include "G4IsotopeProperty.hh"
#include "G4VIsotopeTable.hh"

////////////////////////////////////////////////////////////////////////////////
//
class EXOIsotopeTable : public G4VIsotopeTable
{
public:
  // constructor
  //
  EXOIsotopeTable ();

  // destructor
  virtual ~EXOIsotopeTable();

public:
  virtual G4bool FindIsotope(G4IsotopeProperty* property);
  // The FindIsotope function will replace the pure virtual one defined in the
  // abstract base class G4VIstopeTable.  We don't use this fuction in this
  // implementation, instead we use the next function.
  //
  virtual G4IsotopeProperty* GetIsotope(G4int Z, G4int A, G4double E);
  //
  //   again it will replace the pure virtual one in the abstract base class.
  //
  //   Z: Atomic Number
  //   A: Atomic Mass
  //   E: Excitaion energy

  void SetLifetimeLimit(G4double v) { limit = v; }
  G4double GetLifetimeLimit() const { return limit; }

private:

  G4VIsotopeTable* FindDelegate();
  void ApplyEXOLifetimeFix(G4IsotopeProperty* properties);

  G4VIsotopeTable* delegate;
  G4double         limit;    // upper limit on halflife
};

#endif
