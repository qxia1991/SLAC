#include "EXOEventDisplayGui.hh"
#include <TApplication.h>
#include <TGClient.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main( int argc, char **argv ) 

{

  // Print out an error if the user's display is not set properly.
  char * display;
  display = getenv("DISPLAY");
  if (display!=NULL)
    cout << "Using display" << display << "." << endl;
  else {
     cout << "Display is not set properly. Please open an X-terminal and try again." << endl << "If you are using ssh, disconnect and ssh again with the option '-X', as in" << endl << "'ssh -X <your username>@<your host>.'" << endl;
    exit(1);
  }



  if ( argc > 1 ) {
    std::string filename = argv[1];
    EXOEventApp theApp("App",&argc,argv);
    new EXOEventDisplayGui( gClient->GetRoot(), 200, 200, filename );
    theApp.Run();
  }
  else {
    EXOEventApp theApp("App",&argc,argv);
    new EXOEventDisplayGui( gClient->GetRoot(), 200, 200, "" );
    theApp.Run();
  }

  return 0;

}
