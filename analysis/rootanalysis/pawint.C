/*
pawint.C is a collection of C functions which attempt to imitate
PAW commands in a ROOT environment. It is intended primarily
for making plots in interactive sessions. The basic ideas are:

    - Histograms, vectors, and ntuples are accessed and manipulated
    using compiled C functions, rather than directly at the command
    line, so the the user does not have to worry about memory management.

    - All histogram and ntuple functions are overloaded so that the 
    user can refer to the histogram or ntuple by name or by an integer 
    ID number. For example, histogram "h101" can be plotted with 
    hplot("h101") or hplot(101), while histogram "myhist" can only be 
    plotted with hplot("myhist").

    - A set of global variables act as environment variables for 
    making plots. For example, set_hcol(2) sets the histogram line
    color to red, and all histograms subsequently plotted will
    pick up this attribute. The global variables are listed below.

    - If multiple histograms are plotted on top of each other using
    the option "same", the line style changes for each histogram 
    automatically. (Unless the user sets dmod, in which case all
    histograms use the dmod line style. This stops when the user
    sets dmod to 0.) 

    - If a canvas is subdivided using zon(nx,ny), the sub-pad
    number automatically advances for each plot unless the plot
    option "same" is given.

    - Vectors are stored as histograms with the x-axis running from
    0 to nentries-1. Vectors can only be referred to by name.

    - Simple text files containing comma separated values can be read into 
    vectors and ntuples using vread() and ntread(). 

    - Some simple vector arithmetic is available.


////////////////////Global variables////////////////////////////

Int_t hcol = 1;               // histogram line color
Int_t subpad = 0;             
Int_t npad = 1;
Int_t dmod = 1;               // histogram line style
bool dmodfixed = false;
Int_t mtyp = 1;               // marker style
Double_t mscf = 1.0;          // marker size
Int_t pmci = 1;               // marker color
bool ndc = false;             // use pad coordinates
Int_t txci = 1;               // text color
Double_t chhe = 0.035;        // text size
Double_t asiz = 0.05;         // axis title size
Int_t logx = 0;               // log x axis
Int_t logy = 0;               // log y axis
Int_t ltyp = 1;               // line style
Int_t plci = 1;               // line color
Int_t lwid = 1;               // line width
Double_t ylab = 1.0;          // x axis title offset
Double_t xlab = 1.5;          // y axis title offset


//////////////////Canvas functions///////////////////////////

void canvas() // create canvas
void print_canvas( char *filename ) // print canvas to an eps file
void zon( Int_t nx, Int_t ny ) // subdivide canvas


////////////////////histogram functions///////////////////////


void hplot( Int_t hid )
void hplot( Int_t hid, char * options ) // options are ROOT drawing options
void hplot( char * hname, char * options )
void hplot( char * hname )
void hcopy( char * h1name, char * h2name )
void hcopy( Int_t hid1, Int_t hid2 )
bool hexist( char *h1name ) 
bool hexist( Int_t hid )
bool hexist( Int_t hid, bool verbose )
bool hexist( char *h1name, bool verbose )
void max( Int_t hid, Double_t maximum )
void max( char * hname, Double_t maximum )
void min( Int_t hid, Double_t minimum )
void min( char * hname, Double_t minimum )
void hcreate( Int_t hid, Int_t nbins, Double_t xlo, Double_t xhi )
void hcreate( char *h1name, Int_t nbins, Double_t xlo, Double_t xhi )
Double_t hgetelement( char *h1name, Int_t i )
void hdel( Int_t hid ) 
void hdel( Int_t hid, bool verbose ) 
void hdel( char * hname ) 
void hdel( char * hname, bool verbose ) 
void atitle( char * hname , char * xtitle, char * ytitle ) 
void atitle( Int_t hid , char * xtitle, char * ytitle )
void itx( Double_t x, Double_t y, char * text ) 
void line( Double_t x1, Double_t x2, Double_t y1, Double_t y2 )
void arrow( Double_t x1, Double_t x2, Double_t y1, Double_t y2, Double_t size )


//////////////////////Set the global variables//////////////////////////// 

void set_hcol( Int_t color )
void set_opt_stat( Int_t mode )
void set_opt_stat()
void opt_nstat()
void set_dmod( Int_t drawmode )
void set_asiz( Double_t axistitlesize )
void set_mtyp( Int_t markertype )
void set_mscf( Double_t markerscalefactor )
void set_ndc( bool value )
void set_chhe( Double_t characterheight )
void set_txci( Int_t textcolor )
void set_pmci( Int_t markercolor )
void opt_logx()
void opt_logy()
void opt_linx()
void opt_liny()
void set_ltyp( Int_t linetype )
void set_plci( Int_t linecolor )
void set_lwid( Int_t linewidth )
void set_xlab( Double_t offset )
void set_ylab( Double_t offset )
void show() // prints the current global variables

///////////////////Vector functions///////////////////////////////////////

Int_t vread( char *vlist, char *filename )
Int_t vlength( char *v1name )
Double_t vgetelement( char *v1name, Int_t i )
void vputelement( char *v1name, Int_t i, Double_t element )
void vdel( char *v1name )
void vdel( char *v1name, bool verbose )
void vcopy( char *v1name, char *v2name )
void vcopy( char *v1name, char *v2name, Int_t i0, Int_t i1 )
bool vexist( char *v1name )
bool vexist( char *v1name, bool verbose )
void vcreate( char *v1name, Int_t nbins )
void vcreate( char *v1name, char *list_of_entries )
bool vsamelength( char *v1name, char *v2name )
void hputvect( char *hist, char *vect )
void hputvect( Int_t hid, char *vect )
void hgetvect( char *hist, char *vect )
void hgetvect( Int_t hid, char *vect )
void vplot( char *v1name, char *v2name, char *options  )
void vplot( char *v1name, char *v2name )
void vdraw( char *v1name, char* hname, char *options  )
void vdraw( char *v1name, Int_t hid, char *options  )
void vdraw( char *v1name, Int_t hid )
void vdraw( char *v1name, char *options )
void vdraw( char *v1name  )

///////////////////Vector math///////////////////////////////////

If the user uses the "vresult" version of these functions, the 
result of the operation is put into a vector of that name. 
Otherwise the result replaces the input vector.

void vscale( char *v1name, Double_t scale )
Int_t vscale( char *v1name, Double_t scale, char *vresult )
void voffset( char *v1name, Double_t offset )
Int_t voffset( char *v1name, Double_t offset, char *vresult )
void vdivide( char *v1name, char *v2name )
Int_t vdivide( char *v1name, char *v2name, char *vresult )
void vmult( char *v1name, char *v2name )
Int_t vmult( char *v1name, char *v2name, char *vresult )
void vadd( char *v1name, char *v2name )
Int_t vadd( char *v1name, char *v2name, char *vresult )
void vsub( char *v1name, char *v2name )
Int_t vsub( char *v1name, char *v2name, char *vresult )
void vinverse( char *v1name )
Int_t vinverse( char *v1name, char *vresult )
void vexp( char *v1name )
Int_t vexp( char *v1name, char *vresult )
void vlog( char *v1name )
Int_t vlog( char *v1name, char *vresult )
void vpow( char *v1name, Double_t exponent )
Int_t vpow( char *v1name, Double_t exponent, char *vresult )

///////////////////ntuple functions////////////////////////////

Int_t ntread( char *treename, char *varlist, char *filename )
Int_t ntread( Int_t treeid, char *varlist, char *filename )
void ntplot( char *treename, char *var, char *cuts, char* options )
void ntplot( Int_t treeid, char *var )
void ntplot( Int_t treeid, char *var, char *cuts )
void ntplot( Int_t treeid, char *var, char *cuts, char *options )
void ntplot( char *treename, char *var )
void ntplot( char *treename, char *var, char *cuts )
void ntprint( char *treename )
void ntprint( Int_t treeid)

//////////These functions are used by vread and ntread//////////////////

Int_t parselist( char *list, Int_t *iname )
Int_t validate_names( char *list, Int_t *iname, Int_t nname )
Int_t line_count( char *filename )

*/



