# Sandbox Makefile M. Marino Nov 2010
#
# This makefile serves to compile user plugin code
# Users should add the appropriate library and include
# flags below:
#
# e.g.
# INCLUDE += -I/path/to/my/plugin/dependency/include
# LDFLAGS += -L/path/to/my/plugin/dependency/lib -lplugindep
#
PKGNAME      = sandbox
top_builddir = ../..
top_srcdir   = ../..
DEPENDLIB    = EXOUtilities
include $(top_builddir)/make/Makefile.inc.plugin

INCLUDE += 
LDFLAGS += 


# The following is an example to check to make sure roofit exists, this is just so that 
# the directory always compiles 
ifndef HAVE_ROOT_ROOFIT
  HAVE_ROOT_ROOFIT = $(shell $(ROOT_CONFIG) --has-roofit)
endif
ifeq ($(HAVE_ROOT_ROOFIT),yes) 
  LDFLAGS += -L$(ROOT_LIBDIR) -lRooFit -lRooFitCore -lMinuit -lFoam
endif

ifeq ($(HAVE_EXOBIN),yes)
  INCLUDE += $(EXOBIN_INCLUDE)
  DEPENDLIB += EXOCalibUtilities
  LDFLAGS += $(if $(findstring no,$(STATIC)), -lTree) $(EXOBIN_LIBS)
endif

