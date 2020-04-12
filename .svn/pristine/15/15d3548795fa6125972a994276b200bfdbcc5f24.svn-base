# This makefile is included by the makefiles in each package

ifeq ($(PKGROOT),)
  PKGROOT      := $(shell cd ../; pwd)
endif

.PHONY: all doall clean 

all: doall

include $(top_builddir)/make/common.mk

##############################################################################
# Secondaries
# The following files exist to enable Make to resolve a target for the various
# directories.  That is, by making these files, one ensures that the directory
# is built, and so Make can properly compute the dependency on the directory.

BUILDBEGIN  := $(BUILDDIR)/.buildbegin
WORKDIRINIT := $(BUILDDIR)/.builddir
LIBDIRINIT  := $(libdir)/.builddir
BINDIRINIT  := $(bindir)/.builddir
PUBLICPKGDIRINIT := $(includedir)/$(PKGNAME)/.builddir
.SECONDARY : $(WORKDIRINIT) $(LIBDIRINIT) $(BINDIRINIT) $(PUBLICPKGDIRINIT) 
.INTERMEDIATE: $(BUILDBEGIN)

# End secondaries
##############################################################################

##############################################################################
# Define general rules for Make

# Making dependencies
# This uses gcc to auto-make the dependencies and then adds the .o and .d file
# to the dependency rule. As well, to avoid ``No rule to make target ...''
# errors, the .d file is parsed again to add empty targets for each file.  This
# enables Make not to fail when a file no longer exists.  See, e.g.
# http://mad-scientist.net/make/autodep.html#norule
MAKEDEPEND = $(SILENT)$(START_TRAP); rm -f $@; \
	$(CC) $(CCFLAGS) -MM $(INCLUDE) $< > $@.$$$$;                                 \
        cp $@.$$$$ $@.temp.$$$$; sed -e 's/\#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
        -e '/^$$/ d' -e 's/$$/ :/' < $@.$$$$ >> $@.temp.$$$$;                         \
	sed 's,\($*\)\.o[ :]*,$(BUILDDIR)/\1.o $@ : ,g' < $@.temp.$$$$ > $@ ;          \
        rm -f $@*.$$$$;$(END_TRAP) 

# Make .o files
# This is the general command to make any .o file
MAKEOFILES = $(SILENT)$(START_TRAP); $(CC) $(CCFLAGS) -c $(INCLUDE) -o $@ $<; $(END_TRAP) 
MAKEOFFILES = $(SILENT)$(START_TRAP); $(FC) $(FFLAGS) -c $(INCLUDE) -o $@ $<; $(END_TRAP) 

# PRINT_START prints the message that the directory is being built.  This
# command checks if BUILDBEGIN is there and only prints if it isn't.  It makes
# the file if it didn't exist.  This allows printing to happen in the correct
# order even in parallel builds
PRINT_START = $(SILENT)if [ ! -d $(dir $(BUILDBEGIN)) ]; then mkdir -p $(dir $(BUILDBEGIN)); fi; \
              if [ ! -f $(BUILDBEGIN) ]; then touch $(BUILDBEGIN) &&                             \
              echo "Building....................... $(PKGNAME)"; fi

# START_TRAP and END_TRAP should bookend commands, allowing all errors in the
# commands to be caught, and properly cleaned up.  That is, if make is
# interrupted by a user, or a command exits with an error status, this ensures
# that the script will still cleanup after itself.  At the moment, it only
# removes the BUILDBEGIN file, but could have more advanced behavior in the
# future. 
START_TRAP = set -e; trap "$(ERASE_BUILDBEGIN); exit 1" INT TERM EXIT
END_TRAP = trap - INT TERM EXIT
ERASE_BUILDBEGIN = rm -f $(BUILDBEGIN)

##############################################################################


doall: init $(PUBLICFILES)               \
       $(BASESO) $(BASERMAP) $(BASEEXE)  \
       $(EXTRAOUTFILES)                  
	@$(ERASE_BUILDBEGIN)

init:: 
	$(PRINT_START)	

%.builddir: 
	$(SILENT)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(SILENT)touch $@

##############################################################################
# Dependencies

$(DEPCCFILES): $(BUILDDIR)/%.d : $(SOURCEDIR)/%.$(CCSuf) $(WORKDIRINIT) 
	$(PRINT_START)
	@echo "    Update dependencies........ $(<F)"
	$(MAKEDEPEND)

$(DEPCFILES): $(BUILDDIR)/%.d : $(SOURCEDIR)/%.$(CSuf) $(WORKDIRINIT) 
	$(PRINT_START)
	@echo "    Update dependencies........ $(<F)"
	$(MAKEDEPEND)

