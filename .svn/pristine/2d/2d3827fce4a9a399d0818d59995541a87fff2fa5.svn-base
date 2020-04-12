#include "EXOUtilities/EXOTriggerConfigReader.hh"
#include "TList.h"

#ifdef HAVE_ROOT_XML
#include "TXMLDocument.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#endif

#include <sstream>
#include <iostream>

EXOTriggerConfigReader::EXOTriggerConfigReader()
{

}

EXOTriggerConfigReader::~EXOTriggerConfigReader()
{
#ifdef HAVE_ROOT_XML
  fParser.ReleaseUnderlying();
#endif
}

int EXOTriggerConfigReader::OpenAndParse(const std::string filename)
{
#ifdef HAVE_ROOT_XML
  fParser.ReleaseUnderlying();
  fParser.SetValidate(false);
  int returncode = fParser.ParseFile(filename.c_str());
  switch(returncode){
    case 0: fpDocument = fParser.GetXMLDocument();
            if(fpDocument == NULL){
              std::cout << "EXOTriggerConfigReader: Parse could not be completed" << std::endl;
              return -7;
            }
            break;

    case -1: std::cout << "EXOTriggerConfigReader: Attempt to parse a second file while a parse is in progress" << std::endl;
             return -1;

    case -2: std::cout << "EXOTriggerConfigReader: Parse context is not created" << std::endl;
             return -2;

    case -3: std::cout << "EXOTriggerConfigReader: An error occured while parsing file" << std::endl;
             return -3;

    case -4: std::cout << "EXOTriggerConfigReader: A fatal error occured while parsing file" << std::endl;
             return -4;

    case -5: std::cout << "EXOTriggerConfigReader: Document is not well-formed" << std::endl;
             return -5;
  }
  fpRootNode = fpDocument->GetRootNode();
  return returncode;
#else
  std::cout << "EXOTriggerConfigReader: Error: ROOT needs to be built with XML support!" << std::endl;
  return -3;
#endif
}

void EXOTriggerConfigReader::Close()
{
#ifdef HAVE_ROOT_XML
  fParser.ReleaseUnderlying();
#endif
}

#ifdef HAVE_ROOT_XML
TXMLNode* EXOTriggerConfigReader::GetChild(TXMLNode* parent, const std::string name)
{
  //std::cout << "Trying to get child of " << parent->GetNodeName() << " with name " << name << std::endl;
  TXMLNode* node = GetChild(parent);
  if(node == NULL) return NULL;
  std::string childname(node->GetNodeName());
  //std::cout << "Candidate is: " << childname << std::endl;
  if(childname.compare(name) == 0){
    return node;
  }
  return GetNextNeighbour(node,name);
}

TXMLNode* EXOTriggerConfigReader::GetChild(TXMLNode* parent)
{
  TXMLNode* node = parent->GetChildren();
  if(node == NULL) return NULL;
  if((node->GetNodeType() == TXMLNode::kXMLTextNode) || (node->GetNodeType() == TXMLNode::kXMLCommentNode)){
    return GetNextNeighbour(node);
  }
  return node;
}

TXMLNode* EXOTriggerConfigReader::GetNextNeighbour(TXMLNode* neighbour, const std::string name)
{
  //std::cout << "Trying to get neighbour of " << neighbour->GetNodeName() << " with name " << name << std::endl;
  TXMLNode* node = GetNextNeighbour(neighbour);
  if(node == NULL) return NULL;
  std::string neighbourname(node->GetNodeName());
  while(neighbourname.compare(name) != 0){
    //std::cout << "Candidate is: " << neighbourname << std::endl;
    node = GetNextNeighbour(node);
    if(node == NULL){
      std::cout << "no neighbour with name " << name << " found" << std::endl;
      return NULL;
    }
    neighbourname = node->GetNodeName();
  }
  //std::cout << "Found neighbour: " << neighbourname << std::endl;
  return node;
}

TXMLNode* EXOTriggerConfigReader::GetNextNeighbour(TXMLNode* neighbour)
{
  TXMLNode* node = neighbour->GetNextNode();
  if(node == NULL) return NULL;
  //std::cout << "Candidate is: " << node->GetNodeName() << std::endl;
  while((node->GetNodeType() == TXMLNode::kXMLTextNode) || (node->GetNodeType() == TXMLNode::kXMLCommentNode)){
    node = node->GetNextNode();
    if(node == NULL) return NULL;
    //std::cout << "Candidate is: " << node->GetNodeName() << std::endl;
  }
  return node;
}

