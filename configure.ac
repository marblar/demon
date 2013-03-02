#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([demon],[0.1],[pub@mblar.us])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 foreign no-define])
AC_CONFIG_SRCDIR([src/Reservoir.cpp])
AC_CONFIG_HEADERS([config.h])
AM_EXTRA_RECURSIVE_TARGETS([coverage])


# Checks for programs.
AC_PROG_CXX
AC_OPENMP
AC_PROG_LIBTOOL
AC_LIBTOOL_DLOPEN
AM_PATH_CPPUNIT(1.12.1)
DEMON_ENABLE_GCOV
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
AX_PATH_GSL(1.13)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([coverage/Makefile])
AC_OUTPUT
