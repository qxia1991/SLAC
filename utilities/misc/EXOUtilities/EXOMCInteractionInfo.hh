#ifndef EXOMCInteractionInfo_hh
#define EXOMCInteractionInfo_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif

#include <string> 
class EXOMCInteractionInfo : public TObject
{                                   
    public:
        std::string fProcessName;   // Name of process
        Float_t     fX;             // Interaction position X, mm
        Float_t     fY;             // Interaction position Y, mm
        Float_t     fZ;             // Interaction osition Z, mm

    public:
        EXOMCInteractionInfo() : TObject() { EXOMCInteractionInfo::Clear(); }
        virtual void Clear(Option_t* = "");
            
    ClassDef(EXOMCInteractionInfo, 1)
};
#endif /* EXOMCInteractionInfo_hh */