TXMLNode* EXOTriggerConfigReader::GetPreviousNeighbour(TXMLNode* neighbour, const std::string name) 
{
  TXMLNode* node = GetPreviousNeighbour(neighbour);
  if(node == NULL) return NULL;
  std::string neighbourname(node->GetNodeName());
  while(neighbourname.compare(name) != 0){
    node = GetPreviousNeighbour(node);
    if(node == NULL) return NULL;
    neighbourname = node->GetNodeName();
  }
  return node;
}

TXMLNode* EXOTriggerConfigReader::GetPreviousNeighbour( TXMLNode* neighbour) 
{
  TXMLNode* node = neighbour->GetPreviousNode();
  if(node == NULL) return NULL;
  while((node->GetNodeType() == TXMLNode::kXMLTextNode) || (node->GetNodeType() == TXMLNode::kXMLCommentNode)){
    node = node->GetPreviousNode();
    if(node == NULL) return NULL;
  }
  return node;
}

TXMLNode* EXOTriggerConfigReader::GetParent( TXMLNode* child) 
{
  return child->GetParent();
}

TXMLAttr* EXOTriggerConfigReader::GetTXMLAttribute( TXMLNode* node, const std::string name) 
{
  TList* attrlist = node->GetAttributes();
  if(attrlist == NULL) return NULL;
  for(int i=0; i<attrlist->GetEntries(); i++){
    TXMLAttr* attr = (static_cast<TXMLAttr*>(attrlist->At(i)));
    std::string attrname(attr->Key());
    if(attrname.compare(name) == 0) return attr;
  }
  return NULL;
}

template <class T>
T EXOTriggerConfigReader::GetAttribute( TXMLNode* node, const std::string name) 
{
  TXMLAttr* attr = GetTXMLAttribute(node,name);
  std::string valuestr;
  if(attr){
    valuestr = attr->GetValue();
  }
  std::stringstream ss(valuestr);
  T value;
  ss >> value;
  return value;
}
#endif

int EXOTriggerConfigReader::FillConfiguration()
{
#ifdef HAVE_ROOT_XML
  std::string name(fpRootNode->GetNodeName());
  if(name.compare("EXO_BaselineOffset") == 0){
    //std::cout << "EXOTriggerConfigReader: Root node is " << name << std::endl;
    //std::cout << "seem to have baseline offset configuration. trying to fill" << std::endl;
    return FillBaselineConfiguration();
  }
  else if(name.compare("EXO_PhysicsTrigger") == 0){
    //std::cout << "EXOTriggerConfigReader: Root node is " << name << std::endl;
    //std::cout << "seem to have physics trigger configuration. trying to fill" << std::endl;
    return FillTriggerConfiguration();
  }
  std::cout << "EXOTriggerConfigReader: error, node " << name << " not known." << std::endl;
  return -1;
#else
  return -1;
#endif
}

