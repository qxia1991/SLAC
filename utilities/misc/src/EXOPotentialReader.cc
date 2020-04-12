//______________________________________________________________________________
/* EXOPotentialReader

This class provides a class to read in potentials that EXODigitizer uses. 

*/
//______________________________________________________________________________
#include "EXOUtilities/EXOPotentialReader.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <fstream>

EXOPotentialReader::EXOPotentialReader(
  double xmin, double xmax,
  double zmin, double zmax,
  double dx, double dz,
  size_t nx, size_t nz)
: fXMin(xmin),
  fXMax(xmax),
  fZMin(zmin),
  fZMax(zmax),
  fDX(dx),
  fDZ(dz),
  fNX(nx), 
  fNZ(nz) 
{
  // Protected constructor for derived classes.
}

//______________________________________________________________________________
void EXOPotentialReader::LoadDataFromFile(
  const std::string& Filename, DblVec& PotentialVector) const
{
  // This code gets run twice, one time for each vector in the class.  Program
  // will terminate via the error logger if this function fails in any way.

  if(sizeof(double) != 8) {
    LogEXOMsg("\"double\" is the wrong size on this platform.  "
              "Your computer cannot read the binary potential files.", EEAlert);
  }

  // First check for a full path
  std::string FullFilename = EXOMiscUtil::SearchForFile(Filename);
  if(FullFilename == "") {
    // then check for a relative path in the data directory
    FullFilename = EXOMiscUtil::SearchForFile("data/maxwell/" + Filename);
    if(FullFilename == "") {
      LogEXOMsg("Failed to find " + Filename, EEAlert);
    }
  }

  std::ifstream Reader(FullFilename.c_str(), std::ios_base::binary | std::ios_base::in);
  if(Reader.fail()) {
    LogEXOMsg("Failed to open " + Filename, EEAlert);
  }

  // Skip the header values (grid dimensions & spacing),
  // since numerical problems can arise if we get our array size from this.
  // These are hard-coded instead.
  Reader.ignore(sizeof(double)*6);
  if(Reader.bad()) {
    LogEXOMsg("An error was encountered reading " + Filename, EEAlert);
  }
  if(Reader.eof()) {
    LogEXOMsg("EOF was prematurely reached in " + Filename, EEAlert);
  }

  // Read in the next rows, corresponding to X = -0.50 mm, -0.48 ... 3.48 mm,
  // for all Z coordinates (Z = 0 ... 25.0 mm).
  PotentialVector.resize(fNX*fNZ);
  Reader.read((char*)&PotentialVector[0], sizeof(double)*fNX*fNZ);
  if(Reader.bad()) {
    LogEXOMsg("An error was encountered reading " + Filename, EEAlert);
  }
  if(Reader.eof()) {
    LogEXOMsg("EOF was prematurely reached in " + Filename, EEAlert);
  }

  // Confirm that this is the end of the file.  Note that there is a
  // long-standing bug in gcc related to istream::ignore and EOF, so I can't
  // use that.
  std::streampos CurrentPosition = Reader.tellg();
  if(CurrentPosition == std::streampos(-1)) {
    LogEXOMsg("Failed to return stream position in " + Filename, EEAlert);
  }
  Reader.seekg(0, std::ios_base::end);
  if(Reader.fail()) {
    LogEXOMsg("Failed to find EOF in " + Filename + " -- file corrupted?", EEAlert);
  }
  std::streampos EOFPosition = Reader.tellg();
  if(EOFPosition == std::streampos(-1)) {
    LogEXOMsg("Failed to return stream position in " + Filename, EEAlert);
  }
  if(EOFPosition != CurrentPosition) {
    LogEXOMsg(Filename + " has EOF after expected location", EEAlert);
  }
}


