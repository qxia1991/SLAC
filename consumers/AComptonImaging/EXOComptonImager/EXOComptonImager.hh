#ifndef EXOCOMPTONIMAGER_H
#define EXOCOMPTONIMAGER_H

//______________________________________________________________________________
//
// EXOComptonImager
// Kurt Barry <kmbarry1@stanford.edu>
//
// This class performs visualization of reconstructed multiple Compton scatters 
// observed in the EXO-200 TPC.
//
// Last modification: 08/29/2010 by K. Barry
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TCanvas;
class TCut;
class TFile;
class TFormula;
class TGraph;
class TH2D;
class TH3D;
class TString;
class TTree;
class TTreeFormula;
class EXOComptonInfo;

#include "Rtypes.h"
#include "EXOComptonImager/EXOCompIm_util.hh"
#include <string>

class EXOComptonImager
{
  public:
    EXOComptonImager();
    EXOComptonImager(TTree * Events, std::string TreeCut);
    virtual ~EXOComptonImager();

    //Determine a 3D map of where the gammas are most likely to be originating, 
    //and copy this histogram to the user-provided one.
    int GetFilled3DHist(TH3D & hist, Double_t size, 
                        bool regen = true);

    //Get a copy of parts of the TPC/cylinder (internally, these are the same, 
    //but for convenience functions for both are provided).
    int GetTPCPart(TH2D & hist, ComptonIm::CylPart part, 
                   bool regen = true);
    int GetCylPart(TH2D & hist, ComptonIm::CylPart part, 
                   Double_t length, Double_t radius, 
                   bool regen = true);
    int GetCathode(TH2D & hist, bool regen = true);

    //Fills the user-provided histogram by sampling at the provided points and 
    //using the provided functions to determine in which bin of the user 
    //histogram to place the sample.
    int ArbitrarySurface(Long64_t nPoints, 
                         Double_t * x, Double_t * y, Double_t * z, 
                         Double_t (* xProj) 
                           (Double_t x, Double_t y, Double_t z),
                         Double_t (* yProj) 
                           (Double_t x, Double_t y, Double_t z),
                         TH2D & hist, bool regen = true);

    //These are display functions--they will create one or more canvases
    //drawing the histograms that they fill.
    int  DisplaySphere(Double_t radius, bool regen = true);
    void DisplayTPC(bool regen = true);
    void DisplayCylinder(Double_t length, Double_t radius, 
                         bool regen = true);
    void DisplayCathode(bool regen = true);

    //Set verious parameters
    void SetBinWidth(Double_t binWidth);
    void SetNumToAccept(unsigned int num);
    void SetExcludeTPC(bool exclude);
    void SetSpatialCut(const char * cut);

    //Get various parameters
    Double_t     GetBinWidth()    const;
    unsigned int GetNumToAccept() const;
    bool         GetExcludeTPC()  const;
    TString      GetSpatialCut()  const;
    TString      GetTreeCut()     const;

    //Return the distance that a cone must have to a point to qualify as 
    //"passing through" the given point.
    Double_t Close() const;

  private:
    void Init();
    int Fill3DHistMC(Double_t sideLength);
    int DoCylinder(Double_t length, Double_t radius);
    int DoCathode(Double_t radius);
    int CalcNumCones(Double_t length);
    void MakeCirclesForEndcaps(Double_t radius);

    EXOComptonImager(const EXOComptonImager &);
    EXOComptonImager & operator=(const EXOComptonImager &);

  private:
    class ComptonCone
    {
      public:
        ComptonCone(const EXOComptonImager & imager);
        ~ComptonCone();
        void SetCone(Double_t * cone);
        void Evaluate(Double_t len, Double_t theta, 
                      Double_t & x, Double_t & y, Double_t & z);
        bool CheckProximity(const Double_t * point) const;
        Double_t GetParam(int idx);

      private:
        const EXOComptonImager & fImager;
        Double_t fCone[7];   //Cone parameters
        Double_t fCC[5];     //Cone constants
        Double_t fU[3];      //Cone direction
        Double_t fSinPhi;
        Double_t fCosPhi;
        bool fTrivFlag;
        bool fIsSet;

        static const Double_t fTolerance;
    };

  private:
    TTree * fEvents;

    TH3D * fSpace;

    TH2D * fCylEnd1;    //plus-z endcap
    TH2D * fCylEnd2;    //minus-z endcap
    TH2D * fCylSide;
    TH2D * fComboCyl;
    TH2D * fCathode;
    TH2D * fSphereProj;

    TGraph * fCirc1;
    TGraph * fCirc2;
    TGraph * fCirc3;

    const TCut * fTPCcut;
    TCut * fUserSpatialCut;
    TFormula * fSpatialCutForm;

    TTreeFormula * fTreeCutForm;

    ComptonCone fCompCone;

    EXOComptonInfo* fComptonInfo;
    
    Double_t fBinWidth;

    unsigned int fNumToAccept;
    unsigned int fPtsPerBin;

    bool fExcludeTPC;

    bool fDoPlusZEndcap;
    bool fDoMinusZEndcap;
    bool fDoLateralArea;

    static const Double_t fPROXFACTOR;
    static const Double_t fCONENORM;
    static const Double_t fTPCRADIUS;
    static const Double_t fTPCLENGTH;
    static const Double_t fMINBINWIDTH;
    static const int fMAXCONES;
};

#endif
