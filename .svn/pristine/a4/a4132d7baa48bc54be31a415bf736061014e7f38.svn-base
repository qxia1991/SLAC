//______________________________________________________________________________
// Created 11-24-10
// Phil Barbeau & Kevin O'sullivan
// Last Modified 12-01-10
// Phil Barbeau
//******************************
// This module reconstructs a compton cone for high quality 2 and 3 site gamma interactions
// and outputs the appropriate cone information to the root file.  The intention of this 
// module is to be used to identify hot spots in the construction materials of the detector,
// as well as calibration sources. To run properly this code requires that the data has gone
// through both reconstruction and all the flags for fiducial volume, 3D clusters, and ghost
// events have been applied. The flags are applied in the physics module. For example, to
// run this module on raw data one would at least have to call "rec physics acompton."
// ====================================================================================
// Changelog:
// 11-29-10: incorporated Kevin's compton reconstruction class into this module
// 11-30-10: fixed a number of bugs.
// 12-01-10: finally created this change log
// 12-15-10: updated documentation
// ======================================================================================

#include "EXOAnalysisManager/EXOATeamComptonModule.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventData.hh"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "TTree.h"
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOATeamComptonModule, "acompton" )

//______________________________________________________________________________
EXOATeamComptonModule::EXOATeamComptonModule()
{

}

//______________________________________________________________________________
EXOATeamComptonModule::~EXOATeamComptonModule()
{
}

