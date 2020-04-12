#                               -*- Autoconf -*-
# EXOAnalysis configure.ac
#
# Initial version written by M. Marino Aug 2011.
# Rewritten by V. Belov Sep 2011.
# Generates the configure script in use during the configuration phase of the
# build for EXOAnalysis. 

#########################################################################
# Init stage
#########################################################################
AC_PREREQ(2.62)
AC_INIT([EXOAnalysis], [1.0])
dnl AC_COPYRIGHT()
dnl AC_REVISION([$ Revision: 0.1 $])
AC_CONFIG_SRCDIR([../utilities/misc/EXOUtilities/EXOEventData.hh])
AC_CONFIG_SRCDIR([../analysis/main/src/EXOAnalysis.cc])
dnl AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
dnl AM_INIT_AUTOMAKE([foreign -Wall])
# Output directory
AS_IF([test x"$EXOOUT" = x],[EXOOUT=/tmp/${USER}/exoout])
AS_IF([test x"${prefix}" = xNONE -a x"$EXOOUT" != x],[prefix=${EXOOUT}])
AC_ARG_VAR(EXOOUT, [Installation directory of EXOAnalysis, use --prefix= instead])
AC_MSG_NOTICE([Configuring to output to: ${prefix}])
# List of source directories will be after all checks
EXOANALYSISROOT=`cd $srcdir/..;pwd`
AC_ARG_VAR(EXEFLAGS,[Flags to pass to the linker when building an executable])

# works for me, even in screen on Darwin
AS_IF([test x"$PS1" != x],[BOLD=`tput smso`;OFFBOLD=`tput rmso`;])dnl make life brighter

#########################################################################
# Check base system
#########################################################################
AC_CANONICAL_HOST
AC_PROG_MAKE_SET
AC_PROG_EGREP
AC_PROG_AWK
AC_PROG_SED
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_SUBST(INSTALL)

AC_ARG_ENABLE(static-external-libs,
              AC_HELP_STRING([--enable-static-external-libs],
                             [use static external libraries]),
              [staticextlibs=$enableval],
              [staticextlibs=no])

AC_MSG_CHECKING([whether to link using static external libraries])
AC_MSG_RESULT($staticextlibs)

STATIC=no
AS_IF([ test x"$staticextlibs" = xyes ],
      [ STATIC=extlibs 
        AC_DEFINE(STATIC)
        AS_IF([test x"$EXEFLAGS" = x],[EXEFLAGS="-rdynamic"])])
AC_SUBST(STATIC)

AC_ARG_ENABLE(link-xrootd-direct,
              AC_HELP_STRING([--enable-link-xrootd-direct],
                             [when statically building, directly link xrootd]),
              [linkxrootddirect=$enableval],
              [linkxrootddirect=no])

AC_ARG_ENABLE(cxx11,
              AC_HELP_STRING([--enable-cxx11],
                             [enable compiling with the c++11 standard]),
              [CXXFLAGS="$CXXFLAGS -std=c++11 -stdlib=libc++"],
              [linkxrootddirect=no])


AC_MSG_CHECKING([whether to link xrootd directly])
AC_MSG_RESULT($linkxrootddirect)

LINKXROOTD=""
AS_IF([ test x"$linkxrootddirect" = xyes ],
      [ LINKXROOTD="-lNetx -lXrdClient" 
        AC_DEFINE(LINKXROOTD)
        ])


#########################################################################
# Check the compiler
#########################################################################
AC_PROG_CC
dnl AC_PROG_CC_C_O
dnl AC_PROG_CPP
AC_PROG_CXX
dnl AC_PROG_CXX_C_O
AC_PROG_CXXCPP
AC_LANG(C++)
AC_PROG_FC
AC_PROG_LIBTOOL
AC_SUBST(LD)
AS_IF([test x"${build_cpu}" = "xx86_64"],[
  case "${host_cpu}" in
    i?86)
      CFLAGS="$CFLAGS -m32"
      CXXFLAGS="$CXXFLAGS -m32"
      FCFLAGS="$FCFLAGS -m32"
#      F77FLAGS="$F77FLAGS -m32"
      LDFLAGS="$LDFLAGS -m32"
      AC_MSG_NOTICE([Cross compiling on an x86_64 for i386... using -m32 flag])
      # Strictly speaking one needs to check that compiler used is not a 
      #   cross-compiler, but it's so rare in _this_ case
  esac])
AC_FC_LIBRARY_LDFLAGS
AC_SUBST(FCLIBS)

# check x86_64
case "${host_cpu}" in
    x86_64*)
        CXXFLAGS="${CXXFLAGS} -fPIC"
        AC_MSG_NOTICE([Compiling on an x86_64 chipset... setting -fPIC flag.])
  # only on amd64 should one compile static libs with this flag
  # Other tested architectures currently compile PIC libraries
  # and so it is no problem to link a dynamic library using the PIC
  # libs
esac

