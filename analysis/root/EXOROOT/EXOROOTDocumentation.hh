#ifndef _EXOROOTDocumentation
#define _EXOROOTDocumentation

// Classes to help deal with documentation
#include "THtml.h"
#include <string>
namespace EXOROOT {
class EXOPathDefinition : public ::THtml::TPathDefinition {

  public:
    bool GetDocDir(const ::TString& module, ::TString& doc_dir) const;

  ClassDef(EXOPathDefinition, 1) // Path definition for EXO to make docs

};
}

namespace EXOROOT {
class EXOFileDefinition : public ::THtml::TFileDefinition {

  public:
    bool GetFileName(const TClass* cl, bool decl, TString& out_filename, TString& out_fsys, THtml::TFileSysEntry** fse = 0) const;
    void SetSourcePath( const std::string& path ) { fSourcePaths = path.c_str(); }

  protected:
    TString fSourcePaths;

  ClassDef(EXOFileDefinition, 1) // File definition for EXO to make docs

};
}


namespace EXOROOT {
class EXOModuleDefinition : public ::THtml::TModuleDefinition {

  public:
    bool GetModule(::TClass* cl, ::THtml::TFileSysEntry* fse,
                    ::TString& out_modulename) const;
     
  ClassDef(EXOModuleDefinition, 0) // Module definition for EXO to make docs

};
}
#endif /* _EXOROOTModules_hh */
