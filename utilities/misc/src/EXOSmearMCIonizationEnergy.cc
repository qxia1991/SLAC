//______________________________________________________________________________
//                                                                        
// EXOSmearMCIonizationEnergy
//
// Smear the ionization energy of the pixelated charge deposits to account for the LXe energy resolution.
// By LXe energy resolution, we mean the variation in how much charge is produced by a given energy deposit,
// where the excess energy goes into heat and scintillation.
//
// Comment (10/20/2011): This is a very simple implementation.  Factors which ought to have a small effect,
// but are ignored here, include the strength of the electric field and
// the density of ionization (higher density -> greater chance of recombination).
//
// Comment (10/20/2011): It seems like it would be more reasonable to have a class, EXOFillMCIonizationEnergy,
// that reads in EXOMCPixelatedChargeDeposit::fTotalEnergy and fills fTotalIonizationEnergy. This would
// need to account for alpha-quenching and LXe energy resolution.  But currently EXOEventAction doesn't write out whether
// a deposit was from alpha or beta/gamma particles, so this would be difficult to make compatible with old monte carlo data.


#include "EXOUtilities/EXOSmearMCIonizationEnergy.hh"
#include "EXOUtilities/EXOMonteCarloData.hh"
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "TRandom3.h"
#include <cmath>

EXOSmearMCIonizationEnergy::EXOSmearMCIonizationEnergy()
: fLXeEnergyResolution(0.0)
{}

void EXOSmearMCIonizationEnergy::SetLXeEnergyResolution(double Resolution)
{
  // Set the energy resolution.
  // At the Q-value, this is our fractional resolution (0.01 -> gaussian smear with sigma = 24.58 keV at the Q value).
  // We scale the RMS of the smear like sqrt(energy).
  // Negative values, like 0, mean no smearing is applied.
  fLXeEnergyResolution = Resolution;
}

double EXOSmearMCIonizationEnergy::GetLXeEnergyResolution() const
{
  // Return the energy resolution.
  // See notes for SetLXeEnergyResolution for a definition.
  return fLXeEnergyResolution;
}

void EXOSmearMCIonizationEnergy::ApplySmear(EXOMonteCarloData& MonteCarloData)
{
  // Apply smearing to modify EXOPixelatedChargeDeposit::fTotalIonizationEnergy.
  if(fLXeEnergyResolution > 0.0) {
    for(size_t i = 0; i < MonteCarloData.GetNumPixelatedChargeDeposits(); i++) {
      EXOMCPixelatedChargeDeposit* PixelDeposit = MonteCarloData.GetPixelatedChargeDeposit(i);
      double SmearSigma = fLXeEnergyResolution * std::sqrt(DOUBLE_BETA_DECAY_Q_VALUE * PixelDeposit->fTotalIonizationEnergy);
      PixelDeposit->fTotalIonizationEnergy += gRandom->Gaus(0.0, SmearSigma); // gRandom is a global defined in TRandom3.
    }
  }
}