#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <iostream>
using namespace std;
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TText.h>
#include <TLine.h>
#include <TArrow.h>
#include <TGraph.h>

#define MAXLIST 1000

Int_t hcol = 1;               // histogram line color
Int_t subpad = 0;       
Int_t npad = 1;
Int_t dmod = 1;               // histogram line style
bool dmodfixed = false;
Int_t mtyp = 1;               // marker style
Double_t mscf = 1.0;          // marker size
Int_t pmci = 1;               // marker color
bool ndc = false;             // use pad coordinates
Int_t txci = 1;               // text color
Double_t chhe = 0.035;        // text size
Double_t asiz = 0.05;         // axis title size
Int_t logx = 0;               // log x axis
Int_t logy = 0;               // log y axis
Int_t ltyp = 1;               // line style
Int_t plci = 1;               // line color
Int_t lwid = 1;               // line width
Double_t ylab = 1.0;          // x axis title offset
Double_t xlab = 1.5;          // y axis title offset

void hplot( Int_t hid )
{
  void hplot( char * hname, char * options );
  char hname[20];
  sprintf( hname, "h%d", hid);
  char options[] = "";
  hplot( hname, options );
}

void hplot( Int_t hid, char * options )
{
  void hplot( char * hname, char * options );
  char hname[20];
  sprintf( hname, "h%d", hid); 
  hplot( hname, options );
}


void hplot( char * hname, char * options )
{

  void canvas();

  TH1F *h = (TH1F*)gROOT->FindObject(hname);

  if ( h == NULL ) {
    cout << "couldn't find histogram " << hname << endl;
    return;
  }

  canvas();

  TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
  if ( c1 == NULL  ) return;

  if ( strstr(options,"same") == NULL ) {

    if ( subpad < npad ) {
      subpad++;
    }
    else {
      subpad = 1;
    }
    c1->cd(subpad);

    if ( dmodfixed == false ) dmod = 1;

  }
  else {

    if ( dmodfixed == false ) {
      dmod++;
      if ( dmod > 4 ) dmod = 1;
    }

  }

  h->UseCurrentStyle();
  h->SetLineColor(hcol);
  h->SetLineStyle(dmod);
  h->SetMarkerStyle(mtyp);
  h->SetMarkerSize(mscf);
  h->SetMarkerColor(pmci);
  h->SetTitleSize(asiz,"X");
  h->SetTitleSize(asiz,"Y");
  h->SetTitleOffset(xlab,"Y");
  h->SetTitleOffset(ylab,"X");

  gPad->SetLogx(logx);
  gPad->SetLogy(logy);

  h->DrawCopy(options);

  c1->Modified(true);
  c1->Update();

}

void hplot( char * hname )

{
  void hplot( char * hname, char * options );
  char options[] = "";
  hplot( hname, options );
}

void canvas()
{

  TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
  if ( c1 != NULL  ) return;

  cout << "creating canvas" << endl;

  gROOT->SetStyle("Plain");
  gStyle->SetLineWidth(1);
  gStyle->SetFillStyle(3000);
  gStyle->SetFillColor(0);
  gStyle->SetOptStat(0000000);
  gStyle->SetPaperSize(10,12);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
 
  c1 = new TCanvas("c1","",-2);
  c1->Clear();
  c1->SetFillStyle(3000);
  c1->SetFillColor(0);

}

void print_canvas( char *filename )
{
  TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
  if ( c1 == NULL  ) {
    cout << "can't find canvas c1" << endl;
    return;
  }
  c1->Print(filename);
}

void set_hcol( Int_t color )
{
  hcol = color;
}

void zon( Int_t nx, Int_t ny )
{

  void canvas();

  canvas();

  TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
  if ( c1 == NULL ) {
    cout << "can't create canvas???" << endl;
    return;
  }

  if ( nx > 1 || ny > 1 ) {
    cout << "setting paper size to 20 x 24" << endl;
    gStyle->SetPaperSize(20,24);
  }
  if ( nx == 1 && ny == 1 ) {
    cout << "setting paper size to 10 x 12" << endl;
    gStyle->SetPaperSize(10,12);
  }    
  if ( nx < 1 || ny < 1 ) {
    cout << "nx and ny should be >= 1" << endl;
    return;
  }


  c1->Clear();
  c1->Divide(nx,ny,0.01,0.01);
  c1->cd(1);

  subpad = 0;
  npad = nx*ny;

}

void set_opt_stat( Int_t mode )

{
  gStyle->SetOptStat(mode);
}
  
void set_opt_stat()

{
  gStyle->SetOptStat(1111111);
}
  

void opt_nstat()

{
  gStyle->SetOptStat(0000000);
}
  
void hcopy( char * h1name, char * h2name )

{


  TH1F *h1 = (TH1F*)gROOT->FindObject(h1name);

  if ( h1 == NULL ) {
    cout << "couldn't find histogram " << h1name << endl;
    return;
  }

  TH1F *htest = (TH1F*)gROOT->FindObject(h2name);

  if ( htest != NULL ) {
    cout << "copying over existing histogram " << h2name << endl;
    delete htest;
  }


  h1->Clone(h2name);

  TH1F *h2 = (TH1F*)gROOT->FindObject(h2name);

  if ( h2 == NULL ) {
    cout << "copy failed, NULL pointer found" << endl;
  }
  else if ( h2 == h1 ) {
    cout << "copy failed, output histogram same as input" << endl;
  }
  else {
    cout << "copy successful" << endl;
  }

  return;

}


