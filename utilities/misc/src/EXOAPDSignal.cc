//______________________________________________________________________________
//
// EXOAPDSignal
//
// Encapsulates signal from an APD
//
#include "EXOUtilities/EXOAPDSignal.hh"
#include "EXOUtilities/EXOScintillationCluster.hh"
#include <iostream>

ClassImp( EXOAPDSignal )

EXOAPDSignal& EXOAPDSignal::operator= (const EXOAPDSignal &other)
{
  if (this == &other) return *this;
  fType = other.fType;
  fChannel = other.fChannel;
  fDescr = other.fDescr;
  fRawCounts = other.fRawCounts;
  fCounts = other.fCounts;
  fCorrectedCounts = other.fCorrectedCounts;
  fCountsError = other.fCountsError;
  fChiSquared = other.fChiSquared;
  return *this;
}
bool EXOAPDSignal::operator== (const EXOAPDSignal &other) const
{
  return (fType == other.fType &&
          fChannel == other.fChannel &&
          fDescr == other.fDescr &&
          fRawCounts == other.fRawCounts && 
          fCounts == other.fCounts && 
          fCorrectedCounts == other.fCorrectedCounts && 
          fCountsError == other.fCountsError && 
          fChiSquared == other.fChiSquared);
}

void EXOAPDSignal::Clear(Option_t * /*opt = ""*/)
{
  fType = kBad;
  fChannel = 0;
  fDescr = "";
  fRawCounts = 0.;
  fCounts = 0.;
  fCorrectedCounts = 0.;
  fCountsError = 0.;
  fChiSquared = 0.;
}
void EXOAPDSignal::Print(Option_t * /*opt = ""*/) const
{
  std::cout 
    << std::string(60,'*') << "\n"
    << "EXOAPDSignal dump: \n" 
    << "Identification type = " << fType << "("<< GetTypeName() << ")\n"
    << "Identification number (like channel number) = " << fChannel << "\n"
    << "Signal description (all details) = '" << fDescr << "'\n"
    << "Raw number of photons collected = " << fRawCounts << "\n"
    << "Number of photons corrected for channel gain = " << fCounts << "\n"
    << "Number of photons corrected for everything = " << fCorrectedCounts << "\n"
    << "Error on corrected number of photons = " << fCountsError << "\n"
    << "Chi^2 of fit (or something similar) = " << fChiSquared << "\n"
    << std::string(60,'*') << std::endl;
}

static const char* SignalTypeNames[] = {
    "kGeneric",
    "kGenericFit",
    "kGenericSum",
    "kGangFit",
    "kPlaneFit",
    "kFullFit",
    "kGangSum",
    "kPlaneSum",
    "kFullSum",
    "kCentroid"
};

const char* EXOAPDSignal::GetTypeName() const { 
  return (kGeneric<=fType && fType<=kCentroid)?SignalTypeNames[fType]:"kBad"; }

double EXOAPDSignal::GetEnergy() const { return 0; }

double EXOAPDSignal::GetError() const { return 0; }

const EXOScintillationCluster* EXOAPDSignal::GetScintCluster() const
{ 
  // Get associated scintillation cluster
  return static_cast< const EXOScintillationCluster* >( fRefScintCluster.GetObject() ); 
}

EXOScintillationCluster* EXOAPDSignal::GetScintCluster() 
{ 
  // Get associated scintillation cluster
  return static_cast< EXOScintillationCluster* >( fRefScintCluster.GetObject() ); 
}

void EXOAPDSignal::SetScintCluster(EXOScintillationCluster *sc)
{
  // Set associated scintillation cluster
  fRefScintCluster = sc;
}

