PKGNAME      = EXOAnalysis
top_builddir ?= ../../..
top_srcdir   = ../../..
MAKEEXE      = 1
DEPENDLIB    = EXOUtilities EXOAnalysisManager $(if $(findstring extlib,$(STATIC)), EXOReconstruction)

EXTRAFILES   = $(wildcard $(PKGROOT)/macros/*)

include $(top_builddir)/make/Makefile.inc
