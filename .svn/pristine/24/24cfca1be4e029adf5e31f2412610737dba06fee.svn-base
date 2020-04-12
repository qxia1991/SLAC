//____________________________________________________
//
// EXOGlitchRecord
//
// Object to store high-voltage glitch occurrences.

#include "EXOUtilities/EXOGlitchRecord.hh"

ClassImp( EXOGlitchRecord )

//____________________________________________________
void EXOGlitchRecord::Clear(Option_t* )
{
  TObject::Clear();

  // Set all information in the glitch record to zero.
  fRunNumber = 0;
  fSerialNumber = 0;
  fSeconds = 0;
  fMicroSeconds = 0;
  fGlitchLevelOne = false;
  fGlitchLevelTwo = false;
  fGlitchLevelThree = false;
  fVersion = 0;
  fTimingError = false;
}

bool EXOGlitchRecord::operator==(const EXOGlitchRecord& rec) const
{
  // Compare two events.
  return ( fRunNumber == rec.fRunNumber and
           fSerialNumber == rec.fSerialNumber and
           fSeconds == rec.fSeconds and
           fMicroSeconds == rec.fMicroSeconds and
           fGlitchLevelOne == rec.fGlitchLevelOne and
           fGlitchLevelTwo == rec.fGlitchLevelTwo and
           fGlitchLevelThree == rec.fGlitchLevelThree and
           fVersion == rec.fVersion and
           fTimingError == rec.fTimingError );

}