int EXOTriggerConfigReader::FillTriggerConfiguration()
{
#ifdef HAVE_ROOT_XML
  fAPDConfig.fSystem = EXOTriggerConfig::APD;
  fVGDConfig.fSystem = EXOTriggerConfig::VGD;
  fHVDConfig.fSystem = EXOTriggerConfig::HVD;

  TXMLNode* apdnode = GetChild(fpRootNode,"apd");
  if(apdnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find apd node" << std::endl;
    return -1;
  }
  TXMLNode* vgdnode = GetChild(fpRootNode,"vgd");
  if(vgdnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find vgd node" << std::endl;
    return -1;
  }
  TXMLNode* hvdnode = GetChild(fpRootNode,"hvd");
  if(hvdnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find hvd node" << std::endl;
    return -1;
  }

  TXMLNode* cdnode = GetChild(apdnode,"channel-disable");
  while(cdnode != NULL){
    int module = GetAttribute<int>(cdnode,"module");
    int channel = GetAttribute<int>(cdnode,"channel");
    fAPDConfig.fDisabledChannels.insert(std::pair<int,int>(module,channel));
    cdnode = GetNextNeighbour(cdnode,"channel-disable");
  }
  cdnode = GetChild(vgdnode,"channel-disable");
  while(cdnode != NULL){
    int module = GetAttribute<int>(cdnode,"module");
    int channel = GetAttribute<int>(cdnode,"channel");
    fVGDConfig.fDisabledChannels.insert(std::pair<int,int>(module,channel));
    cdnode = GetNextNeighbour(cdnode,"channel-disable");
  }
  cdnode = GetChild(hvdnode,"channel-disable");
  while(cdnode != NULL){
    int module = GetAttribute<int>(cdnode,"module");
    int channel = GetAttribute<int>(cdnode,"channel");
    fHVDConfig.fDisabledChannels.insert(std::pair<int,int>(module,channel));
    cdnode = GetNextNeighbour(cdnode,"channel-disable");
  }

  TXMLNode* individualnode = GetChild(apdnode,"individual");
  if(individualnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find apd individual node" << std::endl;
    return -1;
  }
  fAPDConfig.fIndividualHysteresis = GetAttribute<int>(individualnode,"hysteresis");
  TXMLNode* triggernode = GetChild(individualnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fAPDConfig.fIndividualTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }
  TXMLNode* sumnode = GetChild(apdnode,"sum");
  if(sumnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find apd sum node" << std::endl;
    return -1;
  }
  fAPDConfig.fSumHysteresis = GetAttribute<int>(sumnode,"hysteresis");
  fAPDConfig.fBackgroundCount = GetAttribute<size_t>(sumnode,"background-count");
  triggernode = GetChild(sumnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fAPDConfig.fSumTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }

  individualnode = GetChild(vgdnode,"individual");
  if(individualnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find vgd individual node" << std::endl;
    return -1;
  }
  fVGDConfig.fIndividualHysteresis = GetAttribute<int>(individualnode,"hysteresis");
  triggernode = GetChild(individualnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fVGDConfig.fIndividualTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }
  sumnode = GetChild(vgdnode,"sum");
  if(sumnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find vgd sum node" << std::endl;
    return -1;
  }
  fVGDConfig.fSumHysteresis = GetAttribute<int>(sumnode,"hysteresis");
  fVGDConfig.fBackgroundCount = GetAttribute<size_t>(sumnode,"background-count");
  triggernode = GetChild(sumnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fVGDConfig.fSumTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }

  individualnode = GetChild(hvdnode,"individual");
  if(individualnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find hvd individual node" << std::endl;
    return -1;
  }
  fHVDConfig.fIndividualHysteresis = GetAttribute<int>(individualnode,"hysteresis");
  triggernode = GetChild(individualnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fHVDConfig.fIndividualTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }
  sumnode = GetChild(hvdnode,"sum");
  if(sumnode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find hvd sum node" << std::endl;
    return -1;
  }
  fHVDConfig.fSumHysteresis = GetAttribute<int>(sumnode,"hysteresis");
  fHVDConfig.fBackgroundCount = GetAttribute<size_t>(sumnode,"background-count");
  triggernode = GetChild(sumnode,"trigger");
  while(triggernode != NULL){
    EXOTriggerConfig::Trigger trigger;
    trigger.fIndex = GetAttribute<int>(triggernode,"index");
    trigger.fThreshold = GetAttribute<int>(triggernode,"threshold");
    trigger.fPrescale = GetAttribute<int>(triggernode,"prescale");
    trigger.fName = GetAttribute<std::string>(triggernode,"name");
    fHVDConfig.fSumTriggerSet.insert(trigger);
    triggernode = GetNextNeighbour(triggernode,"trigger");
  }
  return 0;
#else
  return -1;
#endif
}