void hcopy( Int_t hid1, Int_t hid2 )

{

  void hcopy( char * h1name, char * h2name );

  char h1name[20];
  sprintf( h1name, "h%d", hid1);
  char h2name[20];
  sprintf( h2name, "h%d", hid2);

  hcopy( h1name, h2name );

}

bool hexist( char *h1name )
{
  bool hexist( char *h1name, bool verbose );
  return hexist(h1name,false);
}

bool hexist( Int_t hid )
{
  bool hexist( char *h1name, bool verbose );
  char hname[20];
  sprintf( hname, "h%d", hid);
  return hexist(hname,false);
}

bool hexist( Int_t hid, bool verbose )
{
  bool hexist( char *h1name, bool verbose );
  char hname[20];
  sprintf( hname, "h%d", hid);
  return hexist(hname,verbose);
}
  
bool hexist( char *h1name, bool verbose )
{
  TH1F *h1 = (TH1F*)gROOT->FindObject(h1name);
  if ( h1 == NULL ) {
    if ( verbose == true ) {
      cout << "couldn't find histogram " << h1name << endl;
    }
    return false;
  }
  return true;
}



void max( Int_t hid, Double_t maximum )

{

  void max( char * hname, Double_t maximum );
  
  char hname[20];
  sprintf( hname, "h%d", hid);

  max( hname, maximum );

}

void max( char * hname, Double_t maximum )

{
  
  TH1F *h = (TH1F*)gROOT->FindObject(hname);

  if ( h == NULL ) {
    cout << "couldn't find histogram " << hname << endl;
    return;
  }

  h->SetMaximum(maximum);

}

void min( Int_t hid, Double_t minimum )

{

  void min( char * hname, Double_t minimum );
  
  char hname[20];
  sprintf( hname, "h%d", hid);

  min( hname, minimum );

}

void min( char * hname, Double_t minimum )

{
  
  TH1F *h = (TH1F*)gROOT->FindObject(hname);

  if ( h == NULL ) {
    cout << "couldn't find histogram " << hname << endl;
    return;
  }

  h->SetMinimum(minimum);

}


void set_dmod( Int_t drawmode )

{
  if ( drawmode == 0 ) {
    dmodfixed = false;
    dmod = 1;
  }
  else {
    dmodfixed = true;
    dmod = drawmode;
  }

}


void hcreate( Int_t hid, Int_t nbins, Double_t xlo, Double_t xhi )
{
  void hcreate( char *hname, Int_t nbins, Double_t xlo, Double_t xhi );
  char hname[20];
  sprintf( hname, "h%d", hid);
  hcreate( hname, nbins, xlo, xhi);
}

void hcreate( char *h1name, Int_t nbins, Double_t xlo, Double_t xhi )
{

  bool hexist( char *h1name );
  void hdel( char * hname ); 
  
  if ( hexist(h1name) == true ) hdel(h1name);
  if ( nbins < 1 ) {
    cout << "should have at least one bin" << endl;
    return;
  }
  if ( xhi <= xlo ) {
    cout << "xhi should be greater than xlo" << endl;
    return;
  }

  TH1F *h1 = new TH1F(h1name,"",nbins,xlo,xhi);
  h1->GetNbinsX(); // dummy statement to get rid of compiler warning

}

Double_t hgetelement( char *h1name, Int_t i )
{
  bool hexist( char *h1name, bool verbose );
  
  if ( hexist(h1name,true) == false ) return 0.0;
  TH1F *h1 = (TH1F*)gROOT->FindObject("h1name");
  Int_t nbins = h1->GetNbinsX();
  if ( i > nbins ) {
    cout << "only " << nbins << " bins in histogram " << h1name << endl;
    return 0.0;
  }
  if ( i < 0 ) {
    cout << "i should be >= 0" << endl;
    return 0.0;
  }
  Double_t result = h1->GetBinContent(i);
  if ( i == 0 ) {
    cout << "returning underflow bin" << endl;
  }
  else if ( i == nbins ) {
    cout << "returning overflow bin" << endl;
  }
  return result;
}
  

void hdel( Int_t hid ) 
{
  void hdel( char * hname, bool verbose );
  char hname[20];
  sprintf( hname, "h%d", hid);
  hdel( hname, true );
  return;
}
  
void hdel( Int_t hid, bool verbose ) 
{
  void hdel( char * hname, bool verbose );
  char hname[20];
  sprintf( hname, "h%d", hid);
  hdel( hname, verbose );
  return;
}
  
void hdel( char * hname ) 
{
  void hdel( char * hname, bool verbose );
  hdel( hname, true );
  return;
}

void hdel( char * hname, bool verbose ) 
{

  TH1F *h = (TH1F*)gROOT->FindObject(hname);
  if ( h == NULL) {
    if ( verbose == true ) {
      cout << "couldn't find histogram " << hname << endl;
      return;
    }
  }

  delete h;
  h = (TH1F*)gROOT->FindObject(hname);
  while ( h != NULL ) {
    delete h;
    h = (TH1F*)gROOT->FindObject(hname);
  }

  return;
}
  
void atitle( char * hname , char * xtitle, char * ytitle )

{

  TH1F *h = (TH1F*)gROOT->FindObject(hname);

  if ( h == NULL ) {
    cout << "couldn't find histogram " << hname << endl;
    return;
  }

  h->SetXTitle(xtitle);
  h->SetYTitle(ytitle);
  
  h->GetXaxis()->CenterTitle(true);
  h->GetYaxis()->CenterTitle(true);

}

  
void atitle( Int_t hid , char * xtitle, char * ytitle )

{

  void atitle( char * hname , char * xtitle, char * ytitle );
  
  char hname[20];
  sprintf( hname, "h%d", hid);

  atitle( hname, xtitle, ytitle );
  
}

void set_asiz( Double_t axistitlesize )

{
  asiz = axistitlesize;
}

void set_mtyp( Int_t markertype )

{
  mtyp = markertype;
}
  
void set_mscf( Double_t markerscalefactor )

{
  mscf = markerscalefactor;
}

void set_ndc( bool value )

{
  ndc = value;
}

void itx( Double_t x, Double_t y, char * text )

{

  TText *t = new TText( x, y, text );
  t->SetTextSize(chhe);
  t->SetTextColor(txci);
  t->SetNDC(ndc);
  t->Draw();

}

