PKGNAME      = EXOReconstruction
top_builddir ?= ../..
top_srcdir   = ../..
DEPENDLIB    = EXOUtilities $(if $(findstring no, $(STATIC)), Minuit Hist Graf RIO $(ROOFIT_LIBS))

include $(top_builddir)/make/Makefile.inc
# for Recon*.hh
INCLUDE      += -I$(INCLUDEDIR)/EXOReconstruction $(if $(findstring yes, $(USE_THREADS)), $(BOOST_INCLUDE))
