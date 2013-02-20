#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([demon],[0.1],[pub@mblar.us])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.12 foreign no-define])
AC_CONFIG_SRCDIR([src/clangomp.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_OPENMP
LT_INIT

AC_CONFIG_MACRO_DIR([build-aux/m4])
ACLOCAL_AMFLAGS= -I build-aux/m4

# Check for Boost
BOOST_REQUIRE(1.52.0)
BOOST_PROGRAM_OPTIONS
BOOST_TIMER
BOOST_SYSTEM

# Check for GSL
AX_PATH_GSL(1.15,,AC_MSG_ERROR(Could not find gsl))

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