void set_chhe( Double_t characterheight )

{
  chhe = characterheight;
}

void set_txci( Int_t textcolor )

{
  txci = textcolor;
}

void set_pmci( Int_t markercolor )

{
  pmci = markercolor;
}

void opt_logx()

{
  logx = 1;
}


void opt_logy()

{
  logy = 1;
}


void opt_linx()

{
  logx = 0;
}

void opt_liny()

{
  logy = 0;
}

void set_ltyp( Int_t linetype )

{
  ltyp = linetype;
}

void set_plci( Int_t linecolor )

{
  plci = linecolor;
}

void set_lwid( Int_t linewidth )

{
  lwid = linewidth;
}

void set_xlab( Double_t offset )

{
  xlab = offset;
}

void set_ylab( Double_t offset )

{
  ylab = offset;
}


void line( Double_t x1, Double_t x2, Double_t y1, Double_t y2 )

{

  TLine *l = new TLine();
  l->SetLineColor(plci);
  l->SetLineWidth(lwid);
  l->SetLineStyle(ltyp);
 
  if ( ndc == true ) {
    l->DrawLineNDC( x1, y1, x2, y2 );
  }
  else {
    l->DrawLine( x1, y1, x2, y2 );
  }

}

void show()

{

  cout << "hcol (histogram color)      = " << hcol << endl;
  cout << "dmod (histogram line style) = " << dmod << endl;
  cout << "mtyp (marker style)         = " << mtyp << endl;
  cout << "mscf (marker size)          = " << mscf << endl;
  cout << "ndc  (pad coordinates)      = " << ndc << endl;
  cout << "txci (text color)           = " << txci << endl;
  cout << "chhe (character height)     = " << chhe << endl;
  cout << "pmci (marker color)         = " << pmci << endl;
  cout << "asiz (axis title size)      = " << asiz << endl;
  cout << "logx (log x axis)           = " << logx << endl;
  cout << "logy (log y axis)           = " << logy << endl;
  cout << "ltyp (line style)           = " << ltyp << endl;
  cout << "plci (line color)           = " << plci << endl;
  cout << "lwid (line width)           = " << lwid << endl;
  cout << "xlab (y axis title offset)  = " << xlab << endl;
  cout << "ylab (x axis title offset)  = " << ylab << endl;

}

void arrow( Double_t x1, Double_t x2, Double_t y1, Double_t y2, Double_t size )

{

  TArrow *a;

  if ( size > 0.0 ) {
    a = new TArrow( x1, y1, x2, y2, fabs(size), "|>");
  }
  else {
    a = new TArrow( x1, y1, x2, y2, fabs(size), "<|>");
  }

  a->SetLineColor(plci);
  a->SetLineWidth(lwid);
  a->SetLineStyle(ltyp);
  a->SetFillColor(plci);

  a->Draw();

}
   
Int_t parselist( char *list, Int_t *iname )

{

  // Parse a comma seperated list and extract the names
  // Will replace the commas with the null character.
  // Returns the starting location of each name in the array iname, 
  // and function returns the number of names found.

  // First get the number of characters in the list

  Int_t listlen = strlen( list );

  if ( listlen < 1 ) {
    cout << "list must have at least one character" << endl;
    return -1;
  }

  if ( list[listlen-1] == ',' || list[0] == ',' ) {
    cout << "misplaced comma in list" << endl;
    return -1;
  }
  
  // Count the number of names in the list

  Int_t nname = 1;
  for ( Int_t i = 0; i < listlen; i++ ) {
    if ( list[i] >= 'a' && list[i] <= 'z' ||
	 list[i] >= 'A' && list[i] <= 'A' ||
	 list[i] >= '0' && list[i] <= '9' ||
	 list[i] == ',' || list[i] == '-' || 
	 list[i] == '+' || list[i] == '.' ) {
      if ( list[i] == ',' ) nname++;
    }
    else {
      cout << "list must be alphanumeric" << endl;
      return -1;
    }
  }

  if ( nname > MAXLIST ) {
    cout << "maximum number of entries in list = " << MAXLIST 
	 << ", found " << nname  << endl;
    return -1;
  }

  // Find the starting positions for the names
  // Replace the commas with null characters

  iname[0] = 0;
  Int_t vcount = 0;
  for ( Int_t i = 1; i < listlen; i++ ) {
    if ( list[i] == ',' ) {
      vcount++;
      if ( vcount < nname ) iname[vcount] = i+1;
      list[i] = '\0';
    }
  }

  return nname;

}


   
Int_t validate_names( char *list, Int_t *iname, Int_t nname )

{


  for ( Int_t i = 0; i < nname; i++ ) {
    if ( list[iname[i]] >= 'a' && list[iname[i]] <= 'z' ||
	 list[iname[i]] >= 'A' && list[iname[i]] <= 'Z' ) {}
    else {
      cout << "names must start with a character" << endl;
      return -1;
    }
  }

  for ( Int_t i = 0; i < nname; i++ ) {
    Int_t len = strlen(&list[iname[i]]);
    for ( Int_t j = 0; j < len; j++ ) {
      if ( list[iname[i]+j] >= 'a' && list[iname[i]+j] <= 'z' ||
	   list[iname[i]+j] >= 'A' && list[iname[i]+j] <= 'Z' || 
	   list[iname[i]+j] >= '0' && list[iname[i]+j] <= '9' ) {}
      else {
        cout << "names must be alphanumeric" << endl;
        return -1;
      }
    }
  }

  for ( Int_t i = 0; i < nname-1; i++ ) {
    for ( Int_t j = i+1; j < nname; j++ ) {
      if ( strcmp(&list[iname[i]], &list[iname[j]]) == 0 ) {
	cout << "names should be unique" << endl;
	return -1;
      }
    }
  }


  return 0;

}



