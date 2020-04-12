//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOFortGen                                                           //
//                                                                      //
// Encapsulation of communication with Fortran particle gnerators.      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "EXOSim/EXOFortGen.hh"
#include "CLHEP/Random/Randomize.h"
#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include <iostream>
#include <map>
#include "G4Alpha.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"


#ifdef EXO_HAS_FORTRAN
#include "include/fgen_wrapper.h"
namespace EXOFortGen
{

  // Set up call throughs to the CLHEP generators.  This is to ensure that
  // everything gets routed to the same random number generator.
  class TWrapRandom : public TRandom {
    public:
     Double_t Rndm(Int_t /*i*/ =0)  { return CLHEP::HepRandom::getTheGenerator()->flat(); } 
  };

  class TFunluxWrap {
    protected:

      // Wrapper class to generate random numbers from a distribution given by a
      // fortran function
      class TWrapFortFunc {
      
        public:
          typedef f77real4(*pt2Func)(f77real4&);
          TWrapFortFunc(void* afunc, double xmin, double xmax) : 
            fLocalFunc( (pt2Func) afunc ),
            fLocalTF1( "", this, xmin, xmax, 0) { }
      
          double operator() (double *x, double *)
            { f77real4 temp = *x; return fLocalFunc(temp); }
          double GetRandom()
            { return fLocalTF1.GetRandom(); }
      
        protected:
          pt2Func fLocalFunc;
          TF1     fLocalTF1;
      
      };

      typedef std::map<int, TWrapFortFunc> FuncMap; 
      static FuncMap fFunctions;
  
    public:
      static void InsertNewFuncWithTag(int tag, void *afunc,
                                       double xmin, double xmax)
      {
        FuncMap::iterator iter = 
          fFunctions.insert(std::make_pair(tag, TWrapFortFunc( afunc, xmin, xmax ))).first;
        iter->second.GetRandom();
      }
  
      static void GetRanNumFromFuncWithTag(int tag, f77real4* xran, int num)
      {
        static TWrapRandom rand;
        FuncMap::iterator iter = fFunctions.find(tag);
        if (iter == fFunctions.end()) {
          std::cout << "Can't find function" << std::endl;
          return;
        }
  
        // Now generate the numbers.  We set gRandom to be a call-through to the CLHEP
        // generator, and then set it back when we're done.
        TRandom* oldRan = gRandom;
        gRandom = &rand;
        for (int i=0;i<num;i++) xran[i] = (f77real4) iter->second.GetRandom();
        gRandom = oldRan;
      }
  
  };
  
}
EXOFortGen::TFunluxWrap::FuncMap EXOFortGen::TFunluxWrap::fFunctions;

// Typedef for fortran complex numbers
typedef struct {
    f77real4 realnum;
    f77real4 imagnum;
} fortcomplex;

