#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
m4_define([MAJOR], [0])
m4_define([MINOR], [0])
m4_define([MICRO], [1])
AC_INIT([libsvdrp],[MAJOR.MINOR.MICRO],[www.geexbox.org])
AC_CONFIG_SRCDIR([src/lib/svdrp.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Get the canonical system type
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Disable libtool checks for C++ and F77
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gethostbyname socket strdup strstr])

# Define library versioning information (current:revision:age)
# - If the library source code has changed at all since the last update, then
#   increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# - If any interfaces have been added, removed, or changed since the last
#   update, increment current, and set revision to 0.
# - If any interfaces have been added since the last public release, then
#   increment age.
# - If any interfaces have been removed since the last public release, then
#   set age to 0.
version_info="0:0:0"
AC_SUBST(version_info)

# Automake settings
AM_INIT_AUTOMAKE(1.6 no-dist-gzip dist-bzip2 foreign)
AM_MAINTAINER_MODE
AM_PROG_CC_C_O

# Doxygen settings
DX_DOXYGEN_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(ON)
DX_INIT_DOXYGEN($PACKAGE_NAME, doxygen.cfg, docs)

# silent rules require automake 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
fi

# logcolor
enable_logcolor="yes"
AC_MSG_CHECKING(whether logcolor is enabled or not)
AC_ARG_ENABLE(logcolor,
  AS_HELP_STRING([--enable-logcolor],[Enable log coloring]),
  [ enable_logcolor=$enableval ]
)
AC_MSG_RESULT($enable_logcolor)
AM_CONDITIONAL(USE_LOGCOLOR, test "x$enable_logcolor" = "xyes")
if test "x$enable_logcolor" = "xyes"; then
  AC_DEFINE(USE_LOGCOLOR, 1, [Log coloring])
fi

AC_CONFIG_FILES([
doxygen.cfg
libsvdrp.pc
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
eval echo "Installation Path.................. : $exec_prefix"
eval echo "Use log coloring................... : $enable_logcolor"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo

