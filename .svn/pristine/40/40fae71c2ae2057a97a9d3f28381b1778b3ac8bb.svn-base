//______________________________________________________________________________
//
// EXOSavitzkyGolaySmoother
//
// Savtizty-Golay waveform smoother.  Written by M. Marino, ported from MGDO by
// M.  Marino on 11 Apr 2012.
//
//______________________________________________________________________________

#include "EXOUtilities/EXOSavitzkyGolaySmoother.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include <cmath>
#ifndef TINY
#define TINY 1.0e-20
#endif

using namespace std;

EXOSavitzkyGolaySmoother::EXOSavitzkyGolaySmoother( size_t smoothSize, 
                                                    size_t derivativeOrder, 
                                                    size_t polynomialDegree) : 

EXOSmoother( smoothSize, "EXOSavitzkyGolaySmoother" )
{
  ResetSmootherAttributes( smoothSize, derivativeOrder, polynomialDegree );
}

void EXOSavitzkyGolaySmoother::SmoothData( const std::vector<double>& input, 
                                            std::vector<double>& output, 
                                            const EXOWaveformRegion region ) const
{
  if ( input.size() < 2*fSmoothSize + 1 ) {
    std::string err = "The size of the input waveform (inputsize) is too small to be used with the smoothing size.\n";
    err += "  inputsize must be >= 2*(smoothing size) + 1";
    LogEXOMsg(err, EEError);
    return;
  }
  // input is here greater than size 0
  size_t place = region.beginning;
  size_t end = (region.end >= input.size()) ? input.size() - 1 : region.end;
  while (  place <= end ) {
    size_t amountToTheLeft  =  (place >= 2*fSmoothSize) ? 2*fSmoothSize : place;
    size_t amountToTheRight =  (end >= 2*fSmoothSize + place) ? 
                               2*fSmoothSize : end - place;
 
    if ( amountToTheRight < fSmoothSize ) {
      amountToTheLeft = 2*fSmoothSize - amountToTheRight;
    } else if ( amountToTheLeft < fSmoothSize ) {
      amountToTheRight = 2*fSmoothSize - amountToTheLeft;
    } else {
      amountToTheLeft = amountToTheRight = fSmoothSize;
    }
    if ( amountToTheLeft + amountToTheRight != 2*fSmoothSize ) {
      LogEXOMsg("Coding error", EEAlert);
      return;
    }

    output[place] = input[place]*fVectorOfCoeffs[ amountToTheRight ][0]; 
    for( size_t i=1;i<=amountToTheLeft;i++ ){
      output[place] += input[place - i]*fVectorOfCoeffs[ amountToTheRight ][i];
    }
    for( size_t i=1;i<=amountToTheRight;i++ ){
      output[place] += input[place + i]*
                       fVectorOfCoeffs[ amountToTheRight ][ fVectorOfCoeffs[ amountToTheRight ].size() - i];
    }
    place++;
  }
}

void EXOSavitzkyGolaySmoother::ResetSmootherAttributes( size_t smoothSize, 
                                                         size_t derivativeOrder, 
                                                         size_t polynomialDegree )
{
  if ( 2*smoothSize + 1 < polynomialDegree ) {
    LogEXOMsg("2*smoothSize + 1 < degree of polynomial!  Construction of coefficients aborted.", EEError);
    return;
  }
  if ( derivativeOrder > polynomialDegree ) {
    LogEXOMsg("Derivative order > degree of polynomial!  Construction of coefficients aborted.", EEError);
    return;
  }

  fVectorOfCoeffs.resize( 2*smoothSize + 1 );
  for ( size_t i = 0;i < fVectorOfCoeffs.size(); i++ ) {
    if ( !SavitzkyGolay::GenerateSavitzkyGolayCoeffs( fVectorOfCoeffs[i], 
                                                      (int) (2*smoothSize - i), (int) i, 
                                                      derivativeOrder, polynomialDegree ) ) {
      LogEXOMsg("Error constructing coefficients.", EEError);
    }
  }
  EXOSmoother::SetSmoothSize( smoothSize );

}

