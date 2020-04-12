PKGNAME      = EXOEventDisplay
DICTHFILES   = $(SOURCEDIR)/EXOEventDisplayGui.hh
top_builddir ?= ../../..
top_srcdir   = ../../..
MAKEEXE      = 1
LDFLAGS      += -L$(ROOT_LIBDIR) 
DEPENDLIB    = EXOUtilities EXOCalibUtilities EXOAnalysisManager \
               $(if $(findstring no,$(STATIC)), Core Cint RIO Net \
               Hist Graf Graf3d Gpad Tree Matrix Physics MathCore Thread Gui Minuit) \
               $(if $(findstring extlib,$(STATIC)), EXOReconstruction) 

include $(top_builddir)/make/Makefile.inc
