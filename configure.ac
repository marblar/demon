#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([demon],[0.1],[pub@mblar.us])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 foreign no-define])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_OPENMP

AC_SUBST(OPENMP_LIB_LDFLAGS)

AC_PROG_LIBTOOL
AC_LIBTOOL_DLOPEN

AM_PATH_CPPUNIT(1.12.1)
LT_INIT

AC_CONFIG_MACRO_DIR([build-aux/m4])
ACLOCAL_AMFLAGS= -I build-aux/m4

# Check for Boost
BOOST_REQUIRE([1.40])
BOOST_PROGRAM_OPTIONS
# TODO: Optional libraries?
# BOOST_FIND_LIB(timer,,timer/timer.hpp,,AC_DEFINE(
# BOOST_SYSTEM

# Check for GSL
AX_PATH_GSL(1.13,,AC_MSG_ERROR(Could not find gsl))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
