PKGNAME      = EXOROOT
top_builddir ?= ../../..
top_srcdir   = ../../..
MODNAME     := EXOROOTModules
DICTHFILES   = $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*.hh) 
LINKDEF      = $(INCLUDEDIR)/$(PKGNAME)/LinkDef.h
DEPENDLIB    = EXOUtilities       \
               EXOCalibUtilities  \
               EXOAnalysisManager \
               EXOReconstruction  \
               $(if $(findstring no,$(STATIC)),Html) \
               $(if $(findstring yes,$(HAVE_GEANT4)),EXOSim) \
               dl

include $(top_builddir)/make/Makefile.inc