void EXOSavitzkyGolaySmoother::SetSmoothSize( size_t /*smoothSize*/)
{
  LogEXOMsg("SetSmoothSize() not used for this class, use ResetSmootherAttributes() instead. ", EEError);
}

bool SavitzkyGolay::GenerateSavitzkyGolayCoeffs(std::vector<double>& coeffs, int nl, int nr, size_t ld, size_t m) 
{

  int np = nl+nr+1;
  coeffs.clear();
  coeffs.resize(np, 0.);
  
  if (nl < 0 || nr < 0 || ld > m || nl+nr < (int)m) {
    /* error */
    return false;
  }

  SGVector< size_t > indexVector(m+1, 0); 
  SGMatrix< double > aMatrix(m+1, 0.);
  SGVector< double > bVector(m+1, 0.);

  for (size_t j=0;j<=m;j++) {
    for (size_t i=j;i<=m;i++) {
      /* Constructing the a matrix which is basically A^T . A */
      double sum = 0.;//= (j+i==0) ? 0.0 : -1.0;
      for (int k=-nl;k<=nr;k++) sum += pow((double)k,(double)(j+i));
      aMatrix[i][j] = sum;
      aMatrix[j][i] = sum;
    }
  }

  double d;
  LUDecomposition< double >(aMatrix,indexVector,d);
 
  bVector[ld]=1.0;
  LUBackSubstitution< double >(aMatrix, indexVector, bVector);
  
  for (int k = -nl;k<=nr;k++) {
    double sum=bVector[0];
    double fac=1.0;
    for (size_t mm=1;mm<=m;mm++) sum += bVector[mm]*(fac *= k);
    int kk=((np-k) % np);
    coeffs[kk]=sum;
  }

  return true;
}



template <class T> void SavitzkyGolay::LUDecomposition(SGMatrix<T>& a, SGVector< size_t >& indx, T& d)
{
  size_t imax;
  T big,dum,sum,temp;

  size_t n = a.size();
  imax = 0; // avoid compilation warning (ML)

  SGVector<T> vv(n);
  d=1.0;
  for (size_t i=0;i<n;i++) {
    big=0.0;
    for (size_t j=0;j<n;j++) {
      if ((temp=abs<T>(a[i][j])) > big) big=temp;
    }
    vv[i]=1.0/big;
  }
  for (size_t j=0;j<n;j++) {
    for (size_t i=0;i<j;i++) {
      sum=a[i][j];
      for (size_t k=0;k<i;k++) sum -= a[i][k]*a[k][j];
      a[i][j]=sum;
    }
    big=0.0;
    for (size_t i=j;i<n;i++) {
      sum=a[i][j];
      for (size_t k=0;k<j;k++)
	sum -= a[i][k]*a[k][j];
      a[i][j]=sum;
      if ( (dum=vv[i]*abs<T>(sum)) >= big) {
	big=dum;
	imax=i;
      }
    }
    if (j != imax) {
      for (size_t k=0;k<n;k++) {
	dum=a[imax][k];
	a[imax][k]=a[j][k];
	a[j][k]=dum;
      }
      d *= -1;
      vv[imax]=vv[j];
    }
    indx[j]=imax;
    if (a[j][j] == 0.0) a[j][j]=TINY;
    if (j != n-1) {
      dum=1.0/(a[j][j]);
      for (size_t i=j+1;i<n;i++) a[i][j] *= dum;
    }
  }
}

template<class T> void SavitzkyGolay::LUBackSubstitution( SGMatrix<T>& a, 
                                                   const SGVector<size_t>& indx, 
                                                   SGVector<T>& b) 
{
  T sum;
  
  size_t n = a.size();

  size_t ii = (size_t)-1;
  for (int i=0;i<(int)n;i++) {
    size_t ip=indx[i];
    sum=b[ip];
    b[ip]=b[i];
    if (ii != (size_t)-1) {
      for (size_t j=ii;j<(size_t)i;j++) sum -= a[i][j]*b[j];
    } else if ( sum != 0) {
      ii=i;
    }
    b[i]=sum;
  }
  for (int i=(int)(n-1);i>=0;i--) {
    sum=b[i];
    for (size_t j=i+1;j<n;j++) sum -= a[i][j]*b[j];
    b[i]=sum/a[i][i];
  }
}

