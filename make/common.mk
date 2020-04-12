# This makefile is included both from the top level Makefile and from Makefile.inc

include $(top_builddir)/make/config.mk

SOURCEDIR    = $(PKGROOT)/src
INCLUDEDIR   = $(PKGROOT)
BUILDDIR     = $(PKGROOT)/build

ourlibs     = utilities/misc utilities/database utilities/calib reconstruction analysis/manager analysis/jni analysis/root geant/EXOsim
ifeq ($(STATIC),extlibs)
EXEFLAGS += -Wl,--export-dynamic $(ROOT_LIBS) # Regardless of command-line options, we always want this.
ifeq ($(USE_THREADS),yes)
EXEFLAGS += $(BOOST_LIBS)
endif
else
LDFLAGS += -L$(ROOT_LIBDIR) -lCore -lCint 
endif
# If DEPENDLIB libs is filled, make sure that the correct flags are added to LDFLAGS
ifneq ($(strip $(DEPENDLIB)),)
  LDFLAGS += -L$(libdir) $(addprefix -l,$(DEPENDLIB)) 
endif

SILENT      ?= @
DllSuf      = $(SHREXT)
CCSuf       := cc
CSuf        := C
HHSuf       := hh
HSuf        := h
FSuf        := f
COMMON_INC  := $(ROOT_INCLUDE) $(JAVA_INCLUDE) $(MYSQL_INCLUDE) $(FFTW_INCLUDE)
INCLUDE     = -I$(SOURCEDIR) -I$(INCLUDEDIR) $(COMMON_INC)
INCLUDE     += $(addprefix -I$(top_builddir)/,$(ourlibs))
CCFLAGS     += $(CPPFLAGS) $(CXXFLAGS) -Wall -Wno-unused-variable -Wno-unused-parameter $(ROOTCFLAGS) $(DEFS)
OutPutOpt   := -o

BASECCS     := $(wildcard $(SOURCEDIR)/*.$(CCSuf))
BASECCO	    := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(basename $(notdir $(BASECCS)))))
DEPCCFILES  := $(addprefix $(BUILDDIR)/,$(addsuffix .d,$(basename $(notdir $(BASECCS)))))
BASECS      := $(wildcard $(SOURCEDIR)/*.$(CSuf))
BASECO	    := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(basename $(notdir $(BASECS)))))
DEPCFILES   := $(addprefix $(BUILDDIR)/,$(addsuffix .d,$(basename $(notdir $(BASECS)))))
BASEFS      := $(wildcard $(SOURCEDIR)/*.$(FSuf))
BASEFO	    := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(basename $(notdir $(BASEFS)))))
DEPFFILES   := $(addprefix $(BUILDDIR)/,$(addsuffix .d,$(basename $(notdir $(BASEFS)))))

PUBLICFILES := $(subst $(INCLUDEDIR),$(includedir),$(wildcard $(INCLUDEDIR)/${PKGNAME}/*.$(HHSuf)))

# NOLINKDEF explicity tells us not to make rootify a directory 
ifndef NOLINKDEF 
# Check to see if either LINKDEF is set or if there's a manual one
  ifndef LINKDEF
    LINKDEF    := $(wildcard $(SOURCEDIR)/*LinkDef.$(HSuf))
  endif
  ifndef LINKDEF
    ifndef MAKEEXE
    # use the auto-linkdef
      AUTOLINKDEF  := $(BUILDDIR)/$(PKGNAME)LinkDef.h
      LINKDEF      := $(AUTOLINKDEF)
      DICTHFILES   = $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*.hh) 
      LINKDEFIN    := $(wildcard $(INCLUDEDIR)/$(PKGNAME)/*LinkDef.$(HSuf).in)
    endif
  endif
endif

DICTCFILES   := $(if $(LINKDEF),$(BUILDDIR)/${PKGNAME}dict.$(CSuf),)
DICTOBJECTS  := $(if $(LINKDEF),$(BUILDDIR)/${PKGNAME}dict.o,)
ifndef MAKEEXE
  BASERMAP     := $(if $(LINKDEF),$(libdir)/lib${PKGNAME}.rootmap,)
endif
EXTRAOUTFILES := $(subst $(PKGROOT),$(prefix),$(EXTRAFILES))

BASESO	:= $(if $(MAKEEXE),,$(libdir)/lib$(PKGNAME).$(DllSuf))
BASEEXE	:= $(if $(MAKEEXE),$(bindir)/$(PKGNAME),)

# keep compartibility
EXOANALYSISROOT := $(top_builddir)
ALLDIRS      := $(BUILD_SUBDIRS)

EXOOUTDIR    := $(prefix)
LIBDIR       := $(libdir)
PUBLICDIR    := $(includedir)
BINDIR       := $(bindir)
PLUGINDIR    := $(prefix)/plugins
HTMLDOCDIR   := $(prefix)/htmldoc

LD          := $(CXX)
CC          := $(CXX)
EXODBUTIL   := EXODBUtilities

check::
	
