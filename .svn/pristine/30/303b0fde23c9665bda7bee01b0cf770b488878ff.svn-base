PKGNAME      = EXOCalibUtilities
#PKGROOT      = ..
top_builddir ?= ../../..
top_srcdir   = ../../..
LDFLAGS      += -L$(ROOT_LIBDIR)
DEPENDLIB    = EXOUtilities $(if $(findstring yes,$(HAVE_MYSQL)),EXODBUtilities) \
                 $(if $(findstring no,$(STATIC)), Hist Physics MathCore Tree RIO)

include $(top_builddir)/make/Makefile.inc
