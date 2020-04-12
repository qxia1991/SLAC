#include "EXOReconstruction/EXORecVerbose.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "TString.h"
#include <algorithm> 
using EXOMiscUtil::TypeOfChannel;
using EXOMiscUtil::ChannelIsUWire;
using EXOMiscUtil::ChannelIsVWire;
using EXOMiscUtil::ChannelIsAPD;
using EXOMiscUtil::ChannelIsUWireInduction;

//______________________________________________________________________________
EXORecVerbose::EXORecVerbose()
: fBitMask(0x0000)
{

}

//______________________________________________________________________________
void EXORecVerbose::SetBit(unsigned int pos, bool value)
{
  unsigned short mask = 0;
  if(value){
    mask = 1 << pos;
    fBitMask = fBitMask | mask;
  }
  else{
    mask = ~(1 << pos);
    fBitMask = fBitMask & mask;
  }
}

//______________________________________________________________________________
bool EXORecVerbose::GetBit(unsigned int pos) const
{
  unsigned short mask = 1 << pos;
  return mask & fBitMask;
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputUWireText(bool value)
{
  SetBit(0,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputVWireText(bool value)
{
  SetBit(1,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputAPDText(bool value)
{
  SetBit(2,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputInductionText(bool value)
{
  SetBit(9,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputUWireToScreen(bool value)
{
  SetBit(3,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputVWireToScreen(bool value)
{
  SetBit(4,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputAPDToScreen(bool value)
{
  SetBit(5,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputInductionToScreen(bool value)
{
  SetBit(10,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputUWireToFile(bool value)
{
  SetBit(6,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputVWireToFile(bool value)
{
  SetBit(7,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputAPDToFile(bool value)
{
  SetBit(8,value);
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputInductionToFile(bool value)
{
  SetBit(11,value);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPrintUWireText() const
{
  return GetBit(0);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPrintVWireText() const
{
  return GetBit(1);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPrintAPDText() const
{
  return GetBit(2);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPrintInductionText() const
{
  return GetBit(9);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotUWireToScreen() const
{
  return GetBit(3);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotVWireToScreen() const
{
  return GetBit(4);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotAPDToScreen() const
{
  return GetBit(5);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotInductionToScreen() const
{
  return GetBit(10);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotUWireToFile() const
{ 
  return GetBit(6);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotVWireToFile() const
{ 
  return GetBit(7);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotAPDToFile() const
{
  return GetBit(8);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotInductionToFile() const
{ 
  return GetBit(11);
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPrintTextForChannel(int channel) const
{
  EXOMiscUtil::EChannelType type = TypeOfChannel(channel);
  if(ChannelIsUWire(type)){
    return ShouldPrintUWireText();
  }
  else if(ChannelIsVWire(type)){
    return ShouldPrintVWireText();
  }
  else if(ChannelIsAPD(type)){
    return ShouldPrintAPDText();
  }
  else if(ChannelIsUWireInduction(type)){
    return ShouldPrintInductionText();
  }
  return false;
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotToScreenForChannel(int channel) const
{
  EXOMiscUtil::EChannelType type = TypeOfChannel(channel);
  if(ChannelIsUWire(type)){
    return ShouldPlotUWireToScreen();
  }
  else if(ChannelIsVWire(type)){
    return ShouldPlotVWireToScreen();
  }
  else if(ChannelIsAPD(type)){
    return ShouldPlotAPDToScreen();
  }
  else if(ChannelIsUWireInduction(type)){
    return ShouldPlotInductionToScreen();
  }
  return false;
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotToFileForChannel(int channel) const
{
  EXOMiscUtil::EChannelType type = TypeOfChannel(channel);
  if(ChannelIsUWire(type)){
    return ShouldPlotUWireToFile();
  }
  else if(ChannelIsVWire(type)){
    return ShouldPlotVWireToFile();
  }
  else if(ChannelIsAPD(type)){
    return ShouldPlotAPDToFile();
  }
  else if(ChannelIsUWireInduction(type)){
    return ShouldPlotInductionToFile();
  }
  return false;
}

//______________________________________________________________________________
bool EXORecVerbose::ShouldPlotForChannel(int channel) const
{
  return ShouldPlotToScreenForChannel(channel) || ShouldPlotToFileForChannel(channel);
}

//______________________________________________________________________________
void EXORecVerbose::TalkTo(const std::string& prefix, EXOTalkToManager* talkTo)
{
  talkTo->CreateCommand(prefix + "/verbose/plot",
                        "Set to plot channels for this recon stage.  "
                        "Form: [APD|U-Wire|V-Wire|Induction] [true|false].",
                        this,
                        "",
                        &EXORecVerbose::SetOutputToScreen );

  talkTo->CreateCommand(prefix + "/verbose/plottofile",
                        "Set to plot channels to file for this recon stage.  "
                        "Form: [APD|U-Wire|V-Wire|Induction] [true|false]",
                        this,
                        "",
                        &EXORecVerbose::SetOutputToFile );

  talkTo->CreateCommand(prefix + "/verbose/outputtext",
                        "Set to output text for channel types for this recon stage.  "
                        "Form: [APD|U-Wire|V-Wire|Induction] [true|false]",
                        this,
                        "",
                        &EXORecVerbose::SetOutputToText );

}

#define DEFINE_REC_VERBOSE_FUNC(def_name, command)                                \
  if (command == "") return;                                                      \
  std::istringstream istr(command);                                               \
  std::string boolStr;                                                            \
  std::string chanType;                                                           \
  istr >> chanType >> boolStr;                                                    \
  std::istringstream nstr(boolStr);                                               \
                                                                                  \
  if ( istr.fail() ) goto fail;                                                   \
                                                                                  \
  bool cmdBool;                                                                   \
  if (! (nstr >> cmdBool) ) {                                                     \
    std::transform( boolStr.begin(), boolStr.end(), boolStr.begin(), ::tolower ); \
    if (boolStr == "true") cmdBool = true;                                        \
    else if (boolStr == "false" ) cmdBool = false;                                \
    else goto fail;                                                               \
  }                                                                               \
  std::transform( chanType.begin(), chanType.end(), chanType.begin(), ::tolower );\
  if (chanType == "apd") SetOutputAPD ## def_name (cmdBool);                      \
  else if (chanType == "u-wire") SetOutputUWire ## def_name (cmdBool);            \
  else if (chanType == "v-wire") SetOutputVWire ## def_name (cmdBool);            \
  else if (chanType == "induction") SetOutputInduction ## def_name (cmdBool);     \
  else goto fail;                                                                 \
                                                                                  \
  goto success;                                                                   \
fail:                                                                             \
  LogEXOMsg(TString::Format("Error parsing string '%s'",                          \
    command.c_str()).Data(), EEError);                                            \
  return;                                                                         \
success:                                                                          \
 
//______________________________________________________________________________
void EXORecVerbose::SetOutputToScreen(std::string command)
{
  // Set output to screen, expects a string that looks like:
  // [apd|u-wire|v-wire] [true|false]
  DEFINE_REC_VERBOSE_FUNC(ToScreen, command); 
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputToText(std::string command)
{
  DEFINE_REC_VERBOSE_FUNC(Text, command); 
}

//______________________________________________________________________________
void EXORecVerbose::SetOutputToFile(std::string command)
{
  DEFINE_REC_VERBOSE_FUNC(ToFile, command); 
}
