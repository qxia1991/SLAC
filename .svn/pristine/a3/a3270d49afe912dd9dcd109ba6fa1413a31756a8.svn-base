dnl -*- mode: autoconf -*- 
dnl
dnl
dnl Autoconf macro to check for existence of CLHEP on the system
dnl Synopsis:
dnl
dnl  CLHEP_PATH([MINIMUM-VERSION, [ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]]])
dnl
dnl Some examples: 
dnl 
dnl    CLHEP_PATH(1.2, , AC_MSG_ERROR(Your CLHEP version is too old))
dnl    CLHEP_PATH(, AC_DEFUN([HAVE_CLHEP]))
dnl 
dnl The macro defines the following substitution variables
dnl
dnl    CLHEP_CONFIG        full path to clhep-config
dnl    CLHEP_INCLUDE       Where the CLHEP headers are 
dnl    CLHEP_LIBS          CLHEP basic libraries 
dnl    CLHEP_LDFLAGS       CLHEP library flags 
dnl
dnl Written by Michael Marino
dnl Adapted by Vladimir Belov <belov@itep.ru>
dnl
AC_DEFUN([CLHEP_PATH],
[
  AC_REQUIRE([AC_PROG_AWK])dnl
  AC_ARG_WITH([clhep-prefix],
              [AC_HELP_STRING([--with-clhep-prefix],[prefix where CLHEP is installed (optional)])],
    	      [clhep_prefix="$withval"],[clhep_prefix=""])

  # be old-compartible
  if test x"$clhep_prefix" = x ; then
    clhep_prefix="$CLHEP_BASE_DIR"
  fi
  if test "x${CLHEP_CONFIG+set}" != xset ; then
    if test x"$clhep_prefix" != x ; then
      CLHEP_CONFIG="$clhep_prefix/bin/clhep-config"
    fi
  fi
  AC_PATH_PROG(CLHEP_CONFIG, clhep-config , no)
  min_clhep_version=ifelse([$1], ,1.0.0.0,$1)
  if test x"$CLHEP_CONFIG" = xno ; then
    no_clhep=yes
  else
    AC_MSG_CHECKING(whether CLHEP version >= [$min_clhep_version])
    clhep_vers_chk=`$CLHEP_CONFIG --version | $ac_cv_prog_AWK "{split(\\$[]2,a,\".\");split(\"$min_clhep_version\",b,\".\");if(a@<:@1@:>@==b@<:@1@:>@){if(a@<:@2@:>@==b@<:@2@:>@){if(a@<:@3@:>@==b@<:@3@:>@){c=a@<:@4@:>@>=b@<:@4@:>@}else{c=a@<:@3@:>@>b@<:@3@:>@}}else{c=a@<:@2@:>@>b@<:@2@:>@}}else{c=a@<:@1@:>@>b@<:@1@:>@};if(c==1){print \"yes\";}else{ print \"no\";}}"`
    if test x"$clhep_vers_chk" = xyes ; then
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
      no_clhep=yes
    fi
  fi

  if test x"$no_clhep" != xyes ; then
    CLHEP_INCLUDE=`$CLHEP_CONFIG --include`
    CLHEP_LIBS=`$CLHEP_CONFIG --libs`
    CLHEP_LDFLAGS=`$CLHEP_CONFIG --ldflags`
    
    AC_SUBST(CLHEP_INCLUDE)
    AC_SUBST(CLHEP_LIBS)
    AC_SUBST(CLHEP_LDFLAGS)
  fi
    
  if test "x$no_clhep" = x ; then 
    ifelse([$2], , :, [$2])     
  else 
    ifelse([$3], , :, [$3])     
  fi
])


#
# EOF
#