int EXOTriggerConfigReader::FillBaselineConfiguration()
{
#ifdef HAVE_ROOT_XML
  TXMLNode* infonode = GetChild(fpRootNode,"info");
  if(infonode == NULL){
    std::cout << "EXOTriggerConfigReader: could not find info node" << std::endl;
    std::cout << "parent node was " << fpRootNode->GetNodeName() << std::endl;
    return -1;
  }
  /*
  fAPDConfig.fBaselineReference = GetAttribute<int>(infonode,"reference");
  fHVDConfig.fBaselineReference = fVGDConfig.fBaselineReference = fAPDConfig.fBaselineReference;
  fAPDConfig.fBaselineComputedFromRun = GetAttribute<int>(infonode,"computedFromRun");
  fHVDConfig.fBaselineComputedFromRun = fVGDConfig.fBaselineComputedFromRun = fAPDConfig.fBaselineComputedFromRun;
  */
  TXMLNode* apdnode = GetChild(fpRootNode,"apd");
  while(apdnode != NULL){
    int module = GetAttribute<int>(apdnode,"module");
    int channel = GetAttribute<int>(apdnode,"channel");
    int offset = GetAttribute<int>(apdnode,"offset");
    std::pair<std::map<std::pair<int,int>,int>::iterator,bool> ret;
    ret = fAPDConfig.fChannelOffsets.insert(std::pair<std::pair<int,int>,int>(std::pair<int,int>(module,channel),offset));
    if(ret.second == false){
      std::cout << "EXOTriggerConfigReader: offset for channel (" << module << "|" << channel << ") already set: " << ret.first->second << std::endl;
      std::cout << "Not setting to new value " << offset << std::endl;
    }
    apdnode = GetNextNeighbour(apdnode,"apd");
  }
  TXMLNode* vgdnode = GetChild(fpRootNode,"vgd");
  while(vgdnode != NULL){
    int module = GetAttribute<int>(vgdnode,"module");
    int channel = GetAttribute<int>(vgdnode,"channel");
    int offset = GetAttribute<int>(vgdnode,"offset");
    std::pair<std::map<std::pair<int,int>,int>::iterator,bool> ret;
    ret = fVGDConfig.fChannelOffsets.insert(std::pair<std::pair<int,int>,int>(std::pair<int,int>(module,channel),offset));
    if(ret.second == false){
      std::cout << "EXOTriggerConfigReader: offset for channel (" << module << "|" << channel << ") already set: " << ret.first->second << std::endl;
      std::cout << "Not setting to new value " << offset << std::endl;
    }
    vgdnode = GetNextNeighbour(vgdnode,"vgd");
  }
  TXMLNode* hvdnode = GetChild(fpRootNode,"hvd");
  while(hvdnode != NULL){
    int module = GetAttribute<int>(hvdnode,"module");
    int channel = GetAttribute<int>(hvdnode,"channel");
    int offset = GetAttribute<int>(hvdnode,"offset");
    std::pair<std::map<std::pair<int,int>,int>::iterator,bool> ret;
    ret = fHVDConfig.fChannelOffsets.insert(std::pair<std::pair<int,int>,int>(std::pair<int,int>(module,channel),offset));
    if(ret.second == false){
      std::cout << "EXOTriggerConfigReader: offset for channel (" << module << "|" << channel << ") already set: " << ret.first->second << std::endl;
      std::cout << "Not setting to new value " << offset << std::endl;
    }
    hvdnode = GetNextNeighbour(hvdnode,"hvd");
  }
  return 0;
#else
  return -1;
#endif
}

bool EXOTriggerConfigReader::CheckConsistency() const
{
  int nsumtriggers, nindivtriggers;
  nsumtriggers = fAPDConfig.fSumTriggerSet.size();
  nindivtriggers = fAPDConfig.fIndividualTriggerSet.size();
  if(nsumtriggers > 4 || nindivtriggers > 4){
    std::cout << "EXOTriggerConfigReader: number of triggers not good!" << std::endl;
    std::cout << nindivtriggers << " individual and " << nsumtriggers << " sum apd triggers!" << std::endl;
    return false;
  }
  nsumtriggers = fVGDConfig.fSumTriggerSet.size();
  nindivtriggers = fVGDConfig.fIndividualTriggerSet.size();
  if(nsumtriggers > 4 || nindivtriggers > 4){
    std::cout << "EXOTriggerConfigReader: number of triggers not good!" << std::endl;
    std::cout << nindivtriggers << " individual and " << nsumtriggers << " sum vgd triggers!" << std::endl;
    return false;
  }
  nsumtriggers = fHVDConfig.fSumTriggerSet.size();
  nindivtriggers = fHVDConfig.fIndividualTriggerSet.size();
  if(nsumtriggers > 4 || nindivtriggers > 4){
    std::cout << "EXOTriggerConfigReader: number of triggers not good!" << std::endl;
    std::cout << nindivtriggers << " individual and " << nsumtriggers << " sum hvd triggers!" << std::endl;
    return false;
  }
  return true;
}
