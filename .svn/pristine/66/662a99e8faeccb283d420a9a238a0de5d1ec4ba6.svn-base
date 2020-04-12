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

PKGNAME      = EXOSimModule
top_builddir ?= ../../..
top_srcdir   = ../../..
DEPENDLIB    = EXOSim EXOUtilities

include $(top_builddir)/make/Makefile.inc.plugin

LDFLAGS += $(if $(findstring no,$(STATIC)), $(GEANT4_LDFLAGS) $(GEANT4_LIBS) $(ROOT_LIBS)) \
           -Wl,-rpath,$(ORIGINFLAGS) 
INCLUDE += $(GEANT4_INCLUDE) $(filter -D% -I% -W%, $(GEANT4_CPPFLAGS))
# filter out GEANT4 build flags that can interfere with ours
ifneq ($(HAVE_GEANT4_CLHEP),yes)
  LDFLAGS += $(CLHEP_LDFLAGS) $(CLHEP_LIBS)
  INCLUDE += $(CLHEP_INCLUDE)
endif

