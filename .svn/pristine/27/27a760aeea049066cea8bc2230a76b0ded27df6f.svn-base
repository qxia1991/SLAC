#ifndef EXOCOMPIM_UTIL_H
#define EXOCOMPIM_UTIL_H

#include "Rtypes.h"
#include "TMath.h"

class TH1;

namespace ComptonIm
{
  enum CylPart {
                   PlusZEndcap, 
                   MinusZEndcap,
                   LateralArea
               };

  const Double_t pi = TMath::Pi();
  const Double_t twoPi = TMath::TwoPi();
  const Double_t piBy2 = TMath::PiOver2();

  Double_t Normalize(Double_t * vec);
  void ReverseHistXaxis(TH1 * hist);
  Double_t SinusoidalX(Double_t x, Double_t y, Double_t z);
  Double_t SinusoidalY(Double_t x, Double_t y, Double_t z);
  Double_t Xcoord(Double_t x, Double_t y, Double_t z);
  Double_t Ycoord(Double_t x, Double_t y, Double_t z);
  Double_t Zcoord(Double_t x, Double_t y, Double_t z);

  void ReportError(const char * funcName, int code);
}

#endif
