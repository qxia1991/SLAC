dnl -*- mode: autoconf -*- 
dnl
dnl
dnl Autoconf macro to check for existence of Geant4 on the system
dnl Synopsis:
dnl
dnl  GEANT4_PATH([MINIMUM-VERSION, [ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]]])
dnl
dnl Some examples: 
dnl 
dnl    GEANT4_PATH(1.2, , AC_MSG_ERROR(Your GEANT version is too old))
dnl    GEANT4_PATH(, AC_DEFUN([HAVE_GEANT4]))
dnl 
dnl The macro defines the following substitution variables
dnl
dnl    GEANT4_CONFIG    full path to Geant4-config
dnl    GEANT4_INCLUDE   directory with header files
dnl    GEANT4_CPPFLAGS  compilation flags
dnl    GEANT4_LDFLAGS   library flags (libraries location)
dnl    GEANT4_LIBS      list of basic libraries (no graphics support)
dnl
dnl Written by Michael Marino 
dnl Adapted by Vladimir Belov <belov@itep.ru>
dnl
AC_DEFUN([GEANT4_PATH],
[
  AC_REQUIRE([AC_PROG_SED])dnl
  AC_REQUIRE([AC_PROG_AWK])dnl AS_VERSION_COMPARE
  AC_REQUIRE([AC_PROG_EGREP])dnl
  AC_ARG_WITH(geant4_prefix,
              [AC_HELP_STRING([--with-geant4-prefix],[prefix where GEANT4 is installed (optional)])],
              [geant4_prefix="$withval"],[geant4_prefix=""])

  AS_IF([test x"$geant4_prefix" != x],
    [ AC_PATH_PROG(GEANT4_CONFIG, geant4-config, no, $geant4_prefix/bin)
      AS_IF([test x"$GEANT4_CONFIG" = xno],
        [ $as_unset ac_cv_path_GEANT4_CONFIG dnl Reset cache
          AC_PATH_PROG(GEANT4_CONFIG, Configure, no, $geant4_prefix/src/geant4)])],
    [ AC_PATH_PROG(GEANT4_CONFIG, geant4-config, no, $PATH)
      AS_IF([test x"$GEANT4_CONFIG" = xno],
        [ $as_unset ac_cv_path_GEANT4_CONFIG dnl Reset cache
          AC_PATH_PROG(GEANT4_CONFIG, Configure, no, $G4INSTALL)])])
          dnl There is no point to search for an unspecified 'Configure' in the entire PATH

  min_geant4_version=ifelse([$1], ,8.0.0,$1)
  dnl 'Configure' support ended with GEANT 4.9.5

  AS_IF([test x"$GEANT4_CONFIG" = xno],
    [ no_geant4=yes],
    [ AS_CASE(["$GEANT4_CONFIG"],
        [*/geant4-config],[
          GEANT4_CPPFLAGS=`. $GEANT4_CONFIG --cflags`
          dnl The following filtering doesn't support quoted strings
          GEANT4_INCLUDE=`$GEANT4_CONFIG --cflags | $ac_cv_path_SED 's/^/ /;s/$/ /;s/ -@<:@^I@:>@\(@<:@^ @:>@\|\\\ \)*/ /g'`
          GEANT4_LDFLAGS=`$GEANT4_CONFIG --libs | $ac_cv_path_SED 's/^/ /;s/$/ /;s/ -@<:@l@:>@\(@<:@^ @:>@\|\\\ \)*/ /g'`
          GEANT4_LIBS=`$GEANT4_CONFIG --libs-without-gui | $ac_cv_path_SED 's/^/ /;s/$/ /;s/ -@<:@^l@:>@\(@<:@^ @:>@\|\\\ \)*/ /g'`
          geant4_version=`$GEANT4_CONFIG --version`
          AS_VERSION_COMPARE($geant4_version,$min_geant4_version,[g4_vers_chk='no'])
          ],
        [*/Configure],[
          GEANT4_CPPFLAGS=`$GEANT4_CONFIG -cppflags`
          GEANT4_INCLUDE=`$GEANT4_CONFIG -incflags | $EGREP ^-I`
          GEANT4_LDFLAGS=`$GEANT4_CONFIG -ldflags | $EGREP ^-L`
          GEANT4_LIBS=`$GEANT4_CONFIG -ldlibs | $EGREP ^-l`
          geant4_verfile=`echo $GEANT4_CONFIG | $ac_cv_path_SED 's|Configure|source/global/management/include/G4Version.hh|'`
          geant4_version=`$ac_cv_path_SED 's/^#define G4VERSION_NUMBER  *\(@<:@0-9@:>@*\)\(@<:@0-9@:>@\)\(@<:@0-9@:>@\)$/\1.\2.\3/ p; d;' $geant4_verfile`
          AS_VERSION_COMPARE($geant4_version,$min_geant4_version,[g4_vers_chk='no'])
          ]
        )
      AC_MSG_CHECKING(whether Geant4 version >= $min_geant4_version)
      AS_IF([test x"$g4_vers_chk" != xno],
          [ AC_MSG_RESULT([yes])],
          [ no_geant4=yes
            $as_unset GEANT4_CPPFLAGS GEANT4_INCLUDE GEANT4_LDFLAGS GEANT4_LIBS
            AC_MSG_RESULT([no])])
      ])

  AC_SUBST(GEANT4_CPPFLAGS)
  AC_SUBST(GEANT4_INCLUDE)
  AC_SUBST(GEANT4_LDFLAGS)
  AC_SUBST(GEANT4_LIBS)

  if test "x$no_geant4" = "x" ; then 
    ifelse([$2], , :, [$2])
  else 
    ifelse([$3], , :, [$3])
  fi
])


dnl
dnl Macro to check if GEANT4 has a specific feature:
dnl
dnl   GEANT4_FEATURE(FEATURE,[ACTION_IF_HAVE,[ACTION_IF_NOT]])
dnl
dnl
AC_DEFUN([GEANT4_FEATURE],
[
  AC_REQUIRE([GEANT4_PATH])
  feat=$1
  res=`$GEANT4_CONFIG --has-feature $feat` 
  if test "x$res" = "xyes" ; then 
    ifelse([$2], , :, [$2])     
  else 
    ifelse([$3], , :, [$3])     
  fi
])

#
# EOF
#