Int_t vread( char *vlist, char *filename )
{


  // This function reads in a list of comma separated values from
  // a text file and puts them into vectors. vlist should be a comma 
  // separated list of vector names. Each line in the text file
  // creates one entry in each vector. 

  Int_t parselist( char *list, Int_t *iname );
  Int_t validate_names( char *list, Int_t *iname, Int_t nname );
  Int_t line_count( char *filename );


  // get the number of names in the list

  Int_t iname[MAXLIST];
  Int_t nv = parselist( vlist, iname );
  
  // Make sure the names are valid
  
  if ( validate_names( vlist, iname, nv ) < 0 ) return -1;

  // Count the number of lines in the file

  Int_t nlines = line_count( filename );
  if ( nlines <= 0 ) return -1;

  // Delete any existing histograms and declare new histograms

  TH1F *hvect[MAXLIST];
  TH1F *htest = NULL;
  for ( Int_t i = 0; i < nv; i++ ) {
    htest = (TH1F*)gROOT->FindObject(&vlist[iname[i]]);
    if ( htest != NULL ) {
      printf("deleting histogram with name %s\n", &vlist[iname[i]]);
      delete htest;
    }
    printf("creating vector with name %s and length %d\n",
	   &vlist[iname[i]],nlines);
    hvect[i] = new TH1F(&vlist[iname[i]],"",nlines,0.0,(Double_t)nlines );
  }    
  
  // Read the data into the histograms

  FILE *fptr;
  fptr = fopen( filename, "r" );
  char line[1000];
  Int_t nentries = 0;
  Int_t ientries[MAXLIST];
  Double_t data;
  for ( Int_t i = 0; i < nlines; i++ ) {
    fscanf( fptr, "%999s", line );
    nentries = parselist( line, ientries );
    if ( nentries != nv ) {
      cout << "line number " << i << " has " << nentries 
	   << " entries" << endl;
      return -1;
    }
    for ( Int_t j = 0; j < nv; j++ ) {
      sscanf( &line[ientries[j]],"%le",&data);
      hvect[j]->SetBinContent(i+1,data);
    }
  }

  return nlines;

}  

Int_t line_count( char *filename )

{

  // Open file.

  FILE *fptr;
  if (  ( fptr = fopen( filename, "r" ) ) == NULL ) {
    printf("can't open input file\n");
    return -1;
  }

  Int_t result = 0;
  char line[1000];
  while ( fscanf( fptr, "%999s", line ) != EOF ) {
    result++;
  }
  fclose(fptr);

  cout << "found " << result << " lines in the file" << endl;

  return result;

}

Int_t vlength( char *v1name )
{
  bool vexist( char *v1name );
  if ( vexist(v1name) == false ) {
    return 0;
  }

  TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
  return (Int_t)v1->GetNbinsX();

}

Double_t vgetelement( char *v1name, Int_t i )
{
  bool vexist( char *v1name );
  if ( vexist(v1name) == false ) return 0.0;
  if ( i < 0 || i >= vlength(v1name) ) {
    cout << "vgetelement: index out of bounds" << endl;
    return 0.0;
  }
  TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
  Double_t result = v1->GetBinContent(i+1);
  return result;
} 

void vputelement( char *v1name, Int_t i, Double_t element )
{
  bool vexist( char *v1name );
  if ( vexist(v1name) == false ) return;
  if ( i < 0 || i >= vlength(v1name) ) {
    cout << "vputelement: index out of bounds" << endl;
    return;
  }
  TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
  v1->SetBinContent(i+1,element);
  return;
} 

void vdel( char *v1name )
{
  void vdel( char *v1name, bool verbose );
  vdel(v1name,false);
}

void vdel( char *v1name, bool verbose )
{

  TH1F *v = (TH1F*)gROOT->FindObject(v1name);
  if ( v == NULL) {
    if ( verbose == true ) {
      cout << "couldn't find vector " << v1name << endl;
      return;
    }
  }

  delete v;
  v = (TH1F*)gROOT->FindObject(v1name);
  while ( v != NULL ) {
    delete v;
    v = (TH1F*)gROOT->FindObject(v1name);
  }
  
  return;
}


void vcopy( char *v1name, char *v2name )
{  
  void vcopy( char * h1name, char * h2name, Int_t i0, Int_t i1 );
  bool vexist( char *v1name );
  Int_t vlength( char *v1name );
  if ( vexist(v1name) == false ) return;
  Int_t nbins = vlength(v1name);
  vcopy( v1name, v2name, 0, nbins-1 );
}


void vcopy( char *v1name, char *v2name, Int_t i0, Int_t i1 )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return;
  Int_t nbins = vlength(v1name);
  if ( i0 < 0 ) {
    cout << "i0 less than zero" << endl;
    return;
  }
  if ( i1 >= nbins ) {
    cout << "i1 should be < nbins" << endl;
    return;
  }
  if ( i1 < i0 ) {
    cout << "i1 should be >= i0" << endl;
    return;
  }

  vdel(v2name); 
  vcreate(v2name,i1-i0+1);

  Double_t result;
  for ( Int_t i = i0; i <= i1; i++ ) {
    result = vgetelement(v1name,i);
    vputelement(v2name,i-i0,result);
  }

  return;

}

bool vexist( char *v1name )
{
  bool vexist( char *v1name, bool verbose );
  bool result = vexist(v1name,false);
  return result;
}

bool vexist( char *v1name, bool verbose )
{
  TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
  if ( v1 == NULL ) {
    if ( verbose == true ) {
      cout << "couldn't find vector " << v1name << endl;
    }
    return false;
  }
  return true;
}
  
void vcreate( char *v1name, Int_t nbins )
{
  bool vexist( char *v1name, bool verbose );
  void vdel( char *v1name, bool verbose );
  if ( vexist(v1name,false) == true ) vdel(v1name,false);
  TH1F *v1 = new TH1F(v1name,"",nbins,0.0,(Double_t)nbins);
  v1->GetNbinsX(); // dummy statement to get rid of compiler warning
  return;
}
  
void vcreate( char *v1name, char *list_of_entries )
{

  void vcreate( char *v1name, Int_t nbins );
  Int_t parselist( char *list, Int_t *iname );
 
  Int_t ientries[MAXLIST];
  Double_t data;
  Int_t nentries = parselist( list_of_entries, ientries );
  if ( nentries < 0 ) return;

  vcreate(v1name,nentries);

  for ( Int_t j = 0; j < nentries; j++ ) {
    sscanf( &list_of_entries[ientries[j]],"%le",&data);
    vputelement(v1name,j,data);
  }

}
  

bool vsamelength( char *v1name, char *v2name )
{
  bool vexist( char *v1name );
  Int_t vlength( char *v1name );
  if ( vexist(v1name) == false ) return false;
  if ( vexist(v2name) == false ) return false;
  if ( vlength(v1name) == vlength(v2name) ) return true;
  cout << "vector " << v1name << " has length " << vlength(v1name) << endl;
  cout << "vector " << v2name << " has length " << vlength(v2name) << endl;
  return false;
}

