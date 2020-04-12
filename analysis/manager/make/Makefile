PKGNAME      = EXOAnalysisManager
top_builddir ?= ../../..
top_srcdir   = ../../..
DEPENDLIB = EXOReconstruction EXOCalibUtilities EXOUtilities  \
            $(if $(findstring no,$(STATIC)), $(if $(findstring HAVE_ROOT_TMVA,$(DEFS)),TMVA XMLIO TreePlayer MLP Graf3d) \
            Physics Gpad Tree Net Spectrum Thread Hist MathCore RIO dl Graf)

svn_check := svn status
svn_prog  := svn
ifneq ($(shell svnversion -cn . | grep export),)
  # This generally means the copy has been exported, but could still be git
  svn_check := $(shell git status -s . > /dev/null 2>&1 && echo 'git status -s') 
  svn_prog  := $(shell git status -s . > /dev/null 2>&1 && echo 'git') 
endif
ifneq ($(svn_check),)
  # Only do this if we are in an svn version
  # if interface differ from svn but irev isn't changed -> update it
  file_name = ../EXOAnalysisManager/EXOAnalysisModule.hh
  file_chk := $(if $(shell $(svn_check) $(file_name)),$(if $(shell $(svn_prog) diff $(file_name) | grep '^\+ *irev *='),,yes))
  #file_chk := yes
  file_upd := $(if $(filter yes,$(file_chk)),$(shell awk 'BEGIN{srand();}{if(/^ *irev *= */){a=$$0;match(a,/^ *irev *= */);b=substr(a,RSTART+RLENGTH);b=b+1;printf("%s%d // do not remove %x\n",substr($$0,RSTART,RLENGTH),b,int(4294967295*rand()));}else{print;}}' $(file_name) >$(file_name).$$$$; mv $(file_name).$$$$ $(file_name)))
endif
  
include $(top_builddir)/make/Makefile.inc
