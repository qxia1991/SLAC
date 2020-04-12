#include "EXOROOT/EXOROOTDocumentation.hh"
#include "TClass.h"
#include <iostream> 
#include "TSystem.h"
ClassImp(EXOROOT::EXOPathDefinition)
ClassImp(EXOROOT::EXOModuleDefinition)
ClassImp(EXOROOT::EXOFileDefinition)

//______________________________________________________________________________
bool EXOROOT::EXOPathDefinition::GetDocDir(const ::TString& module, 
  ::TString& doc_dir) const
{
  // Returns the directory of documentation for each module. Assumes it looks
  // like base/ModuleName

  doc_dir = GetOwner()->GetPathInfo().fDocPath + "/" + module;
  return true;
}

//______________________________________________________________________________
bool EXOROOT::EXOModuleDefinition::GetModule(::TClass* cl, ::THtml::TFileSysEntry* fse,
                                     ::TString& out_modulename) const 
{
  // Return the module name.  In EXOAnslysis, we assume this is the include
  // directory.  For example, in misc/utilities/EXOUtilities are the include
  // files and so EXOUtilities is the module name.

  if (!cl) return false;
  ::TString temp = cl->GetName();
  out_modulename = gSystem->BaseName(gSystem->DirName(cl->GetDeclFileName()));
  if ( out_modulename == "build" ) out_modulename = "EXOROOT";
  else if ( out_modulename == "src" ) out_modulename = "EXOPlugins";
  if (!out_modulename.BeginsWith("EXO")) return false; 
  return true;
}

//______________________________________________________________________________
bool EXOROOT::EXOFileDefinition::GetFileName(const ::TClass* cl, bool decl, 
  ::TString& out_filename, ::TString& out_fsys, ::THtml::TFileSysEntry** fse) const 
{
  // Figures out the filename of a class (both decl and impl files) given class info

  if (!cl) return false;
  ::TString temp = cl->GetName();

  TString impl(cl->GetImplFileName());
  if ( impl == "" ) {
    // We try to use the name of the .hh file in this case.
    ::TString basename = gSystem->BaseName(cl->GetDeclFileName());
    Ssiz_t last = basename.Last('.'); 
    if (last != -1 ) {
      basename.Remove(last); 
    } else {
      basename = temp; 
    }
    impl = "src/" + basename + ".cc";
  }
  out_filename = (decl) ? ::TString(cl->GetDeclFileName()) : impl;

  ::TString adir;
  Ssiz_t start = 0;
  while( fSourcePaths.Tokenize(adir, start, ::THtml::GetDirDelimiter()) ) { 
    ::TString temp_out = out_filename;
    if ( adir != "" && temp_out.BeginsWith(adir)) {
      temp_out.Remove(0, adir.Length());
      while (temp_out[0] == '/') temp_out.Remove(0, 1);
    }
    ::TString scratch = temp_out;
    out_fsys = gSystem->FindFile(adir, scratch, kReadPermission);
    if (out_fsys.Length() != 0) {
      out_filename = (decl) ? temp_out : scratch;
      break; 
    } 
  }
  if (out_fsys.Length() != 0) return true;
  out_filename = "";
  return false;
}