#########################################################################
#Find python, this is necessary for some of the build 
#########################################################################
m4_include(python.m4)
AM_PATH_PYTHON([2.5],
               [HAVE_PYTHON=yes
                PYTHONLIBDIR=`$PYTHON -c 'from distutils.sysconfig import get_python_lib; print "%s/config" % get_python_lib(0,1)'`],
               [HAVE_PYTHON=no
                AC_MSG_ERROR([python not found, required for build])])
AC_SUBST(HAVE_PYTHON)
AC_SUBST(PYTHONLIBDIR)

# Can't we evade this dependency?
AC_PATH_PROG(SYSENV, env, no)
AS_IF([test x"$SYSENV" = xno], 
      [AC_MSG_ERROR([env not found, required for build])])
AC_SUBST(SYSENV)


#########################################################################
# Check standart compilation
#########################################################################
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM
# Check if we have the error attribute that one can apply to functions.  This
# attribute allows an error to be emitted at compile time if a function is
# used.
AC_MSG_CHECKING(whether compiler accepts 'error' attribute)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM('void a() __attribute__((error("a")));',[])], 
                  [AC_MSG_RESULT(yes)
                   AC_DEFINE(HAVE_ERROR_ATTRIBUTE,1,[Define to 1 if compiler allows 'error' attribute.])], 
                  [AC_MSG_RESULT(no)])
AC_CHECK_HEADER([tr1/type_traits],[AC_DEFINE(HAVE_TYPE_TRAITS,1,[Define to 1 if you have the <type_traits> header file.])])

# Checks for library functions.
AC_FUNC_MKTIME
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([floor localtime_r memset pow select sqrt strchr strstr strtol])

# Checks for libraries.

#########################################################################
# Check extra dependencies
#########################################################################
#########################################################################
# Looking for ROOT
#########################################################################
AC_MSG_NOTICE([${BOLD}Checking for external dependencies: ${OFFBOLD}])
m4_include(root.m4)
ROOT_PATH([5.28], 
          [HAVE_ROOT=yes
           AC_DEFINE(HAVE_ROOT,1,[Define to 1 if you have ROOT installed.])], 
          [HAVE_ROOT=no
           AC_MSG_ERROR([ROOT not found, please install])])
AC_SUBST(HAVE_ROOT)
AC_PATH_PROG(ROOTLIBMAP, rlibmap , no, `$ROOT_CONFIG --bindir`)

AC_SUBST(ROOT_INCLUDE,"-I$ROOTINCDIR")
AC_SUBST(ROOT_LIBDIR,"$ROOTLIBDIR")

AS_IF([test x"$STATIC" = xextlibs],
  [
    ROOT_FEATURE([builtin_freetype],[], [AC_MSG_ERROR([static build requires builtin libs])])
    ROOT_FEATURE([builtin_pcre],[], [AC_MSG_ERROR([static build requires builtin libs])])
    ROOT_FEATURE([builtin_zlib],[], [AC_MSG_ERROR([static build requires builtin libs])])
    ROOT_FEATURE([editline],[AC_MSG_ERROR([static build has problems with editline, remove from build.])])
    lzma_LIBS=""
    ROOT_PATH([5.34],[lzma_LIBS="-llzma"]) 
    save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -L$ROOTLIBDIR -lRoot $lzma_LIBS $LINKXROOTD -lfreetype -lpcre $ROOTAUXLIBS"
    AC_LINK_IFELSE([extern int gErrorIgnoreLevel; int main() { return 1; }], [], 
      [ AC_MSG_ERROR([ROOT not statically compiled correctly.  Recommended to 'buildin' libraries.])])
    LDFLAGS="$save_LDFLAGS"
    AC_SUBST(ROOTLIBS,["-lRoot $lzma_LIBS $LINKXROOTD -lfreetype -lpcre $ROOTAUXLIBS"])
  ],
  [ROOT_FEATURE([roofit],
     [AC_DEFINE(HAVE_ROOT_ROOFIT,1,[Define to 1 if ROOT is build with roofit.])
      AC_SUBST(ROOFIT_LIBS,["RooFit RooFitCore Minuit Foam"])
     ], AC_SUBST(ROOFIT_LIBS,[]))
   ROOT_FEATURE([mathmore],
     [AC_DEFINE(HAVE_ROOT_MATHMORE,1,[Define to 1 if ROOT is built with mathmore])
      AC_SUBST(MATHMORE_LIBS,[MathMore])
     ], AC_SUBST(MATHMORE_LIBS,[]))
   ROOT_FEATURE([fftw3],
   [ HAVE_ROOT_FFTW3=yes
     AC_DEFINE(HAVE_ROOT_FFTW3,1,[Define to 1 if ROOT is build with fftw3.]) ])
   ROOT_FEATURE([xml],[AC_DEFINE(HAVE_ROOT_XML,1,[Define to 1 if ROOT is build with XML.]) ])
   ROOT_FEATURE([tmva],[AC_DEFINE(HAVE_ROOT_TMVA,1,[Define to 1 if ROOT is build with TMVA.])])
   ROOT_FEATURE([python],
             [HAVE_ROOT_PYTHON=yes],
             [HAVE_ROOT_PYTHON=no
              AC_MSG_NOTICE([No pyROOT found, disabling documentation built.])])
   AC_SUBST(ROOTLIBS,["-lCore -lCint"])
  ])
