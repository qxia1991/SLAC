#ifndef EXOXmlParser_hh
#define EXOXmlParser_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#include <string>

class TXMLDocument;
class TXMLNode;

class EXOXmlParser : public TObject {

  public:
    EXOXmlParser(TXMLDocument* doc = NULL) : fXMLDoc(doc) {}

    TXMLNode* LookUp(const std::string& pathToNode, char delimiter = ':') const;
    TXMLNode* LookUp(TXMLNode* aNode, const std::string& pathToNode, char delimiter = ':') const;
    TXMLNode* FindChildByName(const std::string& name, TXMLNode* parent) const; 

  private:
    TXMLDocument* fXMLDoc;

  ClassDef( EXOXmlParser, 0 )
};

#endif /* EXOXmlParser_hh */