//______________________________________________________________________________
int EXOATeamComptonModule::Initialize()
{
  num_events = 0;
  num_2_sites = 0;
  num_3_sites = 0;
  pass_2_sites = 0;
  pass_3_sites = 0;

  //  cout << "Initializing " << name << endl;

  // Make sure we've got an error logger and an analysis manager

  return 0;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOATeamComptonModule::BeginOfRun(EXOEventData *ED)
{
    cout << "At BeginOfRun for " << GetName() << ", run number = " 
         << ED->fRunNumber << endl;

  return kOk;
}

//______________________________________________________________________________
EXOAnalysisModule::EventStatus EXOATeamComptonModule::ProcessEvent(EXOEventData *ED)
{
  fCompton.Clear();
  num_events++;
  size_t myncl = ED->GetNumChargeClusters();
  if (myncl == 2) {
    num_2_sites++;
  }
  
  if (myncl == 3) {
    num_3_sites++;
  }

  bool xrec = false;
  bool yrec = false;
  bool zrec = false;
  bool threeD;
  bool fiducial;

  if (myncl < 2 || myncl > 3) goto returnOK; 

  threeD = ED->GetChargeCluster(0)->Is3DCluster();
  fiducial = ED->GetChargeCluster(0)->IsFiducial();

  for (size_t i=1;i<myncl;i++) {
    threeD = threeD && ED->GetChargeCluster(i)->Is3DCluster();
    fiducial = fiducial && ED->GetChargeCluster(i)->IsFiducial();
  }
  

  if ((ED->GetChargeCluster(0)->fX >-999) && (ED->GetChargeCluster(1)->fX > -999)) {
    if (myncl == 2 ){
      xrec = true;
    }
    if ((myncl == 3) && (ED->GetChargeCluster(2)->fX > -999)) {
      xrec = true;
    }
  }

  if ((ED->GetChargeCluster(0)->fY >-999) && (ED->GetChargeCluster(1)->fY > -999)) {
    if (myncl == 2 ){
      yrec = true;
    }
    if ((myncl == 3) && (ED->GetChargeCluster(2)->fX > -999)) {
      yrec = true;
    }
  }

  if ((ED->GetChargeCluster(0)->fZ >-999) && (ED->GetChargeCluster(1)->fZ > -999)) {
    if (myncl == 2 ){
      zrec = true;
    }
    if ((myncl == 3) && (ED->GetChargeCluster(2)->fZ > -999)) {
      zrec = true;
    }
  }

  for (int j = 0; j < 3; j++) {
    ecl[j] = 0.0;
    xcl[j] = 0.0;
    ycl[j] = 0.0;
    zcl[j] = 0.0;
    fidcl[j] = false;
    tdcl[j] = false;
    ghcl[j] = false;
  }    
  
  E_tot = 0.0;
  FOM = 0.0;
  ncl = 0;


  if ( fiducial && threeD && xrec && yrec && zrec) {
    for (unsigned int j = 0; j < myncl; j++){
      EXOChargeCluster* clust = ED->GetChargeCluster(j);
      if (ED->GetTotalPurityCorrectedEnergy() > 0) { 
        // check to make sure that the purity correction has been done
        add_cluster(clust->fPurityCorrectedEnergy, clust->fX, clust->fY, 
                    clust->fZ, clust->IsFiducial(), clust->Is3DCluster(), false);
      }
      else {
        // if not, then use the weighting field corrected energies
        add_cluster(clust->fCorrectedEnergy, clust->fX, clust->fY, 
                    clust->fZ, clust->IsFiducial(), clust->Is3DCluster(), false);
      }
    } 
   
    Order();  
    

    fCompton.fNumClustersUsed = get_ncl();
    fCompton.fEnergy = get_E();
    fCompton.fX1 = get_xcl(0);
    fCompton.fY1 = get_ycl(0);
    fCompton.fZ1 = get_zcl(0);
    fCompton.fX2 = get_xcl(1);
    fCompton.fY2 = get_ycl(1);
    fCompton.fZ2 = get_zcl(1);
    fCompton.fX1Err = 3.82;
    fCompton.fY1Err = 3.82;
    fCompton.fZ1Err = 0.85;
    fCompton.fX2Err = 3.82;
    fCompton.fY2Err = 3.82;
    fCompton.fZ2Err = 0.85;
    fCompton.fFOM = FOM;
    fCompton.fHalfAnglePhi = get_phi_com( 0 );
    fCompton.fHalfAnglePhiErr = delta_phi();

    if (fCompton.fHalfAnglePhi > -999 && fCompton.fNumClustersUsed == 2) {
      pass_2_sites++;
    }
    if (fCompton.fHalfAnglePhi > -999 && fCompton.fNumClustersUsed == 3) {
      pass_3_sites++;
    }

  }

returnOK:
  FillTree(ED);
  return kOk;
}

//______________________________________________________________________________
void EXOATeamComptonModule::SetupBranches(TTree* tree)
{
  tree->Branch("fCompton", &fCompton);
}

//______________________________________________________________________________
int EXOATeamComptonModule::ShutDown()
{
  //  cout << "At ShutDown for " << GetName() << endl;

  //report on results of the trigger module
  cout << endl;
  cout << "+====================================+"<< endl;
  cout << "            Compton Events           +"<< endl;
  cout << "+====================================+"<< endl;
  cout << " Total events: " << num_events << endl;
  cout << "2 site events: " << num_2_sites << endl;
  cout << "3 site events: " << num_3_sites << endl;
  cout << "passing 2 site events: " << pass_2_sites << endl;
  cout << "passing 3 site events: " << pass_3_sites << endl;
  cout << "+====================================+"<< endl;
  cout << endl;
  return 0;
}

//______________________________________________________________________________
int EXOATeamComptonModule::get_ncl(){
  return ncl;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_E(){
  return E_tot;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_ecl(int i){
  if (i<ncl) {return ecl[i];}
  else {
    cout<<"no such site."<<endl;
    return -1;
  }
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_xcl(int i){
  if (i<ncl) {return xcl[i];}
  else {
    cout<<"No such site."<<endl;
  return -1;
  }
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_ycl(int i){
  if (i<ncl) {return ycl[i];}
  else {
    cout<<"No such site."<<endl;
  return -1;
  }
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_zcl(int i){
  if (i<ncl) {return zcl[i];}
  else {
    cout<<"No such site."<<endl;
  return -1;
  }
}

//______________________________________________________________________________
bool EXOATeamComptonModule::get_fidcl(int i){
  if (i<ncl) {return fidcl[i];}
  else {
    cout<<"No such site."<<endl;
  return false;
  }
}

//______________________________________________________________________________
bool EXOATeamComptonModule::get_tdcl(int i){
  if (i<ncl) {return tdcl[i];}
  else {
    cout<<"No such site."<<endl;
  return false;
  }
}

//______________________________________________________________________________
bool EXOATeamComptonModule::get_ghcl(int i){
  if (i<ncl) {return ghcl[i];}
  else {
    cout<<"No such site."<<endl;
  return false;
  }
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_ncl(int i){
  ncl=i;
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_ecl(int i, double E){
  if (i<ncl) {ecl[i] = E;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_xcl(int i, double X){
  if (i<ncl) {xcl[i] = X;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_ycl(int i, double Y){
  if (i<ncl) {ycl[i] = Y;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_zcl(int i, double Z){
  if (i<ncl) {zcl[i] = Z;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_fidcl(int i, bool FID){
  if (i<ncl) {fidcl[i] = FID;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_tdcl(int i, bool TD){
  if (i<ncl) {tdcl[i] = TD;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::set_ghcl(int i, bool GH){
  if (i<ncl) {ghcl[i] = GH;}
  else {cout<<"No such site."<<endl;}
}

//______________________________________________________________________________
void EXOATeamComptonModule::add_cluster(double E, double X, double Y, double Z, bool FID, bool TD, bool GH){
  if (ncl>20) {ncl=0;}
  if (ncl==0) {E_tot=0;}

  ncl++;
  set_ecl(ncl-1, E);
  set_xcl(ncl-1, X);
  set_ycl(ncl-1, Y);
  set_zcl(ncl-1, Z);
  set_fidcl(ncl-1, FID);
  set_tdcl(ncl-1, TD);
  set_ghcl(ncl-1, GH);

  if (ncl>3) {cout<<"Warning: This code is not designed to handle events with more than three sites."<<endl;}

  E_tot=E_tot + E;
  if (ncl==3) {
    U.SetXYZ(xcl[0] - xcl[1], ycl[0] - ycl[1], zcl[0] - zcl[1]);
    V.SetXYZ(xcl[1] - xcl[2], ycl[1] - ycl[2], zcl[1] - zcl[2]);
  }
}

/*!
  Generates the next permutation of the vector v of length n.
  @return 1, if there are no more permutations to be generated
  @returne 0, otherwise
*/

//______________________________________________________________________________
int EXOATeamComptonModule::next(int v[]) {
  /* P2*/
  /* Find the largest i */
  int n = 3;
  int i = n - 2;
  while ((i >= 0) && (v[i] > v[i+1]))
    i--;

  /* If i is smaller than 0, then there are no more permutations. */
  if (i < 0)
    return 1;

  /* Find the largest element after vi but not larger than vi */
  int k = n - 1;
  while (v[i] > v[k])
    --k;
  swap(v[i], v[k]);

  /*Swap the last n - i elements. */
  int j;
  k = 0;
  for (j = i + 1; j < (n + i)/2 + 1; ++j, ++k)
    swap(v[j], v[n - k - 1]);
  return 0;
}  

//______________________________________________________________________________
double EXOATeamComptonModule::delta_E(double E) { //calculates uncertainty at a given energy
  double dE;
  dE = 0.03*sqrt(1333)*sqrt(E);
  return dE;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_phi_geo() {
  double phi_geo = 0;
  if (ncl!=3) {cout<<"This function is only for three site events."<<endl;}
  else {
    phi_geo = acos(U*V/(U.Mag()*V.Mag()));
  }
  return phi_geo;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_cos_phi_geo() {
  double cos_phi_geo = 0;
  if (ncl!=3) {cout<<"This function is only for three site events."<<endl;}
  else {
    cos_phi_geo = U*V/(U.Mag()*V.Mag());
  }
  return cos_phi_geo;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_phi_com(int i) {
  double E_gin = E_tot;
  double phi_com = 0.0;
  mec = 511.0;
  
  if (i>=ncl - 1) {cout<<"Invalid site for this function."<<endl;}
  else {
    for (int j = 0; j < i; j ++) {
      E_gin = E_gin - ecl[j];
    }
    
    if ((1.0 - mec*(1.0/(E_gin - ecl[i])-1.0/E_gin) >= -1.0) && (1.0 - mec*(1.0/(E_gin - ecl[i]) -1.0/E_gin) <=1)) {
      phi_com = acos(1.0-mec*(1.0/(E_gin - ecl[i]) - 1.0/E_gin));
    }
    else {
      phi_com = -999;
      //cout<<"get_phi_com:Site is not consistent with a Compton scatter."<<endl;
    }
  }
return phi_com;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_cos_phi_com(int i) {
  double E_gin = E_tot;
  double cos_phi_com = 0.0;
  mec = 511.0;
  
  if (i>=ncl - 1) {cout<<"Invalid site for this function."<<endl;}
  else {
    for (int j = 0; j < i; j ++) {
      E_gin = E_gin - ecl[j];
    }
    
    if ((1 - mec*(1/(E_gin - ecl[i])-1/E_gin) >= -1) && (1 - mec*(1/(E_gin - ecl[i]) -1/E_gin) <=1)) {
      cos_phi_com = 1-mec*(1/(E_gin - ecl[i]) - 1/E_gin);
    }
    else {
      cos_phi_com = -999;
      //cout<<"get_cos_phi_com:Site is not consistent with a Compton scatter."<<endl;
    }
  }
return cos_phi_com;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_sigma_geo() {
  double sigma_sqd = 0.0;
  double pos_sig[3] = {3.82, 3.82, 0.85};
  double cphi;
  double a;
  cphi = get_cos_phi_geo();
  
  for (int i = 0; i < 3; i ++) {
    a = (V[i]/(U.Mag()*V.Mag())-(U[i]*cphi)/(U.Mag()*V.Mag()))*(V[i]/(U.Mag()*V.Mag())-(U[i]*cphi)/(U.Mag()*V.Mag()))*pos_sig[i]*pos_sig[i];
    sigma_sqd = sigma_sqd + a;
    a = (U[i]/(U.Mag()*V.Mag())-(V[i]*cphi)/(V.Mag2()))*(U[i]/(U.Mag())-(V[i]*cphi)/(V.Mag2()))*pos_sig[i]*pos_sig[i];
    sigma_sqd = sigma_sqd + a;
  }

  return sigma_sqd;
}

//______________________________________________________________________________
double EXOATeamComptonModule::get_sigma_com() {
  double sigma_sqd;
  double W[2];
  double sig_Wi_sqd;
  double sig_WiWii_sqd;
  double mec = 511.0;
  
  W[0] = (E_tot - ecl[0]) / mec;
  W[1] = (E_tot - ecl[0] - ecl[1]) / mec;

  sig_Wi_sqd = delta_E( E_tot - ecl[0]) / mec;
  sig_WiWii_sqd = sig_Wi_sqd + delta_E( E_tot - ecl[0] - ecl[1] ) / mec;
  
  sigma_sqd = 1/( W[0]*W[0]*W[0]*W[0])*sig_Wi_sqd + (1/(W[0]*W[0]) - 1/(W[1]*W[1]))*(1/(W[0]*W[0]) - 1/(W[1]*W[1]))*sig_WiWii_sqd;
  return sigma_sqd;

}

//______________________________________________________________________________
double EXOATeamComptonModule::delta_phi_com() {
  double mec = 511.0;
  double E1;
  double dE1;
  double E2;
  double dE2;
  double sphi;
  double dphi;
  double phi_com;
  
  E1 = ecl[0];
  dE1 = delta_E( E1 );
  E2 = E_tot - ecl[0];
  dE2 = delta_E( E2 );
  phi_com = get_phi_com( 0 );
  sphi = sin( phi_com );
  dphi = mec/sphi*sqrt((dE1/(E_tot*E_tot))*(dE1/(E_tot*E_tot))+((E1*(E1+2*E2)*dE2)/(E_tot*E_tot*E2*E2))*((E1*(E1+2*E2)*dE2)/(E_tot*E_tot*E2*E2)));
  return dphi;
}

//______________________________________________________________________________
double EXOATeamComptonModule::delta_phi_geo() {
  double dphi = 0.0;
  double dphi_U;
  double dphi_V;
  double pos_sig[3] = {3.82, 3.82, 0.85};
  
  for (int j = 0; j < 3; j++) {
    dphi_U = (sqrt(2)/U.Mag())*pos_sig[j]*sqrt(1-(U[j]/U.Mag())*(U[j]/U.Mag()));
    dphi_V = (sqrt(2)/V.Mag())*pos_sig[j]*sqrt(1-(V[j]/V.Mag())*(V[j]/V.Mag()));
    dphi = dphi+dphi_U*dphi_U +dphi_V*dphi_V;
  }
  
  dphi = sqrt(dphi);
  return dphi;
}

//______________________________________________________________________________
double EXOATeamComptonModule::delta_phi() {
  double dphi = 0.0;
  dphi = sqrt( delta_phi_com()*delta_phi_com() + delta_phi_geo()*delta_phi_geo());
  
  return dphi;
}

//______________________________________________________________________________
double EXOATeamComptonModule::seq_FOM(int v[]) {
  double phi_com = 0.0;
  double phi_geo = 0.0;
  double sigma_geo = 0.0;
  double sigma_com = 0.0;
  double figure = 0.0;
  
  if (ncl != 3) {
    cout<<"This function is only for three site events."<<endl;
    FOM = -1;
  }
  
  else{
    Sequence( v );
    phi_com = get_cos_phi_com( 1 );
    phi_geo = get_cos_phi_geo();
    sigma_com = get_sigma_com();
    sigma_geo = get_sigma_geo();
    figure = (phi_com - phi_geo)*(phi_com - phi_geo)/(sigma_geo + sigma_com);
    UnSequence( v );
  }

  return figure;
}

//______________________________________________________________________________
void EXOATeamComptonModule::Sequence( int v[]) {
  double eph[3];
  double xph[3];
  double yph[3];
  double zph[3];
  bool fidph[3];
  bool tdph[3];
  bool ghph[3];

  for (int j = 0; j < ncl; j++) {
    eph[j] = ecl[j];
    xph[j] = xcl[j];
    yph[j] = ycl[j];
    zph[j] = zcl[j];
    fidph[j] = fidcl[j];
    tdph[j] = tdcl[j];
    ghph[j] = ghcl[j];
  }

  for (int j = 0; j < ncl; j++) {
    ecl[j] = eph[v[j]];
    xcl[j] = xph[v[j]];
    ycl[j] = yph[v[j]];
    zcl[j] = zph[v[j]];
    fidcl[j] = fidph[v[j]];
    tdcl[j] = tdph[v[j]];
    ghcl[j] = ghph[v[j]];  
  }
}

//______________________________________________________________________________
void EXOATeamComptonModule::UnSequence( int v[]) {
  int g[3];
  
  for (int j = 0; j < 3; j++) {
    g[v[j]] = j;
  }

  Sequence( g );
}

//______________________________________________________________________________
void EXOATeamComptonModule::Order() {
  int g[3] = {1, 0, 2};
  int h[3] = {0, 1, 2};
  int seq_num = 0;
  double figure = 0.0;
  double  fig_min = 1000;

  if (ncl == 2) {
    if (ecl[0] < ecl[1]) {
      Sequence(g);
    }
  }

  else if (ncl == 3) {
    for (int j = 0; j < 6; j++) {
      figure = seq_FOM(h);
      if (figure < fig_min) {
        fig_min = figure;
        seq_num = j;
      }
      next(h);
    }
    
    FOM = fig_min;
    for (int j = 0; j < 3; j++) {h[j] = j;}
    for (int j = 0; j < seq_num; j++) {next(h);}
    Sequence(h);
  }

  else {cout<<"This function is only for two and three site events."<<endl;}
}

