#ifndef EXOTriggerConfigReader_hh
#define EXOTriggerConfigReader_hh

#ifdef HAVE_ROOT_XML
#include "TDOMParser.h"
#endif

#include "EXOUtilities/EXOTriggerConfig.hh"
#include <string>

class TXMLDocument;
class TXMLNode;
class TXMLAttr;

class EXOTriggerConfigReader
{
  public:
    EXOTriggerConfigReader();
    ~EXOTriggerConfigReader();
    int OpenAndParse(const std::string filename);
    void Close();

#ifdef HAVE_ROOT_XML
    TXMLNode* GetChild( TXMLNode* parent, const std::string name); //Get parents child node that has the name name, return NULL otherwise
    TXMLNode* GetChild( TXMLNode* parent); //Get parents first child node. To access subsequent child nodes call GetNextNeighbour() on the child node. Returns NULL if node has no child.
    TXMLNode* GetNextNeighbour( TXMLNode* neighbour, const std::string name); //Get node's next neighbour with name name. Returns NULL if no next node with this name.
    TXMLNode* GetNextNeighbour( TXMLNode* neighbour); // Get node's next neighbour. Returns NULL if node has no next neighbour.
    TXMLNode* GetPreviousNeighbour( TXMLNode* neighbour, const std::string name); //Get node's previous neighbour with name name. Returns NULL if no previous node with this name.
    TXMLNode* GetPreviousNeighbour( TXMLNode* neighbour); //Get node's previous neighbour. Returns NULL if node has no previous neighbour.
    TXMLNode* GetParent( TXMLNode* child); //Get node's parent node. Returns NULL if node has no parent.

    TXMLAttr* GetTXMLAttribute( TXMLNode* node, const std::string name); //Returns a TXMLAttr* if node has a XML attibute with name name, NULL otherwise.
    template <class T>
    T GetAttribute( TXMLNode* node, const std::string name); //Returns an attribute of type T.
#endif

    int FillConfiguration();
    int FillTriggerConfiguration();
    int FillBaselineConfiguration();
    bool CheckConsistency() const;

    EXOTriggerConfig fAPDConfig, fVGDConfig, fHVDConfig;

  private:

#ifdef HAVE_ROOT_XML
    TDOMParser fParser;
    TXMLDocument *fpDocument;
    TXMLNode *fpRootNode;
#endif

};

#endif
