#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([tscan], [0.9.9], [m.h.vanderklis@uu.nl]) #also change in webservice/setup.py and codemeta.json!
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/tscan.cxx])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX( [g++ c++] )

#force to C++ when running tests
AC_LANG([C++])

# libtool stuff
AC_PROG_LIBTOOL
LT_INIT

# Check for OpenMP support
AC_OPENMP
if test "x$ac_cv_prog_cxx_openmp" != "xunsupported"; then
  if test "x$ac_cv_prog_cxx_openmp" != "x"; then
     CXXFLAGS="$CXXFLAGS $OPENMP_CXXFLAGS"
     AC_DEFINE(HAVE_OPENMP, 1 , Define to 1 if you have OpenMP )
  fi
else
   AC_MSG_NOTICE(We don't have OpenMP. Multithreaded operation is disabled)
fi

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([mkdir])

PKG_PROG_PKG_CONFIG

# ugly hack when PKG_CONFIG_PATH isn't defined.
# couldn't get it to work otherwise

if test "x$prefix" = "xNONE"; then
   prefix="/usr/local"
fi

if test "x$PKG_CONFIG_PATH" = x
then
   export PKG_CONFIG_PATH="$prefix/lib/pkgconfig"
else
   export PKG_CONFIG_PATH="$prefix/lib/pkgconfig:$PKG_CONFIG_PATH"
fi

AC_OSX_PKG( [icu4c] )
AC_MSG_NOTICE([pkg-config: PATH=$PKG_CONFIG_PATH])

PKG_CHECK_MODULES([ICU], [icu-uc >= 50 icu-io] )
CXXFLAGS="$CXXFLAGS $ICU_CFLAGS"
LIBS="$ICU_LIBS $LIBS"

PKG_CHECK_MODULES([ticcutils], [ticcutils >= 0.22] )
CXXFLAGS="$CXXFLAGS $ticcutils_CFLAGS"
LIBS="$ticcutils_LIBS $LIBS"

PKG_CHECK_MODULES([XML2], [libxml-2.0 >= 2.6.16] )
CXXFLAGS="$CXXFLAGS $XML2_CFLAGS"
LIBS="$LIBS $XML2_LIBS"

PKG_CHECK_MODULES([folia], [folia >= 1.10] )
CXXFLAGS="$folia_CFLAGS $CXXFLAGS"
LIBS="$folia_LIBS $LIBS"

PKG_CHECK_MODULES([frog], [frog >= 0.13.6] )
CXXFLAGS="$frog_CFLAGS $CXXFLAGS"
LIBS="$frog_LIBS $LIBS"

PKG_CHECK_MODULES([timbl], [timbl >= 6.4.4] )
CXXFLAGS="$CXXFLAGS $timbl_CFLAGS"
LIBS="$timbl_LIBS $LIBS"

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/tscan/Makefile
                 src/Makefile
		 data/Makefile
		 webservice/Makefile
                 docs/Makefile])
AC_OUTPUT