extern "C"
{
  void ranlux_(f77real4* array, 
               f77integer4 &num) 
  { 
    // ranlux returns a call to the Geant4 generator 
    for(int i=0;i<num;i++) {
      array[i] = (f77real4) CLHEP::HepRandom::getTheGenerator()->flat();
    }
  }

  void rluxgo_(f77integer4 &/*lux*/, 
               f77integer4 &/*aseed*/, 
               f77integer4 &/*k1*/, 
               f77integer4 &/*k2*/) 
  { 
    // rluxgo needs to not do anything, initialization/configuration occurs
    // with CLHEP/Geant4
  }

  fortcomplex cgamma_(fortcomplex &z) 
  { 
    //  cgamma -- Complex gamma function.
    //      Algorithms and coefficient values from "Computation of Special
    //      Functions", Zhang and Jin, John Wiley and Sons, 1996.
    //
    //  Adapted and *fixed* from C. Bond at: www.crbond.com

    static fortcomplex complret;
    complret.realnum = 1e308;
    complret.imagnum = 0;
    static double a[] = {
        8.333333333333333e-02,
       -2.777777777777778e-03,
        7.936507936507937e-04,
       -5.952380952380952e-04,
        8.417508417508418e-04,
       -1.917526917526918e-03,
        6.410256410256410e-03,
       -2.955065359477124e-02,
        1.796443723688307e-01,
       -1.39243221690590};

    // Following are save variables
    double x1 = 0.0;
    double y1 = 0.0;
    double na = 0.0;
    double x = (double)z.realnum;
    double y = (double)z.imagnum;
    if (x > 171) return complret; 
    if ((y == 0.0) && (x == (int)x) && (x <= 0.0)) {
        return  complret;
    } else if (x < 0.0) {
        x1 = x;
        y1 = y;
        x = -x;
        y = -y;
    }
    double x0 = x;
    if (x <= 7.0) {
        na = (int)(7.0-x);
        x0 = x+na;
    }
    double q1 = sqrt(x0*x0+y*y);
    double th = atan(y/x0);
    double gr = (x0-0.5)*TMath::Log(q1)-th*y-x0+0.5*TMath::Log(2.0*TMath::Pi());
    double gi = th*(x0-0.5)+y*TMath::Log(q1)-y;
    for (int k=0;k<10;k++){
        double t = pow(q1,-1.0-2.0*k);
        gr += (a[k]*t*cos((2.0*k+1.0)*th));
        gi -= (a[k]*t*sin((2.0*k+1.0)*th));
    }
    if (x <= 7.0) {
        double gr1 = 0.0;
        double gi1 = 0.0;
        for (int j=0;j<na;j++) {
            gr1 += (0.5*TMath::Log((x+j)*(x+j)+y*y));
            gi1 += atan(y/(x+j));
        }
        gr -= gr1;
        gi -= gi1;
    }
    if (x1 < 0.0) {
        q1 = sqrt(x*x+y*y);
        double th1 = atan(y/x);
        double sr = -sin(TMath::Pi()*x)*cosh(TMath::Pi()*y);
        double si = -cos(TMath::Pi()*x)*sinh(TMath::Pi()*y);
        double q2 = sqrt(sr*sr+si*si);
        double th2 = atan(si/sr);
        if (sr < 0.0) th2 += TMath::Pi();
        gr = TMath::Log(TMath::Pi()/(q1*q2))-gr;
        gi = -th1-th2-gi;
        x = x1;
        y = y1;
    }
    double g0 = exp(gr);
    gr = g0*cos(gi);
    gi = g0*sin(gi);

    complret.realnum = (f77real4)gr;
    complret.imagnum = (f77real4)gi;
    return complret;
  }

  f77real4 gammf_(f77real4 &x) 
  { 
    return (f77real4)TMath::Gamma((double)x); 
  }

  void rnhpre_(f77real4* y, f77integer4 &nbins)
  {
     // Mimics the behavior of rnprhe in mathlib.  Takes a "histogram"
     // distribution and makes a cumlative distribution out of it.  Subsequent
     // calls to rnhran can be made using this y;  From the CERNLIB manual:
     //
     // RNHPRE changes the values Y to form the cumulative distribution which
     // is needed by RNHRAN. If Y already contains the cumulative distribution
     // rather than the probability density, then RNHPRE should not be called,
     // but in that case Y(NBINS) must be exactly equal to one. Numbers may be
     // drawn from several different distributions in the same run by calling
     // RNHRAN with different arrays Y1, Y2, etc. and (if desired) different
     // values of NBINS, XLO, XWID (but always the same values for a given
     // array Y). The routine RNHPRE should be used to initialize each array
     // Yi.
     for (int i = 1;i<nbins;i++) {
       if (y[i] < 0) { 
         std::cout << "rnprhe: Value of inserted histogram less than 0" << std::endl;
         return;
       }
       y[i] += y[i-1];
     }
     float maxdiv = 1./y[nbins-1];
     for (int i = 0;i<nbins-1;i++) y[i] *= maxdiv;
     y[nbins-1] = 1.0;
  }

  void rnhran_(f77real4* y, f77integer4 &nbins, f77real4 &xlo, f77real4 &xwid, f77real4 &xran)
  {
     // From the CERNLIB manual: 
     //
     // RNHRAN generates random numbers distributed according to any empirical
     // (one-dimensional) distribution. The distribution is supplied in the
     // form of a histogram. If the distribution is known in functional form,
     // FUNLUX (V152) should be used instead.

     xran = 0.0;
     if (y[nbins-1] != 1.0) {
       std::cout << "rnhran: Data not in cumulative form, calling rnhpre" << std::endl;
       rnhpre_(y, nbins);
     }
     if (y[nbins-1] != 1.0) {
       std::cout << "rnhran: Error calling rnhpre" << std::endl;
       return; 
     }
     float flatRan = (float)CLHEP::HepRandom::getTheGenerator()->flat();
     Long64_t pos = TMath::BinarySearch(nbins, y, flatRan);
     if (pos == -1) {
       // means it's below the first value
       xran = xlo + xwid*flatRan/y[0]; 
     } else if (y[pos] != flatRan){
       // means it's in between
       xran = xlo + xwid*( pos + 1 + (flatRan - (y[pos]))/(y[pos + 1] - y[pos])); 
     } else {
       xran = xlo + (pos+1)*xwid;
     }

  }

  void funlxp_(void* afunc, f77real4 *angular_space, f77real4 &xmin, f77real4 &xmax) 
  {
    // funlxp is called first to initialize all the arrays.  This function has
    // internal behavior different that to CERNLIB's FUNLXP, in particular it
    // saves different data in angular space.  It is meant to be used with
    // funlux included in this distribution. 
    // 
    // From the CERNLIB manual:
    // FUNLUX generates random numbers distributed according to any
    // (one-dimensional) distribution f(x). The distribution is supplied by the
    // user in the form of a FUNCTION subprogram. If the distribution is known
    // as a histogram only, HISRAN (V150) should be used instead.
    static int saveTag = 0;
    // Call the wrap function
    EXOFortGen::TFunluxWrap::InsertNewFuncWithTag(saveTag, afunc, xmin, xmax); 
    // Save the tag in angular_space. 
    angular_space[0] = saveTag;
    saveTag++;
  }

  void funlux_(f77real4 *angular_space, f77real4 *xran, f77integer4 &len) 
  {
    // funlux generates a set of random numbers according to a distribution
    // saved through a call to funlxp.
    EXOFortGen::TFunluxWrap::GetRanNumFromFuncWithTag((int)angular_space[0],
      xran, len);
  }
}
#endif /* EXO_HAS_FORTRAN */