void vscale( char *v1name, Double_t scale )
{

  // Replaces v1 with v1*scale;

  bool vexist( char *v1name );
  Int_t vscale( char *v1name, Double_t scale, char *vresult );
  void vcopy( char *v1name, char *v2name );
  void vcreate( char *v1name, Int_t nbins );

  if ( vexist(v1name) == false ) return;
  Int_t nbins = vlength(v1name);
  vcreate("vtemp",nbins);
  Int_t err = vscale( v1name, scale, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");

  return;


}

Int_t vscale( char *v1name, Double_t scale, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);

  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i)*scale;
    vputelement(vresult,i,result);
  }

  return 0;

}

void voffset( char *v1name, Double_t offset )
{

  // Replaces v1 with v1+offset;

  bool vexist( char *v1name );
  Int_t voffset( char *v1name, Double_t offset, char *vresult );
  void vcopy( char *v1name, char *v2name );
  void vcreate( char *v1name, Int_t nbins );

  if ( vexist(v1name) == false ) return;
  Int_t nbins = vlength(v1name);
  vcreate("vtemp",nbins);
  Int_t err = voffset( v1name, offset, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");

  return;

}


Int_t voffset( char *v1name, Double_t offset, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);
  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }

  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i) + offset;
    vputelement(vresult,i,result);
  }

  return 0;
}

void vdivide( char *v1name, char *v2name )
{

  // Replaces v1 with v1/v2;

  Int_t vdivide( char *v1name, char*v2name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vdivide( v1name, v2name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;

}

Int_t vdivide( char *v1name, char *v2name, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );
  bool vsamelength( char *v1name, char *v2name );

  if ( vexist(v1name) == false ) return -1;
  if ( vexist(v2name) == false ) return -1;
  if ( vsamelength(v1name,v2name) == false ) return -1;

  if ( strcmp(v1name,vresult) == 0 || strcmp(v2name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  Int_t nbins = vlength(v1name);
  vcreate(vresult,nbins);

  Double_t num, denom, result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    num = vgetelement(v1name,i);
    denom = vgetelement(v2name,i);
    if ( denom == 0.0 ) {
      cout << "vector " << v2name << " has zero at element " << i << endl;
      result = 0.0;
    }
    else {
      result = num/denom;
    }
    vputelement(vresult,i,result);
  }

  return 0;

}


void vmult( char *v1name, char *v2name )
{
  // Replaces v1 with v1*v2;

  Int_t vmult( char *v1name, char*v2name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vmult( v1name, v2name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;

}

Int_t vmult( char *v1name, char *v2name, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );
  bool vsamelength( char *v1name, char *v2name );

  if ( vexist(v1name) == false ) return -1;
  if ( vexist(v2name) == false ) return -1;
  if ( vsamelength(v1name,v2name) == false ) return -1;

  if ( strcmp(v1name,vresult) == 0 || strcmp(v2name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  Int_t nbins = vlength(v1name);
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i)*vgetelement(v2name,i);
    vputelement(vresult,i,result);
  }

  return 0;

}


void vadd( char *v1name, char *v2name )
{
  // Replaces v1 with v1+v2;

  Int_t vadd( char *v1name, char*v2name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vadd( v1name, v2name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;
}

Int_t vadd( char *v1name, char *v2name, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );
  bool vsamelength( char *v1name, char *v2name );

  if ( vexist(v1name) == false ) return -1;
  if ( vexist(v2name) == false ) return -1;
  if ( vsamelength(v1name,v2name) == false ) return -1;

  if ( strcmp(v1name,vresult) == 0 || strcmp(v2name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  Int_t nbins = vlength(v1name);
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i)+vgetelement(v2name,i);
    vputelement(vresult,i,result);
  }

  return 0;

}

void vsub( char *v1name, char *v2name )
{
  // Replaces v1 with v1+v2;

  Int_t vsub( char *v1name, char*v2name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vsub( v1name, v2name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;
}

Int_t vsub( char *v1name, char *v2name, char *vresult )
{

  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );
  bool vsamelength( char *v1name, char *v2name );

  if ( vexist(v1name) == false ) return -1;
  if ( vexist(v2name) == false ) return -1;
  if ( vsamelength(v1name,v2name) == false ) return -1;

  if ( strcmp(v1name,vresult) == 0 || strcmp(v2name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  Int_t nbins = vlength(v1name);
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i)-vgetelement(v2name,i);
    vputelement(vresult,i,result);
  }

  return 0;

}

void vinverse( char *v1name )
{
  // Replaces v1 with 1/v1;

  Int_t vinverse( char *v1name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vinverse( v1name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;

}

Int_t vinverse( char *v1name, char *vresult )
{
  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);

  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i);
    if ( result == 0.0 ) {
      cout << "vector " << v1name << " has zero at element " << i << endl;
      result = 0.0;
    }
    else {
      result = 1.0/result;
    }
    vputelement(vresult,i,result);
  }

  return 0;
}

void vexp( char *v1name )
{
  // Replaces v1 with exp(v1);

  Int_t vexp( char *v1name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vexp( v1name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;
}

Int_t vexp( char *v1name, char *vresult )
{
  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);

  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = exp(vgetelement(v1name,i));
    vputelement(vresult,i,result);
  }

  return 0;

}

void vlog( char *v1name )
{
  // Replaces v1 with ln(v1);

  Int_t vlog( char *v1name, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vlog( v1name, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;
}

Int_t vlog( char *v1name, char *vresult )
{
  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);

  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(v1name,i);
    if ( result <= 0.0 ) {
      cout << "vector " << v1name << " has negative element at " << i << endl;
      result = 0.0;
    }
    else {
      result = log(result);
    }
    vputelement(vresult,i,result);
  }

  return 0;
}


void vpow( char *v1name, Double_t exponent )
{
  // Replaces v1 with v1^exponent;

  Int_t vpow( char *v1name, Double_t exponent, char *vresult );
  void vcopy( char *v1name, char *v2name );

  Int_t err = vpow( v1name, exponent, "vtemp");
  if ( err == 0 ) vcopy("vtemp",v1name);
  vdel("vtemp");
  return;

}

Int_t vpow( char *v1name, Double_t exponent, char *vresult )
{
  bool vexist( char *v1name );
  void vdel( char *v1name );
  void vcreate( char *v1name, Int_t nbins );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(v1name) == false ) return -1;
  Int_t nbins = vlength(v1name);

  if ( strcmp(v1name,vresult) == 0 ) {
    cout << "input and output vectors should not be the same" << endl;
    return -1;
  }
  vcreate(vresult,nbins);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = pow(vgetelement(v1name,i),exponent);
    vputelement(vresult,i,result);
  }

  return 0;

}

void hputvect( char *hist, char *vect )
{

  // Puts the vector contents into the histogram.
  // Histogram must already exist.

  bool vexist( char *v1name, bool verbose );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(vect,true) == false ) return;
  if ( vexist(hist,true) == false ) return;
  if ( vsamelength(hist,vect) == false ) return;
  Int_t nbins = vlength(vect);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(vect,i);
    vputelement(hist,i,result);
  }

  return;

}

