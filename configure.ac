#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([jdemon],
         m4_esyscmd([build-aux/git-version-gen .tarball-version]),                                   
         [pub@mblar.us])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 foreign no-define silent-rules])
AC_CONFIG_SRCDIR([src/lib/Reservoir.cpp])
AC_CONFIG_HEADERS([config.h])

ML_PERFORMANCE_TEST

# Prompt to enable openmpi
  AC_ARG_WITH(mpi, [AS_HELP_STRING([--with-mpi],
      [compile with MPI (parallelization) support. If none is found,
      MPI is not used. Default: auto])
  ],,[with_mpi=yes])
  AX_PROG_CXX_MPI([test x"$with_mpi" != xno],[use_mpi=yes],[
    use_mpi=no
    if test x"$with_mpi" = xyes; then
      AC_MSG_FAILURE([MPI compiler requested, but couldn't use MPI.])
    else
      AC_MSG_WARN([No MPI compiler found, won't use MPI.])
    fi
  ])

# Checks for programs.
AC_PROG_LIBTOOL
AC_LIBTOOL_DLOPEN
DEMON_ENABLE_GCOV
LT_INIT

AC_CONFIG_MACRO_DIR([m4])

# Check for Boost
AX_BOOST_BASE
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SERIALIZATION
AX_BOOST_MPI
AX_BOOST_UNIT_TEST_FRAMEWORK

# Check for GSL
AX_PATH_GSL([1.13])
AX_OPENMP

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/lib/Makefile])
AC_CONFIG_FILES([src/mpi/Makefile])
AC_CONFIG_FILES([scripts/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([test/lib/Makefile])
AC_CONFIG_FILES([test/performance/Makefile])
AC_OUTPUT