namespace EXOFortGen
{

const ParVec& GetParticlesWithGenerator(const std::string& genName, G4double energy)
{
  // Get a set of particles using the generator specified by genName
  GenMap::const_iterator iter;
  if ( (iter = GetFortranGenerators().find(genName)) == GetFortranGenerators().end()) {
    std::cout << "Error finding fortran generator: " << genName << std::endl;
    return GetParticlesWithGenerator(GetFortranGenerators().size() + 1, energy);
  }
  return GetParticlesWithGenerator(iter->second, energy);
}

const ParVec& GetParticlesWithGenerator(int generator, G4double energy)
{
  // Get a set of particles using the generator specified by the generator number
  static ParVec allParticles;
#ifdef EXO_HAS_FORTRAN
  allParticles.clear();
  if ( generator < 1 || 
       generator >= (int)(GetFortranGenerators().size() + 1) ) {
    std::cout << "Out of range for Fortran Generator: " << generator << std::endl;
    return allParticles;
  }

  f77integer4 nr_part, pid[MAXFORTPARTS];
  f77real4 mom_mult[4][MAXFORTPARTS];
  f77real4 e_kin[MAXFORTPARTS],delta_t[MAXFORTPARTS];
  f77integer4 seed=1;
  f77integer4 genid = (f77integer4) generator;
  fgen_wrapper_(genid, 
                seed, 
                nr_part, 
                pid, 
                mom_mult, 
                e_kin, 
                delta_t);

  for (int i=0;i<nr_part;i++) {
    Particle newPart;
    // We need to get into CLHEP units (MeV), these are delivered in GeV
    newPart.fEnergy = e_kin[i]*CLHEP::GeV/CLHEP::MeV;
    newPart.fDeltaT = delta_t[i];
    newPart.fMomentum.set(mom_mult[1][i],
                          mom_mult[2][i],
                          mom_mult[3][i] );
    switch(pid[i]) {
      case 1: newPart.fParticle  = G4Gamma::Definition(); break;
      case 2: newPart.fParticle  = G4Positron::Definition(); break;
      case 3: newPart.fParticle  = G4Electron::Definition(); break;
      case 5: newPart.fParticle  = G4MuonPlus::Definition(); break;
      case 6: newPart.fParticle  = G4MuonMinus::Definition(); break;
      case 47: newPart.fParticle = G4Alpha::Definition(); break;
      default:
        std::cout << "Received unknown particle id: " << (double) pid[i] << std::endl;
        continue;
    }
    allParticles.push_back(newPart);
  }

#endif /* EXO_HAS_FORTRAN */
  return allParticles;
}

const GenMap& GetFortranGenerators()
{
  // Returns the list of available Fortran Generators.
  static GenMap allGenerators;
#ifdef EXO_HAS_FORTRAN
  static bool first_call = true;
  if (first_call) {
    f77integer4 num_gens;
    f77genname name_of_gen; 
    f77integer4 lenname_of_gen; 
    fgen_wrap_max_gens_(num_gens);
    for (f77integer4 i=1;i<num_gens+1;i++) {
       fgen_wrap_name_gen_(i, name_of_gen, lenname_of_gen); 
       std::string strName(name_of_gen, lenname_of_gen);
       allGenerators[strName] = i;
    }
    first_call = false;
  }
#endif /* EXO_HAS_FORTRAN */
  return allGenerators;
}


}
