# Makefile for linking against ROOT and EXOAnalysis Simply type 'make' and then
# ./myapp to run your application.  'make clean' cleans everything in the
# directory. You can, of course, change the names as necessary. 
# M. Marino 26 April 2011 

TARGETS = myapp # Name of application 
SOURCES := $(wildcard *.cc) #add all cc files in directory to your compile list 
OBJS    := $(SOURCES:.cc=.o) 

TARGETOBJ = $(patsubst %, %.o, $(TARGETS))
CXX       := g++
CXXFLAGS  := -Wall -g #-O2 # Uncomment the O2 to apply optimization 
LIBS      :=  

# The CXXFLAGS and the LIBS depend on exo-config, a utility similar to
# root-config for EXOAnalysis, and so you must have $EXOLIB/bin in your path
CXXFLAGS += $(shell $(EXOLIB)/bin/exo-config --cflags)
LIBS += $(shell $(EXOLIB)/bin/exo-config --libs)

all: $(TARGETS)

$(TARGETS): $(TARGETOBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ 

clean:
	@rm -f $(TARGETS)
	@rm -f *.o
