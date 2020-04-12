//______________________________________________________________________________
//
// EXOFitRanges
// Virtual Base class that describes an interface to query ranges for
// magnitudes, times, as well as defining uncertainties.  This allows users
// to customize how ranges for a given signal are calculated.  The default
// behaviour is just to return what's given.
//
// There are 3 classes EXOWireFitRanges, EXOAPDFitRanges, and
// EXOAPDSumFitRanges which are defined, but a user can of course define
// another.  These classes are used in EXOSignalFitterChiSquare.
//______________________________________________________________________________
#include "EXOReconstruction/EXOFitRanges.hh"
#include <algorithm>
using std::min;
double EXOFitRanges::GetMagnitudeUncertainty( double estimatedMag ) const
{
  if ( fFitRange.fMagUnc == 0.0 ) return 0.0;
  double uncertainty = 0.1 * GetMagnitudeMax( estimatedMag );;
  if( estimatedMag > 0.0 and estimatedMag < GetMagnitudeMax( estimatedMag )) {
    uncertainty = min( estimatedMag/2.0, 
                       min( (GetMagnitudeMax(estimatedMag)-estimatedMag)/2.0, uncertainty) );
  }
  return uncertainty;

}
//______________________________________________________________________________
double EXOFitRanges::GetTimeUncertainty( double estimatedTime ) const
{
  if ( fFitRange.fTimeUnc == 0.0 ) return 0.0;
  double uncertainty = 0.1*(GetTimeMax(estimatedTime) - fFitRange.fTimeMin);
  if( estimatedTime > fFitRange.fTimeMin and 
      estimatedTime < GetTimeMax(estimatedTime) ) {
    uncertainty = min( (estimatedTime-fFitRange.fTimeMin)/2.0, 
                       min( (GetTimeMax(estimatedTime)-estimatedTime)/2.0, uncertainty) );
  }
  return uncertainty;
}
//______________________________________________________________________________
EXOFitRanges::EXORanges EXOFitRanges::GetRange(double estimatedMag, double estimatedTime) const
{
  // returns an EXORanges given an estimated magnitude and time

  EXORanges ranges;
  ranges.fMagMax  = GetMagnitudeMax(estimatedMag); 
  ranges.fMagMin  = GetMagnitudeMin(estimatedMag); 
  ranges.fMagUnc  = GetMagnitudeUncertainty(estimatedMag); 
  ranges.fTimeMax = GetTimeMax(estimatedTime); 
  ranges.fTimeMin = GetTimeMin(estimatedTime); 
  ranges.fTimeUnc = GetTimeUncertainty(estimatedTime); 

  return ranges;
}

//______________________________________________________________________________
//
//  Fit ranges for wire signals.
//______________________________________________________________________________
EXOWireFitRanges::EXOWireFitRanges(double t_min, double t_max) : EXOFitRanges() 
{
  fFitRange.fMagMax  = 1.0e6; // maximum amplitude 
  fFitRange.fMagMin  =   0.0; // minimum amplitude
  fFitRange.fMagUnc  =   1.0; // uncertainty != 0 so it will be calculated
  fFitRange.fTimeMax = t_max; // maximum time
  fFitRange.fTimeMin = t_min; // minimum time
  fFitRange.fTimeUnc =   1.0; // time uncertainty != 0 so it will be calculated
}

//______________________________________________________________________________
//
//  Fit ranges for v-wire signals.
//______________________________________________________________________________
EXOVWireFitRanges::EXOVWireFitRanges(double t_min, double t_max) : EXOFitRanges() 
{
  fFitRange.fMagMax  = 1.0e6; // maximum amplitude 
  fFitRange.fMagMin  =   0.0; // minimum amplitude
  fFitRange.fMagUnc  =   1.0; // uncertainty != 0 so it will be calculated
  fFitRange.fTimeMax = t_max; // maximum time
  fFitRange.fTimeMin = t_min; // minimum time
  fFitRange.fTimeUnc =   1.0; // time uncertainty != 0 so it will be calculated
}

//______________________________________________________________________________
//
//  Fit ranges for summed APD signals.
//______________________________________________________________________________
EXOSumAPDFitRanges::EXOSumAPDFitRanges(double t_min, double t_max) : EXOFitRanges() 
{
  fFitRange.fMagMax  = 1.0e6; // maximum amplitude 
  fFitRange.fMagMin  =   0.0; // minimum amplitude
  fFitRange.fMagUnc  =   1.0; // uncertainty != 0 so it will be calculated
  fFitRange.fTimeMax = t_max; // maximum time
  fFitRange.fTimeMin = t_min; // minimum time
  fFitRange.fTimeUnc =   1.0; // time uncertainty != 0 so it will be calculated
}
//______________________________________________________________________________
//
//  Fit ranges for APD gang fits (individual APD fits.
//______________________________________________________________________________
EXOAPDGangFitRanges::EXOAPDGangFitRanges(double t_min, double t_max) : EXOFitRanges() 
{
  
  fFitRange.fMagMax  =  1.0e6; // maximum amplitude 
  fFitRange.fMagMin  = -1.0e6; // minimum amplitude
  fFitRange.fMagUnc  =    1.0; // uncertainty != 0 so it will be calculated
  fFitRange.fTimeMax =  t_max; // maximum time
  fFitRange.fTimeMin =  t_min; // minimum time
  fFitRange.fTimeUnc =    0.0; // time uncertainty == 0 so it will be a fixed parameter
}

//______________________________________________________________________________
double EXOAPDGangFitRanges::GetMagnitudeUncertainty( double estimatedMag ) const
{
  if ( fFitRange.fMagUnc == 0.0 ) return 0.0;
  double uncertainty = 0.2 * GetMagnitudeMax( estimatedMag );;
  if( estimatedMag > GetMagnitudeMin( estimatedMag ) and estimatedMag < GetMagnitudeMax( estimatedMag )) {
    // FixME, is the below a typo?! - MGM
    // should be the min, not the max, but this is taken verbatim from the recon code 
    uncertainty = min( (estimatedMag + GetMagnitudeMax(estimatedMag))/2.0, 
                       min( (GetMagnitudeMax(estimatedMag)-estimatedMag)/2.0, uncertainty) );
  }
  return uncertainty;

}

//______________________________________________________________________________
//
//  Fit ranges for charge injection signals.
//______________________________________________________________________________
EXOChargeInjectionFitRanges::EXOChargeInjectionFitRanges(double t_min, double t_max) : EXOFitRanges() 
{
  fFitRange.fMagMax  =  1.0e6; // maximum amplitude 
  fFitRange.fMagMin  = -1.0e6; // minimum amplitude
  fFitRange.fMagUnc  =   1.0; // uncertainty != 0 so it will be calculated
  fFitRange.fTimeMax = t_max; // maximum time
  fFitRange.fTimeMin = t_min; // minimum time
  fFitRange.fTimeUnc =   1.0; // time uncertainty != 0 so it will be calculated
}
