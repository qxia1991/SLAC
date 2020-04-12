/* Example application for building and linking against EXOAnalysis.  One can
 * use this file together with the Makefile in the same directory to build an
 * analysis program that compiles and links against ROOT and EXOAnalysis,
 * instead of relying on the root or PyROOT interpreter which can both be
 * relatively slow for some analyses.  One can also add files (.cc, .hh files)
 * to this directory as additional classes become necessary.  Of course, these
 * files can be relocated to your local project as well.  M. Marino, 2011 */

#include "EXOUtilities/EXOEventData.hh"
#include <iostream>
using namespace std;

int main() 
{
  EXOEventData ed;
  cout << "Example application for linking against EXOAnalysis" << endl;
  cout << ed.GetNumUWireSignals() << endl;

  return 0;
}
