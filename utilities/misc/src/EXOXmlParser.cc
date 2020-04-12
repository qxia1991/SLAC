#include "EXOUtilities/EXOXmlParser.hh"
#ifdef HAVE_ROOT_XML
#include "TXMLDocument.h"
#include "TXMLNode.h"
#endif
#include "EXOUtilities/EXOErrorLogger.hh"
#include <list>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

//______________________________________________________________________________
TXMLNode* EXOXmlParser::LookUp(const std::string& pathToNode, char delimiter) const
{
  // Look up the path of the current document
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
  return NULL;
#else
  if (!fXMLDoc) return NULL;
  return LookUp(fXMLDoc->GetRootNode(), pathToNode, delimiter);
#endif
}

//______________________________________________________________________________
TXMLNode* EXOXmlParser::LookUp(TXMLNode* aNode, const std::string& pathToNode, char delimiter) const
{
  // Look up a given node given a path
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
  return NULL;
#else
  list<string> items; 
  string item;
  stringstream ss(pathToNode);
  while(getline(ss, item, delimiter)) items.push_back(item);

  TXMLNode* currentNode = aNode; 
  while( currentNode != NULL && items.size() > 0 ) {
    currentNode = FindChildByName(items.front(), currentNode);
    items.pop_front();
  }
  return currentNode;
#endif
}

//______________________________________________________________________________
TXMLNode* EXOXmlParser::FindChildByName(const std::string& name, TXMLNode* parent) const
{
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
  return NULL;
#else
  // Find the child of this node given a certain name
  if (name == "" || parent == NULL) return NULL;

  TXMLNode* child = parent->GetChildren();
  while (child != NULL) {
    if (std::string(child->GetNodeName()) == name) return child;
    child = child->GetNextNode();
  }

  LogEXOMsg(Form("Couldn't find child '%s' under parent '%s'", name.c_str(), parent->GetNodeName()), EEWarning);

  return NULL;
#endif
}