AC_SUBST(HAVE_ROOT_PYTHON)
AC_MSG_NOTICE([${BOLD}ROOT found and properly configured${OFFBOLD}])

#########################################################################
# Looking for FFTW3
#########################################################################
# We use fftw3 directly, check for it.
AC_ARG_WITH(fftw,
            [AC_HELP_STRING([--without-fftw],[disable FFTW3 support])],
            [AS_IF([test x"$withval" != xno],[with_fftw=yes],[with_fftw=no])], [with_fftw=yes])
AC_ARG_WITH([fftw-prefix],
            [AC_HELP_STRING([--with-fftw-prefix],[prefix where FFTW3 is installed (optional)])],
    	    [fftw_prefix="$withval"],[fftw_prefix=""])

FFTW_INCLUDE=""
FFTW_LIBS=""
FFTW_LDFLAGS=""
AS_IF([test x"$with_fftw" = xyes], 
 [
    # First check to see if symbols are in the ROOT libraries, this can happen
    AS_IF([test x"$HAVE_ROOT_FFTW3" = xyes],
           [
             save_LIBS="$LIBS"
             save_LDFLAGS="$LDFLAGS"
             LDFLAGS="$LDFLAGS -L$ROOTLIBDIR"
             AC_MSG_NOTICE([Checking if ROOT FFTW library contains the correct symbols])
             AC_SEARCH_LIBS([fftw_destroy_plan],[FFTW], 
               AC_SEARCH_LIBS([fftw_plan_dft_r2c],[FFTW],
                 AC_SEARCH_LIBS([fftw_plan_dft_c2r],[FFTW],
                   AC_SEARCH_LIBS([fftw_execute],[FFTW], 
                     [HAVE_FFTW=yes
                      USE_ROOT_FFTW=yes
                      FFTW_LIBS="-lFFTW"
                      AC_MSG_NOTICE([Using ROOT's FFTW library])
                      ], [], [-lCore -lCint -ldl])
                   , [], [-lCore -lCint -ldl])
                 , [], [-lCore -lCint -ldl])
               , [], [-lCore -lCint -ldl])
             LIBS="$save_LIBS"
             LDFLAGS="$save_LDFLAGS"
           ] 
         ) 
    
    AS_IF([test x"$USE_ROOT_FFTW" = x],
      [
      AC_MSG_NOTICE([Can't use ROOT FFTW library, trying others.])
      save_CPPFLAGS="$CPPFLAGS"
      save_LIBS="$LIBS"
      save_LDFLAGS="$LDFLAGS"
      AS_IF([test x"$fftw_prefix" != x],
            [FFTW_INCLUDE="-I$fftw_prefix/include"
             FFTW_LDFLAGS="-L$fftw_prefix/lib"])
     
      AS_IF([test x"$STATIC" = xextlibs], 
        [ FFTW_LIBS="-Wl,-static -lfftw3 -Wl,-Bdynamic" ],
        [ FFTW_LIBS="-lfftw3" ])
      CPPFLAGS="$CPPFLAGS $FFTW_INCLUDE" 
      LDFLAGS="$LDFLAGS $FFTW_LDFLAGS" 
      LIBS="$EXEFLAGS $LIBS $FFTW_LIBS"
      AC_CHECK_HEADER([fftw3.h],[HAVE_FFTW=yes],[HAVE_FFTW=no])
      AS_IF([test x"$HAVE_FFTW" = xyes],
        [ 
        AC_MSG_CHECKING([if a program with fftw runs])
        AC_LINK_IFELSE(
      	[AC_LANG_PROGRAM(
            [#include "fftw3.h"],
            [fftw_complex temp;])],
      	[AC_MSG_RESULT(yes)],
      	[AC_MSG_RESULT(no)
      	 HAVE_FFTW=no])
        CPPFLAGS=$save_CPPFLAGS
        LDFLAGS=$save_LDFLAGS
        LIBS=$save_LIBS],
      [
        CPPFLAGS=$save_CPPFLAGS
        LDFLAGS=$save_LDFLAGS
        LIBS=$save_LIBS
        FFTW_LIBS=""
      ])
    ])
 ], [HAVE_FFTW=no])
AS_IF([test x"$HAVE_FFTW" != xno],
  [AC_DEFINE(HAVE_FFTW,1,[Define to 1 if you have FFTW3 installed.])])
AS_IF([test x"$USE_ROOT_FFTW" = xyes],
  [AC_DEFINE(USE_ROOT_FFTW,1,[Define to 1 if you will use the ROOT FFTW installation.])])
AC_SUBST(FFTW_INCLUDE)
AC_SUBST(FFTW_LIBS)
AC_SUBST(FFTW_LDFLAGS)


#########################################################################
# Looking for MYSQL
#########################################################################
  AC_ARG_WITH(mysql,
    [AC_HELP_STRING([--without-mysql],[disable MySQL support])],
    [AS_IF([test x"$withval" != xno],[with_mysql=yes],[with_mysql=no])], [with_mysql=yes])
  AC_ARG_WITH([mysql-prefix],
              [AC_HELP_STRING([--with-mysql-prefix],[prefix where MySQL is installed (optional)])],
    	      [mysql_prefix="$withval"],[mysql_prefix=""])

AS_IF([test x"$with_mysql" = xyes], 
 [if test "x${MYSQL_CONFIG+set}" != xset ; then
    if test x"$mysql_prefix" != x ; then
      MYSQL_CONFIG="$mysql_prefix/bin/mysql_config"
    fi
  fi
  AC_PATH_PROG(MYSQL_CONFIG, mysql_config , no)
dnl  min_mysql_version= ifelse([$1], ,3.0.0,$1)
  min_mysql_version=5.0.0
  if test x"$MYSQL_CONFIG" = xno ; then
    no_mysql=yes
  else
    AC_MSG_CHECKING(whether MySQL version >= [$min_mysql_version])
    MYSQL_VERSION=`$MYSQL_CONFIG --version`
    vers=`echo $MYSQL_VERSION | $SED 's/[[-a-z]].*//; s/.*/.&./; s/\.\([[0-9]]\)\./.0\1./g; s/\.\([[0-9]]\)\./.0\1./g; s/\.//g'`
    requ=patsubst(patsubst(patsubst(.[5.0.0]., [\.\([0-9]\)\.], [.0\1.]), [\.\([0-9]\)\.], [.0\1.]), [\.], [])
    if test 0"$vers" -lt 0"$requ" ; then
        AC_MSG_RESULT(no)
	no_mysql=yes
    else 
        AC_MSG_RESULT(yes)
    fi
    if test x"$no_mysql" != xyes ; then
      MYSQL_INCLUDE=`$MYSQL_CONFIG --include`
      MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
      MYSQL_LIBS=`$MYSQL_CONFIG --libs | $SED 's/-L\([[^ ]]*\)/=L\1/g; s/-l\(mysql[[^ ]]*\)/=l\1/g; s/-[[^ ]]*//g; s/=/-/g'`
      save_CPPFLAGS=$CPPFLAGS
      save_LIBS=$LIBS
      CPPFLAGS="$CPPFLAGS $MYSQL_INCLUDE"
      LIBS="$LIBS $MYSQL_LIBS"
      AC_MSG_CHECKING([for mysqlclient library usability])
      AC_LINK_IFELSE(
    	[AC_LANG_PROGRAM(
          [#include <mysql.h>],
          [mysql_get_client_version();])],
    	[AC_MSG_RESULT(yes)],
    	[AC_MSG_RESULT(no)
    	 no_mysql=yes])
      AC_MSG_CHECKING([if a program with mysql runs])
      AC_RUN_IFELSE(
        [AC_LANG_PROGRAM(
          [#include <mysql.h>],
          [MYSQL amysql_object;])],
        [AC_MSG_RESULT([yes])], 
        [AC_MSG_RESULT([no])
         no_mysql=yes
         AC_MSG_WARN([Cannot run as linked against mysql.])])
      CPPFLAGS=$save_CPPFLAGS
      LIBS=$save_LIBS
    fi
  fi], [no_mysql=yes])
AS_IF([test x"$no_mysql" != xyes],
 [HAVE_MYSQL=yes
  AC_DEFINE(HAVE_MYSQL,1,[Define to 1 if you have MySQL installed.])
  AC_MSG_NOTICE([${BOLD}MySQL found. ${OFFBOLD}])],
 [HAVE_MYSQL=no
  AC_MSG_NOTICE([${BOLD}MySQL not found. ${OFFBOLD}])])
AC_SUBST(HAVE_MYSQL)
AC_SUBST(MYSQL_INCLUDE)
AC_SUBST(MYSQL_LIBS)
AC_SUBST(MYSQL_VERSION)

#########################################################################
# Should we use multiple threads?
# Note: we don't help the user out much, they should have a proper setup.
# We assume boost can be found and used, and that it should always be linked statically.
# Anyone who wants to make this configuration more generic is welcome!!
#########################################################################
AC_ARG_WITH(threads,
            [AC_HELP_STRING([--with-threads],[Use multiple threads (experts only!)])],
            [AS_IF([test x"$withval" != xno],[with_threads=yes],[with_threads=no])], [with_threads=no])

AS_IF([test x"$with_threads" = xyes],
      [AC_MSG_NOTICE([YOU'VE SELECTED A THREADED BUILD!  You'd better know what you're doing...])
       AS_IF([test x"$USE_ROOT_FFTW" = xyes],
             [AC_MSG_WARN([Threaded builds shouldn't use ROOT's FFT library -- it doesn't expose all of the functions we need])])
       AC_DEFINE(USE_THREADS,1,[Define to 1 if you are attempting to use threads.])
       USE_THREADS=yes
       BOOST_INCLUDE="-I$BOOST_DIR/include"
       BOOST_LIBS="-L$BOOST_LIB -Wl,-static -lboost_thread -lboost_system -Wl,-Bdynamic -lrt"],
      [USE_THREADS=no])

AC_SUBST(USE_THREADS)
AC_SUBST(BOOST_INCLUDE)
AC_SUBST(BOOST_LIBS)

#########################################################################
# Looking for JNI
#########################################################################
  AC_ARG_WITH(jni,
    [AC_HELP_STRING([--with-jni],[enable Java Native Interface support])],
    [AS_IF([test x"$withval" != xno],[with_jni=yes],[with_jni=no])], [with_jni=no])
  AC_ARG_WITH([java-prefix],
              [AC_HELP_STRING([--with-java-prefix],[prefix where Java is installed (optional)])],
    	      [java_prefix="$withval"],[java_prefix=""])
  AC_ARG_VAR([JAVA_HOME], [Java Runtime Environment (JRE) location])

AS_IF([test x"$with_jni" = xyes], 
 [AS_IF([test x"$java_prefix" = x],[java_prefix="$JAVA_HOME"])
  save_CPPFLAGS=$CPPFLAGS
  AS_IF([test x"$java_prefix" != x],
   [JAVA_INCLUDE="-I${java_prefix}/include"
    case "$host_os" in
      linux*) JAVA_INCLUDE="$JAVA_INCLUDE -I${java_prefix}/include/linux" ;;
    esac
    CPPFLAGS="$CPPFLAGS $JAVA_INCLUDE"
    AC_CHECK_HEADER([jni.h],
      [HAVE_JNI=yes
       AC_MSG_NOTICE([${BOLD}JNI found. ${OFFBOLD}])],
      [HAVE_JNI=no])
    AS_IF([test x"$HAVE_JNI" = xno], AC_MSG_FAILURE([Java native interface required for Java]))
    AS_IF([test -z "$JAVA_HOME"],
          [AC_MSG_WARN([JAVA_HOME has not been set. JAVA_HOME must be set at run time to locate libjvm.])])
    CPPFLAGS=$save_CPPFLAGS
  ])])
AC_SUBST(HAVE_JNI)
AC_SUBST(JAVA_INCLUDE)

#########################################################################
# Look for CLHEP
#########################################################################
m4_include(clhep.m4)
CLHEP_PATH([2.0.4.5],
 [save_CPPFLAGS=$CPPFLAGS
  save_LIBS=$LIBS

  AS_IF([test x"$STATIC" = xextlibs], 
    [ CLHEP_LIBS="-Wl,-static $CLHEP_LIBS -Wl,-Bdynamic" ])
  CPPFLAGS="$CPPFLAGS $CLHEP_INCLUDE"
  LIBS="$LIBS $CLHEP_LIBS"
  AC_MSG_CHECKING([for CLHEP library usability])
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [#include <CLHEP/GenericFunctions/Abs.hh>],
      [Genfun::Abs a;])],
    [AC_MSG_RESULT(yes)],
    [AC_MSG_RESULT(no)
	 no_clhep=yes])
  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
  ])
AS_IF([test x"$no_clhep" != xyes],
 [HAVE_CLHEP=yes
  AC_DEFINE(HAVE_CLHEP,1,[Define to 1 if you have CLHEP installed.])
  AC_MSG_NOTICE([${BOLD}CLHEP found. ${OFFBOLD}])],
 [HAVE_CLHEP=no
  AC_MSG_NOTICE([${BOLD}CLHEP not found. ${OFFBOLD}])])
AC_SUBST(HAVE_CLHEP)

  AC_ARG_WITH(geant4,
    [AC_HELP_STRING([--without-geant4],[disable Geant4 support])],
    [AS_IF([test x"$withval" != xno],[with_geant4=yes],[with_geant4=no])], [with_geant4=yes])
#########################################################################
# Look for Geant4 
#########################################################################
m4_include(geant4.m4)
AS_IF([test x"$with_geant4" = xyes],
 [GEANT4_PATH([9.3.2],
   [AS_IF([test x"$STATIC" = xextlibs], 
     [ GEANT4_LIBS="-Wl,-static $GEANT4_LIBS -Wl,-Bdynamic" ])
    g4clhep=`$ECHO $GEANT4_LIBS | $AWK 'BEGIN{r="no";}{for(i=1;i<NF;i++)if($i=="-lG4clhep")r="yes";}END{print r;}'`
    AS_IF([test x"$g4clhep" = xyes],
     [# Found built-in clhep
      HAVE_GEANT4_CLHEP=yes
      AS_IF([test x"$HAVE_CLHEP" = xyes],
       [AC_MSG_WARN([Found external CLHEP, but GEANT is built with private one.])])],
     [# Using external clhep
      HAVE_GEANT4_CLHEP=no
      AS_IF([test x"$HAVE_CLHEP" != xyes],[AC_MSG_FAILURE([Can't find CLHEP library for GEANT.])])])
#
         AC_MSG_CHECKING([if G4UIterminal and G4UItcsh exist.])
         save_CPPFLAGS=$CPPFLAGS
         save_LIBS=$LIBS
         save_LDFLAGS="$LDFLAGS"

         CPPFLAGS="$CPPFLAGS $GEANT4_INCLUDE"
         AS_VERSION_COMPARE($geant4_version,9.6.0,[g4_many_lib='yes'])
         AS_IF([test x"$g4_many_lib" = xyes],
          [LIBS="$LIBS -lG4UIbasic -lG4UIcommon -lG4intercoms -lG4globman"],
          [LIBS="$LIBS -lG4interfaces -lG4intercoms -lG4global -lXt"])
         LDFLAGS="$LDFLAGS $GEANT4_LDFLAGS"
         AS_IF([test x"$HAVE_GEANT4_CLHEP" != xyes],
          [CPPFLAGS="$CPPFLAGS $CLHEP_INCLUDE"
           LIBS="$LIBS $CLHEP_LIBS"
           LDFLAGS="$LDFLAGS $CLHEP_LDFLAGS"],
          [LIBS="$LIBS -lG4clhep"])

         AC_LINK_IFELSE(
       	 [AC_LANG_PROGRAM(
              [#include "G4UIterminal.hh"]
              [#include "G4UItcsh.hh"]
              [G4UIterminal session(new G4UItcsh);])],
       	 [AC_MSG_RESULT(yes)
          AC_DEFINE(G4_HAS_VIS)],
       	 [AC_MSG_RESULT(no)])
         CPPFLAGS="$save_CPPFLAGS"
         LIBS="$save_LIBS"
         LDFLAGS="$save_LDFLAGS"
#
    ],
   [no_geant=yes])],
 [no_geant=yes])

AS_IF([test x"$no_geant" != xyes],
 [HAVE_GEANT4=yes
  AC_DEFINE(HAVE_GEANT4,1,[Define to 1 if you have Geant4 installed.])
  AC_MSG_NOTICE([${BOLD}Geant4 found. ${OFFBOLD}])],
 [HAVE_GEANT4=no
  AC_MSG_NOTICE([${BOLD}Geant4 NOT found. ${OFFBOLD}])])

AC_SUBST(HAVE_GEANT4)
AC_SUBST(HAVE_GEANT4_CLHEP)

# Look for EXOBinary
AC_ARG_VAR([LIBS],[libraries to pass to the linker, e.g. -l<library>])
AC_ARG_WITH(exobinary,[AC_HELP_STRING([--without-exobinary],[disable use of EXOBinary])],
    [AS_IF([test x"$withval" != xno],[with_exobin=yes],[with_exobin=no])], [with_exobin=yes])
AC_ARG_WITH(exobinary-prefix,[AC_HELP_STRING([--with-exobinary-prefix],[prefix where EXOBinary is installed (optional)])],
    [exobin_prefix="$withval"],[exobin_prefix=""])

AS_IF([test x"$with_exobin" = xyes],
 [AS_IF([test x"$exobin_prefix" = x],[exobin_prefix="$EXOOUT"])
  EXOBIN_INCLUDE="-I${exobin_prefix}/include"
  EXOBIN_LIBS="-L${exobin_prefix}/lib -lEXOBinaryPackage"
  save_CPPFLAGS="$CPPFLAGS"
  save_LIBS="$LIBS"
  CPPFLAGS="$CPPFLAGS $EXOBIN_INCLUDE"
  LIBS="$EXEFLAGS $LIBS $EXOBIN_LIBS"
  AC_CHECK_HEADER([EXOBinaryPackage/File/Reader.hh],[HAVE_EXOBIN=yes],[HAVE_EXOBIN=no])
  AC_MSG_CHECKING(for EXOBinaryPackage library usability)
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [#include <EXOBinaryPackage/File/Reader.hh>],
      [Exo::File::Reader rd;])],
    [AC_MSG_RESULT(yes)],
    [AC_MSG_RESULT(no)
     HAVE_EXOBIN=no])
  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
])
AC_SUBST(HAVE_EXOBIN)
AC_SUBST(EXOBIN_INCLUDE)
AC_SUBST(EXOBIN_LIBS)
AS_IF([test x"$HAVE_EXOBIN" = xyes],
  [AC_DEFINE([HAVE_EXOBIN],1,[Define to 1 if you have EXOBinary package installed.])])

#########################################################################
# Looking for CURL
#########################################################################
AC_ARG_ENABLE(curl, 
  AC_HELP_STRING([--disable-curl],[disable curl]),
  [AS_IF([test x"$enableval" != xno],[BUILD_CURL=yes],[BUILD_CURL=no])], [BUILD_CURL=yes])
AS_IF([test x"$BUILD_CURL" = xyes],
  [
    AC_CHECK_HEADERS([curl/curl.h])
    AC_CHECK_LIB(curl, curl_easy_init, [CURL_LIB=-lcurl])
    AC_SUBST(CURL_LIB)
  ], [AC_MSG_NOTICE(CURL disabled)])

#########################################################################
# Other user options
#########################################################################
AC_ARG_ENABLE(debug, 
  AC_HELP_STRING([--enable-debug],[enable debug flags]),
  [AS_IF([test x"$enableval" != xno],[BUILD_DEBUG=yes],[BUILD_DEBUG=no])], [BUILD_DEBUG=no])

AC_MSG_CHECKING(for debug build)
AS_IF([test x"$BUILD_DEBUG" = xyes],
  [AC_MSG_RESULT([yes])
   CXXFLAGS="$CXXFLAGS -g"
   AC_DEFINE([HAVE_DEBUG],1,[Define to 1 if you want to build with debug information.])],
  [AC_MSG_RESULT([no])])
AC_SUBST(BUILD_DEBUG)

AC_ARG_ENABLE(doc, 
  AC_HELP_STRING([--enable-doc],[enable build of documentaion]),
  [AS_IF([test x"$enableval" != xno],[BUILD_DOC=yes],[BUILD_DOC=no])], [BUILD_DOC=no])
AC_SUBST(BUILD_DOC)

AC_ARG_ENABLE(plugins, 
  AC_HELP_STRING([--disable-plugins],[disable build of plugins]),
  [AS_IF([test x"$enableval" != xno],[BUILD_PLUGINS=yes],[BUILD_PLUGINS=no])], [BUILD_PLUGINS=yes])
AC_SUBST(BUILD_PLUGINS)

AC_ARG_ENABLE(display, 
  AC_HELP_STRING([--disable-display],[disable build of EXOEventDisplay]),
  [AS_IF([test x"$enableval" != xno],[BUILD_DISPLAY=yes],[BUILD_DISPLAY=no])], [BUILD_DISPLAY=yes])
AC_SUBST(BUILD_DISPLAY)

AC_ARG_ENABLE(viewer3d, 
  AC_HELP_STRING([--disable-viewer3d],[disable build of EXO3DViewer]),
  [AS_IF([test x"$enableval" != xno],[BUILD_VIEWER3D=yes],[BUILD_VIEWER3D=no])], [BUILD_VIEWER3D=yes])
AC_SUBST(BUILD_VIEWER3D)
AS_IF([test x"$BUILD_VIEWER3D" == xyes],[AC_DEFINE(BUILD_VIEWER3D)])

AC_ARG_ENABLE(comptimg, 
  AC_HELP_STRING([--enable-comptimg],[enable build of AComptonImaging]),
  [AS_IF([test x"$enableval" != xno],[BUILD_COMPTIMG=yes],[BUILD_COMPTIMG=no])], [BUILD_COMPTIMG=no])
AC_SUBST(BUILD_COMPTIMG)

#########################################################################
# Check pointer size 
#########################################################################
AC_CHECK_SIZEOF([void *])

#########################################################################
# Make build flags
#########################################################################
# handle dynamic libraries
case "$host_os" in
  darwin*)
#    EXTRALIBFLAGS="-bind_at_load" 
#    SOFLAGS="-dynamiclib"
    SHREXT=dylib
    SOFLAGS="-dynamiclib -install_name @rpath/"
    ORIGINFLAGS="@loader_path/../lib"
    LIBRARY_SEARCH_PATH=DYLD_LIBRARY_PATH
  ;;
  linux*)
    SOFLAGS="-shared -Wl,-soname,"
    SHREXT=so
    LDFLAGS="$LDFLAGS -Wl,-rpath-link,\$(prefix)/lib"
    AS_IF([test x"$STATIC" = xno], [LDFLAGS="$LDFLAGS -Wl,-z,defs"])
    ORIGINFLAGS=\'\$\$ORIGIN/../lib\',--enable-new-dtags
    LIBRARY_SEARCH_PATH=LD_LIBRARY_PATH
  ;;
  *)
  AC_MSG_ERROR([Don't know how to build dynamic libraries on your system])
  ;;
esac
AC_SUBST(SOFLAGS)
AC_SUBST(SHREXT)
AC_SUBST(ORIGINFLAGS)
AC_SUBST(LIBRARY_SEARCH_PATH)

# Make build ID
THEDATE=`date`
WHOAMI=`whoami`
SVNDEF="?"
BUILDDEF="Built $THEDATE by $WHOAMI using svn id $SVNDEF"
AC_PATH_PROG(SVN_VERSION, svnversion, no)
BUILD_TAG=${BUILD_TAG-}
AS_IF([test x"$SVN_VERSION" != no],
      [
        AC_MSG_CHECKING(for source code revision)
        SVNDEF=`$SVN_VERSION -cn $EXOANALYSISROOT | $ac_cv_prog_AWK -F: '{print $NF}'`
        AC_MSG_RESULT($SVNDEF)
        BUILDDEF="Built $THEDATE by $WHOAMI using svn id $SVNDEF $BUILD_TAG"
      ])
AC_DEFINE_UNQUOTED(SVN_REV,["$SVNDEF"],?)
AC_DEFINE_UNQUOTED(BUILD_ID,["$BUILDDEF"],?)
AC_SUBST(SVN_REV,["$SVNDEF"])
AC_SUBST(BUILD_ID,["$BUILDDEF"])

AS_IF([test x"$HAVE_MYSQL" = xyes],
 [EXODBDIR=utilities/database
  EXODBUTIL=EXODBUtilities],
 [EXODBDIR=""
  EXODBUTIL=""])
#AC_SUBST(EXODBDIR)
AC_SUBST(EXODBUTIL)
#AC_SUBST(EXOG4DIR,geant/EXOsim)
#AC_SUBST(EXOG4MODDIR,geant/exosim_module)
#AC_SUBST(EXOSIMLIB,EXOSim)
#AC_SUBST(EXOJNI,analysis/jni)

AC_SUBST(EXOOUT,$prefix)
AS_IF([test x"$as_echo" = x],
 [AC_SUBST(AX_ECHO,[echo])
  AC_SUBST(AX_ECHO_N,[echo $ECHO_N " $ECHO_C"])],
 [AC_SUBST(AX_ECHO,$as_echo)
  AC_SUBST(AX_ECHO_N,$as_echo_n)])
#---------------------------------------
# Get directories to compile
_AS_ECHO_N([looking for all compilable directories... ])
DIRECTORIES=`find $EXOANALYSISROOT -type d -exec sh -c 'if test -d {}/src; then printf "{} "; fi' \;`
DIRECTORIES=`echo $DIRECTORIES | $SED "s|$EXOANALYSISROOT/||g"`
AC_MSG_RESULT(done)
AC_SUBST(DIRECTORIES)

#EXTRACXXFLAGS="${EXTRACXXFLAGS} -O2 -ggdb"
#AC_SUBST(EXTRACXXFLAGS)

dnl AC_SUBST(CPPFLAGS)
dnl LDFLAGS=`$ROOTCONF --ldflags --glibs` 
dnl AC_SUBST(LDFLAGS)

#########################################################################
# Make output
#########################################################################

# deliver all DEFINES via command line as well
AC_OUTPUT_MAKE_DEFS
AX_DEFS=$DEFS
AC_SUBST(AX_DEFS)
# rootcint has issues with some of the defines, in particular those that define
# the arguments of the select function.  Therefore, we remove those defines and
# add the rest to ROOTCINTDEFS
ROOTCINTDEFS=`echo $AX_DEFS | $SED "s/-DSELECT_TYPE_ARG[[1-5]]*=[[^-D]]*//g"`
AC_SUBST(ROOTCINTDEFS)
#
AC_CONFIG_FILES([
                 Makefile
                 config.mk
                 ../doc/wrap_python.sh
                ])
AC_OUTPUT

m4_define([EXO_OUTPUT_STATUS],[AS_IF([test x"$[]$1" = xyes],[AC_MSG_NOTICE([    $2])])])
AC_MSG_NOTICE
AC_MSG_NOTICE([**********************************************************************************])
AC_MSG_NOTICE([Configuration summary:])
AC_MSG_NOTICE([  Installing to:])
AC_MSG_NOTICE([    $EXOOUT])
AC_MSG_NOTICE([  Compiling with the following options:])
EXO_OUTPUT_STATUS(HAVE_PYTHON,python)
EXO_OUTPUT_STATUS(HAVE_ROOT,ROOT)
EXO_OUTPUT_STATUS(HAVE_MYSQL,MySQL)
EXO_OUTPUT_STATUS(HAVE_CLHEP,clhep)
EXO_OUTPUT_STATUS(HAVE_GEANT4,Geant4)
EXO_OUTPUT_STATUS(HAVE_JNI,JNI)
EXO_OUTPUT_STATUS(HAVE_EXOBIN,EXOBinary)
EXO_OUTPUT_STATUS(BUILD_DEBUG,DEBUG)
AC_MSG_NOTICE([  All compileable directories:])
for i in $DIRECTORIES ; do
  j=`echo $i | $SED "s|^$EXOANALYSISROOT/||"`
  AC_MSG_NOTICE([    $j])
done
AC_MSG_NOTICE
AC_MSG_NOTICE([**********************************************************************************])
AC_MSG_NOTICE([Run])
AC_MSG_NOTICE
AC_MSG_NOTICE([  ./configure --help])
AC_MSG_NOTICE
AC_MSG_NOTICE([for a list of options and configuration flags to adjust the configuration.])
AC_MSG_NOTICE([**********************************************************************************])

AC_MSG_NOTICE([You have successfully configured EXOAnalysis.])
AC_MSG_NOTICE([Please now compile by issuing the command (where the @%:@ in -j@%:@ refers to the CPUs ]) 
AC_MSG_NOTICE([across which you'd like to split the build process @<:@optional@:>@):])
AC_MSG_NOTICE
AC_MSG_NOTICE([  make clean ])
AC_MSG_NOTICE([  make @<:@-j@%:@@:>@ ])
AC_MSG_NOTICE


