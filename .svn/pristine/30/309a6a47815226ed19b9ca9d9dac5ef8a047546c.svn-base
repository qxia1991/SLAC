PKGNAME      = EXOComptonImager
top_builddir ?= ../../..
top_srcdir   = ../../..
LDFLAGS      += -L$(ROOT_LIBDIR) 
DEPENDLIB    = $(if $(findstring no,$(STATIC)), Hist Graf Gpad MathCore Tree TreePlayer) EXOUtilities

include $(top_builddir)/make/Makefile.inc
