#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([bcp-tools], [0.2.19], [s.kondrat@fz-juelich.de])

AM_INIT_AUTOMAKE()
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([lib/defines.h])
AC_CONFIG_HEADERS([config.h])

AC_SUBST(BCPT_MAINTAINER)
BCPT_MAINTAINER="valiska@gmail.com"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(AR, ar, :)
AC_PROG_LIBTOOL


AX_CXX_COMPILE_STDCXX_11(ext,optional)
#		[AC_MSG_ERROR(upgrade your compiler to 0x11 compatible)])


# Check towhee binary
AC_ARG_WITH(bdbox,
            [  --with-bdbox=DIR        Directory where BD_BOX is installed (optional)],
            BDBOX_DIR="$withval")

BDBOX=
AC_PATH_PROG([BDBOX], [bd_flex],, [$BDBOX_DIR/:$BDBOX_DIR/bin:$PATH])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADERS([getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC


# Scripts substitute
# dnl adl_RECURSIVE_EVAL
# see http://ac-archive.sourceforge.net/adl/relpaths.html
AC_DEFUN([adl_RECURSIVE_EVAL],
[_lcl_receval="$1"
$2=`(test "x$prefix" = xNONE && prefix="$ac_default_prefix"
     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
    _lcl_receval_old=''
     while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
       _lcl_receval_old="[$]_lcl_receval"
       eval _lcl_receval="\"[$]_lcl_receval\""
     done
     echo "[$]_lcl_receval")`])

AC_SUBST(BINDIR)
adl_RECURSIVE_EVAL($bindir, BINDIR)

AC_SUBST(BCPT_MOLSYS)
BCPT_MOLSYS=$(echo ${BINDIR}/molsys-create)

AC_SUBST(BCPT_XYZ)
BCPT_XYZ=$(echo ${BINDIR}/analyze-xyz.py)
AC_SUBST(BCPT_AVERAGE)
BCPT_AVERAGE=$(echo ${BINDIR}/average-me.py)

AC_SUBST(BDFLEX_RUN)
BDFLEX_RUN=$(echo ${BINDIR}/bd_flex-run)

# SUBST DONE, INTERRUPT and RUNNING
AC_SUBST(DONE)
DONE=.DONE
AC_SUBST(INTERRUPTED)
INTERRUPTED=.INTERRUPTED
AC_SUBST(RUNNING)
RUNNING=.RUNNING
AC_SUBST(QUEUE)
QUEUE=.QUEUED

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/tests/Makefile
                 tools/Makefile
		 scripts/Makefile
		 scripts/bd_flex-run
		 scripts/bdbox-run
		 scripts/bdbox-xyz
		 contrib/Makefile
		 contrib/bd_flex-echo
		 contrib/bd_flex-msub
		 ])
AC_OUTPUT
