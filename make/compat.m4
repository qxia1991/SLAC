# This file is part of Autoconf.                       -*- Autoconf -*-
# Checking for programs.

# Copyright (C) 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
# 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

# As a special exception, the Free Software Foundation gives unlimited
# permission to copy, distribute and modify the configure scripts that
# are the output of Autoconf.  You need not follow the terms of the GNU
# General Public License when using or distributing such scripts, even
# though portions of the text of Autoconf appear in them.  The GNU
# General Public License (GPL) does govern all other use of the material
# that constitutes the Autoconf program.
#
# Certain portions of the Autoconf source text are designed to be copied
# (in certain cases, depending on the input) into the output of
# Autoconf.  We call these the "data" portions.  The rest of the Autoconf
# source text consists of comments plus executable code that decides which
# of the data portions to output in any given case.  We call these
# comments and executable code the "non-data" portions.  Autoconf never
# copies any of the non-data portions into its output.
#
# This special exception to the GPL applies to versions of Autoconf
# released by the Free Software Foundation.  When you make and
# distribute a modified version of Autoconf, you may extend this special
# exception to the GPL to apply to your modified version as well, *unless*
# your modified version has the potential to copy into its output some
# of the text that was the non-data portion of the version that you started
# with.  (In other words, unless your change moves or copies text from
# the non-data portions to the data portions.)  If your modification has
# such potential, you must delete any notice of this special exception
# to the GPL from your modified version.
#
# Written by David MacKenzie, with help from
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.


## -------------------------- ##
## Generic checks for tools.  ##
## -------------------------- ##

## ---------------- ##
## Specific tests.  ##
## ---------------- ##

# _AC_PATH_PROGS_FEATURE_CHECK(VARIABLE, PROGNAME-LIST, FEATURE-TEST,
#                              [ACTION-IF-NOT-FOUND], [PATH=$PATH])
# -------------------------------------------------------------------
# FEATURE-TEST is called repeatedly with $ac_path_VARIABLE set to the
# name of a program in PROGNAME-LIST found in PATH.  FEATURE-TEST must set
# $ac_cv_path_VARIABLE to the path of an acceptable program, or else
# ACTION-IF-NOT-FOUND is executed; the default action (for internal use
# only) issues a fatal error message.  If a suitable $ac_path_VARIABLE is
# found in the FEATURE-TEST macro, it can set $ac_path_VARIABLE_found=':'
# to accept that value without any further checks.
m4_define([_AC_PATH_PROGS_FEATURE_CHECK],
[if test -z "$$1"; then
  ac_path_$1_found=false
  # Loop through the user's path and test for each of PROGNAME-LIST
  _AS_PATH_WALK([$5],
  [for ac_prog in $2; do
    for ac_exec_ext in '' $ac_executable_extensions; do
      ac_path_$1="$as_dir/$ac_prog$ac_exec_ext"
      AS_EXECUTABLE_P(["$ac_path_$1"]) || continue
$3
      $ac_path_$1_found && break 3
    done
  done])dnl
  if test -z "$ac_cv_path_$1"; then
    m4_default([$4],
      [AC_MSG_ERROR([no acceptable m4_bpatsubst([$2], [ .*]) could be dnl
found in m4_default([$5], [\$PATH])])])
  fi
else
  ac_cv_path_$1=$$1
fi
])


# AC_PATH_PROGS_FEATURE_CHECK(VARIABLE, PROGNAME-LIST,
#                             FEATURE-TEST, [ACTION-IF-NOT-FOUND=:],
#                             [PATH=$PATH])
# ----------------------------------------------------------------
# Designed to be used inside AC_CACHE_VAL.  It is recommended,
# but not required, that the user also use AC_ARG_VAR([VARIABLE]).
# If VARIABLE is not empty, set the cache variable
# $ac_cv_path_VARIABLE to VARIABLE without any further tests.
# Otherwise, call FEATURE_TEST repeatedly with $ac_path_VARIABLE
# set to the name of a program in PROGNAME-LIST found in PATH.  If
# no invocation of FEATURE-TEST sets $ac_cv_path_VARIABLE to the
# path of an acceptable program, ACTION-IF-NOT-FOUND is executed.
# FEATURE-TEST is invoked even when $ac_cv_path_VARIABLE is set,
# in case a better candidate occurs later in PATH; to accept the
# current setting and bypass further checks, FEATURE-TEST can set
# $ac_path_VARIABLE_found=':'.  Note that, unlike AC_CHECK_PROGS,
# this macro does not have any side effect on the current value
# of VARIABLE.
m4_define([AC_PATH_PROGS_FEATURE_CHECK],
[_$0([$1], [$2], [$3], m4_default([$4], [:]), [$5])dnl
])