$(DEPFFILES): $(BUILDDIR)/%.d : $(SOURCEDIR)/%.$(FSuf) $(WORKDIRINIT) 
	$(PRINT_START)
	@echo "    Update dependencies........ $(<F)"
	$(MAKEDEPEND)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPCCFILES) $(DEPCFILES)
endif

# End dependencies
##############################################################################

##############################################################################
# Rootifying

$(DICTCFILES) : $(DICTHFILES) $(LINKDEF) $(WORKDIRINIT) 
	@echo "    Running rootcint........... $(notdir $(LINKDEF))"
	$(SILENT)$(START_TRAP); $(ROOTCINT) -f $@ -c $(INCLUDE) -p $(ROOTCINTDEFS) $(DICTHFILES) $(LINKDEF); $(END_TRAP)

$(AUTOLINKDEF): $(LINKDEFIN) $(DICTHFILES) $(WORKDIRINIT)
	@echo "    Auto-making Linkdef........ $(@F)"
	$(SILENT)$(START_TRAP);\
        $(PYTHON) $(top_builddir)/make/makeLinkDef.py \
        --output=$(AUTOLINKDEF)                           \
        --basebuild=$(INCLUDEDIR)                         \
        --input_linkdef=$(LINKDEFIN) $(DICTHFILES); $(END_TRAP) 

##############################################################################

##############################################################################
# Making object files

$(DICTOBJECTS): $(BUILDDIR)/%.o : $(BUILDDIR)/%.$(CSuf) $(WORKDIRINIT) 
	@echo "    Building dictionary........ $(<F)"
	$(MAKEOFILES)

$(BASECCO): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.$(CCSuf) $(WORKDIRINIT) 
	@echo "    Compile.................... $(<F)"
	$(MAKEOFILES)	

$(BASECO): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.$(CSuf) $(WORKDIRINIT) 
	@echo "    Compile.................... $(<F)"
	$(MAKEOFILES)	

$(BASEFO): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.$(FSuf) $(WORKDIRINIT) 
	@echo "    Compile.................... $(<F)"
	$(MAKEOFFILES)	

# End making object files
##############################################################################

##############################################################################
# Making so's, exes

$(BASESO): $(BASECCO) $(BASECO) $(BASEFO) $(DICTOBJECTS) $(EXTRAOBJECTS) $(LIBDIRINIT)
	@echo "    Build shareable............ $(@F)"
	$(SILENT)$(START_TRAP); $(LD) $(SOFLAGS)$(@F) -Wl,-rpath,$(ORIGINFLAGS) \
                 $(filter-out $(LIBDIRINIT),$^) $(LDFLAGS) $(OutPutOpt) $@; $(END_TRAP)
ifneq ($(DllSuf),so)
	$(SILENT)ln -s -f $(@F) $(basename $@).so  
endif

$(BASERMAP): $(BASESO) $(LINKDEF) $(LIBDIRINIT)
	@echo "    Building rootmap........... $(@F)"
	$(SILENT)$(START_TRAP); $(ROOTLIBMAP) -f -r $@ -l $(notdir $(BASESO)) \
          -d $(foreach alib,$(DEPENDLIB),lib$(alib).$(DllSuf)) \
          -c $(LINKDEF); touch $@; $(END_TRAP) 

$(BASEEXE): $(BASECCO) $(BASECO) $(DICTOBJECTS) $(BINDIRINIT)
	@echo "    Build executable........... $(@F)"
	$(SILENT)$(START_TRAP); $(LD) $(filter-out $(BINDIRINIT),$^) \
                 $(LDFLAGS) $(EXEFLAGS) -Wl,-rpath,$(ORIGINFLAGS) $(OutPutOpt) $@; $(END_TRAP)

# End making so's, exes
##############################################################################

##############################################################################
# Copy header, extra files 

$(includedir)/${PKGNAME}/%.$(HHSuf) : $(INCLUDEDIR)/${PKGNAME}/%.$(HHSuf) $(PUBLICPKGDIRINIT)
	@echo "    Copying header............. $(@F)"
	$(SILENT)cp $< $@

# 3/31/11 S.S.  Added flag "-r" to cp to work with Calibration macros
$(EXTRAOUTFILES) : $(prefix)/% : $(PKGROOT)/%
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)cp -r $< $@
	$(SILENT)if [ -d $@/.svn ]; then rm -Rf $@/.svn; fi

# End copy header, extra files 
##############################################################################

clean::
	@echo "Start clean of package ........... $(PKGNAME)"
	$(SILENT)rm -rf $(BUILDDIR)
	$(SILENT)rm -rf $(PUBLICFILES)
	$(SILENT)rm -rf $(EXTRAOUTFILES)
	$(SILENT)rm -f $(if $(MAKEEXE),$(BASEEXE),$(BASESO) $(BASERMAP))
	@echo "End   clean of package ........... $(PKGNAME)"
