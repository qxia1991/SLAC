PKGNAME      = EXOUtilities
top_builddir ?= ../../..
top_srcdir   = ../../..
DICTHFILES   = $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*.hh)
LDFLAGS      += -L$(ROOT_LIBDIR) $(FFTW_LDFLAGS) $(FFTW_LIBS) $(CURL_LIB) 
DEPENDLIB    = $(if $(findstring no,$(STATIC)),Geom)                        \
               $(if $(findstring no,$(STATIC)), Minuit Gpad Core Cint              \
               $(if $(findstring HAVE_ROOT_XML,$(DEFS)), XMLParser) Graf    \
               Hist MathCore Matrix RIO Tree Net Thread) 

EXTRAFILES   = $(wildcard $(PKGROOT)/data/*/*) 

include $(top_builddir)/make/Makefile.inc