void hputvect( Int_t hid, char *vect )
{
  void hputvect( char *hist, char *vect );
  char hname[20];
  sprintf( hname, "h%d", hid);
  hputvect( hname, vect );
}

void hgetvect( char *hist, char *vect )
{

  // Puts the histogram contents into the vector.
  // Histogram must already exist.

  bool vexist( char *v1name );
  bool vsamelength( char *v1name, char *v2name );
  Double_t vgetelement( char *v1name, Int_t i );
  void vputelement( char *v1name, Int_t i, Double_t element );

  if ( vexist(vect) == false ) return;
  if ( vexist(hist) == false ) return;
  if ( vsamelength(hist,vect) == false ) return;
  Int_t nbins = vlength(vect);

  Double_t result;
  for ( Int_t i = 0; i < nbins; i++ ) {
    result = vgetelement(hist,i);
    vputelement(vect,i,result);
  }

  return;

}

void hgetvect( Int_t hid, char *vect )
{
  void hgetvect( char *hist, char *vect );
  char hname[20];
  sprintf( hname, "h%d", hid);
  hgetvect( hname, vect );
}


void vplot( char *v1name, char *v2name, char *options  )
{

  // Make sure the vectors exist

  TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
  if ( v1 == NULL ) {
    cout << "couldn't find vector " << v1name << endl;
    return;
  }

  TH1F *v2 = (TH1F*)gROOT->FindObject(v2name);
  if ( v2 == NULL ) {
    cout << "couldn't find vector " << v2name << endl;
    return;
  }

  Int_t nbins1 = v1->GetNbinsX();
  Int_t nbins2 = v2->GetNbinsX();
  if ( nbins1 != nbins2 ) {
    cout << "vector " << v1name << " has " << nbins1 << " bins" << endl;
    cout << "vector " << v2name << " has " << nbins2 << " bins" << endl;
    return;
  }

  // See if we should superpose this plot on the existing one

  bool superpose = false;
  Int_t len = strlen( options );
  for ( Int_t i = 0; i < len; i++ ) {
    if ( options[i] == 's' ) superpose = true;
  }

    // Use TTree::Draw() to fill and draw scatterplot

  TH1F *h12345;  
  if ( superpose == false ) {

    // advance the pad number

    canvas();
    TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
    if ( c1 == NULL  ) return;
    if ( subpad < npad ) {
      subpad++;
    }
    else {
      subpad = 1;
    }
    c1->cd(subpad);

    // Create an empty histogram for the background

    Double_t xmax = v1->GetMaximum();
    Double_t xmin = v1->GetMinimum();
    Double_t ymax = v2->GetMaximum();
    Double_t ymin = v2->GetMinimum();

    Double_t xavg = (xmax + xmin)/2.0;
    Double_t yavg = (ymax + ymin)/2.0;

    Double_t xmin0 = xavg - 0.8*(xmax - xmin);
    Double_t xmax0 = xavg + 0.8*(xmax - xmin);
    Double_t ymin0 = yavg - 0.8*(ymax - ymin);
    Double_t ymax0 = yavg + 0.8*(ymax - ymin);

    h12345 = (TH1F*)gROOT->FindObject("h12345");
    if ( h12345 != NULL ) delete h12345;
    h12345 = new TH1F("h12345","",1,xmin0,xmax0);
    h12345->SetMaximum(ymax0);
    h12345->SetMinimum(ymin0);
    h12345->SetBinContent(1,2.0*ymax0);
    h12345->DrawCopy();

  }

  // Make sure that we use the current polymarker

  char newoptions[1000];
  newoptions[0] = '\0';
  strcpy(newoptions,options);
  strcat(newoptions,"psame");
      
  TGraph *g12345 = new TGraph(nbins1);
  for ( Int_t i = 0; i < nbins1; i++ ) {
    g12345->SetPoint(i,v1->GetBinContent(i+1),v2->GetBinContent(i+1));
  }

  g12345->SetMarkerStyle(mtyp);
  g12345->SetMarkerSize(mscf);
  g12345->SetMarkerColor(pmci);

  g12345->Draw(newoptions);

}

void vplot( char *v1name, char *v2name )
{
  void vplot( char *v1name, char *v2name, char *options  );
  char options[] = "";
  vplot(v1name,v2name,options);
}


void vdraw( char *v1name, char* hname, char *options  )
{
  // Uses the vector v1name to fill histogram hname.
  // If hname doesn't exist, it is created.

  bool vexist( char *v1name, bool verbose );
  bool hexist( char *h1name, bool verbose );
  void hcreate( char *h1name, Int_t nbins, Double_t xlo, Double_t xhi );
  Double_t vgetelement( char *v1name, Int_t i );
  void hplot( char * hname, char * options );
 
  // Make sure the vector exists

  if ( vexist(v1name,true) == false ) return;

  // See if the histogram exists

  if ( hexist(hname,false) != true ) {
    cout << "creating histogram " << hname << endl;
    TH1F *v1 = (TH1F*)gROOT->FindObject(v1name);
    Double_t max = v1->GetMaximum();
    Double_t min = v1->GetMinimum();
    Double_t xavg = (max + min)/2.0;
    Double_t xlo = xavg - 0.8*(max - min);
    Double_t xhi = xavg + 0.8*(max - min);
    hcreate(hname, 100, xlo, xhi);
  }

  TH1F *hist = (TH1F*)gROOT->FindObject(hname);
  hist->Reset();

  Int_t length = vlength(v1name);
  for ( Int_t i = 0; i < length; i++ ) {
    hist->Fill( vgetelement(v1name,i) );
  }

  hplot(hname,options);

}

void vdraw( char *v1name, Int_t hid, char *options  )
{
  void vdraw( char *v1name, char* hname, char *options  );
  char hname[20];
  sprintf( hname, "h%d", hid);
  vdraw( v1name, hname, options);
}

void vdraw( char *v1name, Int_t hid )
{
  void vdraw( char *v1name, char* hname, char *options  );
  char hname[20];
  sprintf( hname, "h%d", hid);
  char options[] = "";  
  vdraw( v1name, hname, options);
}

void vdraw( char *v1name, char *options )
{
  void vdraw( char *v1name, char* hname, char *options  );
  char hname[] = "h12345";  
  vdraw( v1name, hname, options);
}

void vdraw( char *v1name  )
{
  void vdraw( char *v1name, char* hname, char *options  );
  char hname[] = "h12345";
  char options[] = "";  
  vdraw( v1name, hname, options);
}

