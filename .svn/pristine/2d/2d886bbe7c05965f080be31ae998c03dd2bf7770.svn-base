import ROOT
import sys
import glob
import os.path

def gen_module_doc( output_dir, lib_dir, top_builddir,
                    input_dir, include_dirs,
                    doc_dir ):
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = ROOT.kError
  print "Outputting to: ", output_dir
  if ROOT.gSystem.Load(lib_dir + "/libEXOROOT") < 0:
    print "Error loading lib"
    return

  for plugin in glob.glob(lib_dir+"/../plugins/*.so"):
      ROOT.EXOROOT.LoadPlugin(plugin)

  ct = ROOT.gClassTable

  num = ct.Classes()
  ct.Init()
  gc = ROOT.TClass.GetClass
  seen_TObject = False
  for i in range(num):
      o = ct.Next()
      if seen_TObject and o == 'TObject':
          continue
      if o == 'TObject': seen_TObject = True
      gc(o, True)

  html = ROOT.THtml()
  exopd = ROOT.EXOROOT.EXOPathDefinition()
  exofd = ROOT.EXOROOT.EXOFileDefinition()
  #exofd.SetBasePath("/Users/mgmarino/software/exc_offline")
  all_paths = ""
  for apath in include_dirs: all_paths += "%s:" % os.path.abspath(os.path.join(top_builddir, apath))
  all_paths = all_paths[:-1]
  exofd.SetSourcePath(all_paths)
  exomd = ROOT.EXOROOT.EXOModuleDefinition()
  html.SetPathDefinition(exopd)
  html.SetModuleDefinition(exomd)
  html.SetFileDefinition(exofd)
  html.SetOutputDir(output_dir)
  html.SetDocPath(doc_dir)
  html.SetInputDir(input_dir)
  html.SetIncludePath(input_dir)
  html.SetIncludePath(all_paths)
  html.SetSourceDir(input_dir)
  html.SetHeader("exo_header.html")
  html.SetProductName("EXOoffline")
  html.SetHomepage("http://www-project.slac.stanford.edu/exo/")
  try:
    html.MakeAll(False, "EXO*", 1)
  except NotImplementedError: pass
  output_string = """
 #exo_header {
   margin:0px -13px
 }" << endl <<
 #exo_header img {
   margin: 10px
 }"""
  open(output_dir + "/ROOT.css", 'a').write(output_string)

if __name__ == '__main__':
  gen_module_doc(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[5], sys.argv[6:], sys.argv[4])