# _AC_FEATURE_CHECK_LENGTH(PROGPATH, CACHE-VAR, CHECK-CMD, [MATCH-STRING])
# ------------------------------------------------------------------------
# For use as the FEATURE-TEST argument to _AC_PATH_PROGS_FEATURE_TEST.
# On each iteration run CHECK-CMD on an input file, storing the value
# of PROGPATH in CACHE-VAR if the CHECK-CMD succeeds.  The input file
# is always one line, starting with only 10 characters, and doubling
# in length at each iteration until approx 10000 characters or the
# feature check succeeds.  The feature check is called at each
# iteration by appending (optionally, MATCH-STRING and) a newline
# to the file, and using the result as input to CHECK-CMD.
m4_define([_AC_FEATURE_CHECK_LENGTH],
[# Check for GNU $1 and select it if it is found.
  _AC_PATH_PROG_FLAVOR_GNU([$$1],
    [$2="$$1" $1_found=:],
  [ac_count=0
  _AS_ECHO_N([0123456789]) >"conftest.in"
  while :
  do
    cat "conftest.in" "conftest.in" >"conftest.tmp"
    mv "conftest.tmp" "conftest.in"
    cp "conftest.in" "conftest.nl"
    _AS_ECHO(['$4']) >> "conftest.nl"
    $3 < "conftest.nl" >"conftest.out" 2>/dev/null || break
    diff "conftest.out" "conftest.nl" >/dev/null 2>&1 || break
    ac_count=`expr $ac_count + 1`
    if test $ac_count -gt ${$1_max-0}; then
      # Best one so far, save it but keep looking for a better one
      $2="$$1"
dnl   # Using $1_max so that each tool feature checked gets its
dnl   # own variable.  Don't reset it otherwise the implied search
dnl   # for best performing tool in a list breaks down.
      $1_max=$ac_count
    fi
    # 10*(2^10) chars as input seems more than enough
    test $ac_count -gt 10 && break
  done
  rm -f conftest.in conftest.tmp conftest.nl conftest.out])dnl
])


# _AC_PATH_PROG_FLAVOR_GNU(PROGRAM-PATH, IF-SUCCESS, [IF-FAILURE])
# ----------------------------------------------------------------
m4_define([_AC_PATH_PROG_FLAVOR_GNU],
[# Check for GNU $1
case `"$1" --version 2>&1` in
*GNU*)
  $2;;
m4_ifval([$3],
[*)
  $3;;
])esac
])# _AC_PATH_PROG_FLAVOR_GNU


# AC_PROG_SED
# -----------
# Check for a fully functional sed program that truncates
# as few characters as possible.  Prefer GNU sed if found.
AC_DEFUN([AC_PROG_SED],
[AC_CACHE_CHECK([for a sed that does not truncate output], ac_cv_path_SED,
    [dnl ac_script should not contain more than 99 commands (for HP-UX sed),
     dnl but more than about 7000 bytes, to catch a limit in Solaris 8 /usr/ucb/sed.
     ac_script=s/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb/
     for ac_i in 1 2 3 4 5 6 7; do
       ac_script="$ac_script$as_nl$ac_script"
     done
     echo "$ac_script" 2>/dev/null | sed 99q >conftest.sed
     $as_unset ac_script || ac_script=
     _AC_PATH_PROGS_FEATURE_CHECK(SED, [sed gsed],
	[_AC_FEATURE_CHECK_LENGTH([ac_path_SED], [ac_cv_path_SED],
		["$ac_path_SED" -f conftest.sed])])])
 SED="$ac_cv_path_SED"
 AC_SUBST([SED])dnl
 rm -f conftest.sed
])# AC_PROG_SED