Int_t ntread( char *treename, char *varlist, char *filename )
{

  // This function reads in a list of comma separated values from
  // a text file and puts them into an ntuple. varlist should be a 
  // comma separated list of variable names for the new ntuple. Each 
  // line in the text file creates one event in the ntuple. 

  Int_t parselist( char *list, Int_t *iname );
  Int_t validate_names( char *list, Int_t *iname, Int_t nname );
  Int_t line_count( char *filename );
  Double_t vgetelement( char *v1name, Int_t i );

  // get the number of names in the list

  Int_t iname[MAXLIST];
  Int_t nvar = parselist( varlist, iname );
  
  // Make sure the names are valid
  
  if ( validate_names( varlist, iname, nvar ) < 0 ) return -1;

  // Create dummy vector names

  char vectlist[1000];
  vectlist[0] = '\0';
  char vtempname[16];
  char comma[] = ",\0";
  for ( Int_t i = 0; i < nvar; i++ ) {
    sprintf(vtempname, "vtemp%d", i );
    if ( strlen(vectlist) + strlen(vtempname) > 990 ) {
      cout << "too many variable names" << endl;
      return -1;
    }
    strcat(vectlist,vtempname);
    if ( i < nvar - 1 ) {
      strcat(vectlist,comma);
    }
  }

  // Call vread to read the data into the vectors

  Int_t nevents = vread( vectlist, filename);
  if ( nevents < 0 ) return -1;

  // Get memory for the variables

  Double_t *values = new Double_t[nvar];
  for ( Int_t i = 0; i < nvar; i++ ) values[i] = 0.0;

  // See if the ntuple already exists

  TTree *rootTree = (TTree*)gROOT->FindObject(treename);
  if ( rootTree != NULL ) {
    cout << "deleting existing ntuple " << treename << endl;
    delete rootTree;
  }

  // Create the tree

  rootTree = new TTree(treename,"data from run");

  // Create the branches

  char vartype[100];
  vartype[0] = '\0';
  for ( Int_t i = 0; i < nvar; i++ ) {
    strcpy(vartype,&varlist[iname[i]]);
    strcat(vartype,"/D");
    rootTree->Branch(&varlist[iname[i]], &values[i], vartype);
  }

  // Fill the ntuple

  for ( Int_t j = 0; j < nevents; j++ ) {
    for ( Int_t i = 0; i < nvar; i++ ) {
      sprintf(vtempname, "vtemp%d", i );
      values[i] = vgetelement(vtempname,j);
    }
    rootTree->Fill();
  }

  // delete the temporary vectors

  for ( Int_t i = 0; i < nvar; i++ ) {
    sprintf(vtempname, "vtemp%d", i );
    vdel(vtempname);
  }

    
  return nevents;

}

Int_t ntread( Int_t treeid, char *varlist, char *filename )
{
  Int_t ntread( char *treename, char *varlist, char *filename );
  char tname[20];
  sprintf( tname, "t%d", treeid);
  Int_t result = ntread( tname, varlist, filename);
  return result;
}


void ntplot( char *treename, char *var, char *cuts, char* options )
{

  void canvas();

  TTree *tree = (TTree*)gROOT->FindObject(treename);
  if ( tree == NULL ) {
    cout << "can't find ntuple " << treename << endl;
    return;
  }

  // See if we're doing a histogram or a scatterplot

  bool scatterplot = false;
  if ( strchr(var,':') != NULL ) scatterplot = true;

  // See if we're filling an existing histogram

  bool fillexisting = false;
  char *existingname = strstr(var,">>");
  if ( existingname != NULL ) {
    fillexisting = true;
    existingname = existingname + 2;
  }

  // Draw histograms using hplot

  if ( scatterplot == false ) { 
    // Use TTree:Draw() to fill histogram
    // Suppress the graphics output of TTree::Draw()
    tree->Draw(var,cuts,"goff");
    if ( fillexisting == false ) {
      hplot("htemp",options);
    }
    else {
      hplot(existingname,options);
    }
  }
  
  // Draw scatterplots 

  else {
    // Use TTree::Draw() to fill and draw scatterplot
    canvas();
    TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
    if ( c1 == NULL  ) return;
    if ( strstr(options,"same") == NULL ) {
      if ( subpad < npad ) {
        subpad++;
      }
      else {
        subpad = 1;
      }
      c1->cd(subpad);
    }
    if ( fillexisting == false ) {
      tree->SetMarkerStyle(mtyp);
      tree->SetMarkerSize(mscf);
      tree->SetMarkerColor(pmci);
      tree->Draw(var,cuts,options);
    }
    else {
      TH1F *hexisting = (TH1F*)gROOT->FindObject(existingname);
      hexisting->SetMarkerStyle(mtyp);
      hexisting->SetMarkerSize(mscf);
      hexisting->SetMarkerColor(pmci);
      tree->Draw(var,cuts,options);
    }
  }

}

void ntplot( Int_t treeid, char *var )
{
  void ntplot( char *treename, char *var, char *cuts, char *options );
  char tname[20];
  sprintf( tname, "t%d", treeid);
  char cuts[] = "";
  char options[] = "";
  ntplot( tname, var, cuts, options );
}

void ntplot( Int_t treeid, char *var, char *cuts )
{
  void ntplot( char *treename, char *var, char *cuts, char *options );
  char tname[20];
  sprintf( tname, "t%d", treeid);
  char options[] = "";
  ntplot( tname, var, cuts, options );
}

void ntplot( Int_t treeid, char *var, char *cuts, char *options )
{
  void ntplot( char *treename, char *var, char *cuts, char *options );
  char tname[20];
  sprintf( tname, "t%d", treeid);
  ntplot( tname, var, cuts, options );
}

void ntplot( char *treename, char *var )
{
  void ntplot( char *treename, char *var, char *cuts, char *options );
  char cuts[] = "";
  char options[] = "";
  ntplot( treename, var, cuts, options );
}

void ntplot( char *treename, char *var, char *cuts )
{
  void ntplot( char *treename, char *var, char *cuts, char *options );
  char options[] = "";
  ntplot( treename, var, cuts, options );
}


void ntprint( char *treename )
{
  TTree *tree = (TTree*)gROOT->FindObject(treename);
  if ( tree == NULL ) {
    cout << "can't find ntuple " << treename << endl;
    return;
  }
  tree->Print();
}

void ntprint( Int_t treeid)
{
  void nprint( char *treename);
  char tname[20];
  sprintf( tname, "t%d", treeid);
  ntprint( tname);
}

  
