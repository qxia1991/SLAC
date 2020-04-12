#ifndef EXOFitRanges_hh
#define EXOFitRanges_hh

class EXOFitRanges 
{
  public:
    struct EXORanges {
      double fMagMax;
      double fMagMin;
      double fMagUnc;
      double fTimeMax;
      double fTimeMin;
      double fTimeUnc;
    };

    EXOFitRanges(const EXORanges& range) : fFitRange(range) {}
    EXOFitRanges() {}
    virtual ~EXOFitRanges() {}

    virtual EXORanges GetRange(double estimatedMag, double estimatedTime) const;

    virtual double GetMagnitudeMax( double /*estimatedMag*/ ) const 
      { return fFitRange.fMagMax; } 
    virtual double GetMagnitudeMin( double /*estimatedMag*/ ) const 
      { return fFitRange.fMagMin; } 
    virtual double GetMagnitudeUncertainty( double estimatedMag ) const; 

    virtual double GetTimeMax( double /*estimatedTime*/ ) const 
      { return fFitRange.fTimeMax; } 
    virtual double GetTimeMin( double /*estimatedTime*/ ) const 
      { return fFitRange.fTimeMin; } 
    virtual double GetTimeUncertainty( double estimatedTime ) const; 

  protected:
    struct EXORanges fFitRange;
};

//______________________________________________________________________________
//
//  Fit ranges for Wire signals.
//______________________________________________________________________________
class EXOWireFitRanges : public EXOFitRanges
{
  public:
    EXOWireFitRanges(double t_min, double t_max);
    //double GetTimeMin(double /*estTime */) const { return 0.0; }
};

//______________________________________________________________________________
//
//  Fit ranges for V-Wire signals.
//______________________________________________________________________________
class EXOVWireFitRanges : public EXOFitRanges
{
  public:
    EXOVWireFitRanges(double t_min, double t_max);
    //double GetTimeMin(double /*estTime */) const { return 0.0; }
};

//______________________________________________________________________________
//
//  Fit ranges for summed APD signals.
//______________________________________________________________________________
class EXOSumAPDFitRanges : public EXOFitRanges
{
  public:
    EXOSumAPDFitRanges(double t_min, double t_max);
    //double GetTimeMin(double /*estTime */) const { return 0.0; }
};

//______________________________________________________________________________
//
//  Fit ranges for Gang signals.
//______________________________________________________________________________
class EXOAPDGangFitRanges : public EXOFitRanges
{
  public:
    EXOAPDGangFitRanges(double t_min, double t_max);
    virtual double GetMagnitudeUncertainty( double estimatedMag ) const; 
    //double GetTimeMin( double /*estTime*/ ) const { return 0.0; }
};

//______________________________________________________________________________
//
//  Fit ranges for Charge Injection signals.
//______________________________________________________________________________
class EXOChargeInjectionFitRanges : public EXOFitRanges
{
  public:
    EXOChargeInjectionFitRanges(double t_min, double t_max);
    //double GetTimeMin( double /*estTime*/ ) const { return 0.0; }
};

#endif /* EXOFitRanges_hh */
