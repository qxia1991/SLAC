#include "EXOComptonImager/EXOCompIm_util.hh"

#include "TH1.h"
#include "TGaxis.h"
#include "TMath.h"
#include "TPad.h"

#include <iostream>

namespace ComptonIm
{

  Double_t Normalize(Double_t * vec)
  {
    const Double_t norm = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
    for (int i = 0; i < 3; i++) vec[i] /= norm;
    return norm;
  }

  void ReverseHistXaxis(TH1 * hist)
  {
    hist->GetXaxis()->SetLabelOffset(999.);
    hist->SetTickLength(0);
    gPad->Update();
    TGaxis * newaxis = new TGaxis(gPad->GetUxmax(),
                                  gPad->GetUymin(),
                                  gPad->GetUxmin(),
                                  gPad->GetUymin(),
                                  hist->GetXaxis()->GetXmin(),
                                  hist->GetXaxis()->GetXmax(),
                                  510,"-");
    newaxis->SetLabelOffset(-0.03);
    newaxis->SetTitle("x [mm]");
    newaxis->Draw();

    return;
  }

  Double_t SinusoidalX(Double_t x, Double_t y, Double_t z)
  {
    Double_t phi, theta;
    phi = TMath::ATan2(y, x);
    theta = TMath::ACos(z/sqrt(x*x + y*y + z*z));
    return (phi*cos(-1.*theta + piBy2));
  }

  Double_t SinusoidalY(Double_t x, Double_t y, Double_t z)
  {
    Double_t theta;
    theta = TMath::ACos(z/sqrt(x*x + y*y + z*z));
    return (-1.*theta + piBy2);
  }

  Double_t Xcoord(Double_t x, Double_t y, Double_t z)
  { return x; }

  Double_t Ycoord(Double_t x, Double_t y, Double_t z)
  { return y; }

  Double_t Zcoord(Double_t x, Double_t y, Double_t z)
  { return z; }

  void ReportError(const char * funcName, int code)
  {
    std::cout << "<" << funcName << "> : ";
    switch (code)
    {
      case 1:
        std::cout << "Tree not set.";
        break;

      case 2:
        std::cout << "The size of the space must exceed the bin width";
        break;

      case 3:
        std::cout << "Unrecognized cylinder part code.";
        break;

      default:
        std::cout << "Oh, indescribable abomination!";
        break;
    }
    std::endl(std::cout);
    return;
  }
}
