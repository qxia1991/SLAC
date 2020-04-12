PKGNAME      = EXOSim
top_builddir ?= ../../..
top_srcdir   = ../../..

ifeq ($(GEANT4_INCDIR),)
  GEANT4_INCDIR1 = $(word 1, $(filter %/include/Geant4 %/include/geant4, $(patsubst -I%, %, $(filter -I%, $(GEANT4_INCLUDE)))))
  GEANT4_INCDIR2 = $(if $(GEANT4_INCDIR1),$(GEANT4_INCDIR1),$(shell echo "$(patsubst -I%, %, $(filter -I%, $(GEANT4_INCLUDE)))" | $(SED) 's/\([^ ]*[Gg][Ee][Aa][Nn][Tt][^ ]*\)/\1/ p;d;'))
  GEANT4_INCDIR = $(if $(GEANT4_INCDIR2),$(GEANT4_INCDIR2),$(word 1, $(patsubst -I%, %, $(filter -I%, $(GEANT4_INCLUDE)))))
endif

EXTRAFILES   = $(wildcard $(PKGROOT)/data/initial_seeds.txt) 
#NOLINKDEF    = no
FILTERDICTFILES = $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*SD.hh)\
                  $(addprefix $(INCLUDEDIR)/$(PKGNAME)/, EXOSteppingVerbose.hh EXOElectronConversion.hh)
DICTHFILES = $(filter-out $(FILTERDICTFILES), $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*.hh)) \
             $(addprefix $(GEANT4_INCDIR)/, G4UImanager.hh G4UIcommandTree.hh G4String.hh G4UIcommand.hh) 

LINKDEF      = $(INCLUDEDIR)/$(PKGNAME)/EXOSimLinkDef.h
DEPENDLIB    = EXOUtilities $(if $(findstring no,$(STATIC)), Tree RIO)

include $(top_builddir)/make/Makefile.inc

ifneq ($(strip $(BASEFS)),)
  INCLUDE += -DEXO_HAS_FORTRAN 
  LDFLAGS += -lgfortran
endif

LDFLAGS += $(GEANT4_LDFLAGS) $(GEANT4_LIBS) $(if $(findstring no,$(STATIC)), $(ROOT_LIBS))
INCLUDE += $(GEANT4_INCLUDE) $(filter -D% -I% -W%, $(GEANT4_CPPFLAGS)) -Wno-non-virtual-dtor
# filter out GEANT4 build flags that can interfere with ours
ifneq ($(HAVE_GEANT4_CLHEP),yes)
  LDFLAGS += $(CLHEP_LDFLAGS) $(CLHEP_LIBS)
  INCLUDE += $(CLHEP_INCLUDE)
endif

