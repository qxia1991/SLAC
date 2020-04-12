#ifndef _EXOSavitzkyGolaySmoother_hh_
#define _EXOSavitzkyGolaySmoother_hh_ 1

#include "EXOSmoother.hh"
#include <vector>

class EXOSavitzkyGolaySmoother : public EXOSmoother
{
  public:
    EXOSavitzkyGolaySmoother( size_t smoothSize = 2, 
                               size_t derivativeOrder = 0, 
                               size_t polynomialDegree = 2);
    virtual ~EXOSavitzkyGolaySmoother() {}

    virtual void ResetSmootherAttributes( size_t smoothSize, 
      size_t derivativeOrder, size_t polynomialDegree );
    virtual void SetSmoothSize( size_t /*smoothSize*/);

  protected:
    virtual void SmoothData( const std::vector<double>& input, 
                             std::vector<double>& output, 
                             const EXOWaveformRegion region ) const;

    std::vector< std::vector< double > > fVectorOfCoeffs; 
};



namespace SavitzkyGolay {

  template<class T>
  class SGVector : public std::vector<T> {
    public:
      SGVector( size_t n, T someValue ) :  std::vector< T >(n, someValue ) {} 
      SGVector( size_t n ) : std::vector< T > (n) {} 
      SGVector( ) : std::vector< T > () {} 
  };
  
  template<class T> class SGMatrix : public std::vector< std::vector<T> >
  {
    public:
      SGMatrix( size_t n, T someValue ) : 
        std::vector< std::vector< T > >(n, std::vector< T > (n, someValue) ) {} 
      SGMatrix( size_t n ) : 
        std::vector< std::vector< T > >(n, std::vector< T > (n) ) {} 
  };



  template<class T> void LUBackSubstitution( SGMatrix<T>& a, 
                                             const SGVector< size_t >& indx, 
                                             SGVector<T>& b);
  template<class T> void LUDecomposition( SGMatrix<T>& a,
                                          SGVector< size_t >& indx, 
                                          T& d );
  template<class T> T abs(T aVal) { return ( aVal < 0 ) ? -aVal : aVal; }

  bool GenerateSavitzkyGolayCoeffs(std::vector<double>& coeffs, int nl, int nr, size_t ld, size_t m);
    /* Returns Savitzy-Golay coefficients in the coeffs vector.  They are in 
       the order: 0, -1, ... -(nl-1), -(nl),nr, nr-1, ..., 1 

       The function returns false if it failed to generated the correct coefficients. */

};


#endif
