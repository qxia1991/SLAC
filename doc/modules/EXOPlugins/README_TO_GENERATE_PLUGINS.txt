README for generating plugins, M. Marino Nov 2010, updated Apr 2011
Developed by V. Belov and M. Marino

A Plugin is a library that is loaded at run-time rather than something that is
linked at compile time.  By providing such a mechanism, it is possible to keep
the compilation and linking process for EXOAnalysis clean from all possible
user dependencies.

I. Creation of a plugin module.

The 'sandbox' directory has been provided to ease how a user creates plugins.
The creation of a plugin is generally exactly the same as the creation of any
other module, but a few easy steps must be followed:

  1. Copy your code (.hh and .cc files ) into the sandbox/src directory
  2. Ensure that in your header file, the following two lines are included
     at the bottom of your header:

#include "EXOUtilities/EXOPluginUtilities.hh"
EXO_DEFINE_PLUGIN_MODULE(EXOMGMPluginTwo, "test_two")

where the macro takes the name of your class and the desired nickname.  This
macro generates the necessary C-wrapping functions for loading your plugin at
run-time.  If you're interested, check out the definition of the macro in the
EXOUtilities/EXOPluginUtilities.hh file.

IMPORTANT: A plugin should NOT use the IMPLEMENT_EXO_ANALYSIS_MODULE macro.  It
should only use the DEFINE_EXO_ANALYSIS_MODULE and the DEFINE_EXO_PLUGIN_MODULE
macros.  

That is all that needs to be done.  For your convenience, two example plugins
have been included in the sandbox directory.  Note that if you already have a
module that you've made, making a plugin should be as simple as moving your
code into the sandbox directory and adding those two lines at the bottom of the
header.

II.  Building the plugin module.  

Again, this should be very simple.  If your plugin has external dependencies
open 

  >vim sandbox/make/Makefile 

and add your necessary include and linking flags for your plugin. (Instructions
are within that file.) Once this is done, you may build your plugin using:

  >cd sandbox/make/Makefile
  >make

A successful build should output as it last statement something along the lines
of:

  The following plugins have been built:
    /Users/mgmarino/Dropbox/software/EXO/prod/offline/sandbox/build/EXOMGMPluginTwo.dylib
    /Users/mgmarino/Dropbox/software/EXO/prod/offline/sandbox/build/EXOMGMTest.dylib

[Note, that on linux systems the extension of the module will be so, not
dylib.]

This tells you the directory and placement of your built libraries.  If you'd
like you can inspect the linking requirements of the libraries using 'ldd'
('otool -L' on Mac OS X).

III.  Including your plugin in an exo macro

Now that you have your plugin, including it in an exo macro is simple.  For
example, to include above plugins, the following must be added to the top of
your exo macro file:

  load  $EXOLIB/plugins/EXOMGMPluginTwo.so
  load  $EXOLIB/plugins/EXOMGMTest.so

or, equivalently: 
 
  load ${EXOLIB/plugins/*.so 

This MUST be at the top of your exo macro file.  The load command may also use
wildcards.  The rest of your macro file should be the same and 'use' commands
can be used to properly order compiled modules and plugins as necessary.  After
loading, you should see output like:

  Loading module from file: ../sandbox/build/EXOMGMTest.dylib
    New module available: EXOMGMTest (test)
  Loading module from file: ../sandbox/build/EXOMGMPluginTwo.dylib
    New module available: EXOMGMPluginTwo (test_two)

where the name in the parentheses is the nickname (to be used with the 'use'
command) which you have passed to the macro.

IV.  Including your plugin in a (py)ROOT script  

See documentation for EXOROOT::LoadPlugin.

V.  Testing

In the base directory, there is temporarily a test folder which includes a test
macro and a test input file.  In particular, running EXOAnalysis with
test_plugin.exo should allow
you to test a module. 

VI. Known issues and future development

Currently, the sandbox folder builds each pair of .cc and .hh files into its
own plugin.  Also, it is currently not possible to pass different include and
linking flags to each module.  Support for this will likely come in the future. 

