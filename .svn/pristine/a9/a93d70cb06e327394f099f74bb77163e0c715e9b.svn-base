#ifndef EXORecVerbose_hh
#define EXORecVerbose_hh
#include <string>

class EXOTalkToManager;
class EXORecVerbose
{
  public:
    EXORecVerbose();

    void SetOutputUWireText(bool value = true);
    void SetOutputVWireText(bool value = true);
    void SetOutputAPDText(bool value = true);
    void SetOutputUWireToScreen(bool value = true);
    void SetOutputVWireToScreen(bool value = true);
    void SetOutputAPDToScreen(bool value = true);
    void SetOutputUWireToFile(bool value = true);
    void SetOutputVWireToFile(bool value = true);
    void SetOutputAPDToFile(bool value = true);
    void SetOutputInductionText(bool value = true);
    void SetOutputInductionToScreen(bool value = true);
    void SetOutputInductionToFile(bool value = true);
    bool ShouldPrintUWireText() const;
    bool ShouldPrintVWireText() const;
    bool ShouldPrintAPDText() const;
    bool ShouldPrintInductionText() const;
    bool ShouldPrintTextForChannel(int channel) const;
    bool ShouldPlotUWireToScreen() const;
    bool ShouldPlotVWireToScreen() const;
    bool ShouldPlotAPDToScreen() const;
    bool ShouldPlotInductionToScreen() const;
    bool ShouldPlotToScreenForChannel(int channel) const;
    bool ShouldPlotUWireToFile() const;
    bool ShouldPlotVWireToFile() const;
    bool ShouldPlotAPDToFile() const;
    bool ShouldPlotInductionToFile() const;
    bool ShouldPlotToFileForChannel(int channel) const;

    bool ShouldPlotForChannel(int channel) const;

    bool ShouldDoNothing() const 
      { return fBitMask == 0; }

    void TalkTo(const std::string& prefix, EXOTalkToManager* talkTo);

    void SetOutputToScreen(std::string);
    void SetOutputToFile(std::string);
    void SetOutputToText(std::string);

  protected:
    void SetBit(unsigned int pos, bool value);
    bool GetBit(unsigned int pos) const;

    unsigned short fBitMask; // 12 bits 876543210:
                      //        0: U-Wire text output?
                      //        1: V-Wire text output?
                      //        2: APD text output?
                      //        3: U-Wire plot on screen?
                      //        4: V-Wire plot on screen?
                      //        5: APD plot on screen?
                      //        6: U-Wire plot to file?
                      //        7: V-Wire plot to file?
                      //        8: APD plot to file?
                      //        9: Induction text output?
                      //       10: Induction plot on screen?
                      //       11: Induction plot to file?
};

#endif
